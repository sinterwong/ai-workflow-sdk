# android-infer-sdk
This is a cross-platform inference SDK for Android. It supports ONNX Runtime and NCNN.

## Features ✨
*   **Cross-platform:** Supports Android.
*   **Multiple backends:** ONNX Runtime and NCNN.
*   **Easy to use:** Simple API for inference.

## Build 🚀

### Manual Dependency Management

1. **Link Libraries:** Symbolically link manually compiled libraries to `/repo/3rdparty/target/${TARGET_OS}_${TARGET_ARCH}` (e.g., `/repo/3rdparty/target/Linux_x86_64/opencv`).
2. **Manage Dependencies:** Use `/repo/load_3rdparty.cmake` to manage the loading of your libraries.

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
