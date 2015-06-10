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
	{ WWT_CAPTION,			0,	1,			WW - 2,		1,			14,			STR_SACK_STAFF,			STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	WW-13,		WW - 3,		2,			13,			STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },
	{ WWT_DROPDOWN_BUTTON,	0,	10,			94,			WH - 20,	WH - 9,		STR_YES,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	0,	WW - 95,	WW - 11,	WH - 20,	WH - 9,		STR_SAVE_PROMPT_CANCEL,	STR_NONE },
	{ WIDGETS_END }
};

static void window_staff_fire_emptysub(){}
static void window_staff_fire_mouseup();
static void window_staff_fire_invalidate();
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
	window_staff_fire_invalidate,
	window_staff_fire_paint,
	window_staff_fire_emptysub
};
/** Based off of rct2: 0x6C0A77 */
void window_staff_fire_prompt_open(rct_peep* peep){
	// Check if the confirm window already exists.
	if (window_bring_to_front_by_number(WC_FIRE_PROMPT, peep->sprite_index)) {
		return;
	}

	rct_window* w = window_create_centred(WW, WH, (uint32*)0x992C3C, 0x1A, 0);
	w->widgets = window_staff_fire_widgets;
	w->enabled_widgets |= (1 << WIDX_CLOSE) | (1 << WIDX_YES) | (1 << WIDX_CANCEL);

	window_init_scroll_widgets(w);

	colour_scheme_update(w);

	w->flags |= WF_TRANSPARENT;
	w->number = peep->sprite_index;
}


/**
*
*  rct2: 0x006C0B40
*/
static void window_staff_fire_mouseup(){
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	rct_peep* peep = &g_sprite_list[w->number].peep;
	
	switch (widgetIndex){
	case WIDX_YES:
		game_do_command(peep->x, 1, peep->y, w->number, GAME_COMMAND_FIRE_STAFF_MEMBER, 0, 0);
		break;
	case WIDX_CANCEL:
	case WIDX_CLOSE:
		window_close(w);
	}
}

static void window_staff_fire_invalidate()
{
	rct_window *w;

	window_get_register(w);
	colour_scheme_update(w);
}

/**
*
*  rct2: 0x006C0AF2
*/
static void window_staff_fire_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	rct_peep* peep = &g_sprite_list[w->number].peep;
	
	RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
	RCT2_GLOBAL(0x13CE954, uint32) = peep->id;
	
	int x = w->x + WW / 2;
	int y = w->y + (WH / 2) - 3;
	
	gfx_draw_string_centred_wrapped(dpi, (void*)0x13CE952, x, y, WW - 4, STR_FIRE_STAFF_ID, 0);
}
