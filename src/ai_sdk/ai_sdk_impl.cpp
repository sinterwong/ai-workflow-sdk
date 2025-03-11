/**
 * @file ai_sdk_impl.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "ai_sdk_impl.hpp"
#include "api/ai_types.h"
#include "logger/logger.hpp"
#include <chrono>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <thread>

namespace android_infer {

AndroidSDKImpl::AndroidSDKImpl() {}

AndroidSDKImpl::~AndroidSDKImpl() {
  if (isRunning) {
    terminate();
  }
}

ErrorCode AndroidSDKImpl::initialize(const SDKConfig &config) {
  isRunning.store(true);
  processThread = std::thread(&AndroidSDKImpl::processLoop, this);
  return ErrorCode::SUCCESS;
}

ErrorCode AndroidSDKImpl::calcCurrentROI(const ImageData &input, Rect &roi) {
  return ErrorCode::SUCCESS;
}

ErrorCode AndroidSDKImpl::pushInput(const InputPacket &input) {
  inputQueue.push(input);
  return ErrorCode::SUCCESS;
}

ErrorCode AndroidSDKImpl::tryGetNextOutput(OutputPacket &output) {
  auto output_ = outputQueue.wait_pop_for(std::chrono::milliseconds(1000));
  if (!output_) {
    return ErrorCode::TRY_GET_NEXT_OVERTIME;
  }
  output = *output_;

  return ErrorCode::SUCCESS;
}

ErrorCode AndroidSDKImpl::terminate() {

  isRunning.store(false);
  processThread.join();

  // clean queue
  inputQueue.clear();
  outputQueue.clear();

  LOG_INFOS << "AndroidSDKImpl::terminate success";
  return ErrorCode::SUCCESS;
}

void AndroidSDKImpl::processLoop() {
  while (isRunning) {
    auto input = inputQueue.wait_pop_for(std::chrono::milliseconds(100));
    if (!input) {
      std::this_thread::yield();
      continue;
    }
    OutputPacket output;
    // TODO: 处理完成output后推入输出队列
    outputQueue.push(output);
  }
}

} // namespace android_infer
