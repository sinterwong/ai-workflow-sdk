/**
 * @file runtime_node.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-05-31
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __PIPE_RUNTIME_NODE_HPP__
#define __PIPE_RUNTIME_NODE_HPP__

#include "node_base.hpp"
#include "pipe_types.hpp"
#include "runtime_node_input_port.hpp"
#include "utils/thread_pool.hpp"
#include <atomic>
#include <mutex>

namespace ai_pipe {

class Pipeline;

class RuntimeNode : public std::enable_shared_from_this<RuntimeNode> {
public:
  enum class State { IDLE, READY_TO_RUN, RUNNING, FAILED };

  RuntimeNode(std::shared_ptr<NodeBase> nodeLogic, Pipeline *ownerPipeline,
              const std::string &name)
      : nodeLogic_(nodeLogic), ownerPipeline_(ownerPipeline) {}
  ~RuntimeNode() = default;

  const std::string &getName() const;

  State getState() const { return state_.load(std::memory_order_acquire); }

  // 由Pipeline在图构建之后调用
  void addOutputConnection(const std::string &sourcePort,
                           std::weak_ptr<RuntimeNode> destNode,
                           const std::string &destPort);

  // 由上游（通过Pipeline)调用，将数据推入输入端口
  // 返回true如果此操作可能使节点就绪
  bool pushToInputPort(const std::string &portName, const PortData &data);

  // 检查是否所有阻塞输入端口都有数据
  // 如果是，则准备输入快照，并返回true，状态变为READY_TO_RUN
  // NOTE: 此方法需要线程安全的访问输入队列和内部状态
  bool tryPrepareForExecution();

  // 给线程池提交任务
  void scheduleExecution(utils::thread_pool *pool);

private:
  void executeInternal();

private:
  std::shared_ptr<NodeBase> nodeLogic_;
  Pipeline *ownerPipeline_; // 非拥有
  std::string name_;

  std::map<std::string, RuntimeNodeInputPort> inputPortsMap_;

  // <本节点output端口名, list of <下游节点weak指针, 下游节点input端口名>>
  std::map<std::string,
           std::vector<std::pair<std::weak_ptr<RuntimeNode>, std::string>>>
      outputConnections_;

  std::atomic<State> state_{State::IDLE};

  // safe tryPrepareForExecution and currentInputSnapshot_
  std::mutex preparationMutex_;

  // 为下一次process准备的输入数据
  PortDataMap currentInputSnapshot_;
};

} // namespace ai_pipe

#endif
