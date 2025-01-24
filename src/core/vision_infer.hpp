/**
 * @file vision_infer.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __INFERENCE_VISION_INFER_HPP__
#define __INFERENCE_VISION_INFER_HPP__

#include "frame_infer.hpp"
#include "infer.hpp"
#include "infer_types.hpp"
#include "infer_wrapper.hpp"
#include "logger/logger.hpp"
#include <memory>

namespace infer::dnn {
template <typename Vision> class VisionInfer {
public:
  VisionInfer(const FrameInferParam &param, const AlgoPostprocParams &postproc)
      : inferParams(param), postprocParams(postproc){};

  InferErrorCode initialize() {
    engine =
        std::make_shared<InferSafeWrapper<FrameInference, FrameInferParam>>(
            inferParams);
    vision = std::make_shared<Vision>(postprocParams);
    return engine->initialize();
  }

  InferErrorCode infer(AlgoInput &input, AlgoOutput &output) {
    if (!engine->tryAcquire()) {
      LOGGER_ERROR("engine is busy");
      return InferErrorCode::INFER_FAILED;
    }

    ModelOutput modelOutput;
    auto ret = engine->getEngine()->infer(input, modelOutput);
    if (ret != InferErrorCode::SUCCESS) {
      engine->release();
      return ret;
    }

    auto frameInput = input.getParams<FrameInput>();
    if (frameInput == nullptr) {
      LOGGER_ERROR("frameInput is nullptr");
      engine->release();
      return InferErrorCode::INFER_FAILED;
    }

    bool result = vision->processOutput(modelOutput, frameInput->args, output);
    engine->release();
    return result ? InferErrorCode::SUCCESS : InferErrorCode::INFER_FAILED;
  }

  InferErrorCode terminate() { return engine->getEngine()->terminate(); }

  const ModelInfo &getModelInfo() {
    return engine->getEngine()->getModelInfo();
  }

private:
  FrameInferParam inferParams;
  AlgoPostprocParams postprocParams;

  std::shared_ptr<InferSafeWrapper<FrameInference, FrameInferParam>> engine;
  std::shared_ptr<Vision> vision;
};
} // namespace infer::dnn
#endif
