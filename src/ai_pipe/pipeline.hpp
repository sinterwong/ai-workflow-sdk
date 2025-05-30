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
#include <future>

namespace ai_pipe {
class Pipeline {
public:
  Pipeline(Graph graph, PipelineContext context, ExecutionEngine engine);
  ~Pipeline();

  std::future<void> submit(const PipeInputData &inputData);

private:
  Graph graph_;
  std::shared_ptr<PipelineContext> context_;
  std::shared_ptr<ExecutionEngine> engine_;
};

} // namespace ai_pipe

#endif