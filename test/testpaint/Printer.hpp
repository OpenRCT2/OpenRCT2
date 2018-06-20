/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <string>
#include <vector>

#include "FunctionCall.hpp"
#include "SideTunnelCall.hpp"
#include "SegmentSupportHeightCall.hpp"

namespace Printer {
    std::string PrintFunctionCall(function_call call, uint16_t baseHeight);
    std::string PrintFunctionCalls(std::vector <function_call> calls, uint16_t baseHeight);

    std::string PrintSegmentSupportHeightCalls(std::vector<SegmentSupportCall> calls);

    std::string PrintSideTunnelCalls(TunnelCall tunnelCalls[4][4]);

    std::string PrintHeightOffset(uint16_t height, uint16_t baseHeight);
}
