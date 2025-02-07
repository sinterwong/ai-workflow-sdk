#include "status_pipe.hpp"
#include <mutex>

namespace us_pipe {

StatusPipeline::StatusPipeline(const std::string &model_folder, int device_id) {
}

StatusPipeline::~StatusPipeline() {}

cv::Rect StatusPipeline::process_single_image(const cv::Mat &image) noexcept {
  cv::Rect ret;
  return ret;
}

VideoStatus StatusPipeline::get_video_status() noexcept { return videoStatus; }

void StatusPipeline::reset() {
  std::lock_guard lk(mtx);

  // TODO: reset resources
}

} // namespace us_pipe
