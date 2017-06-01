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

#include "VerticalTunnelCall.hpp"

uint16 VerticalTunnelCall::GetTunnelHeight(uint16 baseHeight, uint8 *calls) {
    if (calls[0] == 0 && calls[1] == 0 && calls[2] == 0) {
        return 0;
    }

    for (sint16 offset = 0; offset <= 256; offset += 8) {
        if (calls[0] != (baseHeight - 8 + offset) / 16) continue;
        if (calls[1] != (baseHeight + 0 + offset) / 16) continue;
        if (calls[2] != (baseHeight + 8 + offset) / 16) continue;

        return baseHeight + offset;
    }

    log_error("Unknown tunnel height. (%d, %d, %d)", calls[0], calls[1], calls[2]);
    return 0;
}

bool VerticalTunnelCall::HeightIsConsistent(uint16 *heights) {
    for (int i = 1; i < 4; ++i) {
        if (heights[i] != heights[0]) return false;
    }

    return true;
}
