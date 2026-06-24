/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "River.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../GameState.h"
#include "../../profiling/Profiling.h"
#include "MapHelpers.h"
#include "TileQueue.hpp"

#include <numbers>

namespace OpenRCT2::World::MapGenerator
{
    static constexpr float kP = 1.1f;

    using Backref = std::optional<TileCoordsXY>;

    struct TileState
    {
        // TODO use an uint8_t to encode the kNeighbourOffsets index
        Backref backref;
        bool visited;
        bool pit;
    };

    using BackrefMap = BaseMap<Backref>;
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

        StateMap state(hydroMaps.dimensions);
        StableTileQueue queue;
        std::queue<TileCoordsXY> fillQueue;

        // prepare queue and mark pits
        for (int32_t y = 0; y < heightMap.height; y++)
        {
            for (int32_t x = 0; x < heightMap.width; x++)
            {
                TileCoordsXY pos{ x, y };

                if (x == 0 || y == 0 || x == heightMap.width - 1 || y == heightMap.height - 1)
                {
                    queue.emplace(pos, heightMap[pos]);
                    state[pos].visited = true;
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

                if (!heightMap.inBounds(nPos) || state[nPos].visited)
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                fillQueue.push(nPos);
                state[nPos].visited = true;
                state[nPos].backref = std::make_optional(tile.pos);

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
        const bool isCardinal = from.x == 0 || from.y == 0 || to.x == 0 || to.y == 0;
        const float distance = isCardinal ? 1.0f : std::numbers::sqrt2;
        const float slope = (heightMap[from] - heightMap[to]) / distance;
        return std::max(0.0f, std::pow(slope, kP));
    }

    static float flowFraction(const HeightMap& heightMap, const TileCoordsXY& from, const TileCoordsXY& to)
    {
        float sum = 0.0f;

        for (const auto& offset : kNeighbourOffsets)
        {
            TileCoordsXY nPos{ from + offset };

            if (!heightMap.inBounds(nPos))
            {
                continue;
            }

            sum += downSlope(heightMap, from, nPos);
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
                TileCoordsXY nPos{ pos + offset };

                if (!heightMap.inBounds(nPos))
                {
                    continue;
                }

                if (heightMap[nPos] > heightMap[pos])
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
                TileCoordsXY pos{ x, y };
                aggregateNeighbour(context, pos);
            }
        }
    }

    static void postProcessTile(
        MapGenCtx& context, StableTileQueue& queue, MaskMap& visited, BackrefMap& backrefMap, Backref backref,
        const TileCoordsXY& pos)
    {
        HydroMaps& hydroMaps = context.hydroMaps.value();

        if (hydroMaps.catchment[pos] < context.settings.catchmentThreshold)
        {
            return;
        }

        queue.emplace(pos, context.heightMap[pos]);
        visited[pos] = Mask::True;
        backrefMap[pos] = backref;

        hydroMaps.flags[pos].set(river);
        hydroMaps.flags[pos].set(skeleton);
    }

    /**
     * Applies the catchment threshold and implicitly removes orphans.
     */
    static void postProcessCatchment(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        const HydroMaps& hydroMaps = context.hydroMaps.value();

        StableTileQueue queue;
        MaskMap visited(hydroMaps.dimensions);
        BackrefMap backrefMap{ hydroMaps.dimensions };

        for (int32_t y = 0; y < hydroMaps.dimensions.y; y++)
        {
            postProcessTile(context, queue, visited, backrefMap, std::nullopt, { 0, y });
            postProcessTile(context, queue, visited, backrefMap, std::nullopt, { hydroMaps.dimensions.x - 1, y });
        }

        for (int32_t x = 1; x < hydroMaps.dimensions.x - 1; x++)
        {
            postProcessTile(context, queue, visited, backrefMap, std::nullopt, { x, 0 });
            postProcessTile(context, queue, visited, backrefMap, std::nullopt, { x, hydroMaps.dimensions.y - 1 });
        }

        // process tiles and find springs
        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!hydroMaps.flags.inBounds(nPos) || visited[nPos] == Mask::True)
                {
                    continue;
                }

                postProcessTile(context, queue, visited, backrefMap, tile.pos, nPos);
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
            for (int32_t y = 1; y < hydroMaps.dimensions.y - 1; y++)
            {
                for (int32_t x = 1; x < hydroMaps.dimensions.x - 1; x++)
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
    static void prepareRiverQueue(
        MapGenCtx& context, StableTileQueue& queue, MaskMap& visited,
        const HydroFlag flag = river)
    {
        HydroMaps& hydroMaps = context.hydroMaps.value();

        for (int32_t y = 0; y < hydroMaps.dimensions.y - 1; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (hydroMaps.flags[left].has(flag))
            {
                queue.emplace(left, context.heightMap[left]);
                visited[left] = Mask::True;
            }

            const TileCoordsXY right{ hydroMaps.dimensions.x - 1, y };
            if (hydroMaps.flags[right].has(flag))
            {
                queue.emplace(right, context.heightMap[right]);
                visited[right] = Mask::True;
            }
        }

        for (int32_t x = 1; x < hydroMaps.dimensions.x - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (hydroMaps.flags[top].has(flag))
            {
                queue.emplace(top, context.heightMap[top]);
                visited[top] = Mask::True;
            }

            const TileCoordsXY bottom{ x, hydroMaps.dimensions.y - 1 };
            if (hydroMaps.flags[bottom].has(flag))
            {
                queue.emplace(bottom, context.heightMap[bottom]);
                visited[bottom] = Mask::True;
            }
        }
    }

    /**
     * Returns the two cardinal neighbours for the given ordinal offset.
     */
    static std::array<TileCoordsXY, 2> cardinalNeighbours(const TileCoordsXY& offset)
    {
        return {
            TileCoordsXY{ 0, offset.y },
            TileCoordsXY{ offset.x, 0 },
        };
    }

    /**
     * Checks if the tile at the offset from the given position share a cardinal neighbour with the given flag.
     */
    static bool haveCommonCardinalNeighbour(
        const HydroMaps& hydroMaps, const TileCoordsXY& pos, const TileCoordsXY& offset, const HydroFlag flag = river)
    {
        for (const TileCoordsXY& scnOffset : cardinalNeighbours(offset))
        {
            const TileCoordsXY scnPos{ pos + scnOffset };

            if (hydroMaps.flags[scnPos].has(flag))
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
        MapGenCtx& context, StableTileQueue& queue, MaskMap& visited, BackrefMap& backrefMap,
        DistanceMap& distanceMap, const TileCoordsXY& pos, int32_t& upstreamCount,int32_t& blockedCount, const TileCoordsXY& offset,
        const bool isCardinal)
    {
        const TileCoordsXY nPos{ pos + offset };

        if (!context.hydroMaps.value().flags.inBounds(nPos) || !context.hydroMaps.value().flags[nPos].has(skeleton))
        {
            return;
        }

        if (visited[nPos] == Mask::True)
        {
            if (context.heightMap[nPos] >= context.heightMap[pos]) // bifurcations
            {
                blockedCount++;
            }
            return;
        }

        // a bit of a workaround, the thinning algorithm leaves T-junctions instead of reducing to Y), prefer the ordinal
        if (isCardinal && haveCommonCardinalNeighbour(context.hydroMaps.value(), pos, offset, skeleton))
        {
            blockedCount++;
            return;
        }

        backrefMap[nPos] = pos;
        queue.emplace(nPos, context.heightMap[nPos]);
        visited[nPos] = Mask::True;
        upstreamCount++;
        distanceMap[nPos] = distanceMap[pos] + 1.0f;
    }

    /**
     * Prunes tributary streams that are shorter than kMaxPruneLength.
     * This does a bit of a dance, the more straightforward solution of only using the skeleton to reconstruct the river in the
     * widen step below produces rather ugly rivers as much of the variation from the flow-fraction aggregation is lost.
     * So instead keep track of the nearest skeleton tile for each river tile and unset if the skeleton tile was pruned.
     * The pruning itself works by first identifying springs and confluences. Next the max length spring for each confluence is
     * identified. Finally prune from each spring if the distance to the nearest confluence is below the threshold and the
     * spring isn't the furthest for the confluence.
     */
    static void pruneShortStreams(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HydroMaps& hydroMaps = context.hydroMaps.value();
        BackrefMap nearestSkeletonMap{ hydroMaps.dimensions };

        // setup nearest skeleton map for later
        for (int32_t y = 0; y < hydroMaps.dimensions.y ; y++)
        {
            for (int32_t x = 0; x < hydroMaps.dimensions.x ; x++)
            {
                TileCoordsXY pos{ x, y };
                if (!hydroMaps.flags[pos].has(river))
                {
                    continue;
                }

                if (hydroMaps.flags[pos].has(skeleton))
                {
                    nearestSkeletonMap[pos] = pos;
                    continue;
                }

                // TODO this is kinda dumb
                std::queue<TileCoordsXY> findSkeletonQueue;
                MaskMap findSkeletonVisited{hydroMaps.dimensions};

                findSkeletonQueue.emplace(pos);
                findSkeletonVisited[pos] = Mask::True;

                while (!findSkeletonQueue.empty())
                {
                    TileCoordsXY candidateSkeletonPos = findSkeletonQueue.front();
                    findSkeletonQueue.pop();

                    if (hydroMaps.flags[candidateSkeletonPos].has(skeleton))
                    {
                        nearestSkeletonMap[pos] = candidateSkeletonPos;
                        break;
                    }

                    for (const auto& offset : kNeighbourOffsets)
                    {
                        TileCoordsXY candidateSkeletonNeighborPos = candidateSkeletonPos + offset;

                        if (!hydroMaps.flags.inBounds(candidateSkeletonNeighborPos)
                            || !hydroMaps.flags[candidateSkeletonNeighborPos].has(river))
                        {
                            continue;
                        }

                        findSkeletonQueue.emplace(candidateSkeletonNeighborPos);
                        findSkeletonVisited[candidateSkeletonNeighborPos] = Mask::True;
                    }
                }
            }
        }

        int32_t iterations = 0;
        while (true)
        {
            StableTileQueue queue;
            MaskMap visited{ hydroMaps.dimensions };
            prepareRiverQueue(context, queue, visited, skeleton);

            BackrefMap backrefMap{ hydroMaps.dimensions };
            MaskMap confluenceMap{ hydroMaps.dimensions };
            BackrefMap confluenceMaxSpringMap{ hydroMaps.dimensions };
            DistanceMap distanceMap{ hydroMaps.dimensions };

            std::vector<TileCoordsXY> springs;

            // process tiles and find springs
            while (!queue.empty())
            {
                const QueueTile tile = queue.top();
                queue.pop();


                int32_t upstreamCount = 0;
                int32_t blockedCount = 0;

                for (const auto& offset : kNeighbourOffsetsOrdinal)
                {
                    pruneVisit(context, queue, visited, backrefMap, distanceMap, tile.pos, upstreamCount, blockedCount, offset, false);
                }
                for (const auto& offset : kNeighbourOffsetsCardinal)
                {
                    pruneVisit(context, queue, visited, backrefMap, distanceMap, tile.pos, upstreamCount, blockedCount, offset, true);
                }

                if (upstreamCount > 1)
                {
                    confluenceMap[tile.pos] = Mask::True;
                }
                else if (upstreamCount == 0 && blockedCount == 0)
                {
                    springs.push_back(tile.pos);
                }
            }

            // find furthest spring for each confluence
            for (const auto& spring : springs)
            {
                std::optional<TileCoordsXY> currentTile = std::make_optional(spring);
                while (currentTile.has_value())
                {
                    if (confluenceMap[currentTile.value()] == Mask::True)
                    {
                        const Backref& maybeMaxSpring = confluenceMaxSpringMap[currentTile.value()];
                        if (!maybeMaxSpring.has_value() || distanceMap[spring] > distanceMap[maybeMaxSpring.value()])
                        {
                            confluenceMaxSpringMap[currentTile.value()] = spring;
                        }
                        else
                        {
                            break;
                        }
                    }
                    currentTile = backrefMap[currentTile.value()];
                }
            }

            bool pruned = false;
            // prune if length is below limit and isn't the furthest spring
            for (const auto& spring : springs)
            {
                bool prune = false;
                std::optional<TileCoordsXY> currentTile = std::make_optional(spring);

                while (currentTile.has_value())
                {
                    if (distanceMap[spring] - distanceMap[currentTile.value()] > context.settings.maxPruneLength)
                    {
                        break;
                    }

                    // checking next here to also prune short streams at the map edge
                    const auto& next = backrefMap[currentTile.value()];
                    if (confluenceMap[currentTile.value()] == Mask::True || !next.has_value())
                    {
                        const auto& maybeMaxSpring = confluenceMaxSpringMap[currentTile.value()];
                        if (!maybeMaxSpring.has_value() || maybeMaxSpring.value() != spring)
                        {
                            prune = true;
                        }
                        break;
                    }
                    currentTile = next;
                }

                if (prune)
                {
                    currentTile = std::make_optional(spring);
                    while (currentTile.has_value() && confluenceMap[currentTile.value()] != Mask::True)
                    {
                        hydroMaps.flags[currentTile.value()].unset(skeleton);
                        currentTile = backrefMap[currentTile.value()];
                    }
                    pruned = true;
                }
            }

            if (!pruned)
            {
                break;
            }

            iterations+=1;
        }

        LOG_INFO("prune iterations=%d", iterations );

        // throw out river tiles that point to a no-longer-skeleton as their nearest skeleton.
        for (int32_t y = 0; y < hydroMaps.dimensions.y ; y++)
        {
            for (int32_t x = 0; x < hydroMaps.dimensions.x ; x++)
            {
                TileCoordsXY pos{ x, y };
                if (!hydroMaps.flags[pos].has(river))
                {
                    continue;
                }

                if (nearestSkeletonMap[pos].has_value() && !hydroMaps.flags[nearestSkeletonMap[pos].value()].has(skeleton))
                {
                    hydroMaps.flags[pos].unset(river);
                }
            }
        }
    }

    /**
     * Slightly widen the river based on catchment.
     */
    static void adjustStreamWidth(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        const Settings& settings = context.settings;
        HeightMap& heightMap = context.heightMap;
        HydroMaps& hydroMaps = context.hydroMaps.value();

        StableTileQueue queue;
        MaskMap visited(hydroMaps.dimensions);

        DistanceMap distanceMap(hydroMaps.dimensions);
        distanceMap.fill(std::numeric_limits<float>::infinity());

        prepareRiverQueue(context, queue, visited, river);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!hydroMaps.flags.inBounds(nPos) || visited[nPos] == Mask::True || !hydroMaps.flags[nPos].has(river))
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                visited[nPos] = Mask::True;
            }

            const float radius = std::log2(std::log2(hydroMaps.catchment[tile.pos] / settings.catchmentThreshold));
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
                    visited[deltaPos] = Mask::True;
                }
            }
        }
    }

    /**
     * Ensure diagonal channels render nicely by asserting each river tile has at least one cardinal neighbour.
     */
    static void ensureCardinalNeighbours(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HeightMap& heightMap = context.heightMap;
        HydroMaps& hydroMaps = context.hydroMaps.value();

        StableTileQueue queue;
        MaskMap visited(hydroMaps.dimensions);

        prepareRiverQueue(context, queue, visited);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsetsCardinal)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!hydroMaps.flags.inBounds(nPos) || visited[nPos] == Mask::True || !hydroMaps.flags[nPos].has(river))
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                visited[nPos] = Mask::True;

                if (haveCommonCardinalNeighbour(hydroMaps, tile.pos, offset))
                {
                    continue;
                }

                for (const TileCoordsXY& scnOffset : cardinalNeighbours(offset))
                {
                    const TileCoordsXY scnPos{ tile.pos + scnOffset };
                    heightMap[scnPos] = heightMap[tile.pos];
                    hydroMaps.catchment[scnPos] = context.settings.catchmentThreshold;
                    hydroMaps.flags[scnPos].set(river);
                    visited[scnPos] = Mask::True;
                }
            }

            for (const auto& offset : kNeighbourOffsetsOrdinal)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!hydroMaps.flags.inBounds(nPos) || visited[nPos] == Mask::True || !hydroMaps.flags[nPos].has(river))
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                visited[nPos] = Mask::True;
            }
        }
    }

    /**
     * Carve a riverbed around the river increasing with catchment.
     */
    static void carveRiverbed(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        const Settings& settings = context.settings;
        HeightMap& heightMap = context.heightMap;
        HydroMaps& hydroMaps = context.hydroMaps.value();

        HeightMap heightCopy = heightMap;

        StableTileQueue queue;
        MaskMap visited(hydroMaps.dimensions);

        prepareRiverQueue(context, queue, visited);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            const float radius = std::log2(std::log2(hydroMaps.catchment[tile.pos] / settings.catchmentThreshold)) + 1.0f;
            const float radiusMinusOne = radius - 1.0f;
            const float radiusSquared = radius * radius;
            const float radiusMinusOneSquared = radiusMinusOne * radiusMinusOne;

            hydroMaps.flags[tile.pos].set(riverbed);
            hydroMaps.height[tile.pos] = std::floor(heightCopy[tile.pos] - 2.0f);
            heightMap[tile.pos] = std::floor(heightCopy[tile.pos] - (4.0f + std::max(0.0f, radius * 0.66f)));

            for (int32_t dy = -radius; dy <= radius; dy++)
            {
                for (int32_t dx = -radius; dx <= radius; dx++)
                {
                    const TileCoordsXY deltaPos = tile.pos + TileCoordsXY{ dx, dy };
                    const auto distance = dx * dx + dy * dy;

                    if (!heightMap.inBounds(deltaPos) || hydroMaps.flags[deltaPos].has(river) || distance > radiusSquared)
                    {
                        continue;
                    }

                    heightMap[deltaPos] = std::min(heightCopy[deltaPos] - 2.0f, heightMap[deltaPos]);

                    if (distance <= radiusMinusOneSquared && hydroMaps.flags[deltaPos].has(filled))
                    {
                        hydroMaps.flags[deltaPos].set(riverbed);
                    }
                }
            }

            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!hydroMaps.flags.inBounds(nPos) || !hydroMaps.flags[nPos].has(river) || visited[nPos] == Mask::True)
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                visited[nPos] = Mask::True;
            }
        }
    }

    /**
     * Ensure there are no river tiles with lower land neighbours or sinks.
     *
     * TODO this post-hoc pass shouldn't be needed in the first place...
     */
    static void ensureConsistent(MapGenCtx& context)
    {
        PROFILED_FUNCTION();
        HydroMaps& hydroMaps = context.hydroMaps.value();

        for (int32_t y = 1; y < hydroMaps.dimensions.y - 1 ; y++)
        {
            for (int32_t x = 1; x < hydroMaps.dimensions.x - 1 ; x++)
            {
                const TileCoordsXY pos { x, y };

                if (!hydroMaps.flags[pos].has(river))
                {
                    float minHeight = context.heightMap[pos];
                    for (const auto & offset : kNeighbourOffsetsOrdinal)
                    {
                        const TileCoordsXY nPos{ pos + offset };
                        if (hydroMaps.flags[nPos].has(river))
                        {
                            minHeight = std::max(minHeight, hydroMaps.height[nPos]);
                        }
                    }

                    context.heightMap[pos] = minHeight;
                }
                else
                {
                    bool hasSource = false;
                    bool hasSink = false;
                    for (const auto & offset : kNeighbourOffsetsOrdinal)
                    {
                        const TileCoordsXY nPos{ pos + offset };
                        if (hydroMaps.flags[nPos].has(river))
                        {
                            if (hydroMaps.height[pos] >= hydroMaps.height[nPos])
                            {
                                hasSink = true;
                            }
                            else if (hydroMaps.height[pos] < hydroMaps.height[nPos])
                            {
                                hasSource = true;
                            }
                        }
                    }
                    if (hasSource && !hasSink)
                    {
                        // TODO should done in a separate pass before checking land neighbour height?
                        hydroMaps.height[pos] += 2.0f;
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
        constructRiverSkeletons(context);
        pruneShortStreams(context);
        ensureCardinalNeighbours(context);
        carveRiverbed(context);
        ensureConsistent(context);
    }
} // namespace OpenRCT2::World::MapGenerator
