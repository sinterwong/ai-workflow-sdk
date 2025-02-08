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
#include <opencv2/imgcodecs.hpp>
#include <thread>

namespace ultra_sound {

UltraSoundSDKImpl::UltraSoundSDKImpl() {}

UltraSoundSDKImpl::~UltraSoundSDKImpl() {}

void UltraSoundSDKImpl::outputCallback(
    std::vector<us_pipe::ThyroidInsu> &thyLesions, const int &frameIndex,
    const cv::Rect2i &roi, bool isSkip) {
  // TODO:
  // 这里使用vector<ThyroidInsu>是为了键兼容已有的pipeline_runner,所有里面只有1个元素;
  // 如果只想输出ThyroidInsu(这样语义更明确),
  // 可以写一个甲状腺保险自己的pipeline_runner;

  if (thyLesions.empty()) {
    LOGGER_INFO("Null frame received, ignoring.");
    return;
  }

  auto thyLesion = thyLesions[0];
  std::cout << "thyroid_insurance callback: Frame index = " << frameIndex
            << std::endl;

  if (!thyLesion.lesions.empty()) {
    std::cout << "num of det_lesions in current frame..." << std::endl;
  }

  if (!thyLesion.lesion_reprs.empty()) {
    std::cout << "show 3 positive images..." << std::endl;
  }

  if (!thyLesion.negative_img.empty()) {
    std::cout << " show one negative image..." << std::endl;
  }

  OutputPacket output;
  output.lesions.reserve(thyLesion.lesions.size());
  for (const auto &lesion : thyLesion.lesions) {
    LesionOutput outLesion;
    outLesion.box =
        Rect{lesion.box.x, lesion.box.y, lesion.box.width, lesion.box.height};
    outLesion.smoothBox =
        Rect{lesion.smooth_box.x, lesion.smooth_box.y, lesion.smooth_box.width,
             lesion.smooth_box.height};

    outLesion.frameIndex = lesion.track_id;
    outLesion.label = lesion.label;
    outLesion.score = lesion.score;
    outLesion.maxArea = lesion.max_area;
    outLesion.fprModelBirads = lesion.fpr_model_birads;
    outLesion.lastDetScore = lesion.last_det_score;
    outLesion.lastFprScore = lesion.last_fpr_score;
    outLesion.trackId = lesion.track_id;
    output.lesions.emplace_back(outLesion);
  }

  output.positiveImages.reserve(thyLesion.lesion_reprs.size());
  for (auto &repr : thyLesion.lesion_reprs) {
    for (auto &img : repr.positive_imgs) {
      ImageData imgData;
      cv::imencode(".png", img, imgData.frameData);
      imgData.height = img.rows;
      imgData.width = img.cols;
      imgData.frameIndex = frameIndex;
      output.positiveImages.push_back(imgData);
    }
  }

  ImageData negImgData;
  cv::imencode(".png", thyLesion.negative_img, negImgData.frameData);
  negImgData.height = thyLesion.negative_img.rows;
  negImgData.width = thyLesion.negative_img.cols;
  negImgData.frameIndex = frameIndex;
  output.negativeImage = negImgData;

  outputQueue.push(output);
}

ErrorCode UltraSoundSDKImpl::initialize(const SDKConfig &config) {
  // init logger system
  Logger::getInstance(config.logPath).init(true, true, true, true);
  Logger::getInstance().setLevel(config.logLevel);

  dispatch = std::make_unique<us_pipe::ThyroidDispatch>();
  us_pipe::ThyDispatchConfig dispatchConfig;
  // TODO: fill dispatch config
  dispatch->init(dispatchConfig);

  dispatch->registerInsuranceCallback(std::bind(
      &UltraSoundSDKImpl::outputCallback, this, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

  isRunning.store(true);
  processThread = std::thread(&UltraSoundSDKImpl::processLoop, this);
  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::calcCurrentROI(const ImageData &input, Rect &roi) {
  // FIXME: 从dispatch中获取当前的roi，会有不同步的问题（调整接口？）
  auto curRoi = dispatch->getCurrentRoi();
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

    us_pipe::Frame frame;
    frame.index = input->frame.frameIndex;
    frame.roi =
        cv::Rect{input->roi.x, input->roi.y, input->roi.w, input->roi.h};
    frame.image =
        cv::imdecode(cv::Mat(input->frame.frameData), cv::IMREAD_COLOR);

    cv::Mat image(100, 100, CV_8UC3);
    frame.image = image;

    // 传入dispatch等回调
    auto framePtr = std::make_shared<us_pipe::Frame>(frame);
    dispatch->processFrame(framePtr);
  }
}

} // namespace ultra_sound
