/**
 * @file ai_sdk.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __ANDROID_INFER_SDK_HPP__
#define __ANDROID_INFER_SDK_HPP__
#include "api/ai_export.h"
#include "api/ai_types.h"
#include <memory>
#include <string>

namespace android_infer {

class AndroidSDKImpl;

class ANDROID_SDK_API AndroidSDK {
public:
  AndroidSDK();
  ~AndroidSDK();

  ErrorCode initialize(const SDKConfig &config);

  ErrorCode pushInput(const InputPacket &input);

  ErrorCode calcCurrentROI(const ImageData &input, Rect &roi);

  ErrorCode tryGetNextOutput(OutputPacket &output);

  ErrorCode terminate();

  static std::string getVersion();

private:
  std::unique_ptr<AndroidSDKImpl> impl_;

  AndroidSDK(const AndroidSDK &) = delete;
  AndroidSDK &operator=(const AndroidSDK &) = delete;
};

} // namespace android_infer

#endif
