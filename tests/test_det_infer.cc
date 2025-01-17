#include "core/infer_wrapper.hpp"
#include "core/rtm_det.hpp"
#include <gtest/gtest.h>

class DetInferTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(DetInferTest, Normal) {

  android_infer::infer::AlgoBase base;
  base.name = "rtmdet";
  base.modelPath = "/path/to/your/model";

  android_infer::infer::InferSafeWrapper<
      android_infer::infer::dnn::RTMDetInference>
      inferWrapper(base);
  ASSERT_EQ(inferWrapper.initialize(),
            android_infer::infer::InferErrorCode::SUCCESS);

  android_infer::infer::AlgoInput input;
  cv::Mat image = cv::imread("/path/to/your/image");
  if (image.empty()) {
    FAIL() << "Could not load image";
  }
  input.setParams(android_infer::infer::FrameInput{image, 1.0f, 0.0f});

  android_infer::infer::AlgoOutput output;
  ASSERT_TRUE(inferWrapper.tryAcquire());
  ASSERT_EQ(inferWrapper.get()->infer(input, output),
            android_infer::infer::InferErrorCode::SUCCESS);
  inferWrapper.release();

  auto *detRet = output.getParams<android_infer::infer::DetRet>();
  ASSERT_NE(detRet, nullptr);
}
