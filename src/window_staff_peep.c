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
#include "game.h"
#include "peep.h"
#include "string_ids.h"
#include "sprite.h"
#include "sprites.h"
#include "viewport.h"
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"

enum WINDOW_STAFF_PEEP_PAGE {
	WINDOW_STAFF_PEEP_OVERVIEW,
	WINDOW_STAFF_PEEP_OPTIONS,
	WINDOW_STAFF_PEEP_STATISTICS,
};

enum WINDOW_STAFF_PEEP_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_RESIZE,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,
	WIDX_TAB_4,
	WIDX_VIEWPORT,
	WIDX_BTM_LABEL,
	WIDX_PICKUP,
	WIDX_PATROL,
	WIDX_RENAME,
	WIDX_LOCATE,
	WIDX_FIRE
};

void window_staff_peep_emptysub(){};

rct_widget window_staff_peep_overview_widgets[] = { 
	{ WWT_FRAME,	0, 0,	189,	0,		179,	0x0FFFFFFFF,	STR_NONE },				// Panel / Background
	{ WWT_CAPTION,	0, 1,	188,	1,		14,		0x361,			STR_WINDOW_TITLE_TIP }, // Title
	{ WWT_CLOSEBOX, 0, 177, 187,	2,		13,		0x338,			STR_CLOSE_WINDOW_TIP }, // Close x button
	{ WWT_RESIZE,	1, 0,	189,	43,		179,	0x0FFFFFFFF,	STR_NONE },				// Resize
	{ WWT_TAB,		1, 3,	33,		17,		43,		0x2000144E,		1939 },					// Tab 1
	{ WWT_TAB,		1, 34,	64,		17,		43,		0x2000144E,		1945},					// Tab 2
	{ WWT_TAB,		1, 65,	95,		17,		43,		0x2000144E,		2348},					// Tab 3
	{ WWT_TAB,		1, 96,	126,	17,		43,		0x2000144E,		STR_NONE},				// Tab 4
	{ WWT_VIEWPORT,	1, 3,	164,	47,		166,	0x0FFFFFFFF,	STR_NONE},				// Viewport
	{ WWT_12,		1, 3,	164,	167,	177,	0x0FFFFFFFF,	STR_NONE },				// Label at bottom of viewport
	{ WWT_FLATBTN,	1, 165,	188,	45,		68,		0x1436,			1706},					// Pickup Button
	{ WWT_FLATBTN,	1, 165, 188,	69,		92,		0x1437,			1708},					// Patrol Button
	{ WWT_FLATBTN,	1, 165, 188,	93,		116,	0x1430,			1056},					// Rename Button
	{ WWT_FLATBTN,	1, 165, 188,	117,	140,	0x142F,			1027},					// Locate Button
	{ WWT_FLATBTN,	1, 165, 188,	141,	164,	0x142D,			1705},					// Fire Button
	{ WIDGETS_END },
};

rct_widget *window_staff_peep_page_widgets[] = {
	window_staff_peep_overview_widgets,
	(rct_widget *)0x9AF910,
	(rct_widget *)0x9AF9F4
};

void window_staff_peep_set_page(rct_window* w, int page);
void window_staff_peep_disable_widgets(rct_window* w);

void window_staff_peep_close();
void window_staff_peep_mouse_up();

// 0x992AEC
static void* window_staff_peep_overview_events[] = {
	window_staff_peep_close,
	window_staff_peep_mouse_up,
	(void*)0x6BE558,
	(void*)0x6BDF98,
	(void*)0x6BDFA3,
	window_staff_peep_emptysub,
	(void*)0x6BE602,
	window_staff_peep_emptysub,
	window_staff_peep_emptysub,
	(void*)0x6BDFD8,
	(void*)0x6BDFC3,
	window_staff_peep_emptysub,
	window_staff_peep_emptysub,
	(void*)0x6BDFAE,
	window_staff_peep_emptysub,
	window_staff_peep_emptysub,
	window_staff_peep_emptysub,
	window_staff_peep_emptysub,
	window_staff_peep_emptysub,
	(void*)0x6BDFED,
	(void*)0x6BE5FC,
	window_staff_peep_emptysub,
	window_staff_peep_emptysub,
	window_staff_peep_emptysub,
	window_staff_peep_emptysub,
	(void*)0x6BDD91, //Invalidate
	(void*)0x6BDEAF, //Paint
	(void*)0x6BE62A
};

void* window_staff_peep_page_events[] = {
	window_staff_peep_overview_events,
	(void*)0x992B5C,
	(void*)0x992BCC
};

uint32 window_staff_peep_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_PICKUP) |
	(1 << WIDX_PATROL) |
	(1 << WIDX_RENAME) |
	(1 << WIDX_LOCATE) |
	(1 << WIDX_FIRE),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3)
};

/**
*
*  rct2: 0x006BEE98
*/
void window_staff_peep_open(rct_peep* peep)
{
	rct_window* w = window_bring_to_front_by_id(WC_PEEP, peep->sprite_index);
	if (w == NULL) {
		w = window_create_auto_pos(190, 180, (uint32*)window_staff_peep_overview_events, WC_PEEP, (uint16)0x400);

		w->widgets = RCT2_GLOBAL(0x9AF81C, rct_widget*);
		w->enabled_widgets = RCT2_GLOBAL(0x9929B0, uint32);
		w->number = peep->sprite_index;
		w->page = 0;
		w->viewport_focus_coordinates.y = 0;
		w->frame_no = 0;

		RCT2_GLOBAL((int*)w + 0x496, uint16) = 0; // missing, var_494 should perhaps be uint16?

		window_staff_peep_disable_widgets(w);

		w->min_width = 190;
		w->min_height = 180;
		w->max_width = 500;
		w->max_height = 450;

		w->flags = 1 << 8;

		w->colours[0] = 1;
		w->colours[1] = 4;
		w->colours[2] = 4;
	}
	w->page = 0;
	window_invalidate(w);

	w->widgets = window_staff_peep_overview_widgets;
	w->enabled_widgets = window_staff_peep_page_enabled_widgets[0];
	w->var_020 = RCT2_GLOBAL(0x9929BC, uint32);
	w->event_handlers = window_staff_peep_page_events[0];
	w->pressed_widgets = 0;
	window_staff_peep_disable_widgets(w);
	window_init_scroll_widgets(w);
	RCT2_CALLPROC_X(0x006BEDA3, 0, 0, 0, 0, (int)w, 0, 0);
	if (g_sprite_list[w->number].peep.state == PEEP_STATE_PICKED) {
		RCT2_CALLPROC_X(w->event_handlers[WE_MOUSE_UP], 0, 0, 0, 10, (int)w, 0, 0);
	}
}

/**
* rct2: 0x006BED21
* Disable the staff pickup if not in pickup state.
*/
void window_staff_peep_disable_widgets(rct_window* w)
{
	rct_peep* peep = &g_sprite_list[w->number].peep;

	int eax = 0 | 0x80;

	if (peep->staff_type == 2) {
		eax |= 0x20;
	}

	//RCT2_CALLFUNC_X(0x698827, 0, 0, 0, 0, 0, 0, 0);
	// sub_698827
	// This is here due to needing the Carry Flag.

	int CF = 0;
	int res = RCT2_GLOBAL(0x982004 + peep->state, uint8) & 1;

	if (res == 0) {
		CF = 1;
	}
	else {
		eax = eax & eax;
	}

	// end sub_698827

	int a = 0;

	// pop esi
	if (CF == 1 && w->page == 0) {
		eax |= 0x400; //or      eax, 400h

		a = w->disabled_widgets & (1 << 0xA); //bt      dword ptr[esi + 10h], 0Ah

	}

	if (a == 0) {
		CF = w->disabled_widgets & (1 << 0xA); //bt      dword ptr [esi+10h], 0Ah
		if (CF == 1) {
			window_invalidate(w);
		}
	}

	w->disabled_widgets = eax;
}

/**
 * Same as window_peep_close.
 * rct2: 0x006BDFF8
 */
void window_staff_peep_close()
{
	rct_window* w;

	window_get_register(w);

	if (RCT2_GLOBAL(0x9DE518, uint32) & (1 << 3)){
		if (w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) &&
			w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber))
			tool_cancel();
	}
}

/** rct2: 0x6C0A77 */
void window_staff_peep_fire(rct_window* w)
{
	RCT2_CALLPROC_X(0x6C0A77, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 * Mostly similar to window_peep_set_page.
 * rct2: 0x006BE023
 */
void window_staff_peep_set_page(rct_window* w, int page)
{
	if (RCT2_GLOBAL(0x9DE518,uint32) & (1 << 3))
	{
		if(w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber) &&
		   w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass))
			tool_cancel();
	
	}
	
	int listen = 0;
	if (page == WINDOW_STAFF_PEEP_OVERVIEW && w->page == WINDOW_STAFF_PEEP_OVERVIEW && w->viewport){
		if (!(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
			listen = 1;
	}

	
	w->page = page;
	w->frame_no = 0;

	rct_viewport* viewport = w->viewport;
	w->viewport = 0;
	if (viewport){
		viewport->width = 0;
	}

	w->enabled_widgets = window_staff_peep_page_enabled_widgets[page];
	w->var_020 = RCT2_ADDRESS(0x9929BC, uint32)[page];
	w->event_handlers = window_staff_peep_page_events[page];
	w->pressed_widgets = 0;
	w->widgets = window_staff_peep_page_widgets[page];

	window_staff_peep_disable_widgets(w);
	window_invalidate(w);

	RCT2_CALLPROC_X(w->event_handlers[WE_RESIZE], 0, 0, 0, 0, (int)w, 0, 0);
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);

	window_init_scroll_widgets(w);
	window_invalidate(w);

	if (listen && w->viewport) w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

/** rct2: 0x006BDF55 */
void window_staff_peep_mouse_up()
{
	short widgetIndex;
	rct_window* w;
	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
		
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
		window_staff_peep_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_LOCATE: // 0xD
		window_scroll_to_viewport(w);
		break;
	case WIDX_PICKUP: // 0xA
		// 0x6BE236
		break;
	case WIDX_FIRE: // 0xE
		window_staff_peep_fire(w);
		break;
	case WIDX_RENAME: // 0xC
		// 6BE4BC
		break;
	}
}