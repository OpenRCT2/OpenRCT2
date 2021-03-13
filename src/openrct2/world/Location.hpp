/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <algorithm>

constexpr const int16_t LOCATION_NULL = -32768;

constexpr const int32_t COORDS_XY_STEP = 32;
constexpr const int32_t COORDS_XY_HALF_TILE = (COORDS_XY_STEP / 2);
constexpr const int32_t COORDS_Z_STEP = 8;
constexpr const int32_t COORDS_Z_PER_TINY_Z = 16;

constexpr const auto NumOrthogonalDirections = 4;

#pragma pack(push, 1)

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

constexpr int32_t COORDS_NULL = 0xFFFF8000;

struct ScreenCoordsXY
{
    int32_t x = 0;
    int32_t y = 0;

    ScreenCoordsXY() = default;
    constexpr ScreenCoordsXY(int32_t _x, int32_t _y)
        : x(_x)
        , y(_y)
    {
    }

    const ScreenCoordsXY operator-(const ScreenCoordsXY& rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    ScreenCoordsXY& operator+=(const ScreenCoordsXY& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    ScreenCoordsXY& operator-=(const ScreenCoordsXY& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    const ScreenCoordsXY operator+(const ScreenCoordsXY& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    bool operator==(const ScreenCoordsXY& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const ScreenCoordsXY& other) const
    {
        return !(*this == other);
    }
};

struct ScreenSize
{
    int32_t width{};
    int32_t height{};

    ScreenSize() = default;
    constexpr ScreenSize(int32_t _width, int32_t _height)
        : width(_width)
        , height(_height)
    {
    }

    bool operator==(const ScreenSize& other) const
    {
        return width == other.width && height == other.height;
    }

    bool operator!=(const ScreenSize& other) const
    {
        return !(*this == other);
    }
};

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

    constexpr const CoordsXY operator+(const CoordsXY& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    constexpr const CoordsXY operator-(const CoordsXY& rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    constexpr const CoordsXY operator*(const int32_t rhs) const
    {
        return { x * rhs, y * rhs };
    }

    constexpr const CoordsXY operator/(const int32_t rhs) const
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
        return ToTileStart() + CoordsXY{ COORDS_XY_HALF_TILE, COORDS_XY_HALF_TILE };
    }

    constexpr CoordsXY ToTileStart() const
    {
        return { floor2(x, COORDS_XY_STEP), floor2(y, COORDS_XY_STEP) };
    }

    constexpr bool isNull() const
    {
        return x == COORDS_NULL;
    };

    constexpr void setNull()
    {
        x = COORDS_NULL;
        y = 0;
    }
};

struct CoordsXYZ : public CoordsXY
{
    int32_t z = 0;

    CoordsXYZ() = default;
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

    const CoordsXYZ operator+(const CoordsXYZ& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z };
    }

    const CoordsXYZ operator-(const CoordsXYZ& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z };
    }

    bool operator==(const CoordsXYZ& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    CoordsXYZ ToTileStart() const
    {
        return { floor2(x, COORDS_XY_STEP), floor2(y, COORDS_XY_STEP), z };
    }

    CoordsXYZ ToTileCentre() const
    {
        return ToTileStart() + CoordsXYZ{ COORDS_XY_HALF_TILE, COORDS_XY_HALF_TILE, 0 };
    }
};

struct CoordsXYRangedZ : public CoordsXY
{
    int32_t baseZ = 0;
    int32_t clearanceZ = 0;

    CoordsXYRangedZ() = default;
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

struct TileCoordsXY
{
    int32_t x = 0;
    int32_t y = 0;

    TileCoordsXY() = default;
    constexpr TileCoordsXY(int32_t x_, int32_t y_)
        : x(x_)
        , y(y_)
    {
    }

    explicit TileCoordsXY(const CoordsXY& c)
        : x(c.x / COORDS_XY_STEP)
        , y(c.y / COORDS_XY_STEP)
    {
    }

    const TileCoordsXY operator+(const TileCoordsXY& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    TileCoordsXY& operator+=(const TileCoordsXY& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    TileCoordsXY& operator-=(const TileCoordsXY& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    CoordsXY ToCoordsXY() const
    {
        if (isNull())
        {
            CoordsXY ret{};
            ret.setNull();
            return ret;
        }

        return { x * COORDS_XY_STEP, y * COORDS_XY_STEP };
    }

    TileCoordsXY Rotate(int32_t direction) const
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

    bool operator==(const TileCoordsXY& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const TileCoordsXY& other) const
    {
        return !(*this == other);
    }

    bool isNull() const
    {
        return x == COORDS_NULL;
    };

    void setNull()
    {
        x = COORDS_NULL;
        y = 0;
    }
};

struct TileCoordsXYZ : public TileCoordsXY
{
    int32_t z = 0;

    TileCoordsXYZ() = default;
    constexpr TileCoordsXYZ(int32_t x_, int32_t y_, int32_t z_)
        : TileCoordsXY(x_, y_)
        , z(z_)
    {
    }

    TileCoordsXYZ(const TileCoordsXY& c, int32_t z_)
        : TileCoordsXY(c.x, c.y)
        , z(z_)
    {
    }

    TileCoordsXYZ(const CoordsXY& c, int32_t z_)
        : TileCoordsXY(c)
        , z(z_)
    {
    }

    explicit TileCoordsXYZ(const CoordsXYZ& c)
        : TileCoordsXY(c)
        , z(c.z / COORDS_Z_STEP)
    {
    }

    TileCoordsXYZ& operator+=(const TileCoordsXY& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    TileCoordsXYZ& operator-=(const TileCoordsXY& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
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

    CoordsXYZ ToCoordsXYZ() const
    {
        if (isNull())
        {
            CoordsXYZ ret{};
            ret.setNull();
            return ret;
        }
        return { x * COORDS_XY_STEP, y * COORDS_XY_STEP, z * COORDS_Z_STEP };
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

const Direction INVALID_DIRECTION = 0xFF;

/**
 * Array of all valid cardinal directions, to make it easy to write range-based for loops like:
 *   for (Direction d : ALL_DIRECTIONS)
 */
constexpr Direction ALL_DIRECTIONS[] = { 0, 1, 2, 3 };

/**
 * Given a direction, return the direction that points the other way,
 * on the same axis.
 */
[[maybe_unused]] static constexpr Direction direction_reverse(Direction dir)
{
    return dir ^ 2;
}

[[maybe_unused]] static constexpr bool direction_valid(Direction dir)
{
    return dir < NumOrthogonalDirections;
}

/**
 * Given a direction, return the next cardinal direction, wrapping around if necessary.
 * (TODO: Figure out if this is CW or CCW)
 */
[[maybe_unused]] static constexpr Direction direction_next(Direction dir)
{
    return (dir + 1) & 0x03;
}

/**
 * Given a direction, return the previous cardinal direction, wrapping around if necessary.
 * (TODO: Figure out if this is CW or CCW)
 */
[[maybe_unused]] static constexpr Direction direction_prev(Direction dir)
{
    return (dir - 1) & 0x03;
}

/**
 * Given two positions, return the cardinal direction which is closest to the direction from 'from' to 'to'.
 */
[[maybe_unused]] static constexpr Direction DirectionFromTo(const CoordsXY& from, const CoordsXY& to)
{
    int16_t x_diff = to.x - from.x;
    int16_t y_diff = to.y - from.y;

    int16_t abs_x = x_diff < 0 ? -x_diff : x_diff;
    int16_t abs_y = y_diff < 0 ? -y_diff : y_diff;

    if (abs_x <= abs_y)
    {
        return y_diff < 0 ? 3 : 1;
    }
    else
    {
        return x_diff < 0 ? 0 : 2;
    }
}

struct CoordsXYZD : public CoordsXYZ
{
    Direction direction = 0;

    CoordsXYZD() = default;
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

    bool operator==(const CoordsXYZD& other) const
    {
        return x == other.x && y == other.y && z == other.z && direction == other.direction;
    }

    bool operator!=(const CoordsXYZD& other) const
    {
        return !(*this == other);
    }

    CoordsXYZD& operator+=(const CoordsXY& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    const CoordsXYZD operator+(const CoordsXY& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z, direction };
    }

    const CoordsXYZD operator+(const CoordsXYZ& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z, direction };
    }

    const CoordsXYZD operator-(const CoordsXY& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z, direction };
    }

    const CoordsXYZD operator-(const CoordsXYZ& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z, direction };
    }

    CoordsXYZD ToTileStart() const
    {
        return { floor2(x, COORDS_XY_STEP), floor2(y, COORDS_XY_STEP), z, direction };
    }

    CoordsXYZD ToTileCentre() const
    {
        return ToTileStart() + CoordsXYZD{ COORDS_XY_HALF_TILE, COORDS_XY_HALF_TILE, 0, 0 };
    }
};

struct TileCoordsXYZD : public TileCoordsXYZ
{
    Direction direction;

    TileCoordsXYZD() = default;
    constexpr TileCoordsXYZD(int32_t x_, int32_t y_, int32_t z_, Direction d_)
        : TileCoordsXYZ(x_, y_, z_)
        , direction(d_)
    {
    }

    TileCoordsXYZD(const TileCoordsXY& t_, int32_t z_, Direction d_)
        : TileCoordsXYZ(t_, z_)
        , direction(d_)
    {
    }

    TileCoordsXYZD(const CoordsXY& c_, int32_t z_, Direction d_)
        : TileCoordsXYZ(c_, z_)
        , direction(d_)
    {
    }

    TileCoordsXYZD(const CoordsXYZ& c_, Direction d_)
        : TileCoordsXYZ(c_)
        , direction(d_)
    {
    }

    TileCoordsXYZD(const CoordsXYZD& c_)
        : TileCoordsXYZ(c_)
        , direction(c_.direction)
    {
    }

    CoordsXYZD ToCoordsXYZD() const
    {
        if (isNull())
        {
            CoordsXYZD ret{};
            ret.setNull();
            return ret;
        }
        return { x * COORDS_XY_STEP, y * COORDS_XY_STEP, z * COORDS_Z_STEP, direction };
    }
};

/**
 * Represents a range of the map using regular coordinates.
 */
template<class T> struct CoordsRange
{
    T Point1{ 0, 0 };
    T Point2{ 0, 0 };

    int32_t GetX1() const
    {
        return Point1.x;
    }
    int32_t GetY1() const
    {
        return Point1.y;
    }
    int32_t GetX2() const
    {
        return Point2.x;
    }
    int32_t GetY2() const
    {
        return Point2.y;
    }

    CoordsRange() = default;
    CoordsRange(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
        : CoordsRange({ x1, y1 }, { x2, y2 })
    {
    }

    CoordsRange(const T& pointOne, const T& pointTwo)
        : Point1(pointOne)
        , Point2(pointTwo)
    {
    }
};

template<class T> struct RectRange : public CoordsRange<T>
{
    using CoordsRange<T>::CoordsRange;

    int32_t GetLeft() const
    {
        return CoordsRange<T>::GetX1();
    }
    int32_t GetTop() const
    {
        return CoordsRange<T>::GetY1();
    }
    int32_t GetRight() const
    {
        return CoordsRange<T>::GetX2();
    }
    int32_t GetBottom() const
    {
        return CoordsRange<T>::GetY2();
    }

    RectRange(int32_t left, int32_t top, int32_t right, int32_t bottom)
        : RectRange({ left, top }, { right, bottom })
    {
    }

    RectRange(const T& leftTop, const T& rightBottom)
        : CoordsRange<T>(leftTop, rightBottom)
    {
    }
};

/**
 * Represents a rectangular range of the map using regular coordinates (32 per tile).
 */

struct MapRange : public RectRange<CoordsXY>
{
    using RectRange::RectRange;

    MapRange Normalise() const
    {
        auto result = MapRange(
            std::min(GetLeft(), GetRight()), std::min(GetTop(), GetBottom()), std::max(GetLeft(), GetRight()),
            std::max(GetTop(), GetBottom()));
        return result;
    }
};

/**
 * Represents a line on the screen
 */

struct ScreenLine : public CoordsRange<ScreenCoordsXY>
{
    ScreenLine(const ScreenCoordsXY& leftTop, const ScreenCoordsXY& rightBottom)
        : CoordsRange<ScreenCoordsXY>(leftTop, rightBottom)
    {
    }
};

/**
 * Represents a rectangular range on the screen
 */

struct ScreenRect : public RectRange<ScreenCoordsXY>
{
    using RectRange::RectRange;

    int32_t GetWidth() const
    {
        return GetRight() - GetLeft();
    }
    int32_t GetHeight() const
    {
        return GetBottom() - GetTop();
    }
    bool Contains(const ScreenCoordsXY& coords) const
    {
        return coords.x >= GetLeft() && coords.x <= GetRight() && coords.y >= GetTop() && coords.y <= GetBottom();
    }
};
