/**
 * @file ai_sdk_impl.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __ANDROID_INFER_SDK_IMPL_HPP__
#define __ANDROID_INFER_SDK_IMPL_HPP__

#include "api/ai_types.h"
#include "utils/thread_safe_queue.hpp"
#include <atomic>
#include <thread>

namespace android_infer {

class AndroidSDKImpl {

public:
  AndroidSDKImpl();

  ~AndroidSDKImpl();

  ErrorCode initialize(const SDKConfig &config);

  ErrorCode calcCurrentROI(const ImageData &input, Rect &roi);

  ErrorCode pushInput(const InputPacket &input);

  ErrorCode tryGetNextOutput(OutputPacket &output);

  ErrorCode terminate();

private:
  utils::ThreadSafeQueue<InputPacket> inputQueue;

  utils::ThreadSafeQueue<OutputPacket> outputQueue;

  std::thread processThread;

  std::atomic<bool> isRunning;

private:
  void processLoop();
};

} // namespace android_infer

#endif