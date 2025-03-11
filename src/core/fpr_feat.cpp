/**
 * @file fpr_feat.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-02-15
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "fpr_feat.hpp"
#include "infer_types.hpp"
#include "logger/logger.hpp"
#include <utility>

namespace infer::dnn::vision {
bool FprFeature::processOutput(const ModelOutput &modelOutput,
                               const FramePreprocessArg &args,
                               AlgoOutput &algoOutput) {
  if (modelOutput.outputs.empty()) {
    LOG_ERRORS << "modelOutput.outputs is empty";
    return false;
  }

  const auto &outputShapes = modelOutput.outputShapes;
  const auto &outputs = modelOutput.outputs;

  // just one output
  auto output = outputs.at(0);
  auto outputShape = outputShapes.at(0);

  FeatureRet ret;
  ret.feature = std::move(output);
  ret.featSize = outputShape.at(outputShape.size() - 1);
  algoOutput.setParams(ret);
  return true;
}
} // namespace infer::dnn::vision
