/**
 * @file vision_util.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __INFERENCE_VISION_UTILS_HPP_
#define __INFERENCE_VISION_UTILS_HPP_

#include "infer_types.hpp"

namespace infer::utils {

std::pair<float, float> scaleRatio(Shape const &originShape,
                                   Shape const &inputShape, bool isScale) {
  float rw, rh;
  if (isScale) {
    rw = std::min(static_cast<float>(inputShape.w) / originShape.w,
                  static_cast<float>(inputShape.h) / originShape.h);
    rh = rw;
  } else {
    rw = static_cast<float>(inputShape.w) / originShape.w;
    rh = static_cast<float>(inputShape.h) / originShape.h;
  }
  return std::make_pair(rw, rh);
}

float calculateIoU(const BBox &bbox1, const BBox &bbox2) {
  float x1 = std::max(bbox1.rect.x, bbox2.rect.x);
  float y1 = std::max(bbox1.rect.y, bbox2.rect.y);
  float x2 = std::min(bbox1.rect.x + bbox1.rect.width,
                      bbox2.rect.x + bbox2.rect.width);
  float y2 = std::min(bbox1.rect.y + bbox1.rect.height,
                      bbox2.rect.y + bbox2.rect.height);

  if (x2 < x1 || y2 < y1) {
    return 0.0f;
  }

  float intersection = (x2 - x1) * (y2 - y1);
  float area1 = bbox1.rect.width * bbox1.rect.height;
  float area2 = bbox2.rect.width * bbox2.rect.height;
  float unionArea = area1 + area2 - intersection;

  return intersection / unionArea;
}

std::vector<BBox> NMS(const std::vector<BBox> &results, float nmsThre,
                      float confThre) {
  std::unordered_map<int, std::vector<BBox>> classResults;
  for (const auto &result : results) {
    classResults[result.label].push_back(result);
  }

  std::vector<BBox> nmsResults;
  for (auto &pair : classResults) {
    auto &classResult = pair.second;

    std::vector<cv::Rect> boxes;
    std::vector<float> scores;
    std::vector<int> indices;

    for (const auto &result : classResult) {
      boxes.push_back(result.rect);
      scores.push_back(result.score);
    }

    cv::dnn::NMSBoxes(boxes, scores, confThre, nmsThre, indices);

    for (int idx : indices) {
      nmsResults.push_back(classResult[idx]);
    }
  }

  return nmsResults;
}

Shape escaleResizeWithPad(const cv::Mat &src, cv::Mat &dst, int targetWidth,
                          int targetHeight, const cv::Scalar &pad) {
  float scale = std::min(static_cast<float>(targetWidth) / src.cols,
                         static_cast<float>(targetHeight) / src.rows);
  cv::Size newSize(static_cast<int>(src.cols * scale),
                   static_cast<int>(src.rows * scale));
  cv::resize(src, dst, newSize, 0, 0, cv::INTER_LINEAR);
  Shape padRet;
  padRet.h = (targetHeight - dst.rows) / 2;
  padRet.w = (targetWidth - dst.cols) / 2;
  int bottomPad = targetHeight - dst.rows - padRet.h;
  int rightPad = targetWidth - dst.cols - padRet.w;
  cv::copyMakeBorder(dst, dst, padRet.h, bottomPad, padRet.w, rightPad,
                     cv::BORDER_CONSTANT, pad);

  return padRet;
}

} // namespace infer::utils
#endif