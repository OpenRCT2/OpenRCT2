/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapTraversalUtils.h"

#include "MapHelpers.h"

namespace OpenRCT2::World::MapGenerator
{
    void completeDistanceMap(DistanceMap& distanceMap, TrackingStableTileQueue& queue)
    {
        while (!queue.empty())
        {
            QueueTile tile = queue.top();
            queue.pop();

            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };

                const float distance = tile.value
                    + sqrt(neighbour.offset.x * neighbour.offset.x + neighbour.offset.y * neighbour.offset.y);

                if (!distanceMap.inBounds(nPos) || queue.isMarked(nPos) || distance >= distanceMap[nPos])
                {
                    continue;
                }

                distanceMap[nPos] = distance;
                queue.emplaceAndMark(nPos, distance);
            }
        }
    }

    void initZeroDistance(const TileCoordsXY& pos, DistanceMap& distanceMap, TrackingStableTileQueue& queue)
    {
        distanceMap[pos] = 0.0f;
        queue.emplaceAndMark(pos, 0.0f);
    }

    void computeRiverFlagBasedDistanceMap(
        const MapGenContext& ctx, DistanceMap& distanceMap, const River::RiverFlag flag, bool invert)
    {

        distanceMap = DistanceMap{ ctx.dimensions };
        distanceMap.fill(std::numeric_limits<float>::infinity());

        if (!ctx.riverContext.has_value())
        {
            return;
        }

        const auto& riverCtx = ctx.riverContext.value();
        TrackingStableTileQueue queue{ ctx.dimensions };

        for (int32_t y = 0; y < distanceMap.height; y++)
        {
            for (int32_t x = 0; x < distanceMap.width; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (riverCtx.flags[pos].has(flag) == !invert)
                {
                    initZeroDistance(pos, distanceMap, queue);
                }
            }
        }

        completeDistanceMap(distanceMap, queue);
    }

} // namespace OpenRCT2::World::MapGenerator
