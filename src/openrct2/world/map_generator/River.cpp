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
#include "MapHelpers.h"
#include "TileQueue.hpp"

#include <numbers>

namespace OpenRCT2::World::MapGenerator
{
    static constexpr float kP = 1.1f;
    static constexpr int32_t kMaxPruneLength = 16;

    using Backref = std::optional<TileCoordsXY>;

    struct TileState
    {
        // TODO use an uint8_t to encode the kNeighbourOffsets index
        Backref backref = std::nullopt;
        bool processed = false;
        bool pit = false;
    };

    using BackrefMap = BaseMap<Backref>;
    using NeighbourCountMap = BaseMap<int32_t>;
    using StateMap = BaseMap<TileState>;

    /**
     *
     * Fill up or breach out of depressions in the heightmap to ensure there is a strictly monotonic downhill path for rivers
     * to follow.
     *
     * based on
     *
     * Lindsay, J.B., 2016. Efficient hybrid breaching‐filling sink removal methods for flow path enforcement in digital
     * elevation models. Hydrological Processes, 30(6), pp.846-857.
     */
    static void fillOrBreachDepressions(MapGenCtx& context)
    {
        HeightMap& heightMap = context.heightMap;
        RiverMap& riverMap = context.riverMap.value();

        StateMap state(heightMap.height, heightMap.width);
        StableTileQueue queue;
        std::queue<TileCoordsXY> tilesToFill;

        // prepare queue and mark pits
        for (int32_t y = 0; y < heightMap.height; y++)
        {
            for (int32_t x = 0; x < heightMap.width; x++)
            {
                TileCoordsXY pos{ x, y };

                if (x == 0 || y == 0 || x == heightMap.width - 1 || y == heightMap.height - 1)
                {
                    queue.emplace(pos, heightMap[pos]);
                    state[pos].processed = true;
                    continue;
                }

                float minNeighbour = std::numeric_limits<float>::infinity();

                for (const auto& offset : kNeighbourOffsets)
                {
                    const TileCoordsXY nPos{ pos + offset };
                    // no contains check needed per if-clause above
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

        // process lowest tile from queue, moving inward from map edges and building up a lowest path graph
        while (!queue.empty())
        {
            const QueueTile c = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ c.pos + offset };

                if (!heightMap.inBounds(nPos) || state[nPos].processed)
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                tilesToFill.push(nPos);
                state[nPos].processed = true;
                state[nPos].backref = std::make_optional(c.pos);

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
                            riverMap[currentTile.value()].isBreached = true;

                            targetHeight = std::nextafter(targetHeight, std::numeric_limits<float>::lowest());
                            currentTile = state[currentTile.value()].backref;
                        }
                    }
                }
            }
        }

        // fill depressions
        while (!tilesToFill.empty())
        {
            const TileCoordsXY& toFill = tilesToFill.front();
            if (state[toFill].backref.has_value())
            {
                const TileCoordsXY& prev = state[toFill].backref.value();

                // The fill queue is populated in the lowest path order, encountering a node that is lower than the previous
                // node at this point means no breach was found and the depression needs to be filled. To maintain the overall
                // height gradient adjust height to just above the prev node
                if (heightMap[toFill] <= heightMap[prev])
                {
                    heightMap[toFill] = std::nextafter(heightMap[prev], std::numeric_limits<float>::infinity());
                    riverMap[toFill].isFilled = true;
                }
            }
            tilesToFill.pop();
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
        auto& heightMap = context.heightMap;
        auto& riverMap = context.riverMap.value();

        if (riverMap[pos].catchment <= 0.0f)
        {
            riverMap[pos].catchment = 1.0f;
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
                    riverMap[pos].catchment += neighbourContribution;
                }
            }
        }
        return riverMap[pos].catchment;
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
        for (int32_t y = 0; y < context.heightMap.height; y++)
        {
            for (int32_t x = 0; x < context.heightMap.width; x++)
            {
                TileCoordsXY pos{ x, y };
                aggregateNeighbour(context, pos);
            }
        }
    }

    static bool postProcessTile(
        MapGenCtx& context, StableTileQueue& queue, MaskMap& visited, BackrefMap& backrefMap, Backref backref,
        const TileCoordsXY& pos)
    {
        RiverState& riverState = context.riverMap.value()[pos];

        if (riverState.catchment < context.settings.catchmentThreshold)
        {
            return false;
        }

        queue.emplace(pos, context.heightMap[pos]);
        visited[pos] = Mask::True;
        backrefMap[pos] = backref;
        riverState.isRiver = true;

        return true;
    }

    /**
     * Applies the catchment threshold and removes artifacts. Orphans (possible due to the fractional nature of the catchment
     * calculation) are implicitly removed by tracing streams from the sinks at the map edges. Short streams are explicitly
     * removed by first identifying springs and tracing downstream until meeting a wider stream.
     *
     * TODO there are also (rare) non-edge sinks that should be pruned
     */
    static void postProcessCatchment(MapGenCtx& context)
    {
        auto& riverMap = context.riverMap.value();

        StableTileQueue queue;
        MaskMap visited(riverMap.width, riverMap.height);

        NeighbourCountMap neighbourCounts{ riverMap.width, riverMap.height };
        BackrefMap backrefMap{ riverMap.width, riverMap.height };

        std::queue<TileCoordsXY> springs;

        // prepare queue
        for (int32_t y = 0; y < riverMap.height; y++)
        {
            postProcessTile(context, queue, visited, backrefMap, std::nullopt, { 0, y });
            postProcessTile(context, queue, visited, backrefMap, std::nullopt, { riverMap.width - 1, y });
        }

        for (int32_t x = 1; x < riverMap.width - 1; x++)
        {
            postProcessTile(context, queue, visited, backrefMap, std::nullopt, { x, 0 });
            postProcessTile(context, queue, visited, backrefMap, std::nullopt, { x, riverMap.height - 1 });
        }

        // process tiles and find springs
        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            int32_t riverNeighbours = 0;
            bool hasUpstreamNeighbours = false;
            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!riverMap.inBounds(nPos))
                {
                    continue;
                }

                if (visited[nPos] == Mask::True)
                {
                    riverNeighbours++;
                    continue;
                }

                if (postProcessTile(context, queue, visited, backrefMap, tile.pos, nPos))
                {
                    riverNeighbours++;
                    hasUpstreamNeighbours = true;
                }
            }

            neighbourCounts[tile.pos] = riverNeighbours;
            if (!hasUpstreamNeighbours && riverNeighbours == 1 && riverMap[tile.pos].isFilled)
            {
                springs.push(tile.pos);
            }
        }

        int32_t springsFound = springs.size();
        int32_t springsPruned = 0;

        // prune if length is below limit
        while (!springs.empty())
        {
            const TileCoordsXY spring = springs.front();
            springs.pop();

            float pathLength = 0.0f;

            std::optional<TileCoordsXY> currentTile = std::make_optional(spring);

            while (currentTile.has_value() && neighbourCounts[currentTile.value()] <= 2)
            {
                pathLength++;
                currentTile = backrefMap[currentTile.value()];
            }

            currentTile = std::make_optional(spring);

            if (pathLength <= kMaxPruneLength)
            {
                while (currentTile.has_value() && neighbourCounts[currentTile.value()] <= 2)
                {
                    riverMap[currentTile.value()].isRiver = false;
                    currentTile = backrefMap[currentTile.value()];
                }

                springsPruned++;
            }
        }

        LOG_INFO("%d springs found, %d pruned", springsFound, springsPruned);
    }

    static void prepareRiverQueue(MapGenCtx& context, StableTileQueue& queue, MaskMap& visited)
    {
        auto& heightMap = context.heightMap;
        auto& riverMap = context.riverMap.value();

        for (int32_t y = 0; y < riverMap.height; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (riverMap[left].isRiver)
            {
                queue.emplace(left, heightMap[left]);
                visited[left] = Mask::True;
            }

            const TileCoordsXY right{ riverMap.width - 1, y };
            if (riverMap[right].isRiver)
            {
                queue.emplace(right, heightMap[right]);
                visited[right] = Mask::True;
            }
        }

        for (int32_t x = 1; x < riverMap.width - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (riverMap[top].isRiver)
            {
                queue.emplace(top, heightMap[top]);
                visited[top] = Mask::True;
            }

            const TileCoordsXY bottom{ x, riverMap.height - 1 };
            if (riverMap[bottom].isRiver)
            {
                queue.emplace(bottom, heightMap[bottom]);
                visited[bottom] = Mask::True;
            }
        }
    }

    /**
     * Slightly widen the river in proportion to the catchment.
     */
    static void adjustStreamWidth(MapGenCtx& context)
    {
        auto& settings = context.settings;
        auto& heightMap = context.heightMap;
        auto& riverMap = context.riverMap.value();

        StableTileQueue queue;
        MaskMap visited(riverMap.width, riverMap.height);
        prepareRiverQueue(context, queue, visited);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!riverMap.inBounds(nPos) || visited[nPos] == Mask::True || !riverMap[nPos].isRiver)
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                visited[nPos] = Mask::True;
            }

            const float radius = 0.75f * std::log2(std::log2(riverMap[tile.pos].catchment / settings.catchmentThreshold));;
            const float radiusSquared = radius * radius;

            for (int32_t dy = -radius; dy <= radius; dy++)
            {
                for (int32_t dx = -radius; dx <= radius; dx++)
                {
                    TileCoordsXY deltaPos = tile.pos + TileCoordsXY{ dx, dy };

                    int32_t distance = dx * dx + dy * dy;
                    if (!heightMap.inBounds(deltaPos) || distance > radiusSquared || riverMap[deltaPos].isRiver)
                    {
                        continue;
                    }

                    float riverHeight = std::min(heightMap[tile.pos], heightMap[deltaPos]);
                    heightMap[deltaPos] = riverHeight;
                    riverMap[deltaPos].isRiver = true;
                    visited[deltaPos] = Mask::True;
                }
            }
        }
    }



    /**
     * Ensure diagonal channels render nicely by asserting each river tile has at least one cardinal river neighbour.
     */
    static void ensureCardinalNeighbours(MapGenCtx& context)
    {
        auto& heightMap = context.heightMap;
        auto& riverMap = context.riverMap.value();

        StableTileQueue queue;
        MaskMap visited(riverMap.width, riverMap.height);
        prepareRiverQueue(context, queue, visited);

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsetsOrdinal)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!riverMap.inBounds(nPos) || visited[nPos] == Mask::True || !riverMap[nPos].isRiver)
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                visited[nPos] = Mask::True;

                const TileCoordsXY sharedCardinalNeighbours[] = {
                    TileCoordsXY{ 0, offset.y },
                    TileCoordsXY{ offset.x, 0 },
                };

                bool hasCardinalNeighbour = false;

                for (const TileCoordsXY& scnOffset : sharedCardinalNeighbours)
                {
                    const TileCoordsXY scnPos{ tile.pos + scnOffset };

                    if (riverMap[scnPos].isRiver)
                    {
                        hasCardinalNeighbour = true;
                        break;
                    }
                }

                if (hasCardinalNeighbour)
                {
                    continue;
                }

                for (const TileCoordsXY& scnOffset : sharedCardinalNeighbours)
                {
                    const TileCoordsXY scnPos{ tile.pos + scnOffset };
                    heightMap[scnPos] = heightMap[tile.pos];
                    riverMap[scnPos].catchment = context.settings.catchmentThreshold;
                    riverMap[scnPos].isRiver = true;
                    visited[scnPos] = Mask::True;
                }
            }

            for (const auto& offset : kNeighbourOffsetsCardinal)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!riverMap.inBounds(nPos) || visited[nPos] == Mask::True || !riverMap[nPos].isRiver)
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                visited[nPos] = Mask::True;
            }
        }
    }

    /**
     * Carve a riverbed around the river on flat parts with increasing width based on the tile catchment.
     * TODO deepen river in proportion to catchment
     */
    static void carveRiverbed(MapGenCtx& context)
    {
        auto& settings = context.settings;
        auto& heightMap = context.heightMap;
        auto& riverMap = context.riverMap.value();

        HeightMap heightCopy = heightMap;

        for (int32_t y = 0; y < heightMap.height; y++)
        {
            for (int32_t x = 0; x < heightMap.width; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (!riverMap[pos].isRiver)
                {
                    continue;
                }

                float riverHeight = heightCopy[pos] - 4.0f;
                if (riverHeight < heightMap[pos])
                {
                    heightMap[pos] = riverHeight;
                }
                else
                {
                    riverHeight = heightMap[pos];
                }

                riverMap[pos].isRiverbed = true;

                const float radius = std::log2(riverMap[pos].catchment / settings.catchmentThreshold);
                const float radiusSquared = radius * radius;

                for (int32_t dy = -radius; dy <= radius; dy++)
                {
                    for (int32_t dx = -radius; dx <= radius; dx++)
                    {
                        TileCoordsXY deltaPos = pos + TileCoordsXY{ dx, dy };

                        int32_t distance = dx * dx + dy * dy;
                        if (!heightMap.inBounds(deltaPos) || distance > radiusSquared)
                        {
                            continue;
                        }

                        float riverbedHeight = heightCopy[deltaPos] - 2.0f;
                        if (riverbedHeight < heightMap[deltaPos])
                        {
                            heightMap[deltaPos] = riverbedHeight;

                            if (riverbedHeight - (riverHeight + 2.0f) < 1.0f && riverMap[deltaPos].isFilled)
                            {
                                riverMap[deltaPos].isRiverbed = true;
                            }
                        }
                    }
                }
            }
        }
    }

    void generateRivers(MapGenCtx& context)
    {
        fillOrBreachDepressions(context);
        aggregateCatchment(context);
        postProcessCatchment(context);
        adjustStreamWidth(context);
        ensureCardinalNeighbours(context);
        carveRiverbed(context);
    }
} // namespace OpenRCT2::World::MapGenerator
