/*****************************************************************************
* Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/FlagHolder.hpp"
#include "BaseMap.hpp"

namespace OpenRCT2::World::MapGenerator
{
    struct MapGenCtx;

    enum HydroFlag : uint8_t
    {
        river,
        riverbed,
        filled,
        breached,
        skeleton
    };

    using HydroFlags = FlagHolder<uint8_t, HydroFlag>;

    struct HydroMaps
    {
        BaseMap<float> catchment;
        BaseMap<float> height;
        BaseMap<HydroFlags> flags;
        TileCoordsXY dimensions;

        HydroMaps(const TileCoordsXY size)
            : catchment(size)
            , height(size)
            , flags(size)
            , dimensions(size)
        {}
    };

} // namespace OpenRCT2::World::MapGenerator
