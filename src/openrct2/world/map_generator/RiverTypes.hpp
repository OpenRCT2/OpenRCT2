/*****************************************************************************
* Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "BaseMap.hpp"


namespace OpenRCT2::World::MapGenerator
{
    struct MapGenCtx;

    struct RiverState
    {
        float catchment;
        bool isRiver;
        bool isRiverbed;
        bool isFilled;
        bool isBreached;
    };

    using RiverMap = BaseMap<RiverState>;

} // namespace OpenRCT2::World::MapGenerator
