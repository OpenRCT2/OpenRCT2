/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "River.h"

#include "../../../Context.h"
#include "../../../Diagnostic.h"
#include "../../../GameState.h"
#include "../../../profiling/Profiling.h"
#include "../BaseMap.hpp"
#include "../MapHelpers.h"
#include "../TileQueue.hpp"
#include "HydroUtils.h"

#include <format>
#include <ranges>

namespace OpenRCT2::World::MapGenerator::Hydro
{
    static constexpr float kP = 1.1f;

    /**
     * Fill up or breach out of depressions in the heightmap to ensure there is a monotonic downhill path for rivers to follow.
     *
     * based on
     *
     * Lindsay, J.B., 2016. Efficient hybrid breaching‐filling sink removal methods for flow path enforcement in digital
     * elevation models. Hydrological Processes, 30(6), pp.846-857.
     */
    static void fillOrBreachDepressions(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();
        HeightMap& heightMap = ctx.heightMap;
        HydroContext& hydroCtx = ctx.hydroContext.value();
        BooleanMap pitMap{ ctx.dimensions };
        ReferenceMap backrefMap{ ctx.dimensions };
        TrackingStableTileQueue queue{ ctx.dimensions };
        std::queue<TileCoordsXY> fillQueue;

        // prepare queue and mark pits
        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (heightMap.onEdge(pos))
                {
                    queue.emplaceAndMark(pos, heightMap[pos]);
                    continue;
                }

                float minNeighbour = std::numeric_limits<float>::infinity();
                for (const auto& neighbour : kNeighbours)
                {
                    const TileCoordsXY nPos{ pos + neighbour.offset }; // no bounds check needed, if-clause above
                    minNeighbour = std::min(heightMap[nPos], minNeighbour);
                }

                // this is a pit, raise to just below the lowest neighbour to shorten/shallow potential breach path
                // TODO doesn't this fail to detect a pit of equal height neighbours? Fill will take care of it I guess..
                if (heightMap[pos] < minNeighbour)
                {
                    heightMap[pos] = std::nextafter(minNeighbour, std::numeric_limits<float>::lowest());
                    pitMap[pos] = true;
                    hydroCtx.stats.pitsFound++;
                }
            }
        }

        // process lowest tile from queue, moving inward from map edges and building up a 'lowest path' graph
        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };
                if (!heightMap.inBounds(nPos) || queue.isMarked(nPos))
                {
                    continue;
                }

                queue.emplaceAndMark(nPos, heightMap[nPos]);
                fillQueue.push(nPos);
                backrefMap[nPos] = tile.pos;

                // check if there is a valid path to breach out of the pit
                if (pitMap[nPos])
                {
                    float pathLength = 0.0f;
                    float pathDepth = std::numeric_limits<float>::lowest();

                    std::optional<TileCoordsXY> currentTile = std::make_optional(nPos);
                    float targetHeight = heightMap[nPos];

                    // trace path to lower tile along backref to get length and depth
                    while (currentTile.has_value() && heightMap[currentTile.value()] >= targetHeight)
                    {
                        pathDepth = std::max(pathDepth, heightMap[currentTile.value()] - targetHeight);
                        pathLength++;

                        targetHeight = std::nextafter(targetHeight, std::numeric_limits<float>::lowest());
                        currentTile = backrefMap[currentTile.value()];
                    }

                    currentTile = std::make_optional(nPos);
                    targetHeight = heightMap[nPos];

                    // if the path limits are not exceeded, adjust height along path to assert overall gradient is maintained
                    if (pathLength <= ctx.settings.breachMaxLength && pathDepth <= ctx.settings.breachMaxDepth)
                    {
                        while (currentTile.has_value() && heightMap[currentTile.value()] >= targetHeight)
                        {
                            heightMap[currentTile.value()] = targetHeight;
                            hydroCtx.flags[currentTile.value()].set(breached);
                            hydroCtx.stats.pitsBreachedTiles++;

                            targetHeight = std::nextafter(targetHeight, std::numeric_limits<float>::lowest());
                            currentTile = backrefMap[currentTile.value()];
                        }
                        hydroCtx.stats.pitsBreachSuccess++;
                    }
                }
            }
        }

        // fill depressions based on the 'lowest-path' graph so some structure of the filled up topology is maintained in the
        // new micro surface, which makes for more interesting river paths compared to epsilon priority-flood. The queue order
        // ensures the backref node has already been handled.
        while (!fillQueue.empty())
        {
            const TileCoordsXY& fillPos = fillQueue.front();
            if (backrefMap[fillPos].has_value())
            {
                const TileCoordsXY& backrefPos = backrefMap[fillPos].value();
                if (heightMap[fillPos] <= heightMap[backrefPos])
                {
                    heightMap[fillPos] = std::nextafter(heightMap[backrefPos], std::numeric_limits<float>::infinity());
                    hydroCtx.flags[fillPos].set(filled);
                    hydroCtx.stats.pitsFilledTiles++;
                }
            }
            fillQueue.pop();
        }
    }

    static float downSlope(const MapGenContext& ctx, const TileCoordsXY& from, const TileCoordsXY& to, const float distance)
    {
        const float slope = (ctx.heightMap[from] - ctx.heightMap[to]) / distance;
        return std::max(0.0f, std::pow(slope, kP));
    }

    static float flowFraction(const MapGenContext& ctx, const TileCoordsXY& from, const TileCoordsXY& to, const float distance)
    {
        const HydroContext& hydroCtx = ctx.hydroContext.value();
        float sum = 0.0f;

        for (const auto& neighbour : kNeighbours)
        {
            const TileCoordsXY nPos{ from + neighbour.offset };
            if (ctx.heightMap.inBounds(nPos) && hydroCtx.flowsOut[from].has(neighbour.direction))
            {
                sum += downSlope(ctx, from, nPos, neighbour.distance);
            }
        }

        return downSlope(ctx, from, to, distance) / sum;
    }

    static float aggregateNeighbour(MapGenContext& ctx, const TileCoordsXY& pos)
    {
        HydroContext& hydroCtx = ctx.hydroContext.value();

        if (hydroCtx.catchment[pos] <= 0.0f)
        {
            const int32_t multiplier = isInWorldMap(ctx, pos) ? 1 : ctx.settings.offMapCatchmentMultiplier;

            hydroCtx.catchment[pos] = 1.0f * multiplier;
            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ pos + neighbour.offset };
                if (hydroCtx.flowsIn.inBounds(nPos) && hydroCtx.flowsIn[pos].has(neighbour.direction))
                {
                    const float neighbourFraction = flowFraction(ctx, nPos, pos, neighbour.distance);
                    const float neighbourCatchment = aggregateNeighbour(ctx, nPos);
                    const float neighbourContribution = neighbourFraction * neighbourCatchment;
                    hydroCtx.catchment[pos] += neighbourContribution;
                }
            }

            hydroCtx.stats.flowAggMax = std::max(hydroCtx.stats.flowAggMax, hydroCtx.catchment[pos]);
        }

        return hydroCtx.catchment[pos];
    }

    static void fillFlowMaps(MapGenContext& ctx, const TileCoordsXY& pos)
    {
        HydroContext& hydroCtx = ctx.hydroContext.value();
        for (const auto& neighbour : kNeighbours)
        {
            const TileCoordsXY nPos{ pos + neighbour.offset };
            if (ctx.heightMap.inBounds(nPos))
            {
                if (ctx.heightMap[pos] > ctx.heightMap[nPos])
                {
                    hydroCtx.flowsIn[nPos].set(neighbour.opposite);
                    hydroCtx.flowsOut[pos].set(neighbour.direction);
                }
                else if (ctx.heightMap[pos] < ctx.heightMap[nPos])
                {
                    hydroCtx.flowsIn[pos].set(neighbour.direction);
                    hydroCtx.flowsOut[nPos].set(neighbour.opposite);
                }
                // no lateral flows
            }
        }
    }

    /**
     * Recursively calculates the catchment (drainage basin) of each tile, catchment is passed down to lower neighbouring tiles
     * in proportion to slope.
     *
     * based on
     *
     * Freeman, T.G., 1991. Calculating catchment area with divergent flow based on a regular grid. Computers & geosciences,
     * 17(3), pp.413-422.
     */
    static void aggregateCatchment(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();

        for (int32_t y = 0; y < ctx.heightMap.height; y++)
        {
            for (int32_t x = 0; x < ctx.heightMap.width; x++)
            {
                fillFlowMaps(ctx, { x, y });
            }
        }
        for (int32_t y = 0; y < ctx.heightMap.height; y++)
        {
            for (int32_t x = 0; x < ctx.heightMap.width; x++)
            {
                aggregateNeighbour(ctx, { x, y });
            }
        }
    }

    static void postProcessTile(MapGenContext& ctx, TrackingStableTileQueue& queue, const TileCoordsXY& pos)
    {
        HydroContext& hydroCtx = ctx.hydroContext.value();

        if (hydroCtx.catchment[pos] >= ctx.settings.catchmentThreshold)
        {
            queue.emplaceAndMark(pos, ctx.heightMap[pos]);
            hydroCtx.flags[pos].set(river);
        }
    }

    /**
     * Due to the fractional catchment aggregation it is possible for downstream tiles to have a lower catchment than their
     * upstream(s), leading to non-edge sinks or orphans if these tiles fall below the threshold. Implicitly remove them by
     * tracing the stream networks upstream from the edge sinks.
     */
    static void postProcessCatchment(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();
        const HydroContext& hydroCtx = ctx.hydroContext.value();
        TrackingStableTileQueue queue{ ctx.dimensions };

        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            postProcessTile(ctx, queue, { 0, y });
            postProcessTile(ctx, queue, { ctx.dimensions.x - 1, y });
        }

        for (int32_t x = 1; x < ctx.dimensions.x - 1; x++)
        {
            postProcessTile(ctx, queue, { x, 0 });
            postProcessTile(ctx, queue, { x, ctx.dimensions.y - 1 });
        }

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };
                if (hydroCtx.flags.inBounds(nPos) && !queue.isMarked(nPos)
                    && hydroCtx.flowsIn[tile.pos].has(neighbour.direction))
                {
                    postProcessTile(ctx, queue, nPos);
                }
            }
        }
    }

    /**
     * Calculate the river width at the given position by scaling the catchment based on the max width and growth exponent
     * settings.
     */
    static float riverWidth(const MapGenContext& ctx, const TileCoordsXY& pos)
    {
        const HydroContext& hydroCtx = ctx.hydroContext.value();

        const float catchmentMin = 1.0f;
        const float catchmentMax = ctx.settings.mapSize.x * ctx.overscan * ctx.settings.mapSize.y * ctx.overscan;

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
    static float riverDepth(const float width)
    {
        const float depth = std::pow(width / 18.8f, 1.0f / 1.41f);
        return std::max(2.0f, 8.0f * depth); // rescale for rct
    }

    /**
     * Slightly widen the river based on catchment.
     */
    static void adjustStreamWidth(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();
        HeightMap& heightMap = ctx.heightMap;
        HydroContext& hydroCtx = ctx.hydroContext.value();
        TrackingStableTileQueue queue{ ctx.dimensions };
        DistanceMap distanceMap{ ctx.dimensions };
        distanceMap.fill(std::numeric_limits<float>::infinity());
        initHydroFlagQueue(ctx, queue);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };
                if (hydroCtx.flags.inBounds(nPos) && !queue.isMarked(nPos) && hydroCtx.flags[nPos].has(river))
                {
                    queue.emplaceAndMark(nPos, heightMap[nPos]);
                }
            }

            const float radius = riverWidth(ctx, tile.pos) / 2.0f;
            const float radiusSquared = radius * radius;

            for (int32_t dy = -radius; dy <= radius; dy++)
            {
                for (int32_t dx = -radius; dx <= radius; dx++)
                {
                    TileCoordsXY deltaPos = tile.pos + TileCoordsXY{ dx, dy };
                    const int32_t distance = dx * dx + dy * dy;
                    if (!heightMap.inBounds(deltaPos) || distance > radiusSquared
                        || heightMap[deltaPos] - heightMap[tile.pos] > 4.0f) // make waterfalls slightly thinner
                    {
                        continue;
                    }

                    // make waterfalls look nice, set height of newly minted river tiles to the height of the closest river tile
                    if (hydroCtx.flags[deltaPos].has(river))
                    {
                        if (distanceMap[deltaPos] < std::numeric_limits<float>::infinity())
                        {
                            if (distance < distanceMap[deltaPos])
                            {
                                heightMap[deltaPos] = heightMap[tile.pos];
                                distanceMap[deltaPos] = distance;
                            }
                            else if (distance == distanceMap[deltaPos])
                            {
                                heightMap[deltaPos] = std::min(heightMap[tile.pos], heightMap[deltaPos]);
                            }
                        }
                        continue;
                    }

                    heightMap[deltaPos] = heightMap[tile.pos];
                    distanceMap[deltaPos] = distance;
                    hydroCtx.flags[deltaPos].set(river);
                    queue.mark(deltaPos);
                    hydroCtx.stats.widthAdjustNewTiles++;
                }
            }
        }
    }

    /**
     * Remove short streams below the configured threshold.
     */
    static void pruneShortStreams(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();

        HydroContext& hydroCtx = ctx.hydroContext.value();
        std::stack<TileCoordsXY> stack;
        BaseMap<int8_t> inflowsOutstanding{ ctx.dimensions };
        ReferenceMap sourceReferenceAt{ ctx.dimensions };
        DistanceMap sourceDistanceAt{ ctx.dimensions };
        DistanceMap sourceLength{ ctx.dimensions };
        inflowsOutstanding.fill(-1);
        sourceDistanceAt.fill(-1);
        sourceLength.fill(-1);

        // populate inflowsOutstanding, identify sinks and init stack
        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (hydroCtx.flags[pos].has(river))
                {
                    inflowsOutstanding[pos] = countRiverInflows(ctx, pos);
                    if (inflowsOutstanding[pos] == 0)
                    {
                        stack.push(pos);
                        sourceReferenceAt[pos] = pos;
                        sourceDistanceAt[pos] = 0.0f;
                        hydroCtx.stats.pruneSourcesFound++;
                    }
                }
            }
        }

        // trace rivers from the sources, propagating the furthest source downstream after all inflows have been considered
        while (!stack.empty())
        {
            const TileCoordsXY currentPos = stack.top();
            stack.pop();
            auto currentSource = sourceReferenceAt[currentPos].value();

            for (const Neighbour& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ currentPos + neighbour.offset };
                if (hydroCtx.flags.inBounds(nPos) && hydroCtx.flowsOut[currentPos].has(neighbour.direction)
                    && hydroCtx.flags[nPos].has(river))
                {
                    const float distance = sourceDistanceAt[currentPos] + neighbour.distance;

                    if (sourceLength[currentSource] < distance)
                    {
                        sourceLength[currentSource] = distance;
                    }

                    if (sourceDistanceAt[nPos] < distance)
                    {
                        sourceDistanceAt[nPos] = distance;
                        sourceReferenceAt[nPos] = currentSource;
                    }

                    inflowsOutstanding[nPos]--;
                    if (inflowsOutstanding[nPos] == 0)
                    {
                        stack.push(nPos);
                    }
                }
            }
        }

        // clear tiles with a source ref that is below the threshold and mark valid sources
        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (hydroCtx.flags[pos].has(river))
                {
                    if (sourceLength[sourceReferenceAt[pos].value()] < ctx.settings.pruneThreshold)
                    {
                        hydroCtx.flags[pos].unset(river);
                        hydroCtx.stats.pruneSourcesTilesRemoved++;
                    }
                    if (sourceLength[pos] >= ctx.settings.pruneThreshold)
                    {
                        hydroCtx.flags[pos].set(source);
                        hydroCtx.stats.pruneSourcesRemaining++;
                        hydroCtx.stats.pruneSourcesLongest = std::max(hydroCtx.stats.pruneSourcesLongest, sourceLength[pos]);
                    }
                }
            }
        }
    }

    /**
     * Ensure diagonal channels render nicely by making sure each river tile has at least one ordinal neighbour.
     * Uses the game coordinate convention, i.e. the diagonal neighbours are cardinal directions.
     */
    static void ensureOrdinalNeighbours(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();
        HeightMap& heightMap = ctx.heightMap;
        HydroContext& hydroCtx = ctx.hydroContext.value();
        TrackingStableTileQueue queue{ ctx.dimensions };
        initHydroFlagQueue(ctx, queue);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& neighbour : kNeighboursCardinal)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };
                if (hydroCtx.flags.inBounds(nPos) && hydroCtx.flags[nPos].has(river))
                {
                    if (!queue.isMarked(nPos))
                    {
                        queue.emplaceAndMark(nPos, heightMap[nPos]);
                    }

                    if (!haveCommonOrdinalNeighbour(hydroCtx, tile.pos, neighbour.offset))
                    {
                        for (const TileCoordsXY& ordinalOffset : ordinalNeighbours(neighbour.offset))
                        {
                            const TileCoordsXY sharedOrdinalPos{ tile.pos + ordinalOffset };
                            heightMap[sharedOrdinalPos] = heightMap[tile.pos];
                            hydroCtx.catchment[sharedOrdinalPos] = ctx.settings.catchmentThreshold;
                            hydroCtx.flags[sharedOrdinalPos].set(river);
                            queue.emplaceAndMark(sharedOrdinalPos, heightMap[sharedOrdinalPos]);
                            hydroCtx.stats.ensureOrdinalNewTiles++;
                        }
                    }
                }
            }

            for (const auto& neighbour : kNeighboursOrdinal)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };
                if (hydroCtx.flags.inBounds(nPos) && !queue.isMarked(nPos) && hydroCtx.flags[nPos].has(river))
                {
                    queue.emplaceAndMark(nPos, heightMap[nPos]);
                }
            }
        }
    }

    /**
     * Carve a riverbed around the river increasing with catchment.
     */
    static void carveRiverbed(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();
        HeightMap& heightMap = ctx.heightMap;
        HydroContext& hydroCtx = ctx.hydroContext.value();
        HeightMap heightCopy = heightMap;
        TrackingStableTileQueue queue{ ctx.dimensions };
        initHydroFlagQueue(ctx, queue);
        const float seafloorMaxCarveDepth = static_cast<float>(ctx.settings.waterLevel - kRiversSeafloorMaxCarveDepth);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            const float width = riverWidth(ctx, tile.pos);
            const float radius = width / 2.0f + 1.0f;
            const float radiusSquared = radius * radius;
            const float depth = riverDepth(width);
            const float riverHeight = heightCopy[tile.pos] - 2.0f;

            hydroCtx.height[tile.pos] = riverHeight;

            for (int32_t dy = -radius; dy <= radius; dy++)
            {
                for (int32_t dx = -radius; dx <= radius; dx++)
                {
                    const float dxSquared = dx * dx;
                    const float dySquared = dy * dy;
                    const float distanceSquared = dxSquared + dySquared;

                    const TileCoordsXY deltaPos = tile.pos + TileCoordsXY{ dx, dy };
                    if (heightMap.inBounds(deltaPos) && distanceSquared <= radiusSquared)
                    {
                        float candidateHeight = std::max(riverHeight, seafloorMaxCarveDepth);

                        if (hydroCtx.flags[deltaPos].has(river))
                        {
                            const float dz = depth * std::sqrt(1.0f - distanceSquared / radiusSquared);
                            candidateHeight = candidateHeight - dz;
                        }

                        heightMap[deltaPos] = std::min(candidateHeight, heightMap[deltaPos]);
                    }
                }
            }

            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };
                if (hydroCtx.flags.inBounds(nPos) && hydroCtx.flags[nPos].has(river) && !queue.isMarked(nPos))
                {
                    queue.emplaceAndMark(nPos, heightCopy[nPos]);
                }
            }
        }
    }

    /**
     * Adjust single tile river bank indentations; 1 = river, O = land, center tile not in masks:
     *
     *      O O O     1 O O     O O 1     1 1 1
     *      O 1 O     1 1 O     O 1 1     O 1 O
     *      1 1 1     1 O O     O O 1     O O O
     */
    static void adjustBankIndentations(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();
        HydroContext& hydroCtx = ctx.hydroContext.value();

        constexpr std::array kIndentationMasks = { MapDirectionMask{ South, SouthEast, East },
                                                   MapDirectionMask{ South, SouthWest, West },
                                                   MapDirectionMask{ North, NorthEast, East },
                                                   MapDirectionMask{ North, NorthWest, West } };

        for (int32_t y = 1; y < ctx.dimensions.y - 1; y++)
        {
            for (int32_t x = 1; x < ctx.dimensions.x - 1; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (hydroCtx.flags[pos].has(river) && !hydroCtx.flags[pos].has(source))
                {
                    for (const auto& mask : kIndentationMasks)
                    {
                        bool maskMatches = true;
                        for (const Neighbour& neighbour : kNeighbours)
                        {
                            const TileCoordsXY nPos{ pos + neighbour.offset };
                            if (mask.has(neighbour.direction) != hydroCtx.flags[nPos].has(river))
                            {
                                maskMatches = false;
                                break;
                            }
                        }
                        if (maskMatches)
                        {
                            auto riverHeight = hydroCtx.height[pos];
                            for (const Neighbour& neighbour : kNeighbours)
                            {
                                const TileCoordsXY nPos{ pos + neighbour.offset };
                                if (!hydroCtx.flags[nPos].has(river))
                                {
                                    ctx.heightMap[nPos] = std::min(riverHeight, ctx.heightMap[nPos]);
                                }
                            }
                            hydroCtx.stats.bankIndentationsAdjusted++;
                            break;
                        }
                    }
                }
            }
        }
    }

    /**
     * Ensure there are no river tiles with lower land neighbours or reachable sinks/sources.
     */
    static void ensureConsistent(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();

        HydroContext& hydroCtx = ctx.hydroContext.value();

        // quantize heights for segmentation
        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                ctx.heightMap[pos] = quantizeHeight(ctx.heightMap[pos]);
                if (hydroCtx.flags[pos].has(river))
                {
                    hydroCtx.height[pos] = quantizeHeight(hydroCtx.height[pos]);
                }
            }
        }

        // find and handle isolated segments in iterations until none are found, greedily altering the smallest segment
        while (true)
        {
            BooleanMap visited{ ctx.dimensions };
            std::unordered_map<SegmentKey, TileCoordsXYSet, SegmentKeyHash> segments;

            for (int32_t y = 0; y < ctx.dimensions.y; y++)
            {
                for (int32_t x = 0; x < ctx.dimensions.x; x++)
                {
                    const TileCoordsXY pos{ x, y };
                    if (!hydroCtx.flags[pos].has(river) || visited[pos])
                    {
                        continue;
                    }

                    TileCoordsXYSet segment;
                    std::queue<TileCoordsXY> queue;
                    queue.emplace(pos);
                    segment.insert(pos);
                    visited[pos] = true;
                    bool hasSource = false;
                    bool hasSink = false;
                    bool belowSeaLevel = hydroCtx.height[pos] <= ctx.settings.waterLevel;

                    while (!queue.empty())
                    {
                        TileCoordsXY qPos = queue.front();
                        queue.pop();

                        if (hydroCtx.flags.onEdge(qPos))
                        {
                            hasSink = true;
                        }

                        if (hydroCtx.flags[qPos].has(source))
                        {
                            hasSource = true;
                        }

                        for (const auto& neighbour : kNeighboursOrdinal)
                        {
                            const TileCoordsXY nPos{ qPos + neighbour.offset };
                            if (hydroCtx.flags.inBounds(nPos) && hydroCtx.flags[nPos].has(river))
                            {
                                if (hydroCtx.height[qPos] > hydroCtx.height[nPos])
                                {
                                    hasSink = true;
                                }
                                else if (hydroCtx.height[qPos] < hydroCtx.height[nPos])
                                {
                                    hasSource = true;
                                }
                                else if (!visited[nPos])
                                {
                                    queue.emplace(nPos);
                                    segment.insert(nPos);
                                    visited[nPos] = true;
                                }
                            }
                        }
                    }

                    if (!hasSource && !hasSink && !belowSeaLevel)
                    {
                        segments[{ pos, segment.size(), ConsistencyOperation::remove }] = std::move(segment);
                    }
                    else if (!hasSource && hasSink && !belowSeaLevel)
                    {
                        segments[{ pos, segment.size(), ConsistencyOperation::lower }] = std::move(segment);
                    }
                    else if (hasSource && !hasSink && !belowSeaLevel)
                    {
                        segments[{ pos, segment.size(), ConsistencyOperation::raise }] = std::move(segment);
                    }
                }
            }

            if (segments.empty())
            {
                break;
            }

            const auto minIterator = std::ranges::min_element(segments | std::views::keys);
            const auto& candidate = *minIterator;
            switch (candidate.operation)
            {
                case ConsistencyOperation::lower:
                {
                    for (const TileCoordsXY& sPos : segments[candidate])
                    {
                        hydroCtx.height[sPos] -= 2.0f;
                        ctx.heightMap[sPos] = std::min(hydroCtx.height[sPos] - 2.0f, ctx.heightMap[sPos]);

                        if (hydroCtx.height[sPos] < 0.0f)
                        {
                            throw std::runtime_error(std::format("({},{}) lowered below 0", sPos.x, sPos.y));
                        }
                    }
                    hydroCtx.stats.consistencySegmentsLowered++;
                    hydroCtx.stats.consistencySegmentsLoweredMaxSize = std::max(
                        hydroCtx.stats.consistencySegmentsLoweredMaxSize, static_cast<int32_t>(segments[candidate].size()));
                    break;
                }
                case ConsistencyOperation::raise:
                {
                    for (const TileCoordsXY& sPos : segments[candidate])
                    {
                        hydroCtx.height[sPos] += 2.0f;

                        if (hydroCtx.height[sPos] >= 256.0f)
                        {
                            throw std::runtime_error(std::format("({},{}) raised above 256", sPos.x, sPos.y));
                        }
                    }
                    hydroCtx.stats.consistencySegmentsRaised++;
                    hydroCtx.stats.consistencySegmentsRaisedMaxSize = std::max(
                        hydroCtx.stats.consistencySegmentsRaisedMaxSize, static_cast<int32_t>(segments[candidate].size()));
                    break;
                }
                case ConsistencyOperation::remove: // should not happen
                {
                    for (const TileCoordsXY& sPos : segments[candidate])
                    {
                        hydroCtx.height[sPos] = 0.0f;
                        hydroCtx.flags[sPos].unset(river);
                    }
                    hydroCtx.stats.consistencySegmentsRemoved++;
                    hydroCtx.stats.consistencySegmentsRemovedMaxSize = std::max(
                        hydroCtx.stats.consistencySegmentsRemovedMaxSize, static_cast<int32_t>(segments[candidate].size()));
                    break;
                }
            }
            hydroCtx.stats.consistencySegmentsIterations++;
        }

        // make sure waterfalls are properly enclosed
        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (!hydroCtx.flags[pos].has(river))
                {
                    float minHeight = ctx.heightMap[pos];
                    for (const auto& neighbour : kNeighboursOrdinal)
                    {
                        const TileCoordsXY nPos{ pos + neighbour.offset };
                        if (hydroCtx.flags.inBounds(nPos) && hydroCtx.flags[nPos].has(river))
                        {
                            minHeight = std::max(minHeight, hydroCtx.height[nPos]);
                        }
                    }

                    if (minHeight > ctx.heightMap[pos])
                    {
                        hydroCtx.stats.consistencyBanksRaised++;
                        ctx.heightMap[pos] = minHeight;
                    }
                }
            }
        }
    }

    /**
     * Unset river and riverbed flags below sea level for more intuitive behavior in texture/scenery rules.
     */
    static void clearRiversBelowSeaLevel(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();
        HydroContext& hydroCtx = ctx.hydroContext.value();
        const float seafloorMaxCarveDepth = static_cast<float>(ctx.settings.waterLevel - kRiversSeafloorMaxCarveDepth);

        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (hydroCtx.height[pos] < seafloorMaxCarveDepth)
                {
                    hydroCtx.flags[pos].unset(river);
                }
            }
        }
    }

    void generateRivers(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();

        fillOrBreachDepressions(ctx);
        aggregateCatchment(ctx);
        postProcessCatchment(ctx);
        pruneShortStreams(ctx);
        ensureOrdinalNeighbours(ctx);
        adjustStreamWidth(ctx);
        carveRiverbed(ctx);
        adjustBankIndentations(ctx);
        ensureConsistent(ctx);
        clearRiversBelowSeaLevel(ctx);

        LOG_INFO("river generation statistics:%s", summarizeHydroStatistics(ctx).c_str());
    }
} // namespace OpenRCT2::World::MapGenerator::Hydro
