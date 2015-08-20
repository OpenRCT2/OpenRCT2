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
#include "../config.h"
#include "../game.h"
#include "../input.h"
#include "../management/marketing.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../ride/ride.h"
#include "../ride/ride_data.h"
#include "../scenario.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../interface/themes.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../world/footpath.h"
#include "../world/map.h"
#include "../world/sprite.h"
#include "dropdown.h"
#include "error.h"

enum WINDOW_GUEST_PAGE {
	WINDOW_GUEST_OVERVIEW,
	WINDOW_GUEST_STATS,
	WINDOW_GUEST_RIDES,
	WINDOW_GUEST_FINANCE,
	WINDOW_GUEST_THOUGHTS,
	WINDOW_GUEST_INVENTORY
};

enum WINDOW_GUEST_WIDGET_IDX {
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

	WIDX_MARQUEE = 10,
	WIDX_VIEWPORT,
	WIDX_ACTION_LBL,	
	WIDX_PICKUP,
	WIDX_RENAME,
	WIDX_LOCATE,
	WIDX_TRACK,
	
	WIDX_RIDE_SCROLL = 10
};

rct_widget window_guest_overview_widgets[] = { 
	{ WWT_FRAME,	0, 0,	191,	0,		156,	0x0FFFFFFFF,	STR_NONE },				// Panel / Background
	{ WWT_CAPTION,	0, 1,	190,	1,		14,		865,			STR_WINDOW_TITLE_TIP }, // Title
	{ WWT_CLOSEBOX, 0, 179, 189,	2,		13,		824,			STR_CLOSE_WINDOW_TIP }, // Close x button
	{ WWT_RESIZE,	1, 0,	191,	43,		156,	0x0FFFFFFFF,	STR_NONE },				// Resize
	{ WWT_TAB,		1, 3,	33,		17,		43,		0x2000144E,		1938 },					// Tab 1
	{ WWT_TAB,		1, 73,	64,		17,		43,		0x2000144E,		1940},					// Tab 2
	{ WWT_TAB,		1, 65,	95,		17,		43,		0x2000144E,		1941},					// Tab 3
	{ WWT_TAB,		1, 96,	126,	17,		43,		0x2000144E,		1942},					// Tab 4
	{ WWT_TAB,		1, 127, 157,	17,		43,		0x2000144E,		1943},					// Tab 5
	{ WWT_TAB,		1, 158, 188,	17,		43,		0x2000144E,		1944},					// Tab 6
	{ WWT_12,		1, 3,	166,	45,		56,		0x0FFFFFFFF,	STR_NONE},				// Label Thought marquee
	{ WWT_VIEWPORT, 1, 3,	166,	57,		143,	0x0FFFFFFFF,	STR_NONE },				// Viewport
	{ WWT_12,		1, 3,	166,	144,	154,	0x0FFFFFFFF,	STR_NONE},				// Label Action
	{ WWT_FLATBTN,	1, 167, 190,	45,		68,		0x1436,			1706},					// Pickup Button
	{ WWT_FLATBTN,	1, 167, 190,	69,		92,		SPR_RENAME,		1055},					// Rename Button
	{ WWT_FLATBTN,	1, 167, 190,	93,		116,	SPR_LOCATE,		STR_LOCATE_SUBJECT_TIP},// Locate Button
	{ WWT_FLATBTN,	1, 167, 190,	117,	140,	SPR_TRACK_PEEP,	1930},					// Track Button
	{ WIDGETS_END },
};

rct_widget window_guest_stats_widgets[] = {
	{WWT_FRAME,		0, 0,	191,	0,	156,	-1,			STR_NONE},
	{WWT_CAPTION,	0, 1,	190,	1,	14,		865,		STR_WINDOW_TITLE_TIP},
	{WWT_CLOSEBOX,	0, 179,	189,	2,	13,		824,		STR_CLOSE_WINDOW_TIP},
	{WWT_RESIZE,	1, 0,	191,	43,	156,	-1,			STR_NONE},
	{WWT_TAB,		1, 3,	33,		17,	43,		0x2000144E,	1938},
	{WWT_TAB,		1, 34,	64,		17,	43,		0x2000144E,	1940},
	{WWT_TAB,		1, 65,	95,		17,	43,		0x2000144E,	1941},
	{WWT_TAB,		1, 96,	126,	17,	43,		0x2000144E,	1942},
	{WWT_TAB,		1, 127,	157,	17,	43,		0x2000144E,	1943},
	{WWT_TAB,		1, 158,	188,	17,	43,		0x2000144E,	1944},
	{WIDGETS_END},
};

rct_widget window_guest_rides_widgets[] = {
	{WWT_FRAME,		0, 0,	191,	0,	156,	-1,			STR_NONE},
	{WWT_CAPTION,	0, 1,	190,	1,	14,		865,		STR_WINDOW_TITLE_TIP},
	{WWT_CLOSEBOX,	0, 179,	189,	2,	13,		824,		STR_CLOSE_WINDOW_TIP},
	{WWT_RESIZE,	1, 0,	191,	43,	156,	-1,			STR_NONE},
	{WWT_TAB,		1, 3,	33,		17,	43,		0x2000144E,	1938},
	{WWT_TAB,		1, 34,	64,		17,	43,		0x2000144E,	1940},
	{WWT_TAB,		1, 65,	95,		17,	43,		0x2000144E,	1941},
	{WWT_TAB,		1, 96,	126,	17,	43,		0x2000144E,	1942},
	{WWT_TAB,		1, 127,	157,	17,	43,		0x2000144E,	1943},
	{WWT_TAB,		1, 158,	188,	17,	43,		0x2000144E,	1944},
	{WWT_SCROLL,	1, 3,	188,	57,	143,	2,			STR_NONE},
	{WIDGETS_END},
};

rct_widget window_guest_finance_widgets[] = {
	{WWT_FRAME,		0, 0,	191,	0,	156,	-1,			STR_NONE},
	{WWT_CAPTION,	0, 1,	190,	1,	14,		865,		STR_WINDOW_TITLE_TIP},
	{WWT_CLOSEBOX,	0, 179,	189,	2,	13,		824,		STR_CLOSE_WINDOW_TIP},
	{WWT_RESIZE,	1, 0,	191,	43,	156,	-1,			STR_NONE},
	{WWT_TAB,		1, 3,	33,		17,	43,		0x2000144E,	1938},
	{WWT_TAB,		1, 34,	64,		17,	43,		0x2000144E,	1940},
	{WWT_TAB,		1, 65,	95,		17,	43,		0x2000144E,	1941},
	{WWT_TAB,		1, 96,	126,	17,	43,		0x2000144E,	1942},
	{WWT_TAB,		1, 127,	157,	17,	43,		0x2000144E,	1943},
	{WWT_TAB,		1, 158,	188,	17,	43,		0x2000144E,	1944},
	{WIDGETS_END},
};

rct_widget window_guest_thoughts_widgets[] = {
	{WWT_FRAME,		0, 0,	191,	0,	156,	-1,			STR_NONE},
	{WWT_CAPTION,	0, 1,	190,	1,	14,		865,		STR_WINDOW_TITLE_TIP},
	{WWT_CLOSEBOX,	0, 179,	189,	2,	13,		824,		STR_CLOSE_WINDOW_TIP},
	{WWT_RESIZE,	1, 0,	191,	43,	156,	-1,			STR_NONE},
	{WWT_TAB,		1, 3,	33,		17,	43,		0x2000144E,	1938},
	{WWT_TAB,		1, 34,	64,		17,	43,		0x2000144E,	1940},
	{WWT_TAB,		1, 65,	95,		17,	43,		0x2000144E,	1941},
	{WWT_TAB,		1, 96,	126,	17,	43,		0x2000144E,	1942},
	{WWT_TAB,		1, 127,	157,	17,	43,		0x2000144E,	1943},
	{WWT_TAB,		1, 158,	188,	17,	43,		0x2000144E,	1944},
	{WIDGETS_END},
};

rct_widget window_guest_inventory_widgets[] = {
	{WWT_FRAME,		0, 0,	191,	0,	156,	-1,			STR_NONE},
	{WWT_CAPTION,	0, 1,	190,	1,	14,		865,		STR_WINDOW_TITLE_TIP},
	{WWT_CLOSEBOX,	0, 179,	189,	2,	13,		824,		STR_CLOSE_WINDOW_TIP},
	{WWT_RESIZE,	1, 0,	191,	43,	156,	-1,			STR_NONE},
	{WWT_TAB,		1, 3,	33,		17,	43,		0x2000144E,	1938},
	{WWT_TAB,		1, 34,	64,		17,	43,		0x2000144E,	1940},
	{WWT_TAB,		1, 65,	95,		17,	43,		0x2000144E,	1941},
	{WWT_TAB,		1, 96,	126,	17,	43,		0x2000144E,	1942},
	{WWT_TAB,		1, 127,	157,	17,	43,		0x2000144E,	1943},
	{WWT_TAB,		1, 158,	188,	17,	43,		0x2000144E,	1944},
	{WIDGETS_END},
};

//0x981D0C
rct_widget *window_guest_page_widgets[] = {
	window_guest_overview_widgets,
	window_guest_stats_widgets,
	window_guest_rides_widgets,
	window_guest_finance_widgets,
	window_guest_thoughts_widgets,
	window_guest_inventory_widgets
};

void window_guest_set_page(rct_window* w, int page);
void window_guest_disable_widgets(rct_window* w);
void window_guest_viewport_init(rct_window* w);

void window_guest_overview_close(rct_window *w);
void window_guest_overview_resize(rct_window *w);
void window_guest_overview_mouse_up(rct_window *w, int widgetIndex);
void window_guest_overview_paint(rct_window *w, rct_drawpixelinfo *dpi);
void window_guest_overview_invalidate(rct_window *w);
void window_guest_overview_unknown_14(rct_window *w);
void window_guest_overview_update(rct_window* w);
void window_guest_overview_text_input(rct_window *w, int widgetIndex, char *text);
void window_guest_overview_tool_update(rct_window* w, int widgetIndex, int x, int y);
void window_guest_overview_tool_down(rct_window* w, int widgetIndex, int x, int y);
void window_guest_overview_tool_abort(rct_window *w, int widgetIndex);

void window_guest_mouse_up(rct_window *w, int widgetIndex);
void window_guest_unknown_05(rct_window *w);

void window_guest_stats_resize(rct_window *w);
void window_guest_stats_update(rct_window *w);
void window_guest_stats_invalidate(rct_window *w);
void window_guest_stats_paint(rct_window *w, rct_drawpixelinfo *dpi);

void window_guest_rides_resize(rct_window *w);
void window_guest_rides_update(rct_window *w);
void window_guest_rides_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId);
void window_guest_rides_scroll_get_size(rct_window *w, int scrollIndex, int *width, int *height);
void window_guest_rides_scroll_mouse_down(rct_window *w, int scrollIndex, int x, int y);
void window_guest_rides_scroll_mouse_over(rct_window *w, int scrollIndex, int x, int y);
void window_guest_rides_invalidate(rct_window *w);
void window_guest_rides_paint(rct_window *w, rct_drawpixelinfo *dpi);
void window_guest_rides_scroll_paint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

void window_guest_finance_resize(rct_window *w);
void window_guest_finance_update(rct_window *w);
void window_guest_finance_invalidate(rct_window *w);
void window_guest_finance_paint(rct_window *w, rct_drawpixelinfo *dpi);

void window_guest_thoughts_resize(rct_window *w);
void window_guest_thoughts_update(rct_window *w);
void window_guest_thoughts_invalidate(rct_window *w);
void window_guest_thoughts_paint(rct_window *w, rct_drawpixelinfo *dpi);

void window_guest_inventory_resize(rct_window *w);
void window_guest_inventory_update(rct_window *w);
void window_guest_inventory_invalidate(rct_window *w);
void window_guest_inventory_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_guest_overview_events = {
	window_guest_overview_close,
	window_guest_overview_mouse_up,
	window_guest_overview_resize,
	NULL,
	NULL,
	NULL,
	window_guest_overview_update,
	NULL,
	NULL,
	window_guest_overview_tool_update,
	window_guest_overview_tool_down,
	NULL,
	NULL,
	window_guest_overview_tool_abort,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_guest_overview_text_input,
	window_guest_overview_unknown_14,
	NULL,
	NULL,
	NULL,
	NULL,
	window_guest_overview_invalidate,
	window_guest_overview_paint,
	NULL
};

static rct_window_event_list window_guest_stats_events = {
	NULL,
	window_guest_mouse_up,
	window_guest_stats_resize,
	NULL,
	NULL,
	window_guest_unknown_05,
	window_guest_stats_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_guest_stats_invalidate,
	window_guest_stats_paint,
	NULL
};

static rct_window_event_list window_guest_rides_events = {
	NULL,
	window_guest_mouse_up,
	window_guest_rides_resize,
	NULL,
	NULL,
	window_guest_unknown_05,
	window_guest_rides_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_guest_rides_scroll_get_size,
	window_guest_rides_scroll_mouse_down,
	NULL,
	window_guest_rides_scroll_mouse_over,
	NULL,
	NULL,
	NULL,
	window_guest_rides_tooltip,
	NULL,
	NULL,
	window_guest_rides_invalidate,
	window_guest_rides_paint,
	window_guest_rides_scroll_paint
};

static rct_window_event_list window_guest_finance_events = {
	NULL,
	window_guest_mouse_up,
	window_guest_finance_resize,
	NULL,
	NULL,
	window_guest_unknown_05,
	window_guest_finance_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_guest_finance_invalidate,
	window_guest_finance_paint,
	NULL
};

static rct_window_event_list window_guest_thoughts_events = {
	NULL,
	window_guest_mouse_up,
	window_guest_thoughts_resize,
	NULL,
	NULL,
	window_guest_unknown_05,
	window_guest_thoughts_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_guest_thoughts_invalidate,
	window_guest_thoughts_paint,
	NULL
};

static rct_window_event_list window_guest_inventory_events = {
	NULL,
	window_guest_mouse_up,
	window_guest_inventory_resize,
	NULL,
	NULL,
	window_guest_unknown_05,
	window_guest_inventory_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_guest_inventory_invalidate,
	window_guest_inventory_paint,
	NULL
};

//0x981D24
static rct_window_event_list *window_guest_page_events[] = {
	&window_guest_overview_events,
	&window_guest_stats_events,
	&window_guest_rides_events,
	&window_guest_finance_events,
	&window_guest_thoughts_events,
	&window_guest_inventory_events
};

void window_guest_set_colours();

//0x981D3C
uint32 window_guest_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6) |
	(1 << WIDX_RENAME)|
	(1 << WIDX_PICKUP)|
	(1 << WIDX_LOCATE)|
	(1 << WIDX_TRACK),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6) |
	(1 << WIDX_RIDE_SCROLL),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_TAB_4) |
	(1 << WIDX_TAB_5) |
	(1 << WIDX_TAB_6)
};

/**
 * rct2: 0x006989E9
 *
 */
void window_guest_open(rct_peep* peep){
	
	if (peep->type == PEEP_TYPE_STAFF){
		window_staff_open(peep);
		return;
	}

	rct_window* window;

	window = window_bring_to_front_by_number(WC_PEEP, peep->sprite_index);
	if (window == NULL){
		window = window_create_auto_pos(192, 157, &window_guest_overview_events, WC_PEEP, WF_RESIZABLE);
		window->widgets = window_guest_overview_widgets;
		window->enabled_widgets = window_guest_page_enabled_widgets[0];
		window->number = peep->sprite_index;
		window->page = 0;
		window->viewport_focus_coordinates.y = 0;
		window->frame_no = 0;
		window->list_information_type = 0;
		window->var_492 = 0;
		window->var_494 = 0;
		window_guest_disable_widgets(window);
		window->min_width = 192;
		window->min_height = 157;
		window->max_width = 500;
		window->max_height = 450;
		window->no_list_items = 0;
		window->selected_list_item = -1;

		window->viewport_focus_coordinates.y = -1;
	}
	
	window->page = 0;
	window_invalidate(window);
	
	window->widgets = window_guest_page_widgets[WINDOW_GUEST_OVERVIEW];
	window->enabled_widgets = window_guest_page_enabled_widgets[WINDOW_GUEST_OVERVIEW];
	window->hold_down_widgets = 0;
	window->event_handlers = window_guest_page_events[WINDOW_GUEST_OVERVIEW];
	window->pressed_widgets = 0;
	
	window_guest_disable_widgets(window);
	window_init_scroll_widgets(window);
	window_guest_viewport_init(window);
}

/* rct2: 0x006987A6 
 * Disables the finance tab when no money.
 * Disables peep pickup when in certain no pickup states.
 */
void window_guest_disable_widgets(rct_window* w){
	rct_peep* peep = &g_sprite_list[w->number].peep;
	uint64 disabled_widgets = 0;

	if (peep_can_be_picked_up(peep)){
		if (w->disabled_widgets & (1 << WIDX_PICKUP))
			window_invalidate(w);
	}
	else{
		disabled_widgets = (1 << WIDX_PICKUP);
		if (!(w->disabled_widgets & (1 << WIDX_PICKUP)))
			window_invalidate(w);
	}
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY){
		disabled_widgets |= (1 << WIDX_TAB_4); //Disable finance tab if no money
	}
	w->disabled_widgets = disabled_widgets;
}

/* rct2: 0x00696A75 */
void window_guest_overview_close(rct_window *w)
{	
	if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE){
		if (w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS,rct_windowclass) && 
			w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER,rct_windownumber)) 
			tool_cancel();
	}
}

/* rct2: 0x00696FBE */
void window_guest_overview_resize(rct_window *w){
	window_guest_disable_widgets(w);
	window_event_invalidate_call(w);
	
	widget_invalidate(w, WIDX_MARQUEE);
	
	window_set_resize(w, 192, 159, 500, 450);
	
	rct_viewport* view = w->viewport;
	
	if (view){
		if ((w->width - 30) == view->width){
			if ((w->height - 72) == view->height){
				window_guest_viewport_init(w);
				return;
			}
		}
		uint8 zoom_amount = 1 << view->zoom;
		view->width = w->width - 30;
		view->height = w->height - 72;
		view->view_width = view->width / zoom_amount;
		view->view_height = view->height / zoom_amount;
	}
	window_guest_viewport_init(w);
}

/* rct2: 0x00696A06 */
void window_guest_overview_mouse_up(rct_window *w, int widgetIndex)
{
	rct_peep* peep = GET_PEEP(w->number);

	switch(widgetIndex){
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
		window_guest_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_PICKUP:
		if (!peep_can_be_picked_up(peep)) {
			return;
		}
		if (tool_set(w, widgetIndex, 7)) {
			return;
		}
		
		w->var_48C = peep->x;

		remove_peep_from_ride(peep);
		invalidate_sprite((rct_sprite*)peep);

		sprite_move(0x8000, peep->y, peep->z, (rct_sprite*)peep);
		peep_decrement_num_riders(peep);
		peep->state = PEEP_STATE_PICKED;
		peep->sub_state = 0;
		peep_window_state_update(peep);
		break;
	case WIDX_RENAME:
		window_text_input_open(w, widgetIndex, 0x5AC, 0x5AD, peep->name_string_idx, peep->id, 32);
		break;
	case WIDX_LOCATE:
		window_scroll_to_viewport(w);
		break;
	case WIDX_TRACK:
		g_sprite_list[w->number].peep.flags ^= PEEP_FLAGS_TRACKING;
		break;
	}
}

/* rct2: 0x696AA0 */
void window_guest_set_page(rct_window* w, int page){
	if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE)
	{
		if(w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber) &&
		   w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass))
			tool_cancel();
	
	}
	int listen = 0;
	if ( page == WINDOW_GUEST_OVERVIEW && w->page==WINDOW_GUEST_OVERVIEW && w->viewport){
		if(!(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
			listen = 1;
	}
	
	
	w->page = page;
	w->frame_no = 0;
	w->no_list_items = 0;
	w->selected_list_item = -1;
	
	rct_viewport* viewport = w->viewport;
	w->viewport = 0;
	if (viewport){
		viewport->width = 0;
	}
	
	w->enabled_widgets = window_guest_page_enabled_widgets[page];
	w->hold_down_widgets = 0;
	w->event_handlers = window_guest_page_events[page];
	w->pressed_widgets = 0;
	w->widgets = window_guest_page_widgets[page];
	window_guest_disable_widgets(w);
	window_invalidate(w);
	window_event_resize_call(w);
	window_event_invalidate_call(w);
	window_init_scroll_widgets(w);
	window_invalidate(w);
	
	if (listen && w->viewport) w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

void window_guest_overview_unknown_14(rct_window *w)
{
	window_guest_viewport_init(w);
}

/* rct2: 0x0069883C */
void window_guest_viewport_init(rct_window* w){
	if (w->page != WINDOW_GUEST_OVERVIEW) return;

	union{
		sprite_focus sprite;
		coordinate_focus coordinate;
	} focus = { 0 }; //The focus will be either a sprite or a coordinate.

	focus.sprite.sprite_id = w->number;
	
	rct_peep* peep = GET_PEEP(w->number);

	if (peep->state == PEEP_STATE_PICKED){
		focus.sprite.sprite_id = -1;
	}
	else{
		uint8 final_check = 1;
		if (peep->state == PEEP_STATE_ON_RIDE 
			|| peep->state == PEEP_STATE_ENTERING_RIDE 
			|| (peep->state == PEEP_STATE_LEAVING_RIDE && peep->x == SPRITE_LOCATION_NULL)){

			rct_ride* ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[peep->current_ride]);
			if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK){
				rct_vehicle* train = GET_VEHICLE(ride->vehicles[peep->current_train]);
				int car = peep->current_car;

				for (; car != 0; car--){
					train = GET_VEHICLE(train->next_vehicle_on_train);
				}

				focus.sprite.sprite_id = train->sprite_index;
				final_check = 0;
			}
		}
		if (peep->x == SPRITE_LOCATION_NULL && final_check){
			rct_ride* ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[peep->current_ride]);
			int x = (ride->overall_view & 0xFF) * 32 + 16;
			int y = (ride->overall_view >> 8) * 32 + 16;
			int height = map_element_height(x, y);
			height += 32;
			focus.coordinate.x = x;
			focus.coordinate.y = y;
			focus.coordinate.z = height;
			focus.sprite.type |= VIEWPORT_FOCUS_TYPE_COORDINATE;
		}
		else{
			focus.sprite.type |= VIEWPORT_FOCUS_TYPE_SPRITE | VIEWPORT_FOCUS_TYPE_COORDINATE;
			focus.sprite.pad_486 &= 0xFFFF;
		}
		focus.coordinate.rotation = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8);
	}

	uint16 viewport_flags;

	if (w->viewport){
		//Check all combos, for now skipping y and rot
		if (focus.coordinate.x == w->viewport_focus_coordinates.x && 
			focus.coordinate.y == w->viewport_focus_coordinates.y &&
			focus.coordinate.z == w->viewport_focus_coordinates.z &&
			focus.coordinate.rotation == w->viewport_focus_coordinates.rotation)
			return;

		viewport_flags = w->viewport->flags;
		w->viewport->width = 0;
		w->viewport = 0;

		viewport_update_pointers();
	}
	else{
		viewport_flags = 0;
		if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & 0x1)
			viewport_flags |= VIEWPORT_FLAG_GRIDLINES;
	}

	window_event_invalidate_call(w);

	w->viewport_focus_coordinates.x = focus.coordinate.x;
	w->viewport_focus_coordinates.y = focus.coordinate.y;
	w->viewport_focus_coordinates.z = focus.coordinate.z;
	w->viewport_focus_coordinates.rotation = focus.coordinate.rotation;

	if (peep->state != PEEP_STATE_PICKED){
		if (!(w->viewport)){
			rct_widget* view_widget = &w->widgets[WIDX_VIEWPORT];

			int x = view_widget->left + 1 + w->x;
			int y = view_widget->top + 1 + w->y;
			int width = view_widget->right - view_widget->left - 1;
			int height = view_widget->bottom - view_widget->top - 1;

			viewport_create(w, x, y, width, height, 0, focus.coordinate.x, focus.coordinate.y, focus.coordinate.z, focus.sprite.type & VIEWPORT_FOCUS_TYPE_MASK, focus.sprite.sprite_id);
			w->flags |= WF_2;
			window_invalidate(w);
		}
	}

	if (w->viewport)
		w->viewport->flags = viewport_flags;
	window_invalidate(w);
}

/**
 * rct2: 0x6983dd 
 * used by window_staff as well
 */
void window_guest_overview_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1<<WIDX_TAB_1))
		return;
	
	rct_widget* widget = &w->widgets[WIDX_TAB_1];
	int width = widget->right - widget->left - 1;
	int height = widget->bottom - widget->top - 1;
	int x = widget->left + 1 + w->x;
	int y = widget->top + 1 + w->y;
	if (w->page == WINDOW_GUEST_OVERVIEW) height++;
	
	rct_drawpixelinfo* clip_dpi = clip_drawpixelinfo(dpi, x, width, y, height );
	if (!clip_dpi) return;
	
	x = 14;
	y = 20;
	
	rct_peep* peep = GET_PEEP(w->number);
	
	if (peep->type == PEEP_TYPE_STAFF && peep->staff_type == STAFF_TYPE_ENTERTAINER)
		y++;
	
	int ebx = *(RCT2_ADDRESS(0x982708, uint32*)[peep->sprite_type * 2]) + 1;
	
	int eax = 0;
	
	if (w->page == WINDOW_GUEST_OVERVIEW){
		eax = w->var_494>>16;
		eax &= 0xFFFC;
	}
	ebx += eax;
	
	int sprite_id = ebx | (peep->tshirt_colour << 19) | (peep->trousers_colour << 24) | 0xA0000000;
	gfx_draw_sprite( clip_dpi, sprite_id, x, y, 0);
	
	// If holding a balloon
	if (ebx >= 0x2A1D && ebx < 0x2A3D){
		ebx += 32;
		ebx |= (peep->balloon_colour << 19) | 0x20000000;
		gfx_draw_sprite( clip_dpi, ebx, x, y, 0);
	}
	
	// If holding umbrella
	if (ebx >= 0x2BBD && ebx < 0x2BDD){
		ebx += 32;
		ebx |= (peep->umbrella_colour << 19) | 0x20000000;
		gfx_draw_sprite(clip_dpi, ebx, x, y, 0);
	}
	
	// If wearing hat
	if (ebx >= 0x29DD && ebx < 0x29FD){
		ebx += 32;
		ebx |= (peep->hat_colour << 19) | 0x20000000;
		gfx_draw_sprite( clip_dpi, ebx, x, y, 0);
	}

	rct2_free(clip_dpi);
}

/* rct2: 0x69869b */
void window_guest_stats_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1 << WIDX_TAB_2))
		return;

	rct_widget* widget = &w->widgets[WIDX_TAB_2];
	int x = widget->left + w->x;
	int y = widget->top + w->y;

	rct_peep* peep = GET_PEEP(w->number);
	int image_id = get_peep_face_sprite_large(peep);
	if (w->page == WINDOW_GUEST_STATS){
		// If currently viewing this tab animate tab
		// if it is very sick or angry.
		switch (image_id){
		case SPR_PEEP_LARGE_FACE_VERY_VERY_SICK:
			image_id += (w->frame_no / 4) & 0xF;
			break;
		case SPR_PEEP_LARGE_FACE_VERY_SICK:
			image_id += (w->frame_no / 8) & 0x3;
			break;
		case SPR_PEEP_LARGE_FACE_ANGRY:
			image_id += (w->frame_no / 8) & 0x3;
			break;
		}
	}
	gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/* rct2: 0x69861F */
void window_guest_rides_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1 << WIDX_TAB_3)) return;

	rct_widget* widget = &w->widgets[WIDX_TAB_3];
	int x = widget->left + w->x;
	int y = widget->top + w->y;

	int image_id = SPR_TAB_RIDE_0;

	if ( w->page == WINDOW_GUEST_RIDES ){
		image_id += (w->frame_no / 4) & 0xF;		
	}
	
	gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/* rct2: 0x698597 */
void window_guest_finance_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1 << WIDX_TAB_4)) return;

	rct_widget* widget = &w->widgets[WIDX_TAB_4];
	int x = widget->left + w->x;
	int y = widget->top + w->y;

	int image_id = SPR_TAB_FINANCES_SUMMARY_0;

	if ( w->page == WINDOW_GUEST_FINANCE ){
		image_id += (w->frame_no / 2) & 0x7;		
	}
	
	gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/* rct2: 0x6985D8 */
void window_guest_thoughts_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1 << WIDX_TAB_5)) return;

	rct_widget* widget = &w->widgets[WIDX_TAB_5];
	int x = widget->left + w->x;
	int y = widget->top + w->y;

	int image_id = 5269;

	if ( w->page == WINDOW_GUEST_THOUGHTS ){
		image_id += (w->frame_no / 2) & 0x7;		
	}
	
	gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/* rct2: 0x698661 */
void window_guest_inventory_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1 << WIDX_TAB_6)) return;

	rct_widget* widget = &w->widgets[WIDX_TAB_6];
	int x = widget->left + w->x;
	int y = widget->top + w->y;

	int image_id = 5326;
	
	gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/* rct2: 0x696887 */
void window_guest_overview_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_guest_overview_tab_paint(w, dpi);
	window_guest_stats_tab_paint(w, dpi);
	window_guest_rides_tab_paint(w, dpi);
	window_guest_finance_tab_paint(w, dpi);
	window_guest_thoughts_tab_paint(w, dpi);
	window_guest_inventory_tab_paint(w, dpi);

	// Draw the viewport no sound sprite
	if (w->viewport){
		window_draw_viewport(dpi, w);
		rct_viewport* viewport = w->viewport;
		if (viewport->flags & VIEWPORT_FLAG_SOUND_ON){
			gfx_draw_sprite(dpi, SPR_HEARING_VIEWPORT, w->x + 2, w->y + 2, 0);
		}
	}

	// Draw the centered label
	uint32 argument1, argument2;
	rct_peep* peep = GET_PEEP(w->number);
	get_arguments_from_action(peep, &argument1, &argument2);
	RCT2_GLOBAL(0x13CE952, uint32) = argument1;
	RCT2_GLOBAL(0x13CE952 + 4, uint32) = argument2;
	rct_widget* widget = &w->widgets[WIDX_ACTION_LBL];
	int x = (widget->left + widget->right) / 2 + w->x;
	int y = w->y + widget->top - 1;
	int width = widget->right - widget->left;
	gfx_draw_string_centred_clipped(dpi, 1191, (void*)0x13CE952, 0, x, y, width);

	// Draw the marquee thought
	widget = &w->widgets[WIDX_MARQUEE];
	width = widget->right - widget->left - 3;
	int left = widget->left + 2 + w->x;
	int top = widget->top + w->y;
	int height = widget->bottom - widget->top;
	rct_drawpixelinfo* dpi_marquee = clip_drawpixelinfo(dpi, left, width, top, height);

	if (!dpi_marquee)return;
	int i = 0;
	for (; i < PEEP_MAX_THOUGHTS; ++i){
		if (peep->thoughts[i].type == PEEP_THOUGHT_TYPE_NONE){
			w->list_information_type = 0;
			return;
		}
		if (peep->thoughts[i].var_2 == 1){ // If a fresh thought
			break;
		}
	}
	if (i == PEEP_MAX_THOUGHTS){
		w->list_information_type = 0;
		return;
	}

	get_arguments_from_thought(peep->thoughts[i], &argument1, &argument2);

	RCT2_GLOBAL(0x13CE952, uint32) = argument1;
	RCT2_GLOBAL(0x13CE952 + 4, uint32) = argument2;
	RCT2_GLOBAL(0x13CE952 + 8, uint16) = 0;

	x = widget->right - widget->left - w->list_information_type;
	gfx_draw_string_left(dpi_marquee, 1193, (void*)0x13CE952, 0, x, 0);

	rct2_free(dpi_marquee);
}

/* rct2: 0x696749*/
void window_guest_overview_invalidate(rct_window *w)
{
	colour_scheme_update(w);
	
	if (window_guest_page_widgets[w->page] != w->widgets){
		w->widgets = window_guest_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}
	
	w->pressed_widgets &= ~(WIDX_TAB_1 | WIDX_TAB_2 |WIDX_TAB_3 |WIDX_TAB_4 |WIDX_TAB_5 |WIDX_TAB_6);
	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);
	
	rct_peep* peep = GET_PEEP(w->number);
	RCT2_GLOBAL(0x13CE952,uint16) = peep->name_string_idx;
	RCT2_GLOBAL(0x13CE954,uint32) = peep->id;
	
	w->pressed_widgets &= ~(1<<WIDX_TRACK);
	if (peep->flags & 0x8){
		w->pressed_widgets |= (1<<WIDX_TRACK);
	}
	
	window_guest_overview_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_guest_overview_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	
	window_guest_overview_widgets[WIDX_PAGE_BACKGROUND].right =w->width - 1;
	window_guest_overview_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
	
	window_guest_overview_widgets[WIDX_TITLE].right = w->width - 2;
	
	window_guest_overview_widgets[WIDX_CLOSE].left = w->width - 13;
	window_guest_overview_widgets[WIDX_CLOSE].right = w->width - 3;

	window_guest_overview_widgets[WIDX_VIEWPORT].right = w->width - 26;
	window_guest_overview_widgets[WIDX_VIEWPORT].bottom = w->height - 14;

	window_guest_overview_widgets[WIDX_ACTION_LBL].top = w->height - 12;
	window_guest_overview_widgets[WIDX_ACTION_LBL].bottom = w->height - 3;
	window_guest_overview_widgets[WIDX_ACTION_LBL].right = w->width - 24;
	
	window_guest_overview_widgets[WIDX_MARQUEE].right = w->width - 24;

	window_guest_overview_widgets[WIDX_PICKUP].right = w->width - 2;
	window_guest_overview_widgets[WIDX_RENAME].right = w->width - 2;
	window_guest_overview_widgets[WIDX_LOCATE].right = w->width - 2;
	window_guest_overview_widgets[WIDX_TRACK].right = w->width - 2;

	window_guest_overview_widgets[WIDX_PICKUP].left = w->width - 25;
	window_guest_overview_widgets[WIDX_RENAME].left = w->width - 25;
	window_guest_overview_widgets[WIDX_LOCATE].left = w->width - 25;
	window_guest_overview_widgets[WIDX_TRACK].left = w->width - 25;
	
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/* rct2: 0x696F45 */
void window_guest_overview_update(rct_window* w){
	int var_496 = w->var_494 >> 16;
	var_496++;
	var_496 %= 24;
	w->var_494 &= 0x0000FFFF;
	w->var_494 |= var_496 << 16;

	widget_invalidate(w, WIDX_TAB_1);
	widget_invalidate(w, WIDX_TAB_2);
	
	w->list_information_type += 2;

	if ((w->var_494 & 0xFFFF) == 0xFFFF)
		w->var_494 &= 0xFFFF0000;
	else
		w->var_494++;

	// Create the "I have the strangest feeling I am being watched thought"
	if ((w->var_494 & 0xFFFF) >= 3840){
		if (!(w->var_494 & 0x3FF)){
			int rand = scenario_rand() & 0xFFFF;
			if (rand <= 0x2AAA){
				rct_peep* peep = GET_PEEP(w->number);
				peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_WATCHED, 0xFF);
			}
		}
	}
}

/* rct2:0x696A6A */
void window_guest_overview_text_input(rct_window *w, int widgetIndex, char *text)
{
	if (widgetIndex != WIDX_RENAME)
		return;

	if (text == NULL)
		return;
	
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, uint16) = 1454;
	game_do_command(1, GAME_COMMAND_FLAG_APPLY, w->number, *((int*)(text + 0)), GAME_COMMAND_SET_PEEP_NAME, *((int*)(text + 8)), *((int*)(text + 4)));
	game_do_command(2, GAME_COMMAND_FLAG_APPLY, w->number, *((int*)(text + 12)), GAME_COMMAND_SET_PEEP_NAME, *((int*)(text + 20)), *((int*)(text + 16)));
	game_do_command(0, GAME_COMMAND_FLAG_APPLY, w->number, *((int*)(text + 24)), GAME_COMMAND_SET_PEEP_NAME, *((int*)(text + 32)), *((int*)(text + 28)));

	rct_peep* peep = GET_PEEP(w->number);
	if (peep_get_easteregg_name_id(peep) == EASTEREGG_PEEP_NAME_LOADSAMONEY)
		peep->cash_in_pocket = MONEY(31415,92);
}

/* rct2: 0x696A5F */
void window_guest_overview_tool_update(rct_window* w, int widgetIndex, int x, int y)
{
	if (widgetIndex != WIDX_PICKUP)
		return;

	map_invalidate_selection_rect();

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);

	int map_x, map_y;
	footpath_get_coordinates_from_pos(x, y + 16, &map_x, &map_y, NULL, NULL);
	if (map_x != (sint16)0x8000){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 1;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = map_x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = map_x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = map_y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = map_y;
		map_invalidate_selection_rect();
	}

	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, sint32) = -1;

	int interactionType;
	get_map_coordinates_from_pos(x, y, VIEWPORT_INTERACTION_MASK_NONE, NULL, NULL, &interactionType, NULL, NULL);
	if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE)
		return;

	x--;
	y += 16;
	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_X, uint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_Y, uint16) = y;
	w->var_492++;
	if (w->var_492 >= 48)w->var_492 = 0;
	
	rct_peep* peep;
	peep = GET_PEEP(w->number);
	int ebx = (RCT2_ADDRESS(0x982708, uint32*)[peep->sprite_type * 2])[22];
	ebx += w->var_492 >> 2;

	int ebp = peep->tshirt_colour << 19;
	int ecx = peep->trousers_colour << 24;

	ebx |= ebp | ecx | 0xA0000000;
	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, uint32) = ebx;
}

/* rct2: 0x696A54 */
void window_guest_overview_tool_down(rct_window* w, int widgetIndex, int x, int y)
{
	if (widgetIndex != WIDX_PICKUP)
		return;

	int dest_x, dest_y;
	rct_map_element *mapElement;
	footpath_get_coordinates_from_pos(x, y + 16, &dest_x, &dest_y, NULL, &mapElement);

	if (dest_x == (sint16)0x8000)return;

	// Set the coordinate of destination to be exactly 
	// in the middle of a tile.
	dest_x += 16;
	dest_y += 16;
	// Set the tile coordinate to top left of tile
	int tile_y = dest_y & 0xFFE0;
	int tile_x = dest_x & 0xFFE0;

	int dest_z = mapElement->base_height * 8 + 16;

	if (!map_is_location_owned(tile_x, tile_y, dest_z)){
		window_error_open(0x785,-1);
		return;
	}

	if (!map_can_construct_at(tile_x, tile_y, dest_z / 8, (dest_z / 8) + 1, 15)){
		if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) != 0x3A5 ){
			if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) != 0x49B){
				window_error_open(0x785, -1);
				return;
			}
		}
	}

	rct_peep* peep = GET_PEEP(w->number);
	sprite_move(dest_x, dest_y, dest_z, (rct_sprite*)peep);
	invalidate_sprite((rct_sprite*)peep);
	peep_decrement_num_riders(peep);
	peep->state = 0;
	peep_window_state_update(peep);
	peep->action = 0xFF;
	peep->var_6D = 0;
	peep->action_sprite_image_offset = 0;
	peep->action_sprite_type = 0xFF;
	peep->var_C4 = 0;

	peep->happiness_growth_rate = max(peep->happiness_growth_rate - 10, 0);

	sub_693B58(peep);
	tool_cancel();
	RCT2_GLOBAL(0x9DE550, sint32) = -1;
}

/* rct2: 0x696A49 */
void window_guest_overview_tool_abort(rct_window *w, int widgetIndex)
{
	if (widgetIndex != WIDX_PICKUP)
		return;

	rct_peep* peep = GET_PEEP(w->number);
	if (peep->state != PEEP_STATE_PICKED)
		return;

	sprite_move( w->var_48C, peep->y, peep->z + 8, (rct_sprite*)peep);
	invalidate_sprite((rct_sprite*)peep);

	if (peep->x != (sint16)0x8000){
		peep_decrement_num_riders(peep);
		peep->state = 0;
		peep_window_state_update(peep);
		peep->action = 0xFF;
		peep->var_6D = 0;
		peep->action_sprite_image_offset = 0;
		peep->action_sprite_type = 0;
		peep->var_C4 = 0;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, sint32) = -1;
}

/* rct2:0x69744F, 0x697795, 0x697BDD, 0x697E18, 0x698279
 * This is a combination of 5 functions that were identical
 */
void window_guest_mouse_up(rct_window *w, int widgetIndex)
{
	switch (widgetIndex){
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
		window_guest_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	}
}

/* rct2: 0x697488 */
void window_guest_stats_resize(rct_window *w)
{
	window_set_resize(w, 192, 162, 192, 162);
}

/* rct2: 0x6974ED, 0x00697959, 0x00697C7B, 0x00697ED2, 0x00698333 
 * This is a combination of 5 functions that were identical
 */
void window_guest_unknown_05(rct_window *w)
{
	widget_invalidate(w, WIDX_TAB_1);
}

/* rct2: 0x69746A */
void window_guest_stats_update(rct_window *w)
{
	w->frame_no++;
	rct_peep* peep = GET_PEEP(w->number);
	peep->window_invalidate_flags &= ~PEEP_INVALIDATE_PEEP_STATS;

	window_invalidate(w);
}

/* rct2: 0x69707D */
void window_guest_stats_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	if (w->widgets != window_guest_page_widgets[w->page]) {
		w->widgets = window_guest_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	rct_peep* peep = GET_PEEP(w->number);
	RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
	RCT2_GLOBAL(0x13CE954, uint32) = peep->id;

	window_guest_stats_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_guest_stats_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	window_guest_stats_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	window_guest_stats_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

	window_guest_stats_widgets[WIDX_TITLE].right = w->width - 2;

	window_guest_stats_widgets[WIDX_CLOSE].left = w->width - 13;
	window_guest_stats_widgets[WIDX_CLOSE].right = w->width - 3;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/**
*
*  rct2: 0x0066ECC1
* 
*  ebp: colour, contains flag 0x80000000 for blinking
*/
void window_guest_stats_bars_paint(int value, int x, int y, rct_window *w, rct_drawpixelinfo *dpi, int colour){
	value *= 0x76;
	value >>= 8;

	gfx_fill_rect_inset(dpi, x + 0x3A, y + 1, x + 0x3A + 0x79, y + 9, w->colours[1], 0x30);

	int blink_flag = colour & (1 << 0x1F); //0x80000000
	colour &= ~(1 << 0x1F);
	if (!blink_flag ||
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) != 0 ||
		(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 8) == 0)
	{
		if (value <= 2)
			return;
		gfx_fill_rect_inset(dpi, x + 0x3C, y + 2, x + 0x3C + value - 1, y + 8, colour, 0);
	}

}

/* rct2: 0x0069711D */
void window_guest_stats_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_guest_overview_tab_paint(w, dpi);
	window_guest_stats_tab_paint(w, dpi);
	window_guest_rides_tab_paint(w, dpi);
	window_guest_finance_tab_paint(w, dpi);
	window_guest_thoughts_tab_paint(w, dpi);
	window_guest_inventory_tab_paint(w, dpi);

	//ebx
	rct_peep* peep = GET_PEEP(w->number);

	// Not sure why this is not stats widgets
	//cx
	int x = w->x + window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	//dx
	int y = w->y + window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].top + 4;

	//Happiness
	gfx_draw_string_left(dpi, 1662, (void*)0x13CE952, 0, x, y);

	int happiness = peep->happiness;
	if (happiness < 10)happiness = 10;
	int ebp = 14;
	if (happiness < 50){
		ebp |= 0x80000000;
	}
	window_guest_stats_bars_paint(happiness, x, y, w, dpi, ebp);

	//Energy
	y += 10;
	gfx_draw_string_left(dpi, 1664, (void*)0x13CE952, 0, x, y);

	int energy = ((peep->energy - 32) * 85) / 32;
	ebp = 14;
	if (energy < 50){
		ebp |= 0x80000000;
	}
	if (energy < 10)energy = 10;
	window_guest_stats_bars_paint(energy, x, y, w, dpi, ebp);

	//Hunger
	y += 10;
	gfx_draw_string_left(dpi, 1665, (void*)0x13CE952, 0, x, y);

	int hunger = peep->hunger;
	if (hunger > 190) hunger = 190;

	hunger -= 32;
	if (hunger < 0) hunger = 0;
	hunger *= 51;
	hunger /= 32;
	hunger = 0xFF & ~hunger;

	ebp = 28;
	if (hunger > 170){
		ebp |= 0x80000000;
	}
	window_guest_stats_bars_paint(hunger, x, y, w, dpi, ebp);

	//Thirst
	y += 10;
	gfx_draw_string_left(dpi, 1666, (void*)0x13CE952, 0, x, y);

	int thirst = peep->thirst;
	if (thirst > 190) thirst = 190;

	thirst -= 32;
	if (thirst < 0) thirst = 0;
	thirst *= 51;
	thirst /= 32;
	thirst = 0xFF & ~thirst;

	ebp = 28;
	if (thirst > 170){
		ebp |= 0x80000000;
	}
	window_guest_stats_bars_paint(thirst, x, y, w, dpi, ebp);

	//Nausea
	y += 10;
	gfx_draw_string_left(dpi, 1663, (void*)0x13CE952, 0, x, y);

	int nausea = peep->nausea - 32;

	if (nausea  < 0) nausea = 0;
	nausea *= 36;
	nausea /= 32;

	ebp = 28;
	if (nausea > 120){
		ebp |= 0x80000000;
	}
	window_guest_stats_bars_paint(nausea, x, y, w, dpi, ebp);

	//Bathroom
	y += 10;
	gfx_draw_string_left(dpi, 1667, (void*)0x13CE952, 0, x, y);

	int bathroom = peep->bathroom - 32;
	if (bathroom > 210) bathroom = 210;

	bathroom -= 32;
	if (bathroom < 0) bathroom = 0;
	bathroom *= 45;
	bathroom /= 32;

	ebp = 28;
	if (bathroom > 160){
		ebp |= 0x80000000;
	}
	window_guest_stats_bars_paint(bathroom, x, y, w, dpi, ebp);

	// Time in park
	y += 11;
	if (peep->time_in_park != -1){
		int eax = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32);
		eax -= peep->time_in_park;
		eax >>= 11;
		RCT2_GLOBAL(0x13CE952, uint16) = eax & 0xFFFF;
		gfx_draw_string_left(dpi, 1458, (void*)0x13CE952, 0, x, y);
	}

	y += 19;
	gfx_fill_rect_inset(dpi, x, y - 6, x + 179, y - 5, w->colours[1], 32);

	// Preferred Ride
	gfx_draw_string_left(dpi, 1657, (void*)0, 0, x, y);
	y += 10;

	// Intensity
	int intensity = peep->intensity / 16;
	RCT2_GLOBAL(0x13CE952, uint16) = intensity;
	int string_id = 1658;
	if (peep->intensity & 0xF){
		RCT2_GLOBAL(0x13CE952, uint16) = peep->intensity & 0xF;
		RCT2_GLOBAL(0x13CE954, uint16) = intensity;
		string_id = 1659;
		if (intensity == 15) string_id = 1660;
	}

	gfx_draw_string_left(dpi, string_id, (void*)0x13CE952, 0, x + 4, y);

	// Nausea tolerance
	y += 10;
	int nausea_tolerance = peep->nausea_tolerance & 0x3;
	RCT2_GLOBAL(0x13CE952, uint16) = nausea_tolerance + 2368;
	gfx_draw_string_left(dpi, 1661, (void*)0x13CE952, 0, x, y);
}

/* rct2: 0x006978F4 */
void window_guest_rides_resize(rct_window *w)
{
	window_set_resize(w, 192, 128, 500, 400);
}

/* rct2: 0x6977B0 */
void window_guest_rides_update(rct_window *w)
{
	w->frame_no++;

	widget_invalidate(w, WIDX_TAB_2);
	widget_invalidate(w, WIDX_TAB_3);

	rct_peep* peep = GET_PEEP(w->number);

	// Every 2048 ticks do a full window_invalidate
	int number_of_ticks = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32) - peep->time_in_park;
	if (!(number_of_ticks & 0x7FF)) window_invalidate(w);

	uint8 curr_list_position = 0;
	for (uint8 ride_id = 0; ride_id < 255; ++ride_id){
		// Offset to the ride_id bit in peep_rides_been_on
		uint8 ride_id_bit = ride_id & 0x7;
		uint8 ride_id_offset = ride_id / 8;
		if (peep->rides_been_on[ride_id_offset] & (1 << ride_id_bit)){
			rct_ride* ride = GET_RIDE(ride_id);
			if (RCT2_ADDRESS(0x97C3AF, uint8)[ride->type] == 0){
				w->list_item_positions[curr_list_position] = ride_id;
				curr_list_position++;
			}
		}
	}
	// If there are new items
	if (w->no_list_items != curr_list_position){
		w->no_list_items = curr_list_position;
		window_invalidate(w);
	}
}

/* rct2: 0x697844 */
void window_guest_rides_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId)
{
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

/* rct2: 0x69784E */
void window_guest_rides_scroll_get_size(rct_window *w, int scrollIndex, int *width, int *height)
{
	*height = w->no_list_items * 10;

	if (w->selected_list_item != -1){
		w->selected_list_item = -1;
		window_invalidate(w);
	}

	int visable_height = *height 
		- window_guest_rides_widgets[WIDX_RIDE_SCROLL].bottom 
		+ window_guest_rides_widgets[WIDX_RIDE_SCROLL].top
		+ 21;

	if (visable_height < 0) visable_height = 0;

	if (visable_height < w->scrolls[0].v_top){
		w->scrolls[0].v_top = visable_height;
		window_invalidate(w);
	}
}

/* rct2: 0x006978CC */
void window_guest_rides_scroll_mouse_down(rct_window *w, int scrollIndex, int x, int y)
{
	int index;

	index = y / 10;
	if (index >= w->no_list_items) return;

	window_ride_main_open(w->list_item_positions[index]);
}

/* rct2: 0x0069789C */
void window_guest_rides_scroll_mouse_over(rct_window *w, int scrollIndex, int x, int y)
{
	int index;

	index = y / 10;
	if (index >= w->no_list_items)return;

	if (index == w->selected_list_item)return;
	w->selected_list_item = index;

	window_invalidate(w);
}

/* rct2: 0x0069757A */
void window_guest_rides_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	if (window_guest_page_widgets[w->page] != w->widgets){
		w->widgets = window_guest_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	rct_peep* peep = GET_PEEP(w->number);
	RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
	RCT2_GLOBAL(0x13CE954, uint32) = peep->id;

	window_guest_rides_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_guest_rides_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

	window_guest_rides_widgets[WIDX_TITLE].right = w->width - 2;

	window_guest_rides_widgets[WIDX_CLOSE].left = w->width - 13;
	window_guest_rides_widgets[WIDX_CLOSE].right = w->width - 3;

	window_guest_rides_widgets[WIDX_RIDE_SCROLL].right = w->width - 4;
	window_guest_rides_widgets[WIDX_RIDE_SCROLL].bottom = w->height - 15;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/* rct2: 0x00697637 */
void window_guest_rides_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_guest_overview_tab_paint(w, dpi);
	window_guest_stats_tab_paint(w, dpi);
	window_guest_rides_tab_paint(w, dpi);
	window_guest_finance_tab_paint(w, dpi);
	window_guest_thoughts_tab_paint(w, dpi);
	window_guest_inventory_tab_paint(w, dpi);

	rct_peep* peep = GET_PEEP(w->number);

	//cx
	int x = w->x + window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].left + 2;
	//dx
	int y = w->y + window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].top + 2;

	gfx_draw_string_left(dpi, 2292, (void*)0, 0, x, y);

	y = w->y + window_guest_rides_widgets[WIDX_PAGE_BACKGROUND].bottom - 12;

	int ride_string_id = 3094; 
	int ride_string_arguments = 0;
	if (peep->favourite_ride != 0xFF){
		rct_ride* ride = GET_RIDE(peep->favourite_ride);
		ride_string_arguments = ride->name_arguments;
		ride_string_id = ride->name;
	}
	RCT2_GLOBAL(0x13CE952, uint16) = ride_string_id;
	RCT2_GLOBAL(0x13CE954, uint32) = ride_string_arguments;

	gfx_draw_string_left_clipped(dpi, 3093, (void*)0x13CE952, 0, x, y, w->width - 14);
}

/* rct2: 0x006976FC */
void window_guest_rides_scroll_paint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	// ax
	int left = dpi->x;
	// bx
	int right = dpi->x + dpi->width - 1;
	// cx
	int top = dpi->y;
	// dx
	int bottom = dpi->y + dpi->height - 1;

	int colour = RCT2_ADDRESS(0x141FC48, uint8)[w->colours[1] * 8];
	gfx_fill_rect(dpi, left, top, right, bottom, colour);

	for (int list_index = 0; list_index < w->no_list_items; list_index++){
		int y = list_index * 10;
		int string_format = 1191;
		if (list_index == w->selected_list_item){
			gfx_fill_rect(dpi, 0, y, 800, y + 9, 0x2000031);
			string_format = 1193;
		}
		rct_ride* ride = GET_RIDE(w->list_item_positions[list_index]);

		gfx_draw_string_left(dpi, string_format, (void*)&ride->name, 0, 0, y - 1);
	}
}

/* rct2: 0x00697C16 */
void window_guest_finance_resize(rct_window *w)
{
	window_set_resize(w, 210, 134, 210, 134);
}

/* rct2: 0x00697BF8 */
void window_guest_finance_update(rct_window *w)
{
	w->frame_no++;

	widget_invalidate(w, WIDX_TAB_2);
	widget_invalidate(w, WIDX_TAB_4);
}

/* rct2: 0x00697968 */
void window_guest_finance_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	if (window_guest_page_widgets[w->page] != w->widgets){
		w->widgets = window_guest_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	rct_peep* peep = GET_PEEP(w->number);

	RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
	RCT2_GLOBAL(0x13CE954, uint32) = peep->id;

	window_guest_finance_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_guest_finance_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	window_guest_finance_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	window_guest_finance_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

	window_guest_finance_widgets[WIDX_TITLE].right = w->width - 2;

	window_guest_finance_widgets[WIDX_CLOSE].left = w->width - 13;
	window_guest_finance_widgets[WIDX_CLOSE].right = w->width - 3;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/* rct2: 0x00697A08 */
void window_guest_finance_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_guest_overview_tab_paint(w, dpi);
	window_guest_stats_tab_paint(w, dpi);
	window_guest_rides_tab_paint(w, dpi);
	window_guest_finance_tab_paint(w, dpi);
	window_guest_thoughts_tab_paint(w, dpi);
	window_guest_inventory_tab_paint(w, dpi);

	rct_peep* peep = GET_PEEP(w->number);

	//cx
	int x = w->x + window_guest_finance_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	//dx
	int y = w->y + window_guest_finance_widgets[WIDX_PAGE_BACKGROUND].top + 4;

	// Cash in pocket
	RCT2_GLOBAL(0x13CE952, money32) = peep->cash_in_pocket;
	gfx_draw_string_left(dpi, 1457, (void*)0x13CE952, 0, x, y);

	// Cash spent
	y += 10;
	RCT2_GLOBAL(0x13CE952, money32) = peep->cash_spent;
	gfx_draw_string_left(dpi, 1456, (void*)0x13CE952, 0, x, y);

	y += 20;
	gfx_fill_rect_inset(dpi, x, y - 6, x + 179, y - 5, w->colours[1], 32);

	// Paid to enter
	RCT2_GLOBAL(0x13CE952, money32) = peep->paid_to_enter;
	gfx_draw_string_left(dpi, 2296, (void*)0x13CE952, 0, x, y);

	// Paid on rides
	y += 10;
	RCT2_GLOBAL(0x13CE952, money32) = peep->paid_on_rides;
	RCT2_GLOBAL(0x13CE956, uint16) = peep->no_of_rides;
	if (peep->no_of_rides != 1){
		gfx_draw_string_left(dpi, 2298, (void*)0x13CE952, 0, x, y);
	}
	else{
		gfx_draw_string_left(dpi, 2297, (void*)0x13CE952, 0, x, y);
	}

	// Paid on food
	y += 10;
	RCT2_GLOBAL(0x13CE952, money32) = peep->paid_on_food;
	RCT2_GLOBAL(0x13CE956, uint16) = peep->no_of_food;
	if (peep->no_of_food != 1){
		gfx_draw_string_left(dpi, 2300, (void*)0x13CE952, 0, x, y);
	}
	else{
		gfx_draw_string_left(dpi, 2299, (void*)0x13CE952, 0, x, y);
	}

	// Paid on drinks
	y += 10;
	RCT2_GLOBAL(0x13CE952, money32) = peep->paid_on_drink;
	RCT2_GLOBAL(0x13CE956, uint16) = peep->no_of_drinks;
	if (peep->no_of_drinks != 1){
		gfx_draw_string_left(dpi, 2302, (void*)0x13CE952, 0, x, y);
	}
	else{
		gfx_draw_string_left(dpi, 2301, (void*)0x13CE952, 0, x, y);
	}

	// Paid on souvenirs
	y += 10;
	RCT2_GLOBAL(0x13CE952, money32) = peep->paid_on_souvenirs;
	RCT2_GLOBAL(0x13CE956, uint16) = peep->no_of_souvenirs;
	if (peep->no_of_souvenirs != 1){
		gfx_draw_string_left(dpi, 2304, (void*)0x13CE952, 0, x, y);
	}
	else{
		gfx_draw_string_left(dpi, 2303, (void*)0x13CE952, 0, x, y);
	}
}

/* rct2: 0x00697E33 */
void window_guest_thoughts_resize(rct_window *w)
{
	rct_peep* peep = GET_PEEP(w->number);
	if (peep->window_invalidate_flags & PEEP_INVALIDATE_PEEP_THOUGHTS){
		peep->window_invalidate_flags &= ~PEEP_INVALIDATE_PEEP_THOUGHTS;
		window_invalidate(w);
	}

	window_set_resize(w, 192, 159, 500, 450);
}

/* rct2: 0x00697EB4 */
void window_guest_thoughts_update(rct_window *w)
{
	w->frame_no++;

	widget_invalidate(w, WIDX_TAB_2);
	widget_invalidate(w, WIDX_TAB_5);
}

/* rct2: 0x00697C8A */
void window_guest_thoughts_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	if (window_guest_page_widgets[w->page] != w->widgets){
		w->widgets = window_guest_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	rct_peep* peep = GET_PEEP(w->number);

	RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
	RCT2_GLOBAL(0x13CE954, uint32) = peep->id;

	window_guest_thoughts_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_guest_thoughts_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

	window_guest_thoughts_widgets[WIDX_TITLE].right = w->width - 2;

	window_guest_thoughts_widgets[WIDX_CLOSE].left = w->width - 13;
	window_guest_thoughts_widgets[WIDX_CLOSE].right = w->width - 3;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/* rct2: 0x00697D2A */
void window_guest_thoughts_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_guest_overview_tab_paint(w, dpi);
	window_guest_stats_tab_paint(w, dpi);
	window_guest_rides_tab_paint(w, dpi);
	window_guest_finance_tab_paint(w, dpi);
	window_guest_thoughts_tab_paint(w, dpi);
	window_guest_inventory_tab_paint(w, dpi);

	rct_peep* peep = GET_PEEP(w->number);

	//cx
	int x = w->x + window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	//dx
	int y = w->y + window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].top + 4;

	gfx_draw_string_left(dpi, 1654, (void*)0, 0, x, y);

	y += 10;
	for (rct_peep_thought* thought = peep->thoughts; thought < &peep->thoughts[PEEP_MAX_THOUGHTS]; ++thought){
		if (thought->type == PEEP_THOUGHT_TYPE_NONE) return;
		if (thought->var_2 == 0) continue;

		uint32 argument1, argument2;
		get_arguments_from_thought(*thought, &argument1, &argument2);
		RCT2_GLOBAL(0x13CE952, uint32) = argument1;
		RCT2_GLOBAL(0x13CE956, uint32) = argument2;

		int width = window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].right 
			- window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].left
			- 8;

		y += gfx_draw_string_left_wrapped(dpi, (void*)0x13CE952, x, y, width, 1191, 0);

		// If this is the last visable line end drawing.
		if (y > w->y + window_guest_thoughts_widgets[WIDX_PAGE_BACKGROUND].bottom - 32) return;
	}
}


/* rct2: 0x00698294 */
void window_guest_inventory_resize(rct_window *w)
{
	rct_peep* peep = GET_PEEP(w->number);
	if (peep->window_invalidate_flags & PEEP_INVALIDATE_PEEP_INVENTORY){
		peep->window_invalidate_flags &= ~PEEP_INVALIDATE_PEEP_INVENTORY;
		window_invalidate(w);
	}

	window_set_resize(w, 192, 159, 500, 450);
}

/* rct2: 0x00698315 */
void window_guest_inventory_update(rct_window *w)
{
	w->frame_no++;

	widget_invalidate(w, WIDX_TAB_2);
	widget_invalidate(w, WIDX_TAB_6);
}

/* rct2: 0x00697EE1 */
void window_guest_inventory_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	if (window_guest_page_widgets[w->page] != w->widgets){
		w->widgets = window_guest_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	rct_peep* peep = GET_PEEP(w->number);

	RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
	RCT2_GLOBAL(0x13CE954, uint32) = peep->id;

	window_guest_inventory_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_guest_inventory_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	window_guest_inventory_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	window_guest_inventory_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

	window_guest_inventory_widgets[WIDX_TITLE].right = w->width - 2;

	window_guest_inventory_widgets[WIDX_CLOSE].left = w->width - 13;
	window_guest_inventory_widgets[WIDX_CLOSE].right = w->width - 3;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

static rct_string_id window_guest_inventory_format_item(rct_peep *peep, int item, uint8 *args)
{
	rct_ride *ride;

	// Default arguments
	RCT2_GLOBAL(args + 0, uint32) = ShopItemImage[item];
	RCT2_GLOBAL(args + 4, uint16) = ShopItemStringIds[item].display;
	RCT2_GLOBAL(args + 6, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_PARK_NAME, uint16);
	RCT2_GLOBAL(args + 8, uint32) = RCT2_GLOBAL(RCT2_ADDRESS_PARK_NAME_ARGS, uint32);

	// Special overrides
	switch (item) {
	case SHOP_ITEM_BALLOON:
		RCT2_GLOBAL(args + 0, uint32) |= 0x20000000 | peep->balloon_colour << 19;
		break;
	case SHOP_ITEM_PHOTO:
		ride = GET_RIDE(peep->photo1_ride_ref);
		RCT2_GLOBAL(args + 6, uint16) = ride->name;
		RCT2_GLOBAL(args + 8, uint32) = ride->name_arguments;
		break;
	case SHOP_ITEM_UMBRELLA:
		RCT2_GLOBAL(args + 0, uint32) |= 0x20000000 | peep->umbrella_colour << 19;
		break;
	case SHOP_ITEM_VOUCHER:
		switch (peep->voucher_type) {
		case VOUCHER_TYPE_PARK_ENTRY_FREE:
			RCT2_GLOBAL(args + 6, uint16) = 2418;
			RCT2_GLOBAL(args + 8, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_PARK_NAME, uint16);
			RCT2_GLOBAL(args + 10, uint32) = RCT2_GLOBAL(RCT2_ADDRESS_PARK_NAME_ARGS, uint32);
			break;
		case VOUCHER_TYPE_RIDE_FREE:
			ride = GET_RIDE(peep->voucher_arguments);
			RCT2_GLOBAL(args + 6, uint16) = 2419;
			RCT2_GLOBAL(args + 8, uint16) = ride->name;
			RCT2_GLOBAL(args + 10, uint32) = ride->name_arguments;
			break;
		case VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE:
			RCT2_GLOBAL(args + 6, uint16) = 2420;
			RCT2_GLOBAL(args + 8, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_PARK_NAME, uint16);
			RCT2_GLOBAL(args + 10, uint32) = RCT2_GLOBAL(RCT2_ADDRESS_PARK_NAME_ARGS, uint32);
			break;
		case VOUCHER_TYPE_FOOD_OR_DRINK_FREE:
			RCT2_GLOBAL(args + 6, uint16) = 2421;
			RCT2_GLOBAL(args + 8, uint16) = ShopItemStringIds[peep->voucher_arguments].singular;
			break;
		}
		break;
	case SHOP_ITEM_HAT:
		RCT2_GLOBAL(args + 0, uint32) |= 0x20000000 | peep->hat_colour << 19;
		break;
	case SHOP_ITEM_TSHIRT:
		RCT2_GLOBAL(args + 0, uint32) |= 0x20000000 | peep->tshirt_colour << 19;
		break;
	case SHOP_ITEM_PHOTO2:
		ride = GET_RIDE(peep->photo2_ride_ref);
		RCT2_GLOBAL(args + 6, uint16) = ride->name;
		RCT2_GLOBAL(args + 8, uint32) = ride->name_arguments;
		break;
	case SHOP_ITEM_PHOTO3:
		ride = GET_RIDE(peep->photo3_ride_ref);
		RCT2_GLOBAL(args + 6, uint16) = ride->name;
		RCT2_GLOBAL(args + 8, uint32) = ride->name_arguments;
		break;
	case SHOP_ITEM_PHOTO4:
		ride = GET_RIDE(peep->photo4_ride_ref);
		RCT2_GLOBAL(args + 6, uint16) = ride->name;
		RCT2_GLOBAL(args + 8, uint32) = ride->name_arguments;
		break;
	}

	return STR_GUEST_ITEM_FORMAT;
}

/* rct2: 0x00697F81 */
void window_guest_inventory_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_guest_overview_tab_paint(w, dpi);
	window_guest_stats_tab_paint(w, dpi);
	window_guest_rides_tab_paint(w, dpi);
	window_guest_finance_tab_paint(w, dpi);
	window_guest_thoughts_tab_paint(w, dpi);
	window_guest_inventory_tab_paint(w, dpi);

	rct_peep *peep = GET_PEEP(w->number);

	rct_widget *pageBackgroundWidget = &window_guest_inventory_widgets[WIDX_PAGE_BACKGROUND];
	int x = w->x + pageBackgroundWidget->left + 4;
	int y = w->y + pageBackgroundWidget->top + 2;
	int itemNameWidth = pageBackgroundWidget->right - pageBackgroundWidget->left - 8;

	int maxY = w->y + w->height - 22;
	int numItems = 0;

	gfx_draw_string_left(dpi, STR_CARRYING, NULL, 0, x, y);
	y += 10;

	for (int item = 0; item < SHOP_ITEM_COUNT; item++) {
		if (y >= maxY) break;
		if (!peep_has_item(peep, item)) continue;
		
		void *args = (void*)0x013CE952;
		rct_string_id stringId = window_guest_inventory_format_item(peep, item, (uint8*)args);
		y += gfx_draw_string_left_wrapped(dpi, args, x, y, itemNameWidth, stringId, 0);
		numItems++;
	}

	if (numItems == 0) {
		gfx_draw_string_left(dpi, STR_NOTHING, (void*)0, 0, x, y);
	}
}
