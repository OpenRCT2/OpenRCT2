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

#include <numbers>
#include <unistd.h>

namespace OpenRCT2::World::MapGenerator
{
    static constexpr float kP = 1.1f;

    template<class... Arrays>
    consteval auto concat(Arrays... arrays)
    {
        return std::apply([](auto... args) { return std::array{ args... }; }, std::tuple_cat(arrays...));
    }

    static constexpr std::array kNeighborOffsetsCardinal = {
        TileCoordsXY{ -1, 0 },
        TileCoordsXY{ 1, 0 },
        TileCoordsXY{ 0, -1 },
        TileCoordsXY{ 0, 1 },
    };

    static constexpr std::array kNeighborOffsetsOrdinal = {
        TileCoordsXY{ 1, 1 },
        TileCoordsXY{ 1, -1 },
        TileCoordsXY{ -1, 1 },
        TileCoordsXY{ -1, -1 }
    };

    static constexpr std::array kNeighborOffsets = concat(kNeighborOffsetsCardinal, kNeighborOffsetsOrdinal);

    struct QueueTile
    {
        TileCoordsXY pos;
        float height;
        uint32_t orderIdx;

        friend bool operator<(const QueueTile& lhs, const QueueTile& rhs)
        {
            if (lhs.height < rhs.height)
                return true;
            if (rhs.height < lhs.height)
                return false;
            return lhs.orderIdx < rhs.orderIdx;
        }
        friend bool operator<=(const QueueTile& lhs, const QueueTile& rhs)
        {
            return !(rhs < lhs);
        }
        friend bool operator>(const QueueTile& lhs, const QueueTile& rhs)
        {
            return rhs < lhs;
        }
        friend bool operator>=(const QueueTile& lhs, const QueueTile& rhs)
        {
            return !(lhs < rhs);
        }
    };

    using StableTileQueueBase = std::priority_queue<QueueTile, std::vector<QueueTile>, std::greater<QueueTile>>;
    class StableTileQueue : public StableTileQueueBase
    {
    private:
        uint32_t insertIdx = 0;

    public:
        void push() = delete;

        template<class... Args>
        void emplace(Args... args)
        {
            StableTileQueueBase::emplace(std::forward<Args>(args)..., insertIdx++);
        }
    };

    struct TileState
    {
        // TODO use an uint8_t to encode the kNeighborOffsets index
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
    void floodFill(MapGenCtx& context)
    {
        HeightMap& heightMap = context.heightMap;
        BaseMap<TileState> state(heightMap.height, heightMap.width);
        StableTileQueue queue;
        std::queue<TileCoordsXY> tilesToFlood;

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

                float minNeighbor = std::numeric_limits<float>::max();

                for (const auto& offset : kNeighborOffsets)
                {
                    const TileCoordsXY nPos{ pos + offset };
                    // no contains check needed per if-clause above
                    minNeighbor = std::min(heightMap[nPos], minNeighbor);
                }

                if (heightMap[pos] < minNeighbor)
                {
                    heightMap[pos] = std::nextafter(minNeighbor, std::numeric_limits<float>::lowest());
                    state[pos].pit = true;
                }
            }
        }

        // process tiles from queue
        while (!queue.empty())
        {
            const QueueTile c = queue.top();
            queue.pop();

            // handle neighbors
            for (const auto& offset : kNeighborOffsets)
            {
                const TileCoordsXY nPos{ c.pos + offset };

                if (!heightMap.contains(nPos) || state[nPos].processed)
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                tilesToFlood.push(nPos);
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

                            targetHeight = std::nextafter(targetHeight, std::numeric_limits<float>::lowest());
                            currentTile = state[currentTile.value()].backlink;
                        }
                    }
                }
            }
        }

        // fill depressions
        while (!tilesToFlood.empty())
        {
            const TileCoordsXY& toFlood = tilesToFlood.front();
            if (state[toFlood].backlink.has_value())
            {
                auto parent = state[toFlood].backlink.value();
                if (heightMap[toFlood] <= heightMap[parent])
                {
                    heightMap[toFlood] = std::nextafter(heightMap[parent], std::numeric_limits<float>::infinity());
                }
            }
            tilesToFlood.pop();
        }
    }

    static void postProcessTile(
        RiverMap& catchment, const Settings& settings, std::queue<TileCoordsXY>& queue, BaseMap<int8_t>& visited,
        const TileCoordsXY& pos)
    {
        if (catchment[pos] >= settings.catchmentThreshold)
        {
            queue.push(pos);
            visited[pos] = 1;
        }
    }

    static void removeOrphans(MapGenCtx& context)
    {

        auto& settings = context.settings;
        auto& riverMap = context.riverMap.value();

        std::queue<TileCoordsXY> queue;
        BaseMap<int8_t> visited(riverMap.width, riverMap.height);

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

            for (const auto& offset : kNeighborOffsets)
            {
                const TileCoordsXY nPos{ pos + offset };

                if (!riverMap.contains(nPos) || visited[nPos] > 0)
                {
                    continue;
                }

                postProcessTile(riverMap, settings, queue, visited, nPos);
            }

            queue.pop();
        }

        for (int32_t y = 0; y < riverMap.height; y++)
        {
            for (int32_t x = 0; x < riverMap.width; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (visited[pos] == 0 || riverMap[pos] < settings.catchmentThreshold)
                {
                    riverMap[pos] = 0.0f;
                }
            }
        }
    }

    static void ensureCardinalNeighbors(MapGenCtx& context)
    {
        auto& heightMap = context.heightMap;
        auto& riverMap = context.riverMap.value();

        StableTileQueue queue;
        BaseMap<int8_t> visited(riverMap.width, riverMap.height);

        for (int32_t y = 0; y < riverMap.height; y++)
        {
            const TileCoordsXY left{ 0, y };
            if (riverMap[left] > 0.0f)
            {
                queue.emplace(left, heightMap[left]);
                visited[left] = 1;
            }

            const TileCoordsXY right{ riverMap.width - 1, y };
            if (riverMap[right] > 0.0f)
            {
                queue.emplace(right, heightMap[right]);
                visited[right] = 1;
            }
        }

        for (int32_t x = 1; x < riverMap.width - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            if (riverMap[top] > 0.0f)
            {
                queue.emplace(top, heightMap[top]);
                visited[top] = 1;
            }

            const TileCoordsXY bottom{ x, riverMap.height - 1 };
            if (riverMap[bottom] > 0.0f)
            {
                queue.emplace(bottom, heightMap[bottom]);
                visited[bottom] = 1;
            }
        }

        while (!queue.empty())
        {
            const QueueTile tile = queue.top();
            queue.pop();

            for (const auto& offset : kNeighborOffsetsOrdinal)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!riverMap.contains(nPos) || visited[nPos] > 0 || riverMap[nPos] == 0.0f)
                {
                    continue;
                }

                queue.emplace(nPos, heightMap[nPos]);
                visited[nPos] = 1;

                const TileCoordsXY sharedCardinalNeighbours[] = {
                    TileCoordsXY{ 0, offset.y },
                    TileCoordsXY{ offset.x, 0 },
                };

                bool hasCardinalNeighbor = false;

                for (const TileCoordsXY& scnOffset : sharedCardinalNeighbours)
                {
                    const TileCoordsXY scnPos{ tile.pos + scnOffset };

                    if (riverMap[scnPos] > 0)
                    {
                        hasCardinalNeighbor = true;
                        break;
                    }
                }

                if (hasCardinalNeighbor)
                {
                    continue;
                }

                for (const TileCoordsXY& scnOffset : sharedCardinalNeighbours)
                {
                    const TileCoordsXY scnPos{ tile.pos + scnOffset };
                    heightMap[scnPos] = heightMap[tile.pos];
                    riverMap[scnPos] = riverMap[tile.pos];
                    visited[scnPos] = 1;
                }
            }

            for (const auto& offset : kNeighborOffsetsCardinal)
            {
                const TileCoordsXY nPos{ tile.pos + offset };

                if (!riverMap.contains(nPos) || visited[nPos] > 0 || riverMap[nPos] == 0.0f)
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
        ensureCardinalNeighbors(context);
    }

    static float downSlope(const HeightMap& heightMap, const TileCoordsXY& from, const TileCoordsXY& to)
    {
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

        for (const auto& offset : kNeighborOffsets)
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

    static float checkNeighbor(MapGenCtx& context, const TileCoordsXY& pos)
    {
        auto& heightMap = context.heightMap;
        auto& riverMap = context.riverMap.value();

        if (riverMap[pos] <= 0.0f)
        {
            riverMap[pos] = 1.0f;
            for (const auto& offset : kNeighborOffsets)
            {
                TileCoordsXY nPos{ pos + offset };

                if (!heightMap.contains(nPos))
                {
                    continue;
                }

                if (heightMap[nPos] > heightMap[pos])
                {
                    const float flowFraction = std::max(0.0f, fractionalFlow(heightMap, nPos, pos));
                    const float neighborCatchment = checkNeighbor(context, nPos);
                    const float fractionalContribution = flowFraction * neighborCatchment;
                    riverMap[pos] += fractionalContribution;
                }
            }
        }
        return riverMap[pos];
    }

    /**
     * based on
     *
     * Freeman, T.G., 1991. Calculating catchment area with divergent flow based on a regular grid. Computers & geosciences,
     * 17(3), pp.413-422.
     */
    void genCatchment(MapGenCtx& context)
    {
        for (int32_t y = 0; y < context.heightMap.height; y++)
        {
            for (int32_t x = 0; x < context.heightMap.width; x++)
            {
                TileCoordsXY pos{ x, y };
                checkNeighbor(context, pos);
            }
        }

        postProcessCatchment(context);
    }
} // namespace OpenRCT2::World::MapGenerator
