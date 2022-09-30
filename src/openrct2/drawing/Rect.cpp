/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
void gfx_fill_rect_inset(rct_drawpixelinfo* dpi, const ScreenRect& rect, int32_t colour, uint8_t flags)
{
    const auto leftTop = ScreenCoordsXY{ rect.GetLeft(), rect.GetTop() };
    const auto leftBottom = ScreenCoordsXY{ rect.GetLeft(), rect.GetBottom() };
    const auto rightTop = ScreenCoordsXY{ rect.GetRight(), rect.GetTop() };
    const auto rightBottom = ScreenCoordsXY{ rect.GetRight(), rect.GetBottom() };
    if (colour & (COLOUR_FLAG_TRANSLUCENT | COLOUR_FLAG_8))
    {
        translucent_window_palette palette;
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
            gfx_filter_rect(dpi, rect, palette.base);
        }
        else if (flags & INSET_RECT_FLAG_BORDER_INSET)
        {
            // Draw outline of box
            gfx_filter_rect(dpi, { leftTop, leftBottom }, palette.highlight);
            gfx_filter_rect(dpi, { leftTop, rightTop }, palette.highlight);
            gfx_filter_rect(dpi, { rightTop, rightBottom }, palette.shadow);
            gfx_filter_rect(dpi, { leftBottom, rightBottom }, palette.shadow);

            if (!(flags & INSET_RECT_FLAG_FILL_NONE))
            {
                gfx_filter_rect(dpi, { leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } }, palette.base);
            }
        }
        else
        {
            // Draw outline of box
            gfx_filter_rect(dpi, { leftTop, leftBottom }, palette.shadow);
            gfx_filter_rect(dpi, { leftTop, rightTop }, palette.shadow);
            gfx_filter_rect(dpi, { rightTop, rightBottom }, palette.highlight);
            gfx_filter_rect(dpi, { leftBottom, rightBottom }, palette.highlight);

            if (!(flags & INSET_RECT_FLAG_FILL_NONE))
            {
                gfx_filter_rect(
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
            gfx_fill_rect(dpi, rect, fill);
        }
        else if (flags & INSET_RECT_FLAG_BORDER_INSET)
        {
            // Draw outline of box
            gfx_fill_rect(dpi, { leftTop, leftBottom }, shadow);
            gfx_fill_rect(dpi, { leftTop + ScreenCoordsXY{ 1, 0 }, rightTop }, shadow);
            gfx_fill_rect(dpi, { rightTop + ScreenCoordsXY{ 0, 1 }, rightBottom - ScreenCoordsXY{ 0, 1 } }, hilight);
            gfx_fill_rect(dpi, { leftBottom + ScreenCoordsXY{ 1, 0 }, rightBottom }, hilight);

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
                gfx_fill_rect(dpi, { leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } }, fill);
            }
        }
        else
        {
            // Draw outline of box
            gfx_fill_rect(dpi, { leftTop, leftBottom - ScreenCoordsXY{ 0, 1 } }, hilight);
            gfx_fill_rect(dpi, { leftTop + ScreenCoordsXY{ 1, 0 }, rightTop - ScreenCoordsXY{ 1, 0 } }, hilight);
            gfx_fill_rect(dpi, { rightTop, rightBottom - ScreenCoordsXY{ 0, 1 } }, shadow);
            gfx_fill_rect(dpi, { leftBottom, rightBottom }, shadow);

            if (!(flags & INSET_RECT_FLAG_FILL_NONE))
            {
                if (flags & INSET_RECT_FLAG_FILL_GREY)
                {
                    fill = ColourMapA[COLOUR_BLACK].light;
                }
                gfx_fill_rect(dpi, { leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } }, fill);
            }
        }
    }
}
