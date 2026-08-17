/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

template<class T>
struct CoordsRange
{
    T Point1{ 0, 0 };
    T Point2{ 0, 0 };

    constexpr int32_t GetX1() const
    {
        return Point1.x;
    }
    constexpr int32_t GetY1() const
    {
        return Point1.y;
    }
    constexpr int32_t GetX2() const
    {
        return Point2.x;
    }
    constexpr int32_t GetY2() const
    {
        return Point2.y;
    }

    constexpr CoordsRange() = default;
    constexpr CoordsRange(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
        : CoordsRange({ x1, y1 }, { x2, y2 })
    {
    }

    constexpr CoordsRange(const T& pointOne, const T& pointTwo)
        : Point1(pointOne)
        , Point2(pointTwo)
    {
    }
};
