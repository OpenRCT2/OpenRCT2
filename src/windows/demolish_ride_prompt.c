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

#include "../addresses.h"
#include "../game.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../sprites.h"
#include "../world/sprite.h"
#include "../interface/themes.h"

#define WW 200
#define WH 100

enum WINDOW_RIDE_DEMOLISH_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_DEMOLISH,
	WIDX_CANCEL
};

// 0x009AEBA0
static rct_widget window_ride_demolish_widgets[] = {
		{ WWT_FRAME,			0, 0,		WW - 1,		0,			WH - 1, STR_NONE,				STR_NONE },
		{ WWT_CAPTION,			0, 1,		WW - 2,		1,			14,		STR_DEMOLISH_RIDE,		STR_WINDOW_TITLE_TIP },
		{ WWT_CLOSEBOX,			0, WW - 13, WW - 3,		2,			13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },
		{ WWT_DROPDOWN_BUTTON,	0, 10,		94,			WH - 20,	WH - 9, STR_DEMOLISH,			STR_NONE },
		{ WWT_DROPDOWN_BUTTON,	0, WW - 95, WW - 11,	WH - 20,	WH - 9, STR_SAVE_PROMPT_CANCEL, STR_NONE },
		{ WIDGETS_END }
};

static void window_ride_demolish_emptysub(){}
static void window_ride_demolish_mouseup();
static void window_ride_demolish_invalidate();
static void window_ride_demolish_paint();

//0x0098E2E4
static void* window_ride_demolish_events[] = {
	window_ride_demolish_emptysub,
	window_ride_demolish_mouseup,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_emptysub,
	window_ride_demolish_invalidate,
	window_ride_demolish_paint,
	window_ride_demolish_emptysub
};

/** Based off of rct2: 0x006B486A */
void window_ride_demolish_prompt_open(int rideIndex){
	rct_window *w;

	w = window_bring_to_front_by_number(WC_DEMOLISH_RIDE_PROMPT, rideIndex);
	if (w != NULL)
		return;

	w = window_create_centred(WW, WH, (uint32*)window_ride_demolish_events, WC_DEMOLISH_RIDE_PROMPT, WF_TRANSPARENT);
	w->widgets = window_ride_demolish_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_CANCEL) | (1 << WIDX_DEMOLISH);
	window_init_scroll_widgets(w);
	w->number = rideIndex;
}


/**
*
*  rct2: 0x006B4933
*/
static void window_ride_demolish_mouseup(){
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex){
	case WIDX_DEMOLISH:
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = STR_CANT_DEMOLISH_RIDE;
		game_do_command(0, 1, 0, w->number, GAME_COMMAND_7, 0, 0);
		break;
	case WIDX_CANCEL:
	case WIDX_CLOSE:
		window_close(w);
	}
}

static void window_ride_demolish_invalidate()
{
	rct_window *w;

	window_get_register(w);
	colour_scheme_update(w);
}

/**
*
*  rct2: 0x006B48E5
*/
static void window_ride_demolish_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	rct_ride* ride = GET_RIDE(w->number);

	RCT2_GLOBAL(0x13CE952, uint16) = ride->name;
	RCT2_GLOBAL(0x13CE954, uint32) = ride->name_arguments;

	int x = w->x + WW / 2;
	int y = w->y + (WH / 2) - 3;

	gfx_draw_string_centred_wrapped(dpi, (void*)0x13CE952, x, y, WW - 4, STR_DEMOLISH_RIDE_ID, 0);
}
