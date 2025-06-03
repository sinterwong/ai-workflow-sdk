/**
 * @file pipeline_context.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "pipeline_context.hpp"
namespace ai_pipe {

// 资源设置接口
void PipelineContext::setAlgoManager(
    std::shared_ptr<infer::dnn::AlgoManager> manager) {
  algoManager_ = std::move(manager);
}

void PipelineContext::setThreadPool(std::shared_ptr<ThreadPool> pool) {
  threadPool_ = std::move(pool);
}

// void PipelineContext::setDataProducer(std::shared_ptr<DataProducer> producer)
// {
//   dataProducer_ = std::move(producer);
// }

// 资源获取接口
std::shared_ptr<infer::dnn::AlgoManager>
PipelineContext::getAlgoManager() const {
  return algoManager_;
}

std::shared_ptr<ThreadPool> PipelineContext::getThreadPool() const {
  return threadPool_;
}

// std::shared_ptr<DataProducer> PipelineContext::getDataProducer() const {
//   return dataProducer_;
// }

// 检查必要资源是否已设置
bool PipelineContext::isValid() const {
  return algoManager_ != nullptr && threadPool_ != nullptr;
}

} // namespace ai_pipe
