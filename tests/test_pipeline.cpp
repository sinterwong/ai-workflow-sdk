/**
 * @file test_pipeline.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-06-01
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "ai_pipe/demo_nodes.hpp"
#include "ai_pipe/pipe_types.hpp"
#include "ai_pipe/pipeline.hpp"
#include "ai_pipe/pipeline_context.hpp"
#include "core/algo_manager.hpp"
#include "gtest/gtest.h"

#include <memory>

namespace testing_demo_pipeline {
using namespace ai_pipe;

class PipelineDemoTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(PipelineDemoTest, Normal) {
  Pipeline pipeline;
  Graph graph;
  PipelineContext context;

  context.setAlgoManager(std::make_shared<infer::dnn::AlgoManager>());

  auto source = std::make_shared<DemoSourceNode>("source_A", 100);
  auto processor1 = std::make_shared<DemoProcessingNode>("processor_1", 10);

  std::vector<std::string> sink1NeededPorts{"demo_process_output"};
  auto sink1 = std::make_shared<DemoSinkNode>("sink1", sink1NeededPorts);

  graph.addNode(source);
  graph.addNode(processor1);
  graph.addNode(sink1);

  // Source -> Porcessor1 -> Sink1
  // Source -> Sink1
  graph.addEdge(source->getName(), "demo_source_output_0",
                processor1->getName(), "demo_process_input");
  graph.addEdge(processor1->getName(), "demo_process_output", sink1->getName(),
                "demo_process_output");

  ASSERT_TRUE(pipeline.initializeWithGraph(
      std::move(graph), std::make_shared<PipelineContext>(std::move(context)),
      4));

  PortDataMap initialInputs;

  // Execute and wait for completion
  ASSERT_TRUE(pipeline.feedDataAndGetResultFuture(initialInputs).get());

  // Check final state
  ASSERT_EQ(pipeline.getState(), PipelineState::IDLE);

  // Check node states (optional, for debugging)
  auto nodeStates = pipeline.getNodeStates();
  ASSERT_EQ(nodeStates[source->getName()], NodeExecutionState::COMPLETED);
  ASSERT_EQ(nodeStates[processor1->getName()], NodeExecutionState::COMPLETED);
  ASSERT_EQ(nodeStates[sink1->getName()], NodeExecutionState::COMPLETED);

  // Reset the pipeline for potential reuse
  pipeline.reset();
  ASSERT_EQ(pipeline.getState(), PipelineState::IDLE);
}

TEST_F(PipelineDemoTest, SinkDoublePorts) {}

} // namespace testing_demo_pipeline