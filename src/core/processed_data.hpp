/**
 * @file processed_data.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-02-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __CORE_INFER_PROCESSED_DATA_HPP_
#define __CORE_INFER_PROCESSED_DATA_HPP_
#include "infer_types.hpp"
namespace infer {
struct PreprocessedData {
  DataType dataType;
  std::vector<std::vector<uint8_t>> data; // raw data
  std::vector<size_t> elementCounts;

  template <typename T> std::vector<const T *> getTypedPtrs() const {
    std::vector<const T *> result;
    result.reserve(data.size());
    for (const auto &buffer : data) {
      result.push_back(reinterpret_cast<const T *>(buffer.data()));
    }
    return result;
  }

  std::vector<size_t> getElementCounts() const {
    if (!elementCounts.empty()) {
      return elementCounts;
    }

    std::vector<size_t> counts;
    counts.reserve(data.size());
    size_t elemSize = getElementSize(dataType);

    for (const auto &buffer : data) {
      counts.push_back(buffer.size() / elemSize);
    }
    return counts;
  }

  static size_t getElementSize(DataType type) {
    switch (type) {
    case DataType::FLOAT32:
      return sizeof(float);
    case DataType::FLOAT16:
      return sizeof(uint16_t);
    case DataType::INT8:
      return sizeof(int8_t);
    default:
      return 0;
    }
  }
};

} // namespace infer
#endif