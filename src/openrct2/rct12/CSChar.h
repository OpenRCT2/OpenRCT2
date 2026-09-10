/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace OpenRCT2
{
    enum class CSChar : uint8_t
    {
        quoteClose = 0x22,

        aOgonekUc = 0x9F,
        up = 0xA0,

        cAcuteUc = 0xA2,
        eOgonekUc = 0xA6,
        lStrokeUc = 0xA7,

        down = 0xAA,
        tick = 0xAC,
        cross = 0xAD,
        right = 0xAF,
        railway = 0xB1,
        quoteOpen = 0xB4,
        euro = 0xB5,
        road = 0xB6,
        air = 0xB7,
        water = 0xB8,
        superscriptMinusOne = 0xB9,
        bullet = 0xBA,
        smallUp = 0xBC,
        smallDown = 0xBD,
        left = 0xBE,

        nAcuteUc = 0xC6,
        sAcuteUc = 0xD0,
        zAcuteUc = 0xD7,
        zDotUc = 0xD8,
        aOgonek = 0xDD,
        cAcute = 0xDE,
        eOgonek = 0xE6,
        nAcute = 0xF0,
        oCircumflex = 0xF4,
        lStroke = 0xF7,
        sAcute = 0xF8,
        zAcute = 0xFE,

        zDot = 0xFD,
    };
}
