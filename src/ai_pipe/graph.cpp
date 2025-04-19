/**
 * @file graph.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "graph.hpp"
namespace ai_pipe {
void Graph::addNode(std::shared_ptr<NodeBase> node) {}

void Graph::addEdge(std::shared_ptr<NodeBase> sourceNode,
                    std::string sourcePort, std::shared_ptr<NodeBase> destNode,
                    std::string destPort) {}

void Graph::removeNode(std::shared_ptr<NodeBase> node) {}

void Graph::run() {}

} // namespace ai_pipe
