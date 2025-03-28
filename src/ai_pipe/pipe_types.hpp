/**
 * @file pipe_types.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __PIPE_TYPES_HPP__
#define __PIPE_TYPES_HPP__

#include "core/infer_types.hpp"
namespace ai_pipe {

enum class PipeErrorCode { SUCCESS = 0, FAILED = -1 };

struct LesionDetResult {
  infer::BBox bbox;
  int detIndex;
};

struct Patch {
  int frameIndex;
  LesionDetResult lesionDetResult;
};

struct Tracklet {
  Patch patch;
  bool active;
  bool finished;
};

} // namespace ai_pipe

#endif
