/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/CallingConventions.h"
#include "../core/Money.hpp"
#include "../core/StringTypes.h"

#include <cstdio>
#include <ctime>
#include <optional>
#include <type_traits>
#include <vector>

#ifdef _MSC_VER
#    include <intrin.h>
#endif

int32_t SquaredMetresToSquaredFeet(int32_t squaredMetres);
int32_t MetresToFeet(int32_t metres);
int32_t FeetToMetres(int32_t feet);
int32_t MphToKmph(int32_t mph);
int32_t MphToDmps(int32_t mph);
int32_t BaseZToMetres(int16_t baseZ);
uint8_t MetresToBaseZ(int16_t metres);
int32_t HeightUnitsToMetres(int32_t heightUnit);
int32_t ToHumanReadableSpeed(int32_t baseSpeed);

inline int32_t UtilBitScanForward(uint32_t source)
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400) // Visual Studio 2005
    unsigned long i;
    uint8_t success = _BitScanForward(&i, source);
    return success != 0 ? i : -1;
#elif defined(__GNUC__)
    int32_t success = __builtin_ffs(source);
    return success - 1;
#else
#    pragma message("Falling back to iterative bitscan forward, consider using intrinsics")
    // This is a low-hanging optimisation boost, check if your compiler offers
    // any intrinsic.
    // cf. https://github.com/OpenRCT2/OpenRCT2/pull/2093
    for (int32_t i = 0; i < 32; i++)
        if (source & (1u << i))
            return i;

    return -1;
#endif
}

inline int32_t UtilBitScanForward(uint64_t source)
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400) && defined(_M_X64) // Visual Studio 2005
    unsigned long i;
    uint8_t success = _BitScanForward64(&i, source);
    return success != 0 ? i : -1;
#elif defined(__GNUC__)
    int32_t success = __builtin_ffsll(source);
    return success - 1;
#else
#    pragma message("Falling back to iterative bitscan forward, consider using intrinsics")
    // This is a low-hanging optimisation boost, check if your compiler offers
    // any intrinsic.
    // cf. https://github.com/OpenRCT2/OpenRCT2/pull/2093
    for (int32_t i = 0; i < 64; i++)
        if (source & (1uLL << i))
            return i;

    return -1;
#endif
}

int32_t StrLogicalCmp(char const* a, char const* b);
char* SafeStrCpy(char* destination, const char* source, size_t num);
char* SafeStrCat(char* destination, const char* source, size_t size);

uint32_t UtilRand();
float UtilRandNormalDistributed();

bool UtilGzipCompress(FILE* source, FILE* dest);
std::vector<uint8_t> Gzip(const void* data, const size_t dataLen);
std::vector<uint8_t> Ungzip(const void* data, const size_t dataLen);

template<typename T> constexpr T AddClamp(T value, T valueToAdd)
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

size_t StrCatFTime(char* buffer, size_t bufferSize, const char* format, const struct tm* tp);

template<typename T> [[nodiscard]] constexpr uint64_t EnumToFlag(T v)
{
    static_assert(std::is_enum_v<T>);
    return 1uLL << static_cast<std::underlying_type_t<T>>(v);
}

template<typename... T> [[nodiscard]] constexpr uint64_t EnumsToFlags(T... types)
{
    return (EnumToFlag(types) | ...);
}

template<typename TEnum> constexpr auto EnumValue(TEnum enumerator) noexcept
{
    return static_cast<std::underlying_type_t<TEnum>>(enumerator);
}

template<typename T> constexpr bool HasFlag(uint64_t holder, T v)
{
    static_assert(std::is_enum_v<T>);
    return (holder & EnumToFlag(v)) != 0;
}

constexpr uint8_t HiByte(uint16_t value)
{
    return static_cast<uint8_t>(value >> 8);
}

constexpr uint8_t LoByte(uint16_t value)
{
    return static_cast<uint8_t>(value & 0xFFu);
}
