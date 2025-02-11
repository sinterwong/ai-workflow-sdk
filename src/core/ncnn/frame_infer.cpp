/**
 * @file dnn_infer.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "frame_infer.hpp"
#include "infer_types.hpp"
#include "logger/logger.hpp"
#include "vision_util.hpp"
#include <opencv2/core/mat.hpp>

namespace infer::dnn {

std::vector<std::pair<std::string, ncnn::Mat>>
FrameInference::preprocess(AlgoInput &input) const {

  std::vector<std::pair<std::string, ncnn::Mat>> ret;
  // Get input parameters
  auto *frameInput = input.getParams<FrameInput>();
  if (!frameInput || frameInput->images.empty()) {
    LOGGER_ERROR("Invalid input parameters");
    throw std::runtime_error("Invalid input parameters");
  }

  int inputWidth = params->inputShape.w;
  int inputHeight = params->inputShape.h;
  auto &args = frameInput->args;
  const std::vector<cv::Mat> &images = frameInput->images;

  // check each image
  for (size_t i = 1; i < images.size(); ++i) {
    if (images[i].size() != images[0].size()) {
      LOGGER_ERROR("Images size not match");
      throw std::runtime_error("Images size not match");
    }
  }

  ncnn::Mat::PixelType pixelType = images.at(0).channels() == 1
                                       ? ncnn::Mat::PIXEL_GRAY
                                       : ncnn::Mat::PIXEL_RGB;

  std::vector<ncnn::Mat> batchMats(images.size());
#pragma omp parallel for
  for (int i = 0; i < (int)images.size(); i++) {
    const cv::Mat &img = images[i];
    cv::Mat croppedImage;
    if (args.roi.area() > 0)
      croppedImage = img(args.roi).clone();
    else
      croppedImage = img;

    ncnn::Mat in;
    if (args.isEqualScale) {
      cv::Mat resizedImage;
      auto padRet = utils::escaleResizeWithPad(
          croppedImage, resizedImage, inputHeight, inputHeight, args.pad);
#pragma omp critical
      {
        // 所有线程写入的值应该一致
        args.topPad = padRet.h;
        args.leftPad = padRet.w;
      }
      in = ncnn::Mat::from_pixels(resizedImage.data, pixelType,
                                  resizedImage.cols, resizedImage.rows);
    } else {
      in = ncnn::Mat::from_pixels_resize(croppedImage.data, pixelType,
                                         croppedImage.cols, croppedImage.rows,
                                         inputWidth, inputHeight);
    }

    std::vector<float> normVals;
    std::transform(args.normVals.begin(), args.normVals.end(),
                   std::back_inserter(normVals),
                   [](float val) { return 1.0f / val; });
    in.substract_mean_normalize(args.meanVals.data(), normVals.data());

    batchMats[i] = in;
  }

  if (!batchMats.empty()) {
    int batch = batchMats.size();
    int w = batchMats[0].w;
    int h = batchMats[0].h;
    int c = batchMats[0].c;
    ncnn::Mat in_batch(w, h, batch, c);
    int elems_per_image = w * h * c;
    for (int i = 0; i < batch; i++) {
      memcpy((float *)in_batch.data + i * elems_per_image, batchMats[i].data,
             elems_per_image * sizeof(float));
    }
    ret.emplace_back(inputNames[0], in_batch);
  }
  return ret;
}
}; // namespace infer::dnn
