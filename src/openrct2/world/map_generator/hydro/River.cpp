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
#include "../DistanceMapUtils.h"
#include "../MapHelpers.h"
#include "../TileQueue.hpp"

#include <format>
#include <numbers>

// #define ENABLE_DEBUG_SIGNS

namespace OpenRCT2::World::MapGenerator::Hydro
{
    static constexpr float kP = 1.1f;

    struct TileState
    {
        // TODO use an uint8_t to encode the kNeighbourOffsets index
        Backref backref;
        bool pit;
    };

    using StateMap = BaseMap<TileState>;

    /**
     * Fill up or breach out of depressions in the heightmap to ensure there is a monotonic downhill path for rivers to follow.
     *
     * based on
     *
     * Lindsay, J.B., 2016. Efficient hybrid breaching‐filling sink removal methods for flow path enforcement in digital
     * elevation models. Hydrological Processes, 30(6), pp.846-857.
     */
    static void fillOrBreachDepressions(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HeightMap& heightMap = context.heightMap;
        HydroMaps& hydroMaps = context.hydroMaps.value();
        StateMap state{ context.dimensions };
        TrackingStableTileQueue queue{ context.dimensions };
        std::queue<TileCoordsXY> fillQueue;

        // prepare queue and mark pits
        for (int32_t y = 0; y < context.dimensions.x; y++)
        {
            for (int32_t x = 0; x < context.dimensions.y; x++)
            {
                TileCoordsXY pos{ x, y };

                if (x == 0 || y == 0 || x == context.dimensions.x - 1 || y == context.dimensions.y - 1)
                {
                    queue.emplaceAndVisit(pos, heightMap[pos]);
                    continue;
                }

                float minNeighbour = std::numeric_limits<float>::infinity();

                for (const auto& offset : kNeighbourOffsets)
                {
                    const TileCoordsXY nPos{ pos + offset };
                    // no bounds check needed per if-clause above
                    minNeighbour = std::min(heightMap[nPos], minNeighbour);
                }

                // this is a pit, raise to just below the lowest neighbour to shorten/shallow potential breach path
                if (heightMap[pos] < minNeighbour)
                {
                    heightMap[pos] = std::nextafter(minNeighbour, std::numeric_limits<float>::lowest());
                    state[pos].pit = true;
                }
            }
        }

        // process lowest tile from queue, moving inward from map edges and building up a 'lowest path' graph
        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!heightMap.inBounds(nPos) || queue.visited(nPos))
                {
                    continue;
                }

                queue.emplaceAndVisit(nPos, heightMap[nPos]);
                fillQueue.push(nPos);
                state[nPos].backref = tile.pos;

                // check if there is a valid path to breach out of the pit
                if (state[nPos].pit)
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
                        currentTile = state[currentTile.value()].backref;
                    }

                    currentTile = std::make_optional(nPos);
                    targetHeight = heightMap[nPos];

                    // if the path limits are not exceeded, adjust height along path to assert overall gradient is maintained
                    if (pathLength <= context.settings.breachMaxLength && pathDepth <= context.settings.breachMaxDepth)
                    {
                        while (currentTile.has_value() && heightMap[currentTile.value()] >= targetHeight)
                        {
                            heightMap[currentTile.value()] = targetHeight;
                            hydroMaps.flags[currentTile.value()].set(breached);

                            targetHeight = std::nextafter(targetHeight, std::numeric_limits<float>::lowest());
                            currentTile = state[currentTile.value()].backref;
                        }
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
            if (state[fillPos].backref.has_value())
            {
                const TileCoordsXY& backrefPos = state[fillPos].backref.value();

                if (heightMap[fillPos] <= heightMap[backrefPos])
                {
                    heightMap[fillPos] = std::nextafter(heightMap[backrefPos], std::numeric_limits<float>::infinity());
                    hydroMaps.flags[fillPos].set(filled);
                }
            }
            fillQueue.pop();
        }
    }

    static float downSlope(const HeightMap& heightMap, const TileCoordsXY& from, const TileCoordsXY& to)
    {
        const bool isOrdinal = from.x == 0 || from.y == 0 || to.x == 0 || to.y == 0;
        const float distance = isOrdinal ? 1.0f : std::numbers::sqrt2;
        const float slope = (heightMap[from] - heightMap[to]) / distance;
        return std::max(0.0f, std::pow(slope, kP));
    }

    static float flowFraction(const HeightMap& heightMap, const TileCoordsXY& from, const TileCoordsXY& to)
    {
        float sum = 0.0f;

        for (const auto& offset : kNeighbourOffsets)
        {
            const TileCoordsXY nPos{ from + offset };

            if (heightMap.inBounds(nPos))
            {
                sum += downSlope(heightMap, from, nPos);
            }
        }

        return downSlope(heightMap, from, to) / sum;
    }

    static float aggregateNeighbour(MapGenCtx& context, const TileCoordsXY& pos)
    {
        HeightMap& heightMap = context.heightMap;
        HydroMaps& hydroMaps = context.hydroMaps.value();

        if (hydroMaps.catchment[pos] <= 0.0f)
        {
            const int32_t multiplier = isInWorldMap(context, pos) ? 1 : context.settings.offMapCatchmentMultiplier;

            hydroMaps.catchment[pos] = 1.0f * multiplier;
            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ pos + offset };

                if (heightMap.inBounds(nPos) && heightMap[nPos] > heightMap[pos])
                {
                    const float neighbourFraction = flowFraction(heightMap, nPos, pos);
                    const float neighbourCatchment = aggregateNeighbour(context, nPos);
                    const float neighbourContribution = neighbourFraction * neighbourCatchment;
                    hydroMaps.catchment[pos] += neighbourContribution;
                }
            }
        }
        return hydroMaps.catchment[pos];
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
    static void aggregateCatchment(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        for (int32_t y = 0; y < context.heightMap.height; y++)
        {
            for (int32_t x = 0; x < context.heightMap.width; x++)
            {
                aggregateNeighbour(context, { x, y });
            }
        }
    }

    static void postProcessTile(MapGenCtx& context, TrackingStableTileQueue& queue, const TileCoordsXY& pos)
    {
        HydroMaps& hydroMaps = context.hydroMaps.value();

        if (hydroMaps.catchment[pos] >= context.settings.catchmentThreshold)
        {
            queue.emplaceAndVisit(pos, context.heightMap[pos]);
            hydroMaps.flags[pos].set(river);
            hydroMaps.flags[pos].set(skeleton);
        }
    }

    /**
     * Applies the catchment threshold and implicitly removes orphans.
     */
    static void postProcessCatchment(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        const HydroMaps& hydroMaps = context.hydroMaps.value();
        TrackingStableTileQueue queue{ context.dimensions };

        for (int32_t y = 0; y < context.dimensions.y; y++)
        {
            postProcessTile(context, queue, { 0, y });
            postProcessTile(context, queue, { context.dimensions.x - 1, y });
        }

        for (int32_t x = 1; x < context.dimensions.x - 1; x++)
        {
            postProcessTile(context, queue, { x, 0 });
            postProcessTile(context, queue, { x, context.dimensions.y - 1 });
        }

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ tile.pos + offset };
                if (hydroMaps.flags.inBounds(nPos) && !queue.visited(nPos))
                {
                    postProcessTile(context, queue, nPos);
                }
            }
        }
    }

    /**
     * To aid short stream pruning reduce the river map to a skeleton graph by running a line thinning algorithm.
     *
     * based on
     *
     * Chen, Y.S. and Hsu, W.H., 1988. A modified fast parallel algorithm for thinning digital patterns. Pattern Recognition
     * Letters, 7(2), pp.99-106.
     */
    static void constructRiverSkeletons(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HydroMaps& hydroMaps = context.hydroMaps.value();
        bool firstSubiteration = true;
        std::vector<TileCoordsXY> toClear;

        while (true)
        {
            for (int32_t y = 1; y < context.dimensions.y - 1; y++)
            {
                for (int32_t x = 1; x < context.dimensions.x - 1; x++)
                {
                    TileCoordsXY p1{ x, y };

                    if (!hydroMaps.flags[p1].has(skeleton))
                    {
                        continue;
                    }

                    union
                    {
                        bool pN[8];
                        struct
                        {
                            bool p2, p3, p4, p5, p6, p7, p8, p9;
                        };
                    } nb = { .p2 = hydroMaps.flags[p1 + TileCoordsXY{ 0, -1 }].has(skeleton),
                             .p3 = hydroMaps.flags[p1 + TileCoordsXY{ 1, -1 }].has(skeleton),
                             .p4 = hydroMaps.flags[p1 + TileCoordsXY{ 1, 0 }].has(skeleton),
                             .p5 = hydroMaps.flags[p1 + TileCoordsXY{ 1, 1 }].has(skeleton),
                             .p6 = hydroMaps.flags[p1 + TileCoordsXY{ 0, 1 }].has(skeleton),
                             .p7 = hydroMaps.flags[p1 + TileCoordsXY{ -1, 1 }].has(skeleton),
                             .p8 = hydroMaps.flags[p1 + TileCoordsXY{ -1, 0 }].has(skeleton),
                             .p9 = hydroMaps.flags[p1 + TileCoordsXY{ -1, -1 }].has(skeleton) };

                    int32_t bOfP1 = 0;
                    for (const auto& n : nb.pN)
                    {
                        if (n)
                        {
                            bOfP1 += 1;
                        }
                    }

                    if (!(2 <= bOfP1 && bOfP1 <= 7))
                    {
                        continue; // condition a
                    }

                    int32_t aOfP1 = 0;
                    for (size_t i = 0; i < 8; ++i)
                    {
                        if (!nb.pN[i] && nb.pN[(i + 1) % 8])
                        {
                            aOfP1 += 1;
                        }
                    }

                    if (aOfP1 != 1 && aOfP1 != 2)
                    {
                        continue; // condition b
                    }

                    if (firstSubiteration)
                    {
                        if (aOfP1 == 1)
                        {
                            if (!((!nb.p2 || !nb.p4 || !nb.p6) && (!nb.p4 || !nb.p6 || !nb.p8)))
                            {
                                continue; // condition {c, d}
                            }
                        }
                        else
                        {
                            if (!(((nb.p2 && nb.p4) && (!nb.p6 && !nb.p7 && !nb.p8))
                                  || ((nb.p4 && nb.p6) && (!nb.p2 && !nb.p8 && !nb.p9))))
                            {
                                continue; // condition {f, g}
                            }
                        }
                    }
                    else
                    {
                        if (aOfP1 == 1)
                        {
                            if (!((!nb.p2 || !nb.p4 || !nb.p8) && (!nb.p2 || !nb.p6 || !nb.p8)))
                            {
                                continue; // condition {c', d'}
                            }
                        }
                        else
                        {
                            if (!(((nb.p2 && nb.p8) && (!nb.p4 && !nb.p5 && !nb.p6))
                                  || ((nb.p6 && nb.p8) && (!nb.p2 && !nb.p3 && !nb.p4))))
                            {
                                continue; // condition {f', g'}
                            }
                        }
                    }

                    toClear.push_back(p1);
                }
            }

            if (toClear.empty())
            {
                break;
            }

            for (const auto& pos : toClear)
            {
                hydroMaps.flags[pos].unset(skeleton);
            }

            toClear.clear();
            firstSubiteration = !firstSubiteration;
        }
    }

    /**
     * Common setup steps for queue based traversal
     */
    static void prepareRiverQueue(MapGenCtx& context, TrackingStableTileQueue& queue, const HydroFlag flag = river)
    {
        HydroMaps& hydroMaps = context.hydroMaps.value();

        for (int32_t y = 0; y < context.dimensions.y - 1; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (hydroMaps.flags[left].has(flag))
            {
                queue.emplaceAndVisit(left, context.heightMap[left]);
            }

            const TileCoordsXY right{ context.dimensions.x - 1, y };
            if (hydroMaps.flags[right].has(flag))
            {
                queue.emplaceAndVisit(right, context.heightMap[right]);
            }
        }

        for (int32_t x = 1; x < context.dimensions.x - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (hydroMaps.flags[top].has(flag))
            {
                queue.emplaceAndVisit(top, context.heightMap[top]);
            }

            const TileCoordsXY bottom{ x, context.dimensions.y - 1 };
            if (hydroMaps.flags[bottom].has(flag))
            {
                queue.emplaceAndVisit(bottom, context.heightMap[bottom]);
            }
        }
    }

    /**
     * Returns the two ordinal neighbours for the given cardinal offset.
     * Uses the game coordinate convention, i.e. the diagonal neighbours are cardinal directions.
     */
    static std::array<TileCoordsXY, 2> ordinalNeighbours(const TileCoordsXY& offset)
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
    static bool haveCommonOrdinalNeighbour(const HydroMaps& hydroMaps, const TileCoordsXY& pos, const TileCoordsXY& offset)
    {
        for (const TileCoordsXY& ordinalOffset : ordinalNeighbours(offset))
        {
            const TileCoordsXY sharedOrdinalPos{ pos + ordinalOffset };
            if (hydroMaps.flags[sharedOrdinalPos].has(river))
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Queue visitor function for pruneShortStreams
     */
    static void pruneVisit(
        MapGenCtx& context, TrackingStableTileQueue& queue, BackrefMap& backrefMap, BackrefsMap& auxBackrefsMap,
        DistanceMap& distanceMap, const TileCoordsXY& pos, int32_t& upstreamCount, int32_t& auxCount,
        const TileCoordsXY& offset)
    {
        const TileCoordsXY nPos{ pos + offset };

        if (!context.hydroMaps.value().flags.inBounds(nPos) || !context.hydroMaps.value().flags[nPos].has(skeleton))
        {
            return;
        }

        if (queue.visited(nPos))
        {
            if (backrefMap[pos].has_value() && backrefMap[pos].value() != nPos)
            {
                auxBackrefsMap[nPos].insert(pos);
                auxCount++;
            }
            return;
        }

        backrefMap[nPos] = pos;
        auxBackrefsMap[nPos].insert(pos);
        queue.emplaceAndVisit(nPos, context.heightMap[nPos]);
        upstreamCount++;
        distanceMap[nPos] = distanceMap[pos] + 1.0f;
    }

    /**
     * Calculate the river width at the given position by scaling the catchment based on the max width and growth exponent
     * settings.
     */
    static float riverWidth(const MapGenCtx& context, const TileCoordsXY& pos)
    {
        const HydroMaps& hydroMaps = context.hydroMaps.value();

        const float catchmentMin = 1.0f;
        const float catchmentMax = context.settings.mapSize.x * kRiversOverscanFactor * context.settings.mapSize.y
            * kRiversOverscanFactor;

        const float widthMin = 0.0f;
        const float widthMax = context.settings.riverWidthMax;

        const float rescaledCatchment = (hydroMaps.catchment[pos] - catchmentMin) / (catchmentMax - catchmentMin);
        const float exponentiatedCatchment = std::pow(
            rescaledCatchment, context.settings.riverGrowthExponent * kRiverGrowthExponentScaling);
        const float width = widthMin + exponentiatedCatchment * (widthMax - widthMin);

        return width;
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
        // rescale for rct
        return std::max(2.0f, 8.0f * depth);
    }

    /**
     * Slightly widen the river based on catchment.
     */
    static void adjustStreamWidth(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HeightMap& heightMap = context.heightMap;
        HydroMaps& hydroMaps = context.hydroMaps.value();
        TrackingStableTileQueue queue{ context.dimensions };
        DistanceMap distanceMap{ context.dimensions };
        distanceMap.fill(std::numeric_limits<float>::infinity());
        prepareRiverQueue(context, queue, river);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!hydroMaps.flags.inBounds(nPos) || queue.visited(nPos) || !hydroMaps.flags[nPos].has(river))
                {
                    continue;
                }

                queue.emplaceAndVisit(nPos, heightMap[nPos]);
            }

            const float radius = riverWidth(context, tile.pos) / 2.0f;
            const float radiusSquared = radius * radius;

            for (int32_t dy = -radius; dy <= radius; dy++)
            {
                for (int32_t dx = -radius; dx <= radius; dx++)
                {
                    TileCoordsXY deltaPos = tile.pos + TileCoordsXY{ dx, dy };

                    int32_t distance = dx * dx + dy * dy;
                    if (!heightMap.inBounds(deltaPos) || distance > radiusSquared)
                    {
                        continue;
                    }

                    // make waterfalls a bit thinner
                    if (heightMap[deltaPos] - heightMap[tile.pos] > 4.0f)
                    {
                        continue;
                    }

                    // ensure waterfalls look nice (i.e. are monotonic) by setting the height of newly minted river tiles to the
                    // height of the closest river tile
                    if (hydroMaps.flags[deltaPos].has(river))
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
                    hydroMaps.flags[deltaPos].set(river);
                    hydroMaps.flags[deltaPos].set(skeleton);
                    queue.visit(deltaPos);
                }
            }
        }
    }

    /**
     * Ensure diagonal channels render nicely by asserting each river tile has at least one ordinal neighbour.
     * Uses the game coordinate convention, i.e. the diagonal neighbours are cardinal directions.
     */
    static void ensureCardinalNeighbours(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HeightMap& heightMap = context.heightMap;
        HydroMaps& hydroMaps = context.hydroMaps.value();
        TrackingStableTileQueue queue{ context.dimensions };
        prepareRiverQueue(context, queue);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsetsCardinal)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!hydroMaps.flags.inBounds(nPos) || queue.visited(nPos) || !hydroMaps.flags[nPos].has(river))
                {
                    continue;
                }

                queue.emplaceAndVisit(nPos, heightMap[nPos]);

                if (!haveCommonOrdinalNeighbour(hydroMaps, tile.pos, offset))
                {
                    for (const TileCoordsXY& ordinalOffset : ordinalNeighbours(offset))
                    {
                        const TileCoordsXY sharedOrdinalPos{ tile.pos + ordinalOffset };
                        heightMap[sharedOrdinalPos] = heightMap[tile.pos];
                        hydroMaps.catchment[sharedOrdinalPos] = context.settings.catchmentThreshold;
                        hydroMaps.flags[sharedOrdinalPos].set(river);
                        hydroMaps.flags[sharedOrdinalPos].set(skeleton);
                        queue.visit(sharedOrdinalPos);
                    }
                }
            }

            for (const auto& offset : kNeighbourOffsetsOrdinal)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!hydroMaps.flags.inBounds(nPos) || queue.visited(nPos) || !hydroMaps.flags[nPos].has(river))
                {
                    continue;
                }

                queue.emplaceAndVisit(nPos, heightMap[nPos]);
            }
        }
    }

    /**
     * Prunes tributary streams that are shorter than context.settings.pruneThreshold.
     *
     * TODO split up and optimize
     */
    static void pruneShortStreams(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HydroMaps& hydroMaps = context.hydroMaps.value();

#ifdef ENABLE_DEBUG_SIGNS
        TileCoordsXYSet prunedSkeletons;
#endif

        // iterate until no streams are pruned
        while (true)
        {
            TrackingStableTileQueue queue{ context.dimensions };
            BackrefsMap springHits{ context.dimensions };
            BackrefsMap auxBackrefsMap{ context.dimensions };
            BackrefMap backrefMap{ context.dimensions };
            BackrefMap confluenceMaxSpringMap{ context.dimensions };
            BooleanMap confluenceMap{ context.dimensions };
            DistanceMap distanceMap{ context.dimensions };
            std::vector<TileCoordsXY> springs;
            prepareRiverQueue(context, queue, skeleton);

            // initial pass; upstream to populate backref, auxBackrefs, confluence and distance maps
            while (!queue.empty())
            {
                const QueueTile tile = queue.top();
                queue.pop();

                int32_t upstreamCount = 0;
                int32_t auxCount = 0;

                for (const auto& offset : kNeighbourOffsets)
                {
                    pruneVisit(
                        context, queue, backrefMap, auxBackrefsMap, distanceMap, tile.pos, upstreamCount, auxCount, offset);
                }

                if (upstreamCount > 1)
                {
                    confluenceMap[tile.pos] = true;
                }
                else if (upstreamCount == 0 && auxCount == 0)
                {
                    springs.push_back(tile.pos);
                }
            }

            // second pass; downstream from springs following backrefs to populate springHits and confluenceMaxSpringMap
            for (const auto& spring : springs)
            {
                std::queue<TileCoordsXY> springTraceQueue;
                BooleanMap springTraceVisited{ context.dimensions };
                springTraceQueue.push(spring);
                springTraceVisited[spring] = true;

                while (!springTraceQueue.empty())
                {
                    TileCoordsXY current = springTraceQueue.front();
                    springTraceQueue.pop();

                    springHits[current].insert(spring);

                    if (confluenceMap[current])
                    {
                        const Backref& maybeMaxSpring = confluenceMaxSpringMap[current];
                        if (!maybeMaxSpring.has_value() || distanceMap[spring] > distanceMap[maybeMaxSpring.value()])
                        {
                            confluenceMaxSpringMap[current] = spring;
                        }
                    }

                    for (const auto& backref : auxBackrefsMap[current])
                    {
                        if (!springTraceVisited[backref])
                        {
                            springTraceQueue.push(backref);
                            springTraceVisited[backref] = true;
                        }
                    }
                }
            }

            // unmark single-spring confluences
            for (int32_t y = 1; y < context.dimensions.y - 1; y++)
            {
                for (int32_t x = 1; x < context.dimensions.x - 1; x++)
                {
                    const TileCoordsXY pos{ x, y };
                    if (confluenceMap[pos] && springHits[pos].size() == 1)
                    {
                        confluenceMap[pos] = false;
                    }
                }
            }

            // third pass; downstream from springs following backrefs until the first confluence to identify prunable streams
            bool pruned = false;
            for (const auto& spring : springs)
            {
                TileCoordsXYSet pruneCandidates;
                TileCoordsXYSet validConfluence;
                std::queue<std::pair<TileCoordsXY, int32_t>> pruneQueue;
                BooleanMap pruneVisited{ context.dimensions };
                bool thresholdExceeded = false;
                bool maxSpring = false;
                pruneQueue.push({ spring, 0 });
                pruneVisited[spring] = true;

                while (!pruneQueue.empty())
                {
                    auto queuePair = pruneQueue.front();
                    TileCoordsXY currentTile = queuePair.first;
                    int32_t distance = queuePair.second;
                    pruneQueue.pop();

                    // if the distance from the spring is exceeded on any path consider the stream not prunable
                    if (distance > context.settings.pruneThreshold)
                    {
                        thresholdExceeded = true;
                    }
                    // if this is the furthest spring of any confluence the stream isn't prunable
                    else if (confluenceMap[currentTile])
                    {
                        const auto& maybeMaxSpring = confluenceMaxSpringMap[currentTile];
                        if (maybeMaxSpring.has_value() && maybeMaxSpring.value() == spring)
                        {
                            maxSpring = true;
                        }
                        else
                        {
                            validConfluence.insert(currentTile);
                        }
                    }
                    // continue downstream, enqueue the 'main' backref or backrefs that are only visited by this spring
                    else
                    {
                        pruneCandidates.insert(currentTile);
                        for (const auto& backref : auxBackrefsMap[currentTile])
                        {
                            if (!pruneVisited[backref])
                            {
                                if ((backrefMap[currentTile].has_value() && backrefMap[currentTile].value() == backref)
                                    || (springHits[currentTile].contains(spring) && springHits[currentTile].size() == 1))
                                {
                                    pruneQueue.push({ backref, distance + 1 });
                                    pruneVisited[backref] = true;
                                }
                            }
                        }
                    }
                }

                // the spring and its downstream stream(s) are prunable
                if (!validConfluence.empty() && !thresholdExceeded && !maxSpring)
                {
                    for (const auto& toPrune : pruneCandidates)
                    {
                        hydroMaps.flags[toPrune].unset(skeleton);
#ifdef ENABLE_DEBUG_SIGNS
                        prunedSkeletons.insert(toPrune);
#endif
                    }
                    pruned = true;
                }
            }

            // no springs pruned in this iteration, mark remaining springs and break from loop
            if (!pruned)
            {
                for (const auto& spring : springs)
                {
                    hydroMaps.flags[spring].set(HydroFlag::spring);
#ifdef ENABLE_DEBUG_SIGNS
                    context.debugSigns.emplace_back(
                        spring, std::format("spring {} {}", distanceMap[spring], auxBackrefsMap[spring].size()),
                        Drawing::Colour::white, Drawing::Colour::lightBlue);
#endif
                }
#ifdef ENABLE_DEBUG_SIGNS
                for (int32_t y = 0; y < context.dimensions.y; y++)
                {
                    for (int32_t x = 0; x < context.dimensions.x; x++)
                    {
                        const TileCoordsXY pos{ x, y };
                        if (confluenceMap[pos])
                        {
                            context.debugSigns.emplace_back(
                                pos, std::format("confluence {} {}", distanceMap[pos], auxBackrefsMap[pos].size()),
                                Drawing::Colour::white,
                                springHits[pos].size() == 1 ? Drawing::Colour::darkPurple : Drawing::Colour::brightPurple);
                        }

                        if (hydroMaps.flags[pos].has(skeleton))
                        {
                            context.debugSigns.emplace_back(
                                pos, std::format("skeleton {} {}", distanceMap[pos], auxBackrefsMap[pos].size()),
                                Drawing::Colour::black, Drawing::Colour::white);
                        }
                    }
                }
#endif
                break;
            }
        }

        // set up an inverted river distance map
        DistanceMap riverRadiusMap{ context.dimensions };
        computeHydroFlagBasedDistanceMap(context, riverRadiusMap, river, true);

        // create a mask consisting of disks with radius riverRadiusMap[pos] around the remaining river skeletons
        BooleanMap skeletonMask{ context.dimensions };
        for (int32_t y = 0; y < context.dimensions.y; y++)
        {
            for (int32_t x = 0; x < context.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (!hydroMaps.flags[pos].has(skeleton))
                {
                    continue;
                }

                float maskRadius = riverRadiusMap[pos];
                float maskRadiusSquared = maskRadius * maskRadius;

                for (int32_t dy = -maskRadius; dy <= maskRadius; dy++)
                {
                    for (int32_t dx = -maskRadius; dx <= maskRadius; dx++)
                    {
                        const TileCoordsXY deltaPos{ pos.x + dx, pos.y + dy };
                        if (skeletonMask.inBounds(deltaPos) && dx * dx + dy * dy <= maskRadiusSquared)
                        {
                            skeletonMask[deltaPos] = true;
                        }
                    }
                }
            }
        }

        // unset the river flag for all tiles not in the skeletonMask
        for (int32_t y = 0; y < context.dimensions.y; y++)
        {
            for (int32_t x = 0; x < context.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (!skeletonMask[pos] && hydroMaps.flags[pos].has(river)) // checking river flag for debug signs
                {
                    hydroMaps.flags[pos].unset(river);
#ifdef ENABLE_DEBUG_SIGNS
                    context.debugSigns.emplace_back(
                        pos, "pruned", Drawing::Colour::white,
                        prunedSkeletons.contains(pos) ? Drawing::Colour::lightOrange : Drawing::Colour::brightRed);
#endif
                }
            }
        }
    }

    /**
     * Carve a riverbed around the river increasing with catchment.
     */
    static void carveRiverbed(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HeightMap& heightMap = context.heightMap;
        HydroMaps& hydroMaps = context.hydroMaps.value();
        HeightMap heightCopy = heightMap;
        TrackingStableTileQueue queue{ context.dimensions };
        prepareRiverQueue(context, queue);
        const float seafloorMaxCarveDepth = static_cast<float>(context.settings.waterLevel - kRiversSeafloorMaxCarveDepth);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            const float width = riverWidth(context, tile.pos);
            const float radius = width / 2.0f + 1.0f;
            const float radiusSquared = radius * radius;
            const float depth = riverDepth(width);
            const float riverHeight = heightCopy[tile.pos] - 2.0f;

            for (int32_t dy = -radius; dy <= radius; dy++)
            {
                for (int32_t dx = -radius; dx <= radius; dx++)
                {
                    const float dxSquared = dx * dx;
                    const float dySquared = dy * dy;
                    const float distanceSquared = dxSquared + dySquared;

                    const TileCoordsXY deltaPos = tile.pos + TileCoordsXY{ dx, dy };

                    if (!heightMap.inBounds(deltaPos) || distanceSquared > radiusSquared)
                    {
                        continue;
                    }

                    float candidateHeight = std::max(riverHeight, seafloorMaxCarveDepth);

                    if (hydroMaps.flags[deltaPos].has(river))
                    {
                        const float dz = depth * std::sqrt(1.0f - distanceSquared / radiusSquared);
                        candidateHeight = candidateHeight - dz;
                    }

                    heightMap[deltaPos] = std::min(candidateHeight, heightMap[deltaPos]);

                    if (hydroMaps.flags[deltaPos].has(river))
                    {
                        // set water level to the lowest valid option to avoid possible artifacts at pruned tributaries (min)
                        // while ensuring streams remain connected (max)
                        const float aboveRiverbedLevel = heightMap[deltaPos] + 2.0f;
                        const float referenceLevel = std::min(riverHeight, heightCopy[deltaPos] - 2.0f);
                        const float candidateLevel = std::max(aboveRiverbedLevel, referenceLevel);

                        // if the height was modified previously, keep the min
                        hydroMaps.height[deltaPos] = hydroMaps.height[deltaPos] > 0.0f
                            ? std::min(hydroMaps.height[deltaPos], candidateLevel)
                            : candidateLevel;
                    }
                }
            }

            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!hydroMaps.flags.inBounds(nPos) || !hydroMaps.flags[nPos].has(river) || queue.visited(nPos))
                {
                    continue;
                }

                queue.emplaceAndVisit(nPos, heightCopy[nPos]);
            }
        }
    }

    /**
     * unset river and riverbed flags below sea level for more intuitive behavior in texture/scenery rules.
     */
    static void clearRiversBelowSeaLevel(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HydroMaps& hydroMaps = context.hydroMaps.value();

        const float seafloorMaxCarveDepth = static_cast<float>(context.settings.waterLevel - kRiversSeafloorMaxCarveDepth);

        for (int32_t y = 1; y < context.dimensions.y - 1; y++)
        {
            for (int32_t x = 1; x < context.dimensions.x - 1; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (hydroMaps.height[pos] < seafloorMaxCarveDepth)
                {
                    hydroMaps.flags[pos].unset(river);
                }
            }
        }
    }

    /**
     * Ensure there are no river tiles with lower land neighbours or sinks/non-spring sources.
     *
     * TODO this post-hoc pass shouldn't be needed in the first place...
     */
    static void ensureConsistent(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HydroMaps& hydroMaps = context.hydroMaps.value();

        for (int32_t y = 1; y < context.dimensions.y - 1; y++)
        {
            for (int32_t x = 1; x < context.dimensions.x - 1; x++)
            {
                const TileCoordsXY pos{ x, y };
                context.heightMap[pos] = quantizeHeight(context.heightMap[pos]);
                if (hydroMaps.flags[pos].has(river))
                {
                    hydroMaps.height[pos] = quantizeHeight(hydroMaps.height[pos]);
                }
            }
        }

        for (int32_t y = 1; y < context.dimensions.y - 1; y++)
        {
            for (int32_t x = 1; x < context.dimensions.x - 1; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (!hydroMaps.flags[pos].has(river))
                {
                    float minHeight = context.heightMap[pos];
                    for (const auto& offset : kNeighbourOffsetsOrdinal)
                    {
                        const TileCoordsXY nPos{ pos + offset };
                        if (hydroMaps.flags[nPos].has(river))
                        {
                            minHeight = std::max(minHeight, hydroMaps.height[nPos]);
                        }
                    }

                    context.heightMap[pos] = minHeight;
                }
                else // TODO should be done in a separate pass before checking land neighbour height?
                {
                    bool hasSource = false;
                    bool hasSink = false;
                    bool hasPeer = false;
                    for (const auto& offset : kNeighbourOffsetsOrdinal)
                    {
                        const TileCoordsXY nPos{ pos + offset };
                        if (hydroMaps.flags[nPos].has(river))
                        {
                            if (hydroMaps.height[pos] > hydroMaps.height[nPos])
                            {
                                hasSink = true;
                            }
                            else if (hydroMaps.height[pos] < hydroMaps.height[nPos])
                            {
                                hasSource = true;
                            }
                            else
                            {
                                hasPeer = true;
                            }
                        }
                    }
                    if (!hasPeer)
                    {
                        if (hasSource && !hasSink)
                        {
                            hydroMaps.height[pos] += 2.0f;
                        }
                        else if (!hasSource && hasSink && !hydroMaps.flags[pos].has(spring))
                        {
                            hydroMaps.height[pos] -= 2.0f;
                            context.heightMap[pos] -= 2.0f;
                        }
                    }
                }
            }
        }
    }

    void generateRivers(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        fillOrBreachDepressions(context);
        aggregateCatchment(context);
        postProcessCatchment(context);
        adjustStreamWidth(context);
        ensureCardinalNeighbours(context);
        constructRiverSkeletons(context);
        pruneShortStreams(context);
        carveRiverbed(context);
        clearRiversBelowSeaLevel(context);
        ensureConsistent(context);
    }
} // namespace OpenRCT2::World::MapGenerator::Hydro
