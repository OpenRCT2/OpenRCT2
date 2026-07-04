/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../Diagnostic.h"
#include "../../../profiling/Profiling.h"
#include "../MapGen.h"
#include "../MapHelpers.h"
#include "../MapTraversalUtils.h"
#include "HydroTypes.hpp"
#include "HydroUtils.h"

namespace OpenRCT2::World::MapGenerator::Hydro
{
    static float getQueueValue(MapGenCtx& context, const TileCoordsXY& pos, QueueMode queueMode)
    {
        switch (queueMode)
        {
            case QueueMode::height:
                return context.heightMap[pos];
            case QueueMode::distance:
                return 0.0f;
        }
        throw std::runtime_error("unsupported queue mode");
    }

    void primeHydroFlagQueue(MapGenCtx& context, TrackingStableTileQueue& queue, const QueueCfg& cfg)
    {
        PROFILED_FUNCTION();
        HydroMaps& hydroMaps = context.hydroMaps.value();

        for (int32_t y = 0; y < context.dimensions.y - 1; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (hydroMaps.flags[left].has(cfg.flag))
            {
                queue.emplaceAndSetMarked(left, getQueueValue(context, left, cfg.mode));
                if (cfg.posCallback.has_value())
                {
                    cfg.posCallback.value()(left);
                }
            }

            const TileCoordsXY right{ context.dimensions.x - 1, y };
            if (hydroMaps.flags[right].has(cfg.flag))
            {
                queue.emplaceAndSetMarked(right, getQueueValue(context, right, cfg.mode));
                if (cfg.posCallback.has_value())
                {
                    cfg.posCallback.value()(right);
                }
            }
        }

        for (int32_t x = 1; x < context.dimensions.x - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (hydroMaps.flags[top].has(cfg.flag))
            {
                queue.emplaceAndSetMarked(top, getQueueValue(context, top, cfg.mode));
                if (cfg.posCallback.has_value())
                {
                    cfg.posCallback.value()(top);
                }
            }

            const TileCoordsXY bottom{ x, context.dimensions.y - 1 };
            if (hydroMaps.flags[bottom].has(cfg.flag))
            {
                queue.emplaceAndSetMarked(bottom, getQueueValue(context, bottom, cfg.mode));
                if (cfg.posCallback.has_value())
                {
                    cfg.posCallback.value()(bottom);
                }
            }
        }
    }

    // TODO can be optimized by keeping the visited set out of the loops in the caller
    static Backref findLateralSetIdentityPos(
        const MapGenCtx& context, const TileCoordsXY& pos, const MapDirectionMaskMap& directionalRefsMap)
    {
        const HydroMaps& hydroMaps = context.hydroMaps.value();

        TileCoordsXY maxHashPos = pos;
        size_t maxHash = TileCoordsXYHash{}(pos);

        TileCoordsXYSet visited;
        std::queue<TileCoordsXY> queue;
        queue.emplace(pos);
        visited.insert(pos);

        while (!queue.empty())
        {
            const TileCoordsXY currentPos{ queue.front() };
            queue.pop();

            if (!directionalRefsMap[currentPos].isEmpty())
            {
                return std::nullopt;
            }

            const size_t currentHash = TileCoordsXYHash{}(currentPos);
            if (currentHash > maxHash)
            {
                maxHashPos = currentPos;
                maxHash = currentHash;
            }

            for (const Neighbour& neighbour : kNeighbours)
            {
                const TileCoordsXY& lateralPos = currentPos + neighbour.offset;
                if (hydroMaps.flowsLateral[currentPos].has(neighbour.direction) && !visited.contains(lateralPos))
                {
                    queue.emplace(lateralPos);
                    visited.insert(lateralPos);
                }
            }
        }

        return maxHashPos;
    }

    void findSourcesAndSinks(MapGenCtx& context, TileCoordsXYSet& sources, TileCoordsXYSet& sinks)
    {
        PROFILED_FUNCTION();

        HydroMaps& hydroMaps = context.hydroMaps.value();

        for (int32_t y = 0; y < context.dimensions.y; y++)
        {
            for (int32_t x = 0; x < context.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (!hydroMaps.flags[pos].has(river))
                {
                    continue;
                }

                auto downstreamCount = 0;
                auto upstreamCount = 0;
                auto lateralCount = 0;

                for (const Neighbour& neighbour : kNeighbours)
                {
                    const TileCoordsXY nPos = pos + neighbour.offset;

                    if (!hydroMaps.flags.inBounds(nPos))
                    {
                        continue;
                    }

                    if (hydroMaps.flowsOut[pos].has(neighbour.direction) && hydroMaps.flags[nPos].has(river))
                    {
                        downstreamCount++;
                    }
                    if (hydroMaps.flowsIn[pos].has(neighbour.direction) && hydroMaps.flags[nPos].has(river))
                    {
                        upstreamCount++;
                    }
                    if (hydroMaps.flowsLateral[pos].has(neighbour.direction) && hydroMaps.flags[nPos].has(river))
                    {
                        lateralCount++;
                    }
                }

                if (upstreamCount == 0)
                {
                    if (lateralCount == 0)
                    {
                        sources.insert(pos);
                    }
                    else
                    {
                        Backref maybeLateralSetSource = findLateralSetIdentityPos(context, pos, hydroMaps.flowsIn);
                        if (maybeLateralSetSource.has_value())
                        {
                            sources.insert(maybeLateralSetSource.value());
                        }
                    }
                }
                else if (downstreamCount == 0 && !hydroMaps.flags.onEdge(pos))
                {
                    if (lateralCount == 0)
                    {
                        sinks.insert(pos);
                    }
                    else
                    {
                        Backref maybeLateralSetSource = findLateralSetIdentityPos(context, pos, hydroMaps.flowsOut);
                        if (maybeLateralSetSource.has_value())
                        {
                            sources.insert(maybeLateralSetSource.value());
                        }
                    }
                }
            }
        }
    }

} // namespace OpenRCT2::World::MapGenerator::Hydro
