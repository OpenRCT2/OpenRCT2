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
#include "../../GameState.h"
#include "MapHelpers.h"
#include "TileQueue.hpp"

#include <numbers>

namespace OpenRCT2::World::MapGenerator
{
    static constexpr float kP = 1.1f;

    struct TileState
    {
        // TODO use an uint8_t to encode the kNeighbourOffsets index
        std::optional<TileCoordsXY> backlink = std::nullopt;
        bool processed = false;
        bool pit = false;
    };

    /**
     * based on
     *
     * Lindsay, J.B., 2016. Efficient hybrid breaching-filling sink removal methods for flow path enforcement in digital
     * elevation models: Efficient Hybrid Sink Removal Methods for Flow Path Enforcement. Hydrological Processes 30, 846--857.
     *
     */
    static void fillOrBreach(MapGenCtx& context)
    {
        HeightMap& heightMap = context.heightMap;
        RiverMap& riverMap = context.riverMap.value();

        BaseMap<TileState> state(heightMap.height, heightMap.width);
        StableTileQueue queue;
        std::queue<TileCoordsXY> tilesToFill;

        // Add edge tiles to queue and mark pits
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

                float minNeighbour = std::numeric_limits<float>::max();

                for (const auto& offset : kNeighbourOffsets)
                {
                    const TileCoordsXY nPos{ pos + offset };
                    // no contains check needed per if-clause above
                    minNeighbour = std::min(heightMap[nPos], minNeighbour);
                }

                if (heightMap[pos] < minNeighbour)
                {
                    heightMap[pos] = std::nextafter(minNeighbour, std::numeric_limits<float>::lowest());
                    state[pos].pit = true;
                }
            }
        }

        // process tiles from queue
        while (!queue.empty())
        {
            const QueueTile c = queue.top();
            queue.pop();

            // handle neighbours
            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ c.pos + offset };

                if (!heightMap.contains(nPos) || state[nPos].processed)
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                tilesToFill.push(nPos);
                state[nPos].processed = true;
                state[nPos].backlink = std::make_optional(c.pos);

                // check if there is a valid path to breach out of the pit
                if (state[nPos].pit)
                {
                    float pathLength = 0.0f;
                    float pathDepth = std::numeric_limits<float>::lowest();

                    std::optional<TileCoordsXY> currentTile = std::make_optional(nPos);
                    float targetHeight = heightMap[nPos];

                    // trace path to lower tile along backlinks to get length and depth
                    while (currentTile.has_value() && heightMap[currentTile.value()] >= targetHeight)
                    {
                        pathDepth = std::max(pathDepth, heightMap[currentTile.value()] - targetHeight);
                        pathLength++;

                        targetHeight = std::nextafter(targetHeight, std::numeric_limits<float>::lowest());
                        currentTile = state[currentTile.value()].backlink;
                    }

                    currentTile = std::make_optional(nPos);
                    targetHeight = heightMap[nPos];

                    // if the path limits are not exceeded, adjust height along path
                    if (pathLength <= context.settings.breachMaxLength && pathDepth <= context.settings.breachMaxDepth)
                    {
                        while (currentTile.has_value() && heightMap[currentTile.value()] >= targetHeight)
                        {
                            heightMap[currentTile.value()] = targetHeight;
                            riverMap[currentTile.value()].isBreached = true;

                            targetHeight = std::nextafter(targetHeight, std::numeric_limits<float>::lowest());
                            currentTile = state[currentTile.value()].backlink;
                        }
                    }
                }
            }
        }

        // fill depressions
        while (!tilesToFill.empty())
        {
            const TileCoordsXY& toFlood = tilesToFill.front();
            if (state[toFlood].backlink.has_value())
            {
                auto parent = state[toFlood].backlink.value();
                if (heightMap[toFlood] <= heightMap[parent])
                {
                    heightMap[toFlood] = std::nextafter(heightMap[parent], std::numeric_limits<float>::infinity());
                    riverMap[toFlood].isFilled = true;
                }
            }
            tilesToFill.pop();
        }
    }

    static void postProcessTile(
        RiverMap& riverMap, const Settings& settings, std::queue<TileCoordsXY>& queue, MaskMap& visited,
        const TileCoordsXY& pos)
    {
        if (riverMap[pos].catchment >= settings.catchmentThreshold)
        {
            queue.push(pos);
            visited[pos] = Mask::True;
            riverMap[pos].isRiver = true;
        }
    }

    static void removeOrphans(MapGenCtx& context)
    {
        auto& settings = context.settings;
        auto& riverMap = context.riverMap.value();

        std::queue<TileCoordsXY> queue;
        MaskMap visited(riverMap.width, riverMap.height);

        for (int32_t y = 0; y < riverMap.height; y++)
        {
            const TileCoordsXY left{ 0, y };
            postProcessTile(riverMap, settings, queue, visited, left);

            const TileCoordsXY right{ riverMap.width - 1, y };
            postProcessTile(riverMap, settings, queue, visited, right);
        }

        for (int32_t x = 1; x < riverMap.width - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            postProcessTile(riverMap, settings, queue, visited, top);

            const TileCoordsXY bottom{ x, riverMap.height - 1 };
            postProcessTile(riverMap, settings, queue, visited, bottom);
        }

        while (!queue.empty())
        {
            const TileCoordsXY& pos = queue.front();

            for (const auto& offset : kNeighbourOffsets)
            {
                const TileCoordsXY nPos{ pos + offset };

                if (!riverMap.contains(nPos) || visited[nPos] == Mask::True)
                {
                    continue;
                }

                postProcessTile(riverMap, settings, queue, visited, nPos);
            }

            queue.pop();
        }
    }

    static void ensureCardinalNeighbours(MapGenCtx& context)
    {
        auto& heightMap = context.heightMap;
        auto& riverMap = context.riverMap.value();

        StableTileQueue queue;
        BaseMap<int8_t> visited(riverMap.width, riverMap.height);

        for (int32_t y = 0; y < riverMap.height; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (riverMap[left].isRiver)
            {
                queue.emplace(left, heightMap[left]);
                visited[left] = 1;
            }

            const TileCoordsXY right{ riverMap.width - 1, y };
            if (riverMap[right].isRiver)
            {
                queue.emplace(right, heightMap[right]);
                visited[right] = 1;
            }
        }

        for (int32_t x = 1; x < riverMap.width - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (riverMap[top].isRiver)
            {
                queue.emplace(top, heightMap[top]);
                visited[top] = 1;
            }

            const TileCoordsXY bottom{ x, riverMap.height - 1 };
            if (riverMap[bottom].isRiver)
            {
                queue.emplace(bottom, heightMap[bottom]);
                visited[bottom] = 1;
            }
        }

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighbourOffsetsOrdinal)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!riverMap.contains(nPos) || visited[nPos] > 0 || !riverMap[nPos].isRiver)
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                visited[nPos] = 1;

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
                    visited[scnPos] = 1;
                }
            }

            for (const auto& offset : kNeighbourOffsetsCardinal)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!riverMap.contains(nPos) || visited[nPos] > 0 || !riverMap[nPos].isRiver)
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                visited[nPos] = 1;
            }
        }
    }

    static void postProcessCatchment(MapGenCtx& context)
    {
        removeOrphans(context);
        ensureCardinalNeighbours(context);
    }

    static float downSlope(const HeightMap& heightMap, const TileCoordsXY& from, const TileCoordsXY& to)
    {
        // TODO could infer from cardinal/ordinal offset instead and pass in
        const int32_t deltaX = from.x - to.x;
        const int32_t deltaY = from.y - to.y;

        const float distance = abs(deltaX) + abs(deltaY) > 1.0f ? std::numbers::sqrt2 : 1.0f;
        const float heightFrom = heightMap[from];
        const float heightTo = heightMap[to];
        const float delta = (heightFrom - heightTo) / distance;

        return std::max(0.0f, std::pow(delta, kP));
    }

    static float fractionalFlow(const HeightMap& heightMap, const TileCoordsXY& from, const TileCoordsXY& to)
    {
        float sum = 0.0f;

        for (const auto& offset : kNeighbourOffsets)
        {
            TileCoordsXY nPos{ from + offset };

            if (!heightMap.contains(nPos))
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

                if (!heightMap.contains(nPos))
                {
                    continue;
                }

                if (heightMap[nPos] > heightMap[pos])
                {
                    const float flowFraction = std::max(0.0f, fractionalFlow(heightMap, nPos, pos));
                    const float neighbourCatchment = aggregateNeighbour(context, nPos);
                    const float fractionalContribution = flowFraction * neighbourCatchment;
                    riverMap[pos].catchment += fractionalContribution;
                }
            }
        }
        return riverMap[pos].catchment;
    }

    /**
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

        postProcessCatchment(context);
    }

    static void carveRiverbed(MapGenCtx& context){
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

                const float radius = 0.75f * std::log2(riverMap[pos].catchment / (0.5f * settings.catchmentThreshold));

                const float radiusSquared = radius * radius;

                for (int32_t dy = -radius; dy <= radius; dy++)
                {
                    for (int32_t dx = -radius; dx <= radius; dx++)
                    {
                        TileCoordsXY deltaPos = pos + TileCoordsXY{dx, dy};

                        int32_t distance = dx * dx + dy * dy;
                        if (!heightMap.contains(deltaPos) || distance > radiusSquared)
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
        fillOrBreach(context);
        aggregateCatchment(context);
        carveRiverbed(context);
    }
} // namespace OpenRCT2::World::MapGenerator
