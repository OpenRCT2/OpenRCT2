/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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
    interface IDrawingEngine;

    interface IDrawingContext
    {
        virtual ~IDrawingContext() { }

        virtual OpenRCT2::Drawing::IDrawingEngine * GetEngine() abstract;

        virtual void Clear(uint8_t paletteIndex)                                                                abstract;
        virtual void FillRect(uint32_t colour, int32_t left, int32_t top, int32_t right, int32_t bottom)            abstract;
        virtual void FilterRect(FILTER_PALETTE_ID palette, int32_t left, int32_t top, int32_t right, int32_t bottom) abstract;
        virtual void DrawLine(uint32_t colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2)                      abstract;
        virtual void DrawSprite(uint32_t image, int32_t x, int32_t y, uint32_t tertiaryColour)                      abstract;
        virtual void DrawSpriteRawMasked(int32_t x, int32_t y, uint32_t maskImage, uint32_t colourImage)            abstract;
        virtual void DrawSpriteSolid(uint32_t image, int32_t x, int32_t y, uint8_t colour)                          abstract;
        virtual void DrawGlyph(uint32_t image, int32_t x, int32_t y, uint8_t * palette)                             abstract;
    };
} // namespace OpenRCT2::Drawing
