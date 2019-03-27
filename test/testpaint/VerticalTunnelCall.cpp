/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "VerticalTunnelCall.hpp"

uint16_t VerticalTunnelCall::GetTunnelHeight(uint16_t baseHeight, uint8_t* calls)
{
    if (calls[0] == 0 && calls[1] == 0 && calls[2] == 0)
    {
        return 0;
    }

    for (int16_t offset = 0; offset <= 256; offset += 8)
    {
        if (calls[0] != (baseHeight - 8 + offset) / 16)
            continue;
        if (calls[1] != (baseHeight + 0 + offset) / 16)
            continue;
        if (calls[2] != (baseHeight + 8 + offset) / 16)
            continue;

        return baseHeight + offset;
    }

    log_error("Unknown tunnel height. (%d, %d, %d)", calls[0], calls[1], calls[2]);
    return 0;
}

bool VerticalTunnelCall::HeightIsConsistent(uint16_t* heights)
{
    for (int i = 1; i < 4; ++i)
    {
        if (heights[i] != heights[0])
            return false;
    }

    return true;
}
