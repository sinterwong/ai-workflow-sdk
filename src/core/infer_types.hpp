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

#include <opencv2/opencv.hpp>
#include <string>
#include <variant>

namespace android_infer::infer {

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

  // release error
  TERMINATE_FAILED = 300
};

enum class DeviceType { CPU = 0, GPU = 1 };

struct FrameInput {
  cv::Mat image;
  float alpha;
  float beta;
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

class AlgoInput {
public:
  using Params = std::variant<std::monostate, FrameInput>;

  template <typename T> void setParams(T params) {
    params_ = std::move(params);
  }

  template <typename Func> void visitParams(Func &&func) {
    std::visit([&](auto &&params) { std::forward<Func>(func)(params); },
               params_);
  }

  template <typename T> T *getParams() { return std::get_if<T>(&params_); }

private:
  Params params_;
};

struct DetRet {
  cv::Rect rect;
  float score;
  int label;
};

struct ClsRet {
  float score;
  int label;
};

class AlgoOutput {
public:
  using Params = std::variant<std::monostate, ClsRet, DetRet>;

  template <typename T> void setParams(T params) {
    params_ = std::move(params);
  }

  template <typename Func> void visitParams(Func &&func) {
    std::visit([&](auto &&params) { std::forward<Func>(func)(params); },
               params_);
  }

  template <typename T> T *getParams() { return std::get_if<T>(&params_); }

private:
  Params params_;
};

struct AlgoBase {
  std::string name;
  std::string modelPath;
};
} // namespace android_infer::infer
#endif
