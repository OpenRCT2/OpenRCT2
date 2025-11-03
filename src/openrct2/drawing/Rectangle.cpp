/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Rectangle.h"

#include "../interface/Colour.h"
#include "../interface/ColourWithFlags.h"
#include "../world/Location.hpp"
#include "Drawing.h"
#include "IDrawingContext.h"
#include "IDrawingEngine.h"

using OpenRCT2::Drawing::IDrawingContext;

namespace OpenRCT2::Drawing::Rectangle
{
    void fill(RenderTarget& rt, const ScreenRect& rect, int32_t colour)
    {
        auto drawingEngine = rt.DrawingEngine;
        if (drawingEngine != nullptr)
        {
            IDrawingContext* dc = drawingEngine->GetDrawingContext();
            dc->FillRect(rt, colour, rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
        }
    }

    /**
     * Draw a rectangle, with optional border or fill
     *
     *  rct2: 0x006E6F81
     * dpi (edi)
     * left (ax)
     * top (cx)
     * right (bx)
     * bottom (dx)
     * colour (ebp)
     * flags (si)
     */
    void fillInset(
        RenderTarget& rt, const ScreenRect& rect, ColourWithFlags colour, BorderStyle borderStyle, FillBrightness brightness,
        FillMode fillMode)
    {
        const auto leftTop = ScreenCoordsXY{ rect.GetLeft(), rect.GetTop() };
        const auto leftBottom = ScreenCoordsXY{ rect.GetLeft(), rect.GetBottom() };
        const auto rightTop = ScreenCoordsXY{ rect.GetRight(), rect.GetTop() };
        const auto rightBottom = ScreenCoordsXY{ rect.GetRight(), rect.GetBottom() };
        if (colour.flags.has(ColourFlag::translucent))
        {
            auto palette = kTranslucentWindowPalettes[colour.colour];

            switch (borderStyle)
            {
                case BorderStyle::none:
                    Rectangle::filter(rt, rect, palette.base);
                    break;

                case BorderStyle::inset:
                    // Draw outline of box
                    Rectangle::filter(rt, { leftTop, leftBottom }, palette.highlight);
                    Rectangle::filter(rt, { leftTop, rightTop }, palette.highlight);
                    Rectangle::filter(rt, { rightTop, rightBottom }, palette.shadow);
                    Rectangle::filter(rt, { leftBottom, rightBottom }, palette.shadow);

                    if (fillMode != FillMode::none)
                    {
                        Rectangle::filter(
                            rt, { leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } }, palette.base);
                    }
                    break;

                case BorderStyle::outset:
                    // Draw outline of box
                    Rectangle::filter(rt, { leftTop, leftBottom }, palette.shadow);
                    Rectangle::filter(rt, { leftTop, rightTop }, palette.shadow);
                    Rectangle::filter(rt, { rightTop, rightBottom }, palette.highlight);
                    Rectangle::filter(rt, { leftBottom, rightBottom }, palette.highlight);

                    if (fillMode != FillMode::none)
                    {
                        Rectangle::filter(
                            rt, { leftTop + ScreenCoordsXY{ 1, 1 }, { rightBottom - ScreenCoordsXY{ 1, 1 } } }, palette.base);
                    }
                    break;
            }
        }
        else
        {
            uint8_t shadow, fill, hilight;
            if (brightness == FillBrightness::dark)
            {
                shadow = ColourMapA[colour.colour].dark;
                fill = ColourMapA[colour.colour].mid_light;
                hilight = ColourMapA[colour.colour].lighter;
            }
            else
            {
                shadow = ColourMapA[colour.colour].mid_dark;
                fill = ColourMapA[colour.colour].light;
                hilight = ColourMapA[colour.colour].lighter;
            }

            switch (borderStyle)
            {
                case BorderStyle::none:
                    Rectangle::fill(rt, rect, fill);
                    break;

                case BorderStyle::inset:
                    // Draw outline of box
                    Rectangle::fill(rt, { leftTop, leftBottom }, shadow);
                    Rectangle::fill(rt, { leftTop + ScreenCoordsXY{ 1, 0 }, rightTop }, shadow);
                    Rectangle::fill(rt, { rightTop + ScreenCoordsXY{ 0, 1 }, rightBottom - ScreenCoordsXY{ 0, 1 } }, hilight);
                    Rectangle::fill(rt, { leftBottom + ScreenCoordsXY{ 1, 0 }, rightBottom }, hilight);

                    if (fillMode != FillMode::none)
                    {
                        if (fillMode != FillMode::dontLightenWhenInset)
                        {
                            fill = ColourMapA[colour.colour].lighter;
                        }
                        Rectangle::fill(rt, { leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } }, fill);
                    }
                    break;

                case BorderStyle::outset:
                    // Draw outline of box
                    Rectangle::fill(rt, { leftTop, leftBottom - ScreenCoordsXY{ 0, 1 } }, hilight);
                    Rectangle::fill(rt, { leftTop + ScreenCoordsXY{ 1, 0 }, rightTop - ScreenCoordsXY{ 1, 0 } }, hilight);
                    Rectangle::fill(rt, { rightTop, rightBottom - ScreenCoordsXY{ 0, 1 } }, shadow);
                    Rectangle::fill(rt, { leftBottom, rightBottom }, shadow);

                    if (fillMode != FillMode::none)
                    {
                        Rectangle::fill(rt, { leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } }, fill);
                    }
                    break;
            }
        }
    }

    void filter(RenderTarget& rt, const ScreenRect& rect, FilterPaletteID palette)
    {
        auto drawingEngine = rt.DrawingEngine;
        if (drawingEngine != nullptr)
        {
            IDrawingContext* dc = drawingEngine->GetDrawingContext();
            dc->FilterRect(rt, palette, rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
        }
    }
} // namespace OpenRCT2::Drawing::Rectangle
