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

struct ColourWithFlags;
enum class FilterPaletteID : int32_t;
struct RenderTarget;
struct ScreenRect;

namespace OpenRCT2::Drawing::Rect
{
    enum class BorderStyle : uint8_t
    {
        outset,
        inset,
        none,
    };

    void fill(RenderTarget& rt, const ScreenRect& rect, int32_t colour);
    void fillInset(
        RenderTarget& rt, const ScreenRect& rect, ColourWithFlags colour, BorderStyle borderStyle = BorderStyle::outset,
        uint8_t flags = 0);
    void filter(RenderTarget& rt, const ScreenRect& rect, FilterPaletteID palette);
} // namespace OpenRCT2::Drawing::Rect
