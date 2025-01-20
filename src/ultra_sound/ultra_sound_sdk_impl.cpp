/**
 * @file ultra_sound_sdk_impl.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "ultra_sound_sdk_impl.hpp"
#include "api/us_types.h"
#include <cmath>
#include <opencv2/core/types.hpp>

namespace ultra_sound {

UltraSoundSDKImpl::UltraSoundSDKImpl() : isRunning(false) {}

ErrorCode UltraSoundSDKImpl::initialize(const SDKConfig &config) {
  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::pushInput(const InputPacket &input) {
  return ErrorCode::SUCCESS;
}

ErrorCode UltraSoundSDKImpl::tryGetNext(OutputPacket &result) {
  return ErrorCode::SUCCESS;
}

void UltraSoundSDKImpl::processLoop() {}

} // namespace ultra_sound
