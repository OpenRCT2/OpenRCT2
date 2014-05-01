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
#include "park.h"
#include "peep.h"
#include "strings.h"
#include "sprite.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"


#define WW 200
#define WH 128

enum {
	WINDOW_CHEATS_PAGE_MONEY,
	WINDOW_CHEATS_PAGE_GUESTS
};

static enum WINDOW_CHEATS_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_HIGH_MONEY,
	WIDX_HAPPY_GUESTS
};

static rct_widget window_cheats_money_widgets[] = {
	{ WWT_FRAME,			0,	0,			WW - 1,	0,		WH - 1,	0x0FFFFFFFF,	65535},					// panel / background
	{ WWT_CAPTION,			0,	1,			WW - 2,	1,		14,		3165,			STR_WINDOW_TITLE_TIP},	// title bar
	{ WWT_CLOSEBOX,			0,	WW - 13,	WW - 3,	2,		13,		0x338,			STR_CLOSE_WINDOW_TIP},	// close x button
	{ WWT_IMGBTN,			1,	0,			WW - 1,	43,		WH - 1,	0x0FFFFFFFF,	65535},					// tab content panel
	{ WWT_TAB,				1,	3,			33,		17,		43,		0x2000144E,		2462},					// tab 1
	{ WWT_TAB,				1,	34,			64,		17,		43,		0x2000144E,		2462},					// tab 2
	{ WWT_CLOSEBOX,			1,	4,			74,		47,		63,		2375,			2375 },					// high money
	{ WIDGETS_END },
};

static rct_widget window_cheats_guests_widgets[] = {
	{ WWT_FRAME,			0, 0,			WW - 1, 0,	WH - 1,		0x0FFFFFFFF,	65535 },				// panel / background
	{ WWT_CAPTION,			0, 1,			WW - 2, 1,	14,			3165,			STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,			0, WW - 13,		WW - 3, 2,	13,			0x338,			STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_IMGBTN,			1, 0,			WW - 1, 43, WH - 1,		0x0FFFFFFFF,	65535 },				// tab content panel
	{ WWT_TAB,				1, 3,			33,		17, 43,			0x2000144E,		2462 },					// tab 1
	{ WWT_TAB,				1, 34,			64,		17, 43,			0x2000144E,		2462 },					// tab 2
	{ WWT_CLOSEBOX,			1, 4,			74,		47, 63,			2375,			2375 },					// happy guests
	{ WIDGETS_END },
};

static rct_widget *window_cheats_page_widgets[] = {
	window_cheats_money_widgets,
	window_cheats_guests_widgets
};

static void window_cheats_emptysub() { }
static void window_cheats_mouseup();
static void window_cheats_update();
static void window_cheats_invalidate();
static void window_cheats_paint();
static void window_cheats_set_page(rct_window *w, int page);

static uint32 window_cheats_money_events[] = {
	window_cheats_emptysub,
	window_cheats_mouseup,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_update,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_invalidate,
	window_cheats_paint,
	window_cheats_emptysub
};

static uint32 window_cheats_guests_events[] = {
	window_cheats_emptysub,
	window_cheats_mouseup,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_update,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_emptysub,
	window_cheats_invalidate,
	window_cheats_paint,
	window_cheats_emptysub
};

static uint32 *window_cheats_page_events[] = {
	window_cheats_money_events,
	window_cheats_guests_events,
};

static uint32 window_cheats_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) | (1 << WIDX_TAB_1) | (1 << WIDX_TAB_2) | (1 << WIDX_HIGH_MONEY),
	(1 << WIDX_CLOSE) | (1 << WIDX_TAB_1) | (1 << WIDX_TAB_2) | (1 << WIDX_HIGH_MONEY) //Change to WIDX_HAPPY_GUESTSs when enabled widgets is figured out.
};

static void window_cheats_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

void window_cheats_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_id(WC_CHEATS, 0);
	if (window != NULL)
		return;

	window = window_create(32, 32, WW, WH, window_cheats_money_events, WC_CHEATS, 0);
	window->widgets = window_cheats_money_widgets;
	window->enabled_widgets = window_cheats_page_enabled_widgets[0];
	window_init_scroll_widgets(window);
	window->page = WINDOW_CHEATS_PAGE_MONEY;
	window->colours[0] = 1;
	window->colours[1] = 19;
	window->colours[2] = 19;
}

static void window_cheats_mouseup()
{
	int i;
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi
	rct_peep* peep;
	uint16 sprite_idx;

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
		window_cheats_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_HIGH_MONEY:
		if (w->page == WINDOW_CHEATS_PAGE_MONEY){
			i = DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32));
			i += 100000;
			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32) = ENCRYPT_MONEY(i);
		}
		else{
			for (sprite_idx = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16); sprite_idx != SPRITE_INDEX_NULL; sprite_idx = peep->next) {
				peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_idx].peep);
				if (peep->type != PEEP_TYPE_GUEST)
					continue;
				if (peep->var_2A != 0)
					continue;
				peep->happiness = 255;
			}
		}
		window_invalidate_by_id(0x40 | WC_BOTTOM_TOOLBAR, 0);
		break;
	case WIDX_HAPPY_GUESTS:
		//At present HAPPY_GUESTS does not activate
		window_invalidate_by_id(0x40 | WC_BOTTOM_TOOLBAR, 0);
		break;
	}
}

static void window_cheats_update()
{
	rct_window *w;

	__asm mov w, esi

	w->var_48E++;
	widget_invalidate(w->classification, w->number, WIDX_TAB_1+w->page);
}

static void window_cheats_invalidate()
{
	int i;
	rct_window *w;

	__asm mov w, esi
	strcpy((char*)0x009BC677, "Cheats");

	rct_widget **widgets = window_cheats_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	// Set correct active tab
	for (i = 0; i < 7; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1 << (WIDX_TAB_1 + w->page);
}

static void window_cheats_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);
	window_cheats_draw_tab_images(dpi, w);
}

static void window_cheats_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	int sprite_idx;

	// Money tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_1))) {
		sprite_idx = 5261;
		if (w->page == WINDOW_CHEATS_PAGE_MONEY)
			sprite_idx += (w->var_48E / 2) % 8;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_1].left, w->y + w->widgets[WIDX_TAB_1].top);
	}

	// Guests tab
	if (!(w->disabled_widgets & (1 << WIDX_TAB_2))) {
		sprite_idx = 5568;
		if (w->page == WINDOW_CHEATS_PAGE_GUESTS)
			sprite_idx += (w->var_48E / 2) % 8;
		gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_2].left, w->y + w->widgets[WIDX_TAB_2].top);
	}
}

static void window_cheats_set_page(rct_window *w, int page)
{
	w->page = page;
	
	w->enabled_widgets = window_cheats_page_enabled_widgets[page];
	
	w->event_handlers = window_cheats_page_events[page];
	w->widgets = window_cheats_page_widgets[page];

	window_invalidate(w);
}