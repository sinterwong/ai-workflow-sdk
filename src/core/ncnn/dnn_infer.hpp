/**
 * @file dnn_infer.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __NCNN_INFERENCE_HPP_
#define __NCNN_INFERENCE_HPP_

#include "infer.hpp"
#include <memory>
#include <ncnn/net.h>

namespace infer::dnn {
class AlgoInference : public Inference {
public:
  AlgoInference(const InferParamBase &param)
      : params(std::make_unique<InferParamBase>(param)) {
    blobPoolAllocator.set_size_compare_ratio(0.f);
    workspacePoolAllocator.set_size_compare_ratio(0.f);
  }

  virtual ~AlgoInference() override {
    for (void *ptr : m_aligned_buffers) {
      free(ptr);
    }
    m_aligned_buffers.clear();
  }

  virtual InferErrorCode initialize() override;

  virtual InferErrorCode infer(AlgoInput &input,
                               ModelOutput &modelOutput) override;

  virtual const ModelInfo &getModelInfo() override;

  virtual InferErrorCode terminate() override;

protected:
  virtual std::vector<std::pair<std::string, ncnn::Mat>>
  preprocess(AlgoInput &input) const = 0;

protected:
  std::unique_ptr<InferParamBase> params;
  std::vector<std::string> inputNames;
  std::vector<std::string> outputNames;

  ncnn::Net net;

  ncnn::UnlockedPoolAllocator blobPoolAllocator;
  ncnn::PoolAllocator workspacePoolAllocator;

private:
  std::vector<void *> m_aligned_buffers;
};
} // namespace infer::dnn
#endif
