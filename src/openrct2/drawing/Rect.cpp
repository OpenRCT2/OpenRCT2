/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Colour.h"
#include "../world/Location.hpp"
#include "Drawing.h"

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
void GfxFillRectInset(
    RenderTarget& rt, const ScreenRect& rect, ColourWithFlags colour, RectBorderStyle borderStyle, uint8_t flags)
{
    const auto leftTop = ScreenCoordsXY{ rect.GetLeft(), rect.GetTop() };
    const auto leftBottom = ScreenCoordsXY{ rect.GetLeft(), rect.GetBottom() };
    const auto rightTop = ScreenCoordsXY{ rect.GetRight(), rect.GetTop() };
    const auto rightBottom = ScreenCoordsXY{ rect.GetRight(), rect.GetBottom() };
    if (colour.hasFlag(ColourFlag::translucent))
    {
        auto palette = kTranslucentWindowPalettes[colour.colour];

        switch (borderStyle)
        {
            case RectBorderStyle::none:
                GfxFilterRect(rt, rect, palette.base);
                break;

            case RectBorderStyle::inset:
                // Draw outline of box
                GfxFilterRect(rt, { leftTop, leftBottom }, palette.highlight);
                GfxFilterRect(rt, { leftTop, rightTop }, palette.highlight);
                GfxFilterRect(rt, { rightTop, rightBottom }, palette.shadow);
                GfxFilterRect(rt, { leftBottom, rightBottom }, palette.shadow);

                if (!(flags & INSET_RECT_FLAG_FILL_NONE))
                {
                    GfxFilterRect(rt, { leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } }, palette.base);
                }
                break;

            case RectBorderStyle::outset:
                // Draw outline of box
                GfxFilterRect(rt, { leftTop, leftBottom }, palette.shadow);
                GfxFilterRect(rt, { leftTop, rightTop }, palette.shadow);
                GfxFilterRect(rt, { rightTop, rightBottom }, palette.highlight);
                GfxFilterRect(rt, { leftBottom, rightBottom }, palette.highlight);

                if (!(flags & INSET_RECT_FLAG_FILL_NONE))
                {
                    GfxFilterRect(
                        rt, { leftTop + ScreenCoordsXY{ 1, 1 }, { rightBottom - ScreenCoordsXY{ 1, 1 } } }, palette.base);
                }
                break;
        }
    }
    else
    {
        uint8_t shadow, fill, hilight;
        if (flags & INSET_RECT_FLAG_FILL_MID_LIGHT)
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
            case RectBorderStyle::none:
                GfxFillRect(rt, rect, fill);
                break;

            case RectBorderStyle::inset:
                // Draw outline of box
                GfxFillRect(rt, { leftTop, leftBottom }, shadow);
                GfxFillRect(rt, { leftTop + ScreenCoordsXY{ 1, 0 }, rightTop }, shadow);
                GfxFillRect(rt, { rightTop + ScreenCoordsXY{ 0, 1 }, rightBottom - ScreenCoordsXY{ 0, 1 } }, hilight);
                GfxFillRect(rt, { leftBottom + ScreenCoordsXY{ 1, 0 }, rightBottom }, hilight);

                if (!(flags & INSET_RECT_FLAG_FILL_NONE))
                {
                    if (!(flags & INSET_RECT_FLAG_FILL_DONT_LIGHTEN))
                    {
                        fill = ColourMapA[colour.colour].lighter;
                    }
                    GfxFillRect(rt, { leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } }, fill);
                }
                break;

            case RectBorderStyle::outset:
                // Draw outline of box
                GfxFillRect(rt, { leftTop, leftBottom - ScreenCoordsXY{ 0, 1 } }, hilight);
                GfxFillRect(rt, { leftTop + ScreenCoordsXY{ 1, 0 }, rightTop - ScreenCoordsXY{ 1, 0 } }, hilight);
                GfxFillRect(rt, { rightTop, rightBottom - ScreenCoordsXY{ 0, 1 } }, shadow);
                GfxFillRect(rt, { leftBottom, rightBottom }, shadow);

                if (!(flags & INSET_RECT_FLAG_FILL_NONE))
                {
                    GfxFillRect(rt, { leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } }, fill);
                }
                break;
        }
    }
}
