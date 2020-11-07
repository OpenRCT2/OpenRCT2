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

    explicit operator int8_t() const;
    ZoomLevel operator++(int);
    ZoomLevel& operator++();
    ZoomLevel operator--(int);
    ZoomLevel& operator--();
    ZoomLevel& operator=(const ZoomLevel& other);
    ZoomLevel& operator+=(const ZoomLevel& rhs);
    ZoomLevel& operator-=(const ZoomLevel& rhs);

    friend ZoomLevel operator+(ZoomLevel lhs, const ZoomLevel& rhs);
    friend ZoomLevel operator-(ZoomLevel lhs, const ZoomLevel& rhs);
    friend bool operator==(const ZoomLevel& lhs, const ZoomLevel& rhs);
    friend bool operator!=(const ZoomLevel& lhs, const ZoomLevel& rhs);
    friend bool operator>=(const ZoomLevel& lhs, const ZoomLevel& rhs);
    friend bool operator<=(const ZoomLevel& lhs, const ZoomLevel& rhs);
    friend bool operator>(const ZoomLevel& lhs, const ZoomLevel& rhs);
    friend bool operator<(const ZoomLevel& lhs, const ZoomLevel& rhs);

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
