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
#include "utils/type_safe_factory.hpp"
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
  INIT_MEMORY_ALLOC_FAILED = 104,
  INIT_DECRYPTION_FAILED = 105,
  NOT_INITIALIZED = 106,

  // infer error
  INFER_FAILED = 200,
  INFER_INPUT_ERROR = 201,
  INFER_OUTPUT_ERROR = 202,
  INFER_DEVICE_ERROR = 203,
  INFER_PREPROCESS_FAILED = 204,
  INFER_MEMORY_ERROR = 205,
  INFER_SET_INPUT_FAILED = 206,
  INFER_EXTRACT_FAILED = 207,
  INFER_UNSUPPORTED_OUTPUT_TYPE = 208,

  // release error
  TERMINATE_FAILED = 300,

  // algo manager error
  ALGO_NOT_FOUND = 400,
  ALGO_REGISTER_FAILED = 401,
  ALGO_UNREGISTER_FAILED = 402,
  ALGO_INFER_FAILED = 403,
};

enum class DeviceType { CPU = 0, GPU = 1 };

enum class DataType { FLOAT32 = 0, FLOAT16 = 1, INT8 = 2 };

struct TypedBuffer {
  DataType dataType;
  std::vector<uint8_t> data; // raw data
  size_t elementCount;

  template <typename T> const T *getTypedPtr() const {
    return reinterpret_cast<const T *>(data.data());
  }

  size_t getElementCount() const {
    size_t elemSize = getElementSize(dataType);
    return data.size() / elemSize;
  }

  static size_t getElementSize(DataType type) {
    switch (type) {
    case DataType::FLOAT32:
      return sizeof(float);
    case DataType::FLOAT16:
      return sizeof(uint16_t);
    case DataType::INT8:
      return sizeof(int8_t);
    default:
      return 0;
    }
  }
};

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
  std::map<std::string, TypedBuffer> outputs;
  std::map<std::string, std::vector<int>> outputShapes;
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

using AlgoInferParams =
    utils::ParamCenter<std::variant<std::monostate, FrameInferParam>>;

using AlgoConstructParams = ::utils::DataPacket;

} // namespace infer
#endif
