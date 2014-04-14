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
#include "date.h"
#include "strings.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"

enum WINDOW_PARK_PAGE {
	WINDOW_PARK_PAGE_ENTRANCE,
	WINDOW_PARK_PAGE_RATING_GRAPH,
	WINDOW_PARK_PAGE_GUESTS_GRAPH,
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
	WIDX_ENTER_NAME
};

static rct_widget window_park_objective_widgets[] = {
	{ WWT_FRAME,			0,	0,		229,	0,		223,	0x0FFFFFFFF,	65535},					// panel / background
	{ WWT_CAPTION,			0,	1,		228,	1,		14,		0x361,			STR_WINDOW_TITLE_TIP},	// title bar
	{ WWT_CLOSEBOX,			0,	217,	227,	2,		13,		0x338,			STR_CLOSE_WINDOW_TIP},	// close x button
	{ WWT_IMGBTN,			1,	0,		229,	43,		173,	0x0FFFFFFFF,	65535},					// tab content panel
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,		2462},					// tab 1
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,		2463},					// tab 2
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,		2464},					// tab 3
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,		2465},					// tab 4
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,		2466},					// tab 5
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,		2467},					// tab 6
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,		2468},					// tab 7
	{ WWT_DROPDOWN_BUTTON,	1,	7,		222,	209,	220,	0x0AE6,			65535},					// enter name
	{ WIDGETS_END },
};

static void window_park_objective_emptysub() { }
static void window_park_objective_mouseup();
static void window_park_objective_update();
static void window_park_objective_invalidate();
static void window_park_objective_paint();

static uint32 window_park_objective_events[] = {
	window_park_objective_emptysub,
	window_park_objective_mouseup,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_update,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_emptysub,
	window_park_objective_invalidate,
	window_park_objective_paint,
	window_park_objective_emptysub
};

static void window_park_align_tabs(rct_window *w);
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
	w->widgets = 0x009A8E68;
	w->enabled_widgets = 0x0003E7F4;
	w->number = 0;
	w->var_48A = WINDOW_PARK_PAGE_ENTRANCE;
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
	window->var_48A = WINDOW_PARK_PAGE_OBJECTIVE;
	window_invalidate(window);
	window->widgets = window_park_objective_widgets;
	window->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_TAB_1) |
		(1 << WIDX_TAB_2) |
		(1 << WIDX_TAB_3) |
		(1 << WIDX_TAB_4) |
		(1 << WIDX_TAB_5) |
		(1 << WIDX_TAB_6) |
		(1 << WIDX_TAB_7) |
		(1 << WIDX_ENTER_NAME);
	window->var_020 = 0;
	window->event_handlers = window_park_objective_events;
	window_init_scroll_widgets(window);
	window->x = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) / 2 - 115;
	window->y = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) / 2 - 87;
	window_invalidate(window);
}

static void window_park_objective_mouseup()
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
		RCT2_CALLPROC_X(0x00668496, 0, 0, 0, widgetIndex, w, 0, 0);
		break;
	case WIDX_ENTER_NAME:
		RCT2_CALLPROC_X(0x006EE308, 2791, 2792, 0, 0, 0, 0, 0);
		break;
	}
}

static void window_park_objective_update()
{
	rct_window *w;

	__asm mov w, esi

	w->var_48E++;
	widget_invalidate(w->classification, w->number, WIDX_TAB_6);
}

static void window_park_objective_invalidate()
{
	int i;
	rct_window *w;

	__asm mov w, esi

	// Set correct active tab
	for (i = 0; i < 7; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1 << (WIDX_TAB_1 + w->var_48A);

	// Set window title arguments
	*((short*)0x013CE952) = RCT2_GLOBAL(0x013573D4, uint16);
	*((short*)0x013CE954) = RCT2_GLOBAL(0x013573D8, uint32);

	// 
	if (RCT2_GLOBAL(0x013573E4, uint32) & 0x02)
		window_park_objective_widgets[WIDX_ENTER_NAME].type = WWT_DROPDOWN_BUTTON;
	else
		window_park_objective_widgets[WIDX_ENTER_NAME].type = WWT_EMPTY;

	// Align the tabs
	window_park_align_tabs(w);

	// Set anchored widgets
	window_park_objective_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_park_objective_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	window_park_objective_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	window_park_objective_widgets[WIDX_PAGE_BACKGROUND].bottom = w->width - 1;
	window_park_objective_widgets[WIDX_TITLE].right = w->width - 2;
	window_park_objective_widgets[WIDX_CLOSE].left = w->width - 13;
	window_park_objective_widgets[WIDX_CLOSE].right = w->width - 2;
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

static void window_park_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	int sprite_idx;

	// Entrance tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_1)))
		gfx_draw_sprite(dpi, 5200, w->x + w->widgets[WIDX_TAB_1].left, w->y + w->widgets[WIDX_TAB_1].top);

	// Rating tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_2))) {
		sprite_idx = 5245;
		if (w->var_48A == WINDOW_PARK_PAGE_RATING_GRAPH)
			sprite_idx += (w->var_48E / 8) % 8;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_2].left, w->y + w->widgets[WIDX_TAB_2].top);
		gfx_draw_sprite(dpi, SPR_RATING_HIGH, w->x + w->widgets[WIDX_TAB_2].left + 7, w->y + w->widgets[WIDX_TAB_2].top + 1);
		gfx_draw_sprite(dpi, SPR_RATING_LOW, w->x + w->widgets[WIDX_TAB_2].left + 16, w->y + w->widgets[WIDX_TAB_2].top + 12);
	}

	// Price tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_4))) {
		sprite_idx = 5253;
		if (w->var_48A == WINDOW_PARK_PAGE_PRICE)
			sprite_idx += (w->var_48E / 2) % 8;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_4].left, w->y + w->widgets[WIDX_TAB_4].top);
	}

	// Statistics tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_5))) {
		sprite_idx = 5277;
		if (w->var_48A == WINDOW_PARK_PAGE_STATS)
			sprite_idx += (w->var_48E / 4) % 7;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_5].left, w->y + w->widgets[WIDX_TAB_5].top);
	}

	// Objective tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_6))) {
		sprite_idx = 5511;
		if (w->var_48A == WINDOW_PARK_PAGE_OBJECTIVE)
			sprite_idx += (w->var_48E / 4) % 16;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_6].left, w->y + w->widgets[WIDX_TAB_6].top);
	}

	// Awards tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_7)))
		gfx_draw_sprite(dpi, 5527, w->x + w->widgets[WIDX_TAB_7].left, w->y + w->widgets[WIDX_TAB_7].top);
}