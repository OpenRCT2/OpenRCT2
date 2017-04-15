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
#include <time.h>
#endif

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef int8_t sint8;
typedef int16_t sint16;
typedef int32_t sint32;
typedef int64_t sint64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#include "diagnostic.h"

typedef char utf8;
typedef utf8* utf8string;
typedef const utf8* const_utf8string;
#ifdef _WIN32
typedef wchar_t utf16;
typedef utf16* utf16string;
#endif

typedef uint32 codepoint_t;
typedef uint8 colour_t;

#define rol8(x, shift)      (((uint8)(x) << (shift)) | ((uint8)(x) >> (8 - (shift))))
#define ror8(x, shift)      (((uint8)(x) >> (shift)) | ((uint8)(x) << (8 - (shift))))
#define rol16(x, shift)     (((uint16)(x) << (shift)) | ((uint16)(x) >> (16 - (shift))))
#define ror16(x, shift)     (((uint16)(x) >> (shift)) | ((uint16)(x) << (16 - (shift))))
#define rol32(x, shift)     (((uint32)(x) << (shift)) | ((uint32)(x) >> (32 - (shift))))
#define ror32(x, shift)     (((uint32)(x) >> (shift)) | ((uint32)(x) << (32 - (shift))))
#define rol64(x, shift)     (((uint64)(x) << (shift)) | ((uint32)(x) >> (64 - (shift))))
#define ror64(x, shift)     (((uint64)(x) >> (shift)) | ((uint32)(x) << (64 - (shift))))

#ifndef __cplusplus
// in C++ you should be using Math::Min and Math::Max
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#define sgn(x)              ((x > 0) ? 1 : ((x < 0) ? -1 : 0))
#define clamp(l, x, h)      (min(h, max(l, x)))

#endif // __cplusplus

// Rounds an integer down to the given power of 2. y must be a power of 2.
#define floor2(x, y)        ((x) & (~((y) - 1)))

// Rounds an integer up to the given power of 2. y must be a power of 2.
#define ceil2(x, y)         (((x) + (y) - 1) & (~((y) - 1)))


#ifndef __cplusplus
// in C++ you should be using Util::CountOf
#ifdef __GNUC__
/**
 * Force a compilation error if condition is true, but also produce a
 * result (of value 0 and type size_t), so the expression can be used
 * e.g. in a structure initializer (or where-ever else comma expressions
 * aren't permitted).
 */
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { sint32:-!!(e); }))

/* &a[0] degrades to a pointer: a different type from an array */
#define __must_be_array(a) \
        BUILD_BUG_ON_ZERO(__builtin_types_compatible_p(typeof(a), typeof(&a[0])))

// based on http://lxr.free-electrons.com/source/include/linux/kernel.h#L54
#define countof(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))
#elif defined (_MSC_VER)
#define countof(arr)            _countof(arr)
#else
#define countof(arr)            (sizeof(arr) / sizeof((arr)[0]))
#endif // __GNUC__
#endif // __cplusplus

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

#define OPENRCT2_MASTER_SERVER_URL  "https://servers.openrct2.website"

// Time (represented as number of 100-nanosecond intervals since 0001-01-01T00:00:00Z)
typedef uint64 datetime64;

#define DATETIME64_MIN ((datetime64)0)

// Represent fixed point numbers. dp = decimal point
typedef uint8 fixed8_1dp;
typedef uint8 fixed8_2dp;
typedef sint16 fixed16_1dp;
typedef sint16 fixed16_2dp;
typedef sint32 fixed32_1dp;
typedef sint32 fixed32_2dp;
typedef sint64 fixed64_1dp;

// Money is stored as a multiple of 0.10.
typedef fixed8_1dp money8;
typedef fixed16_1dp money16;
typedef fixed32_1dp money32;
typedef fixed64_1dp money64;

// Construct a fixed point number. For example, to create the value 3.65 you
// would write FIXED_2DP(3,65)
#define FIXED_XDP(x, whole, fraction)   ((whole) * (10 * x) + (fraction))
#define FIXED_1DP(whole, fraction)      FIXED_XDP(1, whole, fraction)
#define FIXED_2DP(whole, fraction)      FIXED_XDP(10, whole, fraction)

// Construct a money value in the format MONEY(10,70) to represent 10.70. Fractional part must be two digits.
#define MONEY(whole, fraction)          ((whole) * 10 + ((fraction) / 10))

#define MONEY_FREE                      MONEY(0,00)
#define MONEY32_UNDEFINED               ((money32)0x80000000)

typedef void (EMPTY_ARGS_VOID_POINTER)();
typedef uint16 rct_string_id;

#define SafeFree(x) do { free(x); (x) = NULL; } while (0)

#define SafeDelete(x) do { delete (x); (x) = nullptr; } while (0)
#define SafeDeleteArray(x) do { delete[] (x); (x) = nullptr; } while (0)

#ifndef interface
    #define interface struct
#endif
#define abstract = 0

#if defined(__i386__) || defined(_M_IX86)
#define PLATFORM_X86
#else
#define NO_RCT2 1
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
#ifdef __cplusplus
    #ifdef _MSC_VER
        #define RESTRICT __restrict
    #else
        #define RESTRICT __restrict__
    #endif
#else
    #ifdef _MSC_VER
        #define RESTRICT __restrict
    #else
        #define RESTRICT restrict
    #endif
#endif
#ifndef RESTRICT
    #define RESTRICT
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
#if defined(__cplusplus) && __cplusplus > 201402L
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
typedef struct registers {
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
} registers;
assert_struct_size(registers, 7 * 4);
#pragma pack(pop)

#define UNUSED(x)  ((void)(x))

#endif
