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

  std::unique_ptr<ThreadPool> threadPool = std::make_unique<ThreadPool>();
  threadPool->start(1);
  context.setThreadPool(std::move(threadPool));
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
  graph.addEdge(source->getName(), "demo_source_output_1", sink1->getName(),
                "demo_process_output");
  ASSERT_TRUE(
      pipeline.initializeWithGraph(std::move(graph), std::move(context)));

  // Source nodes don't typically take input, but feedData requires it
  PortData inputData;
  ASSERT_TRUE(pipeline.feedData(inputData));

  // Wait for processing to complete (in a real scenario, you'd use callbacks or
  // polling)
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_EQ(pipeline.getState(),
            PipelineState::IDLE); // Should return to IDLE after processing

  ASSERT_TRUE(pipeline.stop());
}

TEST_F(PipelineDemoTest, SinkDoublePorts) {
  Pipeline pipeline;
  Graph graph;
  PipelineContext context;

  std::unique_ptr<ThreadPool> threadPool = std::make_unique<ThreadPool>();
  threadPool->start(1);
  context.setThreadPool(std::move(threadPool));
  context.setAlgoManager(std::make_shared<infer::dnn::AlgoManager>());

  auto source = std::make_shared<DemoSourceNode>("source_A", 100);
  auto processor1 = std::make_shared<DemoProcessingNode>("processor_1", 10);
  auto processor2 = std::make_shared<DemoProcessingNode>("processor_2", 5);

  std::vector<std::string> sink2NeededPorts{"demo_process_output1",
                                            "demo_process_output2"};
  auto sink = std::make_shared<DemoSinkNode>("sink2", sink2NeededPorts);

  graph.addNode(source);
  graph.addNode(processor1);
  graph.addNode(processor2);
  graph.addNode(sink);

  // Source -> Processor1 -> Sink2
  // Source -> Processor2 -> Sink2
  graph.addEdge(source->getName(), "demo_source_output_0",
                processor1->getName(), "demo_process_input");
  graph.addEdge(processor1->getName(), "demo_process_output", sink->getName(),
                "demo_process_output1");

  graph.addEdge(source->getName(), "demo_source_output_0",
                processor2->getName(), "demo_process_input");
  graph.addEdge(processor2->getName(), "demo_process_output", sink->getName(),
                "demo_process_output2");
  ASSERT_TRUE(
      pipeline.initializeWithGraph(std::move(graph), std::move(context)));

  // Source nodes don't typically take input, but feedData requires it
  PortData inputData;
  ASSERT_TRUE(pipeline.feedData(inputData));

  // Wait for processing to complete (in a real scenario, you'd use callbacks or
  // polling)
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_EQ(pipeline.getState(),
            PipelineState::IDLE); // Should return to IDLE after processing

  ASSERT_TRUE(pipeline.stop());
}

} // namespace testing_demo_pipeline