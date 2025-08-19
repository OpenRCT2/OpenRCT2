/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Location.hpp"

#include <cmath>

struct VecXY
{
    float x{};
    float y{};

    constexpr VecXY() = default;
    constexpr VecXY(float _x, float _y)
        : x(_x)
        , y(_y)
    {
    }
    constexpr VecXY(int32_t _x, int32_t _y)
        : x(static_cast<float>(_x))
        , y(static_cast<float>(_y))
    {
    }

    constexpr VecXY& operator+=(const VecXY& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr VecXY& operator-=(const VecXY& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    constexpr VecXY& operator*=(const float rhs)
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    constexpr VecXY& operator/=(const float rhs)
    {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    constexpr VecXY operator+(const VecXY& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    constexpr VecXY operator-(const VecXY& rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    constexpr VecXY operator+(const float rhs) const
    {
        return { x + rhs, y + rhs };
    }

    constexpr VecXY operator-(const float rhs) const
    {
        return { x - rhs, y - rhs };
    }

    constexpr VecXY operator*(const float rhs) const
    {
        return { x * rhs, y * rhs };
    }

    constexpr VecXY operator/(const float rhs) const
    {
        return { x / rhs, y / rhs };
    }

    constexpr bool operator==(const VecXY& other) const
    {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const VecXY& other) const
    {
        return !(*this == other);
    }

    float Length() const
    {
        return sqrt(pow(x, 2) + pow(y, 2));
    }

    VecXY Normalize() const
    {
        const auto length = Length();
        return { x / length, y / length };
    }

    constexpr TileCoordsXY AsTileCoordsXY() const
    {
        return { static_cast<int32_t>(x), static_cast<int32_t>(y) };
    }
};

struct VecXYZ : VecXY
{
    float z{};

    constexpr VecXYZ() = default;
    constexpr VecXYZ(float _x, float _y, float _z)
        : VecXY(_x, _y)
        , z(_z)
    {
    }

    constexpr VecXYZ(const VecXY& c, float _z)
        : VecXY(c)
        , z(_z)
    {
    }

    constexpr VecXYZ& operator+=(const VecXYZ& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    constexpr VecXYZ& operator-=(const VecXYZ& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    constexpr VecXY& operator*=(const float rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    constexpr VecXY& operator/=(const float rhs)
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }

    constexpr VecXYZ operator+(const VecXYZ& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z };
    }

    constexpr VecXYZ operator-(const VecXYZ& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z };
    }

    constexpr VecXYZ operator*(const float rhs) const
    {
        return { x * rhs, y * rhs, z * rhs };
    }

    constexpr VecXYZ operator/(const float rhs) const
    {
        return { x / rhs, y / rhs, z / rhs };
    }

    constexpr bool operator==(const VecXYZ& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    float Length() const
    {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    VecXYZ Normalize() const
    {
        const auto length = Length();
        return { x / length, y / length, z / length };
    }

    constexpr TileCoordsXYZ AsTileCoordsXYZ() const
    {
        return { static_cast<int32_t>(x), static_cast<int32_t>(y), static_cast<int32_t>(z) };
    }

    float Dot(const VecXYZ& rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    float Angle(const VecXYZ& rhs) const
    {
        return std::acos(Dot(rhs) / (Length() * rhs.Length()));
    }
};
