/**
 * @file thy_pipe.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "thy_pipe.hpp"

namespace us_pipe {

void ThyroidInsurancePipeline::reset() {
  mtx_.lock();
  tracks.clear();
  lesionReprs.clear();
  mtx_.unlock();
}

std::vector<ThyroidInsu>
ThyroidInsurancePipeline::process_single_frame(const Frame &frame) {
  // TODO:
  return {};
}

std::vector<infer::BBox>
ThyroidInsurancePipeline::detect_single_frame(const Frame &frame) {
  // TODO:
  return {};
}

std::vector<ThyroidInsu> ThyroidInsurancePipeline::diagnose_single_frame(
    const Frame &frame, std::vector<infer::BBox> &detections) {
  // TODO:
  return {};
}

VideoRepr ThyroidInsurancePipeline::summary() {
  // TODO:

  VideoRepr ret;
  return ret;
}

const ThyroidInsurancePipelineConfig &ThyroidInsurancePipeline::getConfig() {
  // TODO:
  return mConfig;
}

} // namespace us_pipe
