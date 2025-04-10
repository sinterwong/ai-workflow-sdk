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
#include "utils/type_safe_factory.hpp"
#include "vision.hpp"
#include <memory>

namespace infer::dnn::vision {
class VisionInfer {
public:
  VisionInfer(const std::string &moduleName, const FrameInferParam &param,
              const AlgoPostprocParams &postproc)
      : moduleName(moduleName), inferParams(param), postprocParams(postproc){};

  InferErrorCode initialize() {
    engine =
        std::make_shared<InferSafeWrapper<FrameInference, FrameInferParam>>(
            inferParams);

    utils::ConstructorParams params = {{"params", postprocParams}};
    try {
      vision =
          utils::Factory<VisionBase>::instance().create(moduleName, params);
    } catch (const std::exception &e) {
      LOG_ERRORS << "Failed to create vision module: " << e.what();
      return InferErrorCode::INIT_FAILED;
    }

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

  const ModelInfo &getModelInfo() const noexcept {
    if (engine == nullptr) {
      LOG_ERRORS << "Please initialize first";
      static ModelInfo modelInfo;
      return modelInfo;
    }
    return engine->getEngine()->getModelInfo();
  }

  const std::string &getModuleName() const noexcept { return moduleName; };

private:
  std::string moduleName;
  FrameInferParam inferParams;
  AlgoPostprocParams postprocParams;

  std::shared_ptr<InferSafeWrapper<FrameInference, FrameInferParam>> engine;
  std::shared_ptr<VisionBase> vision;
};
} // namespace infer::dnn::vision
#endif
