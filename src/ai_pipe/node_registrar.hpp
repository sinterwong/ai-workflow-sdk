#ifndef __NODE_REGISTRAR_HPP__
#define __NODE_REGISTRAR_HPP__

#include "node_base.hpp"
#include "node_param_types.hpp"
#include "utils/type_safe_factory.hpp"

#include "demo_nodes.hpp"

namespace ai_pipe {

class NodeRegistrar {
public:
  static NodeRegistrar &getInstance() {
    static NodeRegistrar instance;
    return instance;
  }

  NodeRegistrar(const NodeRegistrar &) = delete;
  NodeRegistrar &operator=(const NodeRegistrar &) = delete;
  NodeRegistrar(NodeRegistrar &&) = delete;
  NodeRegistrar &operator=(NodeRegistrar &&) = delete;

private:
  NodeRegistrar() {
    utils::Factory<NodeBase>::instance().registerCreator(
        "DemoSourceNode",
        [](const utils::DataPacket &params) -> std::shared_ptr<NodeBase> {
          auto node_name = params.getParam<std::string>("name");
          auto node_specific_params =
              params.getParam<DemoSourceNodeParams>("node_specific_params");
          return std::make_shared<DemoSourceNode>(node_name,
                                                  node_specific_params);
        });

    utils::Factory<NodeBase>::instance().registerCreator(
        "DemoProcessingNode",
        [](const utils::DataPacket &params) -> std::shared_ptr<NodeBase> {
          auto node_name = params.getParam<std::string>("name");
          auto node_specific_params =
              params.getParam<DemoProcessingNodeParams>("node_specific_params");
          return std::make_shared<DemoProcessingNode>(node_name,
                                                      node_specific_params);
        });

    utils::Factory<NodeBase>::instance().registerCreator(
        "DemoSinkNode",
        [](const utils::DataPacket &params) -> std::shared_ptr<NodeBase> {
          auto node_name = params.getParam<std::string>("name");
          auto node_specific_params =
              params.getParam<DemoSinkNodeParams>("node_specific_params");
          return std::make_shared<DemoSinkNode>(node_name,
                                                node_specific_params);
        });
  }
};

inline const static NodeRegistrar &node_registrar =
    NodeRegistrar::getInstance();

} // namespace ai_pipe

#endif // __NODE_REGISTRAR_HPP__
