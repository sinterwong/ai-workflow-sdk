/**
 * @file ort_dnn_infer.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "dnn_infer.hpp"
#include "crypto.hpp"
#include "logger/logger.hpp"
#include <memory>
#include <onnxruntime_cxx_api.h>

#ifdef _WIN32
#include <codecvt>
#endif

namespace infer::dnn {

inline auto adaPlatformPath(const std::string &path) {
#ifdef _WIN32
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.from_bytes(path);
#else
  return path;
#endif
}

InferErrorCode AlgoInference::initialize() {
  try {
    // create environment
    env = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING,
                                     params->name.c_str());

    // session options
    Ort::SessionOptions sessionOptions;
    int threadNum = std::thread::hardware_concurrency();
    sessionOptions.SetIntraOpNumThreads(threadNum);
    sessionOptions.SetGraphOptimizationLevel(
        GraphOptimizationLevel::ORT_ENABLE_ALL);

    // create session
    std::vector<unsigned char> engineData;
    if (params->needDecrypt) {
      auto cryptoConfig =
          infer::encrypt::Crypto::deriveKeyFromCommit(params->decryptkeyStr);
      infer::encrypt::Crypto crypto(cryptoConfig);
      crypto.decryptData(params->modelPath, engineData);
    }

    if (engineData.empty()) {
      session = std::make_unique<Ort::Session>(
          *env, adaPlatformPath(params->modelPath).c_str(), sessionOptions);
    } else {
      session = std::make_unique<Ort::Session>(
          *env, engineData.data(), engineData.size(), sessionOptions);
    }

    // create memory info
    memoryInfo = std::make_unique<Ort::MemoryInfo>(
        Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault));

    // get input info
    Ort::AllocatorWithDefaultOptions allocator;
    size_t numInputNodes = session->GetInputCount();
    inputNames.resize(numInputNodes);
    inputShapes.resize(numInputNodes);

    for (size_t i = 0; i < numInputNodes; i++) {
      // get input name
      auto inputName = session->GetInputNameAllocated(i, allocator);
      inputNames[i] = inputName.get();

      // get input shape
      auto typeInfo = session->GetInputTypeInfo(i);
      auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
      inputShapes[i] = tensorInfo.GetShape();
    }

    // get output info
    size_t numOutputNodes = session->GetOutputCount();
    outputNames.resize(numOutputNodes);
    outputShapes.resize(numOutputNodes);

    for (size_t i = 0; i < numOutputNodes; i++) {
      // get output name
      auto outputName = session->GetOutputNameAllocated(i, allocator);
      outputNames[i] = outputName.get();

      // get output shape
      auto typeInfo = session->GetOutputTypeInfo(i);
      auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
      outputShapes[i] = tensorInfo.GetShape();
    }

    return InferErrorCode::SUCCESS;
  } catch (const Ort::Exception &e) {
    LOGGER_ERROR("ONNX Runtime error during initialization: {}", e.what());
    return InferErrorCode::INIT_MODEL_LOAD_FAILED;

  } catch (const std::exception &e) {
    LOGGER_ERROR("Error during initialization: {}", e.what());
    return InferErrorCode::INIT_FAILED;
  }
}

InferErrorCode AlgoInference::infer(AlgoInput &input,
                                    ModelOutput &modelOutput) {
  try {
    modelOutput.outputs.clear();
    modelOutput.outputShapes.clear();

    auto startPre = std::chrono::steady_clock::now();
    PreprocessedData prepData = preprocess(input);

    if (prepData.data.empty()) {
      LOGGER_ERROR("Empty input data after preprocessing");
      return InferErrorCode::PREPROCESS_FAILED;
    }

    std::vector<const char *> inputNamesPtr;
    std::vector<const char *> outputNamesPtr;

    inputNamesPtr.reserve(inputNames.size());
    outputNamesPtr.reserve(outputNames.size());

    for (const auto &name : inputNames) {
      inputNamesPtr.push_back(name.c_str());
    }
    for (const auto &name : outputNames) {
      outputNamesPtr.push_back(name.c_str());
    }

    if (prepData.data.size() != inputShapes.size()) {
      LOGGER_ERROR(
          "Input data count ({}) doesn't match input shapes count ({})",
          prepData.data.size(), inputShapes.size());
      return InferErrorCode::INFER_FAILED;
    }

    std::vector<Ort::Value> inputs;
    inputs.reserve(prepData.data.size());

    std::vector<size_t> elemCounts = prepData.getElementCounts();

    switch (prepData.dataType) {
    case DataType::FLOAT32: {
      for (size_t i = 0; i < prepData.data.size(); ++i) {
        inputs.emplace_back(Ort::Value::CreateTensor(
            *memoryInfo, prepData.data[i].data(), prepData.data[i].size(),
            inputShapes[i].data(), inputShapes[i].size(),
            ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT));
      }
      break;
    }

    case DataType::FLOAT16: {
      auto typedPtrs = prepData.getTypedPtrs<uint16_t>();
      for (size_t i = 0; i < prepData.data.size(); ++i) {
#if ORT_API_VERSION >= 12
        inputs.emplace_back(Ort::Value::CreateTensor(
            *memoryInfo, prepData.data[i].data(), prepData.data[i].size(),
            inputShapes[i].data(), inputShapes[i].size(),
            ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16));
#else
        inputs.emplace_back(Ort::Value::CreateTensor<uint16_t>(
            *memoryInfo, const_cast<uint16_t *>(typedPtrs[i]), elemCounts[i],
            inputShapes[i].data(), inputShapes[i].size()));
#endif
      }
      break;
    }

    default:
      LOGGER_ERROR("Unsupported data type: {}",
                   static_cast<int>(prepData.dataType));
      return InferErrorCode::INFER_FAILED;
    }

    auto outputs = session->Run(Ort::RunOptions{nullptr}, inputNamesPtr.data(),
                                inputs.data(), inputs.size(),
                                outputNamesPtr.data(), outputNames.size());

    for (size_t i = 0; i < outputs.size(); ++i) {
      auto &output = outputs[i];
      auto typeInfo = output.GetTensorTypeAndShapeInfo();
      auto elemCount = typeInfo.GetElementCount();

      std::vector<float> outputData;
      outputData.reserve(elemCount);

      auto elemType = typeInfo.GetElementType();

      if (elemType == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) {
        const float *floatData = output.GetTensorData<float>();
        outputData.assign(floatData, floatData + elemCount);
      } else if (elemType == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16) {
        // FP16 -> FP32
        const uint16_t *fp16Data = output.GetTensorData<uint16_t>();
        cv::Mat halfMat(1, elemCount, CV_16F, (void *)fp16Data);
        cv::Mat floatMat(1, elemCount, CV_32F);
        halfMat.convertTo(floatMat, CV_32F);

        outputData.assign((float *)floatMat.data,
                          (float *)floatMat.data + elemCount);
      } else {
        LOGGER_ERROR("Unsupported output tensor data type: {}",
                     static_cast<int>(elemType));
        return InferErrorCode::INFER_FAILED;
      }

      modelOutput.outputs.emplace_back(std::move(outputData));
      std::vector<int> outputShape;
      outputShape.reserve(output.GetTensorTypeAndShapeInfo().GetShape().size());
      for (int64_t dim : output.GetTensorTypeAndShapeInfo().GetShape()) {
        outputShape.push_back(static_cast<int>(dim));
      }
      modelOutput.outputShapes.push_back(outputShape);
    }
    return InferErrorCode::SUCCESS;
  } catch (const Ort::Exception &e) {
    LOGGER_ERROR("ONNX Runtime error during inference: {}", e.what());
    return InferErrorCode::INFER_FAILED;
  } catch (const std::exception &e) {
    LOGGER_ERROR("Error during inference: {}", e.what());
    return InferErrorCode::INFER_FAILED;
  }
}

InferErrorCode AlgoInference::terminate() {
  try {
    session.reset();
    env.reset();
    memoryInfo.reset();

    inputNames.clear();
    inputShapes.clear();
    outputNames.clear();
    outputShapes.clear();

    return InferErrorCode::SUCCESS;
  } catch (const std::exception &e) {
    LOGGER_ERROR("Error during termination: {}", e.what());
    return InferErrorCode::TERMINATE_FAILED;
  }
}

const ModelInfo &AlgoInference::getModelInfo() {
  if (modelInfo)
    return *modelInfo;

  modelInfo = std::make_shared<ModelInfo>();

  modelInfo->name = params->name;
  if (!session) {
    LOGGER_ERROR("Session is not initialized");
    return *modelInfo;
  }
  try {
    Ort::AllocatorWithDefaultOptions allocator;
    size_t numInputNodes = session->GetInputCount();
    modelInfo->inputs.resize(numInputNodes);
    for (size_t i = 0; i < numInputNodes; i++) {
      auto inputName = session->GetInputNameAllocated(i, allocator);
      modelInfo->inputs[i].name = inputName.get();

      auto typeInfo = session->GetInputTypeInfo(i);
      auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();

      modelInfo->inputs[i].shape = tensorInfo.GetShape();

      size_t numOutputNodes = session->GetOutputCount();
      modelInfo->outputs.resize(numOutputNodes);

      for (size_t i = 0; i < numOutputNodes; i++) {
        auto outputName = session->GetOutputNameAllocated(i, allocator);
        modelInfo->outputs[i].name = outputName.get();
        auto typeInfo = session->GetOutputTypeInfo(i);
        auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
        modelInfo->outputs[i].shape = tensorInfo.GetShape();
      }
    }
  } catch (const Ort::Exception &e) {
    LOGGER_ERROR("ONNX Runtime error during getting model info: {}", e.what());
  } catch (const std::exception &e) {
    LOGGER_ERROR("Error during getting model info: {}", e.what());
  }
  return *modelInfo;
}
}; // namespace infer::dnn
