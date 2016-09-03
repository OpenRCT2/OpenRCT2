#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
void gfx_fill_rect_inset(rct_drawpixelinfo* dpi, short left, short top, short right, short bottom, int colour, short flags)
{
	uint8 shadow, fill, hilight;

	// Flags
	int no_border, no_fill, pressed;

	no_border = 8;
	no_fill = 0x10;
	pressed = 0x20;

	if (colour & 0x180) {
		if (colour & 0x100) {
			colour = NOT_TRANSLUCENT(colour);
		} else {
			colour = _9DEDF4[colour];
		}

		colour = colour | 0x2000000; //Transparent

		if (flags & no_border) {
			gfx_fill_rect(dpi, left, top, right, bottom, colour);
		} else if (flags & pressed) {
			// Draw outline of box
			gfx_fill_rect(dpi, left, top, left, bottom, colour + 1);
			gfx_fill_rect(dpi, left, top, right, top, colour + 1);
			gfx_fill_rect(dpi, right, top, right, bottom, colour + 2);
			gfx_fill_rect(dpi, left, bottom, right, bottom, colour + 2);

			if (!(flags & no_fill)) {
				gfx_fill_rect(dpi, left+1, top+1, right-1, bottom-1, colour);
			}
		} else {
			// Draw outline of box
			gfx_fill_rect(dpi, left, top, left, bottom, colour + 2);
			gfx_fill_rect(dpi, left, top, right, top, colour + 2);
			gfx_fill_rect(dpi, right, top, right, bottom, colour + 1);
			gfx_fill_rect(dpi, left, bottom, right, bottom, colour + 1);

			if (!(flags & no_fill)) {
				gfx_fill_rect(dpi, left+1, top+1, right-1, bottom-1, colour);
			}
		}
	} else {
		if (flags & 0x80) {
			shadow	= ColourMapA[colour].dark;
			fill	= ColourMapA[colour].mid_light;
			hilight	= ColourMapA[colour].lighter;
		} else {
			shadow	= ColourMapA[colour].mid_dark;
			fill	= ColourMapA[colour].light;
			hilight	= ColourMapA[colour].lighter;
		}

		if (flags & no_border) {
			gfx_fill_rect(dpi, left, top, right, bottom, fill);
		} else if (flags & pressed) {
			// Draw outline of box
			gfx_fill_rect(dpi, left, top, left, bottom, shadow);
			gfx_fill_rect(dpi, left + 1, top, right, top, shadow);
			gfx_fill_rect(dpi, right, top + 1, right, bottom - 1, hilight);
			gfx_fill_rect(dpi, left + 1, bottom, right, bottom, hilight);

			if (!(flags & no_fill)) {
				if (!(flags & 0x40)) {
					if (flags & 0x04) {
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

			if (!(flags & no_fill)) {
				if (flags & 0x04) {
					fill = ColourMapA[COLOUR_BLACK].light;
				}
				gfx_fill_rect(dpi, left+1, top+1, right-1, bottom-1, fill);
			}
		}
	}
}
