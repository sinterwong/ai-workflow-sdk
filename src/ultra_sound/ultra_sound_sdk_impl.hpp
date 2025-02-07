/**
 * @file ultra_sound_sdk_impl.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __ULTRA_SOUND_SDK_IMPL_HPP__
#define __ULTRA_SOUND_SDK_IMPL_HPP__

#include "api/us_types.h"
#include "us_pipe/thy_pipe.hpp"
#include "utils/thread_safe_queue.hpp"

namespace ultra_sound {

using namespace infer;

class UltraSoundSDKImpl {

public:
  UltraSoundSDKImpl();

  ~UltraSoundSDKImpl();

  ErrorCode initialize(const SDKConfig &config);

  ErrorCode calcCurrentROI(const ImageData &input, Rect &roi);

  ErrorCode processFrame(const InputPacket &input);

  ErrorCode tryGetNextLesion(OutputPacket &output);

  ErrorCode terminate();

private:
  // Pipeline
  std::unique_ptr<us_pipe::ThyroidInsurancePipeline> pipeline;

  utils::ThreadSafeQueue<InputPacket> inputQueue;

  utils::ThreadSafeQueue<OutputPacket> outputQueue;

  std::thread processThread;

  std::atomic<bool> isRunning;

private:
  void processLoop();
};

} // namespace ultra_sound

#endif