# ai-workflow-sdk
This is a cross-platform inference SDK for AI. It supports ONNX Runtime and NCNN currently.

## Features ✨
*   **Cross-platform:** Supports Android and Linux.
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
    *   [Android_aarch64](https://github.com/sinterwong/ai-workflow-sdk/releases/download/v0.1.0-alpha/dependency-Android_aarch64.tgz)
    *   [Linux_x86_64](https://github.com/sinterwong/ai-workflow-sdk/releases/download/v0.1.0-alpha/dependency-Linux_x86_64.tgz)
    *   decompress it to `/repo/3rdparty/target/${TARGET_OS}_${TARGET_ARCH}`

## Project Structure 🏗️

```
ai-workflow-sdk/
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

- [x] Design and implement native pipeline module
- [x] Build CI
- [ ] Implement a demo module that combines a complete algorithm module and a pipeline module.
- [ ] Implement the api interface of the SDK
- [ ] Implement a more powerful AlgoManager
