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

static enum WINDOW_GAME_BOTTOM_TOOLBAR_WIDGET_IDX {
	WIDX_LEFT_OUTSET,
	WIDX_LEFT_INSET,
	WIDX_MONEY,
	WIDX_GUESTS,
	WIDX_PARK_RATING,
	WIDX_MIDDLE_OUTSET,
	WIDX_MIDDLE_INSET,
	WIDX_NEWS_SUBJECT,
	WIDX_NEWS_LOCATE,
	WIDX_RIGHT_OUTSET,
	WIDX_RIGHT_INSET,
	WIDX_DATE,
};

rct_widget window_game_bottom_toolbar_widgets[] = {
	{ WWT_IMGBTN,	0,	0x0000,	0x0077,	0,		33,		0xFFFFFFFF,	STR_NONE },	// Left outset panel
	{ WWT_IMGBTN,	0,	0x0002,	0x0075,	2,		31,		0xFFFFFFFF,	STR_NONE },	// Left inset panel
	{ WWT_FLATBTN,	0,	0x0002,	0x0075,	1,		12,		0xFFFFFFFF,	STR_PROFIT_PER_WEEK_AND_PARK_VALUE_TIP },	// Money window
	{ WWT_FLATBTN,	0,	0x0002,	0x0075,	11,		22,		0xFFFFFFFF,	STR_NONE },	// Guests window
	{ WWT_FLATBTN,	0,	0x0002,	0x0075,	21,		31,		0xFFFFFFFF,	STR_PARK_rating_TIP },	// Park rating window

	{ WWT_IMGBTN,	2,	0x0078,	0x0207,	0,		33,		0xFFFFFFFF,	STR_NONE },	// Middle outset panel
	{ WWT_25,		2,	0x007A,	0x0205,	2,		31,		0xFFFFFFFF,	STR_NONE },	// Middle inset panel
	{ WWT_FLATBTN,	2,	0x007D,	0x0094,	5,		28,		0xFFFFFFFF,	STR_SHOW_SUBJECT_TIP },	// Associated news item window
	{ WWT_FLATBTN,	2,	0x01EB,	0x0202,	5,		28,		SPR_LOCATE,	STR_LOCATE_SUBJECT_TIP },	// Scroll to news item target

	{ WWT_IMGBTN,	0,	0x0208,	0x027F,	0,		33,		0xFFFFFFFF,	STR_NONE },	// Right outset panel
	{ WWT_IMGBTN,	0,	0x020A,	0x027D,	2,		31,		0xFFFFFFFF,	STR_NONE },	// Right inset panel
	{ WWT_FLATBTN,	0,	0x020A,	0x027D,	2,		13,		0xFFFFFFFF,	2290 },	// Date
	{ WIDGETS_END },
};

/**
 * Creates the main game bottom toolbar window.
 *  rct2: 0x0066B52F (part of 0x0066B3E8)
 */
void window_bottom_toolbar_open()
{
	rct_window* window;

	window = window_create(0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) - 32, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16), 32, 0x0097BFDC, WC_BOTTOM_TOOLBAR, 0x32);
	window->widgets = 0x009A953C;
	window->enabled_widgets |=
		(1 << WIDX_LEFT_OUTSET) |
		(1 << WIDX_MONEY) |
		(1 << WIDX_GUESTS) |
		(1 << WIDX_PARK_RATING) |
		(1 << WIDX_MIDDLE_OUTSET) |
		(1 << WIDX_MIDDLE_INSET) |
		(1 << WIDX_NEWS_SUBJECT) |
		(1 << WIDX_NEWS_LOCATE) |
		(1 << WIDX_RIGHT_OUTSET) |
		(1 << WIDX_DATE);

	window->var_48E = 0;
	window_init_scroll_widgets(window);
	window->colours[0] = 140;
	window->colours[1] = 140;
	window->colours[2] = 0;
}