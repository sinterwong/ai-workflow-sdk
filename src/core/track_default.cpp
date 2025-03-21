/**
 * @file track_default.cpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-03-21
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "track_default.hpp"
namespace infer::tracker {
DefaultTrackable::DefaultTrackable(std::shared_ptr<IDetection> initDet,
                                   int trackId, const TrackingConfig &config)
    : id_(trackId), state_(TrackableState::NEW), config_(config) {
  update(initDet);
}

void DefaultTrackable::updateState() {
  float score = calculateScore();

  if (consecutiveMisses_ >= config_.maxConsecutiveMisses) {
    state_ = TrackableState::TERMINATED;
  } else if (score < config_.terminationThreshold) {
    state_ = TrackableState::TERMINATED;
  } else if (score >= config_.activationThreshold) {
    state_ = TrackableState::ACTIVE;
  } else if (score >= config_.inactivationThreshold) {
    state_ = TrackableState::INACTIVE;
  } else {
    state_ = TrackableState::NEW;
  }
}

float DefaultTrackable::calculateScore() const {
  std::lock_guard<std::mutex> guard(mutex_);

  if (history_.empty()) {
    return 0.0f;
  }

  const size_t numSamples = std::min(history_.size(), static_cast<size_t>(5));

  float sum = 0.0f;
  int count = 0;

  auto it = history_.rbegin();
  for (int i = 0; i < numSamples; ++i, ++it) {
    if (*it) {
      sum += (*it)->getScore();
      count++;
    }
  }
  return count > 0 ? sum / count : 0.0f;
}

void DefaultTrackable::update(std::shared_ptr<IDetection> detection) {
  std::lock_guard<std::mutex> guard(mutex_);

  if (detection) {
    // reset missing count when have a detection
    consecutiveMisses_ = 0;

    history_.push_back(detection);

    if (config_.maxHistorySize > 0 &&
        history_.size() > static_cast<size_t>(config_.maxHistorySize)) {
      history_.pop_front();
    }
  } else {
    consecutiveMisses_++;

    if (!history_.empty() && history_.back()) {
      history_.push_back(nullptr);
    }
  }

  updateState();
}

void DefaultTrackable::predict() {
  std::lock_guard<std::mutex> guard(mutex_);
  consecutiveMisses_++;
  updateState();
}

TrackableState DefaultTrackable::getState() const {
  std::lock_guard<std::mutex> guard(mutex_);
  return state_;
}

std::vector<std::shared_ptr<IDetection>> DefaultTrackable::getHistory() const {
  std::lock_guard<std::mutex> guard(mutex_);
  return std::vector<std::shared_ptr<IDetection>>(history_.begin(),
                                                  history_.end());
}

std::shared_ptr<IDetection> DefaultTrackable::getLatestDetection() const {
  std::lock_guard<std::mutex> guard(mutex_);
  if (history_.empty()) {
    return nullptr;
  }
  return history_.back();
}

int DefaultTrackable::getId() const {
  std::lock_guard<std::mutex> guard(mutex_);
  return id_;
}

int DefaultTrackable::getLabel() const {
  std::lock_guard<std::mutex> guard(mutex_);
  if (history_.empty() || !history_.back()) {
    return -1;
  }
  return history_.back()->getLabel();
}

int DefaultTrackable::getMissingCount() const {
  std::lock_guard<std::mutex> guard(mutex_);
  return consecutiveMisses_;
}

} // namespace infer::tracker
