#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <string>
#include <vector>

#include "FunctionCall.hpp"
#include "SideTunnelCall.hpp"
#include "SegmentSupportHeightCall.hpp"

namespace Printer {
    std::string PrintFunctionCall(function_call call, uint16 baseHeight);
    std::string PrintFunctionCalls(std::vector <function_call> calls, uint16 baseHeight);

    std::string PrintSegmentSupportHeightCalls(std::vector<SegmentSupportCall> calls);

    std::string PrintSideTunnelCalls(TunnelCall tunnelCalls[4][4]);

    std::string PrintHeightOffset(uint16 height, uint16 baseHeight);
}
