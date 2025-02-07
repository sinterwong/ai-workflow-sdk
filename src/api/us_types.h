#ifndef __ULTRA_SOUND_TYPES_H__
#define __ULTRA_SOUND_TYPES_H__
#include "us_export.h"
#include <cstdint>
#include <string>
#include <vector>

namespace ultra_sound {

struct Rect {
  int32_t x;
  int32_t y;
  int32_t w;
  int32_t h;
};

struct ULTRA_SOUND_API RetBBox {
  Rect rect;     // 检测框坐标
  float score;   // 目标得分
  int32_t label; // 目标类别
};

struct ULTRA_SOUND_API ImageData {
  std::string uuid;               // 数据标识
  std::vector<uint8_t> frameData; // 图像数据（encode）
  int64_t frameIndex;             // 帧号
  uint32_t width;                 // 图像宽度（TODO: 非必要）
  uint32_t height;                // 图像长度（TODO: 非必要）
};

struct ULTRA_SOUND_API SDKConfig {
  uint32_t numWorkers{1};   // 工作线程数量
  std::string modelPath;    // 模型路径（所有模型所在的文件夹）
  std::string algoConfPath; // 算法配置路径
  std::string logPath;      // 日志路径
  uint logLevel = 2;        // 0-4 [Trace, Debug, Info, Warning, Error]
};

struct ULTRA_SOUND_API InputPacket {
  ImageData frame;   // 图像帧
  Rect roi;          // 待检测区域（TODO: 非必要，内部会计算）
  int64_t timestamp; // 时间戳(微秒)
};

struct ULTRA_SOUND_API LesionOutput {
  std::string uuid;   // 数据标识
  int64_t frameIndex; // 帧号
  int64_t timestamp;  // 时间戳(微秒)

  int32_t trackId;        // 跟踪标识
  Rect box;               // 斑块检测框
  Rect smoothBox;         // 显示所用框
  int32_t label;          // 类别
  float score;            // 检测和分类加权分数
  float lastDetScore;     // 检测分数
  float lastFprScore;     // 去假阳分数
  int32_t fprModelBirads; // 辅助的良恶性结果
  float maxArea;          // 斑块尺寸
};

struct ULTRA_SOUND_API OutputPacket {
  std::vector<LesionOutput> lesions;     // 每帧的病灶检测结果
  std::vector<ImageData> positiveImages; // 阳性图片
  ImageData negativeImage;               // 阴性图片
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
