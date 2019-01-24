/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef _USE_MATH_DEFINES
#    define _USE_MATH_DEFINES
#endif
#undef M_PI

#ifdef _MSC_VER
#    include <ctime>
#endif

#include "Diagnostic.h"

#include <cassert>
#include <cstdint>
#include <limits>

using utf8 = char;
using utf8string = utf8*;
using const_utf8string = const utf8*;
#ifdef _WIN32
using utf16 = wchar_t;
using utf16string = utf16*;
#endif

// Define MAX_PATH for various headers that don't want to include system headers
// just for MAX_PATH
#ifndef MAX_PATH
#    define MAX_PATH 260
#endif

using codepoint_t = uint32_t;
using colour_t = uint8_t;

template<typename _UIntType>
constexpr _UIntType rol(_UIntType x, size_t shift){
    static_assert(std::is_unsigned<_UIntType>::value,
                  "result_type must be an unsigned integral type");
    using limits = typename std::numeric_limits<_UIntType>;
    return (((_UIntType)(x) << shift) | ((_UIntType)(x) >> (limits::digits - shift)));
}

template<typename _UIntType>
constexpr _UIntType ror(_UIntType x, size_t shift){
    static_assert(std::is_unsigned<_UIntType>::value,
                  "result_type must be an unsigned integral type");
    using limits = std::numeric_limits<_UIntType>;
    return (((_UIntType)(x) >> shift) | ((_UIntType)(x) << (limits::digits - shift)));
}

const constexpr auto rol8 = rol<uint8_t>;
const constexpr auto ror8 = ror<uint8_t>;
const constexpr auto rol16 = rol<uint16_t>;
const constexpr auto ror16 = ror<uint16_t>;
const constexpr auto rol32 = rol<uint32_t>;
const constexpr auto ror32 = ror<uint32_t>;
const constexpr auto rol64 = rol<uint64_t>;
const constexpr auto ror64 = ror<uint64_t>;

// Rounds an integer down to the given power of 2. y must be a power of 2.
#define floor2(x, y) ((x) & (~((y)-1)))

// Rounds an integer up to the given power of 2. y must be a power of 2.
#define ceil2(x, y) (((x) + (y)-1) & (~((y)-1)))

// Gets the name of a symbol as a C string
#define nameof(symbol) #symbol

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#    include <unistd.h>
#    define STUB() log_warning("Function %s at %s:%d is a stub.\n", __PRETTY_FUNCTION__, __FILE__, __LINE__)
#    define _strcmpi _stricmp
#    define _stricmp(x, y) strcasecmp((x), (y))
#    define _strnicmp(x, y, n) strncasecmp((x), (y), (n))
#    define _strdup(x) strdup((x))

#    if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#        define RCT2_ENDIANESS __ORDER_LITTLE_ENDIAN__
#        define LOBYTE(w) ((uint8_t)(w))
#        define HIBYTE(w) ((uint8_t)(((uint16_t)(w) >> 8) & 0xFF))
#    endif // __BYTE_ORDER__

#    ifndef RCT2_ENDIANESS
#        error Unknown endianess!
#    endif // RCT2_ENDIANESS

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

#define DATETIME64_MIN ((datetime64)0)

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

// Construct a money value in the format MONEY(10,70) to represent 10.70. Fractional part must be two digits.
#define MONEY(whole, fraction) ((whole)*10 + ((fraction) / 10))

#define MONEY_FREE MONEY(0, 00)
#define MONEY16_UNDEFINED (money16)(uint16_t) 0xFFFF
#define MONEY32_UNDEFINED ((money32)0x80000000)

using BannerIndex = uint8_t;

using EMPTY_ARGS_VOID_POINTER = void();
using rct_string_id = uint16_t;

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

#ifndef interface
#    define interface struct
#endif
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

/**
 * x86 register structure, only used for easy interop to RCT2 code.
 */
#pragma pack(push, 1)
struct registers
{
    union
    {
        int32_t eax;
        int16_t ax;
        struct
        {
            char al;
            char ah;
        };
    };
    union
    {
        int32_t ebx;
        int16_t bx;
        struct
        {
            char bl;
            char bh;
        };
    };
    union
    {
        int32_t ecx;
        int16_t cx;
        struct
        {
            char cl;
            char ch;
        };
    };
    union
    {
        int32_t edx;
        int16_t dx;
        struct
        {
            char dl;
            char dh;
        };
    };
    union
    {
        int32_t esi;
        int16_t si;
    };
    union
    {
        int32_t edi;
        int16_t di;
    };
    union
    {
        int32_t ebp;
        int16_t bp;
    };
};
assert_struct_size(registers, 7 * 4);
#pragma pack(pop)

#endif
