/**
 * @file track.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __TRACK_BASE_HPP__
#define __TRACK_BASE_HPP__

#include "lesion.hpp"
#include "patch.hpp"
#include "pipe_types.hpp"
#include "thy_types.hpp"

namespace us_pipe::tracker {

struct TrackerArgs {
  /*
      和LesionNotifyStatus对应。See Tracker.update_tracklet_status
      ACTIVE和DISPLAY决定了最终输出的算法Lesion结果，其用于精度评估。
      ALIVE仅仅是为了维持track不要死掉，后续还能参与detection的匹配。不参与精度的评估。
      STAGE1: det_result score够大时，对应到WEAK_PASS_DET_THRESH。
  */

  float iou_thresh;

  float active_thresh;  // STRONG_PASS
  float display_thresh; // WEAK_PASS_TRACK_THRESH
  float finish_thresh;
  float stage1_thresh; // weak_display

  float det_display_thresh;

  int max_patch_number_per_track; // C++ 特有, 用来删老的 track
  bool allow_no_det = false;
  int no_det_limit =
      5; // 连续几帧没检测到病灶, 则将track.finish=true, 之前默认是5;
};

template <class LesionT, class DiagResT> struct TrackBase {
  static inline std::atomic<int> count = 0;

  std::vector<Patch<DiagResT>> patches;
  int tid;
  bool finished = false;
  bool active = false;
  float maxPatchArea = 0.0f;
  cv::Mat bestView;
  cv::Rect lastSmoothBbox;

  ThyroidBreastParams domainParams;

  explicit TrackBase(Patch<DiagResT> &&patch) : tid(count++) {
    patches.reserve(64);
    patches.push_back(std::move(patch));
    updateMaxArea();
  }

  [[nodiscard]] const Patch<DiagResT> &refPatch() const {
    assert(!patches.empty() && "Patches must not be empty");
    return patches.back();
  }

  [[nodiscard]] float score() const noexcept {
    const auto n = patches.size();
    return (n == 1) ? patches[0].score()
                    : 0.5f * (patches[n - 1].score() + patches[n - 2].score());
  }

  void append(Patch<DiagResT> &&patch) {
    patches.push_back(std::move(patch));
    updateMaxArea();
  }

  void inheritPreviousDetection(int newFrameIndex) {
    const auto &prevDet = refPatch().detResult;
    append({newFrameIndex, infer::BBox{prevDet.bbox, 0.0f, prevDet.label}});
  }

  [[nodiscard]] std::size_t consecutiveZeroDetCount() const noexcept {
    std::size_t cnt = 0;
    for (auto it = patches.rbegin(); it != patches.rend(); ++it) {
      if (std::abs(it->detResult.score) < EPSILON)
        ++cnt;
      else
        break;
    }
    return cnt;
  }

  [[nodiscard]] Lesion summary() const {
    const auto &lastPatch = refPatch();

    int fprBirads = -1;
    float lastFprScore = 0.0f;
    if (lastPatch.fprRes) {
      fprBirads = lastPatch.fprRes->clsBirads;
      lastFprScore = lastPatch.fprRes->clsResult.score;
    }

    return Lesion{tid,
                  lastPatch.detResult.bbox,
                  lastPatch.smoothBbox,
                  lastPatch.detResult.label,
                  score(),
                  maxPatchArea,
                  fprBirads,
                  lastPatch.notifyStatus,
                  lastPatch.detResult.score,
                  lastFprScore,
                  bestView};
  }

private:
  void updateMaxArea() noexcept {
    const float curArea = patches.back().area();
    if (curArea > maxPatchArea) {
      maxPatchArea = curArea;
      if (!patches.back().view.empty()) {
        bestView = patches.back().view.clone();
      }
    }
  }
};
} // namespace us_pipe::tracker

#endif
