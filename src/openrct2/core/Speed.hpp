/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

// Note: Only valid for 5 decimal places.
constexpr int32_t operator""_mph(long double speedMph)
{
    uint32_t wholeNumber = speedMph;
    uint64_t fraction = (speedMph - wholeNumber) * 100000;
    return wholeNumber << 16 | ((fraction << 16) / 100000);
}
