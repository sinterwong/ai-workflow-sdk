/**
 * @file serial_graph.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "serial_graph.hpp"
#include "logger/logger.hpp"
#include <queue>
#include <stdexcept>
#include <unordered_map>
namespace ai_pipe {
void SerialGraph::addNode(std::shared_ptr<NodeBase> node) {
  if (!node) {
    LOG_ERRORS << "Node cannot be null";
    throw std::invalid_argument("Node cannot be null");
  }

  // check whether to repeat
  if (nodeMap_.find(node->getName()) != nodeMap_.end()) {
    LOG_ERRORS << "Node " << node->getName() << " already exists";
    throw std::invalid_argument("Node " + node->getName() + " already exists");
  }

  nodes_.push_back(node);
  nodeMap_[node->getName()] = node;
  inDegree_[node] = 0;
}

void SerialGraph::addEdge(std::shared_ptr<NodeBase> sourceNode,
                          std::string sourcePort,
                          std::shared_ptr<NodeBase> destNode,
                          std::string destPort) {
  auto sourceNodeIter = nodeMap_.find(sourceNode->getName());
  auto destNodeIter = nodeMap_.find(destNode->getName());

  if (sourceNodeIter == nodeMap_.end()) {
    LOG_ERRORS << "Source node " << sourceNode->getName() << " not found";
    throw std::invalid_argument("Source node " + sourceNode->getName() +
                                " not found");
  }
  if (destNodeIter == nodeMap_.end()) {
    LOG_ERRORS << "Destination node " << destNode->getName() << " not found";
    throw std::invalid_argument("Destination node " + destNode->getName() +
                                " not found");
  }
  edges_.emplace_back(sourceNodeIter->second, sourcePort, destNodeIter->second,
                      destPort);
  // update adjacency list and in-degress
  adj_[sourceNodeIter->second].push_back(destNodeIter->second);
  inDegree_[destNodeIter->second]++;
}

void SerialGraph::removeNode(std::shared_ptr<NodeBase> node) {}

void SerialGraph::run() {
  // Kahn's algorithm
  std::vector<std::shared_ptr<NodeBase>> sortedNodes;
  std::queue<std::shared_ptr<NodeBase>> q;
  std::unordered_map<std::shared_ptr<NodeBase>, int> currentInDegree =
      inDegree_;

  for (const auto &node : nodes_) {
    if (currentInDegree.find(node) == currentInDegree.end() ||
        currentInDegree[node] == 0) {
      currentInDegree[node] = inDegree_[node];
      q.push(node);
      LOG_INFOS << "Initial node with in-degree 0: " << node->getName();
    }
  }
  while (!q.empty()) {
    auto vertex = q.front();
    q.pop();
    sortedNodes.push_back(vertex);

    LOG_INFOS << "Processing node in topological sort: " << vertex->getName();

    if (adj_.count(vertex)) {
      for (const auto &v : adj_[vertex]) {
        currentInDegree[v]--;
        if (currentInDegree[v] == 0) {
          q.push(v);
          LOG_INFOS << "Enqueuing node with in-degree 0: " << v->getName();
        }
      }
    }
  }

  // check whether the graph contains a cycle
  if (sortedNodes.size() != nodes_.size()) {
    LOG_ERRORS << "SerialGraph contains a cycle";
    throw std::runtime_error("SerialGraph contains a cycle");
  }

  LOG_INFOS << "Topological sort successful. Execution order: ";
  for (const auto &node : sortedNodes) {
    LOG_INFOS << node->getName() << " ";
  }

  // execute node based on topological order
  std::map<std::pair<std::shared_ptr<NodeBase>, std::string>, std::any>
      pipelineData;
  for (const auto &currNode : sortedNodes) {
    LOG_INFOS << " Executing node: " << currNode->getName();

    // prepare input data
    PortDataMap inputs;
    for (const auto &edge : edges_) {
      if (edge.destNode == currNode) {
        // get the output data of source from pipelineData
        auto dataKey = std::make_pair(edge.sourceNode, edge.sourcePort);
        auto it = pipelineData.find(dataKey);
        if (it != pipelineData.end()) {
          inputs.params[edge.destPort] = it->second;
        } else {
          LOG_ERRORS << "Data not found for edge: "
                     << edge.sourceNode->getName() << ":" << edge.sourcePort
                     << " -> " << edge.destNode->getName() << ":"
                     << edge.destPort;
          throw std::runtime_error(
              "Data not found for edge: " + edge.sourceNode->getName() + ":" +
              edge.sourcePort + " -> " + edge.destNode->getName() + ":" +
              edge.destPort);
        }
      }
    }

    // execute node's process
    PortDataMap outputs;
    try {
      currNode->process(inputs, outputs);

    } catch (const std::exception &e) {
      LOG_ERRORS << "Error processing node " << currNode->getName() << ": "
                 << e.what();
      throw;
    }
    // update pipelineData
    for (const auto &output : outputs.params) {
      pipelineData[std::make_pair(currNode, output.first)] = output.second;
    }
    LOG_INFOS << "Processed node: " << currNode->getName();
  }
  LOG_INFOS << "Pipeline execution successful.";
}

} // namespace ai_pipe
