/*****************************************************************************
 * Copyright (c) 2014 Ted John
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
#include "../editor.h"
#include "../game.h"
#include "../input.h"
#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "dropdown.h"

enum {
	WIDX_START_NEW_GAME,
	WIDX_CONTINUE_SAVED_GAME,
	WIDX_MULTIPLAYER,
	WIDX_SHOW_TUTORIAL,
	WIDX_GAME_TOOLS
};

static rct_widget window_title_menu_widgets[] = {
	{ WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_MENU_NEW_GAME,		STR_START_NEW_GAME_TIP			},
	{ WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_MENU_LOAD_GAME,		STR_CONTINUE_SAVED_GAME_TIP		},
	{ WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_G2_MENU_MULTIPLAYER,	STR_SHOW_MULTIPLAYER_TIP		},
	{ WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_MENU_TUTORIAL,		STR_SHOW_TUTORIAL_TIP			},
	{ WWT_IMGBTN, 2, 0, 0, 0, 81, SPR_MENU_TOOLBOX,			STR_GAME_TOOLS					},
	{ WIDGETS_END },
};

static void window_title_menu_mouseup(rct_window *w, int widgetIndex);
static void window_title_menu_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_title_menu_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_title_menu_cursor(rct_window *w, int widgetIndex, int x, int y, int *cursorId);
static void window_title_menu_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_title_menu_invalidate(rct_window *w);

static rct_window_event_list window_title_menu_events = {
	NULL,
	window_title_menu_mouseup,
	NULL,
	window_title_menu_mousedown,
	window_title_menu_dropdown,
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
	window_title_menu_cursor,
	NULL,
	window_title_menu_invalidate,
	window_title_menu_paint,
	NULL
};

/**
 * Creates the window containing the menu buttons on the title screen.
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
void window_title_menu_open()
{
	rct_window* window;

	window = window_create(
		0, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 142,
		0, 100,
		&window_title_menu_events,
		WC_TITLE_MENU,
		WF_STICK_TO_BACK | WF_TRANSPARENT | WF_NO_BACKGROUND
	);
	window->widgets = window_title_menu_widgets;
	window->enabled_widgets = (
		(1 << WIDX_START_NEW_GAME) |
		(1 << WIDX_CONTINUE_SAVED_GAME) |
#ifndef DISABLE_NETWORK
		(1 << WIDX_MULTIPLAYER) |
#endif
		// Disable tutorial
		// (1 << WIDX_SHOW_TUTORIAL) |
		(1 << WIDX_GAME_TOOLS)
	);

	int i = 0;
	int x = 0;
	for (rct_widget *widget = window->widgets; widget->type != WWT_LAST; widget++) {
		if (widget_is_enabled(window, i)) {
			widget->left = x;
			widget->right = x + 81;

			x += 82;
		} else {
			widget->type = WWT_EMPTY;
		}
		i++;
	}
	window->width = x;
	window->x = (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) - window->width) / 2;

	window_init_scroll_widgets(window);
}

static void window_title_menu_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_START_NEW_GAME:
		window_scenarioselect_open();
		break;
	case WIDX_CONTINUE_SAVED_GAME:
		game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 0, 0);
		break;
	case WIDX_MULTIPLAYER:
		window_server_list_open();
		break;
	}
}

static void window_title_menu_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	if (widgetIndex == WIDX_GAME_TOOLS) {
		gDropdownItemsFormat[0] = STR_SCENARIO_EDITOR;
		gDropdownItemsFormat[1] = STR_CONVERT_SAVED_GAME_TO_SCENARIO;
		gDropdownItemsFormat[2] = STR_ROLLER_COASTER_DESIGNER;
		gDropdownItemsFormat[3] = STR_TRACK_DESIGNS_MANAGER;
		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[0] | 0x80,
			DROPDOWN_FLAG_STAY_OPEN,
			4
		);
	}
}

static void window_title_menu_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	if (widgetIndex == WIDX_GAME_TOOLS) {
		switch (dropdownIndex) {
		case 0:
			editor_load();
			break;
		case 1:
			editor_convert_save_to_scenario();
			break;
		case 2:
			trackdesigner_load();
			break;
		case 3:
			trackmanager_load();
			break;
		}
	}
}

static void window_title_menu_cursor(rct_window *w, int widgetIndex, int x, int y, int *cursorId)
{
	gTooltipTimeout = 2000;
}

static void window_title_menu_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	gfx_fill_rect(dpi, w->x, w->y, w->x + w->width - 1, w->y + 82 - 1, 0x2000000 | 51);
	window_draw_widgets(w, dpi);
}

static void window_title_menu_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}
