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

template<typename T>
constexpr T AddClamp(T value, T valueToAdd)
{
    if (std::is_same_v<decltype(value), money64>)
    {
        static_assert(sizeof(money64) == sizeof(int64_t));
    }
    auto maxCap = std::numeric_limits<T>::max();
    auto minCap = std::numeric_limits<T>::lowest();
    if ((valueToAdd > 0) && (value > (maxCap - (valueToAdd))))
    {
        return maxCap;
    }
    else if ((valueToAdd < 0) && (value < (minCap - (valueToAdd))))
    {
        return minCap;
    }
    else
    {
        return value + valueToAdd;
    }
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
