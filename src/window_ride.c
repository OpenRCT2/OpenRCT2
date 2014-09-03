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
#include "map.h"
#include "ride.h"
#include "ride_data.h"
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
static void window_ride_main_resize();
static void window_ride_main_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_main_dropdown();
static void window_ride_main_update(rct_window *w);
static void window_ride_main_textinput();
static void window_ride_main_unknown_14();
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
	window_ride_main_resize,
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
	window_ride_main_unknown_14,
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

const int window_ride_tab_animation_divisor[] = { 0, 0, 2, 2, 4, 2, 8, 8, 2, 0 };
const int window_ride_tab_animation_frames[] = { 0, 0, 4, 16, 8, 16, 8, 8, 8, 0 };

// WINDOW_PARK_PAGE_MAIN,
// WINDOW_PARK_PAGE_VEHICLE,
// WINDOW_PARK_PAGE_OPERATING,
// WINDOW_PARK_PAGE_MAINTENANCE,
// WINDOW_PARK_PAGE_COLOUR,
// WINDOW_PARK_PAGE_MUSIC,
// WINDOW_PARK_PAGE_MEASUREMENTS,
// WINDOW_PARK_PAGE_GRAPHS,
// WINDOW_PARK_PAGE_INCOME,
// WINDOW_PARK_PAGE_CUSTOMER

static void window_ride_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, int page, int spriteIndex)
{
	int widgetIndex = WIDX_TAB_1 + page;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		if (w->page == page) {
			int frame = w->frame_no / window_ride_tab_animation_divisor[w->page];
			spriteIndex += (frame % window_ride_tab_animation_frames[w->page]);
		}

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

/**
 * 
 * rct2: 0x006B2E88
 */
static void window_ride_draw_tab_main(rct_drawpixelinfo *dpi, rct_window *w)
{
	int widgetIndex = WIDX_TAB_1 + WINDOW_PARK_PAGE_MAIN;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		int spriteIndex;
		int rideType = GET_RIDE(w->number)->type;

		switch (gRideClassifications[rideType]) {
		case RIDE_CLASS_RIDE:
			spriteIndex = 5442;
			if (w->page == WINDOW_PARK_PAGE_MAIN)
				spriteIndex += (w->frame_no / 4) % 16;
			break;
		case RIDE_CLASS_SHOP_OR_STALL:
			spriteIndex = 5351;
			if (w->page == WINDOW_PARK_PAGE_MAIN)
				spriteIndex += (w->frame_no / 4) % 16;
			break;
		case RIDE_CLASS_KIOSK_OR_FACILITY:
			spriteIndex = 5367;
			if (w->page == WINDOW_PARK_PAGE_MAIN)
				spriteIndex += (w->frame_no / 4) % 8;
			break;
		}

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

/**
 * 
 * rct2: 0x006B2B68
 */
static void window_ride_draw_tab_vehicle(rct_drawpixelinfo *dpi, rct_window *w)
{
	rct_ride *ride;
	rct_widget *widget;
	int widgetIndex, spriteIndex, colour, x, y, width, height;
	uint8 *ebp;
	rct_ride_type *rideEntry, **rideEntries = (rct_ride_type**)0x009ACFA4;

	widgetIndex = WIDX_TAB_1 + WINDOW_PARK_PAGE_VEHICLE;
	widget = &w->widgets[widgetIndex];

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		x = widget->left + 1;
		y = widget->top + 1;
		width = widget->right - x;
		height = widget->bottom - 3 - y;
		if (w->page == WINDOW_PARK_PAGE_VEHICLE)
			height += 4;

		x += w->x;
		y += w->y;

		dpi = clip_drawpixelinfo(dpi, x, width, y, height);
		if (dpi == NULL)
			return;

		x = (widget->right - widget->left) / 2;
		y = (widget->bottom - widget->top) - 12;

		ride = GET_RIDE(w->number);
		RCT2_CALLPROC_X(0x006DE4CD, (ride->var_0C9 << 8) | ride->subtype, 0, 0, 0, 0, 0, 0);

		rideEntry = rideEntries[ride->subtype];
		if (rideEntry->var_008 & 1) {
			dpi->zoom_level = 1;
			dpi->width *= 2;
			dpi->height *= 2;
			x *= 2;
			y *= 2;
			dpi->x *= 2;
			dpi->y *= 2;
		}

		ebp = (uint8*)rideEntry + (RCT2_ADDRESS(0x00F64E38, uint8)[rideEntry->var_013] * 101);
		height += RCT2_GLOBAL(ebp + 0x24, sint8);

		RCT2_GLOBAL(0x00F43480, uint32) = ride->var_1D4;
		colour = (ride->vehicle_colours[1] << 24) | (ride->vehicle_colours[0] << 19);
		spriteIndex = 32;
		if (w->page == WINDOW_PARK_PAGE_VEHICLE)
			spriteIndex += w->frame_no;
		spriteIndex /= 2;
		if (RCT2_GLOBAL(ebp + 0x2C, uint16) & 0x800)
			spriteIndex /= 2;
		spriteIndex &= RCT2_GLOBAL(ebp + 0x1A, uint16);
		spriteIndex *= RCT2_GLOBAL(ebp + 0x30, uint16);
		spriteIndex += RCT2_GLOBAL(ebp + 0x32, uint32);
		spriteIndex |= colour;
		spriteIndex |= 0x80000000;

		gfx_draw_sprite(dpi, spriteIndex, x, y, RCT2_GLOBAL(0x00F43480, uint32));
	}
}

/**
 * 
 * rct2: 0x006B2F42
 */
static void window_ride_draw_tab_customer(rct_drawpixelinfo *dpi, rct_window *w)
{
	int spriteIndex;
	int widgetIndex = WIDX_TAB_1 + WINDOW_PARK_PAGE_CUSTOMER;
	rct_widget *widget = &w->widgets[widgetIndex];

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		spriteIndex = 0;
		if (w->page == WINDOW_PARK_PAGE_CUSTOMER)
			spriteIndex = w->var_492 & ~3;

		spriteIndex += RCT2_GLOBAL(RCT2_GLOBAL(0x00982708, uint32), uint32);
		spriteIndex += 1;
		spriteIndex |= 0xA9E00000;
		
		gfx_draw_sprite(dpi, spriteIndex, w->x + (widget->left + widget->right) / 2, w->y + widget->bottom - 6, 0);
	}
}

/**
 * 
 * rct2: 0x006B2B35
 */
static void window_ride_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	window_ride_draw_tab_vehicle(dpi, w);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_OPERATING, 5201);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_MAINTENANCE, 5205);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_INCOME, 5253);
	window_ride_draw_tab_main(dpi, w);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_MEASUREMENTS, 5229);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_COLOUR, 5221);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_GRAPHS, 5237);
	window_ride_draw_tab_customer(dpi, w);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_MUSIC, 5335);
}

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
	return w;
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
	w->ride.view = 0;
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

static void window_ride_anchor_border_widgets(rct_window *w)
{
	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 13;
	w->widgets[WIDX_CLOSE].right = w->width - 3;
}

#pragma region Main

/**
 * 
 * rct2: 0x006AF994
 */
static void window_ride_init_viewport(rct_window *w)
{
	RCT2_CALLPROC_X(0x006AF994, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 * 
 * rct2: 0x006B4971
 */
static void window_ride_construct(rct_window *w)
{
	int rideIndex = w->number;

	window_close_by_id(WC_RIDE_CONSTRUCTION | 0x80, rideIndex);
	w = window_find_by_id(WC_RIDE, rideIndex);
	if (w == NULL)
		return;

	rct_map_element *trackMapElement;
	int trackX, trackY;

	trackMapElement = sub_6CAF80(rideIndex, &trackX, &trackY);
	if (trackMapElement == (rct_map_element*)-1) {
		RCT2_CALLPROC_X(0x006CC3FB, 0, 0, 0, rideIndex, 0, 0, 0);
	} else {
		trackMapElement = ride_find_track_gap(trackMapElement, &trackX, &trackY);

		w = window_get_main();
		if (w != NULL && ride_try_construct(trackMapElement))
			window_scroll_to_location(w, trackX, trackY, trackMapElement->base_height * 8);
	}
}

/**
 * 
 * rct2: 0x006AF315
 */
static void window_ride_rename(rct_window *w)
{
	rct_ride *ride;

	ride = GET_RIDE(w->number);
	RCT2_GLOBAL(0x013CE962, uint32) = ride->name_arguments;
	window_show_textinput(w, WIDX_RENAME, STR_RIDE_ATTRACTION_NAME, STR_ENTER_NEW_NAME_FOR_THIS_RIDE_ATTRACTION, ride->name);
}

/**
 * 
 * rct2: 0x006AF3B3
 */
static void window_ride_locate(rct_window *w)
{
	rct_window *mainWindow;
	int xy, x, y, z;

	if (w->viewport->width == 0)
		return;

	xy = w->ride.var_482;
	z = w->ride.var_486;
	if (xy == -1)
		return;

	if (xy & 0x80000000) {
		rct_sprite *sprite = &g_sprite_list[xy];
		x = sprite->unknown.x;
		y = sprite->unknown.y;
		z = sprite->unknown.z;
	} else {
		x = (xy & ~0xC0000000) & 0xFFFF;
		y = (xy & ~0xC0000000) >> 16;
		z = z >> 16;
	}

	mainWindow = window_get_main();
	if (mainWindow != NULL)
		window_scroll_to_location(mainWindow, x, y, z);
}

/**
 * 
 * rct2: 0x006B486A
 */
static void window_ride_demolish(rct_window *w)
{
	rct_window *demolishWindow;
	int x, y, screenWidth, screenHeight;

	demolishWindow = window_bring_to_front_by_id(WC_DEMOLISH_RIDE_PROMPT, w->number);
	if (demolishWindow != NULL)
		return;

	screenWidth = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16);
	screenHeight = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16);
	x = screenWidth / 2 - 100;
	y = max(28, screenHeight / 2 - 50);

	demolishWindow = window_create(x, y, 200, 100, (uint32*)0x0098E2E4, WC_DEMOLISH_RIDE_PROMPT, 0);
	demolishWindow->widgets = (rct_widget*)0x009AEBA0;
	demolishWindow->enabled_widgets = 4 | 8 | 16;
	window_init_scroll_widgets(demolishWindow);
	demolishWindow->flags |= WF_TRANSPARENT;
	demolishWindow->number = w->number;
	demolishWindow->colours[0] = 154;
}

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
		window_ride_construct(w);
		break;
	case WIDX_RENAME:
		window_ride_rename(w);
		break;
	case WIDX_LOCATE:
		window_ride_locate(w);
		break;
	case WIDX_DEMOLISH:
		window_ride_demolish(w);
		break;
	}
}

/**
 * 
 * rct2: 0x006AF4A2
 */
static void window_ride_main_resize()
{
	rct_window *w;

	window_get_register(w);

	w->flags |= WF_RESIZABLE;
	window_set_resize(w, 316, 180, 500, 450);
	window_ride_init_viewport(w);
}

/**
 * 
 * rct2: 0x006AF825
 */
static void window_ride_show_view_dropdown(rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget;
	rct_ride *ride;
	int numItems, currentItem, i, j, name;

	dropdownWidget = widget - 1;
	ride = GET_RIDE(w->number);

	numItems = 1;
	if (!(RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x2000)) {
		numItems += ride->num_stations;
		numItems += ride->num_vehicles;
	}

	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		0,
		numItems,
		widget->right - dropdownWidget->left
	);

	// First item
	gDropdownItemsFormat[0] = 1142;
	gDropdownItemsArgs[0] = STR_OVERALL_VIEW;
	currentItem = 1;

	// Vehicles
	name = RideNameConvention[ride->type].vehicle_name + 6;
	for (i = 1; i <= ride->num_vehicles; i++) {
		gDropdownItemsFormat[currentItem] = 1142;
		gDropdownItemsArgs[currentItem] = name | (currentItem << 16);
		currentItem++;
	}

	// Stations
	name = RideNameConvention[ride->type].station_name + 6;
	for (i = 1; i <= ride->num_stations; i++) {
		gDropdownItemsFormat[currentItem] = 1142;
		gDropdownItemsArgs[currentItem] = name | (i << 16);
		currentItem++;
	}

	// Set highlighted item
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)) {
		j = 2;
		for (i = 0; i < ride->num_vehicles; i++) {
			RCT2_GLOBAL(0x009DED34, uint32) |= j;
			j <<= 1;
		}
	}

	// Set checked item
	gDropdownItemsChecked |= (1 << w->ride.view);
}

/**
 * 
 * rct2: 0x006AF64C
 */
static void window_ride_show_open_dropdown(rct_window *w, rct_widget *widget)
{
	rct_ride *ride;
	int numItems, highlightedIndex, checkedIndex;

	ride = GET_RIDE(w->number);

	numItems = 0;
	gDropdownItemsFormat[numItems] = 1142;
	gDropdownItemsArgs[numItems] = STR_CLOSE_RIDE;
	numItems++;

	if (!(RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x800)) {
		gDropdownItemsFormat[numItems] = 1142;
		gDropdownItemsArgs[numItems] = STR_TEST_RIDE;
		numItems++;
	}

	gDropdownItemsFormat[numItems] = 1142;
	gDropdownItemsArgs[numItems] = STR_OPEN_RIDE;
	numItems++;
		
	window_dropdown_show_text(
		w->x + widget->left,
		w->y + widget->top,
		widget->bottom - widget->top + 1,
		w->colours[1],
		0,
		numItems
	);

	checkedIndex = ride->status;
	switch (ride->status) {
	case RIDE_STATUS_CLOSED:
		highlightedIndex = 0;
		if ((ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED) || (ride->lifecycle_flags & RIDE_LIFECYCLE_11))
			break;

		highlightedIndex = 2;
		if (RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x800)
			break;
		if (ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)
			break;

		highlightedIndex = 1;
		break;
	case RIDE_STATUS_TESTING:
		highlightedIndex = 2;
		if (ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)
			break;

		highlightedIndex = 0;
		break;
	case RIDE_STATUS_OPEN:
		highlightedIndex = 0;
		break;
	}

	if (checkedIndex != RIDE_STATUS_CLOSED)
		checkedIndex = 3 - checkedIndex;

	if (RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x800) {
		if (checkedIndex != 0)
			checkedIndex--;
		if (highlightedIndex != 0)
			highlightedIndex--;
	}

	gDropdownItemsChecked |= (1 << checkedIndex);
	RCT2_GLOBAL(0x009DEBA2, sint16) = highlightedIndex;
}

/**
 * 
 * rct2: 0x006AF1BD
 */
static void window_ride_main_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	switch (widgetIndex) {
	case WIDX_VIEW_DROPDOWN:
		window_ride_show_view_dropdown(w, widget);
		break;
	case WIDX_OPEN:
		window_ride_show_open_dropdown(w, widget);
		break;
	}
}

/**
 * 
 * rct2: 0x006AF300
 */
static void window_ride_main_dropdown()
{
	rct_ride *ride;
	rct_window *w;
	int status;
	short widgetIndex, dropdownIndex;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	switch (widgetIndex) {
	case WIDX_VIEW_DROPDOWN:
		if (dropdownIndex == -1) {
			dropdownIndex = w->ride.view;
			ride = GET_RIDE(w->number);
			dropdownIndex++;
			if (dropdownIndex != 0 && dropdownIndex <= ride->num_vehicles && !(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
				dropdownIndex = ride->num_vehicles + 1;

			if (dropdownIndex >= gDropdownNumItems)
				dropdownIndex = 0;
		}

		w->ride.view = dropdownIndex;
		RCT2_CALLPROC_X(0x006AF994, 0, 0, 0, 0, (int)w, 0, 0);
		window_invalidate(w);
		break;
	case WIDX_OPEN:
		if (dropdownIndex == -1)
			dropdownIndex = RCT2_GLOBAL(0x009DEBA2, sint16);

		ride = GET_RIDE(w->number);
		if ((RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x800) && dropdownIndex != 0)
			dropdownIndex++;

		switch (dropdownIndex) {
		case 0:
			status = RIDE_STATUS_CLOSED;
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = 1004;
			break;
		case 1:
			status = RIDE_STATUS_TESTING;
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = 1003;
			break;
		case 2:
			status = RIDE_STATUS_OPEN;
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = 1002;
			break;
		}

		RCT2_GLOBAL(0x013CE952 + 6, uint16) = ride->overall_view;
		RCT2_GLOBAL(0x013CE952 + 8, uint32) = ride->name_arguments;
		game_do_command(0, 1, 0, w->number | (status << 8), GAME_COMMAND_SET_RIDE_OPEN, 0, 0);
		break;
	}
}

/**
 * 
 * rct2: 0x006AF40F
 */
static void window_ride_main_update(rct_window *w)
{
	rct_ride *ride;
	int vehicleIndex;
	uint16 vehicleSpriteIndex;
	rct_vehicle *vehicle;

	// Update tab animation
	w->frame_no++;
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	widget_invalidate(WC_RIDE, w->number, WIDX_TAB_1);

	// Update status
	ride = GET_RIDE(w->number);
	if (!(ride->var_14D & 4)) {
		if (w->ride.view == 0)
			return;

		if (w->ride.view <= ride->num_vehicles) {
			vehicleIndex = w->ride.view - 1;
			vehicleSpriteIndex = ride->vehicles[vehicleIndex];
			if (vehicleSpriteIndex == 0xFFFF)
				return;

			vehicle = &(g_sprite_list[vehicleSpriteIndex].vehicle);
			if (
				vehicle->status != 4 &&
				vehicle->status != 22 &&
				vehicle->status != 10 &&
				vehicle->status != 7
			) {
				return;
			}
		}
	}

	ride->var_14D &= ~4;
	widget_invalidate(WC_RIDE, w->number, WIDX_STATUS);
}

/**
 * 
 * rct2: 0x006AF2F9
 */
static void window_ride_main_textinput()
{
	uint8 result;
	short widgetIndex;
	rct_window *w;
	char *text;

	window_textinput_get_registers(w, widgetIndex, result, text);

	if (widgetIndex != WIDX_RENAME || !result)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_RENAME_RIDE_ATTRACTION;
	game_do_command(1, (w->number << 8) | 1, 0, *((int*)(text +  0)), GAME_COMMAND_SET_RIDE_NAME, *((int*)(text +  8)), *((int*)(text +  4)));
	game_do_command(2, (w->number << 8) | 1, 0, *((int*)(text + 12)), GAME_COMMAND_SET_RIDE_NAME, *((int*)(text + 20)), *((int*)(text + 16)));
	game_do_command(0, (w->number << 8) | 1, 0, *((int*)(text + 24)), GAME_COMMAND_SET_RIDE_NAME, *((int*)(text + 32)), *((int*)(text + 28)));
}

/**
 * 
 * rct2: 0x006AF55A
 */
static void window_ride_main_unknown_14()
{
	rct_window *w;

	window_get_register(w);

	window_ride_init_viewport(w);
}

/**
 * 
 * rct2: 0x006AECF6
 */
static void window_ride_main_invalidate()
{
	rct_window *w;
	rct_widget *widgets;
	int i;

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

	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;
	window_ride_main_widgets[WIDX_OPEN].image = SPR_CLOSED + ride->status;

	window_ride_anchor_border_widgets(w);

	// Anchor main page specific widgets
	window_ride_main_widgets[WIDX_VIEWPORT].right = w->width - 26;
	window_ride_main_widgets[WIDX_VIEWPORT].bottom = w->height - 14;
	window_ride_main_widgets[WIDX_STATUS].right = w->width - 26;
	window_ride_main_widgets[WIDX_STATUS].top = w->height - 13;
	window_ride_main_widgets[WIDX_STATUS].bottom = w->height - 3;
	for (i = WIDX_OPEN; i <= WIDX_DEMOLISH; i++) {
		window_ride_main_widgets[i].left = w->width - 25;
		window_ride_main_widgets[i].right = w->width - 2;
	}
	window_ride_main_widgets[WIDX_VIEW].right = w->width - 60;
	window_ride_main_widgets[WIDX_VIEW_DROPDOWN].right = w->width - 61;
	window_ride_main_widgets[WIDX_VIEW_DROPDOWN].left = w->width - 71;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 * 
 * rct2: 0x006AF10A
 */
static rct_string_id window_ride_get_status_overall_view(rct_window *w, void *arguments)
{
	int formatSecondary, argument;
	rct_string_id stringId;

	ride_get_status(w->number, &formatSecondary, &argument);
	RCT2_GLOBAL((int)arguments + 0, uint16) = formatSecondary;
	RCT2_GLOBAL((int)arguments + 2, uint32) = argument;
	stringId = STR_LOSS;
	if (formatSecondary != STR_BROKEN_DOWN && formatSecondary != STR_CRASHED)
		stringId = STR_COST;

	return stringId;
}

/**
 * 
 * rct2: 0x006AEFEF
 */
static rct_string_id window_ride_get_status_vehicle(rct_window *w, void *arguments)
{
	rct_ride *ride;
	rct_vehicle *vehicle;
	int vehicleIndex;
	uint16 vehicleSpriteIndex;
	rct_string_id stringId;

	ride = GET_RIDE(w->number);

	vehicleIndex = w->ride.view - 1;
	vehicleSpriteIndex = ride->vehicles[vehicleIndex];
	if (vehicleSpriteIndex == 0xFFFF)
		return 0;

	vehicle = &(g_sprite_list[vehicleSpriteIndex].vehicle);
	if (vehicle->status != VEHICLE_STATUS_CRASHING && vehicle->status != VEHICLE_STATUS_CRASHED) {
		int ax = vehicle->var_36 / 4;
		if (ax == 216 || ax == 123 || ax == 9 || ax == 63 || ax == 147 || ax == 155) {
			if (!(RCT2_ADDRESS(0x01357644, uint32)[ride->type] & 0x40) && vehicle->velocity == 0) {
				RCT2_GLOBAL((int)arguments + 0, uint16) = STR_STOPPED_BY_BLOCK_BRAKES;
				return 1191;
			}
		}
	}

	stringId = STR_MOVING_TO_END_OF + vehicle->status;

	// Get speed in mph
	RCT2_GLOBAL((int)arguments + 2, uint16) = (abs(vehicle->velocity) * 9) >> 18;

	if (ride->type == RIDE_TYPE_MINI_GOLF)
		return 0;

	if ((RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 0x100) && stringId > 1104)
		stringId += 23;

	RCT2_GLOBAL((int)arguments + 4, uint16) = RideNameConvention[ride->type].station_name;
	RCT2_GLOBAL((int)arguments + 6, uint16) = vehicle->var_4B + 1;
	if (ride->num_stations > 1)
		RCT2_GLOBAL((int)arguments + 4, uint16) += 6;

	RCT2_GLOBAL((int)arguments + 0, uint16) = stringId;
	return stringId != 1106 && stringId != 1107 ? 1191 : 1192;
}

/**
 * 
 * rct2: 0x006AEF65
 */
static rct_string_id window_ride_get_status_station(rct_window *w, void *arguments)
{
	rct_ride *ride;
	int stationIndex, count, queueLength;
	rct_string_id stringId;

	ride = GET_RIDE(w->number);

	count = w->ride.view - ride->num_vehicles - 1;
	stationIndex = -1;
	do {
		stationIndex++;
		if (ride->station_starts[stationIndex] != 0xFFFF)
			count--;
	} while (count >= 0);

	stringId = 0;

	// Entrance / exit
	if (ride->status == RIDE_STATUS_CLOSED) {
		if (ride->entrances[stationIndex] == 0xFFFF)
			stringId = STR_NO_ENTRANCE;
		else if (ride->exits[stationIndex] == 0xFFFF)
			stringId = STR_NO_EXIT;
	} else {
		if (ride->entrances[stationIndex] == 0xFFFF)
			stringId = STR_EXIT_ONLY;
	}

	// Queue length
	if (stringId == 0) {
		queueLength = ride->queue_length[stationIndex];
		RCT2_GLOBAL((int)arguments + 2, uint16) = queueLength;
		stringId = STR_QUEUE_EMPTY;
		if (queueLength == 1)
			stringId++;
		else if (queueLength > 1)
			stringId += 2;
	}

	RCT2_GLOBAL((int)arguments + 0, uint16) = stringId;
	return 1191;
}

/**
 * 
 * rct2: 0x006AEE73
 */
static rct_string_id window_ride_get_status(rct_window *w, void *arguments)
{
	rct_ride *ride = GET_RIDE(w->number);

	if (w->ride.view == 0)
		return window_ride_get_status_overall_view(w, arguments);
	if (w->ride.view <= ride->num_vehicles)
		return window_ride_get_status_vehicle(w, arguments);
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
		return window_ride_get_status_overall_view(w, arguments);
	return window_ride_get_status_station(w, arguments);
}

/**
 * 
 * rct2: 0x006AEE73
 */
static void window_ride_main_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_ride *ride;
	rct_widget *widget;
	rct_string_id stringId;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	// Viewport and ear icon
	if (w->viewport != NULL) {
		window_draw_viewport(dpi, w);
		if (w->viewport->flags & VIEWPORT_FLAG_SOUND_ON)
			gfx_draw_sprite(dpi, SPR_HEARING_VIEWPORT, w->x + 2, w->y + 2, 0);
	}

	// View dropdown
	ride = GET_RIDE(w->number);
	stringId = STR_OVERALL_VIEW;
	if (w->ride.view != 0) {
		stringId = RideNameConvention[ride->type].vehicle_name + 6;
		if (w->ride.view > ride->num_vehicles) {
			RCT2_GLOBAL(0x013CE952 + 2, uint16) = w->ride.view - ride->num_vehicles;
			stringId = RideNameConvention[ride->type].station_name + 6;
		} else {
			RCT2_GLOBAL(0x013CE952 + 2, uint16) = w->ride.view;
		}
	}
	RCT2_GLOBAL(0x013CE952, uint16) = stringId;

	widget = &window_ride_main_widgets[WIDX_VIEW];
	gfx_draw_string_centred(
		dpi,
		1193,
		w->x + (widget->left + widget->right - 11) / 2,
		w->y + widget->top,
		0,
		(void*)0x013CE952
	);

	// Status
	widget = &window_ride_main_widgets[WIDX_STATUS];
	gfx_draw_string_centred_clipped(
		dpi,
		window_ride_get_status(w, (void*)0x013CE952),
		(void*)0x013CE952,
		0,
		w->x + (widget->left + widget->right) / 2,
		w->y + widget->top - 1,
		widget->right - widget->left
	);
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
