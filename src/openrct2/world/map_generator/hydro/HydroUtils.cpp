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
    /**
     * Prime the given queue with all tiles on the map edges that have the given flag.
     */
    void initHydroFlagQueue(MapGenContext& ctx, TrackingStableTileQueue& queue, HydroFlag flag)
    {
        HydroContext& hydroCtx = ctx.hydroContext.value();

        for (int32_t y = 0; y < ctx.dimensions.y - 1; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (hydroCtx.flags[left].has(flag))
            {
                queue.emplaceAndMark(left, ctx.heightMap[left]);
            }

            const TileCoordsXY right{ ctx.dimensions.x - 1, y };
            if (hydroCtx.flags[right].has(flag))
            {
                queue.emplaceAndMark(right, ctx.heightMap[right]);
            }
        }

        for (int32_t x = 1; x < ctx.dimensions.x - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (hydroCtx.flags[top].has(flag))
            {
                queue.emplaceAndMark(top, ctx.heightMap[top]);
            }

            const TileCoordsXY bottom{ x, ctx.dimensions.y - 1 };
            if (hydroCtx.flags[bottom].has(flag))
            {
                queue.emplaceAndMark(bottom, ctx.heightMap[bottom]);
            }
        }
    }

    /**
     * Count the number of inflows/upstreams for the given tile.
     */
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

    /**
     * Calculate the maximum theoretical catchment a tile could get based on the overscan adjusted map size.
     */
    static float calculateMaxCatchment(const MapGenContext& ctx)
    {
        return ctx.settings.mapSize.x * ctx.overscan * ctx.settings.mapSize.y * ctx.overscan;
    }

    /**
     * Calculate the river width at the given position by scaling the catchment based on the max width and growth exponent
     * settings.
     */
    float riverWidth(const MapGenContext& ctx, const TileCoordsXY& pos)
    {
        const HydroContext& hydroCtx = ctx.hydroContext.value();

        const float catchmentMin = 1.0f;
        const float catchmentMax = calculateMaxCatchment(ctx);

        const float widthMin = 0.0f;
        const float widthMax = ctx.settings.riverWidthMax;

        const float rescaledCatchment = (hydroCtx.catchment[pos] - catchmentMin) / (catchmentMax - catchmentMin);
        const float exponentiatedCatchment = std::pow(
            rescaledCatchment, ctx.settings.riverGrowthExponent * kRiverGrowthExponentScaling);

        return widthMin + exponentiatedCatchment * (widthMax - widthMin);
    }

    /**
     * Determines the river depth based on the width, constants from table 2 in
     *
     * Konsoer, K., Zinger, J. and Parker, G., 2013. Bankfull hydraulic geometry of submarine channels created by turbidity
     * currents: Relations between bankfull channel characteristics and formative flow discharge. Journal of Geophysical
     * Research: Earth Surface, 118(1), pp.216-228.
     */
    float riverDepth(const float width)
    {
        const float depth = std::pow(width / 18.8f, 1.0f / 1.41f);
        return std::max(2.0f, 8.0f * depth); // rescale for rct
    }

    /**
     * Returns a textual summary of the river generation statistics.
     */
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
            "    catchment max theoretical {}\n"
            "    catchment max actual {}\n"
            "    river width max setting {}\n"
            "    river width max actual {}\n",
            calculateMaxCatchment(ctx), stats.flowAggMax, ctx.settings.riverWidthMax,
            (stats.flowAggMax / calculateMaxCatchment(ctx)) * ctx.settings.riverWidthMax);

        const auto pruningSummary = std::format(
            "\n[prune sources]\n"
            "    found {}\n"
            "    removed {}\n"
            "    tiles removed {}\n"
            "    longest {}\n",
            stats.pruneSourcesFound, stats.pruneSourcesFound - stats.pruneSourcesRemaining, stats.pruneSourcesTilesRemoved,
            stats.pruneSourcesLongest);

        const auto widthAdjust = std::format(
            "\n[width adjustment]\n"
            "    river tiles added {}\n",
            stats.widthAdjustNewTiles);

        const auto ensureOrdinal = std::format(
            "\n[ensure ordinal]\n"
            "    river tiles added {}\n",
            stats.ensureOrdinalNewTiles);

        const auto bankIndentationsSummary = std::format(
            "\n[bank indentations]\n"
            "    adjusted {}\n",
            stats.bankIndentationsAdjusted);

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

        return pitSummary + flowSummary + pruningSummary + widthAdjust + ensureOrdinal + bankIndentationsSummary
            + consistencySummary;
    }

} // namespace OpenRCT2::World::MapGenerator::Hydro
