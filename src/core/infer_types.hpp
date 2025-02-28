/**
 * @file types.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __INFERENCE_TYPES_HPP__
#define __INFERENCE_TYPES_HPP__

#include "utils/param_center.hpp"
#include <opencv2/opencv.hpp>
#include <string>

namespace infer {

enum class InferErrorCode : int32_t {
  SUCCESS = 0,

  // init error
  INIT_FAILED = 100,
  INIT_CONFIG_FAILED = 101,
  INIT_MODEL_LOAD_FAILED = 102,
  INIT_DEVICE_FAILED = 103,

  // infer error
  INFER_FAILED = 200,
  INFER_INPUT_ERROR = 201,
  INFER_OUTPUT_ERROR = 202,
  INFER_DEVICE_ERROR = 203,
  PREPROCESS_FAILED = 204,
  MEMORY_ERROR = 205,

  // release error
  TERMINATE_FAILED = 300
};

enum class DeviceType { CPU = 0, GPU = 1 };

enum class DataType { FLOAT32 = 0, FLOAT16 = 1, INT8 = 2 };

struct Shape {
  int w;
  int h;
};

struct FramePreprocessArg {
  cv::Rect roi;
  std::vector<float> meanVals;
  std::vector<float> normVals;
  Shape originShape;

  bool needResize = true;
  bool isEqualScale;
  cv::Scalar pad = {0, 0, 0};
  int topPad = 0;
  int leftPad = 0;
};

struct FrameInput {
  cv::Mat image;
  FramePreprocessArg args;
};

struct ModelInfo {
  std::string name;

  struct InputInfo {
    std::string name;
    std::vector<int64_t> shape;
  };

  struct OutputInfo {
    std::string name;
    std::vector<int64_t> shape;
  };

  std::vector<InputInfo> inputs;
  std::vector<OutputInfo> outputs;
};

// Algo input
using AlgoInput = utils::ParamCenter<std::variant<std::monostate, FrameInput>>;

// Model output(after infering, before postprocess)
struct ModelOutput {
  std::vector<std::vector<float>> outputs;
  std::vector<std::vector<int>> outputShapes;
};

// Algo output
struct BBox {
  cv::Rect rect;
  float score;
  int label;
};

struct ClsRet {
  float score;
  int label;
};

struct FeatureRet {
  std::vector<float> feature;
  int featSize;
};

struct FprClsRet {
  float score;
  int label;
  int birad;
  std::vector<float> scoreProbs;
};

struct DetRet {
  std::vector<BBox> bboxes;
};

// Algo output
using AlgoOutput = utils::ParamCenter<
    std::variant<std::monostate, ClsRet, DetRet, FprClsRet, FeatureRet>>;

// Post-process Params
struct AnchorDetParams {
  float condThre;
  float nmsThre;
  Shape inputShape;
};

using AlgoPostprocParams =
    utils::ParamCenter<std::variant<std::monostate, AnchorDetParams>>;

// Infer Params
struct InferParamBase {
  std::string name;
  std::string modelPath;
  bool needDecrypt = false;
  std::string decryptkeyStr;
  DeviceType deviceType;
  DataType dataType;
};

struct FrameInferParam : public InferParamBase {
  Shape inputShape;
};
} // namespace infer
#endif
