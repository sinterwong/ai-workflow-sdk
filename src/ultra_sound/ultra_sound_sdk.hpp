/**
 * @file ultra_sound_sdk.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __ULTRA_SOUND_SDK_HPP__
#define __ULTRA_SOUND_SDK_HPP__
#include "api/us_export.h"
#include "api/us_types.h"
#include <memory>
#include <string>

namespace ultra_sound {

class UltraSoundSDKImpl;

class ULTRA_SOUND_API UltraSoundSDK {
public:
  UltraSoundSDK();
  ~UltraSoundSDK();

  ErrorCode initialize(const SDKConfig &config);

  ErrorCode processFrame(const InputPacket &input);

  ErrorCode calcCurrentROI(const ImageData &input, Rect &roi);

  ErrorCode tryGetNextLesion(OutputPacket &output);

  ErrorCode terminate();

  static std::string getVersion();

private:
  std::unique_ptr<UltraSoundSDKImpl> impl_;

  UltraSoundSDK(const UltraSoundSDK &) = delete;
  UltraSoundSDK &operator=(const UltraSoundSDK &) = delete;
};

} // namespace ultra_sound

#endif
