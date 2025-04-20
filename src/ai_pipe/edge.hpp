/**
 * @file edge.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __PIPE_EDGE_HPP__
#define __PIPE_EDGE_HPP__
#include "node_base.hpp"
#include <memory>
namespace ai_pipe {
struct Edge {
  std::shared_ptr<NodeBase> sourceNode;
  std::string sourcePort;
  std::shared_ptr<NodeBase> destNode;
  std::string destPort;

  Edge(std::shared_ptr<NodeBase> sourceNode, std::string sourcePort,
       std::shared_ptr<NodeBase> destNode, std::string destPort)
      : sourceNode(sourceNode), sourcePort(sourcePort), destNode(destNode),
        destPort(destPort) {}
};
} // namespace ai_pipe

#endif