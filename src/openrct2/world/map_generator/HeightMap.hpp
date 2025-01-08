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
        const uint16_t width;
        const uint16_t height;

        HeightMap(int32_t tagetWidth, int32_t targetHeight)
            : _height(tagetWidth * targetHeight)
            , width(tagetWidth)
            , height(targetHeight)
        {
        }

        HeightMap(const HeightMap& heightMap) = default;

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

        uint8_t* data()
        {
            return _height.data();
        }

        const uint8_t* data() const
        {
            return _height.data();
        }

        size_t size() const
        {
            return _height.size();
        }
    };
} // namespace OpenRCT2::World::MapGenerator
