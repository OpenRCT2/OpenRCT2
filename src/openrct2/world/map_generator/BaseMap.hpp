/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/FlagHolder.hpp"
#include "../../util/Hash.hpp"
#include "../Location.hpp"
#include "../Vec.hpp"
#include "filesystem.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <unordered_set>
#include <vector>

namespace OpenRCT2::World::MapGenerator
{
    template<typename T>
    class BaseMap
    {
    protected:
        std::vector<T> _value;

    public:
        uint16_t width{};
        uint16_t height{};
        uint8_t density{};

        BaseMap(TileCoordsXY mapSize)
            : _value(mapSize.x * mapSize.y)
            , width(mapSize.x)
            , height(mapSize.y)
            , density(1)
        {
        }

        BaseMap(int32_t targetWidth, int32_t targetHeight)
            : _value(targetWidth * targetHeight)
            , width(targetWidth)
            , height(targetHeight)
            , density(1)
        {
        }

        BaseMap(int32_t baseWidth, int32_t baseHeight, uint8_t density_)
            : _value((baseWidth * density_) * (baseHeight * density_))
            , width(baseWidth * density_)
            , height(baseHeight * density_)
            , density(density_)
        {
        }

        BaseMap() = default;

        T& operator[](const TileCoordsXY pos)
        {
            return _value[idx(pos)];
        }

        const T& operator[](const TileCoordsXY pos) const
        {
            return _value[idx(pos)];
        }

        size_t idx(const TileCoordsXY pos) const
        {
            assert(inBounds(pos));
            return pos.y * width + pos.x;
        }

        TileCoordsXY pos(const size_t idx) const
        {
            assert(idx < size());
            return TileCoordsXY{ static_cast<int32_t>(idx % width), static_cast<int32_t>(idx / width) };
        }

        void fill(const T& value)
        {
            std::fill(_value.begin(), _value.end(), value);
        }

        void clear()
        {
            _value.clear();
        }

        T* data()
        {
            return _value.data();
        }

        const T* data() const
        {
            return _value.data();
        }

        bool empty() const
        {
            return _value.empty();
        }

        size_t size() const
        {
            return _value.size();
        }

        bool inBounds(const TileCoordsXY& pos) const
        {
            return pos.x >= 0 && pos.y >= 0 && pos.x < width && pos.y < height;
        }

        bool onEdge(const TileCoordsXY& pos) const
        {
            return pos.x == 0 || pos.y == 0 || pos.x == width - 1 || pos.y == height - 1;
        }
    };

    // deal with std::vector<bool> specialization
    class BooleanMap : public BaseMap<bool>
    {
    public:
        std::vector<bool>::reference operator[](const TileCoordsXY pos)
        {
            return _value[idx(pos)];
        }

        std::vector<bool>::const_reference operator[](const TileCoordsXY pos) const
        {
            return _value[idx(pos)];
        }
    };

    struct TileCoordsXYHash
    {
        size_t operator()(const TileCoordsXY& pos) const noexcept
        {
            size_t hash = 0;
            Util::Hash::update(hash, pos.x);
            Util::Hash::update(hash, pos.y);
            return hash;
        }
    };

    // map coordinates and orientation, rotated 45deg counter clockwise in game when compass points north
    // +x,-y         -y          -x,-y
    //       +-----+-----+-----+
    //       |  W  | NW  |  N  |
    //       +-----+-----+-----+
    //    +x | SW  |  *  | NE  | -x
    //       +-----+-----+-----+
    //       |  S  | SE  |  E  |
    //       +-----+-----+-----+
    // +x,+y         +y          -x,+y

    enum MapDirection : uint8_t
    {
        North,
        NorthEast,
        East,
        SouthEast,
        South,
        SouthWest,
        West,
        NorthWest,
    };
    using MapDirectionMask = FlagHolder<uint8_t, MapDirection>;

    struct Neighbour
    {
        MapDirection direction;
        MapDirection opposite;
        TileCoordsXY offset;
    };

    static constexpr std::array kMapDirectionOffsets = { TileCoordsXY{ -1, -1 }, TileCoordsXY{ -1, 0 }, TileCoordsXY{ -1, 1 },
                                                         TileCoordsXY{ 0, 1 },   TileCoordsXY{ 1, 1 },  TileCoordsXY{ 1, 0 },
                                                         TileCoordsXY{ 1, -1 },  TileCoordsXY{ 0, -1 } };

    static constexpr std::array kNeighbourOpposites = {
        South, SouthWest, West, NorthWest, North, NorthEast, East, SouthEast,
    };

    static constexpr Neighbour _makeNeighbour(const MapDirection mapDirection)
    {
        return Neighbour{ mapDirection, kNeighbourOpposites[mapDirection], kMapDirectionOffsets[mapDirection] };
    }

    static constexpr Neighbour kNeighbourNorth = _makeNeighbour(North);
    static constexpr Neighbour kNeighbourNorthEast = _makeNeighbour(NorthEast);
    static constexpr Neighbour kNeighbourEast = _makeNeighbour(East);
    static constexpr Neighbour kNeighbourSouthEast = _makeNeighbour(SouthEast);
    static constexpr Neighbour kNeighbourSouth = _makeNeighbour(South);
    static constexpr Neighbour kNeighbourSouthWest = _makeNeighbour(SouthWest);
    static constexpr Neighbour kNeighbourWest = _makeNeighbour(West);
    static constexpr Neighbour kNeighbourNorthWest = _makeNeighbour(NorthWest);

    static constexpr std::array kNeighboursOrdinal = {
        kNeighbourNorthEast,
        kNeighbourSouthWest,
        kNeighbourNorthWest,
        kNeighbourSouthEast,
    };

    static constexpr std::array kNeighboursCardinal = { kNeighbourSouth, kNeighbourWest, kNeighbourEast, kNeighbourNorth };

    static constexpr std::array kNeighbours = { kNeighbourNorthEast, kNeighbourSouthWest, kNeighbourNorthWest,
                                                kNeighbourSouthEast, kNeighbourSouth,     kNeighbourWest,
                                                kNeighbourEast,      kNeighbourNorth };

    using HeightMap = BaseMap<float>;
    using DistanceMap = BaseMap<float>;
    using NormalMap = BaseMap<VecXYZ>;
    using Backref = std::optional<TileCoordsXY>;
    using BackrefMap = BaseMap<Backref>;
    using TileCoordsXYSet = std::unordered_set<TileCoordsXY, TileCoordsXYHash>;
    using BackrefsMap = BaseMap<TileCoordsXYSet>;
    using MapDirectionMaskMap = BaseMap<MapDirectionMask>;

    // inline auto inBoundsNeighbourPositionsFrom(const MapDirectionMaskMap& mapDirectionMaskMap, const TileCoordsXY& pos,const std::span<const Neighbour>& candidates = kNeighbours)
    // {
    //     const MapDirectionMask& mapDirectionMask = mapDirectionMaskMap[pos];
    //
    //     return candidates
    //         | std::ranges::views::filter([mapDirectionMask](const Neighbour& n) { return mapDirectionMask.has(n.direction); })
    //         | std::ranges::views::transform([pos](const Neighbour& n) { return pos + n.offset; })
    //         | std::ranges::views::filter(
    //                [mapDirectionMaskMap](const TileCoordsXY& p) { return mapDirectionMaskMap.inBounds(p); });
    // }
    //
    // template<class T>
    // auto inBoundsNeighbourPositionsOf(const BaseMap<T>& map, const TileCoordsXY& pos, const std::span<const Neighbour>& candidates = kNeighbours)
    // {
    //     return candidates
    //         | std::ranges::views::transform([pos](const Neighbour& n) { return pos + n.offset; })
    //         | std::ranges::views::filter([map](const TileCoordsXY& p) { return map.inBounds(p); });
    // }
} // namespace OpenRCT2::World::MapGenerator
