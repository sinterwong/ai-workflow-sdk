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
#include "infer_types.hpp"

namespace android_infer::infer::dnn {
InferErrorCode AlgoInference::initialize() {
  return InferErrorCode::INIT_FAILED;
}

const ModelInfo &AlgoInference::getModelInfo() {
  if (modelInfo)
    return *modelInfo;

  modelInfo = std::make_shared<ModelInfo>();
  return *modelInfo;
}

InferErrorCode AlgoInference::terminate() {
  return InferErrorCode::TERMINATE_FAILED;
}

void AlgoInference::prettyPrintModelInfos() {
  if (!modelInfo) {
    getModelInfo();
    if (!modelInfo) {
      return;
    }
  }
  std::cout << "Model Name: " << modelInfo->name << std::endl;
  std::cout << "Inputs:" << std::endl;
  for (const auto &input : modelInfo->inputs) {
    std::cout << "  Name: " << input.name << ", Shape: ";
    for (int64_t dim : input.shape) {
      std::cout << dim << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "Outputs:" << std::endl;
  for (const auto &output : modelInfo->outputs) {
    std::cout << "  Name: " << output.name << ", Shape: ";
    for (int64_t dim : output.shape) {
      std::cout << dim << " ";
    }
    std::cout << std::endl;
  }
}
}; // namespace android_infer::infer::dnn
