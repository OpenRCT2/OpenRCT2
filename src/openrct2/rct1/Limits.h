/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
    constexpr const uint16_t MaxTileElements = 0xC000;
    constexpr const uint16_t MaxEntities = 5000;
    constexpr const uint8_t MaxTrainsPerRide = 12;
    constexpr const uint8_t MaxMapSize = 128;
    constexpr const uint8_t MaxStaff = 116;
    constexpr const uint16_t MaxAnimatedObjects = 1000;
    constexpr const uint8_t MaxBanners = 100;
    constexpr int32_t CoordsZStep = 4;
    constexpr const uint32_t Num_LL_CSG_Entries = 69917;
    constexpr const uint32_t LL_CSG1_DAT_FileSize = 41402869;
    constexpr const uint32_t NumTerrainSurfaces = 16;
    constexpr const uint32_t NumTerrainEdges = 15;
} // namespace RCT1::Limits
