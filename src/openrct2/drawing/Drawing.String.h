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

enum class FontStyle : uint8_t;

struct TextDrawInfo
{
    int32_t startX;
    int32_t startY;
    int32_t x;
    int32_t y;
    int32_t maxX;
    int32_t maxY;
    int32_t flags;
    uint8_t palette[8];
    FontStyle fontStyle;
    const int8_t* yOffset;
};
