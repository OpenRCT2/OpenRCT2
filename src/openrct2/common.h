#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#undef M_PI

#ifdef _MSC_VER
#include <ctime>
#endif

#include <cassert>
#include <cstdint>

using sint8 = int8_t;
using sint16 = int16_t;
using sint32 = int32_t;
using sint64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

#include "Diagnostic.h"

using utf8             = char;
using utf8string       = utf8 *;
using const_utf8string = const utf8 *;
#ifdef _WIN32
using utf16 = wchar_t;
using utf16string = utf16*;
#endif

// Define MAX_PATH for various headers that don't want to include system headers
// just for MAX_PATH
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

using codepoint_t = uint32;
using colour_t = uint8;

#define rol8(x, shift)      (((uint8)(x) << (shift)) | ((uint8)(x) >> (8 - (shift))))
#define ror8(x, shift)      (((uint8)(x) >> (shift)) | ((uint8)(x) << (8 - (shift))))
#define rol16(x, shift)     (((uint16)(x) << (shift)) | ((uint16)(x) >> (16 - (shift))))
#define ror16(x, shift)     (((uint16)(x) >> (shift)) | ((uint16)(x) << (16 - (shift))))
#define rol32(x, shift)     (((uint32)(x) << (shift)) | ((uint32)(x) >> (32 - (shift))))
#define ror32(x, shift)     (((uint32)(x) >> (shift)) | ((uint32)(x) << (32 - (shift))))
#define rol64(x, shift)     (((uint64)(x) << (shift)) | ((uint32)(x) >> (64 - (shift))))
#define ror64(x, shift)     (((uint64)(x) >> (shift)) | ((uint32)(x) << (64 - (shift))))

// Rounds an integer down to the given power of 2. y must be a power of 2.
#define floor2(x, y)        ((x) & (~((y) - 1)))

// Rounds an integer up to the given power of 2. y must be a power of 2.
#define ceil2(x, y)         (((x) + (y) - 1) & (~((y) - 1)))

// Gets the name of a symbol as a C string
#define nameof(symbol) #symbol

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#define STUB() log_warning("Function %s at %s:%d is a stub.\n", __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define _strcmpi _stricmp
#define _stricmp(x, y) strcasecmp((x), (y))
#define _strnicmp(x, y, n) strncasecmp((x), (y), (n))
#define _strdup(x) strdup((x))

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define RCT2_ENDIANESS __ORDER_LITTLE_ENDIAN__
#define LOBYTE(w) ((uint8)(w))
#define HIBYTE(w) ((uint8)(((uint16)(w)>>8)&0xFF))
#endif // __BYTE_ORDER__

#ifndef RCT2_ENDIANESS
#error Unknown endianess!
#endif // RCT2_ENDIANESS

#endif // defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

#if !((defined (_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200809L) || (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 700))
char *strndup(const char *src, size_t size);
#endif // !(POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)

// BSD and macOS have MAP_ANON instead of MAP_ANONYMOUS
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#define OPENRCT2_MASTER_SERVER_URL  "https://servers.openrct2.io"

// Time (represented as number of 100-nanosecond intervals since 0001-01-01T00:00:00Z)
using datetime64 = uint64;

#define DATETIME64_MIN ((datetime64)0)

// Represent fixed point numbers. dp = decimal point
using fixed8_1dp = uint8;
using fixed8_2dp = uint8;
using fixed16_1dp = sint16;
using fixed16_2dp = sint16;
using fixed32_1dp = sint32;
using fixed32_2dp = sint32;
using fixed64_1dp = sint64;

// Money is stored as a multiple of 0.10.
using money8 = fixed8_1dp;
using money16 = fixed16_1dp;
using money32 = fixed32_1dp;
using money64 = fixed64_1dp;

// Construct a fixed point number. For example, to create the value 3.65 you
// would write FIXED_2DP(3,65)
#define FIXED_XDP(x, whole, fraction)   ((whole) * (10 * x) + (fraction))
#define FIXED_1DP(whole, fraction)      FIXED_XDP(1, whole, fraction)
#define FIXED_2DP(whole, fraction)      FIXED_XDP(10, whole, fraction)

// Construct a money value in the format MONEY(10,70) to represent 10.70. Fractional part must be two digits.
#define MONEY(whole, fraction)          ((whole) * 10 + ((fraction) / 10))

#define MONEY_FREE                      MONEY(0,00)
#define MONEY16_UNDEFINED               (money16)(uint16)0xFFFF
#define MONEY32_UNDEFINED               ((money32)0x80000000)

using EMPTY_ARGS_VOID_POINTER = void();
using rct_string_id           = uint16;

#define SafeFree(x) do { free(x); (x) = nullptr; } while (false)

#define SafeDelete(x) do { delete (x); (x) = nullptr; } while (false)
#define SafeDeleteArray(x) do { delete[] (x); (x) = nullptr; } while (false)

#ifndef interface
    #define interface struct
#endif
#define abstract = 0

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    #define OPENRCT2_X86
#elif defined(_MSC_VER) && (_MSC_VER >= 1500) && (defined(_M_X64) || defined(_M_IX86)) // VS2008
    #define OPENRCT2_X86
#endif

#if defined(__i386__) || defined(_M_IX86)
#define PLATFORM_X86
#endif

#if defined(__LP64__) || defined(_WIN64)
    #define PLATFORM_64BIT
#else
    #define PLATFORM_32BIT
#endif

// C99's restrict keywords guarantees the pointer in question, for the whole of its lifetime,
// will be the only way to access a given memory region. In other words: there is no other pointer
// aliasing the same memory area. Using it lets compiler generate better code. If your compiler
// does not support it, feel free to drop it, at some performance hit.
#ifdef _MSC_VER
    #define RESTRICT __restrict
#else
    #define RESTRICT __restrict__
#endif

#define assert_struct_size(x, y) static_assert(sizeof(x) == (y), "Improper struct size")

#ifdef PLATFORM_X86
    #ifndef FASTCALL
        #ifdef __GNUC__
            #define FASTCALL __attribute__((fastcall))
        #elif defined(_MSC_VER)
            #define FASTCALL __fastcall
        #else
            #pragma message "Not using fastcall calling convention, please check your compiler support"
            #define FASTCALL
        #endif
    #endif // FASTCALL
#else // PLATFORM_X86
    #define FASTCALL
#endif // PLATFORM_X86

// C++17 or later
#if __cplusplus > 201402L
    #define UNUSED_ATTR [[maybe_unused]]
#else
    #ifdef __GNUC__
        #define UNUSED_ATTR [[gnu::unused]]
    #else
        #define UNUSED_ATTR
    #endif
#endif

/**
 * x86 register structure, only used for easy interop to RCT2 code.
 */
#pragma pack(push, 1)
struct registers {
    union {
        sint32 eax;
        sint16 ax;
        struct {
            char al;
            char ah;
        };
    };
    union {
        sint32 ebx;
        sint16 bx;
        struct {
            char bl;
            char bh;
        };
    };
    union {
        sint32 ecx;
        sint16 cx;
        struct {
            char cl;
            char ch;
        };
    };
    union {
        sint32 edx;
        sint16 dx;
        struct {
            char dl;
            char dh;
        };
    };
    union {
        sint32 esi;
        sint16 si;
    };
    union {
        sint32 edi;
        sint16 di;
    };
    union {
        sint32 ebp;
        sint16 bp;
    };
};
assert_struct_size(registers, 7 * 4);
#pragma pack(pop)

#define UNUSED(x)  ((void)(x))

#endif
