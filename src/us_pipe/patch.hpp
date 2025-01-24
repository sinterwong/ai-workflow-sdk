/**
 * @file patch.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __TRACKER_PATCH_HPP__
#define __TRACKER_PATCH_HPP__

#include "core/infer_types.hpp"
#include "pipe_types.hpp"

namespace us_pipe::tracker {
template <typename DiagRes> class Patch {
public:
  int frameIndex;
  infer::BBox detRet;
  std::optional<int> detIndex;
  std::optional<LesionFprResult> fprRes;
  std::optional<DiagRes> diagRet;
  LesionNotifyStatus notify_status = LesionNotifyStatus::NONE;
  cv::Rect sBBox;
  cv::Mat view;

  Patch(int frameIndex, const infer::BBox &detRet,
        std::optional<int> det_index = std::nullopt)
      : frameIndex(frameIndex), detRet(detRet), detIndex(detIndex),
        sBBox(detRet.rect) {}

  [[nodiscard]] float score() const noexcept {
    return (fprRes ? fprRes->clsRet.score : 0.0f) + detRet.score;
  }

  [[nodiscard]] float area() const noexcept {
    return static_cast<float>(detRet.rect.area());
  }
};

} // namespace us_pipe::tracker

#endif
