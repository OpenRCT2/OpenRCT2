/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"

template<typename THolderType, typename TEnumType>
struct FlagHolder
{
    THolderType holder{};

    constexpr void clearAll()
    {
        holder = 0;
    }

    [[nodiscard]] constexpr bool isEmpty() const
    {
        return holder == 0;
    }

    [[nodiscard]] constexpr bool has(TEnumType flag) const
    {
        return (holder & EnumToFlag(flag)) != 0;
    }

    constexpr void set(TEnumType flag)
    {
        holder |= EnumToFlag(flag);
    }

    constexpr void unset(TEnumType flag)
    {
        holder &= ~EnumToFlag(flag);
    }

    constexpr void flip(TEnumType flag)
    {
        holder ^= EnumToFlag(flag);
    }
};
