/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../util/Hash.hpp"
#include "../Location.hpp"
#include "../Vec.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
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

    using HeightMap = BaseMap<float>;
    using DistanceMap = BaseMap<float>;
    using NormalMap = BaseMap<VecXYZ>;
    using Backref = std::optional<TileCoordsXY>;
    using BackrefMap = BaseMap<Backref>;
    using TileCoordsXYSet = std::unordered_set<TileCoordsXY, TileCoordsXYHash>;
    using BackrefsMap = BaseMap<TileCoordsXYSet>;

} // namespace OpenRCT2::World::MapGenerator
