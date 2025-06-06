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
#include <fstream>
#include <memory>

#include "logger/logger.hpp"
#include "pipeline.hpp"
namespace ai_pipe {

Pipeline::~Pipeline() {
  // Ensure graceful shutdown
  if (state_ == PipelineState::RUNNING || state_ == PipelineState::STOPPING) {
    try {
      stop();
    } catch (const std::exception &e) {
      LOG_ERRORS << "Pipeline destructor: Exception during stop: " << e.what();
    } catch (...) {
      LOG_ERRORS << "Pipeline destructor: Unknown exception during stop.";
    }
  }
}

Pipeline::Pipeline(Pipeline &&other) noexcept
    : graph_(std::move(other.graph_)),
      executionEngine_(std::move(other.executionEngine_)),
      context_(std::move(other.context_)), state_(other.state_.load()),
      onPipelineError_(std::move(other.onPipelineError_)),
      onPipelineResult_(std::move(other.onPipelineResult_)) {
  other.state_ = PipelineState::STOPPED;
}

Pipeline &Pipeline::operator=(Pipeline &&other) noexcept {
  if (this != &other) {
    if (state_ == PipelineState::RUNNING || state_ == PipelineState::STOPPING) {
      stop();
    }
    graph_ = std::move(other.graph_);
    executionEngine_ = std::move(other.executionEngine_);
    context_ = std::move(other.context_);
    state_ = other.state_.load();
    onPipelineError_ = std::move(other.onPipelineError_);
    onPipelineResult_ = std::move(other.onPipelineResult_);

    other.state_ = PipelineState::STOPPED;
  }
  return *this;
}

bool Pipeline::initialize(const PipelineConfig &config,
                          std::shared_ptr<PipelineContext> context_) {
  try {
    graph_ =
        std::make_unique<Graph>(buildGraphFromConfig(config.graphConfigPath));
    context_ = std::move(context_);

    if (graph_->hasCycle()) {
      LOG_ERRORS << "Pipeline: Graph contains a cycle. Initialization failed.";
      return false;
    }

    if (!executionEngine_->initialize(graph_.get(), config.numWorkers)) {
      LOG_ERRORS << "Pipeline: Failed to initialize execution engine.";
      return false;
    }

    state_ = PipelineState::IDLE;

    LOG_INFOS << "Pipeline: Initialized successfully.";
    return true;
  } catch (const std::exception &e) {
    LOG_ERRORS << "Pipeline initialization failed: " << e.what();
    return false;
  }
}

bool Pipeline::initializeWithGraph(Graph &&graph,
                                   std::shared_ptr<PipelineContext> context,
                                   uint8_t numWorkers) {
  graph_ = std::make_unique<Graph>(std::move(graph));
  context_ = std::move(context);
  executionEngine_ = std::make_unique<ExecutionEngine>();

  if (graph_->hasCycle()) {
    return false;
  }

  return executionEngine_->initialize(graph_.get(), numWorkers);
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

PipelineState Pipeline::getState() const {
  return executionEngine_->getState();
}

std::unordered_map<std::string, NodeExecutionState>
Pipeline::getNodeStates() const {
  return executionEngine_->getNodeStates();
}

void Pipeline::setPipelineResultCallback(
    std::function<void(const PortDataMap &finalResults)> callback) {}

void Pipeline::setPipelineErrorCallback(
    std::function<void(const std::string &errorMsg,
                       const std::string &nodeName)>
        callback) {}

void Pipeline::reset() {}

bool Pipeline::feedDataAsync(PortDataMap initialInputs) {}

std::future<bool>
Pipeline::feedDataAndGetResultFuture(PortDataMap initialInputs) {}

Graph Pipeline::buildGraphFromConfig(const std::string &configPath) {
  Graph graph;
  std::ifstream file(configPath);
  if (!file.is_open()) {
    LOG_ERRORS << "Failed to open graph config file: " << configPath;
    throw std::runtime_error("Failed to open graph config file: " + configPath);
  }

  // TODO: 实现具体的配置解析逻辑
  // 1. 解析节点配置，通过工厂创建节点(use type_safe_factory)
  // 2. 解析边配置，调用graph_.addEdge()
  // 3. 验证配置的完整性和一致性

  return graph;
}
} // namespace ai_pipe