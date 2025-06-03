/**
 * @file pipeline.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __PIPE_PIPELINE_HPP__
#define __PIPE_PIPELINE_HPP__
#include "execution_engine.hpp"
#include "graph.hpp"
#include "pipeline_context.hpp"

namespace ai_pipe {
class Pipeline {
public:
  Pipeline() = default;
  ~Pipeline() = default;

  // 禁止拷贝，允许移动
  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(const Pipeline &) = delete;
  Pipeline(Pipeline &&) = default;
  Pipeline &operator=(Pipeline &&) = default;

  // 初始化（从配置文件）
  bool initialize(const std::string &configPath);

  // 手动构建图（用于测试或程序化构建）
  bool initializeWithGraph(Graph &&graph, PipelineContext &&context);

  bool start();

  bool stop();

  bool pause();

  bool resume();

  // 数据驱动执行
  bool feedData(const PortData &data);

  PipelineState getState() const;

  std::unordered_map<std::string, NodeExecutionState> getNodeStates() const;

  // 结果回调设置
  void setResultCallback(std::function<void(const PortData &)> callback);

  // 获取graph（用于可能的调试和监控）
  const Graph &getGraph() const { return *graph_; }

  // 获取context（用于运行时资源访问）
  PipelineContext &getContext() { return *context_; }

private:
  // 从配置文件构建图
  bool buildGraphFromConfig(const std::string &configPath);

private:
  std::unique_ptr<Graph> graph_;
  std::unique_ptr<ExecutionEngine> executionEngine_;
  std::unique_ptr<PipelineContext> context_;
  std::function<void(const PortData &)> resultCallback_;
};
} // namespace ai_pipe

#endif