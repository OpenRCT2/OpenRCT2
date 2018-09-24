/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>

namespace VerticalTunnelCall
{
    uint16_t GetTunnelHeight(uint16_t baseHeight, uint8_t* calls);
    bool HeightIsConsistent(uint16_t* heights);
}; // namespace VerticalTunnelCall
