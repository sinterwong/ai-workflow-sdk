/**
 * @file pipeline.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "pipeline.hpp"
#include <fstream>

namespace ai_pipe {

bool Pipeline::initialize(const std::string &configPath) {
  try {
    if (!buildGraphFromConfig(configPath)) {
      return false;
    }

    if (graph_->hasCycle()) {
      return false;
    }

    if (!executionEngine_->initialize(graph_.get(), context_.get())) {
      return false;
    }

    return true;

  } catch (const std::exception &e) {
    return false;
  }
}

bool Pipeline::initializeWithGraph(Graph &&graph, PipelineContext &&context) {
  graph_ = std::make_unique<Graph>(std::move(graph));
  context_ = std::make_unique<PipelineContext>(std::move(context));
  executionEngine_ = std::make_unique<ExecutionEngine>();

  if (graph_->hasCycle()) {
    return false;
  }

  return executionEngine_->initialize(graph_.get(), context_.get());
}

bool Pipeline::start() {
  if (executionEngine_->getState() == PipelineState::IDLE) {
    return true;
  }
  return false;
}

bool Pipeline::stop() {
  executionEngine_.reset();
  return true;
}

bool Pipeline::pause() {
  // TODO: 暂停功能的实现需要更复杂的状态管理
  return false;
}

bool Pipeline::resume() {
  // TODO: 恢复功能的实现
  return false;
}

// 数据驱动执行（也可以用Source类型的节点）
bool Pipeline::feedData(const PortData &data) {
  return executionEngine_->execute(data);
}

PipelineState Pipeline::getState() const {
  return executionEngine_->getState();
}

std::unordered_map<std::string, NodeExecutionState>
Pipeline::getNodeStates() const {
  return executionEngine_->getNodeStates();
}

void Pipeline::setResultCallback(
    std::function<void(const PortData &)> callback) {
  resultCallback_ = std::move(callback);
}

bool Pipeline::buildGraphFromConfig(const std::string &configPath) {
  std::ifstream file(configPath);
  if (!file.is_open()) {
    return false;
  }

  // TODO: 实现具体的配置解析逻辑
  // 1. 解析节点配置，通过工厂创建节点
  // 2. 解析边配置，调用graph_.addEdge()
  // 3. 验证配置的完整性和一致性

  return true;
}
} // namespace ai_pipe