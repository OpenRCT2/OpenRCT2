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

    FlagHolder() = default;

    template<typename... TTypes>
    constexpr FlagHolder(TTypes... types)
        : holder(EnumsToFlags(types...))
    {
    }

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

    template<typename... TTypes>
    [[nodiscard]] constexpr bool hasAny(TTypes... types) const
    {
        return (holder & EnumsToFlags(types...)) != 0;
    }

    template<typename... TTypes>
    [[nodiscard]] constexpr bool hasAll(TTypes... types) const
    {
        return (holder & EnumsToFlags(types...)) == EnumsToFlags(types...);
    }

    template<typename... TTypes>
    constexpr void set(TTypes... types)
    {
        holder |= EnumsToFlags(types...);
    }

    /**
     * For situations where you don’t know upfront whether to set or unset the flag,
     * e.g. in game actions where this is passed as a variable.
     * Otherwise, use set/unset, which are slightly more efficient.
     */
    constexpr void set(TEnumType flag, bool on)
    {
        if (on)
            set(flag);
        else
            unset(flag);
    }

    constexpr void unset(TEnumType flag)
    {
        holder &= ~EnumToFlag(flag);
    }

    template<typename... TTypes>
    constexpr void unset(TTypes... types)
    {
        holder &= ~EnumsToFlags(types...);
    }

    constexpr void flip(TEnumType flag)
    {
        holder ^= EnumToFlag(flag);
    }

    constexpr FlagHolder operator|(const FlagHolder& other) const noexcept
    {
        FlagHolder res = *this;
        res.holder |= other.holder;
        return res;
    }
};
