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
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../sprites.h"
#include "../world/sprite.h"
#include "dropdown.h"

enum WINDOW_STAFF_PAGE {
	WINDOW_STAFF_OVERVIEW,
	WINDOW_STAFF_OPTIONS,
	WINDOW_STAFF_STATISTICS,
};

enum WINDOW_STAFF_WIDGET_IDX {
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
	WIDX_FIRE,

	WIDX_CHECKBOX_1 = 8,
	WIDX_CHECKBOX_2,
	WIDX_CHECKBOX_3,
	WIDX_CHECKBOX_4,
	WIDX_COSTUME_BOX,
	WIDX_COSTUME_BTN,
};

void window_staff_emptysub(){};

rct_widget window_staff_overview_widgets[] = { 
	{ WWT_FRAME,	0, 0,	189,	0,		179,	0x0FFFFFFFF,	STR_NONE },				// Panel / Background
	{ WWT_CAPTION,	0, 1,	188,	1,		14,		0x361,			STR_WINDOW_TITLE_TIP }, // Title
	{ WWT_CLOSEBOX, 0, 177, 187,	2,		13,		STR_CLOSE_X,	STR_CLOSE_WINDOW_TIP }, // Close x button
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

//0x9AF910
rct_widget window_staff_stats_widgets[] = { 
	{ WWT_FRAME,			0, 0,	189,	0,		179,	0x0FFFFFFFF,	STR_NONE },				// Panel / Background
	{ WWT_CAPTION,			0, 1,	188,	1,		14,		0x361,			STR_WINDOW_TITLE_TIP }, // Title
	{ WWT_CLOSEBOX,			0, 177, 187,	2,		13,		STR_CLOSE_X,	STR_CLOSE_WINDOW_TIP }, // Close x button
	{ WWT_RESIZE,			1, 0,	189,	43,		179,	0x0FFFFFFFF,	STR_NONE },				// Resize
	{ WWT_TAB,				1, 3,	33,		17,		43,		0x2000144E,		1939 },					// Tab 1
	{ WWT_TAB,				1, 34,	64,		17,		43,		0x2000144E,		1945},					// Tab 2
	{ WWT_TAB,				1, 65,	95,		17,		43,		0x2000144E,		2348},					// Tab 3
	{ WWT_TAB,				1, 96,	126,	17,		43,		0x2000144E,		STR_NONE},				// Tab 4
	{ WWT_CHECKBOX,			1, 5,	184,	50,		61,		0x0FFFFFFFF,	STR_NONE},				// Checkbox 1
	{ WWT_CHECKBOX,			1, 5,	184,	67,		78,		0x0FFFFFFFF,	STR_NONE },				// Checkbox 2
	{ WWT_CHECKBOX,			1, 5,	184,	84,		95,		0x0FFFFFFFF,	STR_NONE},				// Checkbox 3
	{ WWT_CHECKBOX,			1, 5,	184,	101,	112,	0x0FFFFFFFF,	STR_NONE},				// Checkbox 4
	{ WWT_DROPDOWN,			1, 5,	184,	50,		61,		0x0FFFFFFFF,	STR_NONE},				// Costume Dropdown
	{ WWT_DROPDOWN_BUTTON,	1, 173, 183,	51,		60,		876,			1946},					// Costume Dropdown Button
	{ WIDGETS_END },
};

//0x9AF9F4
rct_widget window_staff_options_widgets[] = { 
	{ WWT_FRAME,			0, 0,	189,	0,		179,	0x0FFFFFFFF,	STR_NONE },				// Panel / Background
	{ WWT_CAPTION,			0, 1,	188,	1,		14,		865,			STR_WINDOW_TITLE_TIP }, // Title
	{ WWT_CLOSEBOX,			0, 177, 187,	2,		13,		STR_CLOSE_X,	STR_CLOSE_WINDOW_TIP }, // Close x button
	{ WWT_RESIZE,			1, 0,	189,	43,		179,	0x0FFFFFFFF,	STR_NONE },				// Resize
	{ WWT_TAB,				1, 3,	33,		17,		43,		0x2000144E,		1939 },					// Tab 1
	{ WWT_TAB,				1, 34,	64,		17,		43,		0x2000144E,		1945},					// Tab 2
	{ WWT_TAB,				1, 65,	95,		17,		43,		0x2000144E,		2348},					// Tab 3
	{ WWT_TAB,				1, 96,	126,	17,		43,		0x2000144E,		STR_NONE},				// Tab 4
	{ WIDGETS_END },
};

rct_widget *window_staff_page_widgets[] = {
	window_staff_overview_widgets,
	window_staff_options_widgets,
	window_staff_stats_widgets
};

void window_staff_set_page(rct_window* w, int page);
void window_staff_disable_widgets(rct_window* w);
void window_staff_unknown_05();

void window_staff_overview_close();
void window_staff_overview_mouseup();
void window_staff_overview_resize();
void window_staff_overview_mousedown(int widgetIndex, rct_window* w, rct_widget* widget);
void window_staff_overview_dropdown();
void window_staff_overview_update(rct_window* w);

void window_staff_orders_mouseup();
void window_staff_orders_update(rct_window* w);

void window_staff_stats_mouseup();
void window_staff_stats_resize();
void window_staff_stats_update(rct_window* w);

// 0x992AEC
static void* window_staff_overview_events[] = {
	window_staff_overview_close,
	window_staff_overview_mouseup,
	window_staff_overview_resize,
	window_staff_overview_mousedown,
	window_staff_overview_dropdown,
	window_staff_emptysub,
	window_staff_overview_update,
	window_staff_emptysub,
	window_staff_emptysub,
	(void*)0x6BDFD8,
	(void*)0x6BDFC3,
	window_staff_emptysub,
	window_staff_emptysub,
	(void*)0x6BDFAE,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	(void*)0x6BDFED,
	(void*)0x6BE5FC,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	(void*)0x6BDD91, //Invalidate
	(void*)0x6BDEAF, //Paint
	window_staff_emptysub
};

// 0x992B5C
static void* window_staff_orders_events[] = {
	window_staff_emptysub,
	window_staff_orders_mouseup,
	window_staff_stats_resize,
	(void*)0x6BE802,
	(void*)0x6BE809,
	window_staff_unknown_05,
	window_staff_orders_update,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	(void*)0x6BE62B, //Invalidate
	(void*)0x6BE7C6, //Paint
	window_staff_emptysub
};

// 0x992BCC
static void* window_staff_stats_events[] = {
	window_staff_emptysub,
	window_staff_stats_mouseup,
	window_staff_stats_resize,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_unknown_05,
	window_staff_stats_update,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	window_staff_emptysub,
	(void*)0x6BE9E9, //Invalidate
	(void*)0x6BEA86, //Paint
	window_staff_emptysub
};

void* window_staff_page_events[] = {
	window_staff_overview_events,
	window_staff_orders_events,
	window_staff_stats_events
};

uint32 window_staff_page_enabled_widgets[] = {
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
	(1 << WIDX_TAB_3) |
	(1 << WIDX_CHECKBOX_1) |
	(1 << WIDX_CHECKBOX_2) |
	(1 << WIDX_CHECKBOX_3) |
	(1 << WIDX_CHECKBOX_4) |
	(1 << WIDX_COSTUME_BTN),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3)
};

/**
*
*  rct2: 0x006BEE98
*/
void window_staff_open(rct_peep* peep)
{
	rct_window* w = window_bring_to_front_by_id(WC_PEEP, peep->sprite_index);
	if (w == NULL) {
		w = window_create_auto_pos(190, 180, (uint32*)window_staff_overview_events, WC_PEEP, (uint16)0x400);

		w->widgets = RCT2_GLOBAL(0x9AF81C, rct_widget*);
		w->enabled_widgets = RCT2_GLOBAL(0x9929B0, uint32);
		w->number = peep->sprite_index;
		w->page = 0;
		w->viewport_focus_coordinates.y = 0;
		w->frame_no = 0;

		RCT2_GLOBAL((int*)w + 0x496, uint16) = 0; // missing, var_494 should perhaps be uint16?

		window_staff_disable_widgets(w);

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

	w->widgets = window_staff_overview_widgets;
	w->enabled_widgets = window_staff_page_enabled_widgets[0];
	w->var_020 = RCT2_GLOBAL(0x9929BC, uint32);
	w->event_handlers = window_staff_page_events[0];
	w->pressed_widgets = 0;
	window_staff_disable_widgets(w);
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
void window_staff_disable_widgets(rct_window* w)
{
	rct_peep* peep = &g_sprite_list[w->number].peep;
	uint64 disabled_widgets = (1 << WIDX_TAB_4);

	if (peep->staff_type == STAFF_TYPE_SECURITY){
		disabled_widgets |= (1 << WIDX_TAB_2);
	}

	if (w->page == WINDOW_STAFF_OVERVIEW){
		if (peep_can_be_picked_up(peep)){
			if (w->disabled_widgets & (1 << WIDX_PICKUP))
				window_invalidate(w);
		}
		else{
			disabled_widgets |= (1 << WIDX_PICKUP);
			if (!(w->disabled_widgets & (1 << WIDX_PICKUP)))
				window_invalidate(w);
		}
	}

	w->disabled_widgets = disabled_widgets;
}

/**
 * Same as window_peep_overview_close.
 * rct2: 0x006BDFF8
 */
void window_staff_overview_close()
{
	rct_window* w;

	window_get_register(w);

	if (RCT2_GLOBAL(0x9DE518, uint32) & (1 << 3)){
		if (w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) &&
			w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber))
			tool_cancel();
	}
}

/**
 * Mostly similar to window_peep_set_page.
 * rct2: 0x006BE023
 */
void window_staff_set_page(rct_window* w, int page)
{
	if (RCT2_GLOBAL(0x9DE518,uint32) & (1 << 3))
	{
		if(w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber) &&
		   w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass))
			tool_cancel();
	
	}
	
	int listen = 0;
	if (page == WINDOW_STAFF_OVERVIEW && w->page == WINDOW_STAFF_OVERVIEW && w->viewport){
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

	w->enabled_widgets = window_staff_page_enabled_widgets[page];
	w->var_020 = RCT2_ADDRESS(0x9929BC, uint32)[page];
	w->event_handlers = window_staff_page_events[page];
	w->pressed_widgets = 0;
	w->widgets = window_staff_page_widgets[page];

	window_staff_disable_widgets(w);
	window_invalidate(w);

	RCT2_CALLPROC_X(w->event_handlers[WE_RESIZE], 0, 0, 0, 0, (int)w, 0, 0);
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);

	window_init_scroll_widgets(w);
	window_invalidate(w);

	if (listen && w->viewport) w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

/** rct2: 0x006BDF55 */
void window_staff_overview_mouseup()
{
	short widgetIndex;
	rct_window* w;
	window_widget_get_registers(w, widgetIndex);
	rct_peep* peep = GET_PEEP(w->number);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
		window_staff_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_LOCATE: // 0xD
		window_scroll_to_viewport(w);
		break;
	case WIDX_PICKUP: // 0xA
		// 0x6BE236
		if (tool_set(w, widgetIndex, 7)) {
			return;
		}

		w->var_48C = peep->x;

		RCT2_CALLPROC_X(0x0069A512, 0, 0, 0, 0, (int)peep, 0, 0);
		RCT2_CALLPROC_X(0x006EC473, 0, 0, 0, 0, (int)peep, 0, 0);

		RCT2_CALLPROC_X(0x0069E9D3, 0x8000, 0, peep->y, peep->z, (int)peep, 0, 0);
		RCT2_CALLPROC_X(0x0069A409, 0, 0, 0, 0, (int)peep, 0, 0);
		peep->state = 9;
		RCT2_CALLPROC_X(0x0069A42F, 0, 0, 0, 0, (int)peep, 0, 0);
		break;
	case WIDX_FIRE: // 0xE
		window_staff_fire_prompt_open(peep);
		break;
	case WIDX_RENAME: // 0xC
		// 6BE4BC
		window_show_textinput(w, (int)widgetIndex, 0xBA1, 0xBA2, peep->name_string_idx);
		break;
	}
}

/** rct2: 0x006BE558 */
void window_staff_overview_resize()
{
	rct_window* w;
	window_get_register(w);

	window_staff_disable_widgets(w);

	w->min_width = 190;
	w->max_width = 500;
	w->min_height = 180;
	w->max_height = 450;

	if (w->width < w->min_width) {
		w->width = w->min_width;
		window_invalidate(w);
	}

	if (w->width > w->max_width) {
		window_invalidate(w);
		w->width = w->max_width;
	}

	if (w->height < w->min_height) {
		w->height = w->min_height;
		window_invalidate(w);
	}

	if (w->height > w->max_height) {
		window_invalidate(w);
		w->height = w->max_height;
	}

	rct_viewport* viewport = w->viewport;

	if (viewport) {
		int new_width = w->width - 30;
		int new_height = w->height - 62;

		// Update the viewport size
		if (viewport->width != new_width || viewport->height != new_height) {
			viewport->width = new_width;
			viewport->height = new_height;
			viewport->view_width = new_width << viewport->zoom;
			viewport->view_height = new_height << viewport->zoom;
		}
	}

	RCT2_CALLPROC_X(0x006BEDA3, 0, 0, 0, 0, (int)w, 0, 0);
}

/** 
 * Handle the dropdown of patrol button.
 * rct2: 0x006BDF98
 */
void window_staff_overview_mousedown(int widgetIndex, rct_window* w, rct_widget* widget)
{
	if (widgetIndex != WIDX_PATROL) {
		return;
	}

	// Dropdown names
	gDropdownItemsFormat[0] = 0xD75;
	gDropdownItemsFormat[1] = 0xD76;

	int x = widget->left + w->x;
	int y = widget->top + w->y;;
	int extray = widget->bottom - widget->top + 1;
	window_dropdown_show_text(x, y, extray, w->colours[1], 0, 2);
	RCT2_GLOBAL(0x009DEBA2, sint16) = 0;

	rct_peep* peep = GET_PEEP(w->number);

	// Disable clear patrol area if no area is set.
	if (!(RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[peep->staff_id] & 2)) {
		RCT2_GLOBAL(0x009DED34, sint32) |= 1 << 1;
	}
}

/** rct2: 0x006BDFA3 */
void window_staff_overview_dropdown()
{
	short widgetIndex, dropdownIndex;
	rct_window* w;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (widgetIndex != WIDX_PATROL) {
		return;
	}

	// Clear patrol
	if (dropdownIndex == 1) {
		rct_peep* peep = GET_PEEP(w->number);
		int edi = peep->staff_id;
		int ebx = edi << 9;

		for (int i = 0; i < 128; i++)
		{
			RCT2_GLOBAL(0x13B0E72 + ebx + i * 4, uint32) = 0;
		}
		RCT2_GLOBAL(RCT2_ADDRESS_STAFF_MODE_ARRAY + edi, uint16) &= 0xFD; // bug??

		window_invalidate(w);
		//RCT2_CALLPROC_EBPSAFE(0x006C0C3F);
		sub_6C0C3F();
	}
	else {
		if (!tool_set(w, widgetIndex, 22)) {
			show_gridlines();
			RCT2_GLOBAL(0x009DEA50, sint16) = w->number;
			window_invalidate(w);
		}
	}
}

/**
 * Update the animation frame of the tab icon.
 * rct2: 0x6BE602 
 */
void window_staff_overview_update(rct_window* w)
{
	int var_496 = RCT2_GLOBAL((int)w + 0x496, uint16);
	var_496++;
	if (var_496 >= 24) {
		var_496 = 0;
	}
	RCT2_GLOBAL((int)w + 0x496, uint16) = var_496;
	window_invalidate_by_id(0x497, w->number);
}

/** rct2: 0x006BE814 */
void window_staff_set_order(rct_window* w, int order_id)
{
	int eax = 1 << order_id;

	rct_peep* peep = GET_PEEP(w->number);

	int ax = peep->var_C6 ^ eax;
	int flags = (ax << 8) | 1;

	game_do_command(peep->x, flags, peep->y, w->number, GAME_COMMAND_SET_STAFF_ORDER, (int)peep, 0);
}

/** rct2: 0x006BE7DB */
void window_staff_orders_mouseup()
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
		window_staff_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_CHECKBOX_1:
	case WIDX_CHECKBOX_2:
	case WIDX_CHECKBOX_3:
	case WIDX_CHECKBOX_4:
		window_staff_set_order(w, widgetIndex - WIDX_CHECKBOX_1);
		break;
	}
}

/** rct2: 0x006BE960 */
void window_staff_orders_update(rct_window* w)
{
	w->frame_no++;
	window_invalidate_by_id(0x597, w->number);
}

/** rct2: 0x006BEBCF */
void window_staff_stats_mouseup()
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
		window_staff_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	}
}

/** rct2: 0x006BEC1B and rct2: 0x006BE975 */
void window_staff_stats_resize()
{
	rct_window* w;
	window_get_register(w);

	w->min_width = 190;
	w->max_width = 190; 
	w->min_height = 119;
	w->max_height = 119;
	
	if (w->width < w->min_width) {
		w->width = w->min_width;
		window_invalidate(w);
	}

	if (w->width > w->max_width) {
		window_invalidate(w);
		w->width = w->max_width;
	}

	if (w->height < w->min_height) {
		w->height = w->min_height;
		window_invalidate(w);
	}

	if (w->height > w->max_height) {
		window_invalidate(w);
		w->height = w->max_height;
	}
}

/** rct2: 0x006BEBEA */
void window_staff_stats_update(rct_window* w)
{
	w->frame_no++;
	window_invalidate_by_id(0x697, w->number);

	rct_peep* peep = GET_PEEP(w->number);
	if (peep->var_45 & 0x10) {
		peep->var_45 &= 0xEF;
		window_invalidate(w);
	}
}

/* rct2: 0x6BEC80, 0x6BE9DA */
void window_staff_unknown_05(){
	rct_window* w;
	window_get_register(w);

	widget_invalidate(WC_PEEP, w->number, WIDX_TAB_1);
}