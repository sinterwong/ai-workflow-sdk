/**
 * @file vision_registrar.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-09
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __VISION_REGISTRAR_HPP__
#define __VISION_REGISTRAR_HPP__

#include "fpr_cls.hpp"
#include "fpr_feat.hpp"
#include "nano_det.hpp"
#include "rtm_det.hpp"
#include "softmax_cls.hpp"
#include "utils/type_safe_factory.hpp"
#include "vision.hpp"
#include "yolo_det.hpp"

namespace infer::dnn::vision {
class VisionRegistrar {
public:
  static VisionRegistrar &getInstance() {
    static VisionRegistrar instance;
    return instance;
  }

  VisionRegistrar(const VisionRegistrar &) = delete;
  VisionRegistrar &operator=(const VisionRegistrar &) = delete;
  VisionRegistrar(VisionRegistrar &&) = delete;
  VisionRegistrar &operator=(VisionRegistrar &&) = delete;

private:
  VisionRegistrar() {
    utils::Factory<VisionBase>::instance().registerCreator(
        "RTMDet",
        [](const utils::ConstructorParams &params)
            -> std::shared_ptr<VisionBase> {
          return std::make_shared<RTMDet>(params);
        });

    utils::Factory<VisionBase>::instance().registerCreator(
        "Yolov11Det",
        [](const utils::ConstructorParams &params)
            -> std::shared_ptr<VisionBase> {
          return std::make_shared<Yolov11Det>(params);
        });

    utils::Factory<VisionBase>::instance().registerCreator(
        "NanoDet",
        [](const utils::ConstructorParams &params)
            -> std::shared_ptr<VisionBase> {
          return std::make_shared<NanoDet>(params);
        });

    utils::Factory<VisionBase>::instance().registerCreator(
        "SoftmaxCls",
        [](const utils::ConstructorParams &params)
            -> std::shared_ptr<VisionBase> {
          return std::make_shared<SoftmaxCls>(params);
        });

    utils::Factory<VisionBase>::instance().registerCreator(
        "FprCls",
        [](const utils::ConstructorParams &params)
            -> std::shared_ptr<VisionBase> {
          return std::make_shared<FprCls>(params);
        });

    utils::Factory<VisionBase>::instance().registerCreator(
        "FprFeature",
        [](const utils::ConstructorParams &params)
            -> std::shared_ptr<VisionBase> {
          return std::make_shared<FprFeature>(params);
        });
  }
};
} // namespace infer::dnn::vision

#endif