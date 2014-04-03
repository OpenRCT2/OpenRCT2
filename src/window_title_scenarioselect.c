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

static rct_widget window_levelselect_widgets[] = {
	{ WWT_FRAME,	0,	0,		609,	0,		333,	-1,						STR_NONE },				// panel / background
	{ WWT_CAPTION,	0,	1,		608,	1,		14,		STR_SELECT_SCENARIO,	STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,	0,	597,	607,	2,		13,		824,					STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_IMGBTN,	1,	0,		609,	50,		333,	-1,						STR_NONE },				// tab content panel
	{ WWT_TAB,		1,	3,		93,		17,		50,		0x200015BC,				STR_NONE },				// tab 1
	{ WWT_TAB,		1,	94,		184,	17,		50,		0x200015BC,				STR_NONE },				// tab 2
	{ WWT_TAB,		1,	185,	275,	17,		50,		0x200015BC,				STR_NONE },				// tab 3
	{ WWT_TAB,		1,	276,	366,	17,		50,		0x200015BC,				STR_NONE },				// tab 4
	{ WWT_TAB,		1,	367,	457,	17,		50,		0x200015BC,				STR_NONE },				// tab 5
	{ WWT_SCROLL,	1,	3,		433,	54,		329,	2,						STR_NONE },				// level list
	{ WIDGETS_END },
};

/**
 * 
 *  rct2: 0x006781B5
 */
void window_levelselect_open()
{
	rct_window* window;
	int y;

	if (window_bring_to_front_by_id(WC_LEVEL_SELECT, 0) != NULL)
		return;

	// Load scenario list
	RCT2_CALLPROC_EBPSAFE(0x006775A8); // scenario_load_list();

	y = (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) / 2) - 167;
	if (y < 28)
		y = 28;
	window = window_create((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) / 2) - 305, y, 610, 334, 0x0097FC4C, WC_LEVEL_SELECT, 0x0402);
	window->widgets = window_levelselect_widgets;
	
	window->enabled_widgets = 0x04 | 0x10 | 0x20 | 0x40 | 0x80 | 0x100;
	window_init_scroll_widgets(window);
	window->colours[0] = 1;
	window->colours[1] = 26;
	window->colours[2] = 26;
	window->var_480 = -1;
	window->var_494 = -1;

	RCT2_CALLPROC_EBPSAFE(0x00677C8A); // window_levelselect_init_tabs();

	window->var_4AC = 0;
}