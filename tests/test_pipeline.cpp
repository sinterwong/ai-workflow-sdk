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
#include "ai_pipe/pipe_types.hpp"
#include "ai_pipe/pipeline.hpp"
#include "ai_pipe/pipeline_context.hpp"
#include "logger/logger.hpp"

#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <memory>
#include <thread>

namespace testing_demo_pipeline {
using namespace ai_pipe;

class PipelineDemoTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(PipelineDemoTest, InitializeAndRunFromConfig) {
  Pipeline pipeline;
  // PipelineContext context; // Default context is created by
  // pipeline.initialize if null
  // context.setAlgoManager(std::make_shared<infer::dnn::AlgoManager>()); // If
  // needed by nodes

  // Path to the configuration file.
  // This path needs to be accessible when the test is run.
  // Assuming the test binary runs from a directory where "tests/conf/..." is
  // valid. For robustness, tests often copy config files to the build/test
  // output dir or use relative paths carefully. For now, let's assume this path
  // works. Path adjusted to be relative to the test executable's CWD after
  // CMake copy.
  std::string config_path = "conf/test_pipeline_config.json";

  // Ensure the config file exists (the previous step should create it)
  // Alternatively, for hermetic tests, create it programmatically here:
  // std::ofstream outfile(config_path);
  // outfile << R"JSON({ ...your json content... })JSON";
  // outfile.close();

  PipelineConfig pipeline_config;
  pipeline_config.graphConfigPath = config_path;
  pipeline_config.numWorkers = 2; // Example worker count

  // Initialize the pipeline
  // Pass a new context or let pipeline create one.
  // If your demo nodes (or any real nodes) depend on AlgoManager or other
  // context resources, you need to set them up in the context.
  auto context = std::make_shared<PipelineContext>();
  // If AlgoManager is needed by any node that might be created:
  // context->setAlgoManager(std::make_shared<infer::dnn::AlgoManager>());

  ASSERT_TRUE(pipeline.initialize(pipeline_config, context))
      << "Pipeline initialization from config failed.";

  // Verify graph structure (optional, basic checks)
  const auto &graph = pipeline.getGraph();
  ASSERT_EQ(graph.getNodes().size(), 3);
  ASSERT_EQ(graph.getEdges().size(), 3);

  // Check a specific node
  auto sourceNode = graph.getNode("MySource");
  ASSERT_NE(sourceNode, nullptr);
  ASSERT_EQ(sourceNode->getName(), "MySource");

  // Prepare for pipeline execution
  bool result_received = false;
  bool error_occurred = false;
  std::string error_message;

  pipeline.setPipelineResultCallback([&](const PortDataMap &finalResults) {
    LOG_INFOS << "TestFromConfig: Pipeline completed successfully.";
    // Add checks for finalResults if needed
    result_received = true;
  });

  pipeline.setPipelineErrorCallback(
      [&](const std::string &errMsg, const std::string &nodeName) {
        LOG_ERRORS << "TestFromConfig: Pipeline error in node " << nodeName
                   << ": " << errMsg;
        error_occurred = true;
        error_message = errMsg;
      });

  ASSERT_EQ(pipeline.getState(), PipelineState::IDLE);
  ASSERT_TRUE(pipeline.start());
  ASSERT_EQ(pipeline.getState(), PipelineState::RUNNING);

  // Feed initial data (if source node doesn't generate its own without input)
  // DemoSourceNode generates data without input, so initialInputs can be empty.
  PortDataMap initialInputs;
  std::future<bool> submission_future =
      pipeline.feedDataAndGetResultFuture(initialInputs);

  ASSERT_TRUE(submission_future.get()) << "Data submission failed.";

  // Wait for pipeline to process. This is tricky for async pipelines.
  // For a test, we need a way to know it's done.
  // The ExecutionEngine and Pipeline are asynchronous.
  // We need to wait for either result_received or error_occurred.
  // A simple busy wait or a condition variable is needed for robust testing.
  // For now, let's use a timeout loop.

  std::chrono::seconds timeout(10); // 10-second timeout
  auto start_time = std::chrono::steady_clock::now();
  while (!result_received && !error_occurred) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (std::chrono::steady_clock::now() - start_time > timeout) {
      FAIL() << "Pipeline test timed out waiting for result or error.";
      break;
    }
  }

  ASSERT_FALSE(error_occurred) << "Pipeline error: " << error_message;
  ASSERT_TRUE(result_received) << "Pipeline result callback was not invoked.";

  // Stop the pipeline
  ASSERT_TRUE(pipeline.stop());
  ASSERT_EQ(pipeline.getState(), PipelineState::STOPPED);

  // Reset for potential reuse
  pipeline.reset();
  ASSERT_EQ(pipeline.getState(), PipelineState::IDLE);
}

} // namespace testing_demo_pipeline