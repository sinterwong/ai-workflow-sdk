/**
 * @file fpr_cls.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-02-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "fpr_cls.hpp"
#include "infer_types.hpp"
#include "logger/logger.hpp"

namespace infer::dnn::vision {
bool FprCls::processOutput(const ModelOutput &modelOutput,
                           const FramePreprocessArg &args,
                           AlgoOutput &algoOutput) {
  if (modelOutput.outputs.empty()) {
    LOGGER_ERROR("modelOutput.outputs is empty");
    return false;
  }

  const auto &outputShapes = modelOutput.outputShapes;
  const auto &outputs = modelOutput.outputs;

  auto pScores = outputs.at(0);
  auto pBirads = outputs.at(1);

  std::vector<int> pScoresShape = outputShapes.at(0);
  int numClasses = pScoresShape.at(pScoresShape.size() - 1);

  std::vector<int> pBiradsShape = outputShapes.at(1);
  int numBirads = pBiradsShape.at(pBiradsShape.size() - 1);

  cv::Mat scores(1, numClasses, CV_32F, (void *)pScores.data());
  cv::Mat birads(1, numBirads, CV_32F, (void *)pBirads.data());

  cv::Point classIdPoint;
  double score;
  cv::minMaxLoc(scores, nullptr, &score, nullptr, &classIdPoint);

  cv::Point biradsIdPoint;
  double biradsScore;
  cv::minMaxLoc(birads, nullptr, &biradsScore, nullptr, &biradsIdPoint);

  FprClsRet fprRet;
  fprRet.score = score;
  fprRet.label = classIdPoint.x;
  fprRet.scoreProbs = pScores;
  fprRet.birad = biradsIdPoint.x;
  algoOutput.setParams(fprRet);
  return true;
}
} // namespace infer::dnn::vision
