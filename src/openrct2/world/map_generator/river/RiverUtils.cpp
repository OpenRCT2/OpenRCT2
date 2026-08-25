/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RiverUtils.h"

#include "../../../Context.h"
#include "../../../Diagnostic.h"
#include "../../../GameState.h"
#include "../../../PlatformEnvironment.h"
#include "../../../core/Path.hpp"
#include "../../../platform/Platform.h"
#include "../../../profiling/Profiling.h"
#include "../MapGen.h"
#include "../MapGenSerDe.hpp"
#include "../MapHelpers.h"
#include "RiverTypes.hpp"

#include <sstream>
#include <string>

namespace OpenRCT2::World::MapGenerator::River
{
    /**
     * Prime the given queue with all tiles on the map edges that have the given flag.
     */
    void initRiverFlagQueue(MapGenContext& ctx, TrackingStableTileQueue& queue, RiverFlag flag)
    {
        RiverContext& riverCtx = ctx.riverContext.value();

        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (riverCtx.flags[left].has(flag))
            {
                queue.emplaceAndMark(left, ctx.heightMap[left]);
            }

            const TileCoordsXY right{ ctx.dimensions.x - 1, y };
            if (riverCtx.flags[right].has(flag))
            {
                queue.emplaceAndMark(right, ctx.heightMap[right]);
            }
        }

        for (int32_t x = 1; x < ctx.dimensions.x - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (riverCtx.flags[top].has(flag))
            {
                queue.emplaceAndMark(top, ctx.heightMap[top]);
            }

            const TileCoordsXY bottom{ x, ctx.dimensions.y - 1 };
            if (riverCtx.flags[bottom].has(flag))
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
        RiverContext& riverCtx = ctx.riverContext.value();
        int8_t inflows = 0;
        for (const Neighbour& neighbour : kNeighbours)
        {
            const TileCoordsXY nPos{ pos + neighbour.offset };
            if (riverCtx.flowsIn[pos].has(neighbour.direction) && riverCtx.flags[nPos].has(river))
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
    bool haveCommonOrdinalNeighbour(const RiverContext& riverCtx, const TileCoordsXY& pos, const TileCoordsXY& offset)
    {
        for (const TileCoordsXY& ordinalOffset : ordinalNeighbours(offset))
        {
            const TileCoordsXY sharedOrdinalPos{ pos + ordinalOffset };
            if (riverCtx.flags[sharedOrdinalPos].has(river))
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
        const RiverContext& riverCtx = ctx.riverContext.value();

        const float catchmentMin = 1.0f;
        const float catchmentMax = calculateMaxCatchment(ctx);

        const float widthMin = 0.0f;
        const float widthMax = ctx.settings.river.riverWidthMax;

        const float rescaledCatchment = (riverCtx.catchment[pos] - catchmentMin) / (catchmentMax - catchmentMin);
        const float exponentiatedCatchment = std::pow(
            rescaledCatchment, ctx.settings.river.riverGrowthExponent * kRiverGrowthExponentScaling);

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
    std::string summarizeRiverStatistics(const MapGenContext& ctx)
    {
        const auto& stats = ctx.riverContext.value().statistics;
        std::ostringstream oss;

        oss << "\n[breach or fill]\n"
            << "    pits " << stats.pitsFound << "\n"
            << "    breach successes " << stats.pitsBreachSuccess << "\n"
            << "    tiles breached " << stats.pitsBreachedTiles << "\n"
            << "    tiles filled " << stats.pitsFilledTiles << "\n";

        oss << "\n[flow aggregation]\n"
            << "    catchment max theoretical " << calculateMaxCatchment(ctx) << "\n"
            << "    catchment max actual " << stats.flowAggMaxCatchment << "\n"
            << "    river width max setting " << ctx.settings.river.riverWidthMax.get() << "\n"
            << "    river width max actual "
            << ((stats.flowAggMaxCatchment / calculateMaxCatchment(ctx)) * ctx.settings.river.riverWidthMax) << "\n";

        oss << "\n[prune sources]\n"
            << "    found " << stats.pruneSourcesFound << "\n"
            << "    removed " << (stats.pruneSourcesFound - stats.pruneSourcesRemaining) << "\n"
            << "    tiles removed " << stats.pruneSourcesTilesRemoved << "\n"
            << "    longest " << stats.pruneSourcesLongest << "\n";

        oss << "\n[width adjustment]\n"
            << "    river tiles added " << stats.widthAdjustNewTiles << "\n";

        oss << "\n[ensure ordinal]\n"
            << "    river tiles added " << stats.ensureOrdinalNewTiles << "\n";

        oss << "\n[indentations]\n"
            << "    bank tiles adjusted " << stats.bankIndentationsAdjusted << "\n"
            << "    river tiles adjusted " << stats.riverIndentationsAdjusted << "\n";

        oss << "\n[consistency]\n"
            << "    segments raised " << stats.consistencySegmentsRaised << " (max size "
            << stats.consistencySegmentsRaisedMaxSize << ")\n"
            << "    segments lowered " << stats.consistencySegmentsLowered << " (max size "
            << stats.consistencySegmentsLoweredMaxSize << ")\n"
            << "    segments deleted " << stats.consistencySegmentsRemoved << " (max size "
            << stats.consistencySegmentsRemovedMaxSize << ")\n"
            << "    segments iterations " << stats.consistencySegmentsIterations << "\n"
            << "    banks raised " << stats.consistencyBanksRaised << "\n";

        return oss.str();
    }

    /**
     * The root causes should've all been fixed by now but better to keep this around for a while...
     */
    void handleConsistencyRunaway(
        const MapGenContext& ctx, const TileCoordsXY& segment, const TileCoordsXY& pos, const size_t segmentSize,
        const bool lowered)
    {
        auto date = Platform::GetDateLocal();
        auto time = Platform::GetTimeLocal();
        auto& env = GetContext()->GetPlatformEnvironment();
        auto mapgenDir = env.GetDirectoryPath(DirBase::user, DirId::mapgenSettings);
        auto name = std::format(
            "crw_{:04d}-{:02d}-{:02d}_{:02d}-{:02d}-{:02d}.mapgen.json", date.year, date.month, date.day, time.hour,
            time.minute, time.second);
        auto filePath = Path::Combine(mapgenDir, name);

        saveMapgenSettingsToPath(ctx.settings, filePath);

        std::string actionStr = lowered ? "lowered below 0" : "raised above 256";
        auto message = std::format(
            "consistency runaway: ({},{}) of segment ({},{}) size={} {}, settings saved to {}", pos.x, pos.y, segment.x,
            segment.y, segmentSize, actionStr, filePath);

        LOG_FATAL("%s", message.c_str());
        throw std::runtime_error(message);
    }

} // namespace OpenRCT2::World::MapGenerator::River
