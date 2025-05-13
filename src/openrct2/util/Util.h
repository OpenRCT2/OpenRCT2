/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"

#include <limits>
#include <type_traits>

uint32_t UtilRand();
float UtilRandNormalDistributed();

template<typename T1, typename T2 = T1>
constexpr T1 AddClamp(T1 value, T2 valueToAdd)
{
    static_assert(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>);
    if (std::is_same_v<T1, money64>)
    {
        static_assert(sizeof(money64) == sizeof(int64_t));
    }

    using CommonT = decltype(value + valueToAdd);
    auto maxCap = static_cast<CommonT>(std::numeric_limits<T1>::max());
    auto minCap = static_cast<CommonT>(std::numeric_limits<T1>::lowest());

    CommonT result;
    if ((valueToAdd > 0) && (static_cast<CommonT>(value) > (maxCap - valueToAdd)))
    {
        result = maxCap;
    }
    else if ((valueToAdd < 0) && (static_cast<CommonT>(value) < (minCap - valueToAdd)))
    {
        result = minCap;
    }
    else
    {
        result = static_cast<CommonT>(value) + valueToAdd;
    }

    return static_cast<T1>(result);
}

template<typename T1, typename T2 = T1>
constexpr void AddClamp(T1* value, T2 valueToAdd)
{
    static_assert(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>);
    if (std::is_same_v<T1, money64>)
    {
        static_assert(sizeof(money64) == sizeof(int64_t));
    }

    using CommonT = decltype(*value + valueToAdd);
    auto maxCap = static_cast<CommonT>(std::numeric_limits<T1>::max());
    auto minCap = static_cast<CommonT>(std::numeric_limits<T1>::lowest());

    CommonT v = static_cast<CommonT>(*value);
    CommonT result;
    if ((valueToAdd > 0) && (v > (maxCap - valueToAdd)))
    {
        result = maxCap;
    }
    else if ((valueToAdd < 0) && (v < (minCap - valueToAdd)))
    {
        result = minCap;
    }
    else
    {
        result = v + valueToAdd;
    }

    *value = static_cast<T1>(result);
}

uint8_t Lerp(uint8_t a, uint8_t b, float t);
float FLerp(float a, float b, float t);
uint8_t SoftLight(uint8_t a, uint8_t b);

constexpr uint8_t HiByte(uint16_t value)
{
    return static_cast<uint8_t>(value >> 8);
}

constexpr uint8_t LoByte(uint16_t value)
{
    return static_cast<uint8_t>(value & 0xFFu);
}
