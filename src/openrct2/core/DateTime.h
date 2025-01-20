/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef _MSC_VER
    #include <ctime>
#endif

#include <cstdint>

// Time (represented as number of 100-nanosecond intervals since 0001-01-01T00:00:00Z)
using datetime64 = uint64_t;

constexpr datetime64 kDatetime64Min = 0;
