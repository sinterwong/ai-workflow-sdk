/**
 * @file node_param_types.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-06-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "node_param_types.hpp"

namespace ai_pipe {
void from_json(const nlohmann::json &j, DemoSourceNodeParams &p) {
  if (j.contains("source_id")) {
    j.at("source_id").get_to(p.source_id);
  } else {
    throw std::runtime_error(
        "Missing 'source_id' in DemoSourceNodeParams JSON");
  }
}

void from_json(const nlohmann::json &j, DemoProcessingNodeParams &p) {
  if (j.contains("processing_threshold")) {
    j.at("processing_threshold").get_to(p.processing_threshold);
  } else {
    throw std::runtime_error(
        "Missing 'processing_threshold' in DemoProcessingNodeParams JSON");
  }
}

void from_json(const nlohmann::json &j, DemoSinkNodeParams &p) {
  if (j.contains("output_path")) {
    j.at("output_path").get_to(p.output_path);
  } else {
    throw std::runtime_error(
        "Missing 'output_path' in DemoSinkNodeParams JSON");
  }
}

void from_json(const nlohmann::json &j, ImageReaderNodeParams &p) {
  if (j.contains("color_type")) {
    std::string color_type_str = j.at("color_type").get<std::string>();
    if (color_type_str == "RGB888") {
      p.colorType = ColorType::RGB888;
    } else if (color_type_str == "BGR888") {
      p.colorType = ColorType::BGR888;
    } else if (color_type_str == "GRAY") {
      p.colorType = ColorType::GRAY;
    } else if (color_type_str == "YUV") {
      p.colorType = ColorType::YUV;
    } else if (color_type_str == "YUV_I420") {
      p.colorType = ColorType::YUV_I420;
    } else if (color_type_str == "YUV_YV12") {
      p.colorType = ColorType::YUV_YV12;
    } else {
      throw std::runtime_error("Unknown color_type: " + color_type_str);
    }
  } else {
    LOG_WARNINGS << "Missing 'color_type' in ImageReaderNodeParams JSON. "
                    "Defaulting to BGR888.";
    p.colorType = ColorType::BGR888;
  }
}

void from_json(const nlohmann::json &j, VisionInferenceNodeParams &p) {
  if (j.contains("model_name")) {
    j.at("model_name").get_to(p.modelName);
  } else {
    throw std::runtime_error(
        "Missing 'model_name' in VisionInferenceNodeParams JSON");
  }
}

void from_json(const nlohmann::json &j, ResultSaverNodeParams &p) {
  if (j.contains("output_dir")) {
    j.at("output_dir").get_to(p.outputDir);
  } else {
    throw std::runtime_error(
        "Missing 'output_dir' in ResultSaverNodeParams JSON");
  }
}

void from_json(const nlohmann::json &j, VisualizationNodeParams &p) {
  if (j.contains("output_dir")) {
    j.at("output_dir").get_to(p.outputDir);
  } else {
    throw std::runtime_error(
        "Missing 'output_dir' in VisualizationNodeParams JSON");
  }
}
} // namespace ai_pipe
