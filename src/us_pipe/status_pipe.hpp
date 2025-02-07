#ifndef __US_PIPE_STATUS_PIPELINE_HPP__
#define __US_PIPE_STATUS_PIPELINE_HPP__

#include "pipe_types.hpp"
#include <opencv2/core/mat.hpp>
#include <string>

namespace us_pipe {

class StatusPipeline {
public:
  StatusPipeline(const std::string &model_folder, int device_id = 0);
  ~StatusPipeline();

  cv::Rect process_single_image(const cv::Mat &image) noexcept;

  VideoStatus get_video_status() noexcept;

  void reset();

private:
  std::mutex mtx;

  // LayoutParser layoutParser;
  // ROIStabilizer roiStabilizer;
  // StatusAnalyzer statusAnalyzer;
  // StatusStabilizer statusStabilizer;

  VideoStatus videoStatus;
};
} // namespace us_pipe

#endif