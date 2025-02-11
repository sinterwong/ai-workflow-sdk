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

namespace infer::dnn {

std::vector<std::vector<float>>
FrameInference::preprocess(AlgoInput &input) const {

  std::vector<std::vector<float>> ret;
  // Get input parameters
  auto *frameInput = input.getParams<FrameInput>();
  if (!frameInput || frameInput->images.empty()) {
    LOGGER_ERROR("Invalid input parameters");
    throw std::runtime_error("Invalid input parameters");
  }

  if (inputNames.size() != 1) {
    LOGGER_ERROR("Input node number is not 1");
    throw std::runtime_error("Input node number is not 1");
  }

  const auto &inputShape = inputShapes.at(0);

  int inputWidth = inputShape.at(inputShape.size() - 1);
  int inputHeight = inputShape.at(inputShape.size() - 2);
  int inputChannels = inputShape.at(inputShape.size() - 3);
  int inputBatch = inputShape.at(inputShape.size() - 4);

  if (inputBatch != frameInput->images.size()) {
    LOGGER_ERROR("Input batch size {} not match image size {}", inputBatch,
                 frameInput->images.size());
    throw std::runtime_error("Input batch size not match image size");
  }

  const std::vector<cv::Mat> &images = frameInput->images;
  auto &args = frameInput->args;

  // check each image
  for (size_t i = 1; i < images.size(); ++i) {
    if (images[i].size() != images[0].size()) {
      LOGGER_ERROR("Images size not match");
      throw std::runtime_error("Images size not match");
    }
  }

  std::vector<float> tensorDatas(inputBatch * inputChannels * inputHeight *
                                 inputWidth);
  for (int i = 0; i < images.size(); ++i) {
    const cv::Mat &image = images[i];
    // crop roi
    cv::Mat croppedImage;
    if (args.roi.area() > 0) {
      croppedImage = image(args.roi).clone();
    } else {
      croppedImage = image;
    }

    // resize
    cv::Mat resizedImage;
    if (args.isEqualScale) {
      auto padRet = utils::escaleResizeWithPad(
          croppedImage, resizedImage, inputHeight, inputHeight, args.pad);
      args.topPad = padRet.h;
      args.leftPad = padRet.w;
    } else {
      cv::resize(croppedImage, resizedImage, cv::Size(inputWidth, inputHeight),
                 0, 0, cv::INTER_LINEAR);
    }

    // normalize
    cv::Mat floatImage;
    resizedImage.convertTo(floatImage, CV_32FC3);

    cv::Mat normalizedImage;
    if (!args.meanVals.empty() && !args.normVals.empty()) {
      std::vector<cv::Mat> channels(inputChannels);
      cv::split(floatImage, channels);

      for (int i = 0; i < inputChannels; ++i) {
        channels[i] = (channels[i] - args.meanVals[i]) / args.normVals[i];
      }
      cv::merge(channels, normalizedImage);
    } else {
      normalizedImage = floatImage;
    }

    // hwc to chw
    std::vector<float> tensorData(inputChannels * inputHeight * inputWidth);
    int index = 0;
    for (int c = 0; c < inputChannels; ++c) {
      for (int h = 0; h < inputHeight; ++h) {
        for (int w = 0; w < inputWidth; ++w) {
          tensorData[index++] = normalizedImage.at<cv::Vec3f>(h, w)[c];
        }
      }
    }
    std::copy(tensorData.begin(), tensorData.end(),
              tensorDatas.begin() +
                  i * inputChannels * inputHeight * inputWidth);
  }
  return {tensorDatas};
}

}; // namespace infer::dnn
