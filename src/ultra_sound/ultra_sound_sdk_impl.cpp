/**
 * @file ultra_sound_sdk_impl.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "ultra_sound_sdk_impl.hpp"
#include "api/us_types.h"
#include "logger/logger.hpp"
#include "us_pipe/pipe_types.hpp"
#include "us_pipe/thy_types.hpp"
#include <chrono>
#include <memory>
#include <opencv2/core/types.hpp>
#include <thread>

namespace ultra_sound {

UltraSoundSDKImpl::UltraSoundSDKImpl() {}

UltraSoundSDKImpl::~UltraSoundSDKImpl() {}

ErrorCode UltraSoundSDKImpl::initialize(const SDKConfig &config) {
  // init logger system
  Logger::getInstance(config.logPath).init(true, true, true, true);
  Logger::getInstance().setLevel(config.logLevel);

  us_pipe::ThyroidInsurancePipelineConfig pconfig;

  dispatch = std::make_unique<us_pipe::ThyroidDispatch>();

  // FIXME: 初始化不只是有模型路径
  dispatch->init(config.modelPath, true);

  isRunning.store(true);
  processThread = std::thread(&UltraSoundSDKImpl::processLoop, this);
  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::calcCurrentROI(const ImageData &input, Rect &roi) {
  // TODO: calculate current ROI
  us_pipe::Frame frame;
  frame.index = input.frameIndex;
  frame.image = cv::imdecode(cv::Mat(input.frameData), cv::IMREAD_COLOR);
  auto curRoi = dispatch->getCurrentRoi();
  roi = Rect{curRoi.x, curRoi.y, curRoi.width, curRoi.height};
  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::processFrame(const InputPacket &input) {
  inputQueue.push(input);
  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::tryGetNextLesion(OutputPacket &output) {
  auto output_ = outputQueue.wait_pop_for(std::chrono::milliseconds(100));
  if (!output_) {
    return ErrorCode::TRY_GET_NEXT_OVERTIME;
  }
  output = *output_;

  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::terminate() {

  isRunning.store(false);
  processThread.join();

  // clean queue
  inputQueue.clear();
  outputQueue.clear();

  LOGGER_INFO("UltraSoundSDKImpl::terminate success");
  return ErrorCode::SUCCESS;
}

void UltraSoundSDKImpl::processLoop() {
  while (isRunning) {
    auto input = inputQueue.wait_pop_for(std::chrono::milliseconds(100));
    if (!input) {
      std::this_thread::yield();
      continue;
    }

    // TODO: process single frame
    us_pipe::Frame frame;
    frame.index = input->frame.frameIndex;
    frame.roi =
        cv::Rect{input->roi.x, input->roi.y, input->roi.w, input->roi.h};
    frame.image =
        cv::imdecode(cv::Mat(input->frame.frameData), cv::IMREAD_COLOR);

    // TODO: decode image data
    cv::Mat image(100, 100, CV_8UC3);
    frame.image = image;

    // TODO: 传入pipeline执行

    // TODO: 回调结果推入队列
    OutputPacket output;
    outputQueue.push(output);
  }
}

} // namespace ultra_sound
