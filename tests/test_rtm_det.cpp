#include "core/frame_infer.hpp"
#include "infer_types.hpp"
#include "rtm_det.hpp"
#include "gtest/gtest.h"
#include <filesystem>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

namespace testing_rtm_det {
namespace fs = std::filesystem;

using namespace infer;
using namespace infer::dnn;
class RTMDetInferenceTest : public ::testing::Test {
protected:
  void SetUp() override {

    AnchorDetParams anchorDetParams;
    anchorDetParams.condThre = 0.2f;
    anchorDetParams.nmsThre = 0.45f;
    anchorDetParams.inputShape = {640, 640};
    params.setParams(anchorDetParams);

    rtmDet = std::make_shared<vision::RTMDet>(params);
    ASSERT_NE(rtmDet, nullptr);
  }
  void TearDown() override {}

  fs::path dataDir = fs::path("data");

  std::string imagePath = (dataDir / "rtmdet/image.jpg").string();

  AlgoPostprocParams params;

  std::shared_ptr<vision::VisionBase> rtmDet;
};

TEST_F(RTMDetInferenceTest, Normal) {
  FrameInferParam rtmParam;
  rtmParam.name = "test-rtmdet";

#ifdef USE_NCNN
  rtmParam.modelPath = "models/rtmdet.ncnn";
#else
  rtmParam.modelPath = "models/rtmdet-fp16.onnx";
#endif
  rtmParam.inputShape = {640, 640};
  rtmParam.deviceType = DeviceType::CPU;
  rtmParam.dataType = DataType::FLOAT16;

  std::shared_ptr<Inference> engine =
      std::make_shared<FrameInference>(rtmParam);
  ASSERT_NE(engine, nullptr);
  ASSERT_EQ(engine->initialize(), InferErrorCode::SUCCESS);

  cv::Mat imageRGB = cv::imread(imagePath);
  ASSERT_FALSE(imageRGB.empty());

  FrameInput frameInput;
  frameInput.image = imageRGB;
  frameInput.args.originShape = {imageRGB.cols, imageRGB.rows};
  frameInput.args.roi = {0, 0, imageRGB.cols, imageRGB.rows};
  // frameInput.args.roi = {62, 109, 886, 500};
  frameInput.args.isEqualScale = true;
  frameInput.args.pad = {0, 0, 0};
  frameInput.args.meanVals = {120, 120, 120};
  frameInput.args.normVals = {60.f, 60.f, 60.f};

  AlgoInput algoInput;
  algoInput.setParams(frameInput);

  ModelOutput modelOutput;
  ASSERT_EQ(engine->infer(algoInput, modelOutput), InferErrorCode::SUCCESS);

  auto frameInputPtr = algoInput.getParams<FrameInput>();
  AlgoOutput algoOutput;
  ASSERT_TRUE(
      rtmDet->processOutput(modelOutput, frameInputPtr->args, algoOutput));

  auto *detRet = algoOutput.getParams<DetRet>();

  cv::Mat visImage = imageRGB.clone();
  for (const auto &bbox : detRet->bboxes) {
    cv::rectangle(visImage, bbox.rect, cv::Scalar(0, 255, 0), 2);
    std::stringstream ss;
    ss << bbox.label << ":" << bbox.score;
    cv::putText(visImage, ss.str(), bbox.rect.tl(), cv::FONT_HERSHEY_SIMPLEX, 1,
                cv::Scalar(0, 0, 255), 2);
  }
  cv::imwrite("vis_rtmdet.png", visImage);
  engine->terminate();
}
} // namespace testing_rtm_det