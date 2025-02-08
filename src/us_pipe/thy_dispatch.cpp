#include "thy_dispatch.hpp"
#include "logger/logger.hpp"

namespace us_pipe {

ThyroidDispatch::ThyroidDispatch() : curDetRoi(0, 0, 0, 0) {}

ThyroidDispatch::~ThyroidDispatch() { release(); }

void ThyroidDispatch::init(const ThyDispatchConfig &config) {
  std::lock_guard<std::mutex> lock(mtx);
  gblModule.init(config.modelRoot);
  thyPipe = std::make_shared<ThyroidInsurancePipeline>(
      ThyroidInsurancePipelineConfig{});
  pplRunner = std::make_unique<
      DiagnosisPipelineRunner<ThyCBK, ThyroidInsurancePipeline>>(
      thyPipe, config.adaStride);

  LOGGER_INFO("Finished initializing GlobalModules");
}

void ThyroidDispatch::reset() {
  std::lock_guard<std::mutex> lock(mtx);
  gblModule.videoStatusPipe.reset();
  pplRunner.reset();
  thyPipe.reset();
  LOGGER_INFO("ThyroidDispatch reset");
}

void ThyroidDispatch::release() {
  std::lock_guard<std::mutex> lock(mtx);
  gblModule.release();
  thyPipe.reset();
  pplRunner.reset();
  LOGGER_INFO("ThyroidDispatch resources released");
}

void ThyroidDispatch::registerInsuranceCallback(const ThyCBK &callback) {
  std::lock_guard<std::mutex> lock(mtx);
  insuCb = callback;
  LOGGER_INFO("Registered insurance callback");
}

void ThyroidDispatch::registerSummaryCallback(const SummaryCallback &callback) {
  std::lock_guard<std::mutex> lock(mtx);
  summaryCb = callback;
  LOGGER_INFO("Registered summary callback");
}

void ThyroidDispatch::processFrame(const std::shared_ptr<Frame> &frame) {
  if (!frame) {
    LOGGER_INFO("Null frame received, ignoring.");
    return;
  }

  // 第一帧时初始化 ROI 为整张图区域
  {
    std::lock_guard<std::mutex> lock(mtx);
    if (frame->index <= 0) {
      curDetRoi = cv::Rect(0, 0, frame->image.cols, frame->image.rows);
    }
  }

  auto frameCopy = std::make_shared<Frame>(*frame);
  {
    std::lock_guard<std::mutex> lock(mtx);
    frameCopy->roi = curDetRoi;
  }

  dispatchCalc(frameCopy);
}

void ThyroidDispatch::dispatchCalc(const std::shared_ptr<Frame> &frame) {
  // 每 30 帧更新 ROI
  if (frame->index % 30 == 0) {
    LOGGER_INFO("Calculating ROI for frame index {}", frame->index);
    calculateROI(frame);
  }
  processInsuranceDetection(frame);
}

void ThyroidDispatch::processInsuranceDetection(
    const std::shared_ptr<Frame> &frame) {
  LOGGER_INFO("Processing insurance detection for frame index {}",
              frame->index);

  if (insuCb) {
    pplRunner->enqueue_data(frame, insuCb);
  } else {
    LOGGER_ERROR("Insurance callback not registered.");
    throw std::runtime_error("Insurance callback is not registered");
  }
}

void ThyroidDispatch::calculateROI(const std::shared_ptr<Frame> &frame) {
  // FIXME: 异步更新roi，可能会造成当前帧不同步问题
  gblModule.threadPool->submit([this, frame]() {
    auto calculatedROI =
        gblModule.videoStatusPipe->process_single_image(frame->image);
    {
      std::lock_guard<std::mutex> lock(mtx);
      curDetRoi = calculatedROI;
      LOGGER_INFO("Updated ROI: x={}, y={}, w={}, h={}", curDetRoi.x,
                  curDetRoi.y, curDetRoi.width, curDetRoi.height);
    }
  });
}

cv::Rect ThyroidDispatch::getCurrentRoi() {
  std::lock_guard<std::mutex> lock(mtx);
  return curDetRoi;
}

void ThyroidDispatch::summary() {
  LOGGER_INFO("Summary function called (not yet implemented).");
}

} // namespace us_pipe
