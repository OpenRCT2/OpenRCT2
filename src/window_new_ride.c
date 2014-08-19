/*****************************************************************************
* Copyright (c) 2014 Maciek Baron
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

#include <string.h>
#include "addresses.h"
#include "game.h"
#include "news_item.h"
#include "string_ids.h"
#include "widget.h"
#include "window.h"

#define _window_new_ride_current_tab RCT2_GLOBAL(0x00F43824, uint8)

enum {
	WINDOW_NEW_RIDE_PAGE_TRANSPORT,
	WINDOW_NEW_RIDE_PAGE_GENTLE,
	WINDOW_NEW_RIDE_PAGE_ROLLER_COASTER,
	WINDOW_NEW_RIDE_PAGE_THRILL,
	WINDOW_NEW_RIDE_PAGE_WATER,
	WINDOW_NEW_RIDE_PAGE_SHOP,
	WINDOW_NEW_RIDE_PAGE_RESEARCH,
	WINDOW_NEW_RIDE_PAGE_COUNT
};

#pragma region Widgets

enum {
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
	WIDX_RIDE_LIST,
	WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP,
	WIDX_LAST_DEVELOPMENT_GROUP,
	WIDX_LAST_DEVELOPMENT_BUTTON,
	WIDX_RESEARCH_FUNDING_BUTTON
};

static rct_widget window_new_ride_widgets[] = {
	{ WWT_FRAME,			0,	0,		600,	0,		369,	0xFFFFFFFF,								STR_NONE							},
	{ WWT_CAPTION,			0,	1,		599,	1,		14,		0xFFFFFFFF,								STR_WINDOW_TITLE_TIP				},
	{ WWT_CLOSEBOX,			0,	588,	598,	2,		13,		STR_CLOSE_X,							STR_CLOSE_WINDOW_TIP				},
	{ WWT_RESIZE,			1,	0,		600,	43,		369,	0xFFFFFFFF,								STR_NONE							},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,								STR_TRANSPORT_RIDES_TIP				},
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,								STR_GENTLE_RIDES_TIP				},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,								STR_ROLLER_COASTERS_TIP				},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,								STR_THRILL_RIDES_TIP				},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,								STR_WATER_RIDES_TIP					},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,								STR_SHOPS_STALLS_TIP				},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,								STR_RESEARCH_AND_DEVELOPMENT_TIP	},
	{ WWT_SCROLL,			1,	3,		597,	46,		317,	2,										STR_NONE							},
	{ WWT_GROUPBOX,			2,	3,		292,	47,		116,	STR_CURRENTLY_IN_DEVELOPMENT,			STR_NONE							},
	{ WWT_GROUPBOX,			2,	3,		292,	124,	188,	STR_LAST_DEVELOPMENT,					STR_NONE							},
	{ WWT_FLATBTN,			2,	265,	288,	161,	184,	0xFFFFFFFF,								STR_RESEARCH_SHOW_DETAILS_TIP		},
	{ WWT_FLATBTN,			2,	265,	288,	68,		91,		5190,									STR_FINANCES_RESEARCH				},
	{ WIDGETS_END },
};

#pragma endregion

#pragma region Events

static void window_new_ride_emptysub() { }

static void window_new_ride_mouseup();
static void window_new_ride_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_new_ride_update(rct_window *w);
static void window_new_ride_scrollgetsize();
static void window_new_ride_scrollmousedown();
static void window_new_ride_scrollmouseover();
static void window_new_ride_tooltip();
static void window_new_ride_invalidate();
static void window_new_ride_paint();
static void window_new_ride_scrollpaint();

// 0x0098E354
static void* window_new_ride_events[] = {
	window_new_ride_emptysub,
	window_new_ride_mouseup,
	window_new_ride_emptysub,
	window_new_ride_mousedown,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_update,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_scrollgetsize,
	window_new_ride_scrollmousedown,
	window_new_ride_emptysub,
	window_new_ride_scrollmouseover,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_tooltip,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_invalidate,
	window_new_ride_paint,
	window_new_ride_scrollpaint
};

#pragma endregion

const int window_new_ride_tab_animation_loops[] = { 20, 32, 10, 72, 24, 28, 16 };
const int window_new_ride_tab_animation_divisor[] = { 4, 8, 2, 4, 4, 4, 2 };

static void window_new_ride_refresh_widget_sizing(rct_window *w);

/**
*
*  rct2: 0x006ACA58
*/
void window_new_ride_init_vars() {
	// If we are in the track designer, default to the Roller Coaster tab
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER) {
		RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_RIDE_LIST_SELECTED_TAB, uint8) = WINDOW_NEW_RIDE_PAGE_ROLLER_COASTER;
	}
	else {
		RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_RIDE_LIST_SELECTED_TAB, uint8) = WINDOW_NEW_RIDE_PAGE_TRANSPORT;
	}

	for (short i = 0; i < 6; i++) {
		/*
		Reset what is highlighted in each tab.
		Each 16bit number represents the item in its respective tab.
		*/
		RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_RIDE_LIST_HIGHLIGHTED_ITEM, uint16)[i] = 0xFFFF;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_RIDE_LIST_INFORMATION_TYPE, uint8) = 0;
}

/**
 *
 *  rct2: 0x006B3CFF
 */
void window_new_ride_open()
{
	rct_window *w;

	w = window_bring_to_front_by_id(WC_CONSTRUCT_RIDE, 0);
	if (w != NULL)
		return;

	// Not sure what these windows are
	window_close_by_id(161, 0);
	window_close_by_id(162, 0);

	w = window_create_auto_pos(601, 370, (uint32*)window_new_ride_events, WC_CONSTRUCT_RIDE, 0x400);
	w->widgets = window_new_ride_widgets;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_TAB_1) |
		(1 << WIDX_TAB_2) |
		(1 << WIDX_TAB_3) |
		(1 << WIDX_TAB_4) |
		(1 << WIDX_TAB_5) |
		(1 << WIDX_TAB_6) |
		(1 << WIDX_TAB_7) |
		(1 << 14) |
		(1 << 15);
	window_init_scroll_widgets(w);

	w->frame_no = 0;
	w->colours[0] = 24;
	w->colours[1] = 26;
	w->colours[2] = 26;
	w->var_480 = -1;
	w->var_482 = -1;
	RCT2_GLOBAL(0x00F43866, sint16) = -1;
	
	RCT2_CALLPROC_EBPSAFE(0x006B6F3E);
	
	w->var_482 = RCT2_ADDRESS(0x00F43825, sint16)[RCT2_GLOBAL(0x00F43824, uint8)];
	if (w->var_482 == -1)
		w->var_482 = RCT2_GLOBAL(0x00F43523, sint16);
	
	w->width = 1;
	window_new_ride_refresh_widget_sizing(w);
	RCT2_CALLPROC_EBPSAFE(0x006B7220);
}

/**
 *
 *  rct2: 0x006B3DF1
 */
static void window_new_ride_refresh_widget_sizing(rct_window *w)
{
	int width, height;

	// Show or hide unrelated widgets
	if (_window_new_ride_current_tab != WINDOW_NEW_RIDE_PAGE_RESEARCH) {
		window_new_ride_widgets[WIDX_RIDE_LIST].type = WWT_SCROLL;
		window_new_ride_widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].type = WWT_EMPTY;
		window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_GROUP].type = WWT_EMPTY;
		window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_EMPTY;
		window_new_ride_widgets[WIDX_RESEARCH_FUNDING_BUTTON].type = WWT_EMPTY;

		width = 601;
		height = 370;
	} else {
		window_new_ride_widgets[WIDX_RIDE_LIST].type = WWT_EMPTY;
		window_new_ride_widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].type = WWT_GROUPBOX;
		window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_GROUP].type = WWT_GROUPBOX;
		window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_FLATBTN;
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_11))
			window_new_ride_widgets[WIDX_RESEARCH_FUNDING_BUTTON].type = WWT_FLATBTN;

		width = 300;
		height = 196;
	}

	// Handle new window size
	if (w->width != width || w->height != height) {
		window_invalidate(w);

		// Resize widgets to new window size
		window_new_ride_widgets[WIDX_BACKGROUND].right = width - 1;
		window_new_ride_widgets[WIDX_BACKGROUND].bottom = height - 1;
		window_new_ride_widgets[WIDX_PAGE_BACKGROUND].right = width - 1;
		window_new_ride_widgets[WIDX_PAGE_BACKGROUND].bottom = height - 1;
		window_new_ride_widgets[WIDX_TITLE].right = width - 2;
		window_new_ride_widgets[WIDX_CLOSE].left = width - 13;
		window_new_ride_widgets[WIDX_CLOSE].right = width - 3;

		w->width = width;
		w->height = height;
		window_invalidate(w);
	}

	window_init_scroll_widgets(w);
}

static void window_new_ride_set_pressed_tab(rct_window *w)
{
	int i;
	for (i = 0; i < WINDOW_NEW_RIDE_PAGE_COUNT; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + _window_new_ride_current_tab);
}

const int ThrillRidesTabAnimationSequence[] = {
	5, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0
};

static void window_new_ride_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, int page, int spriteIndex)
{
	int widgetIndex = WIDX_TAB_1 + page;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		int frame = 0;
		if (_window_new_ride_current_tab == page)
			frame = w->frame_no / window_new_ride_tab_animation_divisor[page];

		spriteIndex += page == WINDOW_NEW_RIDE_PAGE_THRILL ?
				ThrillRidesTabAnimationSequence[frame] : frame;

		spriteIndex |= w->colours[1] << 19;

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

static void window_new_ride_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_TRANSPORT, 0x200015A1);
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_GENTLE, 5542);
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_ROLLER_COASTER, 0x200015AA);
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_THRILL, 5557);
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_WATER, 5551);
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_SHOP, 5530);
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_RESEARCH, 5327);
}

/**
 *
 *  rct2: 0x006B6B38
 */
static void window_new_ride_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_mouse_up_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_LAST_DEVELOPMENT_BUTTON:
		news_item_open_subject(NEWS_ITEM_RESEARCH, RCT2_GLOBAL(RCT2_ADDRESS_LAST_RESEARCHED_ITEM_SUBJECT, sint32));
		break;
	case WIDX_RESEARCH_FUNDING_BUTTON:
		window_finances_research_open();
		break;
	}
}

/**
 *
 *  rct2: 0x006B6B4F
 */
static void window_new_ride_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	int page;
	if (widgetIndex < WIDX_TAB_1 || widgetIndex > WIDX_TAB_7)
		return;

	page = widgetIndex - WIDX_TAB_1;

	_window_new_ride_current_tab = page;
	w->frame_no = 0;
	w->var_482 = -1;
	w->var_480 = -1;
	RCT2_CALLPROC_EBPSAFE(0x006B6F3E);
	if (page < WINDOW_NEW_RIDE_PAGE_RESEARCH) {
		w->var_482 = RCT2_ADDRESS(0x00F43825, sint16)[page];
		if (w->var_482 == -1)
			w->var_482 = RCT2_GLOBAL(0x00F43523, sint16);
	}

	window_new_ride_refresh_widget_sizing(w);
	window_invalidate(w);
	RCT2_CALLPROC_EBPSAFE(0x006B7220);
}

/**
 *
 *  rct2: 0x006B6CE7
 */
static void window_new_ride_update(rct_window *w)
{
	w->frame_no++;
	if (w->frame_no >= window_new_ride_tab_animation_loops[_window_new_ride_current_tab])
		w->frame_no = 0;

	widget_invalidate(w->classification, w->number, WIDX_TAB_1 + _window_new_ride_current_tab);

	if (w->var_480 != -1 && w->var_488 == 1)
		RCT2_CALLPROC_X(0x006B6B78, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 *
 *  rct2: 0x006B6BC9
 */
static void window_new_ride_scrollgetsize()
{

}

/**
 *
 *  rct2: 0x006B6C89
 */
static void window_new_ride_scrollmousedown()
{

}

/**
 *
 *  rct2: 0x006B6BBF
 */
static void window_new_ride_scrollmouseover()
{

}

/**
 *
 *  rct2: 0x006B6BBF
 */
static void window_new_ride_tooltip()
{
	RCT2_GLOBAL(0x013CE952, uint16) = 3159;
}

/**
 *
 *  rct2: 0x006B6819
 */
static void window_new_ride_invalidate()
{
	rct_window *w;

	window_get_register(w);

	window_new_ride_set_pressed_tab(w);

	window_new_ride_widgets[WIDX_TITLE].image = STR_NEW_TRANSPORT_RIDES + _window_new_ride_current_tab;
	window_new_ride_widgets[WIDX_TAB_7].type = WWT_TAB;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 4)
		window_new_ride_widgets[WIDX_TAB_7].type = WWT_EMPTY;

	if (_window_new_ride_current_tab == WINDOW_NEW_RIDE_PAGE_RESEARCH) {
		window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_EMPTY;
		uint32 typeId = RCT2_GLOBAL(0x01357CF4, uint32);
		if (typeId != 0xFFFFFFFF) {
			window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_FLATBTN;
			window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].image = typeId >= 0x10000 ? 5189 : 5191;
		}
	}
}

/**
 *
 *  rct2: 0x006B689B
 */
static void window_new_ride_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_new_ride_draw_tab_images(dpi, w);

	if (_window_new_ride_current_tab != WINDOW_NEW_RIDE_PAGE_RESEARCH)
		return;

	int x = w->x + 10;
	int y = w->y + window_new_ride_widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].top + 12;

	// Research type
	rct_string_id stringId = STR_RESEARCH_UNKNOWN;
	if (RCT2_GLOBAL(0x01357CF3, uint8) != 0) {
		stringId = STR_TRANSPORT_RIDE + RCT2_GLOBAL(0x013580E6, uint8);
		if (RCT2_GLOBAL(0x01357CF3, uint8) != 1) {
			uint32 typeId = RCT2_GLOBAL(0x013580E0, uint32);
			if (typeId >= 0x10000) {
				uint8 *rideEntry = RCT2_GLOBAL(0x009ACFA4 + (typeId & 0xFF) * 4, uint8*);
				if (RCT2_GLOBAL(rideEntry + 8, uint32) & 0x1000)
					stringId = RCT2_GLOBAL(rideEntry, uint16);
				else
					stringId = (typeId & 0xFF00) + 2;
			} else {
				uint8 *sceneryEntry = RCT2_GLOBAL(0x009ADA90 + (typeId & 0xFFFF) * 4, uint8*);
				stringId = RCT2_GLOBAL(sceneryEntry, uint16);
			}
		}
	}
	gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 296, STR_RESEARCH_TYPE_LABEL, 0);
	y += 25;

	// Progress
	stringId = 2285 + RCT2_GLOBAL(0x01357CF3, uint8);
	gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 296, STR_RESEARCH_PROGRESS_LABEL, 0);
	y += 15;

	// Expected
	RCT2_GLOBAL(0x013CE952, uint16) = STR_UNKNOWN;
	if (RCT2_GLOBAL(0x01357CF3, uint8) != 0) {
		uint16 expectedDay = RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_EXPECTED_DAY, uint8);
		if (expectedDay != 255) {
			RCT2_GLOBAL(0x013CE952 + 2, uint16) = STR_DATE_DAY_1 + expectedDay;
			RCT2_GLOBAL(0x013CE952 + 4, uint16) = STR_MONTH_MARCH + RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_EXPECTED_MONTH, uint8);
			RCT2_GLOBAL(0x013CE952, uint16) = 2289;
		}
	}
	gfx_draw_string_left(dpi, STR_RESEARCH_EXPECTED_LABEL, (void*)0x013CE952, 0, x, y);

	// Last development
	x = w->x + 10;
	y = w->y + window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_GROUP].top + 12;

	uint32 typeId = RCT2_GLOBAL(0x01357CF4, uint32);
	if (typeId != 0xFFFFFFFF) {
		if (typeId >= 0x10000) {
			uint8 *rideEntry = RCT2_GLOBAL(0x009ACFA4 + (typeId & 0xFF) * 4, uint8*);
				if (RCT2_GLOBAL(rideEntry + 8, uint32) & 0x1000)
					stringId = RCT2_GLOBAL(rideEntry, uint16);
				else
					stringId = (typeId & 0xFF00) + 2;
		} else {
			uint8 *sceneryEntry = RCT2_GLOBAL(0x009ADA90 + (typeId & 0xFFFF) * 4, uint8*);
			stringId = RCT2_GLOBAL(sceneryEntry, uint16);
		}
		gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 266, STR_RESEARCH_RIDE_LABEL, 0);
	}
}

/**
 *
 *  rct2: 0x006B6ABF
 */
static void window_new_ride_scrollpaint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);
}