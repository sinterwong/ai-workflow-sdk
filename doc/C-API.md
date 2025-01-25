# 超声SDK接口文档

## 概述
本文档描述超声SDK的C语言接口规范，包含数据结构定义、接口函数说明及错误码说明。

## 数据结构

### 基础结构体

#### Rect
```cpp
struct Rect {
  int32_t x;   // 矩形左上角X坐标
  int32_t y;   // 矩形左上角Y坐标 
  int32_t w;   // 矩形宽度
  int32_t h;   // 矩形高度
};
```
#### RetBBox
```cpp
struct RetBBox {
  Rect rect;    // 检测框坐标
  float score;  // 目标置信度（0-1）
  int label;    // 目标类别标识
};
```

#### ImageData
```cpp
struct ImageData {
  std::string uuid;             // 数据唯一标识
  std::vector<uint8_t> frameData; // opencv编码后的图像数据
  int64_t frameIndex;           // 帧序列号
  uint32_t width;               // 图像宽度（可选）
  uint32_t height;              // 图像高度（可选）
};
```

### 核心接口数据结构

#### SDKConfig
```cpp
struct SDKConfig {
  uint32_t numWorkers{1};     // 工作线程数，默认1
  std::string modelPath;      // 模型文件目录路径
  std::string algoConfPath;   // 算法配置文件路径
  std::string logPath;        // 日志存储路径
  uint logLevel = 2;          // 日志级别 0-4 [Trace, Debug, Info, Warning, Error]
};
```

#### InputPacket
```cpp
struct InputPacket {
  ImageData frame;     // 输入图像帧
  Rect roi;            // 检测区域坐标
  int64_t timestamp;   // 时间戳（微秒）
};
```

#### LesionOutput
```cpp
struct LesionOutput {
  std::vector<RetBBox> bboxes; // 检测框结果
  std::string uuid;            // 数据标识
  int64_t frameIndex;          // 帧号
  int64_t timestamp;           // 时间戳(微秒)
  // TODO: 可能的新增字段...
};
```

#### OutputPacket
```cpp
struct OutputPacket {
  std::vector<LesionOutput> lesions;     // 病灶检测结果集合
  std::vector<ImageData> positiveImages; // 阳性结果图像集合
  ImageData negativeImage;               // 阴性结果图像
  // TODO: 可能的新增字段...
};
```

## API接口说明

### SDK生命周期管理

#### UltraSoundSDK_Create
```cpp
UltraSoundSDKHandle UltraSoundSDK_Create();
```
创建SDK实例句柄

**返回**

`UltraSoundSDKHandle`SDK实例句柄

#### UltraSoundSDK_Destroy
```cpp
void UltraSoundSDK_Destroy(UltraSoundSDKHandle handle);
```
销毁SDK实例句柄

**参数**

`handle`SDK实例句柄

### 核心工作流调用
#### UltraSoundSDK_Initialize
```cpp
int UltraSoundSDK_Initialize(UltraSoundSDKHandle handle, const SDKConfig& config);
```
初始化SDK

**参数**

`handle` SDK实例句柄
`config` SDK配置参数

**返回**

`ErrorCode` 错误码

#### UltraSoundSDK_ProcessFrame
```cpp
ErrorCode UltraSoundSDK_ProcessFrame(UltraSoundSDKHandle handle,
                                    const InputPacket *input);
```
处理输入图像帧

**参数**

`handle` SDK实例句柄
`input` 输入图像帧数据

**返回**

`ErrorCode` 错误码


#### UltraSoundSDK_TryGetNextLesion
```cpp
ErrorCode UltraSoundSDK_TryGetNextLesion(UltraSoundSDKHandle handle,
                                         OutputPacket *result);
```
获取下一帧病灶检测结果

**参数**

`handle` SDK实例句柄
`result` 输出结果数据

**返回**

`ErrorCode` 错误码

#### UltraSoundSDK_Terminate
```cpp
void UltraSoundSDK_Terminate(UltraSoundSDKHandle handle);
```
终止SDK运行

**参数**

`handle` SDK实例句柄

### 版本信息获取
#### UltraSoundSDK_GetVersion
```cpp
const char *UltraSoundSDK_GetVersion();
```
获取SDK版本号

**返回**

版本号字符串（格式：X.Y.Z）

## ErrorCode 说明

错误码 | 值 | 说明
---|---|---
SUCCESS | 0 | 操作成功
INVALID_INPUT | -1 | 非法输入参数
FILE_NOT_FOUND | -2 | 文件不存在
INVALID_FILE_FORMAT | -3 | 文件格式错误
INITIALIZATION_FAILED | -4 | 初始化失败
PROCESSING_ERROR | -5 | 处理过程错误
INVALID_STATE | -6 | 非法状态调用
TRY_GET_NEXT_OVERTIME | -7 | 结果获取超时
