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
	WIDX_MARQUEE,
	WIDX_VIEWPORT,
	WIDX_ACTION_LBL,
	WIDX_RENAME,
	WIDX_PICKUP,
	WIDX_LOCATE,
	WIDX_TRACK
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
	{ WWT_FLATBTN,	1, 167, 190,	45,		68,		SPR_RENAME,		1706},					// Rename Button
	{ WWT_FLATBTN,	1, 167, 190,	69,		92,		0x1430,			1055},					// Pickup Button
	{ WWT_FLATBTN,	1, 167, 190,	93,		116,	SPR_LOCATE,		STR_LOCATE_SUBJECT_TIP},// Locate Button
	{ WWT_FLATBTN,	1, 167, 190,	117,	140,	SPR_TRACK_PEEP,	1930},					// Track Button
	{ WIDGETS_END },
};

//0x981D0C
rct_widget *window_peep_page_widgets[] = {
	window_peep_overview_widgets
};

void window_peep_close();
void window_peep_resize();

static void* window_peep_overview_events[] = {
	window_peep_close,
	(void*)0x696A06,
	window_peep_resize,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	(void*)0x696F45,
	window_peep_emptysub,
	window_peep_emptysub,
	(void*)0x696A5F,
	(void*)0x696A54,
	window_peep_emptysub,
	window_peep_emptysub,
	(void*)0x696A49,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	(void*)0x696A6A,
	(void*)0x697076,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	window_peep_emptysub,
	(void*)0x696749, //Invalidate
	(void*)0x696887, //Paint
	(void*)0x69707C
};

//0x981D24
void* window_peep_page_events[] = {
	window_peep_overview_events
};

//0x981D3C
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
		window->var_482 = 0;
		window->frame_no = 0;
		window->list_information_type = 0;
		window->var_492 = 0;
		window->var_494 = 0;
		RCT2_CALLPROC_X(0x006987A6, 0, 0, 0, 0, (int)window, 0, 0);
		window->min_width = 192;
		window->min_height = 157;
		window->max_width = 500;
		window->max_height = 450;
		window->flags = 1 << 8;
		window->no_list_items = 0;
		window->selected_list_item = -1;
		window->colours[0] = 1;
		window->colours[1] = 15;
		window->colours[2] = 15;
		window->var_482 = -1;
	}
	
	window->page = 0;
	window_invalidate(window);
	
	window->widgets = RCT2_GLOBAL(0x981D0C, rct_widget*);
	window->enabled_widgets = RCT2_GLOBAL(0x981D3C,uint32);
	window->var_020 = RCT2_GLOBAL(0x981D54,uint32);
	window->event_handlers = RCT2_GLOBAL(0x981D24,uint32*);
	window->pressed_widgets = 0;
	
	RCT2_CALLPROC_X(0x006987A6, 0, 0, 0, 0, (int)window, 0, 0);
	window_init_scroll_widgets(window);
	RCT2_CALLPROC_X(0x0069883C, 0, 0, 0, 0, (int)window, 0, 0);
}

/* rct2: 0x00696A75 */
void window_peep_close(){
	rct_window* w;
	
	window_get_register(w);
	
	if (RCT2_GLOBAL(0x9DE518,uint32) & (1<<3)){
		if (w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS,rct_windowclass) && 
		    w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER,rct_windownumber)) 
			tool_cancel();
	}
}

/* rct2: 0x00696FBE */
void window_peep_resize(){
	rct_window* w;
	
	window_get_register(w);
	
	RCT2_CALLPROC_EBPSAFE(0x6987a6);
	RCT2_CALLPROC_EBPSAFE(w->eventhandler[WE_INVALIDATE]);
	
	window_invalidate_by_id(0xA97,w->number);
	
	w->min_width = 192;
	w->max_width = 500;
	w->min_height = 159;
	w->max_height = 450;
	
	if (w->min_width > w->width){
		w->width = w->min_width;
		window_invalidate(w);
	}
	
	if (w->max_width < w->width){
		w->width = w->max_width;
		window_invalidate(w);
	}
	
	if (w->min_height > w->height){
		w->height = w->min_height;
		window_invalidate(w);
	}
	
	if (w->max_height < w->height){
		w->height = w->max_height;
		window_invalidate(w);
	}
	
	rct_viewport* view = w->viewport;
	
	if (view){
		if ((w->width - 30) == view->width){
			if ((w->height - 72) == view->height){
				RCT2_CALLPROC_X(0x0069883C, 0, 0, 0, 0, (int)window, 0, 0);
				return;
			}
		}
		uint8 zoom_amount = 1 << view->zoom;
		view->width = w->width - 30;
		view->height = w->height - 72;
		view->view_width = view->width / zoom_amount;
		view->view_height = view->height / zoom_amount;
	}
	RCT2_CALLPROC_X(0x0069883C, 0, 0, 0, 0, (int)window, 0, 0);
}

void window_peep_overview_mouse_down(int widgetIndex, rct_window* w, rct_widget* widget){
	switch(widgetIndex){
	case WIDX_CLOSE:
		//6ecd4c
		break;
	case WIDX_PAGE_BACKGROUND:
		//696a20
		break;
	case WIDX_TAB1:
	case WIDX_TAB2:
	case WIDX_TAB3:
	case WIDX_TAB4:
	case WIDX_TAB5:
	case WIDX_TAB6:
		window_peep_set_page(w, widgetIndex - WIDX_TAB1);
		break;
	case WIDX_RENAME:
		//696ba6
		break;
	case WIDX_PICKUP:
		//696e4d
		break;
	case WIDX_LOCATE:
		//696ee9
		break;
	case WIDX_TRACK:
		g_sprite_list[w->number].peep.flags ^= PEEP_FLAGS_TRACKING;
		break;
	}
}

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
	RCT2_CALLPROC_X(0x6987A6, 0, 0, 0, 0, (int) w, 0, 0);
	window_invalidate(w);
	
	RCT2_CALLPROC_X(w->event_handlers[WE_RESIZE], 0, 0, 0, 0, (int)w, 0, 0);
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	
	window_init_scroll_widgets(w);
	window_invalidate(w);
	
	if (listen && w->viewport) w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}
