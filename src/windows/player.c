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
#include "../network/network.h"
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
#include "../util/util.h"
#include "../world/footpath.h"
#include "../world/map.h"
#include "../world/sprite.h"
#include "dropdown.h"
#include "error.h"

enum WINDOW_PLAYER_PAGE {
	WINDOW_PLAYER_OVERVIEW
};

enum WINDOW_PLAYER_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_GROUP,
	WIDX_GROUP_DROPDOWN,
	WIDX_LOCATE,
	WIDX_KICK
};

rct_widget window_player_overview_widgets[] = {
	{ WWT_FRAME,			0,	0,		191,	0,		156,	0x0FFFFFFFF,	STR_NONE },				// Panel / Background
	{ WWT_CAPTION,			0,	1,		190,	1,		14,		865,			STR_WINDOW_TITLE_TIP }, // Title
	{ WWT_CLOSEBOX,			0,	179,	189,	2,		13,		824,			STR_CLOSE_WINDOW_TIP }, // Close x button
	{ WWT_RESIZE,			1,	0,		191,	43,		156,	0x0FFFFFFFF,	STR_NONE },				// Resize
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,		STR_NONE },				// Tab 1
	{ WWT_DROPDOWN,			1,	3,		177,	46,		57,		0x0FFFFFFFF,	STR_NONE },				// Permission group
	{ WWT_DROPDOWN_BUTTON,	1,	167,	177,	47,		56,		876,			STR_NONE },				//
	{ WWT_FLATBTN,			1,	179,	190,	45,		68,		SPR_LOCATE,		STR_LOCATE_PLAYER_TIP },// Locate button
	{ WWT_FLATBTN,			1,	179,	190,	69,		92,		SPR_DEMOLISH,	STR_KICK_PLAYER_TIP },	// Kick button
	{ WIDGETS_END },
};

rct_widget *window_player_page_widgets[] = {
	window_player_overview_widgets
};

void window_player_set_page(rct_window* w, int page);

void window_player_overview_close(rct_window *w);
void window_player_overview_mouse_up(rct_window *w, int widgetIndex);
void window_player_overview_resize(rct_window *w);
void window_player_overview_mouse_down(int widgetIndex, rct_window *w, rct_widget *widget);
void window_player_overview_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
void window_player_overview_update(rct_window* w);
void window_player_overview_invalidate(rct_window *w);
void window_player_overview_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_player_overview_events = {
	window_player_overview_close,
	window_player_overview_mouse_up,
	window_player_overview_resize,
	window_player_overview_mouse_down,
	window_player_overview_dropdown,
	NULL,
	window_player_overview_update,
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
	window_player_overview_invalidate,
	window_player_overview_paint,
	NULL
};

static rct_window_event_list *window_player_page_events[] = {
	&window_player_overview_events
};

uint32 window_player_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_GROUP) |
	(1 << WIDX_GROUP_DROPDOWN) |
	(1 << WIDX_LOCATE) |
	(1 << WIDX_KICK)
};

void window_player_open(uint8 id){
	rct_window* window;

	int player = network_get_player_index(id);
	window = window_bring_to_front_by_number(WC_PLAYER, id);
	if (window == NULL){
		window = window_create_auto_pos(210, 134, &window_player_overview_events, WC_PLAYER, WF_RESIZABLE);
		window->widgets = window_player_overview_widgets;
		window->enabled_widgets = window_player_page_enabled_widgets[0];
		window->number = id;
		window->page = 0;
		window->viewport_focus_coordinates.y = 0;
		window->frame_no = 0;
		window->list_information_type = 0;
		window->picked_peep_frame = 0;
		window->highlighted_item = 0;
		window->min_width = 210;
		window->min_height = 134;
		window->max_width = 500;
		window->max_height = 450;
		window->no_list_items = 0;
		window->selected_list_item = -1;

		window->viewport_focus_coordinates.y = -1;
		window->error.var_480 = user_string_allocate(128, network_get_player_name(player)); // repurposing var_480 to store this
	}

	window->page = 0;
	window_invalidate(window);

	window->widgets = window_player_page_widgets[WINDOW_PLAYER_OVERVIEW];
	window->enabled_widgets = window_player_page_enabled_widgets[WINDOW_PLAYER_OVERVIEW];
	window->hold_down_widgets = 0;
	window->event_handlers = window_player_page_events[WINDOW_PLAYER_OVERVIEW];
	window->pressed_widgets = 0;

	window_init_scroll_widgets(window);
}

void window_player_overview_show_group_dropdown(rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget;
	int numItems, i;
	int player = network_get_player_index((uint8)w->number);
	if (player == -1) {
		return;
	}

	dropdownWidget = widget - 1;

	numItems = network_get_num_groups();

	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		0,
		numItems,
		widget->right - dropdownWidget->left
	);

	for (i = 0; i < network_get_num_groups(); i++) {
		gDropdownItemsFormat[i] = 1142;
		gDropdownItemsArgs[i] = network_get_group_name_string_id(i);
	}

	dropdown_set_checked(network_get_group_index(network_get_player_group(player)), true);
}

void window_player_overview_close(rct_window *w)
{
	if (w->error.var_480){
		user_string_free(w->error.var_480);
		w->error.var_480 = 0;
	}
}

void window_player_overview_mouse_up(rct_window *w, int widgetIndex)
{
	switch(widgetIndex){
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
		window_player_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_LOCATE:{
		rct_window* mainWindow = window_get_main();
		if (mainWindow != NULL) {
			int player = network_get_player_index((uint8)w->number);
			if (player == -1) {
				return;
			}
			rct_xyz16 coord = network_get_player_last_action_coord(player);
			if (coord.x || coord.y || coord.z) {
				window_scroll_to_location(mainWindow, coord.x, coord.y, coord.z);
			}
		}
	}break;
	case WIDX_KICK:
		game_do_command(w->number, GAME_COMMAND_FLAG_APPLY, 0, 0, GAME_COMMAND_KICK_PLAYER, 0, 0);
		break;
	}
}

void window_player_overview_mouse_down(int widgetIndex, rct_window *w, rct_widget *widget)
{
	switch(widgetIndex){
	case WIDX_GROUP_DROPDOWN:
		window_player_overview_show_group_dropdown(w, widget);
		break;
	}
}

void window_player_overview_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	int player = network_get_player_index((uint8)w->number);
	if (player == -1) {
		return;
	}
	if (dropdownIndex == -1) {
		return;
	}
	int group = network_get_group_id(dropdownIndex);
	game_do_command(0, GAME_COMMAND_FLAG_APPLY, w->number, group, GAME_COMMAND_SET_PLAYER_GROUP, 0, 0);
	window_invalidate(w);
}

void window_player_overview_resize(rct_window *w){
	window_set_resize(w, 210, 134, 210, 134);
}

void window_player_overview_update(rct_window* w){
	int var_496 = w->highlighted_item >> 16;
	var_496++;
	var_496 %= 24;
	w->highlighted_item &= 0x0000FFFF;
	w->highlighted_item |= var_496 << 16;

	widget_invalidate(w, WIDX_TAB_1);

	w->list_information_type += 2;

	if ((w->highlighted_item & 0xFFFF) == 0xFFFF)
		w->highlighted_item &= 0xFFFF0000;
	else
		w->highlighted_item++;

	if(network_get_player_index((uint8)w->number) == -1) {
		window_close(w);
	}
}

void window_player_set_page(rct_window* w, int page){
	w->page = page;
	w->frame_no = 0;
	w->no_list_items = 0;
	w->selected_list_item = -1;

	w->enabled_widgets = window_player_page_enabled_widgets[page];
	w->hold_down_widgets = 0;
	w->event_handlers = window_player_page_events[page];
	w->pressed_widgets = 0;
	w->widgets = window_player_page_widgets[page];
	window_invalidate(w);
	window_event_resize_call(w);
	window_event_invalidate_call(w);
	window_init_scroll_widgets(w);
	window_invalidate(w);
}

void window_player_overview_tab_paint(rct_window* w, rct_drawpixelinfo* dpi){
	if (w->disabled_widgets & (1<<WIDX_TAB_1))
		return;

	rct_widget* widget = &w->widgets[WIDX_TAB_1];
	int x = widget->left + w->x;
	int y = widget->top + w->y;

	int image_id = SPR_PEEP_LARGE_FACE_NORMAL;
	gfx_draw_sprite(dpi, image_id, x, y, 0);
}

void window_player_overview_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_player_overview_tab_paint(w, dpi);

	int player = network_get_player_index((uint8)w->number);
	if (player == -1) {
		return;
	}

	int groupindex = network_get_group_index(network_get_player_group(player));
	if (groupindex != -1) {
		rct_widget* widget = &window_player_overview_widgets[WIDX_GROUP];
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = network_get_group_name_string_id(groupindex);

		gfx_draw_string_centred(
			dpi,
			1193,
			w->x + (widget->left + widget->right - 11) / 2,
			w->y + widget->top,
			0,
			(void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS
		);
	}

	int x = w->x + window_player_overview_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	int y = w->y + window_player_overview_widgets[WIDX_PAGE_BACKGROUND].top + 4 + 13;

	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = STR_PING;
	gfx_draw_string_left(dpi, STR_WINDOW_COLOUR_2_STRING, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, 0, x, y);
	char ping[64];
	sprintf(ping, "%d ms", network_get_player_ping(player));
	gfx_draw_string(dpi, ping, w->colours[2], x + 60, y);
	
	y += 20;
	gfx_fill_rect_inset(dpi, x, y - 6, x + 177, y - 5, w->colours[1], 32);

	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint32) = network_get_player_commands_ran(player);
	gfx_draw_string_left(dpi, STR_COMMANDS_RAN, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, 0,x, y);

	y += 10;
	
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint32) = network_get_player_money_spent(player);
	gfx_draw_string_left(dpi, STR_MONEY_SPENT, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, 0,x, y);

	y += 10;
	
	int lastaction = network_get_player_last_action(player, 0);
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = STR_ACTION_NA;
	if (lastaction != -999) {
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = network_get_action_name_string_id(lastaction);
	}
	gfx_draw_string_left(dpi, STR_LAST_ACTION_RAN, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, 0,x, y);
}

void window_player_overview_invalidate(rct_window *w)
{
	if (window_player_page_widgets[w->page] != w->widgets) {
		w->widgets = window_player_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	colour_scheme_update(w);

	w->pressed_widgets &= ~(WIDX_TAB_1);
	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = w->error.var_480; // set title caption to player name

	window_player_overview_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_player_overview_widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	window_player_overview_widgets[WIDX_PAGE_BACKGROUND].right =w->width - 1;
	window_player_overview_widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;

	window_player_overview_widgets[WIDX_TITLE].right = w->width - 2;

	window_player_overview_widgets[WIDX_CLOSE].left = w->width - 13;
	window_player_overview_widgets[WIDX_CLOSE].right = w->width - 3;

	window_player_overview_widgets[WIDX_LOCATE].right = w->width - 2;
	window_player_overview_widgets[WIDX_LOCATE].left = w->width - 25;

	window_player_overview_widgets[WIDX_KICK].right = w->width - 2;
	window_player_overview_widgets[WIDX_KICK].left = w->width - 25;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_1);
}

