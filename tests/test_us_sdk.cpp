#include "ultra_sound/ultra_sound_sdk.hpp"

#include "api/us_sdk.h"
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
  void SetUp() override {
    // sdk config
    config.numWorkers = 1;
    config.modelPath = "models/rtmdet.ncnn";
    config.algoConfPath = "../conf/pipe/thy_config.yml";
  }
  void TearDown() override {}

  fs::path dataDir = fs::path("data");

  std::string imagePath = (dataDir / "thy_image.png").string();
  std::string videoPath = (dataDir / "video.mp4").string();

  SDKConfig config;
};

TEST_F(UltraSoundSDKTest, CppAPI) {
  ASSERT_EQ(UltraSoundSDK::getVersion(), "1.0.0");

  UltraSoundSDK sdk;
  ASSERT_EQ(sdk.initialize(config), ErrorCode::SUCCESS);

  cv::Mat imageBGR = cv::imread(imagePath);
  cv::Mat imageRGB;
  cv::cvtColor(imageBGR, imageRGB, cv::COLOR_BGR2RGB);
  ASSERT_FALSE(imageBGR.empty());

  InputPacket input;
  ASSERT_EQ(sdk.processFrame(input), ErrorCode::SUCCESS);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  OutputPacket output;
  ASSERT_EQ(sdk.tryGetNextLesion(output), ErrorCode::SUCCESS);
  // visualize result
  for (const auto &lesion : output.lesions) {
    cv::Rect rect(lesion.smoothBox.x, lesion.smoothBox.y, lesion.smoothBox.w,
                  lesion.smoothBox.h);
    cv::rectangle(imageBGR, rect, cv::Scalar(0, 255, 0), 2);
    std::stringstream ss;
    ss << lesion.label << ":" << lesion.score;
    cv::putText(imageBGR, ss.str(), rect.tl(), cv::FONT_HERSHEY_SIMPLEX, 1,
                cv::Scalar(0, 0, 255), 2);
  }
  cv::imwrite("vis_cpp_api.png", imageBGR);

  ASSERT_EQ(sdk.terminate(), ErrorCode::SUCCESS);
}

TEST_F(UltraSoundSDKTest, C_API) {}
