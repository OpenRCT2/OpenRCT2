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

    static constexpr TileCoordsXY kNeighborOffsets[] = { TileCoordsXY{ -1, 0 }, TileCoordsXY{ 1, 0 },  TileCoordsXY{ 0, -1 },
                                                         TileCoordsXY{ 0, 1 },  TileCoordsXY{ 1, 1 },  TileCoordsXY{ 1, -1 },
                                                         TileCoordsXY{ -1, 1 }, TileCoordsXY{ -1, -1 } };

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

    struct TileState
    {
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
    void floodFill(HeightMap& heightMap, const Settings& settings)
    {
        BaseMap<TileState> state(heightMap.height, heightMap.width);
        std::priority_queue<QueueTile, std::vector<QueueTile>, std::greater<QueueTile>> priorityQueue;
        std::queue<TileCoordsXY> tilesToFlood;

        uint32_t orderIdx = 0;

        // Add edge tiles to queue and mark pits
        for (int y = 0; y < heightMap.height; y++)
        {
            for (int x = 0; x < heightMap.width; x++)
            {
                TileCoordsXY pos{ x, y };

                if (x == 0 || y == 0 || x == heightMap.width - 1 || y == heightMap.height - 1)
                {
                    priorityQueue.emplace(pos, heightMap[pos], orderIdx++);
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
        while (!priorityQueue.empty())
        {
            const QueueTile c = priorityQueue.top();
            priorityQueue.pop();

            // handle neighbors
            for (const auto& offset : kNeighborOffsets)
            {
                const TileCoordsXY nPos{ c.pos + offset };

                if (!heightMap.contains(nPos) || !state[nPos].processed)
                {
                    continue;
                }

                priorityQueue.emplace(nPos, heightMap[nPos], orderIdx++);
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
                    if (pathLength <= settings.breachMaxLength && pathDepth <= settings.breachMaxDepth)
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
        HeightMap& catchment, const Settings& settings, std::queue<TileCoordsXY>& open, BaseMap<int8_t>& mask,
        const TileCoordsXY& pos)
    {
        if (catchment[pos] >= settings.catchmentThreshold)
        {
            open.push(pos);
            mask[pos] = 1;
        }
    }

    /**
     * remove orphans from catchment and 
     */
    static void postProcessCatchment(HeightMap& catchment, const Settings& settings)
    {
        std::queue<TileCoordsXY> open;
        BaseMap<int8_t> mask(catchment.width, catchment.height);

        for (int32_t y = 0; y < catchment.height; y++)
        {
            const TileCoordsXY left{ 0, y };
            postProcessTile(catchment, settings, open, mask, left);

            const TileCoordsXY right{ catchment.width - 1, y };
            postProcessTile(catchment, settings, open, mask, right);
        }

        for (int32_t x = 1; x < catchment.width - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            postProcessTile(catchment, settings, open, mask, top);

            const TileCoordsXY bottom{ x, catchment.height - 1 };
            postProcessTile(catchment, settings, open, mask, bottom);
        }

        while (!open.empty())
        {
            const TileCoordsXY& pos = open.front();

            for (const auto& offset : kNeighborOffsets)
            {
                const TileCoordsXY nPos{ pos + offset };

                if (mask[nPos] > 0)
                {
                    continue;
                }

                postProcessTile(catchment, settings, open, mask, nPos);
            }

            open.pop();
        }

        for (int y = 0; y < catchment.height; y++)
        {
            for (int x = 0; x < catchment.width; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (mask[pos] == 0 || catchment[pos] < settings.catchmentThreshold)
                {
                    catchment[pos] = 0;
                }
            }
        }
    }

    static double downSlope(const HeightMap& heightMap, const TileCoordsXY& from, const TileCoordsXY& to)
    {
        // TODO double -> float
        int32_t deltaX = from.x - to.x;
        int32_t deltaY = from.y - to.y;

        double distance = abs(deltaX) + abs(deltaY) > 1 ? std::numbers::sqrt2 : 1.0;
        double heightFrom = heightMap[from];
        double heightTo = heightMap[to];
        double delta = (heightFrom - heightTo) / distance;
        return std::max(0.0, std::pow(delta, kP));
    }

    static float fractionalFlow(const HeightMap& heightMap, const TileCoordsXY& from, const TileCoordsXY& to)
    {
        double sum = 0.0;

        for (const auto& offset : kNeighborOffsets)
        {
            TileCoordsXY nPos{ from + offset };
            if (!heightMap.contains(nPos))
            {
                continue;
            }

            sum += downSlope(heightMap, from, nPos);
        }

        const double fraction = downSlope(heightMap, from, to) / sum;

        return static_cast<float>(fraction);
    }

    static float checkNeighbor(const HeightMap& heightMap, HeightMap& catchment, const TileCoordsXY& pos)
    {
        if (catchment[pos] <= 0.0f)
        {
            catchment[pos] = 1.0f;
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
                    const float neighborCatchment = checkNeighbor(heightMap, catchment, nPos);
                    const float fractionalContribution = flowFraction * neighborCatchment;
                    catchment[pos] += fractionalContribution;
                }
            }
        }
        return catchment[pos];
    }

    /**
     * based on
     *
     * Freeman, T.G., 1991. Calculating catchment area with divergent flow based on a regular grid. Computers & geosciences,
     * 17(3), pp.413-422.
     */
    HeightMap genCatchment(const HeightMap& heightMap, const Settings& settings)
    {
        HeightMap catchment(heightMap.width, heightMap.height);
        catchment.fill(0.0f);

        for (auto y = 0; y < heightMap.height; y++)
        {
            for (auto x = 0; x < heightMap.width; x++)
            {
                TileCoordsXY pos{ x, y };
                checkNeighbor(heightMap, catchment, pos);
            }
        }

        postProcessCatchment(catchment, settings);

        return catchment;
    }
} // namespace OpenRCT2::World::MapGenerator
