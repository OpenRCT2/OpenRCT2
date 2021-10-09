
/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <stdexcept>

namespace
{
    [[maybe_unused]] constexpr bool is_power_of_2(int v)
    {
        return v && ((v & (v - 1)) == 0);
    }

    // Rounds an integer down to the given power of 2. y must be a power of 2.
    [[maybe_unused]] constexpr int floor2(const int x, const int y)
    {
        if (!is_power_of_2(y))
            throw std::logic_error("floor2 should only operate on power of 2");
        return x & ~(y - 1);
    }

    // Rounds an integer up to the given power of 2. y must be a power of 2.
    [[maybe_unused]] constexpr int ceil2(const int x, const int y)
    {
        if (!is_power_of_2(y))
            throw std::logic_error("ceil2 should only operate on power of 2");
        return (x + y - 1) & ~(y - 1);
    }
} // namespace
