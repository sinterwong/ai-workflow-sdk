
#include "node_registrar.hpp"

#include "node_base.hpp"
#include "node_param_types.hpp"
#include "utils/type_safe_factory.hpp"

#include "demo_nodes.hpp"
#include "image_reader_node.hpp"
#include "result_saver_node.hpp"
#include "vision_inference_node.hpp"
#include "visualization_node.hpp"

namespace ai_pipe {

#define REGISTER_NODE_TYPE(NodeType, NodeParamType)                            \
  NodeFactory::instance().registerCreator(                                     \
      #NodeType,                                                               \
      [](const NodeConstructParams &params) -> std::shared_ptr<NodeBase> {     \
        auto node_name = params.getParam<std::string>("name");                 \
        auto node_specific_params =                                            \
            params.getParam<NodeParamType>("node_specific_params");            \
        return std::make_shared<NodeType>(node_name, node_specific_params);    \
      });                                                                      \
  LOG_INFOS << "Registered " #NodeType " creator.";

NodeRegistrar::NodeRegistrar() {
  REGISTER_NODE_TYPE(DemoSourceNode, DemoSourceNodeParams);
  REGISTER_NODE_TYPE(DemoProcessingNode, DemoProcessingNodeParams);
  REGISTER_NODE_TYPE(DemoSinkNode, DemoSinkNodeParams);
  REGISTER_NODE_TYPE(ImageReaderNode, ImageReaderNodeParams);
  REGISTER_NODE_TYPE(VisionInferenceNode, VisionInferenceNodeParams);
  REGISTER_NODE_TYPE(ResultSaverNode, ResultSaverNodeParams);
  REGISTER_NODE_TYPE(VisualizationNode, VisualizationNodeParams);
}

} // namespace ai_pipe
