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

#include "utils/data_packet.hpp"
#include "utils/thread_pool.hpp"
#include <string>

namespace ai_pipe {

enum class PipeErrorCode { SUCCESS = 0, FAILED = -1 };

using PortData = ::utils::DataPacket;

using PortDataPtr = std::shared_ptr<PortData>;

using ThreadPool = ::utils::thread_pool;

using PortDataMap = std::map<std::string, PortDataPtr>;

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
