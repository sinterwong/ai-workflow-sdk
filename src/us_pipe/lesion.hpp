/**
 * @file lesion.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __US_PIPE_LESION_HPP__
#define __US_PIPE_LESION_HPP__

#include "pipe_types.hpp"

namespace us_pipe {
class Lesion {
public:
  int32_t trackId;
  cv::Rect2i box;
  cv::Rect2i smoothBox;
  int16_t label;
  float score = 0.0f;
  float maxArea = 0.0f;

  int8_t fprModelBirads = -1;
  LesionNotifyStatus notifyStatus = LesionNotifyStatus::NONE;
  float lastDetScore = 0.0f;
  float lastFprScore = 0.0f;

  cv::Mat bestView;

  [[nodiscard]] std::pair<Line, Line> getLongShortDiameter() const noexcept {
    const auto &[x, y, width, height] = box;

    return (width > height)
               ? std::pair{Line{{x, y + height / 2},
                                {x + width, y + height / 2}},
                           Line{{x + width / 2, y},
                                {x + width / 2, y + height}}}
               : std::pair{
                     Line{{x + width / 2, y}, {x + width / 2, y + height}},
                     Line{{x, y + height / 2}, {x + width, y + height / 2}}};
  }

  friend std::ostream &operator<<(std::ostream &os, const Lesion &lesion) {
    os << "trackId: " << lesion.trackId << '\n'
       << "box: [" << lesion.box.x << ", " << lesion.box.y << ", "
       << lesion.box.width << ", " << lesion.box.height << "]\n"
       << "score: " << lesion.score << '\n'
       << "maxArea: " << lesion.maxArea << '\n'
       << "fprModelBirads: " << static_cast<int>(lesion.fprModelBirads) << '\n'
       << "notifyStatus: " << static_cast<int>(lesion.notifyStatus) << '\n'
       << "bestView: " << lesion.bestView.size().width << "x"
       << lesion.bestView.size().height;
    return os;
  }
};
} // namespace us_pipe

#endif
