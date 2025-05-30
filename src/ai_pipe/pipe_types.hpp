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

#include "utils/data_packet.hpp"

namespace ai_pipe {

enum class PipeErrorCode { SUCCESS = 0, FAILED = -1 };

using ::utils::DataPacket;

using PortDataMap = std::map<std::string, DataPacket>;

} // namespace ai_pipe

#endif
