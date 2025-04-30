/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

// Represent fixed point numbers. dp = decimal point
using fixed8_1dp = uint8_t;
using fixed8_2dp = uint8_t;
using fixed16_1dp = int16_t;
using fixed16_2dp = int16_t;
using fixed32_1dp = int32_t;
using fixed32_2dp = int32_t;
using fixed64_1dp = int64_t;

// Construct a fixed point number. For example, to create the value 3.65 you
// would write MakeFixed2dp(3, 65)
template<typename T, uint8_t factor>
static constexpr T MakeFixedXdp(T whole, uint8_t fraction)
{
    return (whole * factor) + fraction;
}

template<typename T>
constexpr T MakeFixed1dp(T whole, uint8_t fraction)
{
    return MakeFixedXdp<T, 10>(whole, fraction);
}

template<typename T>
constexpr T MakeFixed2dp(T whole, uint8_t fraction)
{
    return MakeFixedXdp<T, 100>(whole, fraction);
}

constexpr fixed16_2dp MakeFixed16_2dp(int16_t whole, uint8_t fraction)
{
    return MakeFixed2dp<fixed16_2dp>(whole, fraction);
}
