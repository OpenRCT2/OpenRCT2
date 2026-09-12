/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "ImageIndexType.h"
#include "PaletteType.h"

#include <cstdint>
#include <span>

namespace OpenRCT2::Drawing
{
    enum class PaletteIndex : uint8_t;

    extern GamePalette gPalette;
    extern GamePalette gGamePalette;
    extern uint32_t gPaletteEffectFrame;

    void UpdatePalette(std::span<const BGRAColour> palette, PaletteIndex startIndex, int32_t numColours);
    void UpdatePaletteEffects();
    void GfxTransposePalette(ImageIndex pal, uint8_t product);
    void LoadPalette();
} // namespace OpenRCT2::Drawing
