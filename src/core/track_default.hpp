/**
 * @file track_default.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-03-21
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __CORE_TRACKER_DEFAULT_HPP_
#define __CORE_TRACKER_DEFAULT_HPP_

#include "track_core.hpp"
#include "track_types.hpp"

namespace infer::tracker {
class DefaultTrackable : public ITrackable {
public:
  DefaultTrackable(std::shared_ptr<IDetection> initDet, int trackId,
                   const TrackingConfig &config);

  virtual ~DefaultTrackable() = default;

  virtual void update(std::shared_ptr<IDetection> detection) override;

  virtual void predict() override;

  virtual TrackableState getState() const override;

  virtual std::vector<std::shared_ptr<IDetection>> getHistory() const override;

  virtual std::shared_ptr<IDetection> getLatestDetection() const override;

  virtual int getId() const override;

  virtual int getLabel() const override;

  virtual float calculateScore() const override;

  virtual int getMissingCount() const override;

private:
  void updateState();

private:
  int id_;
  TrackableState state_;
  std::deque<std::shared_ptr<IDetection>> history_;
  int consecutiveMisses_ = 0;
  TrackingConfig config_;

  mutable std::mutex mutex_;
};
} // namespace infer::tracker

#endif