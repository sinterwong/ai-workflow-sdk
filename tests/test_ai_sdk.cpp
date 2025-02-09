#include "ai_sdk/ai_sdk.hpp"
#include "api/ai_sdk.h"
#include "api/ai_types.h"
#include "gtest/gtest.h"
#include <filesystem>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <string>

namespace fs = std::filesystem;

using namespace android_infer;

class AndroidSDKTest : public ::testing::Test {
protected:
  void SetUp() override {
    // sdk config
    config.numWorkers = 1;
    config.modelRoot = "models/rtmdet.ncnn";
    config.algoConfPath = "../conf/pipe/thy_config.yml";
    config.logPath = "./log";
  }
  void TearDown() override {}

  fs::path dataDir = fs::path("data");

  std::string imagePath = (dataDir / "image.png").string();
  std::string videoPath = (dataDir / "video.mp4").string();

  SDKConfig config;
};

TEST_F(AndroidSDKTest, CppAPI) {
  ASSERT_EQ(AndroidSDK::getVersion(), "1.0.0");

  AndroidSDK sdk;
  ASSERT_EQ(sdk.initialize(config), ErrorCode::SUCCESS);

  cv::Mat imageBGR = cv::imread(imagePath);
  cv::Mat imageRGB;
  cv::cvtColor(imageBGR, imageRGB, cv::COLOR_BGR2RGB);
  ASSERT_FALSE(imageBGR.empty());

  InputPacket input;

  ImageData imageData;
  cv::imencode(".png", imageRGB, imageData.frameData);
  imageData.frameIndex = 1;
  input.frame = imageData;

  ASSERT_EQ(sdk.pushInput(input), ErrorCode::SUCCESS);

  OutputPacket output;
  ASSERT_EQ(sdk.tryGetNextOutput(output), ErrorCode::SUCCESS);
  // TODO: visualize result

  ASSERT_EQ(sdk.terminate(), ErrorCode::SUCCESS);
}

TEST_F(AndroidSDKTest, C_API) {
  std::string version = AndroidSDK_GetVersion();
  ASSERT_EQ(version, "1.0.0");

  AndroidSDKHandle handle = AndroidSDK_Create();
  ASSERT_NE(handle, nullptr);

  SDKConfig config_c;
  config_c.numWorkers = 1;
  config_c.modelRoot = "models/rtmdet.ncnn";
  config_c.algoConfPath = "../conf/pipe/thy_config.yml";
  config_c.logPath = "./log";
  ASSERT_EQ(AndroidSDK_Initialize(handle, &config_c), ErrorCode::SUCCESS);

  cv::Mat imageBGR = cv::imread(imagePath);
  cv::Mat imageRGB;
  cv::cvtColor(imageBGR, imageRGB, cv::COLOR_BGR2RGB);
  ASSERT_FALSE(imageBGR.empty());

  InputPacket input_c;

  ImageData imageData_c;
  cv::imencode(".png", imageRGB, imageData_c.frameData);
  imageData_c.frameIndex = 1;
  input_c.frame = imageData_c;

  ASSERT_EQ(AndroidSDK_PushInput(handle, &input_c), ErrorCode::SUCCESS);

  OutputPacket output_c;
  ASSERT_EQ(AndroidSDK_TryGetNextOutput(handle, &output_c), ErrorCode::SUCCESS);
  // TODO: visualize result

  ASSERT_EQ(AndroidSDK_Terminate(handle), ErrorCode::SUCCESS);
  AndroidSDK_Destroy(handle);
}
