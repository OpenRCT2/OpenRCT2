/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

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

// Gets the name of a symbol as a C string
#define nameof(symbol) #symbol

// Time (represented as number of 100-nanosecond intervals since 0001-01-01T00:00:00Z)
using datetime64 = uint64_t;

constexpr datetime64 DATETIME64_MIN = 0;

#define abstract = 0

#define assert_struct_size(x, y) static_assert(sizeof(x) == (y), "Improper struct size")
