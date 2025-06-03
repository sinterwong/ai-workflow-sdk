/**
 * @file runtime_node_input_port.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-05-31
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __PIPE_RUNTIME_NODE_INPUT_PORT_HPP__
#define __PIPE_RUNTIME_NODE_INPUT_PORT_HPP__

#include "pipe_types.hpp"
#include <string>

namespace ai_pipe {

class RuntimeNodeInputPort {
public:
  RuntimeNodeInputPort(const std::string &portName, bool isBlockingPort = true)
      : portName_(portName), isBlockingPort_(isBlockingPort) {}

  void push(const PortData &dataPacket);

  std::optional<PortData> tryPop();

  bool hasData() const;

  const std::string &getName() const { return portName_; }

  bool isBlocking() const { return isBlockingPort_; }

private:
  std::string portName_;
  bool isBlockingPort_; // 表示这个端点必须有数据节点才能执行
};

} // namespace ai_pipe

#endif
