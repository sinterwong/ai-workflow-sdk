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
#include "core/frame_infer.hpp"
#include "core/infer_types.hpp"
#include "core/infer_wrapper.hpp"
#include "core/vision.hpp"
#include "utils/thread_pool.hpp"
#include "utils/thread_safe_queue.hpp"

namespace ultra_sound {

using namespace utils;
using namespace infer;

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
  // TODO: 后续里面大部分的内容应该要放在pipeline里面

  // 第一级队列：接收输入数据
  ThreadSafeQueue<InputPacket> inputQueue;

  // 第二级队列：优先队列，尽可能保证输出顺序
  ThreadSafePriorityQueue<OutputPacket, OutputPacketComparator> outputQueue;

  // 工作线程池
  thread_pool workers;

  // 状态控制
  std::atomic<bool> isRunning;

  // 模型池
  std::vector<
      std::unique_ptr<InferSafeWrapper<dnn::FrameInference, FrameInferParam>>>
      modelInstances;

  // 模型后处理
  std::shared_ptr<dnn::vision::Vision> rtmDet;

public:
  UltraSoundSDKImpl();
  ~UltraSoundSDKImpl();
  ErrorCode initialize(const SDKConfig &config);
  ErrorCode pushInput(const InputPacket &input);
  ErrorCode terminate();
  ErrorCode tryGetNext(OutputPacket &result);

private:
  void processLoop();
};

} // namespace ultra_sound

#endif