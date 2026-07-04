/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "HydroUtils.h"

#include "../../../Diagnostic.h"
#include "../../../profiling/Profiling.h"
#include "../MapGen.h"
#include "../MapHelpers.h"
#include "../MapTraversalUtils.h"
#include "HydroTypes.hpp"

namespace OpenRCT2::World::MapGenerator::Hydro
{
    static float getQueueValue(MapGenContext& ctx, const TileCoordsXY& pos, QueueMode queueMode)
    {
        switch (queueMode)
        {
            case QueueMode::height:
                return ctx.heightMap[pos];
            case QueueMode::distance:
                return 0.0f;
        }
        throw std::runtime_error("unsupported queue mode");
    }

    void primeHydroFlagQueue(MapGenContext& ctx, TrackingStableTileQueue& queue, const QueueCfg& cfg)
    {
        PROFILED_FUNCTION();
        HydroContext& hydroCtx = ctx.hydroContext.value();

        for (int32_t y = 0; y < ctx.dimensions.y - 1; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (hydroCtx.flags[left].has(cfg.flag))
            {
                queue.emplaceAndSetMarked(left, getQueueValue(ctx, left, cfg.mode));
                if (cfg.posCallback.has_value())
                {
                    cfg.posCallback.value()(left);
                }
            }

            const TileCoordsXY right{ ctx.dimensions.x - 1, y };
            if (hydroCtx.flags[right].has(cfg.flag))
            {
                queue.emplaceAndSetMarked(right, getQueueValue(ctx, right, cfg.mode));
                if (cfg.posCallback.has_value())
                {
                    cfg.posCallback.value()(right);
                }
            }
        }

        for (int32_t x = 1; x < ctx.dimensions.x - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (hydroCtx.flags[top].has(cfg.flag))
            {
                queue.emplaceAndSetMarked(top, getQueueValue(ctx, top, cfg.mode));
                if (cfg.posCallback.has_value())
                {
                    cfg.posCallback.value()(top);
                }
            }

            const TileCoordsXY bottom{ x, ctx.dimensions.y - 1 };
            if (hydroCtx.flags[bottom].has(cfg.flag))
            {
                queue.emplaceAndSetMarked(bottom, getQueueValue(ctx, bottom, cfg.mode));
                if (cfg.posCallback.has_value())
                {
                    cfg.posCallback.value()(bottom);
                }
            }
        }
    }

    // TODO can be optimized by keeping the visited set out of the loops in the caller
    static Backref findLateralSetIdentityPos(
        const MapGenContext& ctx, const TileCoordsXY& pos, const MapDirectionMaskMap& directionalRefsMap)
    {
        const HydroContext& hydroCtx = ctx.hydroContext.value();

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
                if (hydroCtx.flowsLateral[currentPos].has(neighbour.direction) && !visited.contains(lateralPos))
                {
                    queue.emplace(lateralPos);
                    visited.insert(lateralPos);
                }
            }
        }

        return maxHashPos;
    }

    void findSourcesAndSinks(MapGenContext& ctx, TileCoordsXYSet& sources, TileCoordsXYSet& sinks)
    {
        PROFILED_FUNCTION();

        HydroContext& hydroCtx = ctx.hydroContext.value();

        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (!hydroCtx.flags[pos].has(river))
                {
                    continue;
                }

                auto downstreamCount = 0;
                auto upstreamCount = 0;
                auto lateralCount = 0;

                for (const Neighbour& neighbour : kNeighbours)
                {
                    const TileCoordsXY nPos = pos + neighbour.offset;

                    if (!hydroCtx.flags.inBounds(nPos))
                    {
                        continue;
                    }

                    if (hydroCtx.flowsOut[pos].has(neighbour.direction) && hydroCtx.flags[nPos].has(river))
                    {
                        downstreamCount++;
                    }
                    if (hydroCtx.flowsIn[pos].has(neighbour.direction) && hydroCtx.flags[nPos].has(river))
                    {
                        upstreamCount++;
                    }
                    if (hydroCtx.flowsLateral[pos].has(neighbour.direction) && hydroCtx.flags[nPos].has(river))
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
                        Backref maybeLateralSetSource = findLateralSetIdentityPos(ctx, pos, hydroCtx.flowsIn);
                        if (maybeLateralSetSource.has_value())
                        {
                            sources.insert(maybeLateralSetSource.value());
                        }
                    }
                }
                else if (downstreamCount == 0 && !hydroCtx.flags.onEdge(pos))
                {
                    if (lateralCount == 0)
                    {
                        sinks.insert(pos);
                    }
                    else
                    {
                        Backref maybeLateralSetSource = findLateralSetIdentityPos(ctx, pos, hydroCtx.flowsOut);
                        if (maybeLateralSetSource.has_value())
                        {
                            sources.insert(maybeLateralSetSource.value());
                        }
                    }
                }
            }
        }
    }

    std::string summarizeHydroStatistics(const MapGenContext& ctx)
    {
        const auto& stats = ctx.hydroContext.value().stats;

        const auto pitSummary = std::format(
            "\n[breach or fill]\n"
            "    pits {}\n"
            "    breach successes {}\n"
            "    tiles breached {}\n"
            "    tiles filled {}\n",
            stats.pitsFound, stats.pitsBreachSuccess, stats.pitsBreachedTiles, stats.pitsFilledTiles);

        const auto flowSummary = std::format(
            "\n[flow aggregation]\n"
            "    catchment max {}\n",
            stats.flowAggMax);

        const auto pruningSummary = std::format(
            "\n[pruning]\n"
            "    sinks removed {}\n"
            "    sources removed {} of {}\n"
            "    iterations {}\n"
            "    deadlocks resolved {}\n",
            stats.pruneSinksFound, stats.pruneSourcesRemoved, stats.pruneSourcesFound, stats.pruneIterations,
            stats.pruneDeadlocks);

        const auto widthAdjust = std::format(
            "\n[width adjustment]\n"
            "    river tiles added {}\n",
            stats.widthAdjustNewTiles);

        const auto ensureCardinal = std::format(
            "\n[ensure cardinal]\n"
            "    river tiles added {}\n",
            stats.ensureCardinalNewTiles);

        const auto bankIndentationsSummary = std::format(
            "\n[bank indentations]\n"
            "    removed {}\n",
            stats.bankIndentationsRemoved);

        const auto consistencySummary = std::format(
            "\n[consistency]\n"
            "    segments raised {} (max size {})\n"
            "    segments lowered {} (max size {})\n"
            "    segments deleted {} (max size {})\n"
            "    segments iterations {}\n"
            "    banks raised {}\n",
            stats.consistencySegmentsRaised, stats.consistencySegmentsRaisedMaxSize, stats.consistencySegmentsLowered,
            stats.consistencySegmentsLoweredMaxSize, stats.consistencySegmentsRemoved, stats.consistencySegmentsRemovedMaxSize,
            stats.consistencySegmentsIterations, stats.consistencyBanksRaised);

        return pitSummary + flowSummary + pruningSummary + widthAdjust + ensureCardinal + bankIndentationsSummary
            + consistencySummary;
    }

} // namespace OpenRCT2::World::MapGenerator::Hydro
