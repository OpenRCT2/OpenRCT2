
/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
    [[maybe_unused]] constexpr bool IsPowerOf2(int v)
    {
        return v && ((v & (v - 1)) == 0);
    }

    // Rounds an integer down to the given power of 2. y must be a power of 2.
    [[maybe_unused]] constexpr int Floor2(const int x, const int y)
    {
        if (!IsPowerOf2(y))
            throw std::logic_error("Floor2 should only operate on power of 2");
        return x & ~(y - 1);
    }

    // Rounds an integer up to the given power of 2. y must be a power of 2.
    [[maybe_unused]] constexpr int Ceil2(const int x, const int y)
    {
        if (!IsPowerOf2(y))
            throw std::logic_error("Ceil2 should only operate on power of 2");
        return (x + y - 1) & ~(y - 1);
    }
} // namespace
