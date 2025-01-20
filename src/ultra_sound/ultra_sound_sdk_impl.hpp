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
#include "utils/thread_pool.hpp"
#include "utils/thread_safe_queue.hpp"

namespace ultra_sound {

using namespace utils;

struct OutputPacketComparator {
  bool operator()(const OutputPacket &a, const OutputPacket &b) {
    if (a.frameIndex == b.frameIndex) {
      return a.timestamp > b.timestamp;
    }
    return a.frameIndex > b.frameIndex;
  }
};

class UltraSoundSDKImpl {
private:
  // 第一级队列：接收输入数据
  ThreadSafeQueue<InputPacket> inputQueue;

  // 第二级队列：优先队列，尽可能保证输出顺序
  ThreadSafePriorityQueue<OutputPacket, OutputPacketComparator> outputQueue;

  // 工作线程池
  thread_pool workers;

  // 状态控制
  std::atomic<bool> isRunning;

  // 处理线程
  std::thread processThread;
  std::mutex mtx;

public:
  UltraSoundSDKImpl();
  ErrorCode initialize(const SDKConfig &config);
  ErrorCode pushInput(const InputPacket &input);
  ErrorCode terminate();
  ErrorCode tryGetNext(OutputPacket &result);

private:
  void processLoop();
};

} // namespace ultra_sound

#endif