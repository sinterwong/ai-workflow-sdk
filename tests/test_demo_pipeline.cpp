#include "ai_pipe/pipe_types.hpp"
#include "ai_pipe/pipeline.hpp"
#include "ai_pipe/pipeline_builder.hpp"
#include "ai_pipe/pipeline_context.hpp"
#include "algo_manager.hpp"
#include "algo_registrar.hpp"
#include "logger/logger.hpp"
#include "nlohmann/json.hpp"
#include "gtest/gtest.h"
#include <filesystem>
#include <fstream>

namespace testing_demo_pipeline {

using namespace infer;
using namespace infer::dnn;

AlgoConstructParams loadParamFromJson(const std::string &configPath) {

  AlgoConstructParams params;

  std::ifstream file(configPath);
  if (!file.is_open()) {
    LOG_ERRORS << "Failed to open config file: " << configPath;
    throw std::runtime_error("Failed to open config file: " + configPath);
  }

  nlohmann::json j;
  try {
    file >> j;
  } catch (const nlohmann::json::parse_error &e) {
    LOG_ERRORS << "Failed to parse config JSON: " << e.what();
    throw std::runtime_error("Failed to parse config JSON: " +
                             std::string(e.what()));
  }

  if (!j.contains("algorithms") || !j["algorithms"].is_array()) {
    LOG_ERRORS << "Config missing 'algorithms' array or it's not an array.";
    throw std::runtime_error(
        "Config missing 'algorithms' array or not an array.");
  }

  if (j["algorithms"].empty()) {
    LOG_ERRORS << "Config 'algorithms' array is empty.";
    throw std::runtime_error("Config 'algorithms' array is empty.");
  }

  // Assuming we are loading parameters for the first algorithm in the list
  const auto &algoConfig = j["algorithms"][0];

  std::string moduleUniqeName = algoConfig.at("name").get<std::string>();
  params.setParam("moduleUniqeName", moduleUniqeName);

  std::string moduleName = algoConfig.at("type").get<std::string>();
  params.setParam("moduleName", moduleName);

  if (algoConfig.contains("inferParams")) {
    AlgoInferParams inferParams;
    const auto &inferJson = algoConfig["inferParams"];
    FrameInferParam frameInferParam;
    frameInferParam.modelPath = inferJson.at("modelPath").get<std::string>();
    if (inferJson.contains("inputShape")) {
      frameInferParam.inputShape.w = inferJson["inputShape"].at("w").get<int>();
      frameInferParam.inputShape.h = inferJson["inputShape"].at("h").get<int>();
    }
    frameInferParam.deviceType =
        static_cast<DeviceType>(inferJson.at("deviceType").get<int>());
    frameInferParam.dataType =
        static_cast<DataType>(inferJson.at("dataType").get<int>());
    inferParams.setParams(frameInferParam);
    params.setParam("inferParams", inferParams);
  }

  if (algoConfig.contains("postProcParams")) {
    AlgoPostprocParams postProcParams;
    const auto &postProcJson = algoConfig["postProcParams"];
    AnchorDetParams anchorDetParams;
    anchorDetParams.condThre = postProcJson.at("condThre").get<float>();
    anchorDetParams.nmsThre = postProcJson.at("nmsThre").get<float>();
    if (postProcJson.contains("inputShape")) {
      anchorDetParams.inputShape.w =
          postProcJson["inputShape"].at("w").get<int>();
      anchorDetParams.inputShape.h =
          postProcJson["inputShape"].at("h").get<int>();
    }
    postProcParams.setParams(anchorDetParams);
    params.setParam("postProcParams", postProcParams);
  }
  return params;
}

TEST(DemoPipelineTest, RunPipeline) {
  const std::string imagePathStr = "data/yolov11/image.png";

  std::filesystem::path inputImagePath(imagePathStr);
  std::string expectedOutputFileName = inputImagePath.stem().string() +
                                       "_0_visualized" +
                                       inputImagePath.extension().string();
  std::string expectedOutputPathStr =
      (std::filesystem::path("output_visualizations") / expectedOutputFileName)
          .string();

  const std::string graphConfigPath = "conf/test_demo_pipeline_config.json";
  ai_pipe::Pipeline pipeline;
  auto context = std::make_shared<ai_pipe::PipelineContext>();
  // create algoManager
  AlgoConstructParams params = loadParamFromJson("conf/test_algo_manager.json");
  std::shared_ptr<AlgoInferBase> engine =
      AlgoInferFactory::instance().create("VisionInfer", params);
  ASSERT_NE(engine, nullptr);
  ASSERT_EQ(engine->initialize(), InferErrorCode::SUCCESS);
  std::shared_ptr<AlgoManager> algoManager = std::make_shared<AlgoManager>();
  ASSERT_NE(algoManager, nullptr);
  auto moduleUniqueName = params.getParam<std::string>("moduleUniqeName");
  ASSERT_EQ(algoManager->registerAlgo(moduleUniqueName, engine),
            InferErrorCode::SUCCESS);
  ASSERT_TRUE(algoManager->hasAlgo(moduleUniqueName));
  ASSERT_NE(algoManager->getAlgo(moduleUniqueName), nullptr);
  context->setAlgoManager(algoManager);
  ASSERT_TRUE(context->isValid());

  ai_pipe::PipelineConfig pipelineConfig;
  pipelineConfig.graphConfigPath = graphConfigPath;
  pipelineConfig.numWorkers = 4;

  ASSERT_TRUE(pipeline.initialize(pipelineConfig, context));
  ASSERT_EQ(pipeline.getState(), ai_pipe::PipelineState::IDLE);

  ai_pipe::PortDataMap inputs;
  auto imagePathData = std::make_shared<ai_pipe::PortData>();
  imagePathData->setParam<std::string>("image_path", imagePathStr);
  // Use the top-level input port name defined in the graph's "inputs" section
  inputs["ImageReader"] = imagePathData;

  bool resultReceived = false;
  bool errorOccurred = false;
  std::string lastErrorMsg;

  pipeline.setPipelineResultCallback(
      [&](const ai_pipe::PortDataMap &finalResults) {
        resultReceived = true;
        const std::string outputPortKey =
            "Visualization:visualized_image_output_data";
        ASSERT_TRUE(finalResults.count(outputPortKey));
        const auto &viz_data = finalResults.at(outputPortKey)
                                   ->getParam<cv::Mat>("visualized_image");
        ASSERT_FALSE(viz_data.empty());
      });

  pipeline.setPipelineErrorCallback(
      [&](const std::string &errorMsg, const std::string &nodeName) {
        errorOccurred = true;
        lastErrorMsg = "Pipeline error in node '" + nodeName + "': " + errorMsg;
        std::cerr << lastErrorMsg << std::endl;
      });

  ASSERT_TRUE(pipeline.start());
  ASSERT_EQ(pipeline.getState(), ai_pipe::PipelineState::RUNNING);

  // Use feedDataAndGetResultFuture for synchronous-like testing
  auto retFuture = pipeline.feedDataAndGetResultFuture(inputs);

  // 10-second timeout
  std::future_status status = retFuture.wait_for(std::chrono::seconds(10));
  ASSERT_EQ(status, std::future_status::ready)
      << "Pipeline execution timed out.";

  // Will rethrow exceptions from pipeline if any
  bool success = retFuture.get();
  ASSERT_TRUE(success);

  ASSERT_TRUE(resultReceived) << "Pipeline result callback was not invoked.";
  ASSERT_FALSE(errorOccurred)
      << "Pipeline error callback was invoked: " << lastErrorMsg;

  ASSERT_TRUE(std::filesystem::exists(expectedOutputPathStr))
      << "Expected output image file was not created: "
      << expectedOutputPathStr;

  ASSERT_TRUE(pipeline.stop());
  ASSERT_EQ(pipeline.getState(), ai_pipe::PipelineState::STOPPED);
  pipeline.reset();
  ASSERT_EQ(pipeline.getState(), ai_pipe::PipelineState::IDLE);

  //   if (std::filesystem::exists(expectedOutputPathStr)) {
  //     std::filesystem::remove(expectedOutputPathStr);
  //   }
}
} // namespace testing_demo_pipeline