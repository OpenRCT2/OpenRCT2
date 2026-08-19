/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/CoordsRange.hpp"
#include "../core/Numerics.hpp"
#include "../world/MapLimits.h"

#include <cstdint>

constexpr int16_t kLocationNull = -32768;
constexpr int32_t kCoordsNull = 0xFFFF8000;

constexpr auto kNumOrthogonalDirections = 4;

/**
 * Tile coordinates use 1 x/y increment per tile and 1 z increment per step.
 * Regular ('big', 'sprite') coordinates use 32 x/y increments per tile and 8 z increments per step.
 */
struct CoordsXY
{
    int32_t x{};
    int32_t y{};

    constexpr CoordsXY() = default;
    constexpr CoordsXY(int32_t _x, int32_t _y)
        : x(_x)
        , y(_y)
    {
    }

    constexpr CoordsXY& operator+=(const CoordsXY& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr CoordsXY& operator-=(const CoordsXY& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    constexpr CoordsXY& operator*=(const int32_t rhs)
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    constexpr CoordsXY& operator/=(const int32_t rhs)
    {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    constexpr bool operator>=(const CoordsXY& rhs) const
    {
        return x >= rhs.x && y >= rhs.y;
    }

    constexpr bool operator<=(const CoordsXY& rhs) const
    {
        return x <= rhs.x && y <= rhs.y;
    }

    constexpr CoordsXY operator+(const CoordsXY& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    constexpr CoordsXY operator-(const CoordsXY& rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    constexpr const CoordsXY operator*(const int32_t rhs) const
    {
        return { x * rhs, y * rhs };
    }

    constexpr CoordsXY operator/(const int32_t rhs) const
    {
        return { x / rhs, y / rhs };
    }

    constexpr CoordsXY Rotate(int32_t direction) const
    {
        CoordsXY rotatedCoords;
        switch (direction & 3)
        {
            default:
            case 0:
                rotatedCoords.x = x;
                rotatedCoords.y = y;
                break;
            case 1:
                rotatedCoords.x = y;
                rotatedCoords.y = -x;
                break;
            case 2:
                rotatedCoords.x = -x;
                rotatedCoords.y = -y;
                break;
            case 3:
                rotatedCoords.x = -y;
                rotatedCoords.y = x;
                break;
        }

        return rotatedCoords;
    }

    constexpr bool operator==(const CoordsXY& other) const
    {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const CoordsXY& other) const
    {
        return !(*this == other);
    }

    constexpr CoordsXY ToTileCentre() const
    {
        return ToTileStart() + CoordsXY{ kCoordsXYHalfTile, kCoordsXYHalfTile };
    }

    constexpr CoordsXY ToTileStart() const
    {
        using namespace OpenRCT2::Numerics;

        return { floor2(x, kCoordsXYStep), floor2(y, kCoordsXYStep) };
    }

    constexpr bool IsNull() const
    {
        return x == kCoordsNull;
    }

    constexpr void SetNull()
    {
        x = kCoordsNull;
        y = 0;
    }
};

struct CoordsXYZ : public CoordsXY
{
    int32_t z{};

    constexpr CoordsXYZ() = default;
    constexpr CoordsXYZ(int32_t _x, int32_t _y, int32_t _z)
        : CoordsXY(_x, _y)
        , z(_z)
    {
    }

    constexpr CoordsXYZ(const CoordsXY& c, int32_t _z)
        : CoordsXY(c)
        , z(_z)
    {
    }

    constexpr CoordsXYZ operator+(const CoordsXYZ& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z };
    }

    constexpr CoordsXYZ operator-(const CoordsXYZ& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z };
    }

    constexpr bool operator==(const CoordsXYZ& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    constexpr CoordsXYZ ToTileStart() const
    {
        using namespace OpenRCT2::Numerics;

        return { floor2(x, kCoordsXYStep), floor2(y, kCoordsXYStep), z };
    }

    constexpr CoordsXYZ ToTileCentre() const
    {
        return ToTileStart() + CoordsXYZ{ kCoordsXYHalfTile, kCoordsXYHalfTile, 0 };
    }

    constexpr void SetNull()
    {
        CoordsXY::SetNull();
        z = 0;
    }
};

struct CoordsXYRangedZ : public CoordsXY
{
    int32_t baseZ{};
    int32_t clearanceZ{};

    constexpr CoordsXYRangedZ() = default;
    constexpr CoordsXYRangedZ(int32_t _x, int32_t _y, int32_t _baseZ, int32_t _clearanceZ)
        : CoordsXY(_x, _y)
        , baseZ(_baseZ)
        , clearanceZ(_clearanceZ)
    {
    }

    constexpr CoordsXYRangedZ(const CoordsXY& _c, int32_t _baseZ, int32_t _clearanceZ)
        : CoordsXY(_c)
        , baseZ(_baseZ)
        , clearanceZ(_clearanceZ)
    {
    }

    constexpr CoordsXYRangedZ(const CoordsXYZ& _c, int32_t _clearanceZ)
        : CoordsXY(_c)
        , baseZ(_c.z)
        , clearanceZ(_clearanceZ)
    {
    }
};

namespace OpenRCT2
{
    struct TileElement;
}

struct CoordsXYE : public CoordsXY
{
    CoordsXYE() = default;
    constexpr CoordsXYE(int32_t _x, int32_t _y, OpenRCT2::TileElement* _e)
        : CoordsXY(_x, _y)
        , element(_e)
    {
    }

    constexpr CoordsXYE(const CoordsXY& c, OpenRCT2::TileElement* _e)
        : CoordsXY(c)
        , element(_e)
    {
    }
    OpenRCT2::TileElement* element = nullptr;
};

struct TileCoordsXY
{
    int32_t x{};
    int32_t y{};

    constexpr TileCoordsXY() = default;
    constexpr TileCoordsXY(int32_t x_, int32_t y_)
        : x(x_)
        , y(y_)
    {
    }

    constexpr explicit TileCoordsXY(const CoordsXY& c)
        : x(c.x / kCoordsXYStep)
        , y(c.y / kCoordsXYStep)
    {
    }

    constexpr TileCoordsXY operator+(const TileCoordsXY& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    constexpr TileCoordsXY& operator+=(const TileCoordsXY& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr TileCoordsXY& operator-=(const TileCoordsXY& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    constexpr CoordsXY ToCoordsXY() const
    {
        if (IsNull())
        {
            CoordsXY ret{};
            ret.SetNull();
            return ret;
        }

        return { x * kCoordsXYStep, y * kCoordsXYStep };
    }

    constexpr TileCoordsXY Rotate(int32_t direction) const
    {
        TileCoordsXY rotatedCoords;
        switch (direction & 3)
        {
            default:
            case 0:
                rotatedCoords.x = x;
                rotatedCoords.y = y;
                break;
            case 1:
                rotatedCoords.x = y;
                rotatedCoords.y = -x;
                break;
            case 2:
                rotatedCoords.x = -x;
                rotatedCoords.y = -y;
                break;
            case 3:
                rotatedCoords.x = -y;
                rotatedCoords.y = x;
                break;
        }

        return rotatedCoords;
    }

    constexpr bool operator==(const TileCoordsXY& other) const
    {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const TileCoordsXY& other) const
    {
        return !(*this == other);
    }

    constexpr bool IsNull() const
    {
        return x == kCoordsNull;
    }

    constexpr void SetNull()
    {
        x = kCoordsNull;
        y = 0;
    }
};

struct TileCoordsXYZ : public TileCoordsXY
{
    int32_t z{};

    constexpr TileCoordsXYZ() = default;
    constexpr TileCoordsXYZ(int32_t x_, int32_t y_, int32_t z_)
        : TileCoordsXY(x_, y_)
        , z(z_)
    {
    }

    constexpr TileCoordsXYZ(const TileCoordsXY& c, int32_t z_)
        : TileCoordsXY(c.x, c.y)
        , z(z_)
    {
    }

    constexpr TileCoordsXYZ(const CoordsXY& c, int32_t z_)
        : TileCoordsXY(c)
        , z(z_)
    {
    }

    constexpr explicit TileCoordsXYZ(const CoordsXYZ& c)
        : TileCoordsXY(c)
        , z(c.z / kCoordsZStep)
    {
    }

    constexpr TileCoordsXYZ& operator+=(const TileCoordsXY& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr TileCoordsXYZ& operator-=(const TileCoordsXY& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    constexpr bool operator==(const TileCoordsXYZ& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    constexpr bool operator!=(const TileCoordsXYZ& other) const
    {
        return !(*this == other);
    }

    constexpr CoordsXYZ ToCoordsXYZ() const
    {
        if (IsNull())
        {
            CoordsXYZ ret{};
            ret.SetNull();
            return ret;
        }
        return { x * kCoordsXYStep, y * kCoordsXYStep, z * kCoordsZStep };
    }

    constexpr void SetNull()
    {
        TileCoordsXY::SetNull();
        z = 0;
    }
};

struct TileCoordsXYRangedZ : public TileCoordsXY
{
    int32_t baseZ{};
    int32_t clearanceZ{};

    constexpr TileCoordsXYRangedZ() = default;
    constexpr TileCoordsXYRangedZ(int32_t _x, int32_t _y, int32_t _baseZ, int32_t _clearanceZ)
        : TileCoordsXY(_x, _y)
        , baseZ(_baseZ)
        , clearanceZ(_clearanceZ)
    {
    }

    constexpr TileCoordsXYRangedZ(const TileCoordsXY& _c, int32_t _baseZ, int32_t _clearanceZ)
        : TileCoordsXY(_c)
        , baseZ(_baseZ)
        , clearanceZ(_clearanceZ)
    {
    }

    constexpr TileCoordsXYRangedZ(const TileCoordsXYZ& _c, int32_t _clearanceZ)
        : TileCoordsXY(_c)
        , baseZ(_c.z)
        , clearanceZ(_clearanceZ)
    {
    }
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
using Direction = uint8_t;

const Direction kInvalidDirection = 0xFF;

/**
 * Array of all valid cardinal directions, to make it easy to write range-based for loops like:
 *   for (Direction d : kAllDirections)
 */
constexpr Direction kAllDirections[] = {
    0,
    1,
    2,
    3,
};

/**
 * Given a direction, return the direction that points the other way,
 * on the same axis.
 */
inline constexpr Direction DirectionReverse(Direction dir)
{
    return dir ^ 2;
}

inline constexpr bool DirectionValid(Direction dir)
{
    return dir < kNumOrthogonalDirections;
}

/**
 * Given a direction, return the next cardinal direction, wrapping around if necessary.
 * (TODO: Figure out if this is CW or CCW)
 */
inline constexpr Direction DirectionNext(Direction dir)
{
    return (dir + 1) & 0x03;
}

/**
 * Given a direction, return the previous cardinal direction, wrapping around if necessary.
 * (TODO: Figure out if this is CW or CCW)
 */
inline constexpr Direction DirectionPrev(Direction dir)
{
    return (dir - 1) & 0x03;
}

/**
 * Given two positions, return the cardinal direction which is closest to the direction from 'from' to 'to'.
 */
inline constexpr Direction DirectionFromTo(const CoordsXY& from, const CoordsXY& to)
{
    int16_t x_diff = to.x - from.x;
    int16_t y_diff = to.y - from.y;

    int16_t abs_x = x_diff < 0 ? -x_diff : x_diff;
    int16_t abs_y = y_diff < 0 ? -y_diff : y_diff;

    if (abs_x <= abs_y)
    {
        return y_diff < 0 ? 3 : 1;
    }

    return x_diff < 0 ? 0 : 2;
}

/*
 * Flips the X axis so 1 and 3 are swapped 0 and 2 will stay the same.
 */
inline constexpr Direction DirectionFlipXAxis(Direction direction)
{
    return (direction * 3) % 4;
}

struct CoordsXYZD : public CoordsXYZ
{
    Direction direction{};

    constexpr CoordsXYZD() = default;
    constexpr CoordsXYZD(int32_t _x, int32_t _y, int32_t _z, Direction _d)
        : CoordsXYZ(_x, _y, _z)
        , direction(_d)
    {
    }

    constexpr CoordsXYZD(const CoordsXY& _c, int32_t _z, Direction _d)
        : CoordsXYZ(_c, _z)
        , direction(_d)
    {
    }

    constexpr CoordsXYZD(const CoordsXYZ& _c, Direction _d)
        : CoordsXYZ(_c)
        , direction(_d)
    {
    }

    constexpr bool operator==(const CoordsXYZD& other) const
    {
        return x == other.x && y == other.y && z == other.z && direction == other.direction;
    }

    constexpr bool operator!=(const CoordsXYZD& other) const
    {
        return !(*this == other);
    }

    constexpr CoordsXYZD& operator+=(const CoordsXY& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr CoordsXYZD operator+(const CoordsXY& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z, direction };
    }

    constexpr CoordsXYZD operator+(const CoordsXYZ& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z, direction };
    }

    constexpr CoordsXYZD operator-(const CoordsXY& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z, direction };
    }

    constexpr CoordsXYZD operator-(const CoordsXYZ& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z, direction };
    }

    constexpr CoordsXYZD ToTileStart() const
    {
        using namespace OpenRCT2::Numerics;

        return { floor2(x, kCoordsXYStep), floor2(y, kCoordsXYStep), z, direction };
    }

    constexpr CoordsXYZD ToTileCentre() const
    {
        return ToTileStart() + CoordsXYZD{ kCoordsXYHalfTile, kCoordsXYHalfTile, 0, 0 };
    }
};

struct TileCoordsXYZD : public TileCoordsXYZ
{
    Direction direction{};

    constexpr TileCoordsXYZD() = default;
    constexpr TileCoordsXYZD(int32_t x_, int32_t y_, int32_t z_, Direction d_)
        : TileCoordsXYZ(x_, y_, z_)
        , direction(d_)
    {
    }

    constexpr TileCoordsXYZD(const TileCoordsXYZ& t_, Direction d_)
        : TileCoordsXYZ(t_)
        , direction(d_)
    {
    }

    constexpr TileCoordsXYZD(const TileCoordsXY& t_, int32_t z_, Direction d_)
        : TileCoordsXYZ(t_, z_)
        , direction(d_)
    {
    }

    constexpr TileCoordsXYZD(const CoordsXY& c_, int32_t z_, Direction d_)
        : TileCoordsXYZ(c_, z_)
        , direction(d_)
    {
    }

    constexpr TileCoordsXYZD(const CoordsXYZ& c_, Direction d_)
        : TileCoordsXYZ(c_)
        , direction(d_)
    {
    }

    constexpr TileCoordsXYZD(const CoordsXYZD& c_)
        : TileCoordsXYZ(c_)
        , direction(c_.direction)
    {
    }

    constexpr CoordsXYZD ToCoordsXYZD() const
    {
        if (IsNull())
        {
            CoordsXYZD ret{};
            ret.SetNull();
            return ret;
        }
        return { x * kCoordsXYStep, y * kCoordsXYStep, z * kCoordsZStep, direction };
    }

    constexpr void SetNull()
    {
        TileCoordsXYZ::SetNull();
        direction = kInvalidDirection;
    }
};

/**
 * Represents a rectangular range of the map using regular coordinates (32 per tile).
 */

struct MapRange : public CoordsRange<CoordsXY>
{
    using CoordsRange::CoordsRange;

    constexpr MapRange Normalise() const
    {
        // Don't use std::min/max, as they require <algorithm>, one of C++'s heaviest
        // in this very common header.
        auto result = MapRange(
            GetX1() < GetX2() ? GetX1() : GetX2(), // min
            GetY1() < GetY2() ? GetY1() : GetY2(), // min
            GetX1() > GetX2() ? GetX1() : GetX2(), // max
            GetY1() > GetY2() ? GetY1() : GetY2()  // max
        );
        return result;
    }
};

// This uses the convention from the kTileSlope constants that north is at the bottom of the tile at rotation 0
struct TileCornersZ
{
    int32_t north;
    int32_t east;
    int32_t south;
    int32_t west;

    constexpr bool operator<=(const TileCornersZ& other) const
    {
        return north <= other.north && east <= other.east && south <= other.south && west <= other.west;
    }
};
