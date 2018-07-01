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

template<size_t N> struct _LocationFieldCombine {};
template<> struct _LocationFieldCombine<1> { typedef uint8_t type; };
template<> struct _LocationFieldCombine<2> { typedef uint16_t type; }; // xy8
template<> struct _LocationFieldCombine<4> { typedef uint32_t type; }; // xy16
template<> struct _LocationFieldCombine<8> { typedef uint64_t type; }; // xy32
template<> struct _LocationFieldCombine<6> // xyz16
{
    struct LocationFieldCombineStub {uint8_t dummy[6];};
    typedef LocationFieldCombineStub type;
};

template<> struct _LocationFieldCombine<12> // xyz32
{
    struct LocationFieldCombineStub { uint8_t dummy[12]; };
    typedef LocationFieldCombineStub type;
};

template<typename T> struct _LocationBaseXY
{
    union
    {
        struct
        {
            T x, y;
        };
        typename _LocationFieldCombine<sizeof(T) * 2>::type xy;
    };

    _LocationBaseXY<T>& operator+=(const _LocationBaseXY<T> rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    _LocationBaseXY<T>& operator-=(const _LocationBaseXY<T> rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    _LocationBaseXY<T>& operator/=(const T val)
    {
        x /= val;
        y /= val;
        return *this;
    }

    _LocationBaseXY<T>& operator*=(const T val)
    {
        x *= val;
        y *= val;
        return *this;
    }
};

template<typename T> struct _LocationBaseXYZ
{
    union
    {
        struct
        {
            T x, y, z;
        };
        typename _LocationFieldCombine<sizeof(T) * 3>::type xyz;
    };

    _LocationBaseXY<T>& getXY()
    {
        return reinterpret_cast<_LocationBaseXY<T>&>(*this);
    }

    _LocationBaseXYZ<T>& operator+=(const _LocationBaseXYZ<T> rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    _LocationBaseXYZ<T>& operator-=(const _LocationBaseXYZ<T> rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    _LocationBaseXYZ<T>& operator/=(const T val)
    {
        x /= val;
        y /= val;
        z /= val;
        return *this;
    }

    _LocationBaseXYZ<T>& operator*=(const T val)
    {
        x *= val;
        y *= val;
        z *= val;
        return *this;
    }
};

struct LocationXY8 : _LocationBaseXY<uint8_t> {};
assert_struct_size(LocationXY8, 2);

struct sLocationXY8 : _LocationBaseXY<int8_t> {};
assert_struct_size(sLocationXY8, 2);

struct LocationXY16 : _LocationBaseXY<int16_t> {};
assert_struct_size(LocationXY16, 4);

struct LocationXY32 : _LocationBaseXY<int32_t> {};
assert_struct_size(LocationXY32, 8);

struct LocationXYZ16 : _LocationBaseXYZ<int16_t> {};
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
