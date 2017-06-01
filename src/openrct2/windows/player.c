#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../config/Config.h"
#include "../game.h"
#include "../input.h"
#include "../management/marketing.h"
#include "../network/network.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../ride/ride.h"
#include "../ride/ride_data.h"
#include "../scenario/scenario.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
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
	WINDOW_PLAYER_PAGE_OVERVIEW,
	WINDOW_PLAYER_PAGE_STATISTICS,
};

#pragma region Widgets

enum WINDOW_PLAYER_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,

	WIDX_GROUP = 6,
	WIDX_GROUP_DROPDOWN,
	WIDX_LOCATE,
	WIDX_KICK,
	WIDX_VIEWPORT,
};

#define WINDOW_PLAYER_COMMON_WIDGETS																									\
	{ WWT_FRAME,			0,	0,		191,	0,		156,	0xFFFFFFFF,				STR_NONE },				/* Panel / Background	*/		\
	{ WWT_CAPTION,			0,	1,		190,	1,		14,		STR_STRING,				STR_WINDOW_TITLE_TIP },	/* Title				*/		\
	{ WWT_CLOSEBOX,			0,	179,	189,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },	/* Close x button		*/		\
	{ WWT_RESIZE,			1,	0,		191,	43,		156,	0xFFFFFFFF,				STR_NONE },				/* Resize				*/		\
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x20000000 | SPR_TAB,	STR_NONE },				/* Tab 1				*/		\
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x20000000 | SPR_TAB,	STR_NONE }				/* Tab 2				*/

rct_widget window_player_overview_widgets[] = {
	WINDOW_PLAYER_COMMON_WIDGETS,
	{ WWT_DROPDOWN,			1,	3,		177,	46,		57,		0xFFFFFFFF,			STR_NONE },					// Permission group
	{ WWT_DROPDOWN_BUTTON,	1,	167,	177,	47,		56,		STR_DROPDOWN_GLYPH,	STR_NONE },					//
	{ WWT_FLATBTN,			1,	179,	190,	45,		68,		SPR_LOCATE,			STR_LOCATE_PLAYER_TIP },	// Locate button
	{ WWT_FLATBTN,			1,	179,	190,	69,		92,		SPR_DEMOLISH,		STR_KICK_PLAYER_TIP },		// Kick button
	{ WWT_VIEWPORT,			1,	3,		177,	60,		120,	0xFFFFFFFF,			STR_NONE },					// Viewport
	{ WIDGETS_END },
};

rct_widget window_player_statistics_widgets[] = {
	WINDOW_PLAYER_COMMON_WIDGETS,
	{ WIDGETS_END },
};

rct_widget *window_player_page_widgets[] = {
	window_player_overview_widgets,
	window_player_statistics_widgets
};

#pragma endregion

#pragma region Events

void window_player_overview_close(rct_window *w);
void window_player_overview_mouse_up(rct_window *w, rct_widgetindex widgetIndex);
void window_player_overview_resize(rct_window *w);
void window_player_overview_mouse_down(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
void window_player_overview_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
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

void window_player_statistics_close(rct_window *w);
void window_player_statistics_mouse_up(rct_window *w, rct_widgetindex widgetIndex);
void window_player_statistics_resize(rct_window *w);
void window_player_statistics_update(rct_window* w);
void window_player_statistics_invalidate(rct_window *w);
void window_player_statistics_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_player_statistics_events = {
	window_player_statistics_close,
	window_player_statistics_mouse_up,
	window_player_statistics_resize,
	NULL,
	NULL,
	NULL,
	window_player_statistics_update,
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
	window_player_statistics_invalidate,
	window_player_statistics_paint,
	NULL
};

static rct_window_event_list *window_player_page_events[] = {
	&window_player_overview_events,
	&window_player_statistics_events
};

#pragma endregion

static void window_player_set_page(rct_window* w, sint32 page);
static void window_player_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);
static void window_player_update_viewport(rct_window *w, bool scroll);
static void window_player_update_title(rct_window* w);

uint32 window_player_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_GROUP) |
	(1 << WIDX_GROUP_DROPDOWN) |
	(1 << WIDX_LOCATE) |
	(1 << WIDX_KICK),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2)
};

void window_player_open(uint8 id)
{
	rct_window* window;

	window = window_bring_to_front_by_number(WC_PLAYER, id);
	if (window == NULL) {
		window = window_create_auto_pos(240, 170, &window_player_overview_events, WC_PLAYER, WF_RESIZABLE);
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
	}

	window->page = 0;
	window_invalidate(window);

	window->widgets = window_player_page_widgets[WINDOW_PLAYER_PAGE_OVERVIEW];
	window->enabled_widgets = window_player_page_enabled_widgets[WINDOW_PLAYER_PAGE_OVERVIEW];
	window->hold_down_widgets = 0;
	window->event_handlers = window_player_page_events[WINDOW_PLAYER_PAGE_OVERVIEW];
	window->pressed_widgets = 0;

	window_init_scroll_widgets(window);
	window_player_set_page(window, WINDOW_PLAYER_PAGE_OVERVIEW);
}

static void window_player_overview_show_group_dropdown(rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget;
	sint32 numItems, i;
	sint32 player = network_get_player_index((uint8)w->number);
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
		0,
		numItems,
		widget->right - dropdownWidget->left
	);

	for (i = 0; i < network_get_num_groups(); i++) {
		gDropdownItemsFormat[i] = STR_OPTIONS_DROPDOWN_ITEM;
		gDropdownItemsArgs[i] = (uintptr_t)network_get_group_name(i);
	}

	dropdown_set_checked(network_get_group_index(network_get_player_group(player)), true);
}

void window_player_overview_close(rct_window *w)
{

}

void window_player_overview_mouse_up(rct_window *w, rct_widgetindex widgetIndex)
{
	switch(widgetIndex){
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
		window_player_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_LOCATE:{
		rct_window* mainWindow = window_get_main();
		if (mainWindow != NULL) {
			sint32 player = network_get_player_index((uint8)w->number);
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

void window_player_overview_mouse_down(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	switch(widgetIndex){
	case WIDX_GROUP_DROPDOWN:
		window_player_overview_show_group_dropdown(w, widget);
		break;
	}
}

void window_player_overview_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	sint32 player = network_get_player_index((uint8)w->number);
	if (player == -1) {
		return;
	}
	if (dropdownIndex == -1) {
		return;
	}
	sint32 group = network_get_group_id(dropdownIndex);
	game_do_command(0, GAME_COMMAND_FLAG_APPLY, w->number, group, GAME_COMMAND_SET_PLAYER_GROUP, 0, 0);
	window_invalidate(w);
}

void window_player_overview_resize(rct_window *w)
{
	window_set_resize(w, 240, 170, 500, 300);
}

void window_player_overview_update(rct_window* w)
{
	w->frame_no++;
	widget_invalidate(w, WIDX_TAB_1 + w->page);

	if (network_get_player_index((uint8)w->number) == -1) {
		window_close(w);
		return;
	}

	// Update viewport
	bool scroll = true;

	// Use this spare window field for rotation check
	if (w->var_4AE != get_current_rotation()) {
		w->var_4AE = get_current_rotation();
		scroll = false;
	}
	window_player_update_viewport(w, scroll);
}

void window_player_overview_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_player_draw_tab_images(dpi, w);

	sint32 player = network_get_player_index((uint8)w->number);
	if (player == -1) {
		return;
	}

	// Draw current group
	sint32 groupindex = network_get_group_index(network_get_player_group(player));
	if (groupindex != -1) {
		rct_widget* widget = &window_player_overview_widgets[WIDX_GROUP];
		char buffer[300];
		char* lineCh;
		lineCh = buffer;
		lineCh = utf8_write_codepoint(lineCh, FORMAT_WINDOW_COLOUR_2);
		safe_strcpy(lineCh, network_get_group_name(groupindex), sizeof(buffer) - (lineCh - buffer));
		set_format_arg(0, const char *, buffer);

		gfx_draw_string_centred_clipped(
			dpi,
			STR_STRING,
			gCommonFormatArgs,
			COLOUR_BLACK,
			w->x + (widget->left + widget->right - 11) / 2,
			w->y + widget->top,
			widget->right - widget->left - 8
		);
	}

	// Draw ping
	sint32 x = w->x + 90;
	sint32 y = w->y + 24;

	set_format_arg(0, rct_string_id, STR_PING);
	gfx_draw_string_left(dpi, STR_WINDOW_COLOUR_2_STRINGID, gCommonFormatArgs, 0, x, y);
	char ping[64];
	snprintf(ping, 64, "%d ms", network_get_player_ping(player));
	gfx_draw_string(dpi, ping, w->colours[2], x + 30, y);

	// Draw last action
	x = w->x + (w->width / 2);
	y = w->y + w->height - 13;
	sint32 width = w->width - 8;
	sint32 lastaction = network_get_player_last_action(player, 0);
	set_format_arg(0, rct_string_id, STR_ACTION_NA);
	if (lastaction != -999) {
		set_format_arg(0, rct_string_id, network_get_action_name_string_id(lastaction));
	}
	gfx_draw_string_centred_clipped(dpi, STR_LAST_ACTION_RAN, gCommonFormatArgs, COLOUR_BLACK, x, y, width);

	if (w->viewport != NULL && w->var_492 != -1) {
		window_draw_viewport(dpi, w);
	}
}

void window_player_overview_invalidate(rct_window *w)
{
	if (window_player_page_widgets[w->page] != w->widgets) {
		w->widgets = window_player_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets &= ~(WIDX_TAB_1);
	w->pressed_widgets &= ~(WIDX_TAB_2);
	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	window_player_update_title(w);

	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].right =w->width - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 13;
	w->widgets[WIDX_CLOSE].right = w->width - 3;
	w->widgets[WIDX_LOCATE].right = w->width - 2;
	w->widgets[WIDX_LOCATE].left = w->width - 25;
	w->widgets[WIDX_KICK].right = w->width - 2;
	w->widgets[WIDX_KICK].left = w->width - 25;
	w->widgets[WIDX_VIEWPORT].right = w->width - 26;
	w->widgets[WIDX_VIEWPORT].bottom = w->height - 14;

	sint32 groupDropdownWidth = w->widgets[WIDX_GROUP].right - w->widgets[WIDX_GROUP].left;
	w->widgets[WIDX_GROUP].left = (w->width - groupDropdownWidth) / 2;
	w->widgets[WIDX_GROUP].right = w->widgets[WIDX_GROUP].left + groupDropdownWidth;
	w->widgets[WIDX_GROUP_DROPDOWN].left = w->widgets[WIDX_GROUP].right - 10;
	w->widgets[WIDX_GROUP_DROPDOWN].right = w->widgets[WIDX_GROUP].right;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_2);

	rct_viewport *viewport = w->viewport;
	if (viewport != NULL) {
		rct_widget *viewportWidget = &window_player_overview_widgets[WIDX_VIEWPORT];

		viewport->x = w->x + viewportWidget->left;
		viewport->y = w->y + viewportWidget->top;
		viewport->width = viewportWidget->right - viewportWidget->left;
		viewport->height = viewportWidget->bottom - viewportWidget->top;
		viewport->view_width = viewport->width << viewport->zoom;
		viewport->view_height = viewport->height << viewport->zoom;
	}
}

void window_player_statistics_close(rct_window *w)
{
	if (w->error.var_480) {
		user_string_free(w->error.var_480);
		w->error.var_480 = 0;
	}
}

void window_player_statistics_mouse_up(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
		window_player_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	}
}

void window_player_statistics_resize(rct_window *w)
{
	window_set_resize(w, 210, 80, 210, 80);
}

void window_player_statistics_update(rct_window* w)
{
	w->frame_no++;
	widget_invalidate(w, WIDX_TAB_1 + w->page);

	if (network_get_player_index((uint8)w->number) == -1) {
		window_close(w);
	}
}

void window_player_statistics_invalidate(rct_window *w)
{
	if (window_player_page_widgets[w->page] != w->widgets) {
		w->widgets = window_player_page_widgets[w->page];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets &= ~(WIDX_TAB_1);
	w->pressed_widgets &= ~(WIDX_TAB_2);
	w->pressed_widgets |= 1ULL << (w->page + WIDX_TAB_1);

	window_player_update_title(w);

	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 13;
	w->widgets[WIDX_CLOSE].right = w->width - 3;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_2);
}

void window_player_statistics_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_player_draw_tab_images(dpi, w);

	sint32 player = network_get_player_index((uint8)w->number);
	if (player == -1) {
		return;
	}

	sint32 x = w->x + window_player_overview_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	sint32 y = w->y + window_player_overview_widgets[WIDX_PAGE_BACKGROUND].top + 4;

	set_format_arg(0, uint32, network_get_player_commands_ran(player));
	gfx_draw_string_left(dpi, STR_COMMANDS_RAN, gCommonFormatArgs, COLOUR_BLACK, x, y);

	y += 10;

	set_format_arg(0, uint32, network_get_player_money_spent(player));
	gfx_draw_string_left(dpi, STR_MONEY_SPENT, gCommonFormatArgs, COLOUR_BLACK, x, y);
}

static void window_player_set_page(rct_window* w, sint32 page)
{
	sint32 originalPage = w->page;

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

	if (page == WINDOW_PLAYER_PAGE_OVERVIEW) {
		if (w->viewport == NULL) {
			viewport_create(w, w->x, w->y, w->width, w->height, 0, 128 * 32, 128 * 32, 0, 1, -1);
			w->flags |= WF_NO_SCROLLING;
			window_event_invalidate_call(w);
			window_player_update_viewport(w, false);
		} else if (originalPage != page) {
			window_event_invalidate_call(w);
			window_player_update_viewport(w, false);
		}
	} else {
		if (w->viewport != NULL) {
			w->viewport->width = 0;
			w->viewport = NULL;
		}
	}
}

static void window_player_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	rct_widget *widget;
	sint32 x, y, imageId;

	// Tab 1
	if (!widget_is_disabled(w, WIDX_TAB_1)) {
		widget = &w->widgets[WIDX_TAB_1];
		x = widget->left + w->x;
		y = widget->top + w->y;
		imageId = SPR_PEEP_LARGE_FACE_NORMAL;
		gfx_draw_sprite(dpi, imageId, x, y, 0);
	}

	// Tab 2
	if (!widget_is_disabled(w, WIDX_TAB_2)) {
		widget = &w->widgets[WIDX_TAB_2];
		x = widget->left + w->x;
		y = widget->top + w->y;
		imageId = SPR_TAB_FINANCES_SUMMARY_0;

		if (w->page == WINDOW_PLAYER_PAGE_STATISTICS) {
			imageId += (w->frame_no / 2) & 7;
		}

		gfx_draw_sprite(dpi, imageId, x, y, 0);
	}
}

static void window_player_update_viewport(rct_window *w, bool scroll)
{
	sint32 playerIndex = network_get_player_index((uint8)w->number);
	if (playerIndex == -1) {
		return;
	}

	rct_viewport *viewport = w->viewport;
	if (viewport != NULL) {
		rct_xyz16 coord = network_get_player_last_action_coord(playerIndex);
		if (coord.x != 0 || coord.y != 0 || coord.z != 0) {
			sint32 viewX, viewY;
			center_2d_coordinates(coord.x, coord.y, coord.z, &viewX, &viewY, viewport);

			// Don't scroll if the view was originally undefined
			if (w->var_492 == -1) {
				scroll = false;
			}

			if (!scroll || w->saved_view_x != viewX || w->saved_view_y != viewY) {
				w->flags |= WF_SCROLLING_TO_LOCATION;
				w->saved_view_x = viewX;
				w->saved_view_y = viewY;
				if (!scroll) {
					w->viewport->view_x = viewX;
					w->viewport->view_y = viewY;
				}
				widget_invalidate(w, WIDX_VIEWPORT);
			}

			// Draw the viewport
			w->var_492 = 0;
		} else {
			// Don't draw the viewport
			w->var_492 = -1;
		}
	}
}

static void window_player_update_title(rct_window* w)
{
	sint32 player = network_get_player_index((uint8)w->number);
	if (player != -1) {
		set_format_arg(0, const char *, network_get_player_name(player)); // set title caption to player name
	} else {
		set_format_arg(0, const char *, "");
	}
}
