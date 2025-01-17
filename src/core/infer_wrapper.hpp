/**
 * @file wav_lip_manager.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __WAV_LIP_MANAGER_HPP__
#define __WAV_LIP_MANAGER_HPP__

#include "dnn_infer.hpp"
#include "infer_types.hpp"
#include <atomic>
#include <memory>

namespace android_infer::infer {

template <typename AlgoClass> class InferSafeWrapper {
public:
  InferSafeWrapper(const infer::AlgoBase &config)
      : model(std::make_unique<AlgoClass>(config)), busy(false) {}

  InferErrorCode initialize() { return model->initialize(); }

  bool tryAcquire() {
    bool expected = false;
    return busy.compare_exchange_strong(expected, true);
  }

  void release() { busy.store(false); }

  dnn::AlgoInference *get() { return model.get(); }

private:
  std::unique_ptr<AlgoClass> model;
  std::atomic<bool> busy;
};

} // namespace android_infer::infer

#endif