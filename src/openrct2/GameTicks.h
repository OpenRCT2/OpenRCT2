/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"

// TODO: Move or remove these constants if
//       we can use the constants defined
//       in Context.h
namespace TickConversion
{
    constexpr uint32_t kGameUpdateFPS = 40;
    constexpr uint32_t kGameUpdateMs = 1000 / kGameUpdateFPS;
} // namespace TickConversion

template<typename T, typename Tag> struct GameTicks
{
    using ValueType = T;

    ValueType Value;

    static constexpr GameTicks FromSeconds(uint32_t seconds)
    {
        GameTicks result = FromMilliseconds(seconds * 1000);
        return result;
    }
    static constexpr GameTicks FromMilliseconds(uint32_t milliseconds)
    {
        GameTicks<T, Tag> result;
        result.Value = (milliseconds + (TickConversion::kGameUpdateTimeMs - 1)) /
            TickConversion::kGameUpdateTimeMs /* 25 */;
        return result;
    }

    constexpr void operator=(T rhs)
    {
        Value = rhs;
    }

    constexpr GameTicks& operator++()
    {
        ++Value;
        return *this;
    }
    constexpr GameTicks operator++(int)
    {
        GameTicks<T, Tag> temp = *this;
        Value++;
        return temp;
    }
    constexpr GameTicks& operator--()
    {
        --Value;
        return *this;
    }
    constexpr GameTicks operator--(int)
    {
        GameTicks<T, Tag> temp = *this;
        Value--;
        return temp;
    }

    constexpr GameTicks operator+(GameTicks rhs) const
    {
        return GameTicks(Value + rhs.Value);
    }
    constexpr GameTicks operator-(GameTicks rhs) const
    {
        return GameTicks(Value - rhs.Value);
    }
    constexpr GameTicks operator*(GameTicks rhs) const
    {
        return GameTicks(Value * rhs.Value);
    }
    constexpr GameTicks operator/(GameTicks rhs) const
    {
        return GameTicks(Value / rhs.GameTicks);
    }

    constexpr GameTicks& operator+=(GameTicks rhs)
    {
        Value += rhs.Value;
        return *this;
    }
    constexpr GameTicks& operator-=(GameTicks rhs)
    {
        Value -= rhs.Value;
        return *this;
    }

    constexpr bool operator==(const GameTicks& rhs) const
    {
        return Value == rhs.Value;
    }

    constexpr bool operator!=(const GameTicks& rhs) const
    {
        return Value != rhs.Value;
    }

    constexpr bool operator<=>(const GameTicks& rhs) const
    {
        return Value <=> rhs.Value;
    }

    constexpr GameTicks& operator%(const GameTicks& rhs)
    {
        GameTicks result;
        result.Value = Value % rhs.Value;
        return result;
    }
};
