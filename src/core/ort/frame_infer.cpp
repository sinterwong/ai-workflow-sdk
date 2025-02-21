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
  if (!frameInput) {
    LOGGER_ERROR("Invalid input parameters");
    throw std::runtime_error("Invalid input parameters");
  }

  if (inputNames.size() != 1) {
    LOGGER_ERROR("Input node number is not 1");
    throw std::runtime_error("Input node number is not 1");
  }

  const auto &inputShape = inputShapes.at(0);
  const size_t numDims = inputShape.size();

  // Parse input dimensions
  int inputWidth = 1, inputHeight = 1, inputChannels = 1, inputBatch = 1;

  // Handle different input dimensions
  if (numDims >= 2) {
    inputWidth = inputShape[numDims - 1];
    inputHeight = inputShape[numDims - 2];
  }
  if (numDims >= 3) {
    inputChannels = inputShape[numDims - 3];
  }
  if (numDims >= 4) {
    inputBatch = inputShape[numDims - 4];
  }

  // Ensure batch size is 1
  if (inputBatch != 1) {
    throw std::runtime_error("Only batch size 1 is supported");
  }

  const cv::Mat &image = frameInput->image;
  auto &args = frameInput->args;

  // Crop ROI
  cv::Mat croppedImage;
  if (args.roi.area() > 0) {
    croppedImage = image(args.roi).clone();
  } else {
    croppedImage = image;
  }

  // Resize
  cv::Mat resizedImage;
  if (args.needResize) {
    if (args.isEqualScale) {
      auto padRet = utils::escaleResizeWithPad(
          croppedImage, resizedImage, inputHeight, inputWidth, args.pad);
      args.topPad = padRet.h;
      args.leftPad = padRet.w;
    } else {
      cv::resize(croppedImage, resizedImage, cv::Size(inputWidth, inputHeight),
                 0, 0, cv::INTER_LINEAR);
    }
  } else {
    resizedImage = croppedImage;
  }

  // Convert to float
  cv::Mat floatImage;
  resizedImage.convertTo(floatImage, CV_32F);

  // Normalization
  cv::Mat normalizedImage;
  if (!args.meanVals.empty() && !args.normVals.empty()) {
    // Validate normalization parameters
    if (args.meanVals.size() != inputChannels ||
        args.normVals.size() != inputChannels) {
      throw std::runtime_error(
          "meanVals and normVals size must match input channels");
    }

    std::vector<cv::Mat> channels(inputChannels);
    cv::split(floatImage, channels);

    // Apply normalization per channel
    for (int i = 0; i < inputChannels; ++i) {
      channels[i] = (channels[i] - args.meanVals[i]) / args.normVals[i];
    }
    cv::merge(channels, normalizedImage);
  } else {
    normalizedImage = floatImage;
  }

  // Prepare output tensor data
  std::vector<float> tensorDatas(inputChannels * inputHeight * inputWidth);

  if (numDims < 3) {
    // For 2D input (e.g., grayscale image), copy data directly
    if (normalizedImage.isContinuous()) {
      std::memcpy(tensorDatas.data(), normalizedImage.data,
                  tensorDatas.size() * sizeof(float));
    } else {
      int index = 0;
      for (int h = 0; h < inputHeight; ++h) {
        for (int w = 0; w < inputWidth; ++w) {
          tensorDatas[index++] = normalizedImage.at<float>(h, w);
        }
      }
    }
  } else {
    // For 3D or 4D input, perform HWC to CHW conversion
    int index = 0;
    if (inputChannels == 3) {
      for (int c = 0; c < inputChannels; ++c) {
        for (int h = 0; h < inputHeight; ++h) {
          for (int w = 0; w < inputWidth; ++w) {
            tensorDatas[index++] = normalizedImage.at<cv::Vec3f>(h, w)[c];
          }
        }
      }
    } else if (inputChannels == 1) {
      for (int h = 0; h < inputHeight; ++h) {
        for (int w = 0; w < inputWidth; ++w) {
          tensorDatas[index++] = normalizedImage.at<float>(h, w);
        }
      }
    } else {
      throw std::runtime_error("Unsupported number of input channels");
    }
  }

  return {tensorDatas};
}
}; // namespace infer::dnn
