#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../common.h"
#include "../interface/colour.h"
#include "../platform/platform.h"
#include "drawing.h"

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
void gfx_fill_rect_inset(rct_drawpixelinfo* dpi, sint16 left, sint16 top, sint16 right, sint16 bottom, sint32 colour, uint8 flags)
{
	if (colour & (COLOUR_FLAG_TRANSLUCENT | COLOUR_FLAG_8)) {
		translucent_window_palette palette;
		if (colour & COLOUR_FLAG_8) {
			// TODO: This can't be added up
			// palette = NOT_TRANSLUCENT(colour);
			assert(false);
			return;
		} else {
			palette = TranslucentWindowPalettes[BASE_COLOUR(colour)];
		}

		if (flags & INSET_RECT_FLAG_BORDER_NONE) {
			gfx_filter_rect(dpi, left, top, right, bottom, palette.base);
		} else if (flags & INSET_RECT_FLAG_BORDER_INSET) {
			// Draw outline of box
			gfx_filter_rect(dpi, left, top, left, bottom, palette.highlight);
			gfx_filter_rect(dpi, left, top, right, top, palette.highlight);
			gfx_filter_rect(dpi, right, top, right, bottom, palette.shadow);
			gfx_filter_rect(dpi, left, bottom, right, bottom, palette.shadow);

			if (!(flags & INSET_RECT_FLAG_FILL_NONE)) {
				gfx_filter_rect(dpi, left+1, top+1, right-1, bottom-1, palette.base);
			}
		} else {
			// Draw outline of box
			gfx_filter_rect(dpi, left, top, left, bottom, palette.shadow);
			gfx_filter_rect(dpi, left, top, right, top, palette.shadow);
			gfx_filter_rect(dpi, right, top, right, bottom, palette.highlight);
			gfx_filter_rect(dpi, left, bottom, right, bottom, palette.highlight);

			if (!(flags & INSET_RECT_FLAG_FILL_NONE)) {
				gfx_filter_rect(dpi, left+1, top+1, right-1, bottom-1, palette.base);
			}
		}
	} else {
		uint8 shadow, fill, hilight;
		if (flags & INSET_RECT_FLAG_FILL_MID_LIGHT) {
			shadow	= ColourMapA[colour].dark;
			fill	= ColourMapA[colour].mid_light;
			hilight	= ColourMapA[colour].lighter;
		} else {
			shadow	= ColourMapA[colour].mid_dark;
			fill	= ColourMapA[colour].light;
			hilight	= ColourMapA[colour].lighter;
		}

		if (flags & INSET_RECT_FLAG_BORDER_NONE) {
			gfx_fill_rect(dpi, left, top, right, bottom, fill);
		} else if (flags & INSET_RECT_FLAG_BORDER_INSET) {
			// Draw outline of box
			gfx_fill_rect(dpi, left, top, left, bottom, shadow);
			gfx_fill_rect(dpi, left + 1, top, right, top, shadow);
			gfx_fill_rect(dpi, right, top + 1, right, bottom - 1, hilight);
			gfx_fill_rect(dpi, left + 1, bottom, right, bottom, hilight);

			if (!(flags & INSET_RECT_FLAG_FILL_NONE)) {
				if (!(flags & INSET_RECT_FLAG_FILL_DONT_LIGHTEN)) {
					if (flags & INSET_RECT_FLAG_FILL_GREY) {
						fill = ColourMapA[COLOUR_BLACK].light;
					} else {
						fill = ColourMapA[colour].lighter;
					}
				}
				gfx_fill_rect(dpi, left+1, top+1, right-1, bottom-1, fill);
			}
		} else {
			// Draw outline of box
			gfx_fill_rect(dpi, left, top, left, bottom - 1, hilight);
			gfx_fill_rect(dpi, left + 1, top, right - 1, top, hilight);
			gfx_fill_rect(dpi, right, top, right, bottom - 1, shadow);
			gfx_fill_rect(dpi, left, bottom, right, bottom, shadow);

			if (!(flags & INSET_RECT_FLAG_FILL_NONE)) {
				if (flags & INSET_RECT_FLAG_FILL_GREY) {
					fill = ColourMapA[COLOUR_BLACK].light;
				}
				gfx_fill_rect(dpi, left+1, top+1, right-1, bottom-1, fill);
			}
		}
	}
}
