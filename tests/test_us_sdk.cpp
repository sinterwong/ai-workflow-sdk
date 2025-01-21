#include "ultra_sound/ultra_sound_sdk.hpp"

#include "api/us_types.h"
#include "gtest/gtest.h"
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>

using namespace ultra_sound;

namespace fs = std::filesystem;

class UltraSoundSDKTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}

  fs::path dataDir = fs::path("data");

  std::string imagePath = (dataDir / "thy_image.png").string();
  std::string videoPath = (dataDir / "video.mp4").string();
};

TEST_F(UltraSoundSDKTest, Normal) {
  UltraSoundSDK sdk;
  SDKConfig config;
  config.numWorkers = 1;
  config.modelPath = "models/rtmdet.ncnn";
  config.inputWidth = 640;
  config.inputHeight = 640;
  ASSERT_EQ(sdk.initialize(config), ErrorCode::SUCCESS);

  cv::Mat imageBGR = cv::imread(imagePath);
  cv::Mat imageRGB;
  cv::cvtColor(imageBGR, imageRGB, cv::COLOR_BGR2RGB);
  ASSERT_FALSE(imageBGR.empty());

  InputPacket input;
  input.uuid = "test_uuid";
  input.frameIndex = 0;
  cv::imencode(".png", imageRGB, input.imageData);
  input.width = 640;
  input.height = 640;
  input.timestamp = 1678886400000000;

  ASSERT_EQ(sdk.pushInput(input), ErrorCode::SUCCESS);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  OutputPacket output;
  ASSERT_EQ(sdk.tryGetNext(output), ErrorCode::SUCCESS);
  // visualize result
  for (const auto &bbox : output.bboxes) {
    cv::Rect rect(bbox.rect[0], bbox.rect[1], bbox.rect[2], bbox.rect[3]);
    cv::rectangle(imageBGR, rect, cv::Scalar(0, 255, 0), 2);
    std::stringstream ss;
    ss << bbox.label << ":" << bbox.score;
    cv::putText(imageBGR, ss.str(), rect.tl(), cv::FONT_HERSHEY_SIMPLEX, 1,
                cv::Scalar(0, 0, 255), 2);
  }
  cv::imwrite("vis_result.png", imageBGR);

  ASSERT_EQ(sdk.terminate(), ErrorCode::SUCCESS);
}

TEST_F(UltraSoundSDKTest, GetVersion) {
  ASSERT_EQ(UltraSoundSDK::getVersion(), "1.0.0");
}
