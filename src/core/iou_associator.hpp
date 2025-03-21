/**
 * @file iou_associator.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-03-21
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __CORE_TRACKER_IOU_ASSOCIATOR_HPP_
#define __CORE_TRACKER_IOU_ASSOCIATOR_HPP_

#include "track_core.hpp"
#include "track_types.hpp"
#include <vector>

namespace infer::tracker {
class IoUAssociator : public IAssociator {
public:
  explicit IoUAssociator(float iouThreshold = 0.5f);

  AssociationResult associate(
      const std::vector<std::shared_ptr<ITrackable>> &trackables,
      const std::vector<std::shared_ptr<IDetection>> &detections) override;

private:
  float iouThreshold_;
};
} // namespace infer::tracker

#endif
