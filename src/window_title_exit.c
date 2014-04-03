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
#include "sprites.h"
#include "strings.h"
#include "widget.h"
#include "window.h"

static rct_widget window_title_exit_widgets[] = {
	{ WWT_IMGBTN, 2, 0, 39, 0, 63, SPR_MENU_EXIT, STR_EXIT },
	{ WIDGETS_END },
};

/**
 * Creates the window containing the exit button on the title screen.
 *  rct2: 0x0066B624 (part of 0x0066B3E8)
 */
void window_title_exit_open()
{
	rct_window* window;

	window = window_create(
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - 40,
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) - 64,
		40, 64, 0x0097BEFC, WC_TITLE_EXIT, 0x02
	);
	window->widgets = window_title_exit_widgets;
	window->enabled_widgets |= 1;
	window_init_scroll_widgets(window);
	window->flags |= 16;
	window->colours[0] = 140;
	window->colours[1] = 140;
	window->colours[2] = 140;
}