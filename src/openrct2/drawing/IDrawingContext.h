/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Drawing.h"
#include "TTF.h"

namespace OpenRCT2::Drawing
{
    struct IDrawingEngine;

    struct IDrawingContext
    {
        virtual ~IDrawingContext() = default;

        virtual void Clear(RenderTarget& dpi, uint8_t paletteIndex) = 0;
        virtual void FillRect(RenderTarget& dpi, uint32_t colour, int32_t left, int32_t top, int32_t right, int32_t bottom)
            = 0;
        virtual void FilterRect(
            RenderTarget& dpi, FilterPaletteID palette, int32_t left, int32_t top, int32_t right, int32_t bottom)
            = 0;
        virtual void DrawLine(RenderTarget& dpi, uint32_t colour, const ScreenLine& line) = 0;
        virtual void DrawSprite(RenderTarget& dpi, const ImageId image, int32_t x, int32_t y) = 0;
        virtual void DrawSpriteRawMasked(
            RenderTarget& dpi, int32_t x, int32_t y, const ImageId maskImage, const ImageId colourImage)
            = 0;
        virtual void DrawSpriteSolid(RenderTarget& dpi, const ImageId image, int32_t x, int32_t y, uint8_t colour) = 0;
        virtual void DrawGlyph(RenderTarget& dpi, const ImageId image, int32_t x, int32_t y, const PaletteMap& palette) = 0;
        virtual void DrawTTFBitmap(
            RenderTarget& dpi, TextDrawInfo* info, TTFSurface* surface, int32_t x, int32_t y, uint8_t hintingThreshold)
            = 0;
    };

} // namespace OpenRCT2::Drawing
