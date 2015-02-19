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
#include "../editor.h"
#include "../game.h"
#include "../input.h"
#include "../sprites.h"
#include "../audio/audio.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/viewport.h"
#include "../localisation/localisation.h"
#include "dropdown.h"
#include "scenery.h"

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

	//WIDX_FASTFORWARD,
	WIDX_FINANCES,
	WIDX_RESEARCH,

	WIDX_SEPARATOR,
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
} TOP_TOOLBAR_VIEW_MENU_DDIDX;

#pragma region Toolbar_widget_ordering

// from left to right
static const int left_aligned_widgets_order[] = {
	WIDX_PAUSE,
	//WIDX_FASTFORWARD,
	WIDX_FILE_MENU,

	WIDX_SEPARATOR,

	WIDX_ZOOM_OUT,
	WIDX_ZOOM_IN,
	WIDX_ROTATE,
	WIDX_VIEW_MENU,
	WIDX_MAP,
};

// from right to left
static const int right_aligned_widgets_order[] = {
	WIDX_GUESTS,
	WIDX_STAFF,
	WIDX_PARK,
	WIDX_RIDES,
	WIDX_RESEARCH,
	WIDX_FINANCES,

	WIDX_SEPARATOR,

	WIDX_CONSTRUCT_RIDE,
	WIDX_PATH,
	WIDX_SCENERY,
	WIDX_WATER,
	WIDX_LAND,
	WIDX_CLEAR_SCENERY,
};

#pragma endregion

static rct_widget window_top_toolbar_widgets[] = {
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

	//{ WWT_TRNBTN,	0,	0x001E,	0x003B,	0,						27,		0x20000000 | 0x15F9,						STR_NONE },							// Fast forward
	{ WWT_TRNBTN,	3,	0x001E, 0x003B, 0,						27,		0x20000000 | 0x15F9,						3235 },								// Finances
	{ WWT_TRNBTN,	3,	0x001E,	0x003B,	0,						27,		0x20000000 | 0x15F9,						2275 },								// Research
	
	{ WWT_EMPTY,	0,	0,		10-1,	0,						0,		0xFFFFFFFF,									STR_NONE },							// Artificial widget separator
	{ WIDGETS_END },
};

static void window_top_toolbar_emptysub() { }
static void window_top_toolbar_mouseup();
static void window_top_toolbar_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_top_toolbar_dropdown();
static void window_top_toolbar_tool_update();
static void window_top_toolbar_tool_down();
static void window_top_toolbar_tool_drag();
static void window_top_toolbar_invalidate();
static void window_top_toolbar_paint();

static void* window_top_toolbar_events[] = {
	window_top_toolbar_emptysub,
	window_top_toolbar_mouseup,
	window_top_toolbar_emptysub,
	window_top_toolbar_mousedown,
	window_top_toolbar_dropdown,
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,					// check if editor versions are significantly different...
	window_top_toolbar_tool_update,					// editor: 0x0066fB0E
	window_top_toolbar_tool_down,					// editor: 0x0066fB5C
	window_top_toolbar_tool_drag,					// editor: 0x0066fB37
	(void*)0x0066CC5B,								// editor: 0x0066fC44
	(void*)0x0066CA58,								// editor: 0x0066fA74
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,
	window_top_toolbar_emptysub,
	window_top_toolbar_invalidate,
	window_top_toolbar_paint,
	window_top_toolbar_emptysub
};

void top_toolbar_init_view_menu(rct_window *window, rct_widget *widget);
void top_toolbar_view_menu_dropdown(short dropdownIndex);

void toggle_footpath_window();
void toggle_land_window(rct_window *topToolbar, int widgetIndex);
void toggle_clear_scenery_window(rct_window *topToolbar, int widgetIndex);
void toggle_water_window(rct_window *topToolbar, int widgetIndex);

/**
 * Creates the main game top toolbar window.
 *  rct2: 0x0066B485 (part of 0x0066B3E8)
 */
void window_top_toolbar_open()
{
	rct_window* window;

	window = window_create(
		0, 0,
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16), 28,
		(uint32*)window_top_toolbar_events,
		WC_TOP_TOOLBAR,
		WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_5
	);
	window->widgets = window_top_toolbar_widgets;

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
static void window_top_toolbar_mouseup()
{
	short widgetIndex;
	rct_window *w, *mainWindow;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_PAUSE:
		game_do_command(0, 1, 0, 0, GAME_COMMAND_TOGGLE_PAUSE, 0, 0);
		break;
	// case WIDX_FASTFORWARD:
	// 	// This is an excellent place to add in debugging statements and
	// 	// print routines, that will be triggered when you press the
	// 	// button in the game. Use "git update-index --skip-worktree
	// 	// src/window_top_toolbar" to avoid committing these changes to
	// 	// version control.
	// 	window_cheats_open();
	// 	break;

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
		toggle_clear_scenery_window(w, WIDX_CLEAR_SCENERY);
		break;
	case WIDX_LAND:
		toggle_land_window(w, WIDX_LAND);
		break;
	case WIDX_WATER:
		toggle_water_window(w, WIDX_WATER);
		break;
	case WIDX_SCENERY:
		if (!tool_set(w, WIDX_SCENERY, 0)) {
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
			window_scenery_open();
		}
		break;
	case WIDX_PATH:
		toggle_footpath_window();
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
	case WIDX_FINANCES:
		window_finances_open();
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
static void window_top_toolbar_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	int numItems;

	switch (widgetIndex) {
	case WIDX_FILE_MENU:
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)) {
			gDropdownItemsFormat[0] = STR_ABOUT;
			gDropdownItemsFormat[1] = STR_OPTIONS;
			gDropdownItemsFormat[2] = STR_SCREENSHOT;
			gDropdownItemsFormat[3] = 0;
			gDropdownItemsFormat[4] = STR_QUIT_TRACK_DESIGNS_MANAGER;

			if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER)
				gDropdownItemsFormat[4] = STR_QUIT_ROLLERCOASTER_DESIGNER;

			numItems = 5;
		} else if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) {
			gDropdownItemsFormat[0] = STR_LOAD_LANDSCAPE;
			gDropdownItemsFormat[1] = STR_SAVE_LANDSCAPE;
			gDropdownItemsFormat[2] = 0;
			gDropdownItemsFormat[3] = STR_ABOUT;
			gDropdownItemsFormat[4] = STR_OPTIONS;
			gDropdownItemsFormat[5] = STR_SCREENSHOT;
			gDropdownItemsFormat[6] = 0;
			gDropdownItemsFormat[7] = STR_QUIT_SCENARIO_EDITOR;
			numItems = 8;
		} else {
			gDropdownItemsFormat[0] = STR_LOAD_GAME;
			gDropdownItemsFormat[1] = STR_SAVE_GAME;
			gDropdownItemsFormat[2] = 0;
			gDropdownItemsFormat[3] = STR_ABOUT;
			gDropdownItemsFormat[4] = STR_OPTIONS;
			gDropdownItemsFormat[5] = STR_SCREENSHOT;
			gDropdownItemsFormat[6] = 0;
			gDropdownItemsFormat[7] = STR_QUIT_GAME;
			numItems = 8;
		}
		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[0] | 0x80,
			0x80,
			numItems
		);
		break;
	case WIDX_VIEW_MENU:
		top_toolbar_init_view_menu(w, widget);
		break;
	case WIDX_MAP:
		gDropdownItemsFormat[0] = 2523;
		gDropdownItemsFormat[1] = 2780;
		numItems = 2;

		if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) && g_editor_step == EDITOR_STEP_LANDSCAPE_EDITOR) {
			gDropdownItemsFormat[2] = 2690;
			numItems++;
		}

		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[1] | 0x80,
			0,
			numItems
		);
		RCT2_GLOBAL(0x009DEBA2, uint16) = 0;
		break;
	}
}

/**
 * 
 *  rct2: 0x0066C9EA
 */
static void window_top_toolbar_dropdown()
{
	short widgetIndex, dropdownIndex;
	rct_window* w;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	switch (widgetIndex) {
	case WIDX_FILE_MENU:
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
			dropdownIndex += DDIDX_ABOUT;

		switch (dropdownIndex) {
		case DDIDX_LOAD_GAME:
			game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 0, 0);
			break;
		case DDIDX_SAVE_GAME:
			if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) {
				// RCT2_CALLPROC_EBPSAFE(0x0066FE2A);
				window_loadsave_open(LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE);
			} else {
				tool_cancel();
				save_game();
			}
			break;
		case DDIDX_ABOUT:
			window_about_open();
			break;
		case DDIDX_OPTIONS:
			window_options_open();
			break;
		case DDIDX_SCREENSHOT:
			RCT2_GLOBAL(RCT2_ADDRESS_SCREENSHOT_COUNTDOWN, sint8) = 10;
			break;
		case DDIDX_QUIT_GAME:
			window_close_by_class(WC_MANAGE_TRACK_DESIGN);
			window_close_by_class(WC_TRACK_DELETE_PROMPT);
			game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 1, 0);
			break;
		}
		break;
	case WIDX_VIEW_MENU:
		top_toolbar_view_menu_dropdown(dropdownIndex);
		break;
	case WIDX_MAP:
		if (dropdownIndex == -1)
			dropdownIndex = RCT2_GLOBAL(0x009DEBA2, uint16);

		switch (dropdownIndex) {
		case 0:
			window_map_open();
			break;
		case 1:
			window_viewport_open();
			break;
		case 2:
			window_mapgen_open();
			break;
		}
		break;
	}
}

/**
 * 
 *  rct2: 0x0066C810
 */
static void window_top_toolbar_invalidate()
{
	int i, x, enabledWidgets, widgetIndex, widgetWidth, firstAlignment;
	rct_window *w;
	rct_widget *widget;

	window_get_register(w);

	// Enable / disable buttons
	window_top_toolbar_widgets[WIDX_PAUSE].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_FILE_MENU].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_ZOOM_OUT].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_ZOOM_IN].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_ROTATE].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_VIEW_MENU].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_MAP].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_LAND].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_WATER].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_SCENERY].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_PATH].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_CONSTRUCT_RIDE].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_RIDES].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_PARK].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_STAFF].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_GUESTS].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_CLEAR_SCENERY].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_FINANCES].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_RESEARCH].type = WWT_TRNBTN;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)) {
		window_top_toolbar_widgets[WIDX_PAUSE].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_RIDES].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_PARK].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_STAFF].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_GUESTS].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_FINANCES].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_RESEARCH].type = WWT_EMPTY;

		if (g_editor_step != EDITOR_STEP_LANDSCAPE_EDITOR) {
			window_top_toolbar_widgets[WIDX_MAP].type = WWT_EMPTY;
			window_top_toolbar_widgets[WIDX_LAND].type = WWT_EMPTY;
			window_top_toolbar_widgets[WIDX_WATER].type = WWT_EMPTY;
			window_top_toolbar_widgets[WIDX_SCENERY].type = WWT_EMPTY;
			window_top_toolbar_widgets[WIDX_PATH].type = WWT_EMPTY;
			window_top_toolbar_widgets[WIDX_CLEAR_SCENERY].type = WWT_EMPTY;
		}
		
		if (g_editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER) {
			window_top_toolbar_widgets[WIDX_CONSTRUCT_RIDE].type = WWT_EMPTY;
		}

		if (g_editor_step != EDITOR_STEP_LANDSCAPE_EDITOR && g_editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER) {
			window_top_toolbar_widgets[WIDX_ZOOM_OUT].type = WWT_EMPTY;
			window_top_toolbar_widgets[WIDX_ZOOM_IN].type = WWT_EMPTY;
			window_top_toolbar_widgets[WIDX_ROTATE].type = WWT_EMPTY;
			window_top_toolbar_widgets[WIDX_VIEW_MENU].type = WWT_EMPTY;
		}
	} else {
		if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)
			window_top_toolbar_widgets[WIDX_FINANCES].type = WWT_EMPTY;
	}

	enabledWidgets = 0;
	for (i = WIDX_PAUSE; i <= WIDX_RESEARCH; i++)
		if (window_top_toolbar_widgets[i].type != WWT_EMPTY)
			enabledWidgets |= (1 << i);
	w->enabled_widgets = enabledWidgets;

	// Align left hand side toolbar buttons
	firstAlignment = 1;
	x = 0;
	for (int i = 0; i < countof(left_aligned_widgets_order); ++i) {
		widgetIndex = left_aligned_widgets_order[i];
		widget = &window_top_toolbar_widgets[widgetIndex];
		if (widget->type == WWT_EMPTY && widgetIndex != WIDX_SEPARATOR)
			continue;

		if (firstAlignment && widgetIndex == WIDX_SEPARATOR)
			continue;

		widgetWidth = widget->right - widget->left;
		widget->left = x;
		x += widgetWidth;
		widget->right = x;
		x += 1;
		firstAlignment = 0;
	}

	// Align right hand side toolbar buttons
	firstAlignment = 1;
	x = max(640, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16));
	for (int i = 0; i < countof(right_aligned_widgets_order); ++i) {
		widgetIndex = right_aligned_widgets_order[i];
		widget = &window_top_toolbar_widgets[widgetIndex];
		if (widget->type == WWT_EMPTY && widgetIndex != WIDX_SEPARATOR)
			continue;

		if (firstAlignment && widgetIndex == WIDX_SEPARATOR)
			continue;

		widgetWidth = widget->right - widget->left;
		x -= 1;
		widget->right = x;
		x -= widgetWidth;
		widget->left = x;
		firstAlignment = 0;
	}

	// Footpath button pressed down
	if (window_find_by_class(WC_FOOTPATH) == NULL)
		w->pressed_widgets &= ~(1 << WIDX_PATH);
	else
		w->pressed_widgets |= (1 << WIDX_PATH);

	// Fast forward button pressed down
	// if (0)
	// 	w->pressed_widgets |= (1 << WIDX_FASTFORWARD);
	// else
	// 	w->pressed_widgets &= ~(1 << WIDX_FASTFORWARD);

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint32) & 1))
		w->pressed_widgets &= ~(1 << WIDX_PAUSE);
	else
		w->pressed_widgets |= (1 << WIDX_PAUSE);

	// Zoomed out/in disable. Not sure where this code is in the original.
	if (window_get_main()->viewport->zoom == 0){
		w->disabled_widgets |= (1 << WIDX_ZOOM_IN);
	} else if (window_get_main()->viewport->zoom == 3){
		w->disabled_widgets |= (1 << WIDX_ZOOM_OUT);
	} else {
		w->disabled_widgets &= ~((1 << WIDX_ZOOM_IN) | (1 << WIDX_ZOOM_OUT));
	}
}

/**
 * 
 *  rct2: 0x0066C8EC
 */
static void window_top_toolbar_paint()
{
	int x, y, imgId;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	// Draw staff button image (setting masks to the staff colours)
	if (window_top_toolbar_widgets[WIDX_STAFF].type != WWT_EMPTY) {
		x = w->x + window_top_toolbar_widgets[WIDX_STAFF].left;
		y = w->y + window_top_toolbar_widgets[WIDX_STAFF].top;
		imgId = 5627;
		if (widget_is_pressed(w, WIDX_STAFF))
			imgId++;
		imgId |= (RCT2_GLOBAL(RCT2_ADDRESS_HANDYMAN_COLOUR, uint8) << 19) | 0xA0000000 | (RCT2_GLOBAL(RCT2_ADDRESS_MECHANIC_COLOUR, uint8) << 24);
		gfx_draw_sprite(dpi, imgId, x, y, 0);
	}

	// Draw research button
	if (window_top_toolbar_widgets[WIDX_RESEARCH].type != WWT_EMPTY) {
		x = w->x + window_top_toolbar_widgets[WIDX_RESEARCH].left - 1;
		y = w->y + window_top_toolbar_widgets[WIDX_RESEARCH].top;
		imgId = SPR_TAB_FINANCES_RESEARCH_0;
		gfx_draw_sprite(dpi, imgId, x, y, 0);
	}

	// Draw finances button
	if (window_top_toolbar_widgets[WIDX_FINANCES].type != WWT_EMPTY) {
		x = w->x + window_top_toolbar_widgets[WIDX_FINANCES].left + 3;
		y = w->y + window_top_toolbar_widgets[WIDX_FINANCES].top + 1;
		imgId = SPR_FINANCE;
		gfx_draw_sprite(dpi, imgId, x, y, 0);
	}
}

/**
 * rct2: 0x6e2cc6
 */
static void window_top_toolbar_scenery_tool_down(short x, short y, rct_window* w, short widgetIndex){
	RCT2_CALLPROC_EBPSAFE(0x006E2712);
	if (window_scenery_is_repaint_scenery_tool_on & 1){
		//6e3158
		RCT2_CALLPROC_X(0x6E2CC6, x, y, 0, widgetIndex, (int)w, 0, 0);
	}

	int selected_tab = window_scenery_selected_scenery_by_tab[window_scenery_active_tab_index];
	if (selected_tab == -1) return;

	sint16 grid_x, grid_y, grid_z;
	uint8 item_colour;
	uint8 model_type;
	int ebp = selected_tab;

	{
		int eax = x, ebx = y, ecx = 0, edx = 0, esi = 0, edi = 0;
		RCT2_CALLFUNC_X(0x6E1F34, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
		item_colour = edi;
		model_type = (ebx & 0xFF00) >> 8;
		grid_x = eax;
		grid_y = ecx;
		grid_z = edx;
	}

	if (grid_x == 0x8000)return;
	
	if (ebp >= 1024){
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1161;

		// The return value will be banner id but the input is colour
		int banner_id = item_colour;
		
		int ebx = (model_type << 8) | 1;

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
 *
 *  rct2: 0x0066CB25
 */
static void window_top_toolbar_tool_update()
{
	short widgetIndex;
	rct_window *w;
	short x, y;

	window_tool_get_registers(w, widgetIndex, x, y);

	switch (widgetIndex){
	case WIDX_CLEAR_SCENERY:
		RCT2_CALLPROC_X(0x0068E213, x, y, 0, widgetIndex, (int)w, 0, 0);
		break;
	case WIDX_LAND:
		RCT2_CALLPROC_X(0x00664280, x, y, 0, widgetIndex, (int)w, 0, 0);
		break;
	case WIDX_WATER:
		RCT2_CALLPROC_X(0x006E6BDC, x, y, 0, widgetIndex, (int)w, 0, 0);
		break;
	case WIDX_SCENERY:
		RCT2_CALLPROC_X(0x006E287B, x, y, 0, widgetIndex, (int)w, 0, 0);
		break;
	}
}

/**
 * rct2: 0x0066CB73
 */
static void window_top_toolbar_tool_down(){
	short widgetIndex;
	rct_window* w;
	short x, y;

	window_tool_get_registers(w, widgetIndex, x, y);

	switch (widgetIndex){
	case WIDX_CLEAR_SCENERY:
		if (!RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & (1 << 0))
			break;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 3438;

		game_do_command(
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16),
			1,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16),
			0,
			GAME_COMMAND_CLEAR_SCENERY,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16)
			);
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 12;
		break;
	case WIDX_LAND:
		if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16)&(1 << 0)){
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1387;
			game_do_command(
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16),
				1,
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16),
				RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) | (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) << 8),
				GAME_COMMAND_CHANGE_SURFACE_STYLE,
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16),
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16)
				);
			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 3;
		}
		break;
	case WIDX_WATER:
		if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16)&(1 << 0)){
			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 3;
		}
		break;
	case WIDX_SCENERY:
		window_top_toolbar_scenery_tool_down(x, y, w, widgetIndex);
		break;
	}
}

/**
*  part of window_top_toolbar_tool_drag(0x0066CB4E)
*  rct2: 0x006E6D4B
*/
void window_top_toolbar_water_tool_drag(short x, short y, rct_window* w, short widgetIndex)
{
	//RCT2_CALLPROC_X(0x006E6D4B, x, y, 0, widgetIndex, (int)w, 0, 0);

	rct_window *window = window_find_from_point(x, y);
	if (!window)
		return;
	int widget_index = window_find_widget_from_point(window, x, y);
	if (widgetIndex == 0xFFFF)
		return;
	rct_widget *widget = &window->widgets[widget_index];
	if (widget->type != WWT_VIEWPORT)
		return;
	rct_viewport *viewport = window->viewport;
	if (!viewport)
		return;

	sint16 dx = 0xFFF0;
	dx >>= viewport->zoom;

	y -= RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, uint16);

	if (y <= dx) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, uint16) += dx;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = STR_CANT_RAISE_WATER_LEVEL_HERE;

		game_do_command(
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16),
			1,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16),
			dx,
			GAME_COMMAND_RAISE_WATER,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16)
			);
		RCT2_GLOBAL(RCT2_ADDRESS_WATER_RAISE_COST, uint32) = 0x80000000;
		RCT2_GLOBAL(RCT2_ADDRESS_WATER_LOWER_COST, uint32) = 0x80000000;

		return;
	}

	dx = -dx;

	if (y >= dx) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, uint16) += dx;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = STR_CANT_LOWER_WATER_LEVEL_HERE;

		game_do_command(
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16),
			1,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16),
			dx,
			GAME_COMMAND_LOWER_WATER,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16)
			);
		RCT2_GLOBAL(RCT2_ADDRESS_WATER_RAISE_COST, uint32) = 0x80000000;
		RCT2_GLOBAL(RCT2_ADDRESS_WATER_LOWER_COST, uint32) = 0x80000000;

		return;
	}
}

/**
 *
 *  rct2: 0x0066CB4E
 */
static void window_top_toolbar_tool_drag()
{
	short widgetIndex;
	rct_window *w;
	short x, y;

	window_tool_get_registers(w, widgetIndex, x, y);

	switch (widgetIndex){
	case WIDX_CLEAR_SCENERY:
		if (window_find_by_class(WC_ERROR) != NULL)
			break;

		if (!RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & (1 << 0))
			break;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 3438;

		game_do_command(
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16),
			1,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16),
			0,
			GAME_COMMAND_CLEAR_SCENERY,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16)
		);
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 12;
		break;
	case WIDX_LAND:
		RCT2_CALLPROC_X(0x00664454, x, y, 0, widgetIndex, (int)w, 0, 0);
		break;
	case WIDX_WATER:
		window_top_toolbar_water_tool_drag(x, y, w, widgetIndex);
		break;
	case WIDX_SCENERY:
		RCT2_CALLPROC_X(0x006E2CBC, x, y, 0, widgetIndex, (int)w, 0, 0);
		break;
	}
}

/**
 *
 *  rct2: 0x0066CDE4
 */
void top_toolbar_init_view_menu(rct_window* w, rct_widget* widget) {
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

	gDropdownItemsArgs[0] = STR_UNDERGROUND_VIEW;
	gDropdownItemsArgs[1] = STR_REMOVE_BASE_LAND;
	gDropdownItemsArgs[2] = STR_REMOVE_VERTICAL_FACES;
	gDropdownItemsArgs[4] = STR_SEE_THROUGH_RIDES;
	gDropdownItemsArgs[5] = STR_SEE_THROUGH_SCENERY;
	gDropdownItemsArgs[6] = STR_INVISIBLE_SUPPORTS;
	gDropdownItemsArgs[7] = STR_INVISIBLE_PEOPLE;
	gDropdownItemsArgs[9] = STR_HEIGHT_MARKS_ON_LAND;
	gDropdownItemsArgs[10] = STR_HEIGHT_MARKS_ON_RIDE_TRACKS;
	gDropdownItemsArgs[11] = STR_HEIGHT_MARKS_ON_PATHS;

	window_dropdown_show_text(
		w->x + widget->left,
		w->y + widget->top,
		widget->bottom - widget->top + 1,
		w->colours[1] | 0x80,
		0,
		12
	);

	// Set checkmarks
	rct_viewport* mainViewport = window_get_main()->viewport;
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
}

/**
 *
 *  rct2: 0x0066CF8A
 */
void top_toolbar_view_menu_dropdown(short dropdownIndex) {
	if (dropdownIndex == -1) dropdownIndex = RCT2_GLOBAL(0x9DEBA2, uint16);
	rct_window* w = window_get_main();
	if (w) {
		switch (dropdownIndex) {
		case DDIDX_UNDERGROUND_INSIDE:
			w->viewport->flags ^= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
			break;
		case DDIDX_HIDE_BASE:
			w->viewport->flags ^= VIEWPORT_FLAG_HIDE_BASE;
			break;
		case DDIDX_HIDE_VERTICAL:
			w->viewport->flags ^= VIEWPORT_FLAG_HIDE_VERTICAL;
			break;
		case DDIDX_SEETHROUGH_RIDES:
			w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_RIDES;
			break;
		case DDIDX_SEETHROUGH_SCENARY:
			w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_SCENERY;
			break;
		case DDIDX_INVISIBLE_SUPPORTS:
			w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_SUPPORTS;
			break;
		case DDIDX_INVISIBLE_PEEPS:
			w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_PEEPS;
			break;
		case DDIDX_LAND_HEIGHTS:
			w->viewport->flags ^= VIEWPORT_FLAG_LAND_HEIGHTS;
			break;
		case DDIDX_TRACK_HEIGHTS:
			w->viewport->flags ^= VIEWPORT_FLAG_TRACK_HEIGHTS;
			break;
		case DDIDX_PATH_HEIGHTS:
			w->viewport->flags ^= VIEWPORT_FLAG_PATH_HEIGHTS;
			break;
		default:
			return;
		}
		window_invalidate(w);
	}
}


/**
 *
 *  rct2: 0x0066CCE7
 */
void toggle_footpath_window()
{
	if (window_find_by_class(WC_FOOTPATH) == NULL) {
		window_footpath_open();
	} else {
		tool_cancel();
		window_close_by_class(WC_FOOTPATH);
	}
}

/**
 *
 * rct2: 0x0066CD54
 */
void toggle_land_window(rct_window *topToolbar, int widgetIndex)
{
	if ((RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == 1 && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) == 7) {
		tool_cancel();
	} else {
		show_gridlines();
		tool_set(topToolbar, widgetIndex, 18);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;
		window_land_open();
	}
}

/**
 *
 * rct2: 0x0066CD0C
 */
void toggle_clear_scenery_window(rct_window *topToolbar, int widgetIndex)
{
	if ((RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == 1 && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) == 16) {
		tool_cancel();
	} else {
		show_gridlines();
		tool_set(topToolbar, widgetIndex, 12);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 2;
		window_clear_scenery_open();
	}
}

/**
 *
 * rct2: 0x0066CD9C
 */
void toggle_water_window(rct_window *topToolbar, int widgetIndex)
{
	if ((RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == 1 && RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) == 8) {
		tool_cancel();
	} else {
		show_gridlines();
		tool_set(topToolbar, widgetIndex, 19);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;
		window_water_open();
	}
}