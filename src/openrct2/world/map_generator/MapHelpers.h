/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Location.hpp"

#include <functional>

namespace OpenRCT2::World::MapGenerator
{
    enum
    {
        SLOPE_S_THRESHOLD_FLAGS = (1 << 0),
        SLOPE_W_THRESHOLD_FLAGS = (1 << 1),
        SLOPE_N_THRESHOLD_FLAGS = (1 << 2),
        SLOPE_E_THRESHOLD_FLAGS = (1 << 3)
    };

    using SmoothFunction = std::function<int32_t(TileCoordsXY)>;

    int32_t smoothTileStrong(TileCoordsXY tileCoord);
    int32_t smoothTileWeak(TileCoordsXY tileCoord);

    void smoothMap(TileCoordsXY mapSize, SmoothFunction smoothFunc);
} // namespace OpenRCT2::World::MapGenerator
