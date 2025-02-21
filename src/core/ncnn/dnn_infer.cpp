/**
 * @file dnn_infer.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "dnn_infer.hpp"
#include "crypto.hpp"
#include "infer_types.hpp"
#include "logger/logger.hpp"
#include <ncnn/allocator.h>
#include <ncnn/cpu.h>
#include <opencv2/core/hal/interface.h>
#include <stdlib.h>
#include <vector>
// #include "logger/logger.hpp"

namespace infer::dnn {

InferErrorCode AlgoInference::initialize() {

  net.clear();
  blobPoolAllocator.clear();
  workspacePoolAllocator.clear();

  ncnn::set_cpu_powersave(2);
  ncnn::set_omp_num_threads(ncnn::get_big_cpu_count());
  net.opt = ncnn::Option();

#if NCNN_VULKAN
  net.opt.use_vulkan_compute = params->deviceType == DeviceType::GPU;
#endif
  net.opt.num_threads = ncnn::get_big_cpu_count();
  net.opt.blob_allocator = &blobPoolAllocator;
  net.opt.workspace_allocator = &workspacePoolAllocator;

  if (net.load_param((params->modelPath + ".param").c_str()) != 0) {
    // LOGGER_ERROR("Failed to load model: {}", params->modelPath + ".bin");
    return InferErrorCode::INIT_MODEL_LOAD_FAILED;
  }

  if (params->needDecrypt) {
    std::vector<uchar> modelData;
    auto cryptoConfig =
        encrypt::Crypto::deriveKeyFromCommit(params->decryptkeyStr);
    encrypt::Crypto crypto(cryptoConfig);
    crypto.decryptData(params->modelPath + ".bin", modelData);
    size_t dataSize = modelData.size();
    size_t alignedSize = ncnn::alignSize(dataSize, 4);
    void *alignedData = nullptr;

    int allocRet =
        posix_memalign(&alignedData, alignof(std::max_align_t), alignedSize);

    if (allocRet != 0) {
      return InferErrorCode::INIT_MODEL_LOAD_FAILED;
    }

    if (!alignedData) {
      return InferErrorCode::INIT_MODEL_LOAD_FAILED;
    }

    memcpy(alignedData, modelData.data(), dataSize);
    if (alignedSize > dataSize) {
      memset(static_cast<uchar *>(alignedData) + dataSize, 0,
             alignedSize - dataSize);
    }

    int ret = net.load_model(static_cast<const unsigned char *>(alignedData));
    if (ret <= 0) {
      free(alignedData);
      return InferErrorCode::INIT_MODEL_LOAD_FAILED;
    }
    // 保存对齐指针以便后续释放
    m_aligned_buffers.push_back(alignedData);
    alignedData = nullptr;
  } else {
    if (net.load_model((params->modelPath + ".bin").c_str()) != 0) {
      return InferErrorCode::INIT_MODEL_LOAD_FAILED;
    }
  }

  // get inputNames and outputNames
  for (auto const &inName : net.input_names()) {
    inputNames.push_back(inName);
  }
  for (auto const &outName : net.output_names()) {
    outputNames.push_back(outName);
  }
  return InferErrorCode::SUCCESS;
}

InferErrorCode AlgoInference::infer(AlgoInput &input,
                                    ModelOutput &modelOutput) {
  try {
    // create infer engine
    ncnn::Extractor ex = net.create_extractor();

    auto inputs = preprocess(input);
    for (auto const &[name, in] : inputs) {
      ex.input(name.c_str(), in);
    }

    int numOutputs = outputNames.size();

    for (auto const &output : outputNames) {
      ncnn::Mat out;
      ex.extract(output.c_str(), out);
      float *outData = reinterpret_cast<float *>(out.data);
      std::vector<float> outputData(outData, outData + out.total());
      std::vector<int> outputShape;
      if (out.dims == 1) {
        outputShape.push_back(out.w);
      } else if (out.dims == 2) {
        outputShape.push_back(out.h);
        outputShape.push_back(out.w);
      } else if (out.dims == 3) {
        outputShape.push_back(out.c);
        outputShape.push_back(out.h);
        outputShape.push_back(out.w);
      } else if (out.dims == 4) {
        outputShape.push_back(out.d);
        outputShape.push_back(out.c);
        outputShape.push_back(out.h);
        outputShape.push_back(out.w);
      }
      modelOutput.outputs.emplace_back(outputData);
      modelOutput.outputShapes.emplace_back(outputShape);
    }
    return InferErrorCode::SUCCESS;
  } catch (const std::exception &e) {
    LOGGER_ERROR("Inference failed: {}", e.what());
    return InferErrorCode::INFER_FAILED;
  }
}

const ModelInfo &AlgoInference::getModelInfo() {
  if (modelInfo)
    return *modelInfo;
  if (!params) {
    LOGGER_ERROR("Invalid algorithm parameters");
    return *modelInfo;
  }

  modelInfo = std::make_shared<ModelInfo>();
  modelInfo->name = params->name;
  for (const auto &inputName : inputNames) {
    ncnn::Mat inputTensor;
    modelInfo->inputs.push_back({inputName, {}});
  }
  for (const auto &outputName : outputNames) {
    ncnn::Mat outputTensor;
    modelInfo->outputs.push_back({outputName, {}});
  }
  return *modelInfo;
}

InferErrorCode AlgoInference::terminate() {
  try {
    net.clear();
    modelInfo.reset();
    return InferErrorCode::SUCCESS;
  } catch (const std::exception &e) {
    LOGGER_ERROR("Error during termination: {}", e.what());
    return InferErrorCode::TERMINATE_FAILED;
  }
}
}; // namespace infer::dnn
