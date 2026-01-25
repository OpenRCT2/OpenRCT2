/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ColourMap.h"

#include "../SpriteIds.h"
#include "../core/Guard.hpp"

// FIXME: only included for GfxGetG1Element()
#include "Drawing.h"

namespace OpenRCT2::Drawing
{
    constexpr uint8_t kIndexColour0 = 243;
    constexpr uint8_t kIndexColour1 = 245;
    constexpr uint8_t kIndexDarkest = 245;
    constexpr uint8_t kIndexDarker = 246;
    constexpr uint8_t kIndexDark = 247;
    constexpr uint8_t kIndexMidDark = 248;
    constexpr uint8_t kIndexMidLight = 249;
    constexpr uint8_t kIndexLight = 250;
    constexpr uint8_t kIndexLighter = 251;
    constexpr uint8_t kIndexLightest = 252;
    constexpr uint8_t kIndexColour10 = 253;
    constexpr uint8_t kIndexColour11 = 254;

    static std::array<ColourShadeMap, kColourNumTotal> _colourMap = {};

    void initColourMaps()
    {
        for (int32_t i = 0; i < kColourNumTotal; i++)
        {
            const auto* g1 = GfxGetG1Element(SPR_PALETTE_2_START + i);
            if (g1 != nullptr)
            {
                _colourMap[i].colour0 = static_cast<PaletteIndex>(g1->offset[kIndexColour0]);
                _colourMap[i].colour1 = static_cast<PaletteIndex>(g1->offset[kIndexColour1]);
                _colourMap[i].darkest = static_cast<PaletteIndex>(g1->offset[kIndexDarkest]);
                _colourMap[i].darker = static_cast<PaletteIndex>(g1->offset[kIndexDarker]);
                _colourMap[i].dark = static_cast<PaletteIndex>(g1->offset[kIndexDark]);
                _colourMap[i].midDark = static_cast<PaletteIndex>(g1->offset[kIndexMidDark]);
                _colourMap[i].midLight = static_cast<PaletteIndex>(g1->offset[kIndexMidLight]);
                _colourMap[i].light = static_cast<PaletteIndex>(g1->offset[kIndexLight]);
                _colourMap[i].lighter = static_cast<PaletteIndex>(g1->offset[kIndexLighter]);
                _colourMap[i].lightest = static_cast<PaletteIndex>(g1->offset[kIndexLightest]);
                _colourMap[i].colour10 = static_cast<PaletteIndex>(g1->offset[kIndexColour10]);
                _colourMap[i].colour11 = static_cast<PaletteIndex>(g1->offset[kIndexColour11]);
            }
        }
    }

    ColourShadeMap getColourMap(Colour colour)
    {
        Guard::Assert(EnumValue(colour) < _colourMap.size());
        return _colourMap[EnumValue(colour)];
    }
} // namespace OpenRCT2::Drawing
