/*****************************************************************************
* Copyright (c) 2014 Ted John, Duncan Frost
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
#include "peep.h"
#include "string_ids.h"
#include "sprite.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"
#include "staff.h"

#define WW 200
#define WH 100

enum WINDOW_STAFF_FIRE_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_YES,
	WIDX_CANCEL
};

// 0x9AFB4C
static rct_widget window_staff_fire_widgets[] = {
	{ WWT_FRAME,			0,	0,			WW - 1,		0,			WH - 1,		STR_NONE,				STR_NONE },
	{ WWT_CAPTION,			0,	1,			WW - 2,		1,			14,			STR_OPTIONS,			STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	WW-13,		WW - 3,		2,			13,			STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },
	{ WWT_DROPDOWN_BUTTON,	0,	10,			94,			WH - 20,	WH - 9,		STR_YES,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	WW - 95,	WW - 11,	WH - 10,	WH - 9,		STR_SAVE_PROMPT_CANCEL,	STR_NONE },
	{ WIDGETS_END }
};

static void window_staff_fire_emptysub(){}
static void window_staff_fire_mouseup();
static void window_staff_fire_paint();

//0x9A3F7C
static void* window_staff_fire_events[] = {
	window_staff_fire_emptysub,
	window_staff_fire_mouseup,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_emptysub,
	window_staff_fire_paint,
	window_staff_fire_emptysub
};
/** Based off of rct2: 0x6C0A77 */
void window_staff_fire_open(rct_peep* peep){
	// Check if the confirm window already exists.
	if (window_bring_to_front_by_id(WC_FIRE_PROMPT, peep->sprite_index)) {
		return;
	}

	// Find center of the screen.
	int screen_height = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16);
	int screen_width = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16);
	int x = screen_width / 2 - WW / 2;
	int y = screen_height / 2 - WH / 2;

	rct_window* w = window_create(x, y, WW, WH, (uint32*)0x992C3C, 0x1A, 0);
	w->widgets = window_staff_fire_widgets;
	w->enabled_widgets |= (1 << WIDX_CLOSE) | (1 << WIDX_YES) | (1 << WIDX_CANCEL);

	window_init_scroll_widgets(w);

	w->flags |= WF_TRANSPARENT;
	w->number = peep->sprite_index;
	w->colours[0] = 0x9A;
}


/**
*
*  rct2: 0x006C0B40
*/
static void window_shortcut_change_mouseup(){
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex){
	case WIDX_YES:
		//6c0b5b
		break;
	case WIDX_CANCEL:
	case WIDX_CLOSE:
		window_close(w);
	}
}

/**
*
*  rct2: 0x006C0AF2
*/
static void window_shortcut_change_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	//To be finished
}