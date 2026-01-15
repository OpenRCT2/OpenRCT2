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

struct ScreenRect;

namespace OpenRCT2
{
    struct ColourWithFlags;
}

namespace OpenRCT2::Drawing
{
    enum class FilterPaletteID : int32_t;
    enum class PaletteIndex : uint8_t;
    struct RenderTarget;
} // namespace OpenRCT2::Drawing

namespace OpenRCT2::Drawing::Rectangle
{
    enum class BorderStyle : uint8_t
    {
        outset,
        inset,
        none,
    };
    enum class FillBrightness : uint8_t
    {
        light,
        dark,
    };
    enum class FillMode : uint8_t
    {
        standard,
        none,
        dontLightenWhenInset,
    };

    void fill(RenderTarget& rt, const ScreenRect& rect, PaletteIndex paletteIndex, bool crossHatch = false);
    void fillInset(
        RenderTarget& rt, const ScreenRect& rect, ColourWithFlags colour, BorderStyle borderStyle = BorderStyle::outset,
        FillBrightness brightness = FillBrightness::light, FillMode fillMode = FillMode::standard);
    void filter(RenderTarget& rt, const ScreenRect& rect, FilterPaletteID palette);
} // namespace OpenRCT2::Drawing::Rectangle
