/**
 * @file pipe_types.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __PIPE_TYPES_HPP__
#define __PIPE_TYPES_HPP__

#include <opencv2/opencv.hpp> // Add this include for cv::Mat
#include "utils/data_packet.hpp"
#include "utils/thread_pool.hpp"
#include <string>

namespace ai_pipe {

enum class PipeErrorCode { SUCCESS = 0, FAILED = -1 };

using PortData = ::utils::DataPacket;

using PortDataPtr = std::shared_ptr<PortData>;

using ThreadPool = ::utils::thread_pool;

using PortDataMap = std::map<std::string, PortDataPtr>;

struct RawImageData {
  cv::Mat image;
  std::string image_path; // Store the original path for context if needed
};

struct BoundingBox {
  int x;
  int y;
  int width;
  int height;
  float score;
  int label;
};

struct InferenceResult {
  std::vector<BoundingBox> boxes;
};

struct VisualizedImageData {
  cv::Mat image;
  std::string output_path; // Path where the visualized image is saved
};

struct PipelineConfig {
  std::string graphConfigPath;
  uint8_t numWorkers = 4;
};

// 执行状态枚举
enum class NodeExecutionState {
  WAITING,   // 等待输入数据
  READY,     // 输入数据就绪，可以执行
  EXECUTING, // 正在执行中
  COMPLETED, // 执行完成
  FAILED     // 执行失败
};

enum class PipelineState {
  IDLE, // 空闲
  RUNNING,
  STOPPING,
  STOPPED,
  ERROR
};
} // namespace ai_pipe

#endif
