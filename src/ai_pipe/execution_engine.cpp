/**
 * @file execution_engine.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "execution_engine.hpp"
namespace ai_pipe {
bool ExecutionEngine::initialize(Graph *graph, PipelineContext *context) {
  if (!graph || !context) {
    return false;
  }

  graph_ = graph;
  context_ = context;

  if (!context_->isValid()) {
    return false;
  }

  // 初始化节点状态
  for (const auto &node : graph_->getNodes()) {
    nodeStates_[node] = NodeExecutionState::WAITING;
    nodeInputBuffers_[node] = PortDataMap{};
    nodeOutputBuffers_[node] = PortDataMap{};
  }

  state_ = PipelineState::IDLE;
  return true;
}

bool ExecutionEngine::execute(const PortData &inputData) {
  if (state_ != PipelineState::IDLE) {
    return false;
  }

  state_ = PipelineState::RUNNING;

  try {
    // 重置执行状态
    resetExecutionState();

    // 找到入度为0的起始节点并分发数据
    auto startNodes = findStartNodes();
    if (startNodes.empty()) {
      state_ = PipelineState::ERROR;
      return false;
    }

    // 将输入数据分发到起始节点
    distributeInputData(startNodes, inputData);

    // 开始执行流程
    bool success = executeFlow();

    state_ = success ? PipelineState::IDLE : PipelineState::ERROR;
    return success;

  } catch (const std::exception &e) {
    state_ = PipelineState::ERROR;
    return false;
  }
}

void ExecutionEngine::reset() {
  state_ = PipelineState::IDLE;
  resetExecutionState();
}

std::unordered_map<std::string, NodeExecutionState>
ExecutionEngine::getNodeStates() const {
  std::unordered_map<std::string, NodeExecutionState> result;
  for (const auto &[node, state] : nodeStates_) {
    result[node->getName()] = state;
  }
  return result;
}

void ExecutionEngine::resetExecutionState() {
  for (auto &[node, state] : nodeStates_) {
    state = NodeExecutionState::WAITING;
  }
  for (auto &[node, buffer] : nodeInputBuffers_) {
    buffer.clear();
  }
  for (auto &[node, buffer] : nodeOutputBuffers_) {
    buffer.clear();
  }
}

std::vector<std::shared_ptr<NodeBase>> ExecutionEngine::findStartNodes() {
  std::vector<std::shared_ptr<NodeBase>> startNodes;
  for (const auto &node : graph_->getNodes()) {
    if (graph_->getInDegree(node) == 0) {
      startNodes.push_back(node);
    }
  }
  return startNodes;
}

void ExecutionEngine::distributeInputData(
    const std::vector<std::shared_ptr<NodeBase>> &startNodes,
    const PortData &inputData) {
  for (const auto &node : startNodes) {
    auto inputPorts = node->getExpectedInputPorts();
    if (inputPorts.empty()) {
      // 没有输入端口的节点，直接标记为就绪
      nodeStates_[node] = NodeExecutionState::READY;
    } else {
      // 将输入数据分发到第一个输入端口
      nodeInputBuffers_[node][inputPorts[0]] = inputData;
      updateNodeReadyState(node);
    }
  }
}

void ExecutionEngine::updateNodeReadyState(std::shared_ptr<NodeBase> node) {
  auto expectedInputs = node->getExpectedInputPorts();
  if (expectedInputs.empty()) {
    nodeStates_[node] = NodeExecutionState::READY;
    return;
  }

  // 检查所有必需的输入端口是否都有数据
  bool allInputsReady = true;
  for (const auto &port : expectedInputs) {
    if (nodeInputBuffers_[node].find(port) == nodeInputBuffers_[node].end()) {
      allInputsReady = false;
      break;
    }
  }

  if (allInputsReady) {
    nodeStates_[node] = NodeExecutionState::READY;
  }
}

bool ExecutionEngine::executeFlow() {
  bool hasProgress = true;

  while (hasProgress) {
    hasProgress = false;

    // 找到所有就绪的节点
    auto readyNodes = findReadyNodes();
    if (readyNodes.empty()) {
      break;
    }

    // 并行执行就绪的节点
    std::vector<std::future<bool>> futures;
    for (const auto &node : readyNodes) {
      nodeStates_[node] = NodeExecutionState::EXECUTING;

      auto future = context_->getThreadPool()->submit(
          [this, node]() { return executeNode(node); });
      futures.push_back(std::move(future));
    }

    // 等待所有节点执行完成
    bool allSuccess = true;
    for (auto &future : futures) {
      if (!future.get()) {
        allSuccess = false;
      }
    }

    if (!allSuccess) {
      return false;
    }

    // 传播输出数据到下游节点
    for (const auto &node : readyNodes) {
      if (nodeStates_[node] == NodeExecutionState::COMPLETED) {
        propagateOutputs(node);
        hasProgress = true;
      }
    }
  }
  return allNodesCompleted();
}

std::vector<std::shared_ptr<NodeBase>> ExecutionEngine::findReadyNodes() {
  std::vector<std::shared_ptr<NodeBase>> readyNodes;
  for (const auto &[node, state] : nodeStates_) {
    if (state == NodeExecutionState::READY) {
      readyNodes.push_back(node);
    }
  }
  return readyNodes;
}

bool ExecutionEngine::executeNode(std::shared_ptr<NodeBase> node) {
  try {
    auto &inputs = nodeInputBuffers_[node];
    auto &outputs = nodeOutputBuffers_[node];

    // 调用节点的处理函数
    node->process(inputs, outputs);

    nodeStates_[node] = NodeExecutionState::COMPLETED;
    return true;

  } catch (const std::exception &e) {
    nodeStates_[node] = NodeExecutionState::FAILED;
    return false;
  }
}

// 传播节点输出到下游节点
void ExecutionEngine::propagateOutputs(std::shared_ptr<NodeBase> node) {
  auto outgoingEdges = graph_->getOutgoingEdges(node);
  auto &outputs = nodeOutputBuffers_[node];

  for (const auto &edge : outgoingEdges) {
    auto sourcePort = edge.sourcePort;
    auto destNode = edge.destNode;
    auto destPort = edge.destPort;

    // 将输出数据复制到目标节点的输入缓冲区
    if (outputs.find(sourcePort) != outputs.end()) {
      nodeInputBuffers_[destNode][destPort] = outputs[sourcePort];

      // 更新目标节点的就绪状态
      updateNodeReadyState(destNode);
    }
  }
}

// 检查所有节点是否都执行完成
bool ExecutionEngine::allNodesCompleted() {
  for (const auto &[node, state] : nodeStates_) {
    if (state != NodeExecutionState::COMPLETED &&
        state != NodeExecutionState::FAILED) {
      return false;
    }
  }
  return true;
}

} // namespace ai_pipe
