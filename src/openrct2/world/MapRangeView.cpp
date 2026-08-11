/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapRangeView.hpp"

#include "../GameState.h"

namespace OpenRCT2::Map
{
    MapRangeView getClampedRange(const TileCoordsXY& posA, const TileCoordsXY& posB)
    {
        constexpr auto clampTileCoord = [](int32_t coord, int32_t numTiles) -> int32_t {
            return std::clamp<int32_t>(coord, 0, numTiles - 1);
        };

        auto mapSize = getGameState().mapSize;
        auto clampedA = TileCoordsXY(clampTileCoord(posA.x, mapSize.x), clampTileCoord(posA.y, mapSize.y));
        auto clampedB = TileCoordsXY(clampTileCoord(posB.x, mapSize.x), clampTileCoord(posB.y, mapSize.y));
        return MapRangeView(clampedA, clampedB);
    }

    MapRangeView getClampedRange(const CoordsXY& posA, const CoordsXY& posB)
    {
        constexpr auto clampCoord = [](int32_t coord, int32_t numTiles) -> int32_t {
            return std::clamp<int32_t>(coord, 0, (numTiles * kCoordsXYStep) - 1);
        };

        auto mapSize = getGameState().mapSize;
        auto clampedA = CoordsXY(clampCoord(posA.x, mapSize.x), clampCoord(posA.y, mapSize.y));
        auto clampedB = CoordsXY(clampCoord(posB.x, mapSize.x), clampCoord(posB.y, mapSize.y));
        return MapRangeView(TileCoordsXY(clampedA), TileCoordsXY(clampedB));
    }

    MapRangeView getClampedRange(const MapRange& range)
    {
        return getClampedRange(range.Point1, range.Point2);
    }

    MapRangeView getDrawableTileRange()
    {
        auto mapSize = getGameState().mapSize;
        return MapRangeView({ 1, 1 }, { mapSize.x - 2, mapSize.y - 2 });
    }

    MapRangeView getWorldRange()
    {
        auto mapSize = getGameState().mapSize;
        return MapRangeView({ 0, 0 }, { mapSize.x - 1, mapSize.y - 1 });
    }
} // namespace OpenRCT2::Map
