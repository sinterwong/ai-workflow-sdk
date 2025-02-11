#include "infer_types.hpp"
#include "vision_infer.hpp"
#include "yolo_det.hpp"
#include "gtest/gtest.h"
#include <filesystem>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;

using namespace infer;
using namespace infer::dnn;
class VisionInferTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}

  fs::path dataDir = fs::path("data");

  std::string imagePath = (dataDir / "image.png").string();
};

TEST_F(VisionInferTest, Normal) {
  AlgoPostprocParams params;
  AnchorDetParams anchorDetParams;
  anchorDetParams.condThre = 0.5f;
  anchorDetParams.nmsThre = 0.45f;
  anchorDetParams.inputShape = {640, 640};
  params.setParams(anchorDetParams);

  FrameInferParam yoloParam;
  yoloParam.name = "test-yolodet";
#ifdef USE_NCNN
  yoloParam.modelPath = "models/yolov11n.ncnn";
#else
  yoloParam.modelPath = "models/yolov11n.onnx";
#endif
  yoloParam.inputShape = {640, 640};
  yoloParam.deviceType = DeviceType::CPU;

  std::shared_ptr<VisionInfer<vision::Yolov11Det>> engine =
      std::make_shared<VisionInfer<vision::Yolov11Det>>(yoloParam, params);
  ASSERT_NE(engine, nullptr);
  std::shared_ptr<vision::Vision> yoloDet =
      std::make_shared<vision::Yolov11Det>(params);
  ASSERT_NE(yoloDet, nullptr);

  cv::Mat image = cv::imread(imagePath);
  cv::Mat imageRGB;
  cv::cvtColor(image, imageRGB, cv::COLOR_BGR2RGB);
  ASSERT_FALSE(image.empty());

  ASSERT_EQ(engine->initialize(), InferErrorCode::SUCCESS);

  FrameInput frameInput;
  frameInput.images = {imageRGB};
  frameInput.args.originShape = {imageRGB.cols, imageRGB.rows};
  frameInput.args.roi = {0, 0, imageRGB.cols, imageRGB.rows};
  frameInput.args.isEqualScale = true;
  frameInput.args.pad = {0, 0, 0};
  frameInput.args.meanVals = {0, 0, 0};
  frameInput.args.normVals = {255.f, 255.f, 255.f};

  AlgoInput algoInput;
  algoInput.setParams(frameInput);

  AlgoOutput algoOutput;
  ASSERT_EQ(engine->infer(algoInput, algoOutput), InferErrorCode::SUCCESS);

  auto *detRet = algoOutput.getParams<DetRet>();
  ASSERT_NE(detRet, nullptr);
  ASSERT_GT(detRet->bboxes.size(), 0);

  cv::Mat visImage = image.clone();
  for (const auto &bbox : detRet->bboxes) {
    cv::rectangle(visImage, bbox.rect, cv::Scalar(0, 255, 0), 2);
    std::stringstream ss;
    ss << bbox.label << ":" << bbox.score;
    cv::putText(visImage, ss.str(), bbox.rect.tl(), cv::FONT_HERSHEY_SIMPLEX, 1,
                cv::Scalar(0, 0, 255), 2);
  }
  cv::imwrite("vis_vision_infer.png", visImage);

  engine->terminate();
}
