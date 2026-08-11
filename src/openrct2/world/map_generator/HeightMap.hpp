/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Location.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace OpenRCT2::World::MapGenerator
{
    class HeightMap
    {
    private:
        std::vector<uint8_t> _height;

    public:
        uint16_t width{};
        uint16_t height{};
        uint8_t density{};

        HeightMap(int32_t targetWidth, int32_t targetHeight)
            : _height(targetWidth * targetHeight)
            , width(targetWidth)
            , height(targetHeight)
            , density(1)
        {
        }

        HeightMap(int32_t baseWidth, int32_t baseHeight, uint8_t density_)
            : _height((baseWidth * density_) * (baseHeight * density_))
            , width(baseWidth * density_)
            , height(baseHeight * density_)
            , density(density_)
        {
        }

        HeightMap() = default;

        uint8_t& operator[](TileCoordsXY pos)
        {
            assert(pos.x >= 0 || pos.y >= 0 || pos.x < width || pos.y < height);
            return _height[pos.y * width + pos.x];
        }

        const uint8_t& operator[](TileCoordsXY pos) const
        {
            assert(pos.x >= 0 || pos.y >= 0 || pos.x < width || pos.y < height);
            return _height[pos.y * width + pos.x];
        }

        void clear()
        {
            _height.clear();
        }

        uint8_t* data()
        {
            return _height.data();
        }

        const uint8_t* data() const
        {
            return _height.data();
        }

        bool empty() const
        {
            return _height.empty();
        }

        size_t size() const
        {
            return _height.size();
        }
    };
} // namespace OpenRCT2::World::MapGenerator
