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
  Pipeline() : state_(PipelineState::IDLE) {}
  ~Pipeline();

  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(const Pipeline &) = delete;

  Pipeline(Pipeline &&other) noexcept;
  Pipeline &operator=(Pipeline &&other) noexcept;

  // 初始化（从配置文件）
  bool initialize(const PipelineConfig &config,
                  std::shared_ptr<PipelineContext> context_);

  // 手动构建图（用于测试或程序化构建）
  bool initializeWithGraph(Graph &&graph,
                           std::shared_ptr<PipelineContext> context,
                           uint8_t numWorkers = 1);

  bool start();

  bool stop();

  void reset();

  // 数据驱动执行
  bool feedDataAsync(PortDataMap initialInputs);

  std::future<bool> feedDataAndGetResultFuture(PortDataMap initialInputs);

  PipelineState getState() const;

  std::unordered_map<std::string, NodeExecutionState> getNodeStates() const;

  // 结果回调设置
  void setPipelineResultCallback(
      std::function<void(const PortDataMap &finalResults)> callback);

  void setPipelineErrorCallback(std::function<void(const std::string &errorMsg,
                                                   const std::string &nodeName)>
                                    callback);

  // 获取graph（用于可能的调试和监控）
  const Graph &getGraph() const { return *graph_; }

  // 获取context（用于运行时资源访问）
  PipelineContext &getContext() { return *context_; }

private:
  // 从配置文件构建图
  Graph buildGraphFromConfig(const std::string &configPath);

private:
  std::unique_ptr<Graph> graph_;
  std::unique_ptr<ExecutionEngine> executionEngine_;
  std::atomic<PipelineState> state_;

  std::shared_ptr<PipelineContext> context_;

  std::function<void(const std::string &errorMsg, const std::string &nodeName)>
      onPipelineError_;
  std::function<void(const PortDataMap &finalResults)> onPipelineResult_;
};
} // namespace ai_pipe

#endif