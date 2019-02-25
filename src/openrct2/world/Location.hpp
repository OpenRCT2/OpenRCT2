/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <algorithm>

#define LOCATION_NULL ((int16_t)(uint16_t)0x8000)
#define RCT_XY8_UNDEFINED 0xFFFF
#define MakeXY16(x, y)                                                                                                         \
    {                                                                                                                          \
        (int16_t)(x), (int16_t)(y)                                                                                             \
    }

#pragma pack(push, 1)
struct LocationXY8
{
    union
    {
        struct
        {
            uint8_t x, y;
        };
        uint16_t xy;
    };
};
assert_struct_size(LocationXY8, 2);

struct sLocationXY8
{
    int8_t x, y;
};
assert_struct_size(sLocationXY8, 2);

struct LocationXY16
{
    int16_t x, y;
};
assert_struct_size(LocationXY16, 4);

struct LocationXYZ16
{
    int16_t x, y, z;
};
assert_struct_size(LocationXYZ16, 6);
#pragma pack(pop)

constexpr int32_t COORDS_NULL = -1;

/**
 * Tile coordinates use 1 x/y increment per tile and 1 z increment per step.
 * Regular ('big', 'sprite') coordinates use 32 x/y increments per tile and 8 z increments per step.
 */
struct CoordsXY
{
    int32_t x = 0;
    int32_t y = 0;

    CoordsXY() = default;
    constexpr CoordsXY(int32_t _x, int32_t _y)
        : x(_x)
        , y(_y)
    {
    }
};

struct TileCoordsXY
{
    TileCoordsXY() = default;
    TileCoordsXY(int32_t x_, int32_t y_)
        : x(x_)
        , y(y_)
    {
    }
    explicit TileCoordsXY(CoordsXY c)
        : x(c.x / 32)
        , y(c.y / 32)
    {
    }
    TileCoordsXY& operator+=(const TileCoordsXY rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    int32_t x = 0, y = 0;
};

struct CoordsXYZ
{
    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;

    CoordsXYZ() = default;
    constexpr CoordsXYZ(int32_t _x, int32_t _y, int32_t _z)
        : x(_x)
        , y(_y)
        , z(_z)
    {
    }
};

struct TileCoordsXYZ
{
    TileCoordsXYZ() = default;
    TileCoordsXYZ(int32_t x_, int32_t y_, int32_t z_)
        : x(x_)
        , y(y_)
        , z(z_)
    {
    }
    explicit TileCoordsXYZ(CoordsXY c, int32_t z_)
        : x(c.x / 32)
        , y(c.y / 32)
        , z(z_)
    {
    }
    explicit TileCoordsXYZ(CoordsXYZ c)
        : x(c.x / 32)
        , y(c.y / 32)
        , z(c.z / 8)
    {
    }
    TileCoordsXYZ& operator+=(const TileCoordsXY rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    bool operator==(const TileCoordsXYZ& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }
    bool operator!=(const TileCoordsXYZ& other) const
    {
        return !(*this == other);
    }

    int32_t x = 0, y = 0, z = 0;
};

/**
 * Cardinal directions are represented by the Direction type. It has four
 * possible values:
 * 0 is X-decreasing
 * 1 is Y-increasing
 * 2 is X-increasing
 * 3 is Y-decreasing
 * Direction is not used to model up/down, or diagonal directions.
 */
typedef uint8_t Direction;

/**
 * Given a direction, return the direction that points the other way,
 * on the same axis.
 */
[[maybe_unused]] static constexpr Direction direction_reverse(Direction dir)
{
    return dir ^ 2;
}

constexpr bool direction_valid(Direction dir)
{
    return dir < 4;
}

struct CoordsXYZD
{
    int32_t x, y, z;
    Direction direction;

    bool isNull() const
    {
        return x == COORDS_NULL;
    };
};

struct TileCoordsXYZD
{
    int32_t x, y, z;
    Direction direction;

    bool isNull() const
    {
        return x == COORDS_NULL;
    };
};

/**
 * Represents a rectangular range of the map using regular coordinates (32 per tile).
 */
struct MapRange
{
    CoordsXY LeftTop;
    CoordsXY RightBottom;

    int32_t GetLeft() const
    {
        return LeftTop.x;
    }
    int32_t GetTop() const
    {
        return LeftTop.y;
    }
    int32_t GetRight() const
    {
        return RightBottom.x;
    }
    int32_t GetBottom() const
    {
        return RightBottom.y;
    }

    MapRange()
        : MapRange(0, 0, 0, 0)
    {
    }
    MapRange(int32_t left, int32_t top, int32_t right, int32_t bottom)
        : LeftTop(left, top)
        , RightBottom(right, bottom)
    {
    }

    MapRange Normalise() const
    {
        auto result = MapRange(
            std::min(GetLeft(), GetRight()), std::min(GetTop(), GetBottom()), std::max(GetLeft(), GetRight()),
            std::max(GetTop(), GetBottom()));
        return result;
    }
};
