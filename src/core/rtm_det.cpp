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

#include "rtm_det.hpp"
#include "infer_types.hpp"
#include "logger/logger.hpp"

namespace android_infer::infer::dnn {
InferErrorCode RTMDetInference::infer(AlgoInput &input, AlgoOutput &output) {
  // Get input parameters
  auto *frameInput = input.getParams<FrameInput>();
  if (!frameInput) {
    LOGGER_ERROR("Invalid input parameters");
    return InferErrorCode::INFER_INPUT_ERROR;
  }

  auto *rtmDetOutput = output.getParams<DetRet>();
  if (!rtmDetOutput) {
    LOGGER_ERROR("Invalid output parameters");
    return InferErrorCode::INFER_OUTPUT_ERROR;
  }

  try {
    // Perform inference here
    // ...
    // Set output parameters
    return InferErrorCode::SUCCESS;
  } catch (const std::exception &e) {
    LOGGER_ERROR("Inference failed: {}", e.what());
    return InferErrorCode::INFER_FAILED;
  }
}

}; // namespace android_infer::infer::dnn
