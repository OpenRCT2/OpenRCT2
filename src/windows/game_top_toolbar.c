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

#include <string.h>
#include "../addresses.h"
#include "../game.h"
#include "../input.h"
#include "../sprites.h"
#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "dropdown.h"
#include "../interface/viewport.h"
#include "scenery.h"
#include "../audio/audio.h"

enum {
	WIDX_PAUSE,
	WIDX_FILE_MENU,
	WIDX_ZOOM_OUT,
	WIDX_ZOOM_IN,
	WIDX_ROTATE,
	WIDX_VIEW_MENU,
	WIDX_MAP,

	WIDX_LAND,
	WIDX_WATER,
	WIDX_SCENERY,
	WIDX_PATH,
	WIDX_CONSTRUCT_RIDE,
	WIDX_RIDES,
	WIDX_PARK,
	WIDX_STAFF,
	WIDX_GUESTS,
	WIDX_CLEAR_SCENERY,

	WIDX_FASTFORWARD,
	WIDX_RESEARCH
};

typedef enum {
	DDIDX_LOAD_GAME = 0,
	DDIDX_SAVE_GAME = 1,
	DDIDX_ABOUT = 3,
	DDIDX_OPTIONS = 4,
	DDIDX_SCREENSHOT = 5,
	DDIDX_QUIT_GAME = 7,
} FILE_MENU_DDIDX;

typedef enum {
	DDIDX_UNDERGROUND_INSIDE = 0,
	DDIDX_HIDE_BASE = 1,
	DDIDX_HIDE_VERTICAL = 2,
	DDIDX_SEETHROUGH_RIDES = 4,
	DDIDX_SEETHROUGH_SCENARY = 5,
	DDIDX_INVISIBLE_SUPPORTS = 6,
	DDIDX_INVISIBLE_PEEPS = 7,
	DDIDX_LAND_HEIGHTS = 9,
	DDIDX_TRACK_HEIGHTS = 10,
	DDIDX_PATH_HEIGHTS = 11,
} VIEW_MENU_DDIDX;

static rct_widget window_game_top_toolbar_widgets[] = {
	{ WWT_TRNBTN,	0,	0x0000,			0x001D,			0,		27,		0x20000000 | SPR_TOOLBAR_PAUSE,				STR_PAUSE_GAME_TIP },				// Pause
	{ WWT_TRNBTN,	0,	0x001E + 30,	0x003B + 30,	0,		27,		0x20000000 | SPR_TOOLBAR_FILE,				STR_DISC_AND_GAME_OPTIONS_TIP },	// File menu
	{ WWT_TRNBTN,	1,	0x0046 + 30,	0x0063 + 30,	0,		27,		0x20000000 | SPR_TOOLBAR_ZOOM_OUT,			STR_ZOOM_OUT_TIP },					// Zoom out
	{ WWT_TRNBTN,	1,	0x0064 + 30,	0x0081 + 30,	0,		27,		0x20000000 | SPR_TOOLBAR_ZOOM_IN,			STR_ZOOM_IN_TIP },					// Zoom in
	{ WWT_TRNBTN,	1,	0x0082 + 30,	0x009F + 30,	0,		27,		0x20000000 | SPR_TOOLBAR_ROTATE,			STR_ROTATE_TIP },					// Rotate camera
	{ WWT_TRNBTN,	1,	0x00A0 + 30,	0x00BD + 30,	0,		27,		0x20000000 | SPR_TOOLBAR_VIEW,				STR_VIEW_OPTIONS_TIP },				// Transparancy menu
	{ WWT_TRNBTN,	1,	0x00BE + 30,	0x00DB + 30,	0,		27,		0x20000000 | SPR_TOOLBAR_MAP,				STR_SHOW_MAP_TIP },					// Map

	{ WWT_TRNBTN,	2,	0x010B,	0x0128,	0,						27,		0x20000000 | SPR_TOOLBAR_LAND,				STR_ADJUST_LAND_TIP },				// Land
	{ WWT_TRNBTN,	2,	0x0129,	0x0146,	0,						27,		0x20000000 | SPR_TOOLBAR_WATER,				STR_ADJUST_WATER_TIP },				// Water
	{ WWT_TRNBTN,	2,	0x0147,	0x0164,	0,						27,		0x20000000 | SPR_TOOLBAR_SCENERY,			STR_PLACE_SCENERY_TIP },			// Scenery
	{ WWT_TRNBTN,	2,	0x0165,	0x0182,	0,						27,		0x20000000 | SPR_TOOLBAR_FOOTPATH,			STR_BUILD_FOOTPATH_TIP },			// Path
	{ WWT_TRNBTN,	2,	0x0183,	0x01A0,	0,						27,		0x20000000 | SPR_TOOLBAR_CONSTRUCT_RIDE,	STR_BUILD_RIDE_TIP },				// Construct ride
	{ WWT_TRNBTN,	3,	0x01EA,	0x0207,	0,						27,		0x20000000 | SPR_TOOLBAR_RIDES,				STR_RIDES_IN_PARK_TIP },			// Rides
	{ WWT_TRNBTN,	3,	0x0208,	0x0225,	0,						27,		0x20000000 | SPR_TOOLBAR_PARK,				STR_PARK_INFORMATION_TIP },			// Park
	{ WWT_TRNBTN,	3,	0x0226,	0x0243,	0,						27,		0x20000000 | 0x15F9,						STR_STAFF_TIP },					// Staff
	{ WWT_TRNBTN,	3,	0x0230,	0x024D,	0,						27,		0x20000000 | SPR_TOOLBAR_GUESTS,			STR_GUESTS_TIP },					// Guests
	{ WWT_TRNBTN,	2,	0x0230,	0x024D,	0,						27,		0x20000000 | SPR_TOOLBAR_CLEAR_SCENERY,		STR_CLEAR_SCENERY_TIP },			// Clear scenery

	{ WWT_TRNBTN,	0,	0x001E,	0x003B,	0,						27,		0x20000000 | 0x15F9,						STR_NONE },							// Fast forward
	{ WWT_TRNBTN,	3,	0x001E,	0x003B,	0,						27,		0x20000000 | 0x15F9,						2275 },								// Research
	{ WIDGETS_END },
};

static void window_game_top_toolbar_emptysub() { }
static void window_game_top_toolbar_mouseup();
static void window_game_top_toolbar_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_game_top_toolbar_dropdown();
static void window_game_top_toolbar_invalidate();
static void window_game_top_toolbar_paint();
static void window_game_top_toolbar_tool_down();

static void* window_game_top_toolbar_events[] = {
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_mouseup,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_mousedown,
	window_game_top_toolbar_dropdown,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	(void*)0x0066CB25, //Update
	window_game_top_toolbar_tool_down,
	(void*)0x0066CB4E,
	(void*)0x0066CC5B,
	(void*)0x0066CA58,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_emptysub,
	window_game_top_toolbar_invalidate,
	window_game_top_toolbar_paint,
	window_game_top_toolbar_emptysub
};

/**
 * Creates the main game top toolbar window.
 *  rct2: 0x0066B485 (part of 0x0066B3E8)
 */
void window_game_top_toolbar_open()
{
	rct_window* window;

	window = window_create(
		0, 0,
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16), 28,
		(uint32*)window_game_top_toolbar_events,
		WC_TOP_TOOLBAR,
		WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_5
	);
	window->widgets = window_game_top_toolbar_widgets;

	window->enabled_widgets |= 
		(1 << WIDX_PAUSE) | 
		(1 << WIDX_FILE_MENU) |
		(1 << WIDX_ZOOM_OUT) | 
		(1 << WIDX_ZOOM_IN) |
		(1 << WIDX_ROTATE) | 
		(1 << WIDX_VIEW_MENU) |
		(1 << WIDX_MAP) | 
		(1 << WIDX_LAND) |
		(1 << WIDX_WATER) | 
		(1 << WIDX_SCENERY) |
		(1 << WIDX_PATH) | 
		(1 << WIDX_CONSTRUCT_RIDE) |
		(1 << WIDX_RIDES) | 
		(1 << WIDX_PARK) |
		(1 << WIDX_STAFF) | 
		(1 << WIDX_GUESTS) |
		(1 << WIDX_CLEAR_SCENERY) |
		(1ULL << WIDX_FASTFORWARD) | 
		(1ULL << WIDX_RESEARCH);

	window_init_scroll_widgets(window);
	window->colours[0] = 7;
	window->colours[1] = 12;
	window->colours[2] = 24;
	window->colours[3] = 1;
}

/**
 * 
 *  rct2: 0x0066C957
 */
static void window_game_top_toolbar_mouseup()
{
	short widgetIndex;
	rct_window *w, *mainWindow;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_PAUSE:
		game_do_command(0, 1, 0, 0, GAME_COMMAND_TOGGLE_PAUSE, 0, 0);
		// Not sure where this was done in the original code
		w->pressed_widgets ^= (1 << WIDX_PAUSE);
		break;
	case WIDX_FASTFORWARD:
		// This is an excellent place to add in debugging statements and
		// print routines, that will be triggered when you press the
		// button in the game. Use "git update-index --skip-worktree
		// src/window_game_top_toolbar" to avoid committing these changes to
		// version control.
		window_cheats_open();
		break;

	case WIDX_ZOOM_OUT:
		if ((mainWindow = window_get_main()) != NULL)
			window_zoom_out(mainWindow);
		break;
	case WIDX_ZOOM_IN:
		if ((mainWindow = window_get_main()) != NULL)
			window_zoom_in(mainWindow);
		break;
	case WIDX_ROTATE:
		if ((mainWindow = window_get_main()) != NULL)
			window_rotate_camera(mainWindow);
		break;
	case WIDX_CLEAR_SCENERY:
		if ((RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == 1 && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) == 16) {
			tool_cancel();
		} else {
			show_gridlines();
			tool_set(w, WIDX_CLEAR_SCENERY, 12);
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 2;
			window_clear_scenery_open();
		}
		break;
	case WIDX_LAND:
		if ((RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == 1 && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) == 7) {
			tool_cancel();
		} else {
			show_gridlines();
			tool_set(w, WIDX_LAND, 18);
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;
			window_land_open();
		}
		break;
	case WIDX_WATER:
		if ((RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == 1 && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) == 8) {
			tool_cancel();
		} else {
			show_gridlines();
			tool_set(w, WIDX_WATER, 19);
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;
			window_water_open();
		}
		break;
	case WIDX_SCENERY:
		if (!tool_set(w, WIDX_SCENERY, 0)) {
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
			window_scenery_open();
		}
		break;
	case WIDX_PATH:
		if (window_find_by_class(WC_FOOTPATH) == NULL) {
			window_footpath_open();
		} else {
			tool_cancel();
			window_close_by_class(WC_FOOTPATH);
		}
		break;
	case WIDX_CONSTRUCT_RIDE:
		window_new_ride_open();
		break;
	case WIDX_RIDES:
		window_ride_list_open();
		break;
	case WIDX_PARK:
		window_park_entrance_open();
		break;
	case WIDX_STAFF:
		window_staff_list_open();
		break;
	case WIDX_GUESTS:
		window_guest_list_open();
		break;
	case WIDX_RESEARCH:
		window_research_open();
		break;
	}
}

/**
 * 
 *  rct2: 0x0066CA3B
 */
static void window_game_top_toolbar_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	rct_viewport *mainViewport;

	switch (widgetIndex) {
	case WIDX_FILE_MENU:
		gDropdownItemsFormat[0] = 882;
		gDropdownItemsFormat[1] = 883;
		gDropdownItemsFormat[2] = 0;
		gDropdownItemsFormat[3] = 847;
		gDropdownItemsFormat[4] = 2327;
		gDropdownItemsFormat[5] = 891;
		gDropdownItemsFormat[6] = 0;
		gDropdownItemsFormat[7] = 886;
		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[0] | 0x80,
			0x80,
			8
		);
		break;
	case WIDX_VIEW_MENU:
		gDropdownItemsFormat[0] = 1156;
		gDropdownItemsFormat[1] = 1156;
		gDropdownItemsFormat[2] = 1156;
		gDropdownItemsFormat[3] = 0;
		gDropdownItemsFormat[4] = 1156;
		gDropdownItemsFormat[5] = 1156;
		gDropdownItemsFormat[6] = 1156;
		gDropdownItemsFormat[7] = 1156;
		gDropdownItemsFormat[8] = 0;
		gDropdownItemsFormat[9] = 1156;
		gDropdownItemsFormat[10] = 1156;
		gDropdownItemsFormat[11] = 1156;

		gDropdownItemsArgs[0] = 939;
		gDropdownItemsArgs[1] = 940;
		gDropdownItemsArgs[2] = 941;
		gDropdownItemsArgs[4] = 942;
		gDropdownItemsArgs[5] = 943;
		gDropdownItemsArgs[6] = 1051;
		gDropdownItemsArgs[7] = 1052;
		gDropdownItemsArgs[9] = 1154;
		gDropdownItemsArgs[10] = 1153;
		gDropdownItemsArgs[11] = 1155;

		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[1] | 0x80,
			0,
			12
		);

		// Set checkmarks
		mainViewport = window_get_main()->viewport;
		if (mainViewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE)
			gDropdownItemsChecked |= (1 << 0);
		if (mainViewport->flags & VIEWPORT_FLAG_HIDE_BASE)
			gDropdownItemsChecked |= (1 << 1);
		if (mainViewport->flags & VIEWPORT_FLAG_HIDE_VERTICAL)
			gDropdownItemsChecked |= (1 << 2);
		if (mainViewport->flags & VIEWPORT_FLAG_SEETHROUGH_RIDES)
			gDropdownItemsChecked |= (1 << 4);
		if (mainViewport->flags & VIEWPORT_FLAG_SEETHROUGH_SCENERY)
			gDropdownItemsChecked |= (1 << 5);
		if (mainViewport->flags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
			gDropdownItemsChecked |= (1 << 6);
		if (mainViewport->flags & VIEWPORT_FLAG_INVISIBLE_PEEPS)
			gDropdownItemsChecked |= (1 << 7);
		if (mainViewport->flags & VIEWPORT_FLAG_LAND_HEIGHTS)
			gDropdownItemsChecked |= (1 << 9);
		if (mainViewport->flags & VIEWPORT_FLAG_TRACK_HEIGHTS)
			gDropdownItemsChecked |= (1 << 10);
		if (mainViewport->flags & VIEWPORT_FLAG_PATH_HEIGHTS)
			gDropdownItemsChecked |= (1 << 11);

		RCT2_GLOBAL(0x9DEBA2, uint16) = 0;
		break;
	case WIDX_MAP:
		gDropdownItemsFormat[0] = 2523;
		gDropdownItemsFormat[1] = 2780;
		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[1] | 0x80,
			0,
			2
		);
		RCT2_GLOBAL(0x9DEBA2, uint16) = 0;
		break;
	}
}

/**
 * 
 *  rct2: 0x0066C9EA
 */
static void window_game_top_toolbar_dropdown()
{
	short widgetIndex, dropdownIndex;
	rct_window* w, *mainWindow;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	switch (widgetIndex) {
	case WIDX_FILE_MENU:
		switch (dropdownIndex) {
		case DDIDX_LOAD_GAME:		// load game
			game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 0, 0);
			break;
		case DDIDX_SAVE_GAME:		// save game
			tool_cancel();
			save_game();
			break;
		case DDIDX_ABOUT:		// about
			window_about_open();
			break;
		case DDIDX_OPTIONS:		// options
			window_options_open();
			break;
		case DDIDX_SCREENSHOT:		// screenshot
			RCT2_GLOBAL(RCT2_ADDRESS_SCREENSHOT_COUNTDOWN, sint8) = 10;
			break;
		case DDIDX_QUIT_GAME:		// quit game
			game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 1, 0);
			break;
		}
		break;
	case WIDX_VIEW_MENU:
		if (dropdownIndex == -1) dropdownIndex = RCT2_GLOBAL(0x9DEBA2, uint16);
		mainWindow = window_get_main();
		if (mainWindow) {
			switch (dropdownIndex){
			case DDIDX_UNDERGROUND_INSIDE:
				mainWindow->viewport->flags ^= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
				break;
			case DDIDX_HIDE_BASE:
				mainWindow->viewport->flags ^= VIEWPORT_FLAG_HIDE_BASE;
				break;
			case DDIDX_HIDE_VERTICAL:
				mainWindow->viewport->flags ^= VIEWPORT_FLAG_HIDE_VERTICAL;
				break;
			case DDIDX_SEETHROUGH_RIDES:
				mainWindow->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_RIDES;
				break;
			case DDIDX_SEETHROUGH_SCENARY:
				mainWindow->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_SCENERY;
				break;
			case DDIDX_INVISIBLE_SUPPORTS:
				mainWindow->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_SUPPORTS;
				break;
			case DDIDX_INVISIBLE_PEEPS:
				mainWindow->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_PEEPS;
				break;
			case DDIDX_LAND_HEIGHTS:
				mainWindow->viewport->flags ^= VIEWPORT_FLAG_LAND_HEIGHTS;
				break;
			case DDIDX_TRACK_HEIGHTS:
				mainWindow->viewport->flags ^= VIEWPORT_FLAG_TRACK_HEIGHTS;
				break;
			case DDIDX_PATH_HEIGHTS:
				mainWindow->viewport->flags ^= VIEWPORT_FLAG_PATH_HEIGHTS;
				break;
			default:
				return;
			}
			window_invalidate(mainWindow);
		}
		break;
	case WIDX_MAP:
		if (dropdownIndex == -1)
			dropdownIndex = RCT2_GLOBAL(0x9DEBA2, uint16);
		switch (dropdownIndex) {
		case 0:
			window_map_open();
			break;
		case 1:
			window_viewport_open();
			break;
		}
		break;
	}
}

/**
 * 
 *  rct2: 0x0066C810
 */
static void window_game_top_toolbar_invalidate()
{
	int x;
	rct_window *w;

	window_get_register(w);

	// Anchor the right half of the buttons to the right
	x = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16);
	if (x < 640)
		x = 640;
	x--;
	window_game_top_toolbar_widgets[WIDX_GUESTS].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_GUESTS].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_STAFF].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_STAFF].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_PARK].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_PARK].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_RIDES].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_RIDES].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_RESEARCH].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_RESEARCH].left = x;
	x -= 11;
	window_game_top_toolbar_widgets[WIDX_CONSTRUCT_RIDE].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_CONSTRUCT_RIDE].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_PATH].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_PATH].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_SCENERY].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_SCENERY].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_WATER].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_WATER].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_LAND].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_LAND].left = x;
	x -= 1;
	window_game_top_toolbar_widgets[WIDX_CLEAR_SCENERY].right = x;
	x -= 29;
	window_game_top_toolbar_widgets[WIDX_CLEAR_SCENERY].left = x;

	// Footpath button pressed down
	if (window_find_by_class(WC_FOOTPATH) == NULL)
		w->pressed_widgets &= ~(1 << WIDX_PATH);
	else
		w->pressed_widgets |= (1 << WIDX_PATH);

	// Fast forward button pressed down
	if (0)
		w->pressed_widgets |= (1 << WIDX_FASTFORWARD);
	else
		w->pressed_widgets &= ~(1 << WIDX_FASTFORWARD);

	// Zoomed out/in disable. Not sure where this code is in the original.
	if (window_get_main()->viewport->zoom == 0){
		w->disabled_widgets |= (1 << WIDX_ZOOM_IN);
	}
	else if (window_get_main()->viewport->zoom == 3){
		w->disabled_widgets |= (1 << WIDX_ZOOM_OUT);
	}
	else
	{
		w->disabled_widgets &= ~((1 << WIDX_ZOOM_IN) | (1 << WIDX_ZOOM_OUT));
	}
}

/**
 * 
 *  rct2: 0x0066C8EC
 */
static void window_game_top_toolbar_paint()
{
	int x, y, imgId;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	// Draw staff button image (setting masks to the staff colours)
	x = w->x + window_game_top_toolbar_widgets[WIDX_STAFF].left;
	y = w->y + window_game_top_toolbar_widgets[WIDX_STAFF].top;
	imgId = 5627;
	if (widget_is_pressed(w, WIDX_STAFF))
		imgId++;
	imgId |= (RCT2_GLOBAL(RCT2_ADDRESS_HANDYMAN_COLOUR, uint8) << 19) | 0xA0000000 | (RCT2_GLOBAL(RCT2_ADDRESS_MECHANIC_COLOUR, uint8) << 24);
	gfx_draw_sprite(dpi, imgId, x, y, 0);

	// Draw research button
	x = w->x + window_game_top_toolbar_widgets[WIDX_RESEARCH].left - 1;
	y = w->y + window_game_top_toolbar_widgets[WIDX_RESEARCH].top - 1;
	imgId = SPR_TAB_FINANCES_RESEARCH_0;
	gfx_draw_sprite(dpi, imgId, x, y, 0);
}

/**
 * rct2: 0x6e2cc6
 */
static void window_game_top_toolbar_scenery_tool_down(short x, short y, rct_window* w, short widgetIndex){
	RCT2_CALLPROC_EBPSAFE(0x006E2712);
	if (window_scenery_is_repaint_scenery_tool_on & 1){
		//6e3158
		RCT2_CALLPROC_X(0x6E2CC6, x, y, 0, widgetIndex, (int)w, 0, 0);
	}

	int selected_tab = window_scenery_selected_scenery_by_tab[window_scenery_active_tab_index];
	if (selected_tab == -1) return;

	sint16 grid_x, grid_y, grid_z;
	uint8 item_colour;
	int ebp = selected_tab;

	{
		int eax = x, ebx = y, ecx = 0, edx = 0, esi = 0, edi = 0, ebp = selected_tab;
		RCT2_CALLFUNC_X(0x6E1F34, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
		item_colour = edi;
		grid_x = eax;
		grid_y = ecx;
		grid_z = edx;
	}

	if (grid_x == 0x8000)return;
	
	if (ebp >= 1024){
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1161;

		// The return value will be banner id but the input is colour
		int banner_id = item_colour;
		
		int ebx = 1;

		{
			int esi = 0, eax = grid_x, ecx = grid_y, edx = grid_z;
			game_do_command_p(GAME_COMMAND_50, &eax, &ebx, &ecx, &edx, &esi, &banner_id, &ebp); 
		}

		if (ebx == 0x80000000)return;

		sound_play_panned(SOUND_PLACE_ITEM, 0x8001, RCT2_GLOBAL(0x009DEA5E, uint16), RCT2_GLOBAL(0x009DEA60, uint16), RCT2_GLOBAL(0x009DEA62, uint16));

		window_banner_open(banner_id);
	}
	else if (ebp >= 768){
		//6e301c
		RCT2_CALLPROC_X(0x6E2CC6, x, y, 0, widgetIndex, (int)w, 0, 0);
	}
	else if (ebp >= 512){
		//6e2f2e
		RCT2_CALLPROC_X(0x6E2CC6, x, y, 0, widgetIndex, (int)w, 0, 0);
	}
	else if (ebp >= 256){
		//6e2eda
		RCT2_CALLPROC_X(0x6E2CC6, x, y, 0, widgetIndex, (int)w, 0, 0);
	}
	else{
		//6e2d2d
		RCT2_CALLPROC_X(0x6E2CC6, x, y, 0, widgetIndex, (int)w, 0, 0);
	}
}

/**
 * rct2: 0x0066CB73
 */
static void window_game_top_toolbar_tool_down(){
	short widgetIndex;
	rct_window* w;
	short x, y;

	window_tool_get_registers(w, widgetIndex, x, y);

	switch (widgetIndex){
	case WIDX_CLEAR_SCENERY:
		if (!RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16)&(1 << 0)){
			return;
		}

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 3438;

		game_do_command(
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16),
			1,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16),
			0,
			GAME_COMMAND_57,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16)
			);
		break;
	case WIDX_LAND:
		RCT2_CALLPROC_X(0x66CBF3, x, y, 0, widgetIndex, (int)w, 0, 0);
		break;
	case WIDX_WATER:
		RCT2_CALLPROC_X(0x66CC48, x, y, 0, widgetIndex, (int)w, 0, 0);
		break;
	case WIDX_SCENERY:
		window_game_top_toolbar_scenery_tool_down(x, y, w, widgetIndex);
		break;
	}
}