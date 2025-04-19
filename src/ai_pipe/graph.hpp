/**
 * @file graph.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __PIPE_GRAPH_HPP__
#define __PIPE_GRAPH_HPP__
#include "edge.hpp"
#include "node_base.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
namespace ai_pipe {
class Graph {
public:
  Graph() {}
  ~Graph() {}
  void addNode(std::shared_ptr<NodeBase> node);

  void addEdge(std::shared_ptr<NodeBase> sourceNode, std::string sourcePort,
               std::shared_ptr<NodeBase> destNode, std::string destPort);

  void removeNode(std::shared_ptr<NodeBase> node);

  void run();

private:
  std::vector<std::shared_ptr<NodeBase>> nodes_;
  std::vector<std::shared_ptr<Edge>> edges_;
  std::unordered_map<std::string, std::shared_ptr<NodeBase>> nodeMap_;

  std::unordered_map<std::shared_ptr<NodeBase>,
                     std::vector<std::shared_ptr<NodeBase>>>
      adj_;

  std::unordered_map<std::shared_ptr<NodeBase>, int> inDegree_;
};

} // namespace ai_pipe

#endif
