/**
 * @file generic_tracker.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-03-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __AI_PIPE_GENERIC_TRACKER_HPP__
#define __AI_PIPE_GENERIC_TRACKER_HPP__

#include "core/track_core.hpp"
#include <atomic>

namespace ai_pipe::tracking {
using namespace infer::tracker;

template <typename DetectionType> class GenericTracker {
public:
  explicit GenericTracker(const TrackingConfig &config = TrackingConfig());

  void reset();

  void track(const std::vector<DetectionType> &detections, int frameIndex);

  std::vector<std::shared_ptr<ITrackable>> getTrackables() const;

  std::vector<std::shared_ptr<ITrackable>> getActiveTrackables() const;

  std::vector<std::shared_ptr<ITrackable>> getValidTrackables() const;

  size_t getTrackableCount() const;

  void configure(const TrackingConfig &config);

  const TrackingConfig &getConfiguration() const;

  void setAssociator(std::unique_ptr<IAssociator> associator);

protected:
  virtual std::shared_ptr<IDetection>
  createDetection(const DetectionType &detection, int frameIndex) = 0;

private:
  void removeDuplicatedTrackables();

  void cleanTerminatedTrackables();

private:
  TrackingConfig config_;

  std::vector<std::shared_ptr<ITrackable>> trackables_;

  std::unique_ptr<IAssociator> associator_;

  std::atomic<int> nextTrackableId_;

  mutable std::mutex mutex_;
};
} // namespace ai_pipe::tracking

#endif
