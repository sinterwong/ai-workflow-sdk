/**
 * @file ultra_sound_sdk.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-30
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ultra_sound_sdk.hpp"
#include "ultra_sound_sdk_impl.hpp"
#include <string>

namespace ultra_sound {

UltraSoundSDK::UltraSoundSDK() : impl_(std::make_unique<UltraSoundSDKImpl>()) {}

UltraSoundSDK::~UltraSoundSDK() {}

ErrorCode UltraSoundSDK::initialize(const SDKConfig &config) {
  if (!impl_) {
    return ErrorCode::INITIALIZATION_FAILED;
  }
  return impl_->initialize(config);
}

ErrorCode UltraSoundSDK::pushInput(const InputPacket &input) {
  if (!impl_) {
    return ErrorCode::INVALID_STATE;
  }
  return impl_->pushInput(input);
}

ErrorCode UltraSoundSDK::terminate() {
  if (!impl_) {
    return ErrorCode::INVALID_STATE;
  }
  return impl_->terminate();
}

ErrorCode UltraSoundSDK::tryGetNext(OutputPacket &result) {
  if (!impl_) {
    return ErrorCode::INVALID_STATE;
  }
  return impl_->tryGetNext(result);
}

std::string UltraSoundSDK::getVersion() { return "1.0.0"; }

} // namespace ultra_sound