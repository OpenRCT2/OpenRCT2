/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/


#include "DistanceMapUtils.h"

#include "MapHelpers.h"

namespace OpenRCT2::World::MapGenerator
{
    void completeDistanceMap(DistanceMap& distanceMap, TrackingStableTileQueue& queue)
    {
        while (!queue.empty())
        {
            QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                const float distance = tile.value + sqrt(offset.x * offset.x + offset.y * offset.y);

                if (!distanceMap.inBounds(nPos) || queue.visited(nPos) || distance >= distanceMap[nPos])
                {
                    continue;
                }

                distanceMap[nPos] = distance;
                queue.emplaceAndVisit(nPos, distance);
            }
        }
    }

    void initZeroDistance(const TileCoordsXY& pos, DistanceMap& distanceMap, TrackingStableTileQueue& queue)
    {
        distanceMap[pos] = 0.0f;
        queue.emplaceAndVisit(pos, 0.0f);
    }

    void computeHydroFlagBasedDistanceMap(const MapGenCtx& genCtx, DistanceMap& distanceMap, const Hydro::HydroFlag flag, bool invert)
    {
        distanceMap = DistanceMap{ genCtx.heightMap.width, genCtx.heightMap.height };
        distanceMap.fill(std::numeric_limits<float>::infinity());

        if (!genCtx.hydroMaps.has_value())
        {
            return;
        }

        const auto& hydroMaps = genCtx.hydroMaps.value();
        TrackingStableTileQueue queue {genCtx.dimensions};

        for (int32_t y = 0; y < distanceMap.height; y++)
        {
            for (int32_t x = 0; x < distanceMap.width; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (hydroMaps.flags[pos].has(flag) == !invert)
                {
                    initZeroDistance(pos, distanceMap, queue);
                }
            }
        }

        completeDistanceMap(distanceMap, queue);
    }
} // namespace OpenRCT2::World::MapGenerator
