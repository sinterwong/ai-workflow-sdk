/**
 * @file node_base.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __PIPE_NODE_BASE_HPP_
#define __PIPE_NODE_BASE_HPP_
#include "pipe_types.hpp"
#include "pipeline_context.hpp"
#include <string>
#include <vector>
namespace ai_pipe {

class NodeBase {
public:
  NodeBase(const std::string name) : name_(name) {}
  virtual ~NodeBase() {}

  const std::string &getName() const { return name_; }

  virtual void process(const PortDataMap &inputs, PortDataMap &outputs,
                       std::shared_ptr<PipelineContext> context = nullptr) = 0;

  virtual std::vector<std::string> getExpectedInputPorts() const { return {}; }

  virtual std::vector<std::string> getExpectedOutputPorts() const { return {}; }

protected:
  std::string name_;
};
} // namespace ai_pipe

#endif
