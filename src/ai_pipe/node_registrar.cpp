
#include "node_registrar.hpp"

#include "node_base.hpp"
#include "node_param_types.hpp"
#include "utils/type_safe_factory.hpp"

#include "demo_nodes.hpp"

namespace ai_pipe {

NodeRegistrar::NodeRegistrar() {
  NodeFactory::instance().registerCreator(
      "DemoSourceNode",
      [](const NodeConstructParams &params) -> std::shared_ptr<NodeBase> {
        auto node_name = params.getParam<std::string>("name");
        auto node_specific_params =
            params.getParam<DemoSourceNodeParams>("node_specific_params");
        return std::make_shared<DemoSourceNode>(node_name,
                                                node_specific_params);
      });

  NodeFactory::instance().registerCreator(
      "DemoProcessingNode",
      [](const NodeConstructParams &params) -> std::shared_ptr<NodeBase> {
        auto node_name = params.getParam<std::string>("name");
        auto node_specific_params =
            params.getParam<DemoProcessingNodeParams>("node_specific_params");
        return std::make_shared<DemoProcessingNode>(node_name,
                                                    node_specific_params);
      });

  NodeFactory::instance().registerCreator(
      "DemoSinkNode",
      [](const NodeConstructParams &params) -> std::shared_ptr<NodeBase> {
        auto node_name = params.getParam<std::string>("name");
        auto node_specific_params =
            params.getParam<DemoSinkNodeParams>("node_specific_params");
        return std::make_shared<DemoSinkNode>(node_name, node_specific_params);
      });
}

} // namespace ai_pipe
