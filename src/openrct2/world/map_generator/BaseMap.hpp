/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Location.hpp"
#include "../Vec.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace OpenRCT2::World::MapGenerator
{
    template<typename T>
    class BaseMap
    {
    private:
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

        T& operator[](TileCoordsXY pos)
        {
            assert(pos.x >= 0 || pos.y >= 0 || pos.x < width || pos.y < height);
            return _value[pos.y * width + pos.x];
        }

        const T& operator[](TileCoordsXY pos) const
        {
            assert(pos.x >= 0 || pos.y >= 0 || pos.x < width || pos.y < height);
            return _value[pos.y * width + pos.x];
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
    };

    using HeightMap = BaseMap<float>;
    using NormalMap = BaseMap<VecXYZ>;
} // namespace OpenRCT2::World::MapGenerator
