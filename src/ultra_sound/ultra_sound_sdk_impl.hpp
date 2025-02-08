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
#include "us_pipe/thy_dispatch.hpp"
#include "utils/thread_safe_queue.hpp"
#include <memory>

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
  // dispatch
  std::unique_ptr<us_pipe::ThyroidDispatch> dispatch;

  utils::ThreadSafeQueue<InputPacket> inputQueue;

  utils::ThreadSafeQueue<OutputPacket> outputQueue;

  std::thread processThread;

  std::atomic<bool> isRunning;

  void outputCallback(std::vector<us_pipe::ThyroidInsu> &thyLesions,
                      const int &frameIndex, const cv::Rect2i &roi,
                      bool isSkip);

private:
  void processLoop();
};

} // namespace ultra_sound

#endif