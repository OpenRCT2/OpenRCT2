/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "../rct12/Limits.h"

namespace RCT1::Limits
{
    using namespace RCT12::Limits;
    constexpr const uint16_t RCT1_MAX_TILE_ELEMENTS = 0xC000;
    constexpr const uint16_t RCT1_MAX_SPRITES = 5000;
    constexpr const uint8_t RCT1_MAX_TRAINS_PER_RIDE = 12;
    constexpr const uint8_t RCT1_MAX_MAP_SIZE = 128;
    constexpr const uint8_t RCT1_MAX_STAFF = 116;
    constexpr const uint16_t RCT1_MAX_ANIMATED_OBJECTS = 1000;
    constexpr const uint8_t RCT1_MAX_BANNERS = 100;
    constexpr int32_t RCT1_COORDS_Z_STEP = 4;
    constexpr const uint32_t RCT1_NUM_LL_CSG_ENTRIES = 69917;
    constexpr const uint32_t RCT1_LL_CSG1_DAT_FILE_SIZE = 41402869;
    constexpr const uint32_t RCT1_NUM_TERRAIN_SURFACES = 16;
    constexpr const uint32_t RCT1_NUM_TERRAIN_EDGES = 15;
} // namespace RCT1::Limits
