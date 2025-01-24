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
ThyroidInsurancePipeline::ThyroidInsurancePipeline() = default;
ThyroidInsurancePipeline::~ThyroidInsurancePipeline() = default;

void ThyroidInsurancePipeline::reset() {
  mtx_.lock();
  tracks.clear();
  lesionReprs.clear();
  mtx_.unlock();
}

std::vector<ThyroidInsu>
ThyroidInsurancePipeline::process_single_frame(const Frame &frame) {
  return {};
}

std::vector<infer::BBox>
ThyroidInsurancePipeline::detect_single_frame(const Frame &frame) {
  return {};
}

std::vector<ThyroidInsu> ThyroidInsurancePipeline::diagnose_single_frame(
    const Frame &frame, std::vector<infer::BBox> &detections) {
  return {};
}

VideoRepr ThyroidInsurancePipeline::summary() {
  VideoRepr ret;
  return ret;
}

void ThyroidInsurancePipeline::set_config(
    const ThyroidInsurancePipelineConfig &config) {}

void ThyroidInsurancePipeline::get_config(
    ThyroidInsurancePipelineConfig &config) {}

} // namespace us_pipe
