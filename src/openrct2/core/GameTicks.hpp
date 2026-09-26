/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <compare>
#include <cstdint>
#include <type_traits>

namespace OpenRCT2
{
    /// Strongly typed game tick counter, to prevent mixing tick counts with other quantities.
    struct GameTicks
    {
        using ValueType = uint32_t;

        ValueType Value{};

        constexpr GameTicks& operator++() noexcept
        {
            ++Value;
            return *this;
        }

        friend constexpr GameTicks operator+(GameTicks lhs, GameTicks rhs) noexcept
        {
            return GameTicks{ static_cast<ValueType>(lhs.Value + rhs.Value) };
        }

        friend constexpr GameTicks operator-(GameTicks lhs, GameTicks rhs) noexcept
        {
            return GameTicks{ static_cast<ValueType>(lhs.Value - rhs.Value) };
        }

        auto operator<=>(const GameTicks&) const = default;
    };

    static_assert(std::is_trivially_copyable_v<GameTicks> && std::is_standard_layout_v<GameTicks>);
} // namespace OpenRCT2