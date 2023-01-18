/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"
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
void GfxFillRectInset(DrawPixelInfo* dpi, const ScreenRect& rect, int32_t colour, uint8_t flags)
{
    const auto leftTop = ScreenCoordsXY{ rect.GetLeft(), rect.GetTop() };
    const auto leftBottom = ScreenCoordsXY{ rect.GetLeft(), rect.GetBottom() };
    const auto rightTop = ScreenCoordsXY{ rect.GetRight(), rect.GetTop() };
    const auto rightBottom = ScreenCoordsXY{ rect.GetRight(), rect.GetBottom() };
    if (colour & (COLOUR_FLAG_TRANSLUCENT | COLOUR_FLAG_8))
    {
        TranslucentWindowPalette palette;
        if (colour & COLOUR_FLAG_8)
        {
            // TODO: This can't be added up
            // palette = NOT_TRANSLUCENT(colour);
            assert(false);
            return;
        }

        palette = TranslucentWindowPalettes[BASE_COLOUR(colour)];

        if (flags & INSET_RECT_FLAG_BORDER_NONE)
        {
            GfxFilterRect(dpi, rect, palette.base);
        }
        else if (flags & INSET_RECT_FLAG_BORDER_INSET)
        {
            // Draw outline of box
            GfxFilterRect(dpi, { leftTop, leftBottom }, palette.highlight);
            GfxFilterRect(dpi, { leftTop, rightTop }, palette.highlight);
            GfxFilterRect(dpi, { rightTop, rightBottom }, palette.shadow);
            GfxFilterRect(dpi, { leftBottom, rightBottom }, palette.shadow);

            if (!(flags & INSET_RECT_FLAG_FILL_NONE))
            {
                GfxFilterRect(dpi, { leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } }, palette.base);
            }
        }
        else
        {
            // Draw outline of box
            GfxFilterRect(dpi, { leftTop, leftBottom }, palette.shadow);
            GfxFilterRect(dpi, { leftTop, rightTop }, palette.shadow);
            GfxFilterRect(dpi, { rightTop, rightBottom }, palette.highlight);
            GfxFilterRect(dpi, { leftBottom, rightBottom }, palette.highlight);

            if (!(flags & INSET_RECT_FLAG_FILL_NONE))
            {
                GfxFilterRect(
                    dpi, { leftTop + ScreenCoordsXY{ 1, 1 }, { rightBottom - ScreenCoordsXY{ 1, 1 } } }, palette.base);
            }
        }
    }
    else
    {
        uint8_t shadow, fill, hilight;
        if (flags & INSET_RECT_FLAG_FILL_MID_LIGHT)
        {
            shadow = ColourMapA[colour].dark;
            fill = ColourMapA[colour].mid_light;
            hilight = ColourMapA[colour].lighter;
        }
        else
        {
            shadow = ColourMapA[colour].mid_dark;
            fill = ColourMapA[colour].light;
            hilight = ColourMapA[colour].lighter;
        }

        if (flags & INSET_RECT_FLAG_BORDER_NONE)
        {
            GfxFillRect(dpi, rect, fill);
        }
        else if (flags & INSET_RECT_FLAG_BORDER_INSET)
        {
            // Draw outline of box
            GfxFillRect(dpi, { leftTop, leftBottom }, shadow);
            GfxFillRect(dpi, { leftTop + ScreenCoordsXY{ 1, 0 }, rightTop }, shadow);
            GfxFillRect(dpi, { rightTop + ScreenCoordsXY{ 0, 1 }, rightBottom - ScreenCoordsXY{ 0, 1 } }, hilight);
            GfxFillRect(dpi, { leftBottom + ScreenCoordsXY{ 1, 0 }, rightBottom }, hilight);

            if (!(flags & INSET_RECT_FLAG_FILL_NONE))
            {
                if (!(flags & INSET_RECT_FLAG_FILL_DONT_LIGHTEN))
                {
                    if (flags & INSET_RECT_FLAG_FILL_GREY)
                    {
                        fill = ColourMapA[COLOUR_BLACK].light;
                    }
                    else
                    {
                        fill = ColourMapA[colour].lighter;
                    }
                }
                GfxFillRect(dpi, { leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } }, fill);
            }
        }
        else
        {
            // Draw outline of box
            GfxFillRect(dpi, { leftTop, leftBottom - ScreenCoordsXY{ 0, 1 } }, hilight);
            GfxFillRect(dpi, { leftTop + ScreenCoordsXY{ 1, 0 }, rightTop - ScreenCoordsXY{ 1, 0 } }, hilight);
            GfxFillRect(dpi, { rightTop, rightBottom - ScreenCoordsXY{ 0, 1 } }, shadow);
            GfxFillRect(dpi, { leftBottom, rightBottom }, shadow);

            if (!(flags & INSET_RECT_FLAG_FILL_NONE))
            {
                if (flags & INSET_RECT_FLAG_FILL_GREY)
                {
                    fill = ColourMapA[COLOUR_BLACK].light;
                }
                GfxFillRect(dpi, { leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } }, fill);
            }
        }
    }
}
