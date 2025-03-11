/**
 * @file softmax_cls.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "softmax_cls.hpp"
#include "infer_types.hpp"
#include "logger/logger.hpp"

namespace infer::dnn::vision {
bool SoftmaxCls::processOutput(const ModelOutput &modelOutput,
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

  std::vector<int> outputShape = outputShapes.at(0);
  int numClasses = outputShape.at(outputShape.size() - 1);

  cv::Mat scores(1, numClasses, CV_32F, (void *)output.data());
  cv::Point classIdPoint;
  double score;
  cv::minMaxLoc(scores, nullptr, &score, nullptr, &classIdPoint);

  ClsRet clsRet;
  clsRet.score = score;
  clsRet.label = classIdPoint.x;
  algoOutput.setParams(clsRet);
  return true;
}
} // namespace infer::dnn::vision
