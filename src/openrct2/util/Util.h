/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"

#include <cstdio>
#include <ctime>
#include <optional>
#include <type_traits>
#include <vector>

namespace OpenRCT2
{
    int32_t SquaredMetresToSquaredFeet(int32_t squaredMetres);
    int32_t MetresToFeet(int32_t metres);
    int32_t MphToKmph(int32_t mph);
    int32_t MphToDmps(int32_t mph);

    bool SSE41Available();
    bool AVX2Available();

    int32_t UtilBitScanForward(int32_t source);
    int32_t UtilBitScanForward(int64_t source);
    void BitCountInit();
    int32_t BitCount(uint32_t source);
    int32_t StrLogicalCmp(char const* a, char const* b);
    char* SafeStrCpy(char* destination, const char* source, size_t num);
    char* SafeStrCat(char* destination, const char* source, size_t size);

    uint32_t UtilRand();
    float UtilRandNormalDistributed();

    bool UtilGzipCompress(FILE* source, FILE* dest);
    std::vector<uint8_t> Gzip(const void* data, const size_t dataLen);
    std::vector<uint8_t> Ungzip(const void* data, const size_t dataLen);

    // TODO: Make these specialized template functions, or when possible Concepts in C++20
    int8_t AddClamp_int8_t(int8_t value, int8_t value_to_add);
    int16_t AddClamp_int16_t(int16_t value, int16_t value_to_add);
    int32_t AddClamp_int32_t(int32_t value, int32_t value_to_add);
    int64_t AddClamp_int64_t(int64_t value, int64_t value_to_add);
    money16 AddClamp_money16(money16 value, money16 value_to_add);
    money32 AddClamp_money32(money32 value, money32 value_to_add);
    money64 AddClamp_money64(money64 value, money64 value_to_add);

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
} // namespace OpenRCT2
