/**
 * @file algo_infer_base.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-23
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __INFERENCE_ALGO_INFER_BASE_HPP__
#define __INFERENCE_ALGO_INFER_BASE_HPP__

#include "infer_types.hpp"
#include "utils/type_safe_factory.hpp"

namespace infer::dnn::vision {
using AlgoConstructorParams = utils::ConstructorParams;

class AlgoInferBase {
public:
  AlgoInferBase(){};
  virtual ~AlgoInferBase(){};

  virtual InferErrorCode initialize() = 0;

  virtual InferErrorCode infer(AlgoInput &input, AlgoOutput &output) = 0;

  virtual InferErrorCode terminate() = 0;

  virtual const ModelInfo &getModelInfo() const noexcept = 0;

  virtual const std::string &getModuleName() const noexcept = 0;
};
} // namespace infer::dnn::vision
#endif
