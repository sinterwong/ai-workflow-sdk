#ifndef __NCNN_INFERENCE_RTM_DET_HPP_
#define __NCNN_INFERENCE_RTM_DET_HPP_

#include "dnn_infer.hpp"
#include "infer_types.hpp"

namespace android_infer::infer::dnn {
class RTMDetInference : public AlgoInference {
public:
  explicit RTMDetInference(const AlgoBase &param) : AlgoInference(param) {}

  InferErrorCode infer(AlgoInput &input, AlgoOutput &output) override;
};
} // namespace android_infer::infer::dnn
#endif