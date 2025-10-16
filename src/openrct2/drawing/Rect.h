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

void GfxFillRect(RenderTarget& rt, const ScreenRect& rect, int32_t colour);

enum class RectBorderStyle : uint8_t
{
    outset,
    inset,
    none,
};

void GfxFillRectInset(
    RenderTarget& rt, const ScreenRect& rect, ColourWithFlags colour, RectBorderStyle borderStyle = RectBorderStyle::outset,
    uint8_t flags = 0);
void GfxFilterRect(RenderTarget& rt, const ScreenRect& rect, FilterPaletteID palette);
