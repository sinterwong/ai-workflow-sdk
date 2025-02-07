#include "thy_dispatch.hpp"
#include "logger/logger.hpp"

namespace us_pipe {

ThyroidDispatch::ThyroidDispatch() : _det_roi(0, 0, 0, 0) {}

ThyroidDispatch::~ThyroidDispatch() { release(); }

void ThyroidDispatch::init(const std::string &model_folder,
                           bool adaptivate_stride) {
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _global_modules.init(model_folder);
    LOGGER_INFO("Finished initializing GlobalModules");

    _thyroid_insu_modules.init(model_folder, adaptivate_stride);
    LOGGER_INFO("Finished initializing ThyroidInsuranceModules");
  }
}

void ThyroidDispatch::reset() {
  std::lock_guard<std::mutex> lock(_mutex);
  // TODO: reset modules
  LOGGER_INFO("ThyroidDispatch reset");
}

void ThyroidDispatch::release() {
  std::lock_guard<std::mutex> lock(_mutex);
  _global_modules.release();
  _thyroid_insu_modules.release();
  LOGGER_INFO("ThyroidDispatch resources released");
}

void ThyroidDispatch::registerInsuranceCallback(
    const InsuranceCallback &callback) {
  std::lock_guard<std::mutex> lock(_mutex);
  _insurance_cb = callback;
  LOGGER_INFO("Registered insurance callback");
}

void ThyroidDispatch::registerSummaryCallback(const SummaryCallback &callback) {
  std::lock_guard<std::mutex> lock(_mutex);
  _summary_cb = callback;
  LOGGER_INFO("Registered summary callback");
}

void ThyroidDispatch::processFrame(const std::shared_ptr<Frame> &frame) {
  if (!frame) {
    LOGGER_INFO("Null frame received, ignoring.");
    return;
  }

  // 如果是第一帧或 frame->index <= 0，则初始化 ROI 为整张图区域
  {
    std::lock_guard<std::mutex> lock(_mutex);
    if (frame->index <= 0) {
      _det_roi = cv::Rect(0, 0, frame->image.cols, frame->image.rows);
    }
  }

  auto frameCopy = std::make_shared<Frame>(*frame);
  {
    std::lock_guard<std::mutex> lock(_mutex);
    frameCopy->roi = _det_roi;
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
  if (_insurance_cb) {
    std::vector<ThyroidInsu> results;
    bool skipFrame = false;
    _insurance_cb(results, frame->index, frame->roi, skipFrame);
  }
}

void ThyroidDispatch::calculateROI(const std::shared_ptr<Frame> &frame) {
  // 异步计算 ROI
  _global_modules.thread_pool->submit([this, frame]() {
    auto calculatedROI =
        _global_modules.video_status_ppl->process_single_image(frame->image);
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _det_roi = calculatedROI;
      LOGGER_INFO("Updated ROI: x={}, y={}, w={}, h={}", _det_roi.x, _det_roi.y,
                  _det_roi.width, _det_roi.height);
    }
  });
}

cv::Rect ThyroidDispatch::getCurrentRoi() {
  std::lock_guard<std::mutex> lock(_mutex);
  return _det_roi;
}

void ThyroidDispatch::summary() {
  LOGGER_INFO("Summary function called (not yet implemented).");
}

} // namespace us_pipe