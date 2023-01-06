/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifndef _USE_MATH_DEFINES
#    define _USE_MATH_DEFINES
#endif
#undef M_PI

#ifdef _MSC_VER
#    include <ctime>
#endif

#include "Diagnostic.h"

#include <cassert>
#include <cstddef>
#include <cstdint>

// Define MAX_PATH for various headers that don't want to include system headers
// just for MAX_PATH
#ifndef MAX_PATH
#    define MAX_PATH 260
#endif

using colour_t = uint8_t;

// Gets the name of a symbol as a C string
#define nameof(symbol) #symbol

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#    include <unistd.h>
#    define STUB() log_warning("Function %s at %s:%d is a stub.", __PRETTY_FUNCTION__, __FILE__, __LINE__)
#    define _strcmpi _stricmp
#    define _stricmp(x, y) strcasecmp((x), (y))
#    define _strnicmp(x, y, n) strncasecmp((x), (y), (n))
#    define _strdup(x) strdup((x))

#    if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#        define RCT2_ENDIANNESS __ORDER_LITTLE_ENDIAN__
#        define LOBYTE(w) (static_cast<uint8_t>(w))
#        define HIBYTE(w) (static_cast<uint8_t>((static_cast<uint16_t>(w) >> 8) & 0xFF))
#    endif // __BYTE_ORDER__

#    ifndef RCT2_ENDIANNESS
#        error Unknown endianness!
#    endif // RCT2_ENDIANNESS

#endif // defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

#ifdef _WIN32
char* strndup(const char* src, size_t size);
#endif

// BSD and macOS have MAP_ANON instead of MAP_ANONYMOUS
#ifndef MAP_ANONYMOUS
#    define MAP_ANONYMOUS MAP_ANON
#endif

#define OPENRCT2_MASTER_SERVER_URL "https://servers.openrct2.io"

// Time (represented as number of 100-nanosecond intervals since 0001-01-01T00:00:00Z)
using datetime64 = uint64_t;

constexpr const datetime64 DATETIME64_MIN = 0;

// Represent fixed point numbers. dp = decimal point
using fixed8_1dp = uint8_t;
using fixed8_2dp = uint8_t;
using fixed16_1dp = int16_t;
using fixed16_2dp = int16_t;
using fixed32_1dp = int32_t;
using fixed32_2dp = int32_t;
using fixed64_1dp = int64_t;

// Money is stored as a multiple of 0.10.
using money8 = fixed8_1dp;
using money16 = fixed16_1dp;
using money32 = fixed32_1dp;
using money64 = fixed64_1dp;

// Construct a fixed point number. For example, to create the value 3.65 you
// would write FIXED_2DP(3,65)
#define FIXED_XDP(x, whole, fraction) ((whole) * (10 * (x)) + (fraction))
#define FIXED_1DP(whole, fraction) FIXED_XDP(1, whole, fraction)
#define FIXED_2DP(whole, fraction) FIXED_XDP(10, whole, fraction)

constexpr money64 operator"" _GBP(long double money) noexcept
{
    return money * 10;
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

#define MONEY16_UNDEFINED static_cast<money16>(static_cast<uint16_t>(0xFFFF))
#define MONEY32_UNDEFINED (static_cast<money32>(0x80000000))
#define MONEY64_UNDEFINED (static_cast<money64>(0x8000000000000000))

constexpr money64 ToMoney64(money32 value)
{
    return value == MONEY32_UNDEFINED ? MONEY64_UNDEFINED : value;
}

constexpr money64 ToMoney64(money16 value)
{
    return value == MONEY16_UNDEFINED ? MONEY64_UNDEFINED : value;
}

using StringId = uint16_t;

#define SafeFree(x)                                                                                                            \
    do                                                                                                                         \
    {                                                                                                                          \
        free(x);                                                                                                               \
        (x) = nullptr;                                                                                                         \
    } while (false)

#define SafeDelete(x)                                                                                                          \
    do                                                                                                                         \
    {                                                                                                                          \
        delete (x);                                                                                                            \
        (x) = nullptr;                                                                                                         \
    } while (false)
#define SafeDeleteArray(x)                                                                                                     \
    do                                                                                                                         \
    {                                                                                                                          \
        delete[](x);                                                                                                           \
        (x) = nullptr;                                                                                                         \
    } while (false)

#define abstract = 0

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#    define OPENRCT2_X86
#elif defined(_MSC_VER) && (_MSC_VER >= 1500) && (defined(_M_X64) || defined(_M_IX86)) // VS2008
#    define OPENRCT2_X86
#endif

#if defined(__i386__) || defined(_M_IX86)
#    define PLATFORM_X86
#endif

#if defined(__LP64__) || defined(_WIN64)
#    define PLATFORM_64BIT
#else
#    define PLATFORM_32BIT
#endif

// C99's restrict keywords guarantees the pointer in question, for the whole of its lifetime,
// will be the only way to access a given memory region. In other words: there is no other pointer
// aliasing the same memory area. Using it lets compiler generate better code. If your compiler
// does not support it, feel free to drop it, at some performance hit.
#ifdef _MSC_VER
#    define RESTRICT __restrict
#else
#    define RESTRICT __restrict__
#endif

#define assert_struct_size(x, y) static_assert(sizeof(x) == (y), "Improper struct size")

#ifdef PLATFORM_X86
#    ifndef FASTCALL
#        ifdef __GNUC__
#            define FASTCALL __attribute__((fastcall))
#        elif defined(_MSC_VER)
#            define FASTCALL __fastcall
#        else
#            pragma message "Not using fastcall calling convention, please check your compiler support"
#            define FASTCALL
#        endif
#    endif // FASTCALL
#else      // PLATFORM_X86
#    define FASTCALL
#endif // PLATFORM_X86
