/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

struct ZoomLevel
{
private:
    int8_t _level{};

public:
    constexpr ZoomLevel() = default;

    constexpr ZoomLevel(int8_t level)
        : _level(level)
    {
    }

    constexpr ZoomLevel(const ZoomLevel& rhs)
        : _level(rhs._level)
    {
    }

    explicit operator int8_t() const
    {
        return _level;
    }

    ZoomLevel operator++(int)
    {
        ZoomLevel tmp(*this);
        operator++();
        return tmp;
    }

    ZoomLevel& operator++()
    {
        _level++;
        return *this;
    }

    ZoomLevel operator--(int)
    {
        ZoomLevel tmp(*this);
        operator--();
        return tmp;
    }

    ZoomLevel& operator--()
    {
        _level--;
        return *this;
    }

    ZoomLevel& operator=(const ZoomLevel& other)
    {
        _level = other._level;
        return *this;
    }

    ZoomLevel& operator+=(const ZoomLevel& rhs)
    {
        _level += rhs._level;
        return *this;
    }

    ZoomLevel& operator-=(const ZoomLevel& rhs)
    {
        _level -= rhs._level;
        return *this;
    }

    friend ZoomLevel operator+(ZoomLevel lhs, const ZoomLevel& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    friend ZoomLevel operator-(ZoomLevel lhs, const ZoomLevel& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    friend bool operator==(const ZoomLevel& lhs, const ZoomLevel& rhs)
    {
        return lhs._level == rhs._level;
    }

    friend bool operator!=(const ZoomLevel& lhs, const ZoomLevel& rhs)
    {
        return lhs._level != rhs._level;
    }

    friend bool operator>=(const ZoomLevel& lhs, const ZoomLevel& rhs)
    {
        return lhs._level >= rhs._level;
    }

    friend bool operator<=(const ZoomLevel& lhs, const ZoomLevel& rhs)
    {
        return lhs._level <= rhs._level;
    }

    friend bool operator>(const ZoomLevel& lhs, const ZoomLevel& rhs)
    {
        return lhs._level > rhs._level;
    }

    friend bool operator<(const ZoomLevel& lhs, const ZoomLevel& rhs)
    {
        return lhs._level < rhs._level;
    }

    template<typename T> friend T operator*(const T& lhs, const ZoomLevel& rhs)
    {
        if (rhs._level < 0)
            return lhs >> -rhs._level;
        else
            return lhs << rhs._level;
    }

    template<typename T> friend T operator/(const T& lhs, const ZoomLevel& rhs)
    {
        if (rhs._level < 0)
            return lhs << -rhs._level;
        else
            return lhs >> rhs._level;
    }

    static ZoomLevel min();

    static constexpr ZoomLevel max()
    {
        return 3;
    }
};
