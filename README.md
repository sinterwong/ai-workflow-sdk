# android-infer-sdk
This is a cross-platform inference SDK for Android. It supports ONNX Runtime and NCNN.

## Features ✨
*   **Cross-platform:** Supports Android.
*   **Multiple backends:** ONNX Runtime and NCNN.
*   **Easy to use:** Simple API for inference.

## Env 🛠️
*   CMake 3.15+
*   NDK r27
*   IDE VSCode
*   Ubuntu 24.04

## Build 🚀

### Manual Dependency Management

1. **Link Libraries:** Symbolically link manually compiled libraries to `/repo/3rdparty/target/${TARGET_OS}_${TARGET_ARCH}` (e.g., `/repo/3rdparty/target/Linux_x86_64/opencv`).
2. **Manage Dependencies:** Use `/repo/load_3rdparty.cmake` to manage the loading of your libraries.
3. You can download dependencies from the following links:
    *   [Android_aarch64](https://drive.google.com/file/d/1MLYxTwMCKGaWMq-BWhFDSxzDZ2GKy_G5/view?usp=drive_link)
    *   [Linux_x86_64](https://drive.google.com/file/d/1Z-hRnXSbFrS9pyNn_5mCBRUK5bgQJUs7/view?usp=drive_link)
    *   decompress it to `/repo/3rdparty/target/${TARGET_OS}_${TARGET_ARCH}`

## Project Structure 🏗️

```
android-infer-sdk/
├── src/
│   ├── api/     # interface to be provided externally
│   └── ai_sdk   # Implementation of api
│   └── core     # Core code
│   └── jni      # Encapsulate the interface used by Android
│   └── logger   # Log system
│   └── ai_pipe  # Core logic
│   └── utils    # common utils for all modules
├── cmake/
├── scripts/
├── platform/
├── tests/
├── build/
├── tools/
└── README.md
```

## Roadmap 🗺️

- [ ] Design and implement pipeline module
- [ ] Build CI
