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

#include "infer_types.hpp"
#include <memory>

namespace android_infer::infer::dnn {
class AlgoInference {
public:
  AlgoInference(const AlgoBase &_param) : mParams(_param) {}

  ~AlgoInference() {}

  /**
   * @brief initialize the network
   *
   * @return true
   * @return false
   */
  virtual InferErrorCode initialize();

  /**
   * @brief Runs the inference engine with input of void*
   *
   * @return true
   * @return false
   */
  virtual InferErrorCode infer(AlgoInput &input, AlgoOutput &output) = 0;

  /**
   * @brief Get the Model Info object
   *
   */
  virtual const ModelInfo &getModelInfo();

  /**
   * @brief Release
   *
   */
  virtual InferErrorCode terminate();

  /**
   * @brief Print model infomations
   *
   */
  virtual void prettyPrintModelInfos();

protected:
  AlgoBase mParams;
  std::shared_ptr<ModelInfo> modelInfo;
};
} // namespace android_infer::infer::dnn
#endif
