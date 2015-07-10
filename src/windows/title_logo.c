/*****************************************************************************
 * Copyright (c) 2014 Ted John, Ben Pye
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "../addresses.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../interface/widget.h"
#include "../interface/window.h"

static rct_widget window_title_logo_widgets[] = {
	{ WWT_EMPTY, 0, 0, 0, 0, 0, 0xFFFFFFFF, STR_NONE },
	{ WIDGETS_END },
};

static void window_title_logo_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_title_logo_events = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_title_logo_paint,
	NULL
};

static void window_title_logo_draw_expansion_packs(rct_drawpixelinfo *dpi);

/**
 * Creates the window containing the logo and the expansion packs on the title screen.
 *  rct2: 0x0066B679 (part of 0x0066B3E8)
 */
void window_title_logo_open()
{
	int i, packs;
	rct_window *window;
	
	// Count number of expansion packs
	packs = 0;
	for (i = 0; i < 16; i++)
		if (RCT2_GLOBAL(RCT2_ADDRESS_EXPANSION_FLAGS, uint16) & (1 << i))
			packs++;

	// Create the window
	window = window_create(
		0,
		0,
		200,
		106 + (10 * packs),
		&window_title_logo_events,
		WC_TITLE_LOGO,
		WF_STICK_TO_BACK | WF_TRANSPARENT
	);
	window->widgets = (rct_widget*)0x009A9658; // mouse move bug in original game, keep this address and no crash happens
	window_init_scroll_widgets(window);
	window->colours[0] = 129;
	window->colours[1] = 129;
	window->colours[2] = 129;
}

/**
*
*  rct2: 0x0066B872
*/
static void window_title_logo_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	// gfx_draw_sprite(dpi, SPR_MENU_LOGO, w->x, w->y, 0);
	int x = 2, y = 2;
	gfx_draw_sprite(dpi, SPR_G2_LOGO, w->x + x, w->y + y, 0);
	gfx_draw_sprite(dpi, SPR_G2_TITLE, w->x + x + 104, w->y + y + 18, 0);
	window_title_logo_draw_expansion_packs(dpi);
}

static void window_title_logo_draw_expansion_packs(rct_drawpixelinfo *dpi)
{
	int packs, x, y, i;
	char *buffer, *names;

	x = 0;
	y = 105;

	packs = RCT2_GLOBAL(RCT2_ADDRESS_EXPANSION_FLAGS, uint16);
	names = RCT2_ADDRESS(RCT2_ADDRESS_EXPANSION_NAMES, char);
	buffer = (char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER;
	while (packs != 0) {
		if (packs & 1) {
			// Prefix for expansion name
			buffer[0] = '\n';
			buffer[1] = '\v';
			buffer[2] = FORMAT_YELLOW; // Colour of the text
			buffer[3] = '+';
			buffer[4] = ' ';

			// Copies the expansion name to the buffer, offset by 5
			i = 0;
			do {
				buffer[5 + i] = names[i];
				i++;
			} while (names[i - 1] != 0);

			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint32) = 0;
			gfx_draw_string(dpi, buffer, 0, x, y);
			y += 10;
		}

		packs = packs >> 1;
		names += 128;
	}
}