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

#include "addresses.h"
#include "editor.h"
#include "game.h"
#include "string_ids.h"
#include "sprites.h"
#include "tutorial.h"
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"

enum {
	WIDX_START_NEW_GAME,
	WIDX_CONTINUE_SAVED_GAME,
	WIDX_SHOW_TUTORIAL,
	WIDX_GAME_TOOLS,
};

static rct_widget window_title_menu_widgets[] = {
	{ WWT_IMGBTN, 2, 0, 81, 0, 81, SPR_MENU_NEW_GAME, STR_START_NEW_GAME_TIP },
	{ WWT_IMGBTN, 2, 82, 163, 0, 81, SPR_MENU_LOAD_GAME, STR_CONTINUE_SAVED_GAME_TIP },
	{ WWT_IMGBTN, 2, 164, 245, 0, 81, SPR_MENU_TUTORIAL, STR_SHOW_TUTORIAL_TIP },
	{ WWT_IMGBTN, 2, 246, 327, 0, 81, SPR_MENU_TOOLBOX, STR_GAME_TOOLS },
	{ WIDGETS_END },
};

static void window_title_menu_emptysub() { }
static void window_title_menu_mouseup();
static void window_title_menu_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_title_menu_dropdown();
static void window_title_menu_unknown17();
static void window_title_menu_paint();

static void* window_title_menu_events[] = {
	window_title_menu_emptysub,
	window_title_menu_mouseup,
	window_title_menu_emptysub,
	window_title_menu_mousedown,
	window_title_menu_dropdown,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_unknown17,
	window_title_menu_emptysub,
	window_title_menu_emptysub,
	window_title_menu_paint,
	window_title_menu_emptysub
};

/**
 * Creates the window containing the menu buttons on the title screen.
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
void window_title_menu_open()
{
	rct_window* window;

	window = window_create(
		(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - 328) / 2, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16) - 142,
		328, 82,
		(uint32*)window_title_menu_events,
		WC_TITLE_MENU,
		WF_STICK_TO_FRONT
	);
	window->widgets = window_title_menu_widgets;
	window->enabled_widgets |= (8 | 4 | 2 | 1);
	window_init_scroll_widgets(window);
	window->flags |= 16;
	window->colours[0] = 140;
	window->colours[1] = 140;
	window->colours[2] = 140;
}

static void window_title_menu_mouseup()
{
	short widgetIndex;
	rct_window* w;

	window_widget_get_registers(w, widgetIndex);

	if (widgetIndex == WIDX_START_NEW_GAME) {
		window_scenarioselect_open();
	} else if (widgetIndex == WIDX_CONTINUE_SAVED_GAME) {
		game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 0, 0);
	}
}

static void window_title_menu_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	if (widgetIndex == WIDX_SHOW_TUTORIAL) {
		gDropdownItemsFormat[0] = STR_TUTORIAL_BEGINNERS;
		gDropdownItemsFormat[1] = STR_TUTORIAL_CUSTOM_RIDES;
		gDropdownItemsFormat[2] = STR_TUTORIAL_ROLLER_COASTER;
		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[0] | 0x80,
			0x80,
			3
		);
	} else if (widgetIndex == WIDX_GAME_TOOLS) {
		gDropdownItemsFormat[0] = STR_SCENARIO_EDITOR;
		gDropdownItemsFormat[1] = STR_CONVERT_SAVED_GAME_TO_SCENARIO;
		gDropdownItemsFormat[2] = STR_ROLLER_COASTER_DESIGNER;
		gDropdownItemsFormat[3] = STR_TRACK_DESIGNS_MANAGER;
		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[0] | 0x80,
			0x80,
			4
		);
	}
}

static void window_title_menu_dropdown()
{
	short widgetIndex, dropdownIndex;
	rct_window *w;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (widgetIndex == WIDX_SHOW_TUTORIAL) {
		tutorial_start(dropdownIndex);
	} else if (widgetIndex == WIDX_GAME_TOOLS) {
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

static void window_title_menu_unknown17()
{
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, sint16) = 2000;
}

static void window_title_menu_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
}
