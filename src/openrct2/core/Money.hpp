/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "FixedPoint.hpp"

// Money is stored as a multiple of 0.10.
using money16 = fixed16_1dp;
using money32 = fixed32_1dp;
using money64 = fixed64_1dp;

// For a user defined floating point literal, the parameter type must be a
// `long double` which is problematic on ppc64el, as the architecture uses a
// pair of `doubles` to represent that type. This cannot be converted to a
// `constexpr`. As a workaround, statically cast the `long double` down to a
// `double`. All of the uses of _GBP constants fit just fine, and if anyone
// really tries to use a gigantic constant that can't fit in a double, they are
// probably going to be breaking other things anyways.
// For more details, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=26374
constexpr money64 operator""_GBP(long double money) noexcept
{
    return static_cast<double>(money) * 10;
}

constexpr money64 ToMoney64FromGBP(int32_t money) noexcept
{
    return money * 10;
}

constexpr money64 ToMoney64FromGBP(int64_t money) noexcept
{
    return money * 10;
}

constexpr money64 ToMoney64FromGBP(double money) noexcept
{
    return money * 10;
}

constexpr money16 kMoney16Undefined = static_cast<money16>(static_cast<uint16_t>(0xFFFF));
constexpr money32 kMoney32Undefined = static_cast<money32>(0x80000000);
constexpr money64 kMoney64Undefined = static_cast<money64>(0x8000000000000000);

constexpr money16 ToMoney16(money64 value)
{
    return value == kMoney64Undefined ? kMoney16Undefined : value;
}

constexpr money64 ToMoney64(money32 value)
{
    return value == kMoney32Undefined ? kMoney64Undefined : value;
}

constexpr money64 ToMoney64(money16 value)
{
    return value == kMoney16Undefined ? kMoney64Undefined : value;
}
