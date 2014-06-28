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
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"

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
	{ WWT_12,		1, 3,	166,	45,		56,		0x0FFFFFFFF,	STR_NONE},				// Label 1
	{ WWT_VIEWPORT, 1, 3,	166,	57,		143,	0x0FFFFFFFF,	STR_NONE },				// Viewport
	{ WWT_12,		1, 3,	166,	144,	154,	0x0FFFFFFFF,	STR_NONE},				// Label 2
	{ WWT_FLATBTN,	1, 167, 190,	45,		68,		SPR_RENAME,		1706},					// Rename Button
	{ WWT_FLATBTN,	1, 167, 190,	69,		92,		0x1430,			1055},					// Pickup Button
	{ WWT_FLATBTN,	1, 167, 190,	93,		116,	SPR_LOCATE,		STR_LOCATE_SUBJECT_TIP},// Locate Button
	{ WWT_FLATBTN,	1, 167, 190,	117,	140,	SPR_TRACK_PEEP,	1930},					// Track Button
	{ WIDGETS_END },
};

rct_widget *window_peep_page_widgets[] = {
	window_peep_overview_widgets
};

static void* window_peep_overview_events[] = {
	0x696A75,
	0x696A06,
	0x696FBE,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	0x696F45,
	window_peep_emptysub,
	window_peep_emptysub,
	0x696A5F,
	0x696A54,
	window_peep_emptysub,
	window_peep_emptysub,
	0x696A49,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	0x696A6A,
	0x697076,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	0x696749, //Invalidate
	0x696887, //Paint
	0x69707C
};

void* window_peep_page_events[] = {
	window_peep_overview_events
};

uint32 window_peep_page_enabled_widgets[] = {
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
		RCT2_CALLPROC_X(0x006989E9, 0, 0, 0, (int)peep, 0, 0, 0);
	}

	rct_window* window;

	window = window_bring_to_front_by_id(WC_PEEP, peep->sprite_index);
	if (window == NULL){
		window = window_create_auto_pos(192, 157, (uint32*)window_peep_overview_events, WC_PEEP, 0);
		window->widgets = window_peep_overview_widgets;
		window->enabled_widgets = window_peep_page_enabled_widgets[0];
		window->number = peep->sprite_index;
		window->page = 0;
		window->var_482 = 0;
		window->var_48E = 0;
		window->var_490 = 0;
		window->var_492 = 0;
		window->var_494 = 0;
		//Call 6987a6
		window->min_width = 192;
		window->min_height = 157;
		window->max_width = 500;
		window->max_height = 450;
		window->flags = 8;
		window->var_476 = 0;
		window->var_47A = -1;
		window->colours[0] = 1;
		window->colours[1] = 15;
		window->colours[2] = 15;
		window->var_482 = -1;
	}
	
	window->page = 0;
	//call 6EB13A
	
	window->widgets = RCT2_GLOBAL(0x981D0C, uint32);
	window->enabled_widgets = RCT2_GLOBAL(0x981D3C,uint32);
	window->var_020 = RCT2_GLOBAL(0x981D54,uint32);
	window->event_handlers = RCT2_GLOBAL(0x981D24,uint32);
	window->pressed_widgets = 0;
	
	//call 6987A6
	window_init_scroll_widgets(window);
	//call 69883c
	
	
	//RCT2_CALLPROC_X(0x006989E9, 0, 0, 0, (int)peep, 0, 0, 0);
	
}
