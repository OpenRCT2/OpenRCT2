/*****************************************************************************
 * Copyright (c) 2014 Ted John
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

#include "addresses.h"
#include "strings.h"
#include "widget.h"
#include "window.h"

static rct_widget window_title_logo_widgets[] = {
	{ WWT_EMPTY, 0, 0, 0, 0, 0, 0xFFFFFFFF, STR_NONE },
	{ WIDGETS_END },
};

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
		if (RCT2_GLOBAL(0x009AB4C0, uint16) & (1 << i))
			packs++;

	// Create the window
	window = window_create(0, 0, 200, 106 + (10 * packs), 0x0097BF6C, WC_TITLE_LOGO, 0x02);
	window->widgets = 0x009A9658; // mouse move bug in original game, keep this address and no crash happens
	window_init_scroll_widgets(window);
	window->flags |= 16;
	window->colours[0] = 129;
	window->colours[1] = 129;
	window->colours[2] = 129;
}
