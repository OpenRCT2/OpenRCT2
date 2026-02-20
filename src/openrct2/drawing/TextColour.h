/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "PaletteIndex.h"

#include <cstddef>
#include <cstdint>

namespace OpenRCT2::Drawing
{
    enum class TextColour : uint8_t
    {
        black = 0,
        grey = 1,
        white = 2,
        red = 3,
        green = 4,
        yellow = 5,
        topaz = 6,
        celadon = 7,
        babyBlue = 8,
        paleLavender = 9,
        paleGold = 10,
        lightPink = 11,
        pearlAqua = 12,
        paleSilver = 13,
    };
    constexpr size_t kNumTextColours = 14;

    struct TextColours
    {
        PaletteIndex fill{};
        PaletteIndex sunnyOutline{};
        PaletteIndex shadowOutline{};
    };

    TextColours getTextColourMapping(TextColour textColour);
} // namespace OpenRCT2::Drawing
