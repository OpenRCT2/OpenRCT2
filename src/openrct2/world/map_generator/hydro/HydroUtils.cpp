/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "HydroUtils.h"

#include "../../../profiling/Profiling.h"
#include "../MapGen.h"
#include "../MapHelpers.h"
#include "../MapTraversalUtils.h"
#include "HydroTypes.hpp"

namespace OpenRCT2::World::MapGenerator::Hydro
{
    void primeHydroFlagHeightQueue(
        MapGenContext& ctx, TrackingStableTileQueue& queue, HydroFlag flag, const QueueInitPosCallback& callback)
    {
        HydroContext& hydroCtx = ctx.hydroContext.value();

        for (int32_t y = 0; y < ctx.dimensions.y - 1; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (hydroCtx.flags[left].has(flag))
            {
                queue.emplaceAndMark(left, ctx.heightMap[left]);
                if (callback.has_value())
                {
                    callback.value()(left);
                }
            }

            const TileCoordsXY right{ ctx.dimensions.x - 1, y };
            if (hydroCtx.flags[right].has(flag))
            {
                queue.emplaceAndMark(right, ctx.heightMap[right]);
                if (callback.has_value())
                {
                    callback.value()(right);
                }
            }
        }

        for (int32_t x = 1; x < ctx.dimensions.x - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (hydroCtx.flags[top].has(flag))
            {
                queue.emplaceAndMark(top, ctx.heightMap[top]);
                if (callback.has_value())
                {
                    callback.value()(top);
                }
            }

            const TileCoordsXY bottom{ x, ctx.dimensions.y - 1 };
            if (hydroCtx.flags[bottom].has(flag))
            {
                queue.emplaceAndMark(bottom, ctx.heightMap[bottom]);
                if (callback.has_value())
                {
                    callback.value()(bottom);
                }
            }
        }
    }

    int8_t countRiverInflows(MapGenContext& ctx, const TileCoordsXY& pos)
    {
        HydroContext& hydroCtx = ctx.hydroContext.value();
        int8_t inflows = 0;
        for (const Neighbour& neighbour : kNeighbours)
        {
            const TileCoordsXY nPos{ pos + neighbour.offset };
            if (hydroCtx.flowsIn[pos].has(neighbour.direction) && hydroCtx.flags[nPos].has(river))
            {
                inflows++;
            }
        }
        return inflows;
    }

    int8_t countRiverOutflows(MapGenContext& ctx, const TileCoordsXY& pos)
    {
        HydroContext& hydroCtx = ctx.hydroContext.value();
        int8_t outflows = 0;
        for (const Neighbour& neighbour : kNeighbours)
        {
            const TileCoordsXY nPos{ pos + neighbour.offset };
            if (hydroCtx.flowsOut[pos].has(neighbour.direction) && hydroCtx.flags[nPos].has(river))
            {
                outflows++;
            }
        }
        return outflows;
    }

    /**
     * Returns the two ordinal neighbours for the given cardinal offset.
     * Uses the game coordinate convention, i.e. the diagonal neighbours are cardinal directions.
     */
    std::array<TileCoordsXY, 2> ordinalNeighbours(const TileCoordsXY& offset)
    {
        return {
            TileCoordsXY{ 0, offset.y },
            TileCoordsXY{ offset.x, 0 },
        };
    }

    /**
     * Checks if the tile at the offset from the given position share an ordinal neighbour with the given flag.
     * Uses the game coordinate convention, i.e. the diagonal neighbours are cardinal directions.
     */
    bool haveCommonOrdinalNeighbour(const HydroContext& hydroCtx, const TileCoordsXY& pos, const TileCoordsXY& offset)
    {
        for (const TileCoordsXY& ordinalOffset : ordinalNeighbours(offset))
        {
            const TileCoordsXY sharedOrdinalPos{ pos + ordinalOffset };
            if (hydroCtx.flags[sharedOrdinalPos].has(river))
            {
                return true;
            }
        }
        return false;
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
            "    sinks tiles removed {}\n"
            "    sources found {}\n"
            "    sources removed {}\n"
            "    sources tiles removed {}\n"
            "    sources longest {}\n",
            stats.pruneSinksFound, stats.pruneSinksTilesRemoved, stats.pruneSourcesFound,
            stats.pruneSourcesFound - stats.pruneSourcesRemaining, stats.pruneSourcesTilesRemoved, stats.pruneSourcesLongest);

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
