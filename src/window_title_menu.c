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
#include "sprites.h"
#include "widget.h"
#include "window.h"

void window_levelselect_open();

enum {
	WIDX_START_NEW_GAME,
	WIDX_CONTINUE_SAVED_GAME,
	WIDX_SHOW_TUTORIAL,
	WIDX_GAME_TOOLS,
};

static rct_widget window_title_menu_widgets[] = {
	{ WWT_IMGBTN, 2, 0, 81, 0, 81, SPR_MENU_NEW_GAME, STR_START_NEW_GAME_TIP },
	{ WWT_IMGBTN, 2, 82, 163, 0, 81, SPR_MENU_LOAD_GAME, STR_CONTINUE_SAVED_GAME_TIP },
	{ WWT_IMGBTN, 2, 164, 245, 0, 81, SPR_MENU_TUTORIAL, STR_SHOW_TUTORIAL_TIP },
	{ WWT_IMGBTN, 2, 246, 327, 0, 81, SPR_MENU_TOOLBOX, STR_GAME_TOOLS },
	{ WIDGETS_END },
};

static uint32 window_title_menu_events[] = {
	0x0066B834,
	0x0066B6EC,
	0x0066B834,
	0x0066B70E,
	0x0066B71F,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B834,
	0x0066B730,
	0x0066B834,
	0x0066B834,
	0x0066B6E6,
	0x0066B834
};

/**
 * Creates the window containing the menu buttons on the title screen.
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
void window_title_menu_open()
{
	rct_window* window;

	window = window_create(
		(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - 328) / 2,
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) - 142,
		328, 82, window_title_menu_events, WC_TITLE_MENU, 0x02
	);
	window->widgets = window_title_menu_widgets;
	window->enabled_widgets |= (8 | 4 | 2 | 1);
	window_init_scroll_widgets(window);
	window->flags |= 16;
	window->colours[0] = 140;
	window->colours[1] = 140;
	window->colours[2] = 140;
}