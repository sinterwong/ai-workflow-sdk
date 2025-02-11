#ifndef __ANDROID_SDK_EXPORT_H_
#define __ANDROID_SDK_EXPORT_H_

#ifdef _WIN32
#ifdef ANDROID_SDK_BUILD_DLL
#define ANDROID_SDK_API __declspec(dllexport)
#else
#define ANDROID_SDK_API __declspec(dllimport)
#endif
#else
#define ANDROID_SDK_API __attribute__((visibility("default")))
#endif

#endif // __ANDROID_SDK_EXPORT_H_
