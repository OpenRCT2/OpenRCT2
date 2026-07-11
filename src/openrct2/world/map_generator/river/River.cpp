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
#include "../MapGenSerDe.hpp"
#include "../MapHelpers.h"
#include "../TileQueue.hpp"
#include "RiverUtils.h"

#include <ranges>

namespace OpenRCT2::World::MapGenerator::River
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
        RiverContext& riverCtx = ctx.riverContext.value();
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
                    riverCtx.statistics.pitsFound++;
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
                    if (pathLength <= ctx.settings.river.breachMaxLength && pathDepth <= ctx.settings.river.breachMaxDepth)
                    {
                        while (currentTile.has_value() && heightMap[currentTile.value()] >= targetHeight)
                        {
                            heightMap[currentTile.value()] = targetHeight;
                            riverCtx.flags[currentTile.value()].set(breached);
                            riverCtx.statistics.pitsBreachedTiles++;

                            targetHeight = std::nextafter(targetHeight, std::numeric_limits<float>::lowest());
                            currentTile = backrefMap[currentTile.value()];
                        }
                        riverCtx.statistics.pitsBreachSuccess++;
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
                    riverCtx.flags[fillPos].set(filled);
                    riverCtx.statistics.pitsFilledTiles++;
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
        const RiverContext& riverCtx = ctx.riverContext.value();
        float sum = 0.0f;

        for (const auto& neighbour : kNeighbours)
        {
            const TileCoordsXY nPos{ from + neighbour.offset };
            if (ctx.heightMap.inBounds(nPos) && riverCtx.flowsOut[from].has(neighbour.direction))
            {
                sum += downSlope(ctx, from, nPos, neighbour.distance);
            }
        }

        return downSlope(ctx, from, to, distance) / sum;
    }

    static float aggregateNeighbour(MapGenContext& ctx, const TileCoordsXY& pos)
    {
        RiverContext& riverCtx = ctx.riverContext.value();

        if (riverCtx.catchment[pos] <= 0.0f)
        {
            const int32_t multiplier = isInWorldMap(ctx, pos) ? 1 : ctx.settings.river.offMapCatchmentMultiplier.get();

            riverCtx.catchment[pos] = 1.0f * multiplier;
            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ pos + neighbour.offset };
                if (riverCtx.flowsIn.inBounds(nPos) && riverCtx.flowsIn[pos].has(neighbour.direction))
                {
                    const float neighbourFraction = flowFraction(ctx, nPos, pos, neighbour.distance);
                    const float neighbourCatchment = aggregateNeighbour(ctx, nPos);
                    const float neighbourContribution = neighbourFraction * neighbourCatchment;
                    riverCtx.catchment[pos] += neighbourContribution;
                }
            }

            riverCtx.statistics.flowAggMaxCatchment = std::max(
                riverCtx.statistics.flowAggMaxCatchment, riverCtx.catchment[pos]);
        }

        return riverCtx.catchment[pos];
    }

    static void fillFlowMaps(MapGenContext& ctx, const TileCoordsXY& pos)
    {
        RiverContext& riverCtx = ctx.riverContext.value();
        for (const auto& neighbour : kNeighbours)
        {
            const TileCoordsXY nPos{ pos + neighbour.offset };
            if (ctx.heightMap.inBounds(nPos))
            {
                if (ctx.heightMap[pos] > ctx.heightMap[nPos])
                {
                    riverCtx.flowsIn[nPos].set(neighbour.opposite);
                    riverCtx.flowsOut[pos].set(neighbour.direction);
                }
                else if (ctx.heightMap[pos] < ctx.heightMap[nPos])
                {
                    riverCtx.flowsIn[pos].set(neighbour.direction);
                    riverCtx.flowsOut[nPos].set(neighbour.opposite);
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
        RiverContext& riverCtx = ctx.riverContext.value();

        if (riverCtx.catchment[pos] >= ctx.settings.river.catchmentThreshold)
        {
            queue.emplaceAndMark(pos, ctx.heightMap[pos]);
            riverCtx.flags[pos].set(river);
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
        const RiverContext& riverCtx = ctx.riverContext.value();
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
                if (riverCtx.flags.inBounds(nPos) && !queue.isMarked(nPos)
                    && riverCtx.flowsIn[tile.pos].has(neighbour.direction))
                {
                    postProcessTile(ctx, queue, nPos);
                }
            }
        }
    }

    /**
     * Slightly widen the river based on catchment.
     */
    static void adjustStreamWidth(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();
        HeightMap& heightMap = ctx.heightMap;
        RiverContext& riverCtx = ctx.riverContext.value();
        TrackingStableTileQueue queue{ ctx.dimensions };
        DistanceMap distanceMap{ ctx.dimensions };
        distanceMap.fill(std::numeric_limits<float>::infinity());
        initRiverFlagQueue(ctx, queue);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };
                if (riverCtx.flags.inBounds(nPos) && !queue.isMarked(nPos) && riverCtx.flags[nPos].has(river))
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
                    if (!heightMap.inBounds(deltaPos) || distance > radiusSquared)
                    {
                        continue;
                    }

                    // make waterfalls look nice, set height of newly minted river tiles to the height of the closest river tile
                    if (riverCtx.flags[deltaPos].has(river))
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
                    riverCtx.flags[deltaPos].set(river);
                    queue.mark(deltaPos);
                    riverCtx.statistics.widthAdjustNewTiles++;
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

        RiverContext& riverCtx = ctx.riverContext.value();
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
                if (riverCtx.flags[pos].has(river))
                {
                    inflowsOutstanding[pos] = countRiverInflows(ctx, pos);
                    if (inflowsOutstanding[pos] == 0)
                    {
                        stack.push(pos);
                        sourceReferenceAt[pos] = pos;
                        sourceDistanceAt[pos] = 0.0f;
                        riverCtx.statistics.pruneSourcesFound++;
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
                if (riverCtx.flags.inBounds(nPos) && riverCtx.flowsOut[currentPos].has(neighbour.direction)
                    && riverCtx.flags[nPos].has(river))
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
                if (riverCtx.flags[pos].has(river))
                {
                    if (sourceLength[sourceReferenceAt[pos].value()] < ctx.settings.river.pruneThreshold)
                    {
                        riverCtx.flags[pos].unset(river);
                        riverCtx.statistics.pruneSourcesTilesRemoved++;
                    }
                    if (sourceLength[pos] >= ctx.settings.river.pruneThreshold)
                    {
                        riverCtx.flags[pos].set(source);
                        riverCtx.statistics.pruneSourcesRemaining++;
                        riverCtx.statistics.pruneSourcesLongest = std::max(
                            riverCtx.statistics.pruneSourcesLongest, sourceLength[pos]);
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
        RiverContext& riverCtx = ctx.riverContext.value();
        TrackingStableTileQueue queue{ ctx.dimensions };
        initRiverFlagQueue(ctx, queue);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& neighbour : kNeighboursCardinal)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };
                if (riverCtx.flags.inBounds(nPos) && riverCtx.flags[nPos].has(river))
                {
                    if (!queue.isMarked(nPos))
                    {
                        queue.emplaceAndMark(nPos, heightMap[nPos]);
                    }

                    if (!haveCommonOrdinalNeighbour(riverCtx, tile.pos, neighbour.offset))
                    {
                        for (const TileCoordsXY& ordinalOffset : ordinalNeighbours(neighbour.offset))
                        {
                            const TileCoordsXY sharedOrdinalPos{ tile.pos + ordinalOffset };
                            heightMap[sharedOrdinalPos] = heightMap[tile.pos];
                            riverCtx.catchment[sharedOrdinalPos] = ctx.settings.river.catchmentThreshold;
                            riverCtx.flags[sharedOrdinalPos].set(river);
                            queue.emplaceAndMark(sharedOrdinalPos, heightMap[sharedOrdinalPos]);
                            riverCtx.statistics.ensureOrdinalNewTiles++;
                        }
                    }
                }
            }

            for (const auto& neighbour : kNeighboursOrdinal)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };
                if (riverCtx.flags.inBounds(nPos) && !queue.isMarked(nPos) && riverCtx.flags[nPos].has(river))
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
        RiverContext& riverCtx = ctx.riverContext.value();
        HeightMap heightCopy = heightMap;
        TrackingStableTileQueue queue{ ctx.dimensions };
        initRiverFlagQueue(ctx, queue);
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

            riverCtx.waterLevel[tile.pos] = riverHeight;

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

                        if (riverCtx.flags[deltaPos].has(river))
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
                if (riverCtx.flags.inBounds(nPos) && riverCtx.flags[nPos].has(river) && !queue.isMarked(nPos))
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
        RiverContext& riverCtx = ctx.riverContext.value();

        constexpr std::array kIndentationMasks = { MapDirectionMask{ South, SouthEast, East },
                                                   MapDirectionMask{ South, SouthWest, West },
                                                   MapDirectionMask{ North, NorthEast, East },
                                                   MapDirectionMask{ North, NorthWest, West } };

        for (int32_t y = 1; y < ctx.dimensions.y - 1; y++)
        {
            for (int32_t x = 1; x < ctx.dimensions.x - 1; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (riverCtx.flags[pos].has(river) && !riverCtx.flags[pos].has(source))
                {
                    for (const auto& mask : kIndentationMasks)
                    {
                        bool maskMatches = true;
                        for (const Neighbour& neighbour : kNeighbours)
                        {
                            const TileCoordsXY nPos{ pos + neighbour.offset };
                            if (mask.has(neighbour.direction) != riverCtx.flags[nPos].has(river))
                            {
                                maskMatches = false;
                                break;
                            }
                        }
                        if (maskMatches)
                        {
                            auto riverHeight = riverCtx.waterLevel[pos];
                            for (const Neighbour& neighbour : kNeighbours)
                            {
                                const TileCoordsXY nPos{ pos + neighbour.offset };
                                if (!riverCtx.flags[nPos].has(river))
                                {
                                    ctx.heightMap[nPos] = std::min(riverHeight, ctx.heightMap[nPos]);
                                }
                            }
                            riverCtx.statistics.bankIndentationsAdjusted++;
                            break;
                        }
                    }
                }
            }
        }
    }

    /**
     * Ensure there are no river tiles with lower land neighbours or without reachable sinks/sources.
     */
    static void ensureConsistent(MapGenContext& ctx)
    {
        PROFILED_FUNCTION();

        RiverContext& riverCtx = ctx.riverContext.value();

        // quantize heights for segmentation
        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                ctx.heightMap[pos] = quantizeHeight(ctx.heightMap[pos]);
                if (riverCtx.flags[pos].has(river))
                {
                    riverCtx.waterLevel[pos] = quantizeHeight(riverCtx.waterLevel[pos]);
                }
            }
        }

        // find and handle isolated segments in iterations until none are found, altering the smallest segment
        while (true)
        {
            BooleanMap visited{ ctx.dimensions };
            std::unordered_map<SegmentKey, TileCoordsXYSet, SegmentKeyHash> segments;

            for (int32_t y = 0; y < ctx.dimensions.y; y++)
            {
                for (int32_t x = 0; x < ctx.dimensions.x; x++)
                {
                    const TileCoordsXY pos{ x, y };
                    if (!riverCtx.flags[pos].has(river) || visited[pos])
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

                    while (!queue.empty())
                    {
                        TileCoordsXY qPos = queue.front();
                        queue.pop();

                        if (riverCtx.flags.onEdge(qPos))
                        {
                            hasSink = true;
                        }

                        if (riverCtx.flags[qPos].has(source))
                        {
                            hasSource = true;
                        }

                        for (const auto& neighbour : kNeighboursOrdinal)
                        {
                            const TileCoordsXY nPos{ qPos + neighbour.offset };
                            if (riverCtx.flags.inBounds(nPos) && riverCtx.flags[nPos].has(river))
                            {
                                if (riverCtx.waterLevel[qPos] > riverCtx.waterLevel[nPos])
                                {
                                    hasSink = true;
                                }
                                else if (riverCtx.waterLevel[qPos] < riverCtx.waterLevel[nPos])
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

                    if (!hasSource && !hasSink)
                    {
                        segments[{ pos, segment.size(), ConsistencyOperation::remove }] = std::move(segment);
                    }
                    else if (!hasSource)
                    {
                        segments[{ pos, segment.size(), ConsistencyOperation::lower }] = std::move(segment);
                    }
                    else if (!hasSink)
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
                        riverCtx.waterLevel[sPos] -= 2.0f;
                        ctx.heightMap[sPos] = std::min(riverCtx.waterLevel[sPos] - 2.0f, ctx.heightMap[sPos]);

                        if (riverCtx.waterLevel[sPos] < 0.0f)
                        {
                            handleConsistencyRunaway(ctx, candidate.pos, sPos, segments[candidate].size(), true);
                        }
                    }
                    riverCtx.statistics.consistencySegmentsLowered++;
                    riverCtx.statistics.consistencySegmentsLoweredMaxSize = std::max(
                        riverCtx.statistics.consistencySegmentsLoweredMaxSize,
                        static_cast<int32_t>(segments[candidate].size()));
                    break;
                }
                case ConsistencyOperation::raise:
                {
                    for (const TileCoordsXY& sPos : segments[candidate])
                    {
                        riverCtx.waterLevel[sPos] += 2.0f;

                        if (riverCtx.waterLevel[sPos] >= 256.0f)
                        {
                            handleConsistencyRunaway(ctx, candidate.pos, sPos, segments[candidate].size(), false);
                        }
                    }
                    riverCtx.statistics.consistencySegmentsRaised++;
                    riverCtx.statistics.consistencySegmentsRaisedMaxSize = std::max(
                        riverCtx.statistics.consistencySegmentsRaisedMaxSize, static_cast<int32_t>(segments[candidate].size()));
                    break;
                }
                case ConsistencyOperation::remove: // should not happen
                {
                    for (const TileCoordsXY& sPos : segments[candidate])
                    {
                        riverCtx.waterLevel[sPos] = 0.0f;
                        riverCtx.flags[sPos].unset(river);
                    }
                    riverCtx.statistics.consistencySegmentsRemoved++;
                    riverCtx.statistics.consistencySegmentsRemovedMaxSize = std::max(
                        riverCtx.statistics.consistencySegmentsRemovedMaxSize,
                        static_cast<int32_t>(segments[candidate].size()));
                    break;
                }
            }
            riverCtx.statistics.consistencySegmentsIterations++;
        }

        // make sure waterfalls are properly enclosed
        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (!riverCtx.flags[pos].has(river))
                {
                    float minHeight = ctx.heightMap[pos];
                    for (const auto& neighbour : kNeighboursOrdinal)
                    {
                        const TileCoordsXY nPos{ pos + neighbour.offset };
                        if (riverCtx.flags.inBounds(nPos) && riverCtx.flags[nPos].has(river))
                        {
                            minHeight = std::max(minHeight, riverCtx.waterLevel[nPos]);
                        }
                    }

                    if (minHeight > ctx.heightMap[pos])
                    {
                        riverCtx.statistics.consistencyBanksRaised++;
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
        RiverContext& riverCtx = ctx.riverContext.value();
        const float seafloorMaxCarveDepth = static_cast<float>(ctx.settings.waterLevel - kRiversSeafloorMaxCarveDepth);

        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (riverCtx.waterLevel[pos] < seafloorMaxCarveDepth)
                {
                    riverCtx.flags[pos].unset(river);
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

        LOG_INFO("river generation statistics:%s", summarizeRiverStatistics(ctx).c_str());
    }
} // namespace OpenRCT2::World::MapGenerator::River
