/**
 * @file pipe_base.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-02-09
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __PIPE_BASE_HPP__
#define __PIPE_BASE_HPP__

#include "pipe_types.hpp"

namespace ai_pipe {

class PipeBase {
public:
  PipeBase();
  virtual ~PipeBase();

  virtual PipeErrorCode initialize() = 0;
  virtual PipeErrorCode process() = 0;
  virtual PipeErrorCode terminate() = 0;
};

} // namespace ai_pipe

#endif
