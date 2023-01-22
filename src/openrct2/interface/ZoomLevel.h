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
namespace OpenRCT2
{
    struct ZoomLevel
    {
    private:
        int8_t _level{};

    public:
        constexpr ZoomLevel() = default;

        explicit constexpr ZoomLevel(int8_t level)
            : _level(level)
        {
        }

        constexpr ZoomLevel(const ZoomLevel& rhs)
            : _level(rhs._level)
        {
        }

        explicit constexpr operator int8_t() const
        {
            return _level;
        }

        ZoomLevel operator++(int);
        ZoomLevel& operator++();
        ZoomLevel operator--(int);
        ZoomLevel& operator--();
        ZoomLevel& operator=(const ZoomLevel& other);
        ZoomLevel& operator+=(const ZoomLevel& rhs);
        ZoomLevel& operator-=(const ZoomLevel& rhs);

        friend ZoomLevel operator+(ZoomLevel lhs, const ZoomLevel& rhs);
        friend ZoomLevel operator-(ZoomLevel lhs, const ZoomLevel& rhs);
        friend ZoomLevel operator+(ZoomLevel lhs, int8_t rhs);
        friend ZoomLevel operator-(ZoomLevel lhs, int8_t rhs);
        friend constexpr bool operator==(const ZoomLevel& lhs, const ZoomLevel& rhs);
        friend constexpr bool operator!=(const ZoomLevel& lhs, const ZoomLevel& rhs);
        friend constexpr bool operator>=(const ZoomLevel& lhs, const ZoomLevel& rhs);
        friend constexpr bool operator<=(const ZoomLevel& lhs, const ZoomLevel& rhs);
        friend constexpr bool operator>(const ZoomLevel& lhs, const ZoomLevel& rhs);
        friend constexpr bool operator<(const ZoomLevel& lhs, const ZoomLevel& rhs);

        template<typename T> T ApplyTo(const T& lhs) const
        {
            if (_level < 0)
                return lhs >> -_level;

            return lhs << _level;
        }

        template<typename T> T ApplyInversedTo(const T& lhs) const
        {
            if (_level < 0)
                return lhs << -_level;

            return lhs >> _level;
        }

        static ZoomLevel min();

        static constexpr ZoomLevel max()
        {
            return ZoomLevel{ 3 };
        }
    };

    constexpr bool operator==(const ZoomLevel& lhs, const ZoomLevel& rhs)
    {
        return lhs._level == rhs._level;
    }

    constexpr bool operator!=(const ZoomLevel& lhs, const ZoomLevel& rhs)
    {
        return lhs._level != rhs._level;
    }

    constexpr bool operator>=(const ZoomLevel& lhs, const ZoomLevel& rhs)
    {
        return lhs._level >= rhs._level;
    }

    constexpr bool operator<=(const ZoomLevel& lhs, const ZoomLevel& rhs)
    {
        return lhs._level <= rhs._level;
    }

    constexpr bool operator>(const ZoomLevel& lhs, const ZoomLevel& rhs)
    {
        return lhs._level > rhs._level;
    }

    constexpr bool operator<(const ZoomLevel& lhs, const ZoomLevel& rhs)
    {
        return lhs._level < rhs._level;
    }
} // namespace OpenRCT2
