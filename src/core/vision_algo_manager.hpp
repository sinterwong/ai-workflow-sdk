/**
 * @file vision_algo_manager.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-04-13
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __CORE_ALGO_MANAGER_HPP_
#define __CORE_ALGO_MANAGER_HPP_
#include "vision_infer.hpp"
#include <memory>
#include <string>
#include <unordered_map>
namespace infer::dnn::vision {
class AlgoManager {
public:
  AlgoManager() = default;
  ~AlgoManager() = default;

  bool regisger(const std::string &name,
                const std::shared_ptr<VisionInfer> &algo);

private:
  std::unordered_map<std::string, dnn::vision::VisionInfer> algoMap;
};

} // namespace infer::dnn::vision

#endif