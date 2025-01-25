#ifndef __ULTRA_SOUND_TYPES_H__
#define __ULTRA_SOUND_TYPES_H__
#include "us_export.h"
#include <array>
#include <cstdint>
#include <string>
#include <sys/types.h>
#include <vector>

namespace ultra_sound {

struct ULTRA_SOUND_API SDKConfig {
  uint32_t numWorkers{1};   // 工作线程数量
  std::string modelPath;    // 模型路径
  std::string algoConfPath; // 算法配置路径
  std::string logPath;      // 日志路径
  uint logLevel = 2;        // 0-4 [Trace, Debug, Info, Warning, Error]
};

struct ULTRA_SOUND_API InputPacket {
  std::string uuid;               // 数据标识
  int64_t frameIndex;             // 帧index
  std::vector<uint8_t> imageData; // 图像数据
  uint32_t width;                 // 帧宽度
  uint32_t height;                // 帧高度
  int64_t timestamp;              // 时间戳(微秒)
};

struct ULTRA_SOUND_API RetBBox {
  std::array<int, 4> rect; // 检测框坐标 [x, y, w, h]
  float score;             // 目标得分
  int label;               // 目标类别
};

struct ULTRA_SOUND_API OutputPacket {
  std::vector<RetBBox> bboxes;    // 检测框结果
  std::vector<uint8_t> frameData; // 算法结果视频帧数据
  std::string uuid;               // 数据标识
  int64_t frameIndex;             // 序列号
  uint32_t width;                 // 帧宽度
  uint32_t height;                // 帧高度
  int64_t timestamp;              // 时间戳(微秒)
};

enum class ErrorCode {
  SUCCESS = 0,
  INVALID_INPUT = -1,
  FILE_NOT_FOUND = -2,
  INVALID_FILE_FORMAT = -3,
  INITIALIZATION_FAILED = -4,
  PROCESSING_ERROR = -5,
  INVALID_STATE = -6,
  TRY_GET_NEXT_OVERTIME = -7
};
} // namespace ultra_sound
#endif
