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
#include <opencv2/core/types.hpp>

namespace ultra_sound {

UltraSoundSDKImpl::UltraSoundSDKImpl() {}

UltraSoundSDKImpl::~UltraSoundSDKImpl() {}

ErrorCode UltraSoundSDKImpl::initialize(const SDKConfig &config) {
  // init logger system
  Logger::getInstance(config.logPath).init(true, true, true, true);
  Logger::getInstance().setLevel(config.logLevel);

  // TODO: load algo config from yaml

  us_pipe::ThyroidInsurancePipelineConfig pconfig;

  pipeline = std::make_unique<us_pipe::ThyroidInsurancePipeline>(pconfig);

  // // TODO: 交由pipeline内部完成主逻辑
  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::calcCurrentROI(const ImageData &input, Rect &roi) {
  // TODO: calculate current ROI
  roi = {100, 100, 200, 200};

  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::processFrame(const InputPacket &input) {
  // TODO: process single frame
  us_pipe::Frame frame;
  frame.index = input.frame.frameIndex;
  frame.roi = cv::Rect{input.roi.x, input.roi.y, input.roi.w, input.roi.h};
  frame.image = cv::imdecode(cv::Mat(input.frame.frameData), cv::IMREAD_COLOR);

  // TODO: decode image data
  cv::Mat image(100, 100, CV_8UC3);
  frame.image = image;

  // TODO: 传入pipeline执行

  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::tryGetNextLesion(OutputPacket &output) {
  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::terminate() {

  LOGGER_INFO("UltraSoundSDKImpl::terminate success");
  return ErrorCode::SUCCESS;
}

} // namespace ultra_sound
