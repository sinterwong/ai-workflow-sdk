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
      LOG_ERRORS << "engine is busy";
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
      LOG_ERRORS << "frameInput is nullptr";
      engine->release();
      return InferErrorCode::INFER_FAILED;
    }
    engine->release();

    // post cost time
    auto startPost = std::chrono::steady_clock::now();
    bool result = vision->processOutput(modelOutput, frameInput->args, output);
    auto endPost = std::chrono::steady_clock::now();
    auto durationPost = std::chrono::duration_cast<std::chrono::milliseconds>(
        endPost - startPost);
    LOG_INFOS << inferParams.name << " postprocess cost "
              << durationPost.count() << " ms";
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
