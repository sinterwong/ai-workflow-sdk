#ifndef __ULTRA_SOUND_EXPORT_H_
#define __ULTRA_SOUND_EXPORT_H_

#ifdef _WIN32
#ifdef ULTRA_SOUND_BUILD_DLL
#define ULTRA_SOUND_API __declspec(dllexport)
#else
#define ULTRA_SOUND_API __declspec(dllimport)
#endif
#else
#define ULTRA_SOUND_API __attribute__((visibility("default")))
#endif

#endif // __ULTRA_SOUND_EXPORT_H_
