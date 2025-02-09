/**
 * @file ai_sdk.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-30
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ai_sdk.hpp"
#include "ai_sdk_impl.hpp"
#include <string>

namespace android_infer {

AndroidSDK::AndroidSDK() : impl_(std::make_unique<AndroidSDKImpl>()) {}

AndroidSDK::~AndroidSDK() {}

ErrorCode AndroidSDK::initialize(const SDKConfig &config) {
  if (!impl_) {
    return ErrorCode::INITIALIZATION_FAILED;
  }
  return impl_->initialize(config);
}

ErrorCode AndroidSDK::pushInput(const InputPacket &input) {
  if (!impl_) {
    return ErrorCode::INVALID_STATE;
  }
  return impl_->pushInput(input);
}

ErrorCode AndroidSDK::calcCurrentROI(const ImageData &input, Rect &roi) {
  if (!impl_) {
    return ErrorCode::INVALID_STATE;
  }
  return impl_->calcCurrentROI(input, roi);
}

ErrorCode AndroidSDK::tryGetNextOutput(OutputPacket &output) {
  if (!impl_) {
    return ErrorCode::INVALID_STATE;
  }
  return impl_->tryGetNextOutput(output);
}

ErrorCode AndroidSDK::terminate() {
  if (!impl_) {
    return ErrorCode::INVALID_STATE;
  }
  return impl_->terminate();
}

std::string AndroidSDK::getVersion() { return "1.0.0"; }

} // namespace android_infer