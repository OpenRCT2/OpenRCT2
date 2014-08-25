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

#include <string.h>
#include "addresses.h"
#include "game.h"
#include "ride.h"
#include "string_ids.h"
#include "sprite.h"
#include "sprites.h"
#include "viewport.h"
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"

enum WINDOW_PARK_PAGE {
	WINDOW_PARK_PAGE_MAIN,
	WINDOW_PARK_PAGE_VEHICLE,
	WINDOW_PARK_PAGE_OPERATING,
	WINDOW_PARK_PAGE_MAINTENANCE,
	WINDOW_PARK_PAGE_COLOUR,
	WINDOW_PARK_PAGE_MUSIC,
	WINDOW_PARK_PAGE_MEASUREMENTS,
	WINDOW_PARK_PAGE_GRAPHS,
	WINDOW_PARK_PAGE_INCOME,
	WINDOW_PARK_PAGE_CUSTOMER
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
	WIDX_TAB_8,
	WIDX_TAB_9,
	WIDX_TAB_10,

	WIDX_VIEWPORT = 14,
	WIDX_VIEW,
	WIDX_VIEW_DROPDOWN,
	WIDX_STATUS,
	WIDX_OPEN,
	WIDX_CONSTRUCTION,
	WIDX_RENAME,
	WIDX_LOCATE,
	WIDX_DEMOLISH,

	WIDX_PLAY_MUSIC = 14,
	WIDX_MUSIC,
	WIDX_MUSIC_DROPDOWN
};

// 0x009ADC34
static rct_widget window_ride_main_widgets[] = {
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0x0FFFFFFFF,					STR_NONE									},
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		0x3DD,							STR_WINDOW_TITLE_TIP						},
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP						},
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0x0FFFFFFFF,					STR_NONE									},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_VIEW_OF_RIDE_ATTRACTION_TIP				},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,						STR_VEHICLE_DETAILS_AND_OPTIONS_TIP			},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_OPERATING_OPTIONS_TIP					},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_MAINTENANCE_OPTIONS_TIP					},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_COLOUR_SCHEME_OPTIONS_TIP				},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_SOUND_AND_MUSIC_OPTIONS_TIP				},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_MEASUREMENTS_AND_TEST_DATA_TIP			},
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,						STR_GRAPHS_TIP								},
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,						STR_INCOME_AND_COSTS_TIP					},
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,						STR_CUSTOMER_INFORMATION_TIP				},

	{ WWT_VIEWPORT,			1,	3,		290,	60,		166,	0x0FFFFFFFE,					STR_NONE									},
	{ WWT_DROPDOWN,			1,	35,		256,	46,		57,		0x0FFFFFFFF,					STR_VIEW_SELECTION							},
	{ WWT_DROPDOWN_BUTTON,	1,	245,	255,	47,		56,		876,							STR_VIEW_SELECTION							},
	{ WWT_12,				1,	3,		290,	167,	177,	0x0FFFFFFFF,					STR_NONE									},
	{ WWT_FLATBTN,			1,	291,	314,	46,		69,		0x0FFFFFFFF,					STR_OPEN_CLOSE_OR_TEST_RIDE					},
	{ WWT_FLATBTN,			1,	291,	314,	70,		93,		SPR_CONSTRUCTION,				STR_CONSTRUCTION							},
	{ WWT_FLATBTN,			1,	291,	314,	94,		117,	SPR_RENAME,						STR_NAME_RIDE_TIP							},
	{ WWT_FLATBTN,			1,	291,	314,	118,	141,	SPR_LOCATE,						STR_LOCATE_SUBJECT_TIP						},
	{ WWT_FLATBTN,			1,	291,	314,	142,	165,	SPR_DEMOLISH,					STR_DEMOLISH_RIDE_TIP						},
	{ WIDGETS_END },
};

// 0x009AE4C8
static rct_widget window_ride_music_widgets[] = {
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0x0FFFFFFFF,					STR_NONE									},
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		0x3DD,							STR_WINDOW_TITLE_TIP						},
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP						},
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0x0FFFFFFFF,					STR_NONE									},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_VIEW_OF_RIDE_ATTRACTION_TIP				},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,						STR_VEHICLE_DETAILS_AND_OPTIONS_TIP			},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_OPERATING_OPTIONS_TIP					},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_MAINTENANCE_OPTIONS_TIP					},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_COLOUR_SCHEME_OPTIONS_TIP				},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_SOUND_AND_MUSIC_OPTIONS_TIP				},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_MEASUREMENTS_AND_TEST_DATA_TIP			},
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,						STR_GRAPHS_TIP								},
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,						STR_INCOME_AND_COSTS_TIP					},
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,						STR_CUSTOMER_INFORMATION_TIP				},

	{ WWT_CHECKBOX,			1,	7,		308,	47,		58,		STR_PLAY_MUSIC,					STR_SELECT_MUSIC_TIP						},
	{ WWT_DROPDOWN,			1,	7,		308,	62,		73,		0,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	63,		72,		876,							STR_SELECT_MUSIC_STYLE_TIP					},
	{ WIDGETS_END },
};

static rct_widget *window_ride_page_widgets[] = {
	window_ride_main_widgets,
	(rct_widget*)0x009ADDA8,
	(rct_widget*)0x009ADEFC,
	(rct_widget*)0x009AE190,
	(rct_widget*)0x009AE2A4,
	window_ride_music_widgets,
	(rct_widget*)0x009AE5DC,
	(rct_widget*)0x009AE710,
	(rct_widget*)0x009AE844,
	(rct_widget*)0x009AE9C8
};

const uint64 window_ride_page_enabled_widgets[] = {
	0x00000000007DBFF4,
	0x00000000001EFFF4,
	0x0000019E777DBFF4,
	0x000000000001FFF4,
	0x00000003F37F3FF4,
	0x000000000001FFF4,
	0x000000000007FFF4,
	0x000000000007BFF4,
	0x0000000000E73FF4,
	0x000000000001FFF4
};

#pragma endregion

#pragma region Events

static void window_ride_emptysub() { }

static void window_ride_main_mouseup();
static void window_ride_main_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_main_dropdown();
static void window_ride_main_update(rct_window *w);
static void window_ride_main_textinput();
static void window_ride_main_invalidate();
static void window_ride_main_paint();

static void window_ride_music_mouseup();
static void window_ride_music_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_music_dropdown();
static void window_ride_music_update(rct_window *w);
static void window_ride_music_invalidate();
static void window_ride_music_paint();

// 0x0098DFD4
static void* window_ride_main_events[] = {
	window_ride_emptysub,
	window_ride_main_mouseup,
	window_ride_emptysub,
	window_ride_main_mousedown,
	window_ride_main_dropdown,
	window_ride_emptysub,
	window_ride_main_update,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_main_textinput,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_main_invalidate,
	window_ride_main_paint,
	window_ride_emptysub
};

// 0x0098E194
static void* window_ride_music_events[] = {
	window_ride_emptysub,
	window_ride_music_mouseup,
	window_ride_emptysub,
	window_ride_music_mousedown,
	window_ride_music_dropdown,
	window_ride_emptysub,
	window_ride_music_update,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_music_invalidate,
	window_ride_music_paint,
	window_ride_emptysub
};

static uint32* window_ride_page_events[] = {
	(uint32*)window_ride_main_events,
	(uint32*)0x0098E204,
	(uint32*)0x0098E0B4,
	(uint32*)0x0098E124,
	(uint32*)0x0098E044,
	(uint32*)window_ride_music_events,
	(uint32*)0x0098DE14,
	(uint32*)0x0098DF64,
	(uint32*)0x0098DEF4,
	(uint32*)0x0098DE84
};

#pragma endregion

/**
 * 
 * rct2: 0x006AEAB4
 */
rct_window *window_ride_open(int rideIndex)
{
	rct_window *w;

	w = window_create_auto_pos(316, 180, window_ride_page_events[0], WC_RIDE, 0x400);
	w->widgets = window_ride_page_widgets[0];
	w->enabled_widgets = window_ride_page_enabled_widgets[0];
	w->number = rideIndex;

	w->page = 0;
	w->var_48C = 0;
	w->frame_no = 0;
	w->list_information_type = 0;
	w->var_492 = 0;
	w->var_494 = 0;
	RCT2_CALLPROC_X(0x006AEB9F, 0, 0, 0, 0, (int)w, 0, 0);
	w->min_width = 316;
	w->min_height = 180;
	w->max_width = 500;
	w->max_height = 450;
	w->flags |= WF_RESIZABLE;
	w->colours[0] = 1;
	w->colours[1] = 26;
	w->colours[2] = 11;

	rct_ride *ride = &g_ride_list[rideIndex];
	uint8 *edx = (uint8*)0x009E32F8;
	if (ride->type != RIDE_TYPE_NULL) {
		int rideType = ride->type;
		do {
			edx++;
			if (*(edx - 1) != 0xFF)
				continue;
		} while (rideType-- != 0);
	}

	int eax, ebx = 0, ecx;
	while (*edx != 0xFF) {
		eax = *edx++;
		ecx = eax >> 5;
		eax &= 0x1F;
		if (!(RCT2_ADDRESS(0x001357424, uint32)[ecx] & (1 << eax)))
			continue;
		ebx++;
	}
	RCT2_GLOBAL((int)w + 496, uint16) = ebx;
}

/**
 * 
 * rct2: 0x006ACC28
 */
void window_ride_main_open(int rideIndex)
{
	rct_window *w;

	w = window_bring_to_front_by_id(WC_RIDE, rideIndex);
	if (w == NULL) {
		w = window_ride_open(rideIndex);
		w->ride.var_482 = -1;
	}

	if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)) {
		if (w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) &&
			w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber)
		) {
			tool_cancel();
		}
	}

	w->page = 0;
	w->width = 316;
	w->height = 180;
	window_invalidate(w);
	w->widgets = window_ride_page_widgets[0];
	w->enabled_widgets = 0x007DBFF4;
	w->var_020 = 0;
	w->event_handlers = window_ride_page_events[0];
	w->pressed_widgets = 0;
	RCT2_CALLPROC_X(0x006AEB9F, 0, 0, 0, 0, (int)w, 0, 0);
	window_init_scroll_widgets(w);
	w->ride.var_480 = 0;
	RCT2_CALLPROC_X(0x006AF994, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 *
 *  rct2: 0x006AF1D2
 */
static void window_ride_set_page(rct_window *w, int page)
{
	int listen;

	if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3))
		if (w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) && w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber))
			tool_cancel();

	// Set listen only to viewport
	listen = 0;
	if (page == WINDOW_PARK_PAGE_MAIN && w->page == WINDOW_PARK_PAGE_MAIN && w->viewport != NULL && !(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
		listen++;

	w->page = page;
	w->frame_no = 0;
	w->var_492 = 0;
	if (w->viewport != NULL) {
		w->viewport->width = 0;
		w->viewport = NULL;
	}

	w->enabled_widgets = window_ride_page_enabled_widgets[page];
	w->var_020 = RCT2_ADDRESS(0x0098DD68, uint32)[page];
	w->event_handlers = window_ride_page_events[page];
	w->widgets = window_ride_page_widgets[page];
	RCT2_CALLPROC_X(0x006AEB9F, 0, 0, 0, 0, (int)w, 0, 0);
	window_invalidate(w);

	RCT2_CALLPROC_X(w->event_handlers[WE_RESIZE], 0, 0, 0, 0, (int)w, 0, 0);
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);

	window_init_scroll_widgets(w);
	window_invalidate(w);

	if (listen != 0 && w->viewport != NULL)
		w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

static void window_ride_set_pressed_tab(rct_window *w)
{
	int i;
	for (i = 0; i < 7; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

#pragma region Main

/**
 * 
 * rct2: 0x006AF17E
 */
static void window_ride_main_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

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
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
		window_ride_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_CONSTRUCTION:
		break;
	case WIDX_RENAME:
		break;
	case WIDX_LOCATE:
		break;
	case WIDX_DEMOLISH:
		break;
	}
}

/**
 * 
 * rct2: 0x006AF1BD
 */
static void window_ride_main_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{

}

/**
 * 
 * rct2: 0x006AF300
 */
static void window_ride_main_dropdown()
{

}

/**
 * 
 * rct2: 0x006AF40F
 */
static void window_ride_main_update(rct_window *w)
{

}

/**
 * 
 * rct2: 0x006AF2F9
 */
static void window_ride_main_textinput()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	if (widgetIndex != WIDX_RENAME)
		return;


}

/**
 * 
 * rct2: 0x006AECF6
 */
static void window_ride_main_invalidate()
{
	rct_window *w;
	rct_widget *widgets;

	window_get_register(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = GET_RIDE(w->number);
	w->disabled_widgets &= 0xFFB7FFFF;
	if (ride->lifecycle_flags & (RIDE_LIFECYCLE_INDESTRUCTIBLE | RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)) {
		w->disabled_widgets |= 0x00400000;
	}
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_19)
		w->disabled_widgets |= 0x80000;

	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->overall_view;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->var_04C;
	window_ride_main_widgets[WIDX_OPEN].image = SPR_CLOSED + ride->status;
}

/**
 * 
 * rct2: 0x006AEE73
 */
static void window_ride_main_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
}

#pragma endregion

#pragma region Music

/**
 * 
 * rct2: 0x006B1ED7
 */
static void window_ride_music_mouseup()
{

}

/**
 * 
 * rct2: 0x006B1EFC
 */
static void window_ride_music_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{

}

/**
 * 
 * rct2: 0x006B1F03
 */
static void window_ride_music_dropdown()
{

}

/**
 * 
 * rct2: 0x006B2198
 */
static void window_ride_music_update(rct_window *w)
{

}

/**
 * 
 * rct2: 0x006B1DEA
 */
static void window_ride_music_invalidate()
{

}

/**
 * 
 * rct2: 0x006B1ECC
 */
static void window_ride_music_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
}

#pragma endregion