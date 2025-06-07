/**
 * @file node_param_types.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-06-07
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __AI_NODE_PARAM_TYPES_HPP__
#define __AI_NODE_PARAM_TYPES_HPP__

#include "logger/logger.hpp"
#include "utils/data_packet.hpp"
#include "utils/param_center.hpp"
#include <nlohmann/json.hpp>
#include <variant>

// Wouldn't it be more suitable to put them in the node_registrar.hpp?
namespace ai_pipe {
struct DemoSourceNodeParams {
  int source_id;
};

struct DemoProcessingNodeParams {
  float processing_threshold;
};

struct DemoSinkNodeParams {
  std::string output_path;
};

using NodeParams = utils::ParamCenter<
    std::variant<std::monostate, DemoSourceNodeParams, DemoProcessingNodeParams,
                 DemoSinkNodeParams>>;

// for get_to
inline void from_json(const nlohmann::json &j, DemoSourceNodeParams &p) {
  if (j.contains("source_id")) {
    j.at("source_id").get_to(p.source_id);
  } else {
    throw std::runtime_error(
        "Missing 'source_id' in DemoSourceNodeParams JSON");
  }
  // Add deserialization for other members if any
}

inline void from_json(const nlohmann::json &j, DemoProcessingNodeParams &p) {
  if (j.contains("processing_threshold")) {
    j.at("processing_threshold").get_to(p.processing_threshold);
  } else {
    throw std::runtime_error(
        "Missing 'processing_threshold' in DemoProcessingNodeParams JSON");
  }
}

inline void from_json(const nlohmann::json &j, DemoSinkNodeParams &p) {
  if (j.contains("output_path")) {
    j.at("output_path").get_to(p.output_path);
  } else {
    throw std::runtime_error(
        "Missing 'output_path' in DemoSinkNodeParams JSON");
  }
}

template <typename ParamsType>
void handleNodeParams(const nlohmann::json &nodeConfig,
                      utils::DataPacket &creationParams,
                      const std::string &name, const std::string &type) {
  ParamsType specificParams;
  if (nodeConfig.contains("params")) {
    nodeConfig.at("params").get_to(specificParams);
  } else {
    LOG_ERRORS << "Node " << name << " of type " << type
               << " has no 'params' block. Using default parameters.";
    throw std::runtime_error("Missing 'params' block for node " + name +
                             " of type " + type);
  }
  creationParams.setParam("node_specific_params", specificParams);
}

using NodeParamHandler = void (*)(const nlohmann::json &, utils::DataPacket &,
                                  const std::string &, const std::string &);

// convert logic to data to simplify code
static const std::unordered_map<std::string, NodeParamHandler> s_paramHandlers =
    {{"DemoSourceNode", &handleNodeParams<DemoSourceNodeParams>},
     {"DemoProcessingNode", &handleNodeParams<DemoProcessingNodeParams>},
     {"DemoSinkNode", &handleNodeParams<DemoSinkNodeParams>}};

} // namespace ai_pipe

#endif
