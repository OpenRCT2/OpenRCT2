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

#include <cstdint>

constexpr int32_t kScreenCoordsTileWidth = 64;
constexpr int32_t kScreenCoordsTileWidthHalf = kScreenCoordsTileWidth / 2;
constexpr int32_t kScreenCoordsTileHeight = 32;
constexpr int32_t kScreenCoordsTileHeightHalf = kScreenCoordsTileHeight / 2;

struct ScreenSize
{
    int32_t width{};
    int32_t height{};

    constexpr ScreenSize() = default;
    constexpr ScreenSize(int32_t _width, int32_t _height)
        : width(_width)
        , height(_height)
    {
    }

    constexpr bool operator==(const ScreenSize& other) const
    {
        return width == other.width && height == other.height;
    }

    constexpr bool operator!=(const ScreenSize& other) const
    {
        return !(*this == other);
    }

    constexpr ScreenSize operator*(int32_t scalar) const
    {
        return ScreenSize{ width * scalar, height * scalar };
    }

    constexpr ScreenSize operator/(int32_t scalar) const
    {
        return ScreenSize{ width / scalar, height / scalar };
    }

    constexpr ScreenSize operator+(const ScreenSize& other) const
    {
        return ScreenSize{ width + other.width, height + other.height };
    }

    constexpr ScreenSize operator-(const ScreenSize& other) const
    {
        return ScreenSize{ width - other.width, height - other.height };
    }
};

struct ScreenCoordsXY
{
    int32_t x{};
    int32_t y{};

    constexpr ScreenCoordsXY() = default;
    constexpr ScreenCoordsXY(int32_t _x, int32_t _y)
        : x(_x)
        , y(_y)
    {
    }

    constexpr ScreenCoordsXY operator-(const ScreenCoordsXY& rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    constexpr ScreenCoordsXY& operator+=(const ScreenCoordsXY& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr ScreenCoordsXY& operator-=(const ScreenCoordsXY& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    constexpr ScreenCoordsXY operator+(const ScreenCoordsXY& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    constexpr ScreenCoordsXY operator+(const ScreenSize& rhs) const
    {
        return { x + rhs.width, y + rhs.height };
    }

    constexpr ScreenCoordsXY operator-(const ScreenSize& rhs) const
    {
        return { x - rhs.width, y - rhs.height };
    }

    constexpr bool operator==(const ScreenCoordsXY& other) const
    {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const ScreenCoordsXY& other) const
    {
        return !(*this == other);
    }
};

/**
 * Represents a line on the screen
 */

struct ScreenLine : public CoordsRange<ScreenCoordsXY>
{
    constexpr ScreenLine(const ScreenCoordsXY& leftTop, const ScreenCoordsXY& rightBottom)
        : CoordsRange<ScreenCoordsXY>(leftTop, rightBottom)
    {
    }
};

template<class T>
struct RectRange : public CoordsRange<T>
{
    using CoordsRange<T>::CoordsRange;

    constexpr int32_t getLeft() const
    {
        return CoordsRange<T>::getX1();
    }
    constexpr int32_t getTop() const
    {
        return CoordsRange<T>::getY1();
    }
    constexpr int32_t getRight() const
    {
        return CoordsRange<T>::getX2();
    }
    constexpr int32_t getBottom() const
    {
        return CoordsRange<T>::getY2();
    }

    constexpr RectRange(int32_t left, int32_t top, int32_t right, int32_t bottom)
        : RectRange({ left, top }, { right, bottom })
    {
    }

    constexpr RectRange(const T& leftTop, const T& rightBottom)
        : CoordsRange<T>(leftTop, rightBottom)
    {
    }
};

/**
 * Represents a rectangular range on the screen
 */

struct ScreenRect : public RectRange<ScreenCoordsXY>
{
    using RectRange::RectRange;

    constexpr int32_t getWidth() const
    {
        return getRight() - getLeft();
    }
    constexpr int32_t getHeight() const
    {
        return getBottom() - getTop();
    }
    constexpr bool contains(const ScreenCoordsXY& coords) const
    {
        return coords.x >= getLeft() && coords.x <= getRight() && coords.y >= getTop() && coords.y <= getBottom();
    }
};
