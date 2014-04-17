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
#include "config.h"
#include "date.h"
#include "park.h"
#include "peep.h"
#include "ride.h"
#include "scenario.h"
#include "strings.h"
#include "sprite.h"
#include "sprites.h"
#include "util.h"
#include "viewport.h"
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"

enum WINDOW_PARK_PAGE {
	WINDOW_PARK_PAGE_ENTRANCE,
	WINDOW_PARK_PAGE_RATING,
	WINDOW_PARK_PAGE_GUESTS,
	WINDOW_PARK_PAGE_PRICE,
	WINDOW_PARK_PAGE_STATS,
	WINDOW_PARK_PAGE_OBJECTIVE,
	WINDOW_PARK_PAGE_AWARDS
};

enum WINDOW_PARK_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,
	WIDX_TAB_4,
	WIDX_TAB_5,
	WIDX_TAB_6,
	WIDX_TAB_7,

	WIDX_VIEWPORT = 11,
	WIDX_STATUS,
	WIDX_OPEN_OR_CLOSE,
	WIDX_BUY_LAND_RIGHTS,
	WIDX_BUY_CONSTRUCTION_RIGHTS,
	WIDX_LOCATE,
	WIDX_RENAME,

	WIDX__ = 11,
	WIDX_PRICE,
	WIDX_INCREASE_PRICE,
	WIDX_DECREASE_PRICE,

	WIDX_ENTER_NAME = 11
};

#pragma region Widgets

static rct_widget window_park_entrance_widgets[] = {
	{ WWT_FRAME,			0,	0,		229,	0,		223,	0x0FFFFFFFF,					STR_NONE },							// panel / background
	{ WWT_CAPTION,			0,	1,		228,	1,		14,		0x361,							STR_WINDOW_TITLE_TIP },				// title bar
	{ WWT_CLOSEBOX,			0,	217,	227,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP },				// close x button
	{ WWT_RESIZE,			1,	0,		229,	43,		173,	0x0FFFFFFFF,					STR_NONE },							// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_PARK_ENTRANCE_TAB_TIP },		// tab 1
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,						STR_PARK_RATING_TAB_TIP },			// tab 2
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_PARK_GUESTS_TAB_TIP },			// tab 3
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_PARK_PRICE_TAB_TIP },			// tab 4
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_PARK_STATS_TAB_TIP },			// tab 5
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_PARK_OBJECTIVE_TAB_TIP },		// tab 6
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_PARK_AWARDS_TAB_TIP },			// tab 7

	{ WWT_VIEWPORT,			1,	3,		204,	46,		160,	0x0FFFFFFFF,					STR_NONE },							// viewport
	{ WWT_12,				1,	3,		204,	161,	171,	0x0FFFFFFFF,					STR_NONE },							// status
	{ WWT_FLATBTN,			1,	205,	228,	49,		72,		0x0FFFFFFFF,					STR_OPEN_OR_CLOSE_PARK_TIP },		// open / close
	{ WWT_FLATBTN,			1,	205,	228,	73,		96,		SPR_BUY_LAND_RIGHTS,			SPR_BUY_LAND_RIGHTS_TIP },			// buy land rights
	{ WWT_FLATBTN,			1,	205,	228,	97,		120,	SPR_BUY_CONSTRUCTION_RIGHTS,	SPR_BUY_CONSTRUCTION_RIGHTS_TIP },	// buy construction rights
	{ WWT_FLATBTN,			1,	205,	228,	121,	144,	SPR_LOCATE,						STR_LOCATE_SUBJECT_TIP },			// locate
	{ WWT_FLATBTN,			1,	205,	228,	145,	168,	SPR_RENAME,						STR_NAME_PARK_TIP },				// rename
	{ WIDGETS_END },
};

static rct_widget window_park_rating_widgets[] = {
	{ WWT_FRAME,			0,	0,		229,	0,		223,	0x0FFFFFFFF,					STR_NONE },							// panel / background
	{ WWT_CAPTION,			0,	1,		228,	1,		14,		0x361,							STR_WINDOW_TITLE_TIP },				// title bar
	{ WWT_CLOSEBOX,			0,	217,	227,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP },				// close x button
	{ WWT_IMGBTN,			1,	0,		229,	43,		173,	0x0FFFFFFFF,					STR_NONE },							// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_PARK_ENTRANCE_TAB_TIP },		// tab 1
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,						STR_PARK_RATING_TAB_TIP },			// tab 2
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_PARK_GUESTS_TAB_TIP },			// tab 3
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_PARK_PRICE_TAB_TIP },			// tab 4
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_PARK_STATS_TAB_TIP },			// tab 5
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_PARK_OBJECTIVE_TAB_TIP },		// tab 6
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_PARK_AWARDS_TAB_TIP },			// tab 7
	{ WIDGETS_END },
};

static rct_widget window_park_guests_widgets[] = {
	{ WWT_FRAME,			0,	0,		229,	0,		223,	0x0FFFFFFFF,					STR_NONE },							// panel / background
	{ WWT_CAPTION,			0,	1,		228,	1,		14,		0x361,							STR_WINDOW_TITLE_TIP },				// title bar
	{ WWT_CLOSEBOX,			0,	217,	227,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP },				// close x button
	{ WWT_IMGBTN,			1,	0,		229,	43,		173,	0x0FFFFFFFF,					STR_NONE },							// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_PARK_ENTRANCE_TAB_TIP },		// tab 1
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,						STR_PARK_RATING_TAB_TIP },			// tab 2
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_PARK_GUESTS_TAB_TIP },			// tab 3
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_PARK_PRICE_TAB_TIP },			// tab 4
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_PARK_STATS_TAB_TIP },			// tab 5
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_PARK_OBJECTIVE_TAB_TIP },		// tab 6
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_PARK_AWARDS_TAB_TIP },			// tab 7
	{ WIDGETS_END },
};

static rct_widget window_park_price_widgets[] = {
	{ WWT_FRAME,			0,	0,		229,	0,		223,	0x0FFFFFFFF,					STR_NONE },							// panel / background
	{ WWT_CAPTION,			0,	1,		228,	1,		14,		0x361,							STR_WINDOW_TITLE_TIP },				// title bar
	{ WWT_CLOSEBOX,			0,	217,	227,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP },				// close x button
	{ WWT_IMGBTN,			1,	0,		229,	43,		173,	0x0FFFFFFFF,					STR_NONE },							// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_PARK_ENTRANCE_TAB_TIP },		// tab 1
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,						STR_PARK_RATING_TAB_TIP },			// tab 2
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_PARK_GUESTS_TAB_TIP },			// tab 3
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_PARK_PRICE_TAB_TIP },			// tab 4
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_PARK_STATS_TAB_TIP },			// tab 5
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_PARK_OBJECTIVE_TAB_TIP },		// tab 6
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_PARK_AWARDS_TAB_TIP },			// tab 7


	{ WWT_24,				1,	7,		146,	50,		61,		STR_ADMISSION_PRICE,			STR_NONE },							// 
	{ WWT_15,				1,	147,	222,	50,		61,		0x595,							STR_NONE },							// price
	{ WWT_DROPDOWN_BUTTON,	1,	211,	221,	51,		55,		STR_NUMERIC_UP,					STR_NONE },							// increase price
	{ WWT_DROPDOWN_BUTTON,	1,	211,	221,	56,		60,		STR_NUMERIC_DOWN,				STR_NONE },							// decrease price
	{ WIDGETS_END },
};

static rct_widget window_park_stats_widgets[] = {
	{ WWT_FRAME,			0,	0,		229,	0,		223,	0x0FFFFFFFF,					STR_NONE },							// panel / background
	{ WWT_CAPTION,			0,	1,		228,	1,		14,		0x361,							STR_WINDOW_TITLE_TIP },				// title bar
	{ WWT_CLOSEBOX,			0,	217,	227,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP },				// close x button
	{ WWT_IMGBTN,			1,	0,		229,	43,		173,	0x0FFFFFFFF,					STR_NONE },							// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_PARK_ENTRANCE_TAB_TIP },		// tab 1
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,						STR_PARK_RATING_TAB_TIP },			// tab 2
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_PARK_GUESTS_TAB_TIP },			// tab 3
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_PARK_PRICE_TAB_TIP },			// tab 4
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_PARK_STATS_TAB_TIP },			// tab 5
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_PARK_OBJECTIVE_TAB_TIP },		// tab 6
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_PARK_AWARDS_TAB_TIP },			// tab 7
	{ WIDGETS_END },
};

static rct_widget window_park_objective_widgets[] = {
	{ WWT_FRAME,			0,	0,		229,	0,		223,	0x0FFFFFFFF,					STR_NONE },							// panel / background
	{ WWT_CAPTION,			0,	1,		228,	1,		14,		0x361,							STR_WINDOW_TITLE_TIP },				// title bar
	{ WWT_CLOSEBOX,			0,	217,	227,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP },				// close x button
	{ WWT_IMGBTN,			1,	0,		229,	43,		173,	0x0FFFFFFFF,					STR_NONE },							// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_PARK_ENTRANCE_TAB_TIP },		// tab 1
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,						STR_PARK_RATING_TAB_TIP },			// tab 2
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_PARK_GUESTS_TAB_TIP },			// tab 3
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_PARK_PRICE_TAB_TIP },			// tab 4
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_PARK_STATS_TAB_TIP },			// tab 5
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_PARK_OBJECTIVE_TAB_TIP },		// tab 6
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_PARK_AWARDS_TAB_TIP },			// tab 7

	{ WWT_DROPDOWN_BUTTON,	1,	7,		222,	209,	220,	STR_ENTER_NAME_INTO_SCENARIO_CHART,			STR_NONE },				// enter name
	{ WIDGETS_END },
};

static rct_widget window_park_awards_widgets[] = {
	{ WWT_FRAME,			0,	0,		229,	0,		223,	0x0FFFFFFFF,					STR_NONE },							// panel / background
	{ WWT_CAPTION,			0,	1,		228,	1,		14,		0x361,							STR_WINDOW_TITLE_TIP },				// title bar
	{ WWT_CLOSEBOX,			0,	217,	227,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP },				// close x button
	{ WWT_IMGBTN,			1,	0,		229,	43,		173,	0x0FFFFFFFF,					STR_NONE },							// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_PARK_ENTRANCE_TAB_TIP },		// tab 1
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,						STR_PARK_RATING_TAB_TIP },			// tab 2
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_PARK_GUESTS_TAB_TIP },			// tab 3
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_PARK_PRICE_TAB_TIP },			// tab 4
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_PARK_STATS_TAB_TIP },			// tab 5
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_PARK_OBJECTIVE_TAB_TIP },		// tab 6
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_PARK_AWARDS_TAB_TIP },			// tab 7
	{ WIDGETS_END },
};

static rct_widget *window_park_page_widgets[] = {
	window_park_entrance_widgets,
	window_park_rating_widgets,
	window_park_guests_widgets,
	window_park_price_widgets,
	window_park_stats_widgets,
	window_park_objective_widgets,
	window_park_awards_widgets
};

#pragma endregion

#pragma region Events

static void window_park_emptysub() { }

static void window_park_entrance_close();
static void window_park_entrance_mouseup();
static void window_park_entrance_resize();
static void window_park_entrance_mousedown();
static void window_park_entrance_dropdown();
static void window_park_entrance_update();
static void window_park_entrance_toolupdate();
static void window_park_entrance_tooldown();
static void window_park_entrance_tooldrag();
static void window_park_entrance_toolabort();
static void window_park_entrance_textinput();
static void window_park_entrance_invalidate();
static void window_park_entrance_paint();

static void window_park_rating_mouseup();
static void window_park_rating_update();
static void window_park_rating_invalidate();
static void window_park_rating_paint();

static void window_park_guests_mouseup();
static void window_park_guests_update();
static void window_park_guests_invalidate();
static void window_park_guests_paint();

static void window_park_price_mouseup();
static void window_park_price_mousedown();
static void window_park_price_update();
static void window_park_price_invalidate();
static void window_park_price_paint();

static void window_park_stats_mouseup();
static void window_park_stats_update();
static void window_park_stats_invalidate();
static void window_park_stats_paint();

static void window_park_objective_mouseup();
static void window_park_objective_update();
static void window_park_objective_invalidate();
static void window_park_objective_paint();

static void window_park_awards_mouseup();
static void window_park_awards_update();
static void window_park_awards_invalidate();
static void window_park_awards_paint();

static uint32 window_park_entrance_events[] = {
	window_park_entrance_close,
	window_park_entrance_mouseup,
	window_park_entrance_resize,
	window_park_entrance_mousedown,
	window_park_entrance_dropdown,
	window_park_emptysub,
	window_park_entrance_update,
	window_park_emptysub,
	window_park_emptysub,
	window_park_entrance_toolupdate,
	window_park_entrance_tooldown,
	window_park_entrance_tooldrag,
	window_park_emptysub,
	window_park_entrance_toolabort,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_entrance_textinput,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_entrance_invalidate,
	window_park_entrance_paint,
	window_park_emptysub
};

static uint32 window_park_rating_events[] = {
	window_park_emptysub,
	window_park_rating_mouseup,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_rating_update,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_rating_invalidate,
	window_park_rating_paint,
	window_park_emptysub
};

static uint32 window_park_guests_events[] = {
	window_park_emptysub,
	window_park_guests_mouseup,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_guests_update,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_guests_invalidate,
	window_park_guests_paint,
	window_park_emptysub
};

static uint32 window_park_price_events[] = {
	window_park_emptysub,
	window_park_price_mouseup,
	window_park_emptysub,
	window_park_price_mousedown,
	window_park_emptysub,
	window_park_emptysub,
	window_park_price_update,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_price_invalidate,
	window_park_price_paint,
	window_park_emptysub
};

static uint32 window_park_stats_events[] = {
	window_park_emptysub,
	window_park_stats_mouseup,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_stats_update,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_stats_invalidate,
	window_park_stats_paint,
	window_park_emptysub
};

static uint32 window_park_objective_events[] = {
	window_park_emptysub,
	window_park_objective_mouseup,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_objective_update,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_objective_invalidate,
	window_park_objective_paint,
	window_park_emptysub
};

static uint32 window_park_awards_events[] = {
	window_park_emptysub,
	window_park_awards_mouseup,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_awards_update,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_emptysub,
	window_park_awards_invalidate,
	window_park_awards_paint,
	window_park_emptysub
};

static uint32 *window_park_page_events[] = {
	window_park_entrance_events,
	window_park_rating_events,
	window_park_guests_events,
	window_park_price_events,
	window_park_stats_events,
	window_park_objective_events,
	window_park_awards_events
};

#pragma endregion

#pragma region Enabled widgets

static uint32 window_park_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6) |
	(1 << WIDX_TAB_7) |
	(1 << WIDX_OPEN_OR_CLOSE) |
	(1 << WIDX_BUY_LAND_RIGHTS) |
	(1 << WIDX_BUY_CONSTRUCTION_RIGHTS) |
	(1 << WIDX_LOCATE) |
	(1 << WIDX_RENAME),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6) |
	(1 << WIDX_TAB_7),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6) |
	(1 << WIDX_TAB_7),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6) |
	(1 << WIDX_TAB_7) |
	(1 << WIDX_INCREASE_PRICE) | 
	(1 << WIDX_DECREASE_PRICE),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6) |
	(1 << WIDX_TAB_7),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6) |
	(1 << WIDX_TAB_7) |
	(1 << WIDX_ENTER_NAME),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6) |
	(1 << WIDX_TAB_7)
};

#pragma endregion

static void window_park_init_viewport(rct_window *w);
static void window_park_scroll_to_viewport(rct_window *w);
static void window_park_set_page(rct_window *w, int page);
static void window_park_anchor_border_widgets(rct_window *w);
static void window_park_align_tabs(rct_window *w);
static void window_park_set_pressed_tab(rct_window *w);
static void window_park_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

/**
 * 
 *  rct2: 0x00667F11
 */
rct_window *window_park_open()
{
	rct_window* w;

	{
		int eax, ebx, ecx, edx, esi, edi, ebp;
		ebx = 0xAE00E6;
		ecx = 0x4001B;
		edx = 0x0097BB6C;
		RCT2_CALLFUNC_X(0x006EA9B1, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
		w = esi;
	}
	w->widgets = window_park_entrance_widgets;
	w->enabled_widgets = window_park_page_enabled_widgets[WINDOW_PARK_PAGE_ENTRANCE];
	w->number = 0;
	w->page = WINDOW_PARK_PAGE_ENTRANCE;
	w->var_482 = 0;
	w->var_48E = 0;
	w->var_490 = -1;
	w->var_48C = -1;
	w->var_492 = 0;
	RCT2_CALLPROC_X(0x00667F8B, 0, 0, 0, 0, w, 0, 0);
	w->colours[0] = 1;
	w->colours[1] = 19;
	w->colours[2] = 19;

	return w;
}

#pragma region Entrance page

/**
 * 
 *  rct2: 0x00667C48
 */
void window_park_entrance_open()
{
	rct_window* window;

	window = window_bring_to_front_by_id(WC_PARK_INFORMATION, 0);
	if (window == NULL) {
		window = window_park_open();
		window->var_482 = -1;
		window->var_484 = -1;
	}

	window->page = WINDOW_PARK_PAGE_ENTRANCE;
	window_invalidate(window);
	window->widgets = window_park_entrance_widgets;
	window->enabled_widgets = window_park_page_enabled_widgets[WINDOW_PARK_PAGE_ENTRANCE];
	window->event_handlers = window_park_entrance_events;
	window->pressed_widgets = 0;
	window_init_scroll_widgets(window);
	window_park_init_viewport(window);
}

/**
 * 
 *  rct2: 0x0066860C
 */
static void window_park_entrance_close()
{
	rct_window *w;

	__asm mov w, esi

	if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3))
		if (w->classification == RCT2_GLOBAL(0x009DE544, rct_windowclass) && w->number == RCT2_GLOBAL(0x009DE542, rct_windownumber))
			RCT2_CALLPROC_EBPSAFE(0x006EE281);
}

/**
 *
 *  rct2: 0x0066817C
 */
static void window_park_entrance_mouseup()
{
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
		window_park_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_BUY_LAND_RIGHTS:
		RCT2_CALLPROC_X(0x006682F7, 0, 0, 0, widgetIndex, w, 0, 0);
		break;
	case WIDX_BUY_CONSTRUCTION_RIGHTS:
		RCT2_CALLPROC_X(0x00668393, 0, 0, 0, widgetIndex, w, 0, 0);
		break;
	case WIDX_LOCATE:
		window_park_scroll_to_viewport(w);
		break;
	case WIDX_RENAME:
		RCT2_GLOBAL(0x013CE962, uint32) = RCT2_GLOBAL(0x013573D8, uint32);
		window_show_textinput(w, WIDX_RENAME, STR_PARK_NAME, STR_ENTER_PARK_NAME, RCT2_GLOBAL(0x013573D4, uint32));
		break;
	}
}

/**
 *
 *  rct2: 0x00668637
 */
static void window_park_entrance_resize()
{
	rct_window *w;

	__asm mov w, esi

	w->flags |= WF_RESIZABLE;
	w->min_width = 230;
	w->max_width = 230 * 2;
	w->min_height = 174 + 9;
	w->max_height = 174 * 2;
	if (w->width < w->min_width) {
		w->width = w->min_width;
		window_invalidate(w);
	}
	if (w->height < w->min_height) {
		w->height = w->min_height;
		window_invalidate(w);
	}
	if (w->width > w->max_width) {
		w->width = w->max_width;
		window_invalidate(w);
	}
	if (w->height > w->max_height) {
		w->height = w->max_height;
		window_invalidate(w);
	}

	window_park_init_viewport(w);
}

/**
 *
 *  rct2: 0x006681BF
 */
static void window_park_entrance_mousedown()
{
	short widgetIndex;
	rct_window *w;
	rct_widget *widget;

	__asm mov widgetIndex, dx
	__asm mov w, esi
	__asm mov widget, edi

	if (widgetIndex == WIDX_OPEN_OR_CLOSE) {
		gDropdownItemsFormat[0] = 1142;
		gDropdownItemsFormat[1] = 1142;
		gDropdownItemsArgs[0] = STR_CLOSE_PARK;
		gDropdownItemsArgs[1] = STR_OPEN_PARK;
		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[1],
			0,
			2
		);

		if (park_is_open()) {
			RCT2_GLOBAL(0x009DEBA2, sint16) = 0;
			gDropdownItemsChecked |= (1 << 1);
		} else {
			RCT2_GLOBAL(0x009DEBA2, sint16) = 1;
			gDropdownItemsChecked |= (1 << 0);
		}
	}
}

/**
 * 
 *  rct2: 0x006682B8
 */
static void window_park_entrance_dropdown()
{
	short widgetIndex, dropdownIndex;

	__asm mov dropdownIndex, ax
	__asm mov widgetIndex, dx

	if (widgetIndex == WIDX_OPEN_OR_CLOSE) {
		if (dropdownIndex == -1)
			dropdownIndex = RCT2_GLOBAL(0x009DEBA2, sint16);
		if (dropdownIndex != 0) {
			dropdownIndex &= 0x00FF;
			dropdownIndex |= 0x0100;
			RCT2_GLOBAL(0x0141E9AE, uint16) = 1724;
		} else {
			dropdownIndex &= 0x00FF;
			RCT2_GLOBAL(0x0141E9AE, uint16) = 1723;
		}
		RCT2_CALLPROC_X(0x006677F2, 0, 1, 0, dropdownIndex, 34, 0, 0);
	}
}

/**
 * 
 *  rct2: 0x006686B5
 */
static void window_park_entrance_update()
{
	rct_window *w;

	__asm mov w, esi

	w->var_48E++;
	window_invalidate_by_id(w->classification, 1179);
}

/**
 * 
 *  rct2: 0x006681D1
 */
static void window_park_entrance_toolupdate()
{
	int x, y;
	short widgetIndex;
	rct_window *w, *mainWindow;

	__asm mov x, eax
	__asm mov y, ebx
	__asm mov widgetIndex, dx
	__asm mov w, esi

	if (widgetIndex == WIDX_BUY_LAND_RIGHTS) {
		RCT2_CALLPROC_X(0x0068AAE1, x, y, 0, 0, w, 0, 0);
		RCT2_GLOBAL(0x009DE58A, uint16) &= 0xFFFE;
		screen_pos_to_map_pos(&x, &y);
		if (x != 0x8000) {
			RCT2_GLOBAL(0x009DE58A, uint16) |= 1;
			RCT2_GLOBAL(0x009DE594, uint16) = 4;
			RCT2_GLOBAL(0x009DE58C, uint16) = x;
			RCT2_GLOBAL(0x009DE58E, uint16) = x;
			RCT2_GLOBAL(0x009DE590, uint16) = y;
			RCT2_GLOBAL(0x009DE592, uint16) = y;
			RCT2_CALLPROC_X(0x0068AAE1, x, y, 0, 0, w, 0, 0);
		}
	}
}

/**
 * 
 *  rct2: 0x006681E6
 */
static void window_park_entrance_tooldown()
{
	short x, y, widgetIndex;
	rct_window *w, *mainWindow;

	__asm mov x, ax
	__asm mov y, bx
	__asm mov widgetIndex, dx
	__asm mov w, esi

	RCT2_CALLPROC_X(0x006681E6, x, y, 0, widgetIndex, w, 0, 0);
}

/**
 * 
 *  rct2: 0x006681FB
 */
static void window_park_entrance_tooldrag()
{
	short x, y, widgetIndex;
	rct_window *w, *mainWindow;

	__asm mov x, ax
	__asm mov y, bx
	__asm mov widgetIndex, dx
	__asm mov w, esi

	RCT2_CALLPROC_X(0x006681FB, x, y, 0, widgetIndex, w, 0, 0);
}

/**
 * 
 *  rct2: 0x0066822A
 */
static void window_park_entrance_toolabort()
{
	short widgetIndex;
	rct_window *w, *mainWindow;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	if (widgetIndex == WIDX_BUY_LAND_RIGHTS) {
		hide_gridlines();
		hide_land_rights();
	} else if (widgetIndex == WIDX_BUY_CONSTRUCTION_RIGHTS) {
		hide_gridlines();
		hide_construction_rights();
	}
}

/**
 * 
 *  rct2: 0x0066848B
 */
static void window_park_entrance_textinput()
{
	uint8 result;
	short widgetIndex;
	rct_window *w;
	char *text;

	__asm mov result, cl
	__asm mov widgetIndex, dx
	__asm mov w, esi
	__asm mov text, edi

	if (widgetIndex == WIDX_RENAME) {
		if (result) {
			RCT2_GLOBAL(0x0141E9AE, uint16) = STR_CANT_RENAME_PARK;
			RCT2_CALLPROC_X(0x006677F2, 1, 1, 0, *((int*)(text + 0)), '!', *((int*)(text + 8)), *((int*)(text + 4)));
			RCT2_CALLPROC_X(0x006677F2, 2, 1, 0, *((int*)(text + 12)), '!', *((int*)(text + 20)), *((int*)(text + 16)));
			RCT2_CALLPROC_X(0x006677F2, 0, 1, 0, *((int*)(text + 24)), '!', *((int*)(text + 32)), *((int*)(text + 28)));
		}
	}
}

/**
 * 
 *  rct2: 0x00667FDC
 */
static void window_park_entrance_invalidate()
{
	int i;
	rct_window *w;

	__asm mov w, esi

	w->widgets = window_park_page_widgets[w->page];
	window_init_scroll_widgets(w);

	window_park_set_pressed_tab(w);

	// Set open / close park button state
	RCT2_GLOBAL(0x013CE952, uint16) = RCT2_GLOBAL(0x013573D4, uint16);
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = RCT2_GLOBAL(0x013573D8, uint32);
	window_park_entrance_widgets[WIDX_OPEN_OR_CLOSE].image = park_is_open() ? SPR_OPEN : SPR_CLOSED;

	// Only allow closing of park for guest / rating objective
	if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) == OBJECTIVE_GUESTS_AND_RATING)
		w->disabled_widgets |= (1 << WIDX_OPEN_OR_CLOSE);

	window_park_align_tabs(w);
	window_park_anchor_border_widgets(w);

	// Anchor entrance page specific widgets
	window_park_entrance_widgets[WIDX_VIEWPORT].right = w->width - 26;
	window_park_entrance_widgets[WIDX_VIEWPORT].bottom = w->height - 14;
	window_park_entrance_widgets[WIDX_STATUS].right = w->width - 26;
	window_park_entrance_widgets[WIDX_STATUS].top = w->height - 13;
	window_park_entrance_widgets[WIDX_STATUS].bottom = w->height - 3;
	for (i = WIDX_OPEN_OR_CLOSE; i <= WIDX_RENAME; i++) {
		window_park_entrance_widgets[i].left = w->width - 25;
		window_park_entrance_widgets[i].right = w->width - 2;
	}

	// Only allow closing of park and purchase of land when there is money
	if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_FLAGS, uint32) & GAME_FLAGS_NO_MONEY) {
		window_park_entrance_widgets[WIDX_OPEN_OR_CLOSE].type = WWT_EMPTY;
		window_park_entrance_widgets[WIDX_BUY_LAND_RIGHTS].type = WWT_EMPTY;
		window_park_entrance_widgets[WIDX_BUY_CONSTRUCTION_RIGHTS].type = WWT_EMPTY;
	} else {
		window_park_entrance_widgets[WIDX_OPEN_OR_CLOSE].type = WWT_FLATBTN;
		window_park_entrance_widgets[WIDX_BUY_LAND_RIGHTS].type = WWT_FLATBTN;
		window_park_entrance_widgets[WIDX_BUY_CONSTRUCTION_RIGHTS].type = WWT_FLATBTN;
	}
}

/**
 * 
 *  rct2: 0x006680D0
 */
static void window_park_entrance_paint()
{
	int i, x, y;
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_widget *labelWidget;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);
	window_park_draw_tab_images(dpi, w);

	// Draw viewport
	if (w->viewport != NULL) {
		window_draw_viewport(dpi, w);
		if (w->viewport->flags & VIEWPORT_FLAG_SOUND_ON)
			gfx_draw_sprite(dpi, SPR_HEARING_VIEWPORT, w->x + 2, w->y + 2);
	}

	// Draw park closed / open label
	RCT2_GLOBAL(0x013CE952, uint16) = park_is_open() ? STR_PARK_OPEN : STR_PARK_CLOSED;

	labelWidget = &window_park_entrance_widgets[WIDX_STATUS];
	gfx_draw_string_centred_clipped(
		dpi,
		1191,
		0x013CE952,
		0,
		w->x + (labelWidget->left + labelWidget->right) / 2,
		w->y + labelWidget->top,
		labelWidget->right - labelWidget->left
	);
}

/**
 * 
 *  rct2: 0x00669B55
 */
static void window_park_init_viewport(rct_window *w)
{
	int i, x, y, z, r, xy, zr, viewportFlags;
	rct_viewport *viewport;
	rct_widget *viewportWidget;

	if (w->page != WINDOW_PARK_PAGE_ENTRANCE)
		return;

	for (i = 0; i < 4; i++) {
		if (RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i] != 0x8000) {
			x = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i] + 16;
			y = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Y, uint16)[i] + 16;
			z = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Z, uint16)[i] + 32;
			r = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8);

			xy = 0x40000000 | (y << 16) | x;
			zr = (z << 16) | (r << 8);
			break;
		}
	}

	if (w->viewport == NULL) {
		viewportFlags = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES) ? VIEWPORT_FLAG_GRIDLINES : 0;
	} else {
		// if (w->var_482 == x && w->var_484 == y && w->var_486 == z && (uint16)w->var_488 >> 8 == r)
		//	return;

		viewport = w->viewport;
		w->viewport = NULL;
		viewportFlags = viewport->flags;
		viewport->width = 0;
		viewport_update_pointers();
	}

	// Call invalidate event
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, w, 0, 0);

	w->var_482 = x;
	w->var_484 = y;
	w->var_486 = z;
	w->var_488 = r << 8;

	if (zr != 0xFFFF) {
		// Create viewport
		if (w->viewport == NULL) {
			viewportWidget = &window_park_entrance_widgets[WIDX_VIEWPORT];
			viewport_create(
				w,
				w->x + viewportWidget->left + 1,
				w->y + viewportWidget->top + 1,
				(viewportWidget->right - viewportWidget->left) - 2,
				(viewportWidget->bottom - viewportWidget->top) - 2,
				zr,
				xy
			);
			w->flags |= (1 << 2);
			window_invalidate(w);
		}
	}

	if (w->viewport != NULL)
		w->viewport->flags = viewportFlags;
	window_invalidate(w);
}

static void window_park_scroll_to_viewport(rct_window *w)
{
	int x, y, z;
	rct_window *mainWindow;

	if (w->viewport == NULL || *((sint32*)&w->var_482) == -1)
		return;

	if (*((uint32*)&w->var_486) & 0x80000000) {
		rct_sprite *sprite = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[w->var_482]);
		x = sprite->unknown.x;
		y = sprite->unknown.y;
		z = sprite->unknown.z;
	} else {
		x = w->var_482;
		y = w->var_484;
		z = w->var_486;
	}

	mainWindow = window_get_main();
	if (mainWindow != NULL)
		window_scroll_to_location(mainWindow, x, y, z);
}

#pragma endregion

#pragma region Rating page

/**
 *
 *  rct2: 0x00668A06
 */
static void window_park_rating_mouseup()
{
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	if (widgetIndex == WIDX_CLOSE)
		window_close(w);
	else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
		window_park_set_page(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x00668A21
 */
static void window_park_rating_update()
{
	rct_window *w;

	__asm mov w, esi

	w->var_48E++;
	widget_invalidate(w->classification, w->number, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x006686CB
 */
static void window_park_rating_invalidate()
{
	int i;
	rct_window *w;
	rct_widget **widgets;

	__asm mov w, esi

	widgets = window_park_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_park_set_pressed_tab(w);

	RCT2_GLOBAL(0x013CE952, uint16) = RCT2_GLOBAL(0x013573D4, uint16);
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = RCT2_GLOBAL(0x013573D8, uint32);

	window_park_align_tabs(w);
	window_park_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x0066875D
 */
static void window_park_rating_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);
	window_park_draw_tab_images(dpi, w);
}

#pragma endregion

#pragma region Guests page

/**
 *
 *  rct2: 0x00668DEB
 */
static void window_park_guests_mouseup()
{
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	if (widgetIndex == WIDX_CLOSE)
		window_close(w);
	else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
		window_park_set_page(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x00668E06
 */
static void window_park_guests_update()
{
	rct_window *w;

	__asm mov w, esi

	w->var_48E++;
	w->var_492 = (w->var_492 + 1) % 24;
	widget_invalidate(w->classification, w->number, WIDX_TAB_3);
}

/**
 *
 *  rct2: 0x00668AB0
 */
static void window_park_guests_invalidate()
{
	int i;
	rct_window *w;
	rct_widget **widgets;

	__asm mov w, esi

	widgets = window_park_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_park_set_pressed_tab(w);

	RCT2_GLOBAL(0x013CE952, uint16) = RCT2_GLOBAL(0x013573D4, uint16);
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = RCT2_GLOBAL(0x013573D8, uint32);

	window_park_align_tabs(w);
	window_park_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x00668B42
 */
static void window_park_guests_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);
	window_park_draw_tab_images(dpi, w);
}

#pragma endregion

#pragma region Price page

/**
 *
 *  rct2: 0x00669011
 */
static void window_park_price_mouseup()
{
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	if (widgetIndex == WIDX_CLOSE)
		window_close(w);
	else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
		window_park_set_page(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0066902C
 */
static void window_park_price_mousedown()
{
	int newFee;
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
		window_park_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_INCREASE_PRICE:
		newFee = min(1000, RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, uint16) + 10);
		RCT2_CALLPROC_X(0x006677F2, 0, 1, 0, 0, 39, newFee, 0);
		break;
	case WIDX_DECREASE_PRICE:
		newFee = max(0, RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, uint16) - 10);
		RCT2_CALLPROC_X(0x006677F2, 0, 1, 0, 0, 39, newFee, 0);
		break;
	}
}

/**
 *
 *  rct2: 0x00669077
 */
static void window_park_price_update()
{
	rct_window *w;

	__asm mov w, esi

	w->var_48E++;
	widget_invalidate(w->classification, w->number, WIDX_TAB_4);
}

/**
 *
 *  rct2: 0x00668EAD
 */
static void window_park_price_invalidate()
{
	int i;
	rct_window *w;
	rct_widget **widgets;

	__asm mov w, esi

	widgets = window_park_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_park_set_pressed_tab(w);

	RCT2_GLOBAL(0x013CE952, uint16) = RCT2_GLOBAL(0x013573D4, uint16);
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = RCT2_GLOBAL(0x013573D8, uint32);

	if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_FLAGS, uint32) & GAME_FLAGS_PARK_FREE_ENTRY) {
		window_park_price_widgets[WIDX_PRICE].type = WWT_12;
		window_park_price_widgets[WIDX_INCREASE_PRICE].type = WWT_EMPTY;
		window_park_price_widgets[WIDX_DECREASE_PRICE].type = WWT_EMPTY;
	} else {
		window_park_price_widgets[WIDX_PRICE].type = WWT_15;
		window_park_price_widgets[WIDX_INCREASE_PRICE].type = WWT_DROPDOWN_BUTTON;
		window_park_price_widgets[WIDX_DECREASE_PRICE].type = WWT_DROPDOWN_BUTTON;
	}

	RCT2_GLOBAL(0x013CE952 + 6, uint32) = RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, uint16);
	window_park_price_widgets[WIDX_PRICE].image = RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, uint16) == 0 ? STR_FREE : 1429;

	window_park_align_tabs(w);
	window_park_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x00668F99
 */
static void window_park_price_paint()
{
	int x, y;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);
	window_park_draw_tab_images(dpi, w);

	x = w->x + window_park_price_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	y = w->y + window_park_price_widgets[WIDX_PAGE_BACKGROUND].top + 30;

	gfx_draw_string_left(dpi, STR_TOTAL_ADMISSIONS, RCT2_ADDRESS_TOTAL_ADMISSIONS, 0, x, y);
	y += 10;
	gfx_draw_string_left(dpi, STR_INCOME_FROM_ADMISSIONS, RCT2_ADDRESS_INCOME_FROM_ADMISSIONS, 0, x, y);
}

#pragma endregion

#pragma region Stats page

/**
 *
 *  rct2: 0x0066928C
 */
static void window_park_stats_mouseup()
{
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	if (widgetIndex == WIDX_CLOSE)
		window_close(w);
	else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
		window_park_set_page(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x006692A8
 */
static void window_park_stats_update()
{
	int i;
	rct_window *w;

	__asm mov w, esi

	w->var_48E++;
	widget_invalidate(w->classification, w->number, WIDX_TAB_5);

	// Invalidate ride count if changed
	i = ride_get_count();
	if (w->var_490 != i) {
		w->var_490 = i;
		widget_invalidate(w->classification, w->number, WIDX_PAGE_BACKGROUND);
	}

	// Invalidate number of staff if changed
	i = peep_get_staff_count();
	if (w->var_48C != i) {
		w->var_48C = i;
		widget_invalidate(w->classification, w->number, WIDX_PAGE_BACKGROUND);
	}
}

/**
 *
 *  rct2: 0x00669106
 */
static void window_park_stats_invalidate()
{
	int i;
	rct_window *w;
	rct_widget **widgets;

	__asm mov w, esi

	widgets = window_park_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_park_set_pressed_tab(w);

	RCT2_GLOBAL(0x013CE952, uint16) = RCT2_GLOBAL(0x013573D4, uint16);
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = RCT2_GLOBAL(0x013573D8, uint32);

	window_park_align_tabs(w);
	window_park_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x00669198
 */
static void window_park_stats_paint()
{
	int x, y, parkSize, stringIndex;
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_award *award;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);
	window_park_draw_tab_images(dpi, w);

	x = w->x + window_park_awards_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	y = w->y + window_park_awards_widgets[WIDX_PAGE_BACKGROUND].top + 4;

	// Draw park size
	parkSize = RCT2_GLOBAL(0x013580EA, sint16) * 10;
	stringIndex = STR_PARK_SIZE_METRIC_LABEL;
	if (!RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, uint8)) {
		stringIndex = STR_PARK_SIZE_IMPERIAL_LABEL;
		parkSize = squaredmetres_to_squaredfeet(parkSize);
	}
	RCT2_GLOBAL(0x013CE952, uint32) = parkSize;
	gfx_draw_string_left(dpi, stringIndex, 0x013CE952, 0, x, y);
	y += 10;

	// Draw number of rides / attractions
	if (w->var_490 != -1) {
		RCT2_GLOBAL(0x013CE952, uint32) = w->var_490;
		gfx_draw_string_left(dpi, STR_NUMBER_OF_RIDES_LABEL, 0x013CE952, 0, x, y);
	}
	y += 10;

	// Draw number of staff
	if (w->var_48C != -1) {
		RCT2_GLOBAL(0x013CE952, uint32) = w->var_48C;
		gfx_draw_string_left(dpi, STR_STAFF_LABEL, 0x013CE952, 0, x, y);
	}
	y += 10;

	// Draw number of guests in park
	gfx_draw_string_left(dpi, STR_GUESTS_IN_PARK_LABEL, RCT2_ADDRESS_GUESTS_IN_PARK, 0, x, y);
}

#pragma endregion

#pragma region Objective page

/**
 * 
 *  rct2: 0x00667E57
 */
void window_park_objective_open()
{
	rct_window* window;

	window = window_bring_to_front_by_id(WC_PARK_INFORMATION, 0);
	if (window == NULL) {
		window = window_park_open();
		window->var_482 = -1;
		window->var_484 = -1;
	}

	if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3))
		if (window->classification == RCT2_GLOBAL(0x009DE544, rct_windowclass) && window->number == RCT2_GLOBAL(0x009DE542, rct_windownumber))
			RCT2_CALLPROC_EBPSAFE(0x006EE281);

	window->viewport = NULL;
	window->page = WINDOW_PARK_PAGE_OBJECTIVE;
	window_invalidate(window);
	window->widgets = window_park_objective_widgets;
	window->enabled_widgets = window_park_page_enabled_widgets[WINDOW_PARK_PAGE_OBJECTIVE];
	window->var_020 = 0;
	window->event_handlers = window_park_objective_events;
	window_init_scroll_widgets(window);
	window->x = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) / 2 - 115;
	window->y = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) / 2 - 87;
	window_invalidate(window);
}

/**
 * 
 *  rct2: 0x006695AA
 */
static void window_park_objective_mouseup()
{
	int tabIndex;
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
		window_park_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_ENTER_NAME:
		RCT2_CALLPROC_X(0x006EE308, 2791, 2792, 0, 0, 0, 0, 0);
		break;
	}
}

/**
 * 
 *  rct2: 0x0066966C
 */
static void window_park_objective_update()
{
	rct_window *w;

	__asm mov w, esi

	w->var_48E++;
	widget_invalidate(w->classification, w->number, WIDX_TAB_6);
}

/**
 * 
 *  rct2: 0x006693B2
 */
static void window_park_objective_invalidate()
{
	int i;
	rct_window *w;

	__asm mov w, esi

	window_park_set_pressed_tab(w);

	// Set window title arguments
	*((short*)0x013CE952) = RCT2_GLOBAL(0x013573D4, uint16);
	*((short*)0x013CE954) = RCT2_GLOBAL(0x013573D8, uint32);

	// 
	if (RCT2_GLOBAL(0x013573E4, uint32) & 0x02)
		window_park_objective_widgets[WIDX_ENTER_NAME].type = WWT_DROPDOWN_BUTTON;
	else
		window_park_objective_widgets[WIDX_ENTER_NAME].type = WWT_EMPTY;

	window_park_align_tabs(w);
	window_park_anchor_border_widgets(w);
}

/**
 * 
 *  rct2: 0x0066945C
 */
static void window_park_objective_paint()
{
	int i, x, y;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);
	window_park_draw_tab_images(dpi, w);

	// Scenario description
	x = w->x + window_park_objective_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	y = w->y + window_park_objective_widgets[WIDX_PAGE_BACKGROUND].top + 7;
	strcpy((char*)0x009BC677, RCT2_ADDRESS(RCT2_ADDRESS_SCENARIO_DETAILS, char));
	*((short*)0x013CE952) = 3165;
	y += gfx_draw_string_left_wrapped(dpi, (void*)0x013CE952, x, y, 222, 1191, 0);
	y += 5;

	// Your objective:
	gfx_draw_string_left(dpi, 2384, NULL, 0, x, y);
	y += 10;

	// Objective
	*((short*)0x013CE952) = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16);
	*((short*)0x013CE954) = date_get_total_months(MONTH_OCTOBER, RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8));
	*((int*)0x013CE956) = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, sint32);

	gfx_draw_string_left_wrapped(dpi, (void*)0x013CE952, x, y, 221, 2385 + RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8), 0);
	y += 5;
	
	// Objective outcome
	if (RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, uint32) != 0x80000000) {
		if (RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, uint32) == 0x80000001) {
			// Objective failed
			gfx_draw_string_left_wrapped(dpi, NULL, x, y, 222, 2789, 0);
		} else {
			// Objective completed
			*((int*)0x013CE952) = RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, uint32);
			gfx_draw_string_left_wrapped(dpi, (void*)0x013CE952, x, y, 222, 2788, 0);
		}
	}
}

#pragma endregion

#pragma region Awards page

/**
 *
 *  rct2: 0x00669851
 */
static void window_park_awards_mouseup()
{
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	if (widgetIndex == WIDX_CLOSE)
		window_close(w);
	else if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
		window_park_set_page(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x0066986D
 */
static void window_park_awards_update()
{
	rct_window *w;

	__asm mov w, esi

	w->var_48E++;
	window_invalidate_by_id(w->classification, 1179);
}

/**
 *
 *  rct2: 0x006696FB
 */
static void window_park_awards_invalidate()
{
	int i;
	rct_window *w;
	rct_widget **widgets;

	__asm mov w, esi

	widgets = window_park_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_park_set_pressed_tab(w);

	RCT2_GLOBAL(0x013CE952, uint16) = RCT2_GLOBAL(0x013573D4, uint16);
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = RCT2_GLOBAL(0x013573D8, uint32);

	window_park_align_tabs(w);
	window_park_anchor_border_widgets(w);
}

/**
 *
 *  rct2: 0x0066978D
 */
static void window_park_awards_paint()
{
	int i, count, x, y;
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_award *award;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);
	window_park_draw_tab_images(dpi, w);

	x = w->x + window_park_awards_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	y = w->y + window_park_awards_widgets[WIDX_PAGE_BACKGROUND].top + 4;

	count = 0;
	for (i = 0; i < 4; i++) {
		award = &RCT2_ADDRESS(RCT2_ADDRESS_AWARD_LIST, rct_award)[i];
		if (award->time == 0)
			continue;

		gfx_draw_sprite(dpi, SPR_AWARD_MOST_UNTIDY + award->type, x, y);
		gfx_draw_string_left_wrapped(dpi, STR_AWARD_MOST_UNTIDY, x + 34, y + 6, 180, 0, 0);

		y += 32;
		count++;
	}

	if (count == 0)
		gfx_draw_string_left(dpi, STR_NO_RECENT_AWARDS, 0, 0, x + 6, y + 6);
}

#pragma endregion

static void window_park_set_page(rct_window *w, int page)
{
	if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3))
		if (w->classification == RCT2_GLOBAL(0x009DE544, rct_windowclass) && w->number == RCT2_GLOBAL(0x009DE542, rct_windownumber))
			RCT2_CALLPROC_EBPSAFE(0x006EE281);

	// Set listen only to viewport
	RCT2_GLOBAL(0x009E32E0, uint8) = 0;
	if (page == WINDOW_PARK_PAGE_ENTRANCE && w->page == WINDOW_PARK_PAGE_ENTRANCE && w->viewport != NULL && !(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
		RCT2_GLOBAL(0x009E32E0, uint8)++;

	w->page = page;
	w->var_48E = 0;
	w->var_492 = 0;
	if (w->viewport != NULL) {
		w->viewport->x = 0;
		w->viewport = NULL;
	}

	w->enabled_widgets = window_park_page_enabled_widgets[page];
	w->var_020 = RCT2_GLOBAL(0x0097BAE0 + (page * 4), uint32);
	w->event_handlers = window_park_page_events[page];
	w->widgets = window_park_page_widgets[page];
	RCT2_CALLPROC_X(0x00667F8B, 0, 0, 0, 0, w, 0, 0);
	window_invalidate(w);

	RCT2_CALLPROC_X(w->event_handlers[WE_UNKNOWN_02], 0, 0, 0, 0, w, 0, 0);
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, w, 0, 0);
	if (RCT2_GLOBAL(0x009E32E0, uint8) != 0 && w->viewport != NULL)
		w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

static void window_park_anchor_border_widgets(rct_window *w)
{
	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 13;
	w->widgets[WIDX_CLOSE].right = w->width - 3;
}

static void window_park_align_tabs(rct_window *w)
{
	int i, x, tab_width;

	x = w->widgets[WIDX_TAB_1].left;
	tab_width = w->widgets[WIDX_TAB_1].right - w->widgets[WIDX_TAB_1].left;
	for (i = 0; i < 7; i++) {
		if (w->disabled_widgets & (1 << (WIDX_TAB_1 + i)))
			continue;
		w->widgets[WIDX_TAB_1 + i].left = x;
		w->widgets[WIDX_TAB_1 + i].right = x + tab_width;
		x += tab_width + 1;
	}
}

static void window_park_set_pressed_tab(rct_window *w)
{
	int i;
	for (i = 0; i < 7; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1 << (WIDX_TAB_1 + w->page);
}

static void window_park_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	int sprite_idx;

	// Entrance tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_1)))
		gfx_draw_sprite(dpi, 5200, w->x + w->widgets[WIDX_TAB_1].left, w->y + w->widgets[WIDX_TAB_1].top);

	// Rating tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_2))) {
		sprite_idx = SPR_TAB_GRAPH_0;
		if (w->page == WINDOW_PARK_PAGE_RATING)
			sprite_idx += (w->var_48E / 8) % 8;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_2].left, w->y + w->widgets[WIDX_TAB_2].top);
		gfx_draw_sprite(dpi, SPR_RATING_HIGH, w->x + w->widgets[WIDX_TAB_2].left + 7, w->y + w->widgets[WIDX_TAB_2].top + 1);
		gfx_draw_sprite(dpi, SPR_RATING_LOW, w->x + w->widgets[WIDX_TAB_2].left + 16, w->y + w->widgets[WIDX_TAB_2].top + 12);
	}

	// Guests tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_3))) {
		sprite_idx = SPR_TAB_GRAPH_0;
		if (w->page == WINDOW_PARK_PAGE_GUESTS)
			sprite_idx += (w->var_48E / 8) % 8;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_3].left, w->y + w->widgets[WIDX_TAB_3].top);
		
		sprite_idx = *RCT2_GLOBAL(0x00982708, sint32*) + 1;
		if (w->page == WINDOW_PARK_PAGE_GUESTS)
			sprite_idx += w->var_492 & 0xFFFFFFFC;

		sprite_idx |= 0xA9E00000;
		gfx_draw_sprite(
			dpi,
			sprite_idx,
			w->x + (w->widgets[WIDX_TAB_3].left + w->widgets[WIDX_TAB_3].right) / 2,
			w->y + w->widgets[WIDX_TAB_3].bottom - 9
		);
	}

	// Price tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_4))) {
		sprite_idx = SPR_TAB_ADMISSION_0;
		if (w->page == WINDOW_PARK_PAGE_PRICE)
			sprite_idx += (w->var_48E / 2) % 8;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_4].left, w->y + w->widgets[WIDX_TAB_4].top);
	}

	// Statistics tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_5))) {
		sprite_idx = SPR_TAB_STATS_0;
		if (w->page == WINDOW_PARK_PAGE_STATS)
			sprite_idx += (w->var_48E / 4) % 7;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_5].left, w->y + w->widgets[WIDX_TAB_5].top);
	}

	// Objective tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_6))) {
		sprite_idx = SPR_TAB_OBJECTIVE_0;
		if (w->page == WINDOW_PARK_PAGE_OBJECTIVE)
			sprite_idx += (w->var_48E / 4) % 16;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_6].left, w->y + w->widgets[WIDX_TAB_6].top);
	}

	// Awards tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_7)))
		gfx_draw_sprite(dpi, SPR_TAB_AWARDS, w->x + w->widgets[WIDX_TAB_7].left, w->y + w->widgets[WIDX_TAB_7].top);
}