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
#include "../MapHelpers.h"
#include "../MapTraversalUtils.h"
#include "../TileQueue.hpp"
#include "../BaseMap.hpp"
#include "RiverUtils.h"

#include <format>
#include <numbers>
#include <ranges>

// #define ENABLE_DEBUG_SIGNS_SINKS
#define ENABLE_DEBUG_SIGNS_PRUNING
// #define ENABLE_DEBUG_SIGNS_CONSISTENCY

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
    static void fillOrBreachDepressions(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HeightMap& heightMap = context.heightMap;
        HydroMaps& hydroMaps = context.hydroMaps.value();
        BooleanMap pitMap{ context.dimensions };
        BackrefMap backrefMap{ context.dimensions };
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
                    queue.emplaceAndSetMarked(pos, heightMap[pos]);
                    continue;
                }

                float minNeighbour = std::numeric_limits<float>::infinity();

                for (const auto& neighbour : kNeighbours)
                {
                    const TileCoordsXY nPos{ pos + neighbour.offset };
                    // no bounds check needed per if-clause above
                    minNeighbour = std::min(heightMap[nPos], minNeighbour);
                }

                // this is a pit, raise to just below the lowest neighbour to shorten/shallow potential breach path
                // TODO doesn't this fail to detect a pit of equal height neighbours? Fill will take care of it I guess..
                if (heightMap[pos] < minNeighbour)
                {
                    heightMap[pos] = std::nextafter(minNeighbour, std::numeric_limits<float>::lowest());
                    pitMap[pos] = true;
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

                queue.emplaceAndSetMarked(nPos, heightMap[nPos]);
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
                    if (pathLength <= context.settings.breachMaxLength && pathDepth <= context.settings.breachMaxDepth)
                    {
                        while (currentTile.has_value() && heightMap[currentTile.value()] >= targetHeight)
                        {
                            heightMap[currentTile.value()] = targetHeight;
                            hydroMaps.flags[currentTile.value()].set(breached);

                            targetHeight = std::nextafter(targetHeight, std::numeric_limits<float>::lowest());
                            currentTile = backrefMap[currentTile.value()];
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
            if (backrefMap[fillPos].has_value())
            {
                const TileCoordsXY& backrefPos = backrefMap[fillPos].value();

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

        for (const auto&  neighbour : kNeighbours)
        {
            const TileCoordsXY nPos{ from + neighbour.offset };

            if (heightMap.inBounds(nPos))
            {
                sum += downSlope(heightMap, from, nPos);
            }
        }

        return downSlope(heightMap, from, to) / sum;
    }

    static float aggregateNeighbour(MapGenCtx& context, const TileCoordsXY& pos)
    {
        const HeightMap& heightMap = context.heightMap;
        HydroMaps& hydroMaps = context.hydroMaps.value();

        if (hydroMaps.catchment[pos] <= 0.0f)
        {
            const int32_t multiplier = isInWorldMap(context, pos) ? 1 : context.settings.offMapCatchmentMultiplier;

            hydroMaps.catchment[pos] = 1.0f * multiplier;
            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ pos + neighbour.offset };

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

    static void fillFlowMaps(MapGenCtx& context, const TileCoordsXY& pos)
    {
        HydroMaps& hydroMaps = context.hydroMaps.value();
        for (const auto& neighbour : kNeighbours)
        {
            const TileCoordsXY nPos{ pos + neighbour.offset };

            if (!context.heightMap.inBounds(nPos))
            {
                continue;
            }

            if (context.heightMap[pos] > context.heightMap[nPos])
            {
                hydroMaps.flowsIn[nPos].set(neighbour.opposite);
                hydroMaps.flowsOut[pos].set(neighbour.direction);
            }
            else if (context.heightMap[pos] < context.heightMap[nPos])
            {
                hydroMaps.flowsIn[pos].set(neighbour.direction);
                hydroMaps.flowsOut[nPos].set(neighbour.opposite);
            }
            else
            {
                hydroMaps.flowsLateral[pos].set(neighbour.direction);
                hydroMaps.flowsLateral[nPos].set(neighbour.opposite);
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
    static void aggregateCatchment(MapGenCtx& context)
    {
        PROFILED_FUNCTION();

        for (int32_t y = 0; y < context.heightMap.height; y++)
        {
            for (int32_t x = 0; x < context.heightMap.width; x++)
            {
                fillFlowMaps(context, { x, y });
            }
        }
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
            queue.emplaceAndSetMarked(pos, context.heightMap[pos]);
            hydroMaps.flags[pos].set(river, skeleton);
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

            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };
                if (hydroMaps.flags.inBounds(nPos) && !queue.isMarked(nPos))
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
            // the algorithm doesn't handle edge coords, resulting in 'T' shapes being left behind, should be ok for this.
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
                        // coordinates differ from the paper to account for inverted x-axis of rct
                    } nb = { .p2 = hydroMaps.flags[p1 + TileCoordsXY{ 0, -1 }].has(skeleton),
                             .p3 = hydroMaps.flags[p1 + TileCoordsXY{ 1, -1 }].has(skeleton),
                             .p4 = hydroMaps.flags[p1 + TileCoordsXY{ 1, 0 }].has(skeleton),
                             .p5 = hydroMaps.flags[p1 + TileCoordsXY{ 1, 1 }].has(skeleton),
                             .p6 = hydroMaps.flags[p1 + TileCoordsXY{ 0, 1 }].has(skeleton),
                             .p7 = hydroMaps.flags[p1 + TileCoordsXY{ -1, 1 }].has(skeleton),
                             .p8 = hydroMaps.flags[p1 + TileCoordsXY{ -1, 0 }].has(skeleton),
                             .p9 = hydroMaps.flags[p1 + TileCoordsXY{ -1, -1 }].has(skeleton) };

                    int32_t bOfP1 = 0; // count neighbours
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

                    int32_t aOfP1 = 0; // count clockwise 0->1 transitions
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
            firstSubiteration = !firstSubiteration; // alternate between subiterations
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
        return std::max(2.0f, 8.0f * depth); // rescale for rct
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
        primeHydroFlagQueue(context, queue, { river, QueueMode::height });

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };

                if (!hydroMaps.flags.inBounds(nPos) || queue.isMarked(nPos) || !hydroMaps.flags[nPos].has(river))
                {
                    continue;
                }

                queue.emplaceAndSetMarked(nPos, heightMap[nPos]);
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
                    hydroMaps.flags[deltaPos].set(river, skeleton);
                    queue.setMarked(deltaPos);
                }
            }
        }
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
     * Ensure diagonal channels render nicely by asserting each river tile has at least one ordinal neighbour.
     * Uses the game coordinate convention, i.e. the diagonal neighbours are cardinal directions.
     */
    static void ensureCardinalNeighbours(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HeightMap& heightMap = context.heightMap;
        HydroMaps& hydroMaps = context.hydroMaps.value();
        TrackingStableTileQueue queue{ context.dimensions };
        primeHydroFlagQueue(context, queue, { river, QueueMode::height });

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& neighbour : kNeighboursCardinal)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };

                if (!hydroMaps.flags.inBounds(nPos) || !hydroMaps.flags[nPos].has(river))
                {
                    continue;
                }

                if (!queue.isMarked(nPos))
                {
                    queue.emplaceAndSetMarked(nPos, heightMap[nPos]);
                }

                if (!haveCommonOrdinalNeighbour(hydroMaps, tile.pos, neighbour.offset))
                {
                    for (const TileCoordsXY& ordinalOffset : ordinalNeighbours(neighbour.offset))
                    {
                        const TileCoordsXY sharedOrdinalPos{ tile.pos + ordinalOffset };
                        heightMap[sharedOrdinalPos] = heightMap[tile.pos];
                        hydroMaps.catchment[sharedOrdinalPos] = context.settings.catchmentThreshold;
                        hydroMaps.flags[sharedOrdinalPos].set(river, skeleton);
                        queue.emplaceAndSetMarked(sharedOrdinalPos, heightMap[sharedOrdinalPos]);
                    }
                }
            }

            for (const auto& neighbour : kNeighboursOrdinal)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };

                if (hydroMaps.flags.inBounds(nPos) && !queue.isMarked(nPos) && hydroMaps.flags[nPos].has(river))
                {
                    queue.emplaceAndSetMarked(nPos, heightMap[nPos]);
                }
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

#ifdef ENABLE_DEBUG_SIGNS_PRUNING
        int32_t prunedTotal = 0;
        int32_t iteration = 0;
#endif

        // identify sources and sinks and connect them to the river skeletons
        TileCoordsXYSet sources;
        TileCoordsXYSet sinks;
        prepareSourcesAndSinksForPruning(context, sources, sinks);

        // iterate until no streams are pruned
        while (true)
        {
            // identify confluences and bifurcations
            BackrefMap mainUpstreamMap{ context.dimensions };
            BackrefMap mainDownstreamMap{ context.dimensions };
            BackrefsMap auxUpstreamsMap{ context.dimensions };
            BackrefsMap auxDownstreamsMap{ context.dimensions };
            BooleanMap confluenceMap{ context.dimensions };
            BooleanMap bifurcationMap{ context.dimensions };
            prepareConfluencesAndBifurcationsForPruning(
                context, mainUpstreamMap, mainDownstreamMap, auxUpstreamsMap, auxDownstreamsMap, confluenceMap, bifurcationMap);

            // populate sourceHits and sinkHits maps and the confluenceMaxSpringMap and unmark single source confluences as well
            // as single sink bifurcations
            BackrefsMap sourceHits{ context.dimensions };
            BackrefsMap sinkHits{ context.dimensions };
            BackrefMap confluenceMaxSpringMap{ context.dimensions };
            traceFromSources(context, sources, sourceHits, auxDownstreamsMap, confluenceMap, confluenceMaxSpringMap);
            traceFromSinks(context, sinks, sourceHits, auxDownstreamsMap, confluenceMap);

            // prune
            int32_t prunedSources = pruneSources(
                context, sources, mainDownstreamMap, auxDownstreamsMap, confluenceMap, sourceHits, confluenceMaxSpringMap);
            int32_t prunedSinks = pruneSinks(context, sinks, mainUpstreamMap, auxUpstreamsMap, bifurcationMap, sinkHits);

            // no streams pruned in this iteration, mark remaining src/sink and break from loop
            if (prunedSources + prunedSinks == 0)
            {
                for (const auto& source : sources)
                {
                    hydroMaps.flags[source].set(HydroFlag::spring);
#ifdef ENABLE_DEBUG_SIGNS_PRUNING
                    context.debugSigns.emplace_back(
                        source,
                        std::format(
                            "src {}-{}-{}-{}-{}", auxDownstreamsMap[source].size(), auxUpstreamsMap[source].size(),
                            sourceHits[source].size(), sinkHits[source].size(), context.heightMap[source]),
                        Drawing::Colour::white, Drawing::Colour::lightBlue);
#endif
                }
                for (const auto& sink : sinks)
                {
#ifdef ENABLE_DEBUG_SIGNS_PRUNING
                    context.debugSigns.emplace_back(
                        sink,
                        std::format(
                            "sink {}-{}-{}-{}-{}", auxDownstreamsMap[sink].size(), auxUpstreamsMap[sink].size(),
                            sourceHits[sink].size(), sinkHits[sink].size(), context.heightMap[sink]),
                        Drawing::Colour::white, Drawing::Colour::peach);
#endif
                }
#ifdef ENABLE_DEBUG_SIGNS_PRUNING
                for (int32_t y = 0; y < context.dimensions.y; y++)
                {
                    for (int32_t x = 0; x < context.dimensions.x; x++)
                    {
                        const TileCoordsXY pos{ x, y };
                        if (confluenceMap[pos] && bifurcationMap[pos])
                        {
                            context.debugSigns.emplace_back(
                                pos,
                                std::format(
                                    "co/bi {}-{}-{}-{}-{}", auxDownstreamsMap[pos].size(), auxUpstreamsMap[pos].size(),
                                    sourceHits[pos].size(), sinkHits[pos].size(), context.heightMap[pos]),
                                Drawing::Colour::white, Drawing::Colour::brightYellow);
                        }
                        else if (confluenceMap[pos])
                        {
                            context.debugSigns.emplace_back(
                                pos,
                                std::format(
                                    "conf {}-{}-{}-{}-{}", auxDownstreamsMap[pos].size(), auxUpstreamsMap[pos].size(),
                                    sourceHits[pos].size(), sinkHits[pos].size(), context.heightMap[pos]),
                                Drawing::Colour::white, Drawing::Colour::brightPurple);
                        }
                        else if (bifurcationMap[pos])
                        {
                            context.debugSigns.emplace_back(
                                pos,
                                std::format(
                                    "bifu {}-{}-{}-{}-{}", auxDownstreamsMap[pos].size(), auxUpstreamsMap[pos].size(),
                                    sourceHits[pos].size(), sinkHits[pos].size(), context.heightMap[pos]),
                                Drawing::Colour::white, Drawing::Colour::brightGreen);
                        }
                        else if (hydroMaps.flags[pos].has(skeleton))
                        {
                            context.debugSigns.emplace_back(
                                pos,
                                std::format(
                                    "skel {}-{}-{}-{}-{}", auxDownstreamsMap[pos].size(), auxUpstreamsMap[pos].size(),
                                    sourceHits[pos].size(), sinkHits[pos].size(), context.heightMap[pos]),
                                Drawing::Colour::black, Drawing::Colour::white);
                        }
                    }
                }
#endif
                break;
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
#ifdef ENABLE_DEBUG_SIGNS_PRUNING
                        context.debugSigns.emplace_back(pos, "pruned", Drawing::Colour::white, Drawing::Colour::brightRed);
#endif
                    }
                }
            }
#ifdef ENABLE_DEBUG_SIGNS_PRUNING
            iteration++;
            prunedTotal += prunedSources + prunedSinks;
            LOG_INFO("iter=%d prunedSources=%d prunedSinks=%d", iteration, prunedSources, prunedSinks);
#endif
        }

#ifdef ENABLE_DEBUG_SIGNS_PRUNING
        LOG_INFO("pruned %d springs in %d iterations", prunedTotal, iteration);
#endif
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
        primeHydroFlagQueue(context, queue, { river, QueueMode::height });
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

            hydroMaps.height[tile.pos] = riverHeight;

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
                }
            }

            for (const auto& neighbour : kNeighbours)
            {
                const TileCoordsXY nPos{ tile.pos + neighbour.offset };

                if (!hydroMaps.flags.inBounds(nPos) || !hydroMaps.flags[nPos].has(river) || queue.isMarked(nPos))
                {
                    continue;
                }

                queue.emplaceAndSetMarked(nPos, heightCopy[nPos]);
            }
        }
    }

    enum class ConsistencyOperation
    {
        raise,
        lower,
        remove
    };

    struct SegmentKey
    {
        TileCoordsXY pos;
        size_t size;
        ConsistencyOperation operation;

        friend bool operator==(const SegmentKey& lhs, const SegmentKey& rhs)
        {
            return lhs.pos == rhs.pos && lhs.size == rhs.size && lhs.operation == rhs.operation;
        }
        friend bool operator!=(const SegmentKey& lhs, const SegmentKey& rhs)
        {
            return !(lhs == rhs);
        }
        friend bool operator<(const SegmentKey& lhs, const SegmentKey& rhs)
        {
            if (lhs.size < rhs.size)
            {
                return true;
            }
            if (rhs.size < lhs.size)
            {
                return false;
            }
            return lhs.operation < rhs.operation;
        }
        friend bool operator<=(const SegmentKey& lhs, const SegmentKey& rhs)
        {
            return !(rhs < lhs);
        }
        friend bool operator>(const SegmentKey& lhs, const SegmentKey& rhs)
        {
            return rhs < lhs;
        }
        friend bool operator>=(const SegmentKey& lhs, const SegmentKey& rhs)
        {
            return !(lhs < rhs);
        }
    };

    struct SegmentKeyHash
    {
        size_t operator()(const SegmentKey& s) const noexcept
        {
            size_t hash = 0;
            Util::Hash::update(hash, s.pos.x);
            Util::Hash::update(hash, s.pos.y);
            Util::Hash::update(hash, s.size);
            Util::Hash::update(hash, s.operation);
            return hash;
        }
    };

    /**
     * Ensure there are no river tiles with lower land neighbours or sinks/non-spring sources.
     */
    static void ensureConsistent(MapGenCtx& context)
    {
        PROFILED_FUNCTION();

        HydroMaps& hydroMaps = context.hydroMaps.value();

        // quantize heights for segmentation
        for (int32_t y = 0; y < context.dimensions.y; y++)
        {
            for (int32_t x = 0; x < context.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                context.heightMap[pos] = quantizeHeight(context.heightMap[pos]);
                if (hydroMaps.flags[pos].has(river))
                {
                    hydroMaps.height[pos] = quantizeHeight(hydroMaps.height[pos]);
                }
#ifdef ENABLE_DEBUG_SIGNS_CONSISTENCY
                if (hydroMaps.flags[pos].has(spring))
                {
                    context.debugSigns.emplace_back(pos, "spring", Drawing::Colour::white, Drawing::Colour::black);
                }
#endif
            }
        }

#ifdef ENABLE_DEBUG_SIGNS_CONSISTENCY
        int32_t iteration = 1;
#endif
        // find and handle isolated segments
        while (true)
        {
            BooleanMap visited{ context.dimensions };
            std::unordered_map<SegmentKey, TileCoordsXYSet, SegmentKeyHash> segments;

            for (int32_t y = 0; y < context.dimensions.y; y++)
            {
                for (int32_t x = 0; x < context.dimensions.x; x++)
                {
                    const TileCoordsXY pos{ x, y };
                    if (!hydroMaps.flags[pos].has(river) || visited[pos])
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
                    bool belowSeaLevel = hydroMaps.height[pos] <= context.settings.waterLevel;

                    while (!queue.empty())
                    {
                        TileCoordsXY qPos = queue.front();
                        queue.pop();

                        if (hydroMaps.flags.onEdge(qPos))
                        {
                            hasSink = true;
                            // hasSource = true; // TODO is this needed?
                        }

                        if (hydroMaps.flags[qPos].has(spring))
                        {
                            hasSource = true;
                        }

                        for (const auto& neighbour : kNeighboursOrdinal)
                        {
                            const TileCoordsXY nPos{ qPos + neighbour.offset };
                            if (hydroMaps.flags.inBounds(nPos) && hydroMaps.flags[nPos].has(river))
                            {
                                if (hydroMaps.height[qPos] > hydroMaps.height[nPos])
                                {
                                    hasSink = true;
                                }
                                else if (hydroMaps.height[qPos] < hydroMaps.height[nPos])
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
                        segments[{ pos, segment.size(), ConsistencyOperation::remove }] = segment;
                    }
                    else if (!hasSource && hasSink && !belowSeaLevel)
                    {
                        segments[{ pos, segment.size(), ConsistencyOperation::lower }] = segment;
                    }
                    else if (hasSource && !hasSink && !belowSeaLevel)
                    {
                        segments[{ pos, segment.size(), ConsistencyOperation::raise }] = segment;
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
                        hydroMaps.height[sPos] -= 2.0f;
                        context.heightMap[sPos] = std::min(hydroMaps.height[sPos] - 2.0f, context.heightMap[sPos]);

                        if (hydroMaps.height[sPos] < 0.0f)
                        {
                            throw std::runtime_error(std::format("({},{}) lowered below 0", sPos.x, sPos.y));
                        }
                    }
#ifdef ENABLE_DEBUG_SIGNS_CONSISTENCY
                    context.debugSigns.emplace_back(
                        candidate.pos, std::format("lower i={} n={}", iteration, candidate.size), Drawing::Colour::white,
                        Drawing::Colour::lightBlue);
                    TileCoordsXY worldCoords = genCoordsToWorldCoords(context, candidate.pos);
                    LOG_INFO(
                        "%d | of n=%d lower segment at (%d,%d) size=%d", iteration, segments.size(), worldCoords.x,
                        worldCoords.y, candidate.size);
#endif
                    break;
                }
                case ConsistencyOperation::raise:
                {
                    for (const TileCoordsXY& sPos : segments[candidate])
                    {
                        hydroMaps.height[sPos] += 2.0f;

                        if (hydroMaps.height[sPos] >= 256.0f)
                        {
                            throw std::runtime_error(std::format("({},{}) raised above 256", sPos.x, sPos.y));
                        }
                    }
#ifdef ENABLE_DEBUG_SIGNS_CONSISTENCY
                    context.debugSigns.emplace_back(
                        candidate.pos, std::format("raise i={} n={}", iteration, candidate.size), Drawing::Colour::white,
                        Drawing::Colour::lightPink);
                    TileCoordsXY worldCoords = genCoordsToWorldCoords(context, candidate.pos);
                    LOG_INFO(
                        "%d | of n=%d raise segment at (%d,%d) size=%d", iteration, segments.size(), worldCoords.x,
                        worldCoords.y, candidate.size);
#endif
                    break;
                }
                case ConsistencyOperation::remove:
                {
                    for (const TileCoordsXY& sPos : segments[candidate])
                    {
                        hydroMaps.height[sPos] = 0.0f;
                        hydroMaps.flags[sPos].unset(river);
                    }
#ifdef ENABLE_DEBUG_SIGNS_CONSISTENCY
                    context.debugSigns.emplace_back(
                        candidate.pos, std::format("rm i={} n={}", iteration, candidate.size), Drawing::Colour::white,
                        Drawing::Colour::lightOrange);
                    TileCoordsXY worldCoords = genCoordsToWorldCoords(context, candidate.pos);
                    LOG_INFO(
                        "%d | of n=%d remove segment at (%d,%d) size=%d", iteration, segments.size(), worldCoords.x,
                        worldCoords.y, candidate.size);
#endif
                    break;
                }
            }
#ifdef ENABLE_DEBUG_SIGNS_CONSISTENCY
            iteration++;
#endif
        }

#ifdef ENABLE_DEBUG_SIGNS_CONSISTENCY
        LOG_INFO("consistency iteration=%d", iteration);
#endif
        // make sure waterfalls are properly enclosed
        for (int32_t y = 0; y < context.dimensions.y; y++)
        {
            for (int32_t x = 0; x < context.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (!hydroMaps.flags[pos].has(river))
                {
                    float minHeight = context.heightMap[pos];
                    for (const auto& neighbour : kNeighboursOrdinal)
                    {
                        const TileCoordsXY nPos{ pos + neighbour.offset };
                        if (hydroMaps.flags.inBounds(nPos) && hydroMaps.flags[nPos].has(river))
                        {
                            minHeight = std::max(minHeight, hydroMaps.height[nPos]);
                        }
                    }

                    context.heightMap[pos] = minHeight;
                }
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

        for (int32_t y = 0; y < context.dimensions.y; y++)
        {
            for (int32_t x = 0; x < context.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };
                if (hydroMaps.height[pos] < seafloorMaxCarveDepth)
                {
                    hydroMaps.flags[pos].unset(river);
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
        ensureConsistent(context);
        clearRiversBelowSeaLevel(context);
    }
} // namespace OpenRCT2::World::MapGenerator::Hydro
