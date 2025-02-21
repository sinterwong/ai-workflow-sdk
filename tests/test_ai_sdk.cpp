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
    config.modelRoot = "models/yolov11n.ncnn";
    config.algoConfPath = "../conf/pipe/config.yml";
    config.logPath = "./log";
  }
  void TearDown() override {}

  fs::path dataDir = fs::path("data/yolov11");

  std::string imagePath = (dataDir / "image.png").string();

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
