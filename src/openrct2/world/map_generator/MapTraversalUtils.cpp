/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapHelpers.h"
#include "MapTraversalUtils.h"

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

                if (!distanceMap.inBounds(nPos) || queue.isMarked(nPos) || distance >= distanceMap[nPos])
                {
                    continue;
                }

                distanceMap[nPos] = distance;
                queue.emplaceAndSetMarked(nPos, distance);
            }
        }
    }

    void completeBackrefMap(BackrefMap& backrefMap, TrackingStableTileQueue& queue)
    {
        while (!queue.empty())
        {
            QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!backrefMap.inBounds(nPos) || queue.isMarked(nPos))
                {
                    continue;
                }

                backrefMap[nPos] = tile.pos;
                queue.emplaceAndSetMarked(nPos, tile.value + 1.0f);
            }
        }
    }

    void initZeroDistance(const TileCoordsXY& pos, DistanceMap& distanceMap, TrackingStableTileQueue& queue)
    {
        distanceMap[pos] = 0.0f;
        queue.emplaceAndSetMarked(pos, 0.0f);
    }

    void computeHydroFlagBasedDistanceMap(const MapGenCtx& context, DistanceMap& distanceMap, const Hydro::HydroFlag flag, bool invert)
    {
        if (!context.hydroMaps.has_value())
        {
            return;
        }

        distanceMap = DistanceMap{context.dimensions};
        distanceMap.fill(std::numeric_limits<float>::infinity());
        const auto& hydroMaps = context.hydroMaps.value();
        TrackingStableTileQueue queue{ context.dimensions };

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

    // assumes backrefMap is sized correctly
    void computeHydroFlagBasedBackrefMap(const MapGenCtx& context, BackrefMap& backrefMap, Hydro::HydroFlag flag)
    {
        if (!context.hydroMaps.has_value())
        {
            return;
        }

        const auto& hydroMaps = context.hydroMaps.value();
        TrackingStableTileQueue queue{ context.dimensions };

        for (int32_t y = 0; y < backrefMap.height; y++)
        {
            for (int32_t x = 0; x < backrefMap.width; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (hydroMaps.flags[pos].has(flag))
                {
                    queue.emplaceAndSetMarked(pos, 0.0f);
                }
            }
        }

        completeBackrefMap(backrefMap, queue);
    }


} // namespace OpenRCT2::World::MapGenerator
