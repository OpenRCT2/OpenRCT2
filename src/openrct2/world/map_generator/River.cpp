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
#include "../Vec.hpp"

#include <numbers>
#include <unistd.h>

namespace OpenRCT2::World::MapGenerator
{

    enum state_t
    {
        open,
        closed
    };

    static constexpr TileCoordsXY kNeighborOffsets[] = {
        TileCoordsXY{ -1, 0 },
        TileCoordsXY{ 1, 0 },
        TileCoordsXY{ 0, -1 },
        TileCoordsXY{ 0, 1 },
        TileCoordsXY{ 1, 1 },
        TileCoordsXY{ 1, -1 },
        TileCoordsXY{ -1, 1 },
        TileCoordsXY{ -1, -1 }
    };

    struct tile_t
    {
        TileCoordsXY pos;
        float height;
        uint32_t orderIdx;

        friend bool operator<(const tile_t& lhs, const tile_t& rhs)
        {
            if (lhs.height < rhs.height)
                return true;
            if (rhs.height < lhs.height)
                return false;
            return lhs.orderIdx < rhs.orderIdx;
        }
        friend bool operator<=(const tile_t& lhs, const tile_t& rhs)
        {
            return !(rhs < lhs);
        }
        friend bool operator>(const tile_t& lhs, const tile_t& rhs)
        {
            return rhs < lhs;
        }
        friend bool operator>=(const tile_t& lhs, const tile_t& rhs)
        {
            return !(lhs < rhs);
        }
    };

    static constexpr float kP = 1.1f;

    void floodFill(HeightMap& heightMap)
    {
        std::priority_queue<tile_t, std::vector<tile_t>, std::greater<tile_t>> open;
        std::queue<tile_t> pit;
        BaseMap<state_t> state(heightMap.width, heightMap.height, heightMap.density);

        state.fill(state_t::open);

        uint32_t orderIdx = 0;
        int32_t pitCellExceeds = 0;
        std::optional<float> pitTop = std::nullopt;

        for (int32_t y = 0; y < heightMap.height; y++)
        {
            // x == 0
            auto pos = TileCoordsXY{ 0, y };
            open.emplace(pos, heightMap[pos], orderIdx++);
            state[pos] = state_t::closed;

            // x == width - 1
            auto pos2 = TileCoordsXY{ heightMap.width - 1, y };
            open.emplace(pos2, heightMap[pos2], orderIdx++);
            state[pos2] = state_t::closed;
        }

        for (int32_t x = 1; x < heightMap.width - 1; x++)
        {
            // y == 0
            auto pos = TileCoordsXY{ x, 0 };
            open.emplace(pos, heightMap[pos], orderIdx++);
            state[pos] = state_t::closed;

            // y == height - 1
            auto pos2 = TileCoordsXY{ x, heightMap.height - 1 };
            open.emplace(pos2, heightMap[pos2], orderIdx++);
            state[pos2] = state_t::closed;
        }

        while (!open.empty() || !pit.empty())
        {
            tile_t tile;

            if (!open.empty() && !pit.empty() && open.top().height == pit.front().height)
            {
                tile = open.top();
                open.pop();
                pitTop = std::nullopt;
            }
            else if (!pit.empty())
            {
                tile = pit.front();
                pit.pop();
                if (!pitTop.has_value())
                {
                    pitTop = std::make_optional(heightMap[tile.pos]);
                }
            }
            else
            {
                tile = open.top();
                open.pop();
                pitTop = std::nullopt;
            }

            for (const auto& offset : kNeighborOffsets)
            {
                TileCoordsXY neighborPos = tile.pos + offset;

                if (!heightMap.contains(neighborPos))
                {
                    continue;
                }

                if (state[neighborPos] == state_t::closed)
                {
                    continue;
                }

                state[neighborPos] = state_t::closed;

                const float nextAfter = std::nextafter(heightMap[tile.pos], std::numeric_limits<float>::infinity());

                if (heightMap[neighborPos] <= nextAfter)
                {
                    if (pitTop.has_value() && pitTop.value() < heightMap[neighborPos] && nextAfter >= heightMap[neighborPos])
                    {
                        ++pitCellExceeds;
                    }

                    heightMap[neighborPos] = nextAfter;
                    pit.emplace(neighborPos, heightMap[neighborPos], orderIdx++);
                }
                else
                {
                    open.emplace(neighborPos, heightMap[neighborPos], orderIdx++);
                }
            }
        }

        if (pitCellExceeds > 0)
        {
            LOG_WARNING("The inside of a pit exceeded the terrain surrounding it (n=%d)", pitCellExceeds);
        }
    }

    static double downSlope(const HeightMap& heightMap, const TileCoordsXY& from, const TileCoordsXY& to)
    {
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

    HeightMap genCatchment(const HeightMap& heightMap)
    {
        HeightMap catchment(heightMap.width, heightMap.height, heightMap.density);
        catchment.fill(0.0f);

        for (auto y = 0; y < heightMap.height; y++)
        {
            for (auto x = 0; x < heightMap.width; x++)
            {
                TileCoordsXY pos{ x, y };
                checkNeighbor(heightMap, catchment, pos);
            }
        }

        return catchment;
    }
} // namespace OpenRCT2::World::MapGenerator
