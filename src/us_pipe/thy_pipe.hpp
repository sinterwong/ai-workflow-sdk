/**
 * @file thy_pipe.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef _THY_PIPE_HPP__
#define _THY_PIPE_HPP__

#include "core/infer_types.hpp"
#include "core/rtm_det.hpp"
#include "core/softmax_cls.hpp"
#include "core/vision_infer.hpp"
#include "pipe_types.hpp"
#include "thy_types.hpp"
#include "track.hpp"
#include <memory>
#include <mutex>
#include <vector>

namespace us_pipe {
using infer::dnn::VisionInfer;
using infer::dnn::vision::RTMDet;
using infer::dnn::vision::SoftmaxCls;

class ThyroidInsurancePipeline {
public:
  using TrackT = tracker::TrackBase<ThyroidLesion, ThyroidDiagResult>;

  ThyroidInsurancePipeline(const ThyroidInsurancePipelineConfig &config)
      : mConfig(config) {}

  ThyroidInsurancePipeline() = default;

  ~ThyroidInsurancePipeline() = default;

  void reset();

  std::vector<ThyroidInsu> process_single_frame(const Frame &frame);
  std::vector<infer::BBox> detect_single_frame(const Frame &frame);
  std::vector<ThyroidInsu>
  diagnose_single_frame(const Frame &frame,
                        std::vector<infer::BBox> &detections);

  VideoRepr summary();

  const ThyroidInsurancePipelineConfig &getConfig();

private:
  std::mutex mtx_;
  ThyroidInsurancePipelineConfig mConfig;
  std::unique_ptr<VisionInfer<RTMDet>> thyDet;
  std::unique_ptr<VisionInfer<SoftmaxCls>> fprCls;

  tracker::TrackerArgs trackerArgs;
  std::vector<std::shared_ptr<TrackT>> tracks;
  std::vector<ThyroidLesionRepr> lesionReprs;
};
} // namespace us_pipe
#endif
