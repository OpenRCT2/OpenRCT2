/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "ColourPalette.h"

#include <cstdint>

namespace OpenRCT2::Drawing
{
    enum class PaletteIndex : uint8_t;
    using BlendColourMapType = std::array<std::array<PaletteIndex, kGamePaletteSize>, kGamePaletteSize>;

    BlendColourMapType* GetBlendColourMap();

#ifndef DISABLE_TTF
    PaletteIndex BlendColours(PaletteIndex paletteIndex1, PaletteIndex paletteIndex2);
#endif
} // namespace OpenRCT2::Drawing
