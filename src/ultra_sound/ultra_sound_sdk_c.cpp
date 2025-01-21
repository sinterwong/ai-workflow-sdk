#include "api/us_sdk.h"
#include "api/us_types.h"
#include "ultra_sound_sdk.hpp"
#include <cstring>

ULTRA_SOUND_API UltraSoundSDKHandle UltraSoundSDK_Create() {
  ultra_sound::UltraSoundSDK *sdk = new ultra_sound::UltraSoundSDK();
  return (UltraSoundSDKHandle)sdk;
}

ULTRA_SOUND_API void UltraSoundSDK_Destroy(UltraSoundSDKHandle handle) {
  if (handle) {
    ultra_sound::UltraSoundSDK *sdk = (ultra_sound::UltraSoundSDK *)handle;
    delete sdk;
  }
}

ULTRA_SOUND_API ultra_sound::ErrorCode
UltraSoundSDK_Initialize(UltraSoundSDKHandle handle,
                         const ultra_sound::SDKConfig *config) {
  if (!handle || !config) {
    return ultra_sound::ErrorCode::INITIALIZATION_FAILED;
  }
  ultra_sound::UltraSoundSDK *sdk = (ultra_sound::UltraSoundSDK *)handle;
  return sdk->initialize(*config);
}

ULTRA_SOUND_API ultra_sound::ErrorCode
UltraSoundSDK_PushInput(UltraSoundSDKHandle handle,
                        const ultra_sound::InputPacket *input) {
  if (!handle || !input) {
    return ultra_sound::ErrorCode::INVALID_INPUT;
  }
  ultra_sound::UltraSoundSDK *sdk = (ultra_sound::UltraSoundSDK *)handle;
  return sdk->pushInput(*input);
}

ULTRA_SOUND_API ultra_sound::ErrorCode
UltraSoundSDK_Terminate(UltraSoundSDKHandle handle) {
  if (!handle) {
    return ultra_sound::ErrorCode::INVALID_STATE;
  }
  ultra_sound::UltraSoundSDK *sdk = (ultra_sound::UltraSoundSDK *)handle;
  return sdk->terminate();
}

ULTRA_SOUND_API ultra_sound::ErrorCode
UltraSoundSDK_TryGetNext(UltraSoundSDKHandle handle,
                         ultra_sound::OutputPacket *result) {
  if (!handle || !result) {
    return ultra_sound::ErrorCode::INVALID_INPUT;
  }
  ultra_sound::UltraSoundSDK *sdk = (ultra_sound::UltraSoundSDK *)handle;
  return sdk->tryGetNext(*result);
}

ULTRA_SOUND_API const char *UltraSoundSDK_GetVersion() {
  std::string version = ultra_sound::UltraSoundSDK::getVersion();
  char *c_version = (char *)malloc(version.length() + 1);
  if (c_version) {
    strcpy(c_version, version.c_str());
  }
  return c_version;
}
