/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Location.hpp"
#include "FilterPaletteIds.h"
#include "PaletteIndex.h"
#include "TTF.h"

struct PaletteMap;
struct TextDrawInfo;

namespace OpenRCT2::Drawing
{
    struct IDrawingEngine;
    struct RenderTarget;

    struct IDrawingContext
    {
        virtual ~IDrawingContext() = default;

        virtual void Clear(RenderTarget& rt, PaletteIndex paletteIndex) = 0;
        virtual void FillRect(
            RenderTarget& rt, PaletteIndex paletteIndex, int32_t left, int32_t top, int32_t right, int32_t bottom,
            bool crossHatch = false)
            = 0;
        virtual void FilterRect(
            RenderTarget& rt, FilterPaletteID palette, int32_t left, int32_t top, int32_t right, int32_t bottom)
            = 0;
        virtual void DrawLine(RenderTarget& rt, PaletteIndex colour, const ScreenLine& line) = 0;
        virtual void DrawSprite(RenderTarget& rt, ImageId image, int32_t x, int32_t y) = 0;
        virtual void DrawSpriteRawMasked(RenderTarget& rt, int32_t x, int32_t y, ImageId maskImage, ImageId colourImage) = 0;
        virtual void DrawSpriteSolid(RenderTarget& rt, ImageId image, int32_t x, int32_t y, PaletteIndex colour) = 0;
        virtual void DrawGlyph(RenderTarget& rt, ImageId image, int32_t x, int32_t y, const PaletteMap& palette) = 0;
        virtual void DrawTTFBitmap(
            RenderTarget& rt, TextDrawInfo* info, TTFSurface* surface, int32_t x, int32_t y, uint8_t hintingThreshold)
            = 0;
    };

} // namespace OpenRCT2::Drawing
