/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Drawing.h"

namespace OpenRCT2::Drawing
{
    struct IDrawingEngine;

    struct IDrawingContext
    {
        virtual ~IDrawingContext() = default;

        virtual void Clear(rct_drawpixelinfo* dpi, uint8_t paletteIndex) abstract;
        virtual void FillRect(
            rct_drawpixelinfo* dpi, uint32_t colour, int32_t left, int32_t top, int32_t right, int32_t bottom) abstract;
        virtual void FilterRect(
            rct_drawpixelinfo* dpi, FilterPaletteID palette, int32_t left, int32_t top, int32_t right, int32_t bottom) abstract;
        virtual void DrawLine(rct_drawpixelinfo* dpi, uint32_t colour, const ScreenLine& line) abstract;
        virtual void DrawSprite(rct_drawpixelinfo* dpi, const ImageId image, int32_t x, int32_t y) abstract;
        virtual void DrawSpriteRawMasked(
            rct_drawpixelinfo* dpi, int32_t x, int32_t y, const ImageId maskImage, const ImageId colourImage) abstract;
        virtual void DrawSpriteSolid(
            rct_drawpixelinfo* dpi, const ImageId image, int32_t x, int32_t y, uint8_t colour) abstract;
        virtual void DrawGlyph(
            rct_drawpixelinfo* dpi, const ImageId image, int32_t x, int32_t y, const PaletteMap& palette) abstract;
        virtual void DrawBitmap(
            rct_drawpixelinfo* dpi, ImageIndex image, const void* pixels, int32_t width, int32_t height, int32_t x,
            int32_t y) abstract;
    };

} // namespace OpenRCT2::Drawing
