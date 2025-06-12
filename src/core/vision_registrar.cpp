/**
 * @file vision_registrar.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-06-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "vision_registrar.hpp"

#include "fpr_cls.hpp"
#include "fpr_feat.hpp"
#include "nano_det.hpp"
#include "rtm_det.hpp"
#include "softmax_cls.hpp"
#include "utils/type_safe_factory.hpp"
#include "vision.hpp"
#include "yolo_det.hpp"

#include "logger/logger.hpp"

namespace infer::dnn::vision {
VisionRegistrar::VisionRegistrar() {
  VisionFactory::instance().registerCreator(
      "RTMDet",
      [](const AlgoConstructParams &cparams) -> std::shared_ptr<VisionBase> {
        auto params = cparams.getParam<AlgoPostprocParams>("params");
        return std::make_shared<RTMDet>(params);
      });
  LOG_INFOS << "Registered RTMDet creator.";

  VisionFactory::instance().registerCreator(
      "Yolov11Det",
      [](const AlgoConstructParams &cparams) -> std::shared_ptr<VisionBase> {
        auto params = cparams.getParam<AlgoPostprocParams>("params");
        return std::make_shared<Yolov11Det>(params);
      });
  LOG_INFOS << "Registered Yolov11Det creator.";

  VisionFactory::instance().registerCreator(
      "NanoDet",
      [](const AlgoConstructParams &cparams) -> std::shared_ptr<VisionBase> {
        auto params = cparams.getParam<AlgoPostprocParams>("params");
        return std::make_shared<NanoDet>(params);
      });
  LOG_INFOS << "Registered NanoDet creator.";

  VisionFactory::instance().registerCreator(
      "SoftmaxCls",
      [](const AlgoConstructParams &cparams) -> std::shared_ptr<VisionBase> {
        auto params = cparams.getParam<AlgoPostprocParams>("params");
        return std::make_shared<SoftmaxCls>(params);
      });
  LOG_INFOS << "Registered SoftmaxCls creator.";

  VisionFactory::instance().registerCreator(
      "FprCls",
      [](const AlgoConstructParams &cparams) -> std::shared_ptr<VisionBase> {
        auto params = cparams.getParam<AlgoPostprocParams>("params");
        return std::make_shared<FprCls>(params);
      });
  LOG_INFOS << "Registered FprCls creator.";

  VisionFactory::instance().registerCreator(
      "FprFeature",
      [](const AlgoConstructParams &cparams) -> std::shared_ptr<VisionBase> {
        auto params = cparams.getParam<AlgoPostprocParams>("params");
        return std::make_shared<FprFeature>(params);
      });
  LOG_INFOS << "Registered FprFeature creator." << std::endl;
}
} // namespace infer::dnn::vision
