#ifndef __ULTRA_SOUND_SDK_H__
#define __ULTRA_SOUND_SDK_H__
#include "us_export.h"
#include "us_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *UltraSoundSDKHandle;

ULTRA_SOUND_API UltraSoundSDKHandle UltraSoundSDK_Create();

ULTRA_SOUND_API void UltraSoundSDK_Destroy(UltraSoundSDKHandle handle);

ULTRA_SOUND_API ultra_sound::ErrorCode
UltraSoundSDK_Initialize(UltraSoundSDKHandle handle,
                         const ultra_sound::SDKConfig *config);

ULTRA_SOUND_API ultra_sound::ErrorCode
UltraSoundSDK_ProcessFrame(UltraSoundSDKHandle handle,
                           const ultra_sound::InputPacket *input);

ULTRA_SOUND_API ultra_sound::ErrorCode
UltraSoundSDK_CalcCurrentROI(UltraSoundSDKHandle handle,
                             const ultra_sound::ImageData *input,
                             ultra_sound::Rect *roi);

ULTRA_SOUND_API ultra_sound::ErrorCode
UltraSoundSDK_TryGetNextLesion(UltraSoundSDKHandle handle,
                               ultra_sound::OutputPacket *result);

ULTRA_SOUND_API ultra_sound::ErrorCode
UltraSoundSDK_Terminate(UltraSoundSDKHandle handle);

ULTRA_SOUND_API const char *UltraSoundSDK_GetVersion();
#ifdef __cplusplus
}
#endif

#endif