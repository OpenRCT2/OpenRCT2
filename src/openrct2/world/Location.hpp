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

#define LOCATION_NULL     ((int16_t)(uint16_t)0x8000)
#define RCT_XY8_UNDEFINED 0xFFFF
#define MakeXY16(x, y)    {(int16_t)(x), (int16_t)(y)}

#pragma pack(push, 1)
struct LocationXY8 {
    union {
        struct {
            uint8_t x, y;
        };
        uint16_t xy;
    };
};
assert_struct_size(LocationXY8, 2);

struct sLocationXY8 {
    int8_t x, y;
};
assert_struct_size(sLocationXY8, 2);

struct LocationXY16 {
    int16_t x, y;
};
assert_struct_size(LocationXY16, 4);


struct LocationXYZ16 {
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
    int32_t x, y;
};

struct TileCoordsXY
{
    TileCoordsXY() = default;
    TileCoordsXY(int32_t x_, int32_t y_) : x(x_), y(y_) {}
    explicit TileCoordsXY(CoordsXY c) : x(c.x / 32), y(c.y / 32) {}
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
    int32_t x, y, z;
};

struct TileCoordsXYZ
{
    TileCoordsXYZ() = default;
    TileCoordsXYZ(int32_t x_, int32_t y_, int32_t z_) : x(x_), y(y_), z(z_) {}
    explicit TileCoordsXYZ(CoordsXY c, int32_t z_) : x(c.x / 32), y(c.y / 32), z(z_) {}
    explicit TileCoordsXYZ(CoordsXYZ c) : x(c.x / 32), y(c.y / 32), z(c.z / 8) {}
    TileCoordsXYZ& operator+=(const TileCoordsXY rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    int32_t x = 0, y = 0, z = 0;
};

struct CoordsXYZD
{
    int32_t x, y, z;
    uint8_t direction;

    bool isNull() const { return x == COORDS_NULL; };
};

struct TileCoordsXYZD
{
    int32_t x, y, z;
    uint8_t direction;

    bool isNull() const { return x == COORDS_NULL; };
};
