/**
 * @file pipeline_context.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __PIPE_PIPELINE_CONTEXT_HPP__
#define __PIPE_PIPELINE_CONTEXT_HPP__

#include "core/algo_manager.hpp"
#include "pipe_types.hpp"

namespace ai_pipe {
class PipelineContext {
public:
  PipelineContext() = default;
  ~PipelineContext() = default;

  // 禁止拷贝，允许移动
  PipelineContext(const PipelineContext &) = delete;
  PipelineContext &operator=(const PipelineContext &) = delete;
  PipelineContext(PipelineContext &&) = default;
  PipelineContext &operator=(PipelineContext &&) = default;

  // 资源设置接口
  void setAlgoManager(std::shared_ptr<infer::dnn::AlgoManager> manager);

  void setThreadPool(std::shared_ptr<ThreadPool> pool);

  // void setDataProducer(std::shared_ptr<DataProducer> producer) ;

  std::shared_ptr<infer::dnn::AlgoManager> getAlgoManager() const;

  std::shared_ptr<ThreadPool> getThreadPool() const;

  // std::shared_ptr<DataProducer> getDataProducer() const;

  bool isValid() const;

private:
  // 所有成员内部线程安全
  std::shared_ptr<infer::dnn::AlgoManager> algoManager_;
  std::shared_ptr<ThreadPool> threadPool_;

  // TODO: 后续实现数据生产者和自定义共享资源
  // std::shared_ptr<DataProducer> dataProducer_;
  // std::unordered_map<std::string, std::any> customResources_;

  // TODO: 暂时不需要锁，因为只会在pipeline initialize时创建
  // std::mutex mutex_;
};
} // namespace ai_pipe

#endif
