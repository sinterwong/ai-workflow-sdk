#ifndef __ANDROID_SDK_SDK_H__
#define __ANDROID_SDK_SDK_H__
#include "ai_export.h"
#include "ai_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *AndroidSDKHandle;

ANDROID_SDK_API AndroidSDKHandle AndroidSDK_Create();

ANDROID_SDK_API void AndroidSDK_Destroy(AndroidSDKHandle handle);

ANDROID_SDK_API android_infer::ErrorCode
AndroidSDK_Initialize(AndroidSDKHandle handle,
                      const android_infer::SDKConfig *config);

ANDROID_SDK_API android_infer::ErrorCode
AndroidSDK_PushInput(AndroidSDKHandle handle,
                     const android_infer::InputPacket *input);

ANDROID_SDK_API android_infer::ErrorCode
AndroidSDK_CalcCurrentROI(AndroidSDKHandle handle,
                          const android_infer::ImageData *input,
                          android_infer::Rect *roi);

ANDROID_SDK_API android_infer::ErrorCode
AndroidSDK_TryGetNextOutput(AndroidSDKHandle handle,
                            android_infer::OutputPacket *result);

ANDROID_SDK_API android_infer::ErrorCode
AndroidSDK_Terminate(AndroidSDKHandle handle);

ANDROID_SDK_API const char *AndroidSDK_GetVersion();
#ifdef __cplusplus
}
#endif

#endif