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
#include "algo_infer_base.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace infer::dnn {
class AlgoManager : public std::enable_shared_from_this<AlgoManager> {
public:
  AlgoManager() = default;
  ~AlgoManager() = default;

  bool regisger(const std::string &name,
                const std::shared_ptr<AlgoInferBase> &algo);

private:
  std::unordered_map<std::string, AlgoInferBase> algoMap;
};

} // namespace infer::dnn

#endif