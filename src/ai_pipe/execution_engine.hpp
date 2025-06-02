/**
 * @file execution_engine.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __PIPE_EXECUTION_ENGINE_HPP__
#define __PIPE_EXECUTION_ENGINE_HPP__
#include "graph.hpp"
#include "pipe_types.hpp"
#include "pipeline_context.hpp"
#include <memory>
#include <string>

namespace ai_pipe {
class ExecutionEngine {
public:
  ExecutionEngine() : state_(PipelineState::IDLE) {}
  ~ExecutionEngine() = default;

  // 禁止拷贝，允许移动
  ExecutionEngine(const ExecutionEngine &) = delete;
  ExecutionEngine &operator=(const ExecutionEngine &) = delete;
  ExecutionEngine(ExecutionEngine &&) = delete;
  ExecutionEngine &operator=(ExecutionEngine &&) = delete;

  bool initialize(Graph *graph, PipelineContext *context);

  // 数据驱动执行
  bool execute(const PortData &inputData);

  // 状态管理
  void reset();

  PipelineState getState() const { return state_; }

  // 获取节点执行统计信息
  std::unordered_map<std::string, NodeExecutionState> getNodeStates() const;

private:
  // 重置执行状态
  void resetExecutionState();

  // 找到起始节点（入度为0的节点）
  std::vector<std::shared_ptr<NodeBase>> findStartNodes();

  // 分发输入数据到起始节点
  void
  distributeInputData(const std::vector<std::shared_ptr<NodeBase>> &startNodes,
                      const PortData &inputData);

  // 更新节点的就绪状态
  void updateNodeReadyState(std::shared_ptr<NodeBase> node);

  // 执行流程
  bool executeFlow();

  // 找到就绪的节点
  std::vector<std::shared_ptr<NodeBase>> findReadyNodes();

  // 执行单个节点
  bool executeNode(std::shared_ptr<NodeBase> node);

  // 传播节点输出到下游节点
  void propagateOutputs(std::shared_ptr<NodeBase> node);

  // 检查所有节点是否都执行完成
  bool allNodesCompleted();

private:
  Graph *graph_;
  PipelineContext *context_;
  std::atomic<PipelineState> state_;

  // 节点执行状态
  std::unordered_map<std::shared_ptr<NodeBase>, NodeExecutionState> nodeStates_;
  std::unordered_map<std::shared_ptr<NodeBase>, PortDataMap> nodeInputBuffers_;
  std::unordered_map<std::shared_ptr<NodeBase>, PortDataMap> nodeOutputBuffers_;
};

} // namespace ai_pipe

#endif
