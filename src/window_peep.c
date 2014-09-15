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
#include "map.h"
#include "ride.h"
#include "peep.h"
#include "scenario.h"
#include "string_ids.h"
#include "staff.h"
#include "sprite.h"
#include "sprites.h"
#include "viewport.h"
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"
#include "window_error.h"

enum WINDOW_PEEP_PAGE {
	WINDOW_PEEP_OVERVIEW,
	WINDOW_PEEP_STATS,
	WINDOW_PEEP_RIDES,
	WINDOW_PEEP_FINANCE,
	WINDOW_PEEP_THOUGHTS,
	WINDOW_PEEP_INVENTORY
};

enum WINDOW_PEEP_WIDGET_IDX {
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

void window_peep_emptysub(){};

rct_widget window_peep_overview_widgets[] = { 
	{ WWT_FRAME,	0, 0,	191,	0,		156,	0x0FFFFFFFF,	STR_NONE },				// Panel / Background
	{ WWT_CAPTION,	0, 1,	190,	1,		14,		865,			STR_WINDOW_TITLE_TIP }, // Title
	{ WWT_CLOSEBOX, 0, 179, 189,	2,		13,		824,			STR_CLOSE_WINDOW_TIP }, // Close x button
	{ WWT_RESIZE,	1, 1,	191,	43,		156,	0x0FFFFFFFF,	STR_NONE },				// Resize
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

rct_widget window_peep_stats_widgets[] = {
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

rct_widget window_peep_rides_widgets[] = {
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

rct_widget window_peep_finance_widgets[] = {
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

rct_widget window_peep_thoughts_widgets[] = {
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

rct_widget window_peep_inventory_widgets[] = {
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
rct_widget *window_peep_page_widgets[] = {
	window_peep_overview_widgets,
	window_peep_stats_widgets,
	window_peep_rides_widgets,
	window_peep_finance_widgets,
	window_peep_thoughts_widgets,
	window_peep_inventory_widgets
};

void window_peep_set_page(rct_window* w, int page);
void window_peep_disable_widgets(rct_window* w);
void window_peep_viewport_init(rct_window* w);

void window_peep_overview_close();
void window_peep_overview_resize();
void window_peep_overview_mouse_up();
void window_peep_overview_paint();
void window_peep_overview_invalidate();
void window_peep_overview_viewport_init_wrapper();
void window_peep_overview_update(rct_window* w);
void window_peep_overview_text_input();
void window_peep_overview_tool_update();
void window_peep_overview_tool_down();
void window_peep_overview_tool_abort();

static void* window_peep_overview_events[] = {
	window_peep_overview_close,
	window_peep_overview_mouse_up,
	window_peep_overview_resize,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_overview_update,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_overview_tool_update,//tool_update
	window_peep_overview_tool_down,//tool_down
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_overview_tool_abort,//tool_abort
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_overview_text_input,//text_input
	window_peep_overview_viewport_init_wrapper,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_overview_invalidate, //Invalidate
	window_peep_overview_paint, //Paint
	window_peep_emptysub
};

void window_peep_mouse_up();
void window_peep_unknown_05();

void window_peep_stats_resize();
void window_peep_stats_update();
void window_peep_stats_invalidate();
void window_peep_stats_paint();

static void* window_peep_stats_events[] = {
	window_peep_emptysub,
	window_peep_mouse_up, //mouse_up
	window_peep_stats_resize, //resize
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_unknown_05,
	window_peep_stats_update,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_stats_invalidate, //invalidate
	window_peep_stats_paint, //paint
	window_peep_emptysub
};

void window_peep_rides_resize();
void window_peep_rides_update();
void window_peep_rides_tooltip();
void window_peep_rides_scroll_get_size();
void window_peep_rides_scroll_mouse_down();
void window_peep_rides_scroll_mouse_over();
void window_peep_rides_invalidate();
void window_peep_rides_paint();
void window_peep_rides_scroll_paint();

static void* window_peep_rides_events[] = {
	window_peep_emptysub,
	window_peep_mouse_up, //mouse_up
	window_peep_rides_resize, //resize
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_unknown_05,
	window_peep_rides_update,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_rides_scroll_get_size, //scroll_get_size
	window_peep_rides_scroll_mouse_down, //scroll_mouse_down
	window_peep_emptysub,
	window_peep_rides_scroll_mouse_over, //scroll_mouse_over
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_rides_tooltip, //tooltip
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_rides_invalidate, //invalidate
	window_peep_rides_paint, //paint
	window_peep_rides_scroll_paint  //scroll_paint
};

void window_peep_finance_resize();
void window_peep_finance_update();
void window_peep_finance_invalidate();
void window_peep_finance_paint();

static void* window_peep_finance_events[] = {
	window_peep_emptysub,
	window_peep_mouse_up, //mouse_up
	window_peep_finance_resize, //resize
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_unknown_05,
	window_peep_finance_update,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_finance_invalidate, //invalidate
	window_peep_finance_paint, //paint
	window_peep_emptysub
};

void window_peep_thoughts_resize();
void window_peep_thoughts_update();
void window_peep_thoughts_invalidate();
void window_peep_thoughts_paint();

static void* window_peep_thoughts_events[] = {
	window_peep_emptysub,
	window_peep_mouse_up, //mouse_up
	window_peep_thoughts_resize, //resize
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_unknown_05,
	window_peep_thoughts_update,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_thoughts_invalidate, //invalidate
	window_peep_thoughts_paint, //paint
	window_peep_emptysub
};

void window_peep_inventory_resize();
void window_peep_inventory_update();
void window_peep_inventory_invalidate();
void window_peep_inventory_paint();

static void* window_peep_inventory_events[] = {
	window_peep_emptysub,
	window_peep_mouse_up, //mouse_up
	window_peep_inventory_resize, //resize
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_unknown_05,
	window_peep_inventory_update,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_inventory_invalidate, //invalidate
	window_peep_inventory_paint, //paint
	window_peep_emptysub
};

//0x981D24
void* window_peep_page_events[] = {
	window_peep_overview_events,
	window_peep_stats_events,
	window_peep_rides_events,
	window_peep_finance_events,
	window_peep_thoughts_events,
	window_peep_inventory_events
};

//0x981D3C
uint32 window_peep_page_enabled_widgets[] = {
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
void window_peep_open(rct_peep* peep){
	
	if (peep->type == PEEP_TYPE_STAFF){
		window_staff_peep_open(peep);
		return;
	}

	rct_window* window;

	window = window_bring_to_front_by_id(WC_PEEP, peep->sprite_index);
	if (window == NULL){
		window = window_create_auto_pos(192, 157, (uint32*)window_peep_overview_events, WC_PEEP, 0);
		window->widgets = window_peep_overview_widgets;
		window->enabled_widgets = window_peep_page_enabled_widgets[0];
		window->number = peep->sprite_index;
		window->page = 0;
		window->viewport_focus_coordinates.y = 0;
		window->frame_no = 0;
		window->list_information_type = 0;
		window->var_492 = 0;
		window->var_494 = 0;
		window_peep_disable_widgets(window);
		window->min_width = 192;
		window->min_height = 157;
		window->max_width = 500;
		window->max_height = 450;
		window->flags = WF_RESIZABLE;
		window->no_list_items = 0;
		window->selected_list_item = -1;
		window->colours[0] = 1;
		window->colours[1] = 15;
		window->colours[2] = 15;
		window->viewport_focus_coordinates.y = -1;
	}
	
	window->page = 0;
	window_invalidate(window);
	
	window->widgets = window_peep_page_widgets[WINDOW_PEEP_OVERVIEW];
	window->enabled_widgets = window_peep_page_enabled_widgets[WINDOW_PEEP_OVERVIEW];
	window->var_020 = RCT2_GLOBAL(0x981D54,uint32);
	window->event_handlers = window_peep_page_events[WINDOW_PEEP_OVERVIEW];
	window->pressed_widgets = 0;
	
	window_peep_disable_widgets(window);
	window_init_scroll_widgets(window);
	window_peep_viewport_init(window);
}

/* rct2: 0x006987A6 
 * Disables the finance tab when no money.
 * Disables peep pickup when in certain no pickup states.
 */
void window_peep_disable_widgets(rct_window* w){
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
void window_peep_overview_close(){
	rct_window* w;
	
	window_get_register(w);
	
	if (RCT2_GLOBAL(0x9DE518,uint32) & (1<<3)){
		if (w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS,rct_windowclass) && 
		    w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER,rct_windownumber)) 
			tool_cancel();
	}
}

/* rct2: 0x00696FBE */
void window_peep_overview_resize(){
	rct_window* w;
	
	window_get_register(w);
	
	window_peep_disable_widgets(w);
	RCT2_CALLPROC_EBPSAFE(w->event_handlers[WE_INVALIDATE]);
	
	widget_invalidate(WC_PEEP, w->number, WIDX_MARQUEE);
	
	window_set_resize(w, 192, 159, 500, 450);
	
	rct_viewport* view = w->viewport;
	
	if (view){
		if ((w->width - 30) == view->width){
			if ((w->height - 72) == view->height){
				window_peep_viewport_init(w);
				return;
			}
		}
		uint8 zoom_amount = 1 << view->zoom;
		view->width = w->width - 30;
		view->height = w->height - 72;
		view->view_width = view->width / zoom_amount;
		view->view_height = view->height / zoom_amount;
	}
	window_peep_viewport_init(w);
}

/* rct2: 0x00696A06 */
void window_peep_overview_mouse_up(){
	short widgetIndex;
	rct_window* w;
	window_widget_get_registers(w, widgetIndex);
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
		window_peep_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_PICKUP:
		if (!peep_can_be_picked_up(peep)) {
			return;
		}
		if (tool_set(w, widgetIndex, 7)) {
			return;
		}
		
		w->var_48C = peep->x;

		RCT2_CALLPROC_X(0x0069A512, 0, 0, 0, 0, (int)peep, 0, 0);
		RCT2_CALLPROC_X(0x006EC473, 0, 0, 0, 0, (int)peep, 0, 0);

		RCT2_CALLPROC_X(0x0069E9D3, 0x8000, 0, peep->y, peep->z, (int)peep, 0, 0);
		RCT2_CALLPROC_X(0x0069A409, 0, 0, 0, 0, (int)peep, 0, 0);
		peep->state = 9;
		peep->pad_2C = 0;
		RCT2_CALLPROC_X(0x0069A42F, 0, 0, 0, 0, (int)peep, 0, 0);
		break;
	case WIDX_RENAME:
		window_show_textinput(w, (int)widgetIndex, 0x5AC, 0x5AD, peep->name_string_idx);
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
void window_peep_set_page(rct_window* w, int page){
	if (RCT2_GLOBAL(0x9DE518,uint32) & (1 << 3))
	{
		if(w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber) &&
		   w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass))
			tool_cancel();
	
	}
	int listen = 0;
	if ( page == WINDOW_PEEP_OVERVIEW && w->page==WINDOW_PEEP_OVERVIEW && w->viewport){
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
	
	w->enabled_widgets = window_peep_page_enabled_widgets[page];
	w->var_020 = RCT2_ADDRESS(0x981D54,uint32)[page];
	w->event_handlers = window_peep_page_events[page];
	w->pressed_widgets = 0;
	w->widgets = window_peep_page_widgets[page];
	window_peep_disable_widgets(w);
	window_invalidate(w);
	
	RCT2_CALLPROC_X(w->event_handlers[WE_RESIZE], 0, 0, 0, 0, (int)w, 0, 0);
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	
	window_init_scroll_widgets(w);
	window_invalidate(w);
	
	if (listen && w->viewport) w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

void window_peep_overview_viewport_init_wrapper(){
	rct_window* w;
	window_get_register(w);
	
	window_peep_viewport_init(w);
}

/* rct2: 0x0069883C */
void window_peep_viewport_init(rct_window* w){
	if (w->page != WINDOW_PEEP_OVERVIEW) return;

	union{
		sprite_focus sprite;
		coordinate_focus coordinate;
	} focus; //The focus will be either a sprite or a coordinate.

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
			int x = ride->overall_view & 0xFF * 32 + 16;
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

	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);

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
void window_peep_overview_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1<<WIDX_TAB_1))
		return;
	
	rct_widget* widget = &w->widgets[WIDX_TAB_1];
	int width = widget->right - widget->left - 1;
	int height = widget->bottom - widget->top - 1;
	int x = widget->left + 1 + w->x;
	int y = widget->top + 1 + w->y;
	if (w->page == WINDOW_PEEP_OVERVIEW) height++;
	
	rct_drawpixelinfo* clip_dpi = clip_drawpixelinfo(dpi, x, width, y, height );
	if (!clip_dpi) return;
	
	x = 14;
	y = 20;
	
	rct_peep* peep = GET_PEEP(w->number);
	
	if (peep->type == PEEP_TYPE_STAFF && peep->staff_type == STAFF_TYPE_ENTERTAINER)
		y++;
	
	int ebx = *(RCT2_ADDRESS(0x982708, uint32*)[peep->sprite_type * 2]) + 1;
	
	int eax = 0;
	
	if (w->page == WINDOW_PEEP_OVERVIEW){
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
}

/* rct2: 0x69869b */
void window_peep_stats_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1 << WIDX_TAB_2))
		return;

	rct_widget* widget = &w->widgets[WIDX_TAB_2];
	int x = widget->left + w->x;
	int y = widget->top + w->y;

	rct_peep* peep = GET_PEEP(w->number);
	int image_id = get_peep_face_sprite_large(peep);
	if (w->page == WINDOW_PEEP_STATS){
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
void window_peep_rides_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1 << WIDX_TAB_3)) return;

	rct_widget* widget = &w->widgets[WIDX_TAB_3];
	int x = widget->left + w->x;
	int y = widget->top + w->y;

	int image_id = SPR_TAB_RIDE_0;

	if ( w->page == WINDOW_PEEP_RIDES ){
		image_id += (w->frame_no / 4) & 0xF;		
	}
	
	gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/* rct2: 0x698597 */
void window_peep_finance_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1 << WIDX_TAB_4)) return;

	rct_widget* widget = &w->widgets[WIDX_TAB_4];
	int x = widget->left + w->x;
	int y = widget->top + w->y;

	int image_id = SPR_TAB_FINANCES_SUMMARY_0;

	if ( w->page == WINDOW_PEEP_FINANCE ){
		image_id += (w->frame_no / 2) & 0x7;		
	}
	
	gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/* rct2: 0x6985D8 */
void window_peep_thoughts_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1 << WIDX_TAB_5)) return;

	rct_widget* widget = &w->widgets[WIDX_TAB_5];
	int x = widget->left + w->x;
	int y = widget->top + w->y;

	int image_id = 5269;

	if ( w->page == WINDOW_PEEP_THOUGHTS ){
		image_id += (w->frame_no / 2) & 0x7;		
	}
	
	gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/* rct2: 0x698661 */
void window_peep_inventory_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1 << WIDX_TAB_6)) return;

	rct_widget* widget = &w->widgets[WIDX_TAB_6];
	int x = widget->left + w->x;
	int y = widget->top + w->y;

	int image_id = 5326;
	
	gfx_draw_sprite(dpi, image_id, x, y, 0);
}

/* rct2: 0x696887 */
void window_peep_overview_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_peep_overview_tab_paint(w, dpi);
	window_peep_stats_tab_paint(w, dpi);
	window_peep_rides_tab_paint(w, dpi);
	window_peep_finance_tab_paint(w, dpi);
	window_peep_thoughts_tab_paint(w, dpi);
	window_peep_inventory_tab_paint(w, dpi);

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
}

/* rct2: 0x696749*/
void window_peep_overview_invalidate(){
	rct_window* w;
	window_get_register(w);
	
	if (window_peep_page_widgets[w->page] != w->widgets){
		w->widgets = window_peep_page_widgets[w->page];
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
	
	window_peep_overview_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_peep_overview_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	
	window_peep_overview_widgets[WIDX_PAGE_BACKGROUND].right =w->width - 1;
	window_peep_overview_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
	
	window_peep_overview_widgets[WIDX_TITLE].right = w->width - 2;
	
	window_peep_overview_widgets[WIDX_CLOSE].left = w->width - 13;
	window_peep_overview_widgets[WIDX_CLOSE].right = w->width - 3;

	window_peep_overview_widgets[WIDX_VIEWPORT].right = w->width - 26;
	window_peep_overview_widgets[WIDX_VIEWPORT].bottom = w->height - 14;

	window_peep_overview_widgets[WIDX_ACTION_LBL].top = w->height - 12;
	window_peep_overview_widgets[WIDX_ACTION_LBL].bottom = w->height - 3;
	window_peep_overview_widgets[WIDX_ACTION_LBL].right = w->width - 24;
	
	window_peep_overview_widgets[WIDX_MARQUEE].right = w->width - 24;

	window_peep_overview_widgets[WIDX_PICKUP].right = w->width - 2;
	window_peep_overview_widgets[WIDX_RENAME].right = w->width - 2;
	window_peep_overview_widgets[WIDX_LOCATE].right = w->width - 2;
	window_peep_overview_widgets[WIDX_TRACK].right = w->width - 2;

	window_peep_overview_widgets[WIDX_PICKUP].left = w->width - 25;
	window_peep_overview_widgets[WIDX_RENAME].left = w->width - 25;
	window_peep_overview_widgets[WIDX_LOCATE].left = w->width - 25;
	window_peep_overview_widgets[WIDX_TRACK].left = w->width - 25;
	
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/* rct2: 0x696F45 */
void window_peep_overview_update(rct_window* w){
	int var_496 = w->var_494 >> 16;
	var_496++;
	var_496 %= 24;
	w->var_494 &= 0x0000FFFF;
	w->var_494 |= var_496 << 16;

	widget_invalidate(WC_PEEP, w->number, WIDX_TAB_1);
	widget_invalidate(WC_PEEP, w->number, WIDX_TAB_2);
	
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
				RCT2_CALLPROC_X(0x699F5A, 0xFF47, 0, 0, 0, (int)peep, 0, 0);
			}
		}
	}
	
}

/* rct2:0x696A6A */
void window_peep_overview_text_input(){
	short widgetIndex;
	rct_window *w;
	char _cl;
	uint32* text;

	window_text_input_get_registers(w, widgetIndex, _cl, text);

	if (widgetIndex != WIDX_RENAME)return;

	if (!_cl) return;
	
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, uint16) = 0x5AE;

	game_do_command(1, 1, w->number, *text, 22, *(text + 2), *(text + 1));
	game_do_command(2, 1, 0, *(text + 3), 22, *(text + 5), *(text + 4));
	game_do_command(0, 1, 0, *(text + 6), 22, *(text + 8), *(text + 7));
}

/* rct2: 0x696A5F */
void window_peep_overview_tool_update(){
	short widgetIndex;
	rct_window* w;
	short x, y;

	window_tool_get_registers(w, widgetIndex, x, y);

	if (widgetIndex != WIDX_PICKUP) return;

	RCT2_CALLPROC_X(0x0068AAE1, x, y, 0, 0, (int)w, 0, 0);

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);
	int temp_y = y + 16;

	int eax = x, ecx = 0, edx = widgetIndex, edi = 0, esi = (int)w, ebp = 0;
	RCT2_CALLFUNC_X(0x689726, &eax, &temp_y, &ecx, &edx, &esi, &edi, &ebp);
	if (eax != 0x8000){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 1;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16) = eax;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16) = eax;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16) = temp_y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16) = temp_y;
		RCT2_CALLPROC_X(0x0068AAE1, eax, temp_y, 0, 0, (int)w, 0, 0);
	}

	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, sint32) = -1;
	eax = x;
	int ebx = y;
	edx = 0;
	RCT2_CALLFUNC_X(0x00685ADC, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	if ((ebx & 0xFF) == 0) return;

	x--;
	y += 16;
	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_X, uint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_Y, uint16) = y;
	w->var_492++;
	if (w->var_492 >= 48)w->var_492 = 0;
	
	rct_peep* peep;
	peep = GET_PEEP(w->number);
	ebx = (RCT2_ADDRESS(0x982708, uint32*)[peep->sprite_type * 2])[22];
	ebx += w->var_492 >> 2;

	ebp = peep->tshirt_colour << 19;
	ecx = peep->trousers_colour << 24;

	ebx |= ebp | ecx | 0xA0000000;
	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, uint32) = ebx;
}

/* rct2: 0x664F72 */
int sub_664F72(int x, int y, int edx){
	if (x > 0x1FFF || y > 0x1FFF){
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = 0x6C1;
		return 1;
	}

	rct_map_element* map_element = map_get_surface_element_at(x / 32, y / 32);
	if (map_element->properties.surface.ownership & 0x20) return 0;
	if (!(map_element->properties.surface.ownership & 0x10)){
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = 0x6C1;
		return 1;
	}

	edx >>= 3;
	if ((edx & 0xFF) < map_element->base_height)return 0;
	edx = (edx & 0xFF) - 2;
	if (edx > map_element->base_height)return 0;

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = 0x6C1;
	return 1;
}

/* rct2: 0x696A54 */
void window_peep_overview_tool_down(){
	short widgetIndex;
	rct_window* w;
	short x, y;

	window_tool_get_registers(w, widgetIndex, x, y);

	if (widgetIndex != WIDX_PICKUP) return;

	int eax = x, ebx = y, ecx = 0, edx = widgetIndex, edi = 0, esi = (int)w, ebp = 0;
	ebx += 16;
	RCT2_CALLFUNC_X(0x689726, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	if (eax == 0x8000)return;

	eax += 16;
	ecx = ebx + 16;
	edx = ((uint8*)edx)[2] * 8 + 16;
	int _eax = eax & 0xFFE0, _ebx = ebx & 0xFFE0;
	if (sub_664F72(eax & 0xFFE0, ebx & 0xFFE0, edx)){
		window_error_open(0x785,-1);
		return;
	}
	int _edx = edx>>3;
	_edx &= 0xFFFF00FF;
	_edx |= edx << 8;
	_edx += 0x100;
	int flags = RCT2_CALLPROC_X(0x68B93A, eax & 0xFFE0, 0xF, ebx & 0xFFE0, _edx, (int)w, 0, 0);

	if (flags & 0x100){
		if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) != 0x3A5 ){
			if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) != 0x49B){
				window_error_open(0x785, -1);
				return;
			}
		}
	}

	rct_peep* peep = GET_PEEP(w->number);
	RCT2_CALLPROC_X(0x0069E9D3, eax, 0, ebx, edx, (int)peep, 0, 0);
	RCT2_CALLPROC_X(0x006EC473, 0, 0, 0, 0, (int)peep, 0, 0);
	RCT2_CALLPROC_X(0x0069A409, 0, 0, 0, 0, (int)peep, 0, 0);
	peep->state = 0;
	RCT2_CALLPROC_X(0x0069A42F, 0, 0, 0, 0, (int)peep, 0, 0);
	peep->var_71 = 0xFF;
	peep->var_6D = 0;
	peep->var_70 = 0;
	peep->var_6E = 0xFF;
	peep->var_C4 = 0;

	peep->happiness_growth_rate -= 10;
	if (peep->happiness_growth_rate < 0)peep->happiness_growth_rate = 0;

	RCT2_CALLPROC_X(0x00693B58, 0, 0, 0, 0, (int)peep, 0, 0);
	tool_cancel();
	RCT2_GLOBAL(0x9DE550, sint32) = -1;
}

/* rct2: 0x696A49 */
void window_peep_overview_tool_abort(){
	short widgetIndex;
	rct_window* w;
	short x, y;

	window_tool_get_registers(w, widgetIndex, x, y);
	if (widgetIndex != WIDX_PICKUP) return;

	rct_peep* peep = GET_PEEP(w->number);
	if (peep->state != PEEP_STATE_PICKED) return;

	RCT2_CALLPROC_X(0x0069E9D3, w->var_48C, 0, peep->y, peep->z + 8, (int)peep, 0, 0);
	RCT2_CALLPROC_X(0x006EC473, 0, 0, 0, 0, (int)peep, 0, 0);

	if (peep->x != 0x8000){
		RCT2_CALLPROC_X(0x0069A409, 0, 0, 0, 0, (int)peep, 0, 0);
		peep->state = 0;
		RCT2_CALLPROC_X(0x0069A42F, 0, 0, 0, 0, (int)peep, 0, 0);
		peep->var_71 = 0xFF;
		peep->var_6D = 0;
		peep->var_70 = 0;
		peep->var_6E = 0;
		peep->var_C4 = 0;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_PICKEDUP_PEEP_SPRITE, sint32) = -1;
}

/* rct2:0x69744F, 0x697795, 0x697BDD, 0x697E18, 0x698279
 * This is a combination of 5 functions that were identical
 */
void window_peep_mouse_up(){
	short widgetIndex;
	rct_window* w;
	window_widget_get_registers(w, widgetIndex);

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
		window_peep_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	}
}

/* rct2: 0x697488 */
void window_peep_stats_resize(){
	rct_window* w;
	window_get_register(w);

	window_set_resize(w, 192, 162, 192, 162);
}

/* rct2: 0x6974ED, 0x00697959, 0x00697C7B, 0x00697ED2, 0x00698333 
 * This is a combination of 5 functions that were identical
 */
void window_peep_unknown_05(){
	rct_window* w;
	window_get_register(w);

	widget_invalidate(WC_PEEP, w->number, WIDX_TAB_1);
}

/* rct2: 0x69746A */
void window_peep_stats_update(){
	rct_window* w;
	window_get_register(w);

	w->frame_no++;
	rct_peep* peep = GET_PEEP(w->number);
	peep->var_45 &= ~(1<<1);

	window_invalidate(w);
}

/* rct2: 0x69707D */
void window_peep_stats_invalidate(){
	rct_window* w;
	window_get_register(w);

	if (w->widgets != window_peep_page_widgets[w->page]) {
		w->widgets = window_peep_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	rct_peep* peep = GET_PEEP(w->number);
	RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
	RCT2_GLOBAL(0x13CE954, uint32) = peep->id;

	window_peep_stats_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_peep_stats_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	window_peep_stats_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	window_peep_stats_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

	window_peep_stats_widgets[WIDX_TITLE].right = w->width - 2;

	window_peep_stats_widgets[WIDX_CLOSE].left = w->width - 13;
	window_peep_stats_widgets[WIDX_CLOSE].right = w->width - 3;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/* rct2: 0x0069711D */
void window_peep_stats_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_peep_overview_tab_paint(w, dpi);
	window_peep_stats_tab_paint(w, dpi);
	window_peep_rides_tab_paint(w, dpi);
	window_peep_finance_tab_paint(w, dpi);
	window_peep_thoughts_tab_paint(w, dpi);
	window_peep_inventory_tab_paint(w, dpi);

	//ebx
	rct_peep* peep = GET_PEEP(w->number);

	// Not sure why this is not stats widgets
	//cx
	int x = w->x + window_peep_rides_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	//dx
	int y = w->y + window_peep_rides_widgets[WIDX_PAGE_BACKGROUND].top + 4;

	//Happiness
	gfx_draw_string_left(dpi, 1662, (void*)0x13CE952, 0, x, y);

	int happiness = peep->happiness;
	if (happiness < 10)happiness = 10;
	int ebp = 14;
	if (happiness < 50){
		ebp |= 0x80000000;
	}
	RCT2_CALLPROC_X(0x6974FC, happiness, (int)peep, x, y, (int)w, (int)dpi, ebp);

	//Energy
	y += 10;
	gfx_draw_string_left(dpi, 1664, (void*)0x13CE952, 0, x, y);

	int energy = ((peep->energy - 32) * 85) / 32;
	ebp = 14;
	if (energy < 50){
		ebp |= 0x80000000;
	}
	if (energy < 10)energy = 10;
	RCT2_CALLPROC_X(0x6974FC, energy, (int)peep, x, y, (int)w, (int)dpi, ebp);

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
	RCT2_CALLPROC_X(0x6974FC, hunger, (int)peep, x, y, (int)w, (int)dpi, ebp);

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
	RCT2_CALLPROC_X(0x6974FC, thirst, (int)peep, x, y, (int)w, (int)dpi, ebp);

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
	RCT2_CALLPROC_X(0x6974FC, nausea, (int)peep, x, y, (int)w, (int)dpi, ebp);

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
	RCT2_CALLPROC_X(0x6974FC, bathroom, (int)peep, x, y, (int)w, (int)dpi, ebp);

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
void window_peep_rides_resize(){
	rct_window* w;
	window_get_register(w);

	window_set_resize(w, 192, 128, 500, 400);
}

/* rct2: 0x6977B0 */
void window_peep_rides_update(){
	rct_window* w;
	window_get_register(w);

	w->frame_no++;

	widget_invalidate(WC_PEEP, w->number, WIDX_TAB_2);
	widget_invalidate(WC_PEEP, w->number, WIDX_TAB_3);

	rct_peep* peep = GET_PEEP(w->number);

	// Every 2048 ticks do a full window_invalidate
	int number_of_ticks = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32) - peep->time_in_park;
	if (!(number_of_ticks & 0x7FF)) window_invalidate(w);

	uint8 curr_list_position = 0;
	for (uint8 ride_id = 0; ride_id < 255; ++ride_id){
		// Offset to the ride_id bit in peep_rides_been_on
		uint8 ride_id_bit = ride_id & 0x3;
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
void window_peep_rides_tooltip(){
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

/* rct2: 0x69784E */
void window_peep_rides_scroll_get_size(){
	rct_window *w;

	window_get_register(w);

	int height = w->no_list_items * 10;

	if (w->selected_list_item != -1){
		w->selected_list_item = -1;
		window_invalidate(w);
	}

	int visable_height = height 
		- window_peep_rides_widgets[WIDX_RIDE_SCROLL].bottom 
		+ window_peep_rides_widgets[WIDX_RIDE_SCROLL].top
		+ 21;

	if (visable_height < 0) visable_height = 0;

	if (visable_height < w->scrolls[0].v_top){
		w->scrolls[0].v_top = visable_height;
		window_invalidate(w);
	}

#ifdef _MSC_VER
	__asm mov ecx, 0
#else
	__asm__("mov ecx, 0 ");
#endif

#ifdef _MSC_VER
	__asm mov edx, height
#else
	__asm__("mov edx, %[height] " : [height] "+m" (height));
#endif
}

/* rct2: 0x006978CC */
void window_peep_rides_scroll_mouse_down(){
	int index;
	short x, y;
	rct_window *w;

	window_scrollmouse_get_registers(w, x, y);

	index = y / 10;
	if (index >= w->no_list_items) return;

	window_ride_main_open(w->list_item_positions[index]);
}

/* rct2: 0x0069789C */
void window_peep_rides_scroll_mouse_over(){
	int index;
	short x, y;
	rct_window *w;

	window_scrollmouse_get_registers(w, x, y);

	index = y / 10;
	if (index >= w->no_list_items)return;

	if (index == w->selected_list_item)return;
	w->selected_list_item = index;

	window_invalidate(w);
}

/* rct2: 0x0069757A */
void window_peep_rides_invalidate(){
	rct_window* w;
	window_get_register(w);
	if (window_peep_page_widgets[w->page] != w->widgets){
		w->widgets = window_peep_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	rct_peep* peep = GET_PEEP(w->number);
	RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
	RCT2_GLOBAL(0x13CE954, uint32) = peep->id;

	window_peep_rides_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_peep_rides_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	window_peep_rides_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	window_peep_rides_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

	window_peep_rides_widgets[WIDX_TITLE].right = w->width - 2;

	window_peep_rides_widgets[WIDX_CLOSE].left = w->width - 13;
	window_peep_rides_widgets[WIDX_CLOSE].right = w->width - 3;

	window_peep_rides_widgets[WIDX_RIDE_SCROLL].right = w->width - 4;
	window_peep_rides_widgets[WIDX_RIDE_SCROLL].bottom = w->height - 15;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/* rct2: 0x00697637 */
void window_peep_rides_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_peep_overview_tab_paint(w, dpi);
	window_peep_stats_tab_paint(w, dpi);
	window_peep_rides_tab_paint(w, dpi);
	window_peep_finance_tab_paint(w, dpi);
	window_peep_thoughts_tab_paint(w, dpi);
	window_peep_inventory_tab_paint(w, dpi);

	rct_peep* peep = GET_PEEP(w->number);

	//cx
	int x = w->x + window_peep_rides_widgets[WIDX_PAGE_BACKGROUND].left + 2;
	//dx
	int y = w->y + window_peep_rides_widgets[WIDX_PAGE_BACKGROUND].top + 2;

	gfx_draw_string_left(dpi, 2292, (void*)0, 0, x, y);

	y = w->y + window_peep_rides_widgets[WIDX_PAGE_BACKGROUND].bottom - 12;

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
void window_peep_rides_scroll_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

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
void window_peep_finance_resize(){
	rct_window* w;
	window_get_register(w);

	window_set_resize(w, 210, 134, 210, 134);
}

/* rct2: 0x00697BF8 */
void window_peep_finance_update(){
	rct_window* w;
	window_get_register(w);

	w->frame_no++;

	widget_invalidate(WC_PEEP, w->number, WIDX_TAB_2);
	widget_invalidate(WC_PEEP, w->number, WIDX_TAB_4);
}

/* rct2: 0x00697968 */
void window_peep_finance_invalidate(){
	rct_window* w;
	window_get_register(w);

	if (window_peep_page_widgets[w->page] != w->widgets){
		w->widgets = window_peep_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	rct_peep* peep = GET_PEEP(w->number);

	RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
	RCT2_GLOBAL(0x13CE954, uint32) = peep->id;

	window_peep_finance_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_peep_finance_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	window_peep_finance_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	window_peep_finance_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

	window_peep_finance_widgets[WIDX_TITLE].right = w->width - 2;

	window_peep_finance_widgets[WIDX_CLOSE].left = w->width - 13;
	window_peep_finance_widgets[WIDX_CLOSE].right = w->width - 3;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/* rct2: 0x00697A08 */
void window_peep_finance_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_peep_overview_tab_paint(w, dpi);
	window_peep_stats_tab_paint(w, dpi);
	window_peep_rides_tab_paint(w, dpi);
	window_peep_finance_tab_paint(w, dpi);
	window_peep_thoughts_tab_paint(w, dpi);
	window_peep_inventory_tab_paint(w, dpi);

	rct_peep* peep = GET_PEEP(w->number);

	//cx
	int x = w->x + window_peep_finance_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	//dx
	int y = w->y + window_peep_finance_widgets[WIDX_PAGE_BACKGROUND].top + 4;

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
	RCT2_GLOBAL(0x13CE956, uint16) = peep->staff_type;
	if (peep->staff_type != 1){
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
void window_peep_thoughts_resize(){
	rct_window* w;
	window_get_register(w);

	rct_peep* peep = GET_PEEP(w->number);
	if (peep->var_45 & 1){
		peep->var_45 &=~(1 << 0);
		window_invalidate(w);
	}

	window_set_resize(w, 192, 159, 500, 450);
}

/* rct2: 0x00697EB4 */
void window_peep_thoughts_update(){
	rct_window* w;
	window_get_register(w);

	w->frame_no++;

	widget_invalidate(WC_PEEP, w->number, WIDX_TAB_2);
	widget_invalidate(WC_PEEP, w->number, WIDX_TAB_5);
}

/* rct2: 0x00697C8A */
void window_peep_thoughts_invalidate(){
	rct_window* w;
	window_get_register(w);

	if (window_peep_page_widgets[w->page] != w->widgets){
		w->widgets = window_peep_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	rct_peep* peep = GET_PEEP(w->number);

	RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
	RCT2_GLOBAL(0x13CE954, uint32) = peep->id;

	window_peep_thoughts_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_peep_thoughts_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	window_peep_thoughts_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	window_peep_thoughts_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

	window_peep_thoughts_widgets[WIDX_TITLE].right = w->width - 2;

	window_peep_thoughts_widgets[WIDX_CLOSE].left = w->width - 13;
	window_peep_thoughts_widgets[WIDX_CLOSE].right = w->width - 3;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/* rct2: 0x00697D2A */
void window_peep_thoughts_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_peep_overview_tab_paint(w, dpi);
	window_peep_stats_tab_paint(w, dpi);
	window_peep_rides_tab_paint(w, dpi);
	window_peep_finance_tab_paint(w, dpi);
	window_peep_thoughts_tab_paint(w, dpi);
	window_peep_inventory_tab_paint(w, dpi);

	rct_peep* peep = GET_PEEP(w->number);

	//cx
	int x = w->x + window_peep_thoughts_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	//dx
	int y = w->y + window_peep_thoughts_widgets[WIDX_PAGE_BACKGROUND].top + 4;

	gfx_draw_string_left(dpi, 1654, (void*)0, 0, x, y);

	y += 10;
	for (rct_peep_thought* thought = peep->thoughts; thought < &peep->thoughts[PEEP_MAX_THOUGHTS]; ++thought){
		if (thought->type == PEEP_THOUGHT_TYPE_NONE) return;
		if (thought->var_2 == 0) continue;

		uint32 argument1, argument2;
		get_arguments_from_thought(*thought, &argument1, &argument2);
		RCT2_GLOBAL(0x13CE952, uint32) = argument1;
		RCT2_GLOBAL(0x13CE956, uint32) = argument2;

		int width = window_peep_thoughts_widgets[WIDX_PAGE_BACKGROUND].right 
			- window_peep_thoughts_widgets[WIDX_PAGE_BACKGROUND].left
			- 8;

		y += gfx_draw_string_left_wrapped(dpi, (void*)0x13CE952, x, y, width, 1191, 0);

		// If this is the last visable line end drawing.
		if (y > w->y + window_peep_thoughts_widgets[WIDX_PAGE_BACKGROUND].bottom - 32) return;
	}
}


/* rct2: 0x00698294 */
void window_peep_inventory_resize(){
	rct_window* w;
	window_get_register(w);

	rct_peep* peep = GET_PEEP(w->number);
	if (peep->var_45 & (1<<3)){
		peep->var_45 &= ~(1 << 3);
		window_invalidate(w);
	}

	window_set_resize(w, 192, 159, 500, 450);
}

/* rct2: 0x00698315 */
void window_peep_inventory_update(){
	rct_window* w;
	window_get_register(w);

	w->frame_no++;

	widget_invalidate(WC_PEEP, w->number, WIDX_TAB_2);
	widget_invalidate(WC_PEEP, w->number, WIDX_TAB_6);
}

/* rct2: 0x00697EE1 */
void window_peep_inventory_invalidate(){
	rct_window* w;
	window_get_register(w);

	if (window_peep_page_widgets[w->page] != w->widgets){
		w->widgets = window_peep_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	rct_peep* peep = GET_PEEP(w->number);

	RCT2_GLOBAL(0x13CE952, uint16) = peep->name_string_idx;
	RCT2_GLOBAL(0x13CE954, uint32) = peep->id;

	window_peep_inventory_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_peep_inventory_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	window_peep_inventory_widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	window_peep_inventory_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

	window_peep_inventory_widgets[WIDX_TITLE].right = w->width - 2;

	window_peep_inventory_widgets[WIDX_CLOSE].left = w->width - 13;
	window_peep_inventory_widgets[WIDX_CLOSE].right = w->width - 3;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_6);
}

/* rct2: 0x00697F81 */
void window_peep_inventory_paint(){
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_peep_overview_tab_paint(w, dpi);
	window_peep_stats_tab_paint(w, dpi);
	window_peep_rides_tab_paint(w, dpi);
	window_peep_finance_tab_paint(w, dpi);
	window_peep_thoughts_tab_paint(w, dpi);
	window_peep_inventory_tab_paint(w, dpi);

	rct_peep* peep = GET_PEEP(w->number);

	//cx
	int x = w->x + window_peep_inventory_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	//dx
	int y = w->y + window_peep_inventory_widgets[WIDX_PAGE_BACKGROUND].top + 2;

	int max_y = w->y + w->height - 22;
	int no_items = 0;

	gfx_draw_string_left(dpi, 1810, (void*)0, 0, x, y);

	y += 10;

	for (int i = 0; y < max_y && i < 28; ++i){
		int item_flag = 1 << i;
		if (!(peep->item_standard_flags & item_flag))continue;
		no_items++;

		RCT2_GLOBAL(0x13CE952, uint32) = 5061 + i;

		switch (item_flag){
		case PEEP_ITEM_TSHIRT:
			RCT2_GLOBAL(0x13CE952, uint32) |= 0x20000000 | peep->tshirt_colour << 19;
			break;
		case PEEP_ITEM_HAT:
			RCT2_GLOBAL(0x13CE952, uint32) |= 0x20000000 | peep->hat_colour << 19;
			break;
		case PEEP_ITEM_BALLOON:
			RCT2_GLOBAL(0x13CE952, uint32) |= 0x20000000 | peep->balloon_colour << 19;
			break;
		case PEEP_ITEM_UMBRELLA:
			RCT2_GLOBAL(0x13CE952, uint32) |= 0x20000000 | peep->umbrella_colour << 19;
			break;
		}

		int string_format = 2072 + i;
		if (string_format >= 2104) string_format += 84; //??? i is never 32

		RCT2_GLOBAL(0x13CE956, uint16) = string_format;
		RCT2_GLOBAL(0x13CE958, uint16) = RCT2_GLOBAL(0x13573D4, uint16);
		RCT2_GLOBAL(0x13CE95A, uint32) = RCT2_GLOBAL(0x13573D8, uint32);
		rct_ride* ride;

		switch (item_flag){
		case PEEP_ITEM_PHOTO:
			ride = GET_RIDE(peep->photo1_ride_ref);
			RCT2_GLOBAL(0x13CE958, uint16) = ride->name;
			RCT2_GLOBAL(0x13CE95A, uint32) = ride->name_arguments;
			break;
		case PEEP_ITEM_VOUCHER:
			RCT2_GLOBAL(0x13CE958, uint16) = peep->var_F0 + 2418;
			RCT2_GLOBAL(0x13CE95A, uint16) = RCT2_GLOBAL(0x13573D4, uint16);
			RCT2_GLOBAL(0x13CE95C, uint32) = RCT2_GLOBAL(0x13573D8, uint32);

			if (peep->var_F0 == 0 || peep->var_F0 == 2)break;

			int voucher_id = peep->var_F1 + 1988;
			if (voucher_id >= 2020) voucher_id += 102;

			RCT2_GLOBAL(0x13CE95A, uint16) = voucher_id;

			if (peep->var_F0 == 3)break;
			ride = GET_RIDE(peep->var_F1);
			RCT2_GLOBAL(0x13CE95A, uint16) = ride->name;
			RCT2_GLOBAL(0x13CE95C, uint32) = ride->name_arguments;
			break;
		}

		int width = window_peep_inventory_widgets[WIDX_PAGE_BACKGROUND].right
			- window_peep_inventory_widgets[WIDX_PAGE_BACKGROUND].left
			- 8;

		y += gfx_draw_string_left_wrapped(dpi, (void*)0x13CE952, x, y, width, 1875, 0);
	}

	for (int i = 0; y < max_y && i < 22; ++i){
		int item_flag = 1 << i;

		if (!(peep->item_extra_flags & item_flag))continue;
		no_items++;

		RCT2_GLOBAL(0x13CE952, uint32) = 5089 + i;
		RCT2_GLOBAL(0x13CE956, uint16) = 2188;
		RCT2_GLOBAL(0x13CE958, uint16) = RCT2_GLOBAL(0x13573D4, uint16);
		RCT2_GLOBAL(0x13CE95A, uint32) = RCT2_GLOBAL(0x13573D8, uint32);

		if (i < 3){
			int ride_id = 0;
			switch (item_flag){
			case PEEP_ITEM_PHOTO2:
				ride_id = peep->photo2_ride_ref;
				break;
			case PEEP_ITEM_PHOTO3:
				ride_id = peep->photo3_ride_ref;
				break;
			case PEEP_ITEM_PHOTO4:
				ride_id = peep->photo4_ride_ref;
				break;
			}

			rct_ride* ride = GET_RIDE(ride_id);
			RCT2_GLOBAL(0x13CE958, uint16) = ride->name;
			RCT2_GLOBAL(0x13CE95A, uint32) = ride->name_arguments;
		}

		int width = window_peep_inventory_widgets[WIDX_PAGE_BACKGROUND].right
			- window_peep_inventory_widgets[WIDX_PAGE_BACKGROUND].left
			- 8;

		y += gfx_draw_string_left_wrapped(dpi, (void*)0x13CE952, x, y, width, 1875, 0);
	}

	if (!no_items){
		gfx_draw_string_left(dpi, 2293, (void*)0, 0, x, y);
	}
}
