
#include "node_registrar.hpp"

#include "node_base.hpp"
#include "node_param_types.hpp"
#include "utils/type_safe_factory.hpp"

#include "demo_nodes.hpp"
#include "image_reader_node.hpp"
#include "inference_node.hpp"
#include "result_saver_node.hpp"
#include "visualization_node.hpp"

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

  // Register new demo nodes
  NodeFactory::instance().registerCreator(
      "ImageReaderNode",
      [](const NodeConstructParams &params) -> std::shared_ptr<NodeBase> {
        auto node_name = params.getParam<std::string>("name");
        // Note: ImageReaderNode as defined doesn't use NodeConstructParams beyond name
        return std::make_shared<ImageReaderNode>(node_name);
      });

  NodeFactory::instance().registerCreator(
      "InferenceNode",
      [](const NodeConstructParams &params) -> std::shared_ptr<NodeBase> {
        auto node_name = params.getParam<std::string>("name");
        // Note: InferenceNode as defined doesn't use NodeConstructParams beyond name
        return std::make_shared<InferenceNode>(node_name);
      });

  NodeFactory::instance().registerCreator(
      "ResultSaverNode",
      [](const NodeConstructParams &params) -> std::shared_ptr<NodeBase> {
        auto node_name = params.getParam<std::string>("name");
        // Note: ResultSaverNode as defined doesn't use NodeConstructParams beyond name
        return std::make_shared<ResultSaverNode>(node_name);
      });

  NodeFactory::instance().registerCreator(
      "VisualizationNode",
      [](const NodeConstructParams &params) -> std::shared_ptr<NodeBase> {
        auto node_name = params.getParam<std::string>("name");
        // Note: VisualizationNode as defined doesn't use NodeConstructParams beyond name
        return std::make_shared<VisualizationNode>(node_name);
      });
}

} // namespace ai_pipe
