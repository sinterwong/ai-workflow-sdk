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
#include "core/infer_types.hpp"
#include "core/rtm_det.hpp"
#include "logger/logger.hpp"
#include <chrono>
#include <cmath>
#include <opencv2/core/types.hpp>

namespace ultra_sound {

UltraSoundSDKImpl::UltraSoundSDKImpl() : isRunning(false) {}

UltraSoundSDKImpl::~UltraSoundSDKImpl() {
  if (isRunning) {
    terminate();
  }
}

ErrorCode UltraSoundSDKImpl::initialize(const SDKConfig &config) {
  FrameInferParam param;
  param.name = "us-infer";
  param.modelPath = config.modelPath;
  // param.deviceType = DeviceType::CPU;
  param.inputShape = Shape{static_cast<int>(config.inputWidth),
                           static_cast<int>(config.inputHeight)};

  // the number of model instaces is consistent with the number of the workers
  for (int i = 0; i < config.numWorkers; ++i) {
    FrameInferParam curParam = param;
    curParam.name = param.name + "-" + std::to_string(i);
    auto engine = std::make_unique<
        InferSafeWrapper<dnn::FrameInference, FrameInferParam>>(curParam);
    if (engine->initialize() != infer::InferErrorCode::SUCCESS) {
      LOGGER_ERROR("UltraSoundSDKImpl::initialize failed, modelPath: {}",
                   config.modelPath.c_str());
      return ErrorCode::INITIALIZATION_FAILED;
    }
    modelInstances.emplace_back(std::move(engine));
  }

  // init rtmDet
  AnchorDetParams anchorDetParams;
  anchorDetParams.condThre = 0.5f;
  anchorDetParams.nmsThre = 0.45f;
  anchorDetParams.inputShape = param.inputShape;
  AlgoPostprocParams params;
  params.setParams(anchorDetParams);
  rtmDet = std::make_shared<dnn::vision::RTMDet>(params);
  if (rtmDet == nullptr) {
    LOGGER_ERROR("UltraSoundSDKImpl::initialize failed, rtmDet is nullptr");
    return ErrorCode::INITIALIZATION_FAILED;
  }

  isRunning.store(true);

  // start thread pool
  workers.start(config.numWorkers);
  for (int i = 0; i < config.numWorkers; ++i) {
    workers.submit([this]() { processLoop(); });
  }

  LOGGER_INFO(
      "UltraSoundSDKImpl::initialize success, modelPath: {}, numWorkers: {}",
      config.modelPath.c_str(), config.numWorkers);
  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::pushInput(const InputPacket &input) {
  if (!isRunning.load()) {
    LOGGER_ERROR("UltraSoundSDKImpl::pushInput, sdk is not running");
    return ErrorCode::INVALID_STATE;
  }
  inputQueue.push(input);
  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::tryGetNext(OutputPacket &result) {
  auto ret = outputQueue.wait_pop_for(std::chrono::milliseconds(100));
  if (!ret.has_value()) {
    LOGGER_WARN(
        "UltraSoundSDKImpl::tryGetNext timeout, please wait and try again.");
    return ErrorCode::TRY_GET_NEXT_OVERTIME;
  }
  result = std::move(ret.value());
  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::terminate() {
  if (!isRunning.load()) {
    LOGGER_ERROR("UltraSoundSDKImpl::terminate, sdk is not running");
    return ErrorCode::INVALID_STATE;
  }
  isRunning = false;
  workers.stop();
  inputQueue.clear();
  outputQueue.clear();
  modelInstances.clear();
  rtmDet.reset();
  LOGGER_INFO("UltraSoundSDKImpl::terminate success");
  return ErrorCode::SUCCESS;
}

void UltraSoundSDKImpl::processLoop() {
  while (isRunning) {
    auto input = inputQueue.wait_pop_for(std::chrono::milliseconds(100));
    if (!input.has_value()) {
      continue;
    }

    // get one model instance
    size_t actualModelIndex;
    for (size_t i = 0; i < modelInstances.size(); ++i) {
      if (modelInstances[i]->tryAcquire()) {
        actualModelIndex = i;
        break;
      }
    }

    auto *engine = modelInstances[actualModelIndex]->getEngine();

    // TODO: will be refactored
    FrameInput frameInput;
    cv::Mat image = cv::imdecode(input->imageData, cv::IMREAD_COLOR);
    int frameWidth = image.cols;
    int frameHeight = image.rows;
    frameInput.image = image;
    frameInput.args.originShape = {frameWidth, frameHeight};
    // frameInput.args.roi = {0, 0, 0, 0}; // TODO: fill it in pipeline?
    frameInput.args.isEqualScale = true;
    frameInput.args.pad = {0, 0, 0};
    frameInput.args.meanVals = {120.0f, 120.0f, 120.0f};
    frameInput.args.normVals = {60.0f, 60.0f, 60.0f};

    AlgoInput algoInput;
    algoInput.setParams(frameInput);

    ModelOutput modelOutput;
    auto errorCode = engine->infer(algoInput, modelOutput);
    if (errorCode != infer::InferErrorCode::SUCCESS) {
      LOGGER_ERROR("UltraSoundSDKImpl::processLoop infer failed, errorCode: {}",
                   static_cast<int>(errorCode));
      continue;
    }

    auto frameInputPtr = algoInput.getParams<FrameInput>();
    AlgoOutput algoOutput;
    rtmDet->processOutput(modelOutput, frameInputPtr->args, algoOutput);

    OutputPacket output;
    output.uuid = input->uuid;
    output.frameIndex = input->frameIndex;
    output.timestamp = input->timestamp;
    // TODO: need drawing the result to the image?
    output.frameData = input->imageData;
    output.width = input->width;
    output.height = input->height;
    auto *detRet = algoOutput.getParams<DetRet>();
    if (detRet) {
      for (const auto &bbox : detRet->bboxes) {
        RetBBox retBBox;
        retBBox.rect = {bbox.rect.x, bbox.rect.y, bbox.rect.width,
                        bbox.rect.height};
        retBBox.score = bbox.score;
        retBBox.label = bbox.label;
        output.bboxes.emplace_back(retBBox);
      }
    }

    modelInstances[actualModelIndex]->release();
    outputQueue.push(std::move(output));
  }
}

} // namespace ultra_sound
