/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

// Represent fixed point numbers. dp = decimal point
using fixed8_1dp = uint8_t;
using fixed8_2dp = uint8_t;
using fixed16_1dp = int16_t;
using fixed16_2dp = int16_t;
using fixed32_1dp = int32_t;
using fixed32_2dp = int32_t;
using fixed64_1dp = int64_t;

// Construct a fixed point number. For example, to create the value 3.65 you
// would write FIXED_2DP(3,65)
#define FIXED_XDP(x, whole, fraction) ((whole) * (10 * (x)) + (fraction))
#define FIXED_1DP(whole, fraction) FIXED_XDP(1, whole, fraction)
#define FIXED_2DP(whole, fraction) FIXED_XDP(10, whole, fraction)
