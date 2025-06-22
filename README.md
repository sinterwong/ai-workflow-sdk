# ai-workflow-sdk
This is a multi-platform AI inference software package. It currently encapsulates two inference frameworks, ONNX Runtime and NCNN, for performing model inference. The pipeline module framework supports DAG-based logic construction, enabling the direct creation of pipelines through configuration files once computing nodes are defined and registered. Plans include supporting additional computing frameworks and platforms in the future.

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
- [x] Implement a demo module that combines a complete algorithm module and a pipeline module.
- [ ] Implement the api interface of the SDK
- [ ] Implement a more powerful AlgoManager
