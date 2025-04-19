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

#include <any>
#include <map>
#include <string>
namespace ai_pipe {

enum class PipeErrorCode { SUCCESS = 0, FAILED = -1 };

using PortDataMap = std::map<std::string, std::any>;

} // namespace ai_pipe

#endif
