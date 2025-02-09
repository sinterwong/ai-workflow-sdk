#include "ai_sdk.hpp"
#include "api/ai_sdk.h"
#include "api/ai_types.h"
#include <cstring>

ANDROID_SDK_API AndroidSDKHandle AndroidSDK_Create() {
  android_infer::AndroidSDK *sdk = new android_infer::AndroidSDK();
  return (AndroidSDKHandle)sdk;
}

ANDROID_SDK_API void AndroidSDK_Destroy(AndroidSDKHandle handle) {
  if (handle) {
    android_infer::AndroidSDK *sdk = (android_infer::AndroidSDK *)handle;
    delete sdk;
  }
}

ANDROID_SDK_API android_infer::ErrorCode
AndroidSDK_Initialize(AndroidSDKHandle handle,
                      const android_infer::SDKConfig *config) {
  if (!handle || !config) {
    return android_infer::ErrorCode::INITIALIZATION_FAILED;
  }
  android_infer::AndroidSDK *sdk = (android_infer::AndroidSDK *)handle;
  return sdk->initialize(*config);
}

ANDROID_SDK_API android_infer::ErrorCode
AndroidSDK_PushInput(AndroidSDKHandle handle,
                     const android_infer::InputPacket *input) {
  if (!handle || !input) {
    return android_infer::ErrorCode::INVALID_STATE;
  }
  android_infer::AndroidSDK *sdk = (android_infer::AndroidSDK *)handle;
  return sdk->pushInput(*input);
}

ANDROID_SDK_API android_infer::ErrorCode
AndroidSDK_CalcCurrentROI(AndroidSDKHandle handle,
                          const android_infer::ImageData *input,
                          android_infer::Rect *roi) {
  if (!handle || !input || !roi) {
    return android_infer::ErrorCode::INVALID_STATE;
  }
  android_infer::AndroidSDK *sdk = (android_infer::AndroidSDK *)handle;
  return sdk->calcCurrentROI(*input, *roi);
}

ANDROID_SDK_API android_infer::ErrorCode
AndroidSDK_TryGetNextOutput(AndroidSDKHandle handle,
                            android_infer::OutputPacket *result) {
  if (!handle || !result) {
    return android_infer::ErrorCode::INVALID_STATE;
  }
  android_infer::AndroidSDK *sdk = (android_infer::AndroidSDK *)handle;
  return sdk->tryGetNextOutput(*result);
}

ANDROID_SDK_API android_infer::ErrorCode
AndroidSDK_Terminate(AndroidSDKHandle handle) {
  if (!handle) {
    return android_infer::ErrorCode::INVALID_STATE;
  }
  android_infer::AndroidSDK *sdk = (android_infer::AndroidSDK *)handle;
  return sdk->terminate();
}

ANDROID_SDK_API const char *AndroidSDK_GetVersion() {
  std::string version = android_infer::AndroidSDK::getVersion();
  char *c_version = (char *)malloc(version.length() + 1);
  if (c_version) {
    strcpy(c_version, version.c_str());
  }
  return c_version;
}
