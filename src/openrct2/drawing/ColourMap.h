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

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
    enum class PaletteIndex : uint8_t;

    struct ColourShadeMap
    {
        PaletteIndex colour0;
        PaletteIndex colour1;
        PaletteIndex darkest;
        PaletteIndex darker;
        PaletteIndex dark;
        PaletteIndex midDark;
        PaletteIndex midLight;
        PaletteIndex light;
        PaletteIndex lighter;
        PaletteIndex lightest;
        PaletteIndex colour10;
        PaletteIndex colour11;
    };

    ColourShadeMap getColourMap(Colour colour);

    void initColourMaps();
} // namespace OpenRCT2::Drawing
