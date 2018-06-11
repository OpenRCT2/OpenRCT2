/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
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

        virtual void Clear(uint8 paletteIndex)                                                                abstract;
        virtual void FillRect(uint32 colour, sint32 left, sint32 top, sint32 right, sint32 bottom)            abstract;
        virtual void FilterRect(FILTER_PALETTE_ID palette, sint32 left, sint32 top, sint32 right, sint32 bottom) abstract;
        virtual void DrawLine(uint32 colour, sint32 x1, sint32 y1, sint32 x2, sint32 y2)                      abstract;
        virtual void DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour)                      abstract;
        virtual void DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage)            abstract;
        virtual void DrawSpriteSolid(uint32 image, sint32 x, sint32 y, uint8 colour)                          abstract;
        virtual void DrawGlyph(uint32 image, sint32 x, sint32 y, uint8 * palette)                             abstract;
    };
} // namespace OpenRCT2::Drawing
