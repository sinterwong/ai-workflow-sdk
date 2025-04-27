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
#ifndef __ALGO_REGISTRAR_HPP__
#define __ALGO_REGISTRAR_HPP__

#include "algo_infer_base.hpp"
#include "utils/type_safe_factory.hpp"
#include "vision_infer.hpp"
#include "vision_registrar.hpp"

namespace infer::dnn {
class AlgoRegistrar {
public:
  static AlgoRegistrar &getInstance() {
    static AlgoRegistrar instance;
    return instance;
  }

  AlgoRegistrar(const AlgoRegistrar &) = delete;
  AlgoRegistrar &operator=(const AlgoRegistrar &) = delete;
  AlgoRegistrar(AlgoRegistrar &&) = delete;
  AlgoRegistrar &operator=(AlgoRegistrar &&) = delete;

private:
  AlgoRegistrar() {
    vision::VisionRegistrar::getInstance();

    utils::Factory<AlgoInferBase>::instance().registerCreator(
        "VisionInfer",
        [](const utils::ConstructorParams &params)
            -> std::shared_ptr<AlgoInferBase> {
          auto moduleName = utils::get_param<std::string>(params, "moduleName");
          auto inferParam =
              utils::get_param<AlgoInferParams>(params, "inferParams");
          auto postproc =
              utils::get_param<AlgoPostprocParams>(params, "postProcParams");
          return std::make_shared<vision::VisionInfer>(moduleName, inferParam,
                                                       postproc);
        });
  }
};
} // namespace infer::dnn

#endif