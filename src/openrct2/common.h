/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#undef M_PI

// Ignore isatty warning on WIN32
#ifndef _CRT_NONSTDC_NO_WARNINGS
#    define _CRT_NONSTDC_NO_WARNINGS
#endif

#ifdef _MSC_VER
#    include <ctime>
#endif

#include "Diagnostic.h"
#include "localisation/StringIdType.h"

#include <cassert>
#include <cstddef>
#include <cstdint>

using colour_t = uint8_t;

// Gets the name of a symbol as a C string
#define nameof(symbol) #symbol

#define OPENRCT2_MASTER_SERVER_URL "https://servers.openrct2.io"

// Time (represented as number of 100-nanosecond intervals since 0001-01-01T00:00:00Z)
using datetime64 = uint64_t;

constexpr datetime64 DATETIME64_MIN = 0;

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
