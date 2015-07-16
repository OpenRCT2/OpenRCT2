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
#include "../cheats.h"
#include "../config.h"
#include "../editor.h"
#include "../game.h"
#include "../input.h"
#include "../sprites.h"
#include "../audio/audio.h"
#include "../interface/screenshot.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/viewport.h"
#include "../localisation/localisation.h"
#include "../network/twitch.h"
#include "../scenario.h"
#include "../world/scenery.h"
#include "../world/banner.h"
#include "dropdown.h"
#include "../interface/themes.h"
#include "../interface/console.h"

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
	WIDX_CHEATS,
	WIDX_DEBUG,
	WIDX_FINANCES,
	WIDX_RESEARCH,
	WIDX_NETWORK,

	WIDX_SEPARATOR,
};

typedef enum {
	DDIDX_LOAD_GAME = 0,
	DDIDX_SAVE_GAME = 1,
	// separator
	DDIDX_ABOUT = 3,
	DDIDX_OPTIONS = 4,
	DDIDX_SCREENSHOT = 5,
	DDIDX_GIANT_SCREENSHOT = 6,
	// separator
	DDIDX_QUIT_TO_MENU = 8,
	DDIDX_EXIT_OPENRCT2 = 9,
	// separator
	DDIDX_ENABLE_TWITCH = 11
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

typedef enum {
	DDIDX_CONSOLE = 0,
	DDIDX_TILE_INSPECTOR = 1,
	DDIDX_OBJECT_SELECTION = 2
} TOP_TOOLBAR_DEBUG_DDIDX;

typedef enum {
	DDIDX_PLAYER_LIST = 0
} TOP_TOOLBAR_NETWORK_DDIDX;

enum {
	DDIDX_CHEATS,
	DDIDX_ENABLE_SANDBOX_MODE = 2,
	DDIDX_DISABLE_CLEARANCE_CHECKS,
	DDIDX_DISABLE_SUPPORT_LIMITS
};

#pragma region Toolbar_widget_ordering

// from left to right
static const int left_aligned_widgets_order[] = {
	WIDX_PAUSE,
	WIDX_FASTFORWARD,
	WIDX_FILE_MENU,
	WIDX_CHEATS,
	WIDX_DEBUG,

	WIDX_SEPARATOR,

	WIDX_ZOOM_OUT,
	WIDX_ZOOM_IN,
	WIDX_ROTATE,
	WIDX_VIEW_MENU,
	WIDX_MAP,
	WIDX_NETWORK,
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

	{ WWT_TRNBTN,	0,	0x001E,	0x003B,	0,						27,		0x20000000 | 0x15F9,						5148 },								// Fast forward
	{ WWT_TRNBTN,	0,	0x001E,	0x003B,	0,						27,		0x20000000 | 0x15F9,						5149 },								// Cheats
	{ WWT_TRNBTN,	0,	0x001E,	0x003B,	0,						27,		0x20000000 | 0x15F9,						STR_DEBUG_TIP },					// Debug
	{ WWT_TRNBTN,	3,	0x001E,	0x003B, 0,						27,		0x20000000 | 0x15F9,						3235 },								// Finances
	{ WWT_TRNBTN,	3,	0x001E,	0x003B,	0,						27,		0x20000000 | 0x15F9,						2275 },								// Research
	{ WWT_TRNBTN,	1,	0x001E,	0x003B,	0,						27,		0x20000000 | 0x15F9,						2276 },								// Network
	
	{ WWT_EMPTY,	0,	0,		10-1,	0,						0,		0xFFFFFFFF,									STR_NONE },							// Artificial widget separator
	{ WIDGETS_END },
};

static void window_top_toolbar_mouseup(rct_window *w, int widgetIndex);
static void window_top_toolbar_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_top_toolbar_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_top_toolbar_tool_update(rct_window* w, int widgetIndex, int x, int y);
static void window_top_toolbar_tool_down(rct_window* w, int widgetIndex, int x, int y);
static void window_top_toolbar_tool_drag(rct_window* w, int widgetIndex, int x, int y);
static void window_top_toolbar_tool_up(rct_window* w, int widgetIndex, int x, int y);
static void window_top_toolbar_tool_abort(rct_window *w, int widgetIndex);
static void window_top_toolbar_invalidate(rct_window *w);
static void window_top_toolbar_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_top_toolbar_events = {
	NULL,
	window_top_toolbar_mouseup,
	NULL,
	window_top_toolbar_mousedown,
	window_top_toolbar_dropdown,
	NULL,
	NULL,
	NULL,
	NULL,											// check if editor versions are significantly different...
	window_top_toolbar_tool_update,					// editor: 0x0066fB0E
	window_top_toolbar_tool_down,					// editor: 0x0066fB5C
	window_top_toolbar_tool_drag,					// editor: 0x0066fB37
	window_top_toolbar_tool_up,						// editor: 0x0066fC44 (Exactly the same)
	window_top_toolbar_tool_abort,					// editor: 0x0066fA74 (Exactly the same)
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
	window_top_toolbar_invalidate,
	window_top_toolbar_paint,
	NULL
};

void top_toolbar_init_view_menu(rct_window *window, rct_widget *widget);
void top_toolbar_view_menu_dropdown(short dropdownIndex);
void top_toolbar_init_fastforward_menu(rct_window *window, rct_widget *widget);
void top_toolbar_fastforward_menu_dropdown(short dropdownIndex);
void top_toolbar_init_rotate_menu(rct_window *window, rct_widget *widget);
void top_toolbar_rotate_menu_dropdown(short dropdownIndex);
void top_toolbar_init_debug_menu(rct_window *window, rct_widget *widget);
void top_toolbar_debug_menu_dropdown(short dropdownIndex);
void top_toolbar_init_network_menu(rct_window *window, rct_widget *widget);
void top_toolbar_network_menu_dropdown(short dropdownIndex);

void toggle_footpath_window();
void toggle_land_window(rct_window *topToolbar, int widgetIndex);
void toggle_clear_scenery_window(rct_window *topToolbar, int widgetIndex);
void toggle_water_window(rct_window *topToolbar, int widgetIndex);

money32 selection_lower_land(uint8 flags);
money32 selection_raise_land(uint8 flags);

static bool _menuDropdownIncludesTwitch;

/**
 * Creates the main game top toolbar window.
 *  rct2: 0x0066B485 (part of 0x0066B3E8)
 */
void window_top_toolbar_open()
{
	rct_window* window;

	window = window_create(
		0, 0,
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16), 28,
		&window_top_toolbar_events,
		WC_TOP_TOOLBAR,
		WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_5
	);
	window->widgets = window_top_toolbar_widgets;

	window_init_scroll_widgets(window);
}

/**
 * 
 *  rct2: 0x0066C957
 */
static void window_top_toolbar_mouseup(rct_window *w, int widgetIndex)
{
	rct_window *mainWindow;

	switch (widgetIndex) {
	case WIDX_PAUSE:
		game_do_command(0, 1, 0, 0, GAME_COMMAND_TOGGLE_PAUSE, 0, 0);
		break;
	case WIDX_ZOOM_OUT:
		if ((mainWindow = window_get_main()) != NULL)
			window_zoom_out(mainWindow);
		break;
	case WIDX_ZOOM_IN:
		if ((mainWindow = window_get_main()) != NULL)
			window_zoom_in(mainWindow);
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
		_menuDropdownIncludesTwitch = false;
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)) {
			gDropdownItemsFormat[0] = STR_ABOUT;
			gDropdownItemsFormat[1] = STR_OPTIONS;
			gDropdownItemsFormat[2] = STR_SCREENSHOT;
			gDropdownItemsFormat[3] = STR_GIANT_SCREENSHOT;
			gDropdownItemsFormat[4] = 0;
			gDropdownItemsFormat[5] = STR_QUIT_TRACK_DESIGNS_MANAGER;
			gDropdownItemsFormat[6] = STR_EXIT_OPENRCT2;

			if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER)
				gDropdownItemsFormat[5] = STR_QUIT_ROLLERCOASTER_DESIGNER;

			numItems = 7;
		} else if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) {
			gDropdownItemsFormat[0] = STR_LOAD_LANDSCAPE;
			gDropdownItemsFormat[1] = STR_SAVE_LANDSCAPE;
			gDropdownItemsFormat[2] = 0;
			gDropdownItemsFormat[3] = STR_ABOUT;
			gDropdownItemsFormat[4] = STR_OPTIONS;
			gDropdownItemsFormat[5] = STR_SCREENSHOT;
			gDropdownItemsFormat[6] = STR_GIANT_SCREENSHOT;
			gDropdownItemsFormat[7] = 0;
			gDropdownItemsFormat[8] = STR_QUIT_SCENARIO_EDITOR;
			gDropdownItemsFormat[9] = STR_EXIT_OPENRCT2;
			numItems = 10;
		} else {
			gDropdownItemsFormat[0] = STR_LOAD_GAME;
			gDropdownItemsFormat[1] = STR_SAVE_GAME;
			gDropdownItemsFormat[2] = 0;
			gDropdownItemsFormat[3] = STR_ABOUT;
			gDropdownItemsFormat[4] = STR_OPTIONS;
			gDropdownItemsFormat[5] = STR_SCREENSHOT;
			gDropdownItemsFormat[6] = STR_GIANT_SCREENSHOT;
			gDropdownItemsFormat[7] = 0;
			gDropdownItemsFormat[8] = STR_QUIT_TO_MENU;
			gDropdownItemsFormat[9] = STR_EXIT_OPENRCT2;
			numItems = 10;

		#ifndef DISABLE_TWITCH
			if (gConfigTwitch.channel != NULL && gConfigTwitch.channel[0] != 0) {
				_menuDropdownIncludesTwitch = true;
				gDropdownItemsFormat[10] = 0;
				gDropdownItemsFormat[11] = 1156;
				gDropdownItemsArgs[11] = STR_TWITCH_ENABLE;
				numItems = 12;
			}
		#endif
		}
		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[0] | 0x80,
			DROPDOWN_FLAG_STAY_OPEN,
			numItems
		);

#ifndef DISABLE_TWITCH
		if (_menuDropdownIncludesTwitch && gTwitchEnable)
			gDropdownItemsChecked |= (1 << 11);
#endif
		break;
	case WIDX_CHEATS:
		gDropdownItemsFormat[0] = 1156;
		gDropdownItemsFormat[1] = 0;
		gDropdownItemsFormat[2] = 1156;
		gDropdownItemsFormat[3] = 1156;
		gDropdownItemsFormat[4] = 1156;
		gDropdownItemsArgs[0] = 5217;
		gDropdownItemsArgs[2] = STR_ENABLE_SANDBOX_MODE;
		gDropdownItemsArgs[3] = STR_DISABLE_CLEARANCE_CHECKS;
		gDropdownItemsArgs[4] = STR_DISABLE_SUPPORT_LIMITS;
		window_dropdown_show_text(
			w->x + widget->left,
			w->y + widget->top,
			widget->bottom - widget->top + 1,
			w->colours[0] | 0x80,
			0,
			5
		);
		if (gCheatsSandboxMode)
			gDropdownItemsChecked |= (1 << DDIDX_ENABLE_SANDBOX_MODE);
		if (gCheatsDisableClearanceChecks)
			gDropdownItemsChecked |= (1 << DDIDX_DISABLE_CLEARANCE_CHECKS);
		if (gCheatsDisableSupportLimits)
			gDropdownItemsChecked |= (1 << DDIDX_DISABLE_SUPPORT_LIMITS);
		RCT2_GLOBAL(0x009DEBA2, uint16) = 0;
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
	case WIDX_FASTFORWARD:
		top_toolbar_init_fastforward_menu(w, widget);
		break;
	case WIDX_ROTATE:
		top_toolbar_init_rotate_menu(w, widget);
		break;
	case WIDX_DEBUG:
		top_toolbar_init_debug_menu(w, widget);
		break;
	case WIDX_NETWORK:
		top_toolbar_init_network_menu(w, widget);
		break;
	}
}

/**
 *
 *  rct2: 0x0066C9EA
 */
static void window_top_toolbar_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
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
				rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;
				window_loadsave_open(LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE, s6Info->name);
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
		case DDIDX_GIANT_SCREENSHOT:
			screenshot_giant();
			break;
		case DDIDX_QUIT_TO_MENU:
			window_close_by_class(WC_MANAGE_TRACK_DESIGN);
			window_close_by_class(WC_TRACK_DELETE_PROMPT);
			game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 1, 0);
			break;
		case DDIDX_EXIT_OPENRCT2:
			rct2_quit();
			break;
#ifndef DISABLE_TWITCH
		case DDIDX_ENABLE_TWITCH:
			gTwitchEnable = !gTwitchEnable;
			break;
#endif
		}
		break;
	case WIDX_CHEATS:
		if (dropdownIndex == -1) dropdownIndex = RCT2_GLOBAL(0x009DEBA2, uint16);
		switch (dropdownIndex) {
		case DDIDX_CHEATS:
			window_cheats_open();
			break;
		case DDIDX_ENABLE_SANDBOX_MODE:
			gCheatsSandboxMode = !gCheatsSandboxMode;
			break;
		case DDIDX_DISABLE_CLEARANCE_CHECKS:
			gCheatsDisableClearanceChecks = !gCheatsDisableClearanceChecks;
			break;
		case DDIDX_DISABLE_SUPPORT_LIMITS:
			gCheatsDisableSupportLimits = !gCheatsDisableSupportLimits;
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
	case WIDX_FASTFORWARD:
		top_toolbar_fastforward_menu_dropdown(dropdownIndex);
		break;
	case WIDX_ROTATE:
		top_toolbar_rotate_menu_dropdown(dropdownIndex);
		break;
	case WIDX_DEBUG:
		top_toolbar_debug_menu_dropdown(dropdownIndex);
		break;
	case WIDX_NETWORK:
		top_toolbar_network_menu_dropdown(dropdownIndex);
		break;
	}
}

/**
 * 
 *  rct2: 0x0066C810
 */
static void window_top_toolbar_invalidate(rct_window *w)
{
	int i, x, enabledWidgets, widgetIndex, widgetWidth, firstAlignment;
	rct_widget *widget;

	colour_scheme_update(w);

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
	window_top_toolbar_widgets[WIDX_FASTFORWARD].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_CHEATS].type = WWT_TRNBTN;
	window_top_toolbar_widgets[WIDX_DEBUG].type = gConfigGeneral.debugging_tools ? WWT_TRNBTN : WWT_EMPTY;
	window_top_toolbar_widgets[WIDX_NETWORK].type = WWT_TRNBTN;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)) {
		window_top_toolbar_widgets[WIDX_PAUSE].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_RIDES].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_PARK].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_STAFF].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_GUESTS].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_FINANCES].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_RESEARCH].type = WWT_EMPTY;
		window_top_toolbar_widgets[WIDX_CHEATS].type = WWT_EMPTY;

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
			window_top_toolbar_widgets[WIDX_FASTFORWARD].type = WWT_EMPTY;
		}

		if (g_editor_step != EDITOR_STEP_LANDSCAPE_EDITOR && g_editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER) {
			window_top_toolbar_widgets[WIDX_ZOOM_OUT].type = WWT_EMPTY;
			window_top_toolbar_widgets[WIDX_ZOOM_IN].type = WWT_EMPTY;
			window_top_toolbar_widgets[WIDX_ROTATE].type = WWT_EMPTY;
			window_top_toolbar_widgets[WIDX_VIEW_MENU].type = WWT_EMPTY;
		}
	} else {
		if ((RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY) || !gConfigInterface.toolbar_show_finances)
			window_top_toolbar_widgets[WIDX_FINANCES].type = WWT_EMPTY;

		if (!gConfigInterface.toolbar_show_research)
			window_top_toolbar_widgets[WIDX_RESEARCH].type = WWT_EMPTY;

		if (!gConfigInterface.toolbar_show_cheats)
			window_top_toolbar_widgets[WIDX_CHEATS].type = WWT_EMPTY;
	}

	enabledWidgets = 0;
	for (i = WIDX_PAUSE; i <= WIDX_NETWORK; i++)
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
	x = max(640, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16));
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
static void window_top_toolbar_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	int x, y, imgId;

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

	// Draw fast forward button
	if (window_top_toolbar_widgets[WIDX_FASTFORWARD].type != WWT_EMPTY) {
		x = w->x + window_top_toolbar_widgets[WIDX_FASTFORWARD].left + 0;
		y = w->y + window_top_toolbar_widgets[WIDX_FASTFORWARD].top + 0;
		if (widget_is_pressed(w, WIDX_FASTFORWARD))
			y++;
		imgId = SPR_G2_FASTFORWARD;
		gfx_draw_sprite(dpi, imgId, x + 6, y + 3, 0);


		for (int i = 0; i < gGameSpeed && gGameSpeed <= 4; i++) {
			gfx_draw_sprite(dpi, SPR_G2_SPEED_ARROW, x + 5 + i * 5, y + 15, 0);
		}
		for (int i = 0; i < 3 && i < gGameSpeed - 4 && gGameSpeed >= 5; i++) {
			gfx_draw_sprite(dpi, SPR_G2_HYPER_ARROW, x + 5 + i * 6, y + 15, 0);
		}
		/*if (gGameSpeed >= 8) {
			gfx_draw_sprite(dpi, SPR_G2_HYPER_ARROWS, x + 5, y + 15, 0);
		}*/
	}

	// Draw cheats button
	if (window_top_toolbar_widgets[WIDX_CHEATS].type != WWT_EMPTY) {
		x = w->x + window_top_toolbar_widgets[WIDX_CHEATS].left - 1;
		y = w->y + window_top_toolbar_widgets[WIDX_CHEATS].top - 1;
		if (widget_is_pressed(w, WIDX_CHEATS))
			y++;
		imgId = SPR_G2_SANDBOX;
		gfx_draw_sprite(dpi, imgId, x, y, 3);
	}

	// Draw debug button
	if (window_top_toolbar_widgets[WIDX_DEBUG].type != WWT_EMPTY) {
		x = w->x + window_top_toolbar_widgets[WIDX_DEBUG].left;
		y = w->y + window_top_toolbar_widgets[WIDX_DEBUG].top - 1;
		if (widget_is_pressed(w, WIDX_DEBUG))
			y++;
		imgId = 5201;
		gfx_draw_sprite(dpi, imgId, x, y, 3);
	}

	// Draw research button
	if (window_top_toolbar_widgets[WIDX_RESEARCH].type != WWT_EMPTY) {
		x = w->x + window_top_toolbar_widgets[WIDX_RESEARCH].left - 1;
		y = w->y + window_top_toolbar_widgets[WIDX_RESEARCH].top;
		if (widget_is_pressed(w, WIDX_RESEARCH))
			y++;
		imgId = SPR_TAB_FINANCES_RESEARCH_0;
		gfx_draw_sprite(dpi, imgId, x, y, 0);
	}

	// Draw finances button
	if (window_top_toolbar_widgets[WIDX_FINANCES].type != WWT_EMPTY) {
		x = w->x + window_top_toolbar_widgets[WIDX_FINANCES].left + 3;
		y = w->y + window_top_toolbar_widgets[WIDX_FINANCES].top + 1;
		if (widget_is_pressed(w, WIDX_FINANCES))
			y++;
		imgId = SPR_FINANCE;
		gfx_draw_sprite(dpi, imgId, x, y, 0);
	}
}

/* rct2: 0x006E3158 */
static void repaint_scenery_tool_down(sint16 x, sint16 y, sint16 widgetIndex){
	// ax, cx, bl
	sint16 grid_x, grid_y;
	int type;
	// edx
	rct_map_element* map_element;
	uint16 flags =
		VIEWPORT_INTERACTION_MASK_SCENERY &
		VIEWPORT_INTERACTION_MASK_WALL &
		VIEWPORT_INTERACTION_MASK_LARGE_SCENERY &
		VIEWPORT_INTERACTION_MASK_BANNER;
	// This is -2 as banner is 12 but flags are offset different

	// not used
	rct_viewport* viewport;
	get_map_coordinates_from_pos(x, y, flags, &grid_x, &grid_y, &type, &map_element, &viewport);

	switch (type){
	case VIEWPORT_INTERACTION_ITEM_SCENERY:
	{
		rct_scenery_entry* scenery_entry = g_smallSceneryEntries[map_element->properties.scenery.type];

		// If can't repaint
		if (!(scenery_entry->small_scenery.flags &
			(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR |
			SMALL_SCENERY_FLAG10)))
			return;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 3103;
		game_do_command(
			grid_x,
			1 | (map_element->type << 8),
			grid_y,
			map_element->base_height | (map_element->properties.scenery.type << 8),
			GAME_COMMAND_SET_SCENERY_COLOUR,
			0,
			window_scenery_primary_colour | (window_scenery_secondary_colour << 8));
		break;
	}
	case VIEWPORT_INTERACTION_ITEM_WALL:
	{
		rct_scenery_entry* scenery_entry = g_wallSceneryEntries[map_element->properties.fence.type];

		// If can't repaint
		if (!(scenery_entry->wall.flags &
			(WALL_SCENERY_FLAG1 |
			WALL_SCENERY_FLAG2)))
			return;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 3103;
		game_do_command(
			grid_x,
			1 | (window_scenery_primary_colour << 8),
			grid_y,
			(map_element->type & MAP_ELEMENT_DIRECTION_MASK) | (map_element->base_height << 8),
			GAME_COMMAND_SET_FENCE_COLOUR,
			0,
			window_scenery_secondary_colour | (window_scenery_tertiary_colour << 8));
		break;
	}
	case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
	{
		rct_scenery_entry* scenery_entry = g_largeSceneryEntries[map_element->properties.scenerymultiple.type & MAP_ELEMENT_LARGE_TYPE_MASK];

		// If can't repaint
		if (!(scenery_entry->large_scenery.flags &
			(1 << 0)))
			return;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 3103;
		game_do_command(
			grid_x,
			1 | ((map_element->type & MAP_ELEMENT_DIRECTION_MASK) << 8),
			grid_y,
			map_element->base_height | ((map_element->properties.scenerymultiple.type >> 10) << 8),
			GAME_COMMAND_SET_LARGE_SCENERY_COLOUR,
			0,
			window_scenery_primary_colour | (window_scenery_secondary_colour << 8));
		break;
	}
	case VIEWPORT_INTERACTION_ITEM_BANNER:
	{
		rct_banner* banner = &gBanners[map_element->properties.banner.index];
		rct_scenery_entry* scenery_entry = g_bannerSceneryEntries[banner->type];

		// If can't repaint
		if (!(scenery_entry->banner.flags &
			(1 << 0)))
			return;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 3103;
		game_do_command(
			grid_x,
			1,
			grid_y,
			map_element->base_height | ((map_element->properties.banner.position & 0x3) << 8),
			GAME_COMMAND_SET_BANNER_COLOUR,
			0,
			window_scenery_primary_colour | (window_scenery_secondary_colour << 8));
		break;
	}
	default:
		return;
	}
}

/* rct2: 0x006E1F34 
 * Outputs
 * eax : grid_x
 * ebx : parameter_1
 * ecx : grid_y
 * edx : parameter_2
 * edi : parameter_3
 */
void sub_6E1F34(sint16 x, sint16 y, uint16 selected_scenery, sint16* grid_x, sint16* grid_y, uint32* parameter_1, uint32* parameter_2, uint32* parameter_3){
	rct_window* w = window_find_by_class(WC_SCENERY);

	if (w == NULL)
	{
		*grid_x = 0x8000;
		return;
	}

	uint8 scenery_type = selected_scenery >> 8;
	// Not sure what this type is yet.
	uint8 type = 0;

	if (scenery_type == 0){
		rct_scenery_entry* scenery_entry = g_smallSceneryEntries[selected_scenery];

		if (scenery_entry->small_scenery.flags & SMALL_SCENERY_FLAG18){
			type = 1;
		}
	}
	else if (scenery_type == 2 || scenery_type == 3){
		type = 1;
	}

	if (type == 0){
		RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_CTRL_PRESSED, uint8) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) = 0;
	}
	else{
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_CTRL_PRESSED, uint8) == 0){
			// CTRL pressed
			if (RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & (1 << 1)){
				rct_map_element* map_element;
				uint16 flags =
					VIEWPORT_INTERACTION_MASK_TERRAIN &
					VIEWPORT_INTERACTION_MASK_RIDE &
					VIEWPORT_INTERACTION_MASK_SCENERY &
					VIEWPORT_INTERACTION_MASK_FOOTPATH &
					VIEWPORT_INTERACTION_MASK_WALL &
					VIEWPORT_INTERACTION_MASK_LARGE_SCENERY;
				int interaction_type;
				get_map_coordinates_from_pos(x, y, flags, NULL, NULL, &interaction_type, &map_element, NULL);

				if (interaction_type != VIEWPORT_INTERACTION_ITEM_NONE){
					RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_CTRL_PRESSED, uint8) = 1;
					RCT2_GLOBAL(RCT2_ADDRESS_CTRL_PRESS_Z_COORDINATE, uint16) = map_element->base_height * 8;
				}
			}
		}
		else{
			// CTRL not pressed
			if (!(RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & (1 << 1))){
				RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_CTRL_PRESSED, uint8) = 0;
			}
		}

		if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) == 0){
			// SHIFT pressed
			if (RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & (1 << 0)){
				RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) = 1;
				RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_X_COORDINATE, uint16) = x;
				RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Y_COORDINATE, uint16) = y;
				RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Z_VECTOR, uint16) = 0;
			}
		}
		else{
			// SHIFT pressed
			if (RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & (1 << 0)){
				RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Z_VECTOR, sint16) =
					(RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Y_COORDINATE, sint16) - y + 4) & 0xFFF8;

				x = RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_X_COORDINATE, sint16);
				y = RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Y_COORDINATE, sint16);
			}
			else{
				// SHIFT not pressed
				RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) = 0;
			}
		}
	}
	
	switch (scenery_type){
	case 0:
	{
		// Small scenery
		rct_scenery_entry* scenery = g_smallSceneryEntries[selected_scenery];
		if (!(scenery->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE)){
			uint8 cl = 0;

			// If CTRL not pressed
			if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_CTRL_PRESSED, uint8) == 0){
				screen_get_map_xy_quadrant(x, y, grid_x, grid_y, &cl);

				if (*grid_x == (sint16)0x8000)
					return;

				RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = 0;

				// If SHIFT pressed
				if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){

					rct_map_element* map_element = map_get_surface_element_at(*grid_x / 32, *grid_y / 32);

					if (map_element == NULL){
						*grid_x = 0x8000;
						return;
					}

					sint16 z = (map_element->base_height * 8) & 0xFFF0;
					z += RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Z_VECTOR, sint16);

					if (z < 16){
						z = 16;
					}

					RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = z;
				}
			}
			else{
				sint16 z = RCT2_GLOBAL(RCT2_ADDRESS_CTRL_PRESS_Z_COORDINATE, sint16);

				screen_get_map_xy_quadrant_with_z(x, y, z, grid_x, grid_y, &cl);

				// If SHIFT pressed
				if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
					z += RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Z_VECTOR, sint16);
				}

				if (z < 16){
					z = 16;
				}

				RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = z;
			}

			if (*grid_x == (sint16)0x8000)
				return;

			uint8 rotation = window_scenery_rotation;

			if (!(scenery->small_scenery.flags & SMALL_SCENERY_FLAG4)){
				rotation = scenario_rand() & 0xFF;
			}

			rotation -= RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8);
			rotation &= 0x3;

			// Also places it in lower but think thats for clobering
			*parameter_1 = (selected_scenery & 0xFF) << 8;
			*parameter_2 = cl ^ (1 << 1) | (window_scenery_primary_colour << 8);
			*parameter_3 = rotation | (window_scenery_secondary_colour << 16);
			return;
		}

		// If CTRL not pressed
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_CTRL_PRESSED, uint8) == 0){
			uint16 flags = 
				VIEWPORT_INTERACTION_MASK_TERRAIN &
				VIEWPORT_INTERACTION_MASK_WATER;
			int interaction_type = 0;
			rct_map_element* map_element;

			get_map_coordinates_from_pos(x, y, flags, grid_x, grid_y, &interaction_type, &map_element, NULL);

			if (interaction_type == VIEWPORT_INTERACTION_ITEM_NONE)
			{
				*grid_x = 0x8000;
				return;
			}

			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = 0;
			uint16 water_height = map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
			if (water_height != 0){
				RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = water_height * 16;
			}

			// If SHIFT pressed
			if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
				rct_map_element* map_element = map_get_surface_element_at(*grid_x / 32, *grid_y / 32);

				if (map_element == NULL){
					*grid_x = 0x8000;
					return;
				}

				sint16 z = (map_element->base_height * 8) & 0xFFF0;
				z += RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Z_VECTOR, sint16);

				if (z < 16){
					z = 16;
				}

				RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = z;
			}
		}
		else{
			sint16 z = RCT2_GLOBAL(RCT2_ADDRESS_CTRL_PRESS_Z_COORDINATE, sint16);
			screen_get_map_xy_with_z(x, y, z, grid_x, grid_y);

			// If SHIFT pressed
			if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
				z += RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Z_VECTOR, sint16);
			}

			if (z < 16){
				z = 16;
			}

			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = z;
		}

		if (*grid_x == (sint16)0x8000)
			return;

		*grid_x &= 0xFFE0;
		*grid_y &= 0xFFE0;
		uint8 rotation = window_scenery_rotation;

		if (!(scenery->small_scenery.flags & SMALL_SCENERY_FLAG4)){
			rotation = scenario_rand() & 0xFF;
		}

		rotation -= RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8);
		rotation &= 0x3;

		// Also places it in lower but think thats for clobering
		*parameter_1 = (selected_scenery & 0xFF) << 8;
		*parameter_2 = 0 | (window_scenery_primary_colour << 8);
		*parameter_3 = rotation | (window_scenery_secondary_colour << 16);
		break;
	}
	case 1:
	{
		// Path bits

		uint16 flags = 
			VIEWPORT_INTERACTION_MASK_FOOTPATH &
			VIEWPORT_INTERACTION_MASK_FOOTPATH_ITEM;
		int interaction_type = 0;
		rct_map_element* map_element;

		get_map_coordinates_from_pos(x, y, flags, grid_x, grid_y, &interaction_type, &map_element, NULL);

		if (interaction_type == VIEWPORT_INTERACTION_ITEM_NONE)
		{
			*grid_x = 0x8000;
			return;
		}

		*parameter_1 = 0 | ((map_element->properties.path.type & 0x7) << 8);
		*parameter_2 = map_element->base_height | ((map_element->properties.path.type >> 4) << 8);
		if (map_element->type & 1){
			*parameter_2 |= 0x8000;
		}
		*parameter_3 = (selected_scenery & 0xFF) + 1;
		break;
	}
	case 2:
	{
		// Walls
		uint8 cl;
		// If CTRL not pressed
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_CTRL_PRESSED, uint8) == 0){
			screen_get_map_xy_side(x, y, grid_x, grid_y, &cl);

			if (*grid_x == (sint16)0x8000)
				return;

			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = 0;

			// If SHIFT pressed
			if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
				rct_map_element* map_element = map_get_surface_element_at(*grid_x / 32, *grid_y / 32);

				if (map_element == NULL){
					*grid_x = 0x8000;
					return;
				}

				sint16 z = (map_element->base_height * 8) & 0xFFF0;
				z += RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Z_VECTOR, sint16);

				if (z < 16){
					z = 16;
				}

				RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = z;
			}
		}
		else{
			sint16 z = RCT2_GLOBAL(RCT2_ADDRESS_CTRL_PRESS_Z_COORDINATE, sint16);
			screen_get_map_xy_side_with_z(x, y, z, grid_x, grid_y, &cl);

			// If SHIFT pressed
			if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
				z += RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Z_VECTOR, sint16);
			}

			if (z < 16){
				z = 16;
			}

			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = z;
		}

		if (*grid_x == (sint16)0x8000)
			return;

		RCT2_GLOBAL(0x00F64F15, uint8) = window_scenery_secondary_colour;
		RCT2_GLOBAL(0x00F64F16, uint8) = window_scenery_tertiary_colour;
		// Also places it in lower but think thats for clobering
		*parameter_1 = (selected_scenery & 0xFF) << 8;
		*parameter_2 = cl | (window_scenery_primary_colour << 8);
		*parameter_3 = 0;
		break;
	}
	case 3:
	{
		// Large scenery

		// If CTRL not pressed
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_CTRL_PRESSED, uint8) == 0){
			sub_68A15E(x, y, grid_x, grid_y, NULL, NULL);

			if (*grid_x == (sint16)0x8000)
				return;

			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = 0;

			// If SHIFT pressed
			if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
				rct_map_element* map_element = map_get_surface_element_at(*grid_x / 32, *grid_y / 32);

				if (map_element == NULL){
					*grid_x = 0x8000;
					return;
				}

				sint16 z = (map_element->base_height * 8) & 0xFFF0;
				z += RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Z_VECTOR, sint16);

				if (z < 16){
					z = 16;
				}

				RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = z;
			}
		}
		else{
			sint16 z = RCT2_GLOBAL(RCT2_ADDRESS_CTRL_PRESS_Z_COORDINATE, sint16);
			screen_get_map_xy_with_z(x, y, z, grid_x, grid_y);

			// If SHIFT pressed
			if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
				z += RCT2_GLOBAL(RCT2_ADDRESS_SHIFT_PRESS_Z_VECTOR, sint16);
			}

			if (z < 16){
				z = 16;
			}

			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = z;
		}

		if (*grid_x == (sint16)0x8000)
			return;

		*grid_x &= 0xFFE0;
		*grid_y &= 0xFFE0;

		uint8 rotation = window_scenery_rotation;
		rotation -= RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8);
		rotation &= 0x3;

		*parameter_1 = (rotation << 8);
		*parameter_2 = window_scenery_primary_colour | (window_scenery_secondary_colour << 8);
		*parameter_3 = selected_scenery & 0xFF;
		break;
	}
	case 4:
	{
		// Banner

		uint16 flags =
			VIEWPORT_INTERACTION_MASK_FOOTPATH &
			VIEWPORT_INTERACTION_MASK_FOOTPATH_ITEM;
		int interaction_type = 0;
		rct_map_element* map_element;

		get_map_coordinates_from_pos(x, y, flags, grid_x, grid_y, &interaction_type, &map_element, NULL);

		if (interaction_type == VIEWPORT_INTERACTION_ITEM_NONE)
		{
			*grid_x = 0x8000;
			return;
		}

		uint8 rotation = window_scenery_rotation;
		rotation -= RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8);
		rotation &= 0x3;
		
		sint16 z = map_element->base_height;

		if (map_element->properties.path.type & (1 << 2)){
			if (rotation != ((map_element->properties.path.type & 3) ^ 2)){
				z += 2;
			}
		}

		z /= 2;

		// Also places it in lower but think thats for clobering
		*parameter_1 = (selected_scenery & 0xFF) << 8;
		*parameter_2 = z | (rotation << 8);
		*parameter_3 = window_scenery_primary_colour;
		break;
	}
	}
}

/**
 * rct2: 0x6e2cc6
 */
static void window_top_toolbar_scenery_tool_down(short x, short y, rct_window* w, short widgetIndex){
	scenery_remove_ghost_tool_placement();
	if (window_scenery_is_repaint_scenery_tool_on & 1){
		repaint_scenery_tool_down(x, y, widgetIndex);
		return;
	}

	int selected_tab = window_scenery_selected_scenery_by_tab[window_scenery_active_tab_index];
	uint8 scenery_type = (selected_tab & 0xFF00) >> 8;
	uint8 selected_scenery = selected_tab & 0xFF;

	if (selected_tab == -1) return;

	sint16 grid_x, grid_y;
	int ebp = selected_tab;
	uint32 parameter_1, parameter_2, parameter_3;

	sub_6E1F34(x, y, selected_tab, &grid_x, &grid_y, &parameter_1, &parameter_2, &parameter_3);

	if (grid_x == (sint16)0x8000)return;
	
	switch (scenery_type){
	case 0:
	{
		int cluster_size = 1;
		if (window_scenery_is_build_cluster_tool_on){
			cluster_size = 35;
		}

		for (; cluster_size > 0; cluster_size--){

			int cluster_z_coordinate = RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16);
			rct_scenery_entry* scenery = g_smallSceneryEntries[(parameter_1 >> 8) & 0xFF];

			sint16 cur_grid_x = grid_x;
			sint16 cur_grid_y = grid_y;

			if (window_scenery_is_build_cluster_tool_on){
				if (!(scenery->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE)){
					parameter_2 &= 0xFF00;
					parameter_2 |= scenario_rand() & 3;
				}

				cur_grid_x += ((scenario_rand() % 16) - 8) * 32;
				cur_grid_y += ((scenario_rand() % 16) - 8) * 32;

				if (!(scenery->small_scenery.flags & SMALL_SCENERY_FLAG4)){
					RCT2_GLOBAL(0x00F64EC0, uint16)++;
					RCT2_GLOBAL(0x00F64EC0, uint16) &= 3;
				}
			}

			uint8 bl = 1;
			if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) != 0 &&
				RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
				bl = 20;
			}
			uint8 success = 0;

			for (; bl != 0; bl--){
				RCT2_GLOBAL(0x009A8C29, uint8) |= 1;

				int ebx = parameter_1;
				ebx &= 0xFF00;
				if (window_scenery_is_build_cluster_tool_on){
					ebx |= 0x9;
				}
				else{
					ebx |= GAME_COMMAND_FLAG_APPLY;
				}

				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1161;

				int cost = game_do_command(cur_grid_x, ebx, cur_grid_y, parameter_2, GAME_COMMAND_PLACE_SCENERY, RCT2_GLOBAL(0x00F64EC0, uint8) | (parameter_3 & 0xFFFF0000), RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16));


				RCT2_GLOBAL(0x009A8C29, uint8) &= ~1;

				if (cost != MONEY32_UNDEFINED){
					window_close_by_class(WC_ERROR);
					sound_play_panned(SOUND_PLACE_ITEM, 0x8001, RCT2_GLOBAL(0x009DEA5E, uint16), RCT2_GLOBAL(0x009DEA60, uint16), RCT2_GLOBAL(0x009DEA62, uint16));
					success = 1;
					break;
				}

				if (RCT2_GLOBAL(0x00141E9AC, rct_string_id) == 827 ||
					RCT2_GLOBAL(0x00141E9AC, rct_string_id) == 1032){
					break;
				}

				RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) += 8;
			}

			if (!success && !window_scenery_is_build_cluster_tool_on){
				sound_play_panned(SOUND_ERROR, 0x8001, RCT2_GLOBAL(0x009DEA5E, uint16), RCT2_GLOBAL(0x009DEA60, uint16), RCT2_GLOBAL(0x009DEA62, uint16));
				return;
			}

			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) = cluster_z_coordinate;
		}
		break;
	}
	case 1:
	{
		// Path Bits
		int ebx = parameter_1;
		ebx &= 0xFF00;
		ebx |= 0x81;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1161;

		int cost = game_do_command(grid_x, ebx, grid_y, parameter_2, GAME_COMMAND_PLACE_PATH, parameter_3, 0);

		if (cost == MONEY32_UNDEFINED){
			return;
		}

		sound_play_panned(SOUND_PLACE_ITEM, 0x8001, RCT2_GLOBAL(0x009DEA5E, uint16), RCT2_GLOBAL(0x009DEA60, uint16), RCT2_GLOBAL(0x009DEA62, uint16));
		break;
	}
	case 2:
	{
		// Walls
		uint8 bl = 1;
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) != 0 &&
			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
			bl = 20;
		}

		for (; bl != 0; bl--){
			RCT2_GLOBAL(0x009A8C29, uint8) |= 1;

			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1811;

			int ebx = (parameter_1 & 0xFF00) | 1;

			int cost = game_do_command(grid_x, ebx, grid_y, parameter_2, GAME_COMMAND_PLACE_FENCE, RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16), RCT2_GLOBAL(0x00F64F15, uint16));

			RCT2_GLOBAL(0x009A8C29, uint8) &= ~1;

			if (cost != MONEY32_UNDEFINED){
				window_close_by_class(WC_ERROR);
				sound_play_panned(SOUND_PLACE_ITEM, 0x8001, RCT2_GLOBAL(0x009DEA5E, uint16), RCT2_GLOBAL(0x009DEA60, uint16), RCT2_GLOBAL(0x009DEA62, uint16));
				return;
			}

			if (RCT2_GLOBAL(0x00141E9AC, rct_string_id) == 827 ||
				RCT2_GLOBAL(0x00141E9AC, rct_string_id) == 1032){
				break;
			}

			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) += 8;
		}

		sound_play_panned(SOUND_ERROR, 0x8001, RCT2_GLOBAL(0x009DEA5E, uint16), RCT2_GLOBAL(0x009DEA60, uint16), RCT2_GLOBAL(0x009DEA62, uint16));
		break;
	}
	case 3:
	{
		// Large Scenery
		uint8 bl = 1;
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) != 0 &&
			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
			bl = 20;
		}

		for (; bl != 0; bl--){
			RCT2_GLOBAL(0x009A8C29, uint8) |= 1;

			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1161;

			int ebx = (parameter_1 & 0xFF00) | 1;

			int cost = game_do_command(grid_x, ebx, grid_y, parameter_2, GAME_COMMAND_PLACE_LARGE_SCENERY, parameter_3, RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16));


			RCT2_GLOBAL(0x009A8C29, uint8) &= ~1;

			if (cost != MONEY32_UNDEFINED){
				window_close_by_class(WC_ERROR);
				sound_play_panned(SOUND_PLACE_ITEM, 0x8001, RCT2_GLOBAL(0x009DEA5E, uint16), RCT2_GLOBAL(0x009DEA60, uint16), RCT2_GLOBAL(0x009DEA62, uint16));
				return;
			}

			if (RCT2_GLOBAL(0x00141E9AC, rct_string_id) == 827 ||
				RCT2_GLOBAL(0x00141E9AC, rct_string_id) == 1032){
				break;
			}

			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) += 8;
		}

		sound_play_panned(SOUND_ERROR, 0x8001, RCT2_GLOBAL(0x009DEA5E, uint16), RCT2_GLOBAL(0x009DEA60, uint16), RCT2_GLOBAL(0x009DEA62, uint16));
	}
		break;
	case 4:
	{
		// Banners
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1161;

		// The return value will be banner id but the input is colour (param 3)
		int banner_id = parameter_3;

		int cost;
		{
			int esi = 0, eax = grid_x, ecx = grid_y, edx = parameter_2, ebx = (parameter_1 & 0xFF00) | 1;
			cost = game_do_command_p(GAME_COMMAND_PLACE_BANNER, &eax, &ebx, &ecx, &edx, &esi, &banner_id, &ebp); 
		}

		if (cost == MONEY32_UNDEFINED)return;

		sound_play_panned(SOUND_PLACE_ITEM, 0x8001, RCT2_GLOBAL(0x009DEA5E, uint16), RCT2_GLOBAL(0x009DEA60, uint16), RCT2_GLOBAL(0x009DEA62, uint16));

		window_banner_open(banner_id);
		break;
	}
	}
}

/**
*
*  rct2: 0x0068E213
*/
void top_toolbar_tool_update_scenery_clear(sint16 x, sint16 y){
	map_invalidate_selection_rect();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);

	rct_xy16 mapTile = { 0 };
	screen_get_map_xy(x, y, &mapTile.x, &mapTile.y, NULL);

	if (mapTile.x == (sint16)0x8000){
		if (RCT2_GLOBAL(0x00F1AD62, money32) != MONEY32_UNDEFINED){
			RCT2_GLOBAL(0x00F1AD62, money32) = MONEY32_UNDEFINED;
			window_invalidate_by_class(WC_CLEAR_SCENERY);
		}
		return;
	}

	uint8 state_changed = 0;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & (1 << 0))){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
		state_changed++;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) != 4){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		state_changed++;
	}

	sint16 tool_size = RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16);
	if (tool_size == 0)
		tool_size = 1;

	sint16 tool_length = (tool_size - 1) * 32;

	// Move to tool bottom left
	mapTile.x -= (tool_size - 1) * 16;
	mapTile.y -= (tool_size - 1) * 16;
	mapTile.x &= 0xFFE0;
	mapTile.y &= 0xFFE0;

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) != mapTile.x){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = mapTile.x;
		state_changed++;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) != mapTile.y){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = mapTile.y;
		state_changed++;
	}

	mapTile.x += tool_length;
	mapTile.y += tool_length;

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) != mapTile.x){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = mapTile.x;
		state_changed++;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) != mapTile.y){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = mapTile.y;
		state_changed++;
	}

	map_invalidate_selection_rect();
	if (!state_changed)
		return;

	money32 cost = map_clear_scenery(
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16),
		0
	);

	if (RCT2_GLOBAL(0x00F1AD62, money32) != cost){
		RCT2_GLOBAL(0x00F1AD62, money32) = cost;
		window_invalidate_by_class(WC_CLEAR_SCENERY);
		return;
	}
}

void top_toolbar_tool_update_land_paint(sint16 x, sint16 y){
	map_invalidate_selection_rect();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);

	rct_xy16 mapTile = { 0 };
	screen_get_map_xy(x, y, &mapTile.x, &mapTile.y, NULL);

	if (mapTile.x == (sint16)0x8000){
		if (RCT2_GLOBAL(0x00F1AD62, money32) != MONEY32_UNDEFINED){
			RCT2_GLOBAL(0x00F1AD62, money32) = MONEY32_UNDEFINED;
			window_invalidate_by_class(WC_CLEAR_SCENERY);
		}
		return;
	}

	uint8 state_changed = 0;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & (1 << 0))){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
		state_changed++;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) != 4){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		state_changed++;
	}

	sint16 tool_size = RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16);
	if (tool_size == 0)
		tool_size = 1;

	sint16 tool_length = (tool_size - 1) * 32;

	// Move to tool bottom left
	mapTile.x -= (tool_size - 1) * 16;
	mapTile.y -= (tool_size - 1) * 16;
	mapTile.x &= 0xFFE0;
	mapTile.y &= 0xFFE0;

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) != mapTile.x){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = mapTile.x;
		state_changed++;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) != mapTile.y){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = mapTile.y;
		state_changed++;
	}

	mapTile.x += tool_length;
	mapTile.y += tool_length;

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) != mapTile.x){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = mapTile.x;
		state_changed++;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) != mapTile.y){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = mapTile.y;
		state_changed++;
	}

	map_invalidate_selection_rect();
	if (!state_changed)
		return;
}

/**
*
*  rct2: 0x00664280
*/
void top_toolbar_tool_update_land(sint16 x, sint16 y){
	map_invalidate_selection_rect();

	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) == 3){
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & (1 << 0)))
			return;

		money32 lower_cost = selection_lower_land(0);
		money32 raise_cost = selection_raise_land(0);

		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, money32) != raise_cost || 
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, money32) != lower_cost){
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, money32) = raise_cost;
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, money32) = lower_cost;
			window_invalidate_by_class(WC_LAND);
		}
		return;
	}

	sint16 tool_size = RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16);
	rct_xy16 mapTile = { .x = x, .y = y };

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);
	if (tool_size == 1){
		int direction;
		screen_pos_to_map_pos(&mapTile.x, &mapTile.y, &direction);

		if (mapTile.x == (sint16)0x8000){
			money32 lower_cost = MONEY32_UNDEFINED;
			money32 raise_cost = MONEY32_UNDEFINED;

			if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, money32) != raise_cost ||
				RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, money32) != lower_cost){
				RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, money32) = raise_cost;
				RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, money32) = lower_cost;
				window_invalidate_by_class(WC_LAND);
			}
			return;
		}

		uint8 state_changed = 0;

		if (!(RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & (1 << 0))){
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
			state_changed++;
		}

		if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) != direction){
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = direction;
			state_changed++;
		}


		if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) != mapTile.x){
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = mapTile.x;
			state_changed++;
		}

		if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) != mapTile.y){
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = mapTile.y;
			state_changed++;
		}

		if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) != mapTile.x){
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = mapTile.x;
			state_changed++;
		}

		if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) != mapTile.y){
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = mapTile.y;
			state_changed++;
		}

		map_invalidate_selection_rect();
		if (!state_changed)
			return;

		money32 lower_cost = selection_lower_land(0);
		money32 raise_cost = selection_raise_land(0);

		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, money32) != raise_cost ||
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, money32) != lower_cost){
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, money32) = raise_cost;
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, money32) = lower_cost;
			window_invalidate_by_class(WC_LAND);
		}
		return;
	}

	screen_get_map_xy(x, y, &mapTile.x, &mapTile.y, NULL);

	if (mapTile.x == (sint16)0x8000){
		money32 lower_cost = MONEY32_UNDEFINED;
		money32 raise_cost = MONEY32_UNDEFINED;

		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, money32) != raise_cost ||
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, money32) != lower_cost){
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, money32) = raise_cost;
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, money32) = lower_cost;
			window_invalidate_by_class(WC_LAND);
		}
		return;
	}

	uint8 state_changed = 0;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & (1 << 0))){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
		state_changed++;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) != 4){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		state_changed++;
	}

	
	if (tool_size == 0)
		tool_size = 1;

	sint16 tool_length = (tool_size - 1) * 32;

	// Move to tool bottom left
	mapTile.x -= (tool_size - 1) * 16;
	mapTile.y -= (tool_size - 1) * 16;
	mapTile.x &= 0xFFE0;
	mapTile.y &= 0xFFE0;

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) != mapTile.x){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = mapTile.x;
		state_changed++;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) != mapTile.y){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = mapTile.y;
		state_changed++;
	}

	mapTile.x += tool_length;
	mapTile.y += tool_length;

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) != mapTile.x){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = mapTile.x;
		state_changed++;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) != mapTile.y){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = mapTile.y;
		state_changed++;
	}

	map_invalidate_selection_rect();
	if (!state_changed)
		return;

	money32 lower_cost = selection_lower_land(0);
	money32 raise_cost = selection_raise_land(0);

	if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, money32) != raise_cost ||
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, money32) != lower_cost){
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, money32) = raise_cost;
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, money32) = lower_cost;
		window_invalidate_by_class(WC_LAND);
	}
}

/**
*
*  rct2: 0x006E6BDC
*/
void top_toolbar_tool_update_water(sint16 x, sint16 y){
	map_invalidate_selection_rect();

	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) == 3){
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & (1 << 0)))
			return;

		money32 lower_cost = lower_water(
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16),
			0);

		money32 raise_cost = raise_water(
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16),
			0);

		if (RCT2_GLOBAL(RCT2_ADDRESS_WATER_RAISE_COST, money32) != raise_cost ||
			RCT2_GLOBAL(RCT2_ADDRESS_WATER_LOWER_COST, money32) != lower_cost){
			RCT2_GLOBAL(RCT2_ADDRESS_WATER_RAISE_COST, money32) = raise_cost;
			RCT2_GLOBAL(RCT2_ADDRESS_WATER_LOWER_COST, money32) = lower_cost;
			window_invalidate_by_class(WC_WATER);
		}
		return;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);

	rct_xy16 mapTile = { 0 };
	int interaction_type = 0;
	get_map_coordinates_from_pos(
		x, 
		y, 
		VIEWPORT_INTERACTION_MASK_TERRAIN & VIEWPORT_INTERACTION_MASK_WATER, 
		&mapTile.x, 
		&mapTile.y, 
		&interaction_type,
		NULL,
		NULL);

	if (interaction_type == VIEWPORT_INTERACTION_ITEM_NONE){
		if (RCT2_GLOBAL(RCT2_ADDRESS_WATER_RAISE_COST, money32) != MONEY32_UNDEFINED ||
			RCT2_GLOBAL(RCT2_ADDRESS_WATER_LOWER_COST, money32) != MONEY32_UNDEFINED){
			RCT2_GLOBAL(RCT2_ADDRESS_WATER_RAISE_COST, money32) = MONEY32_UNDEFINED;
			RCT2_GLOBAL(RCT2_ADDRESS_WATER_LOWER_COST, money32) = MONEY32_UNDEFINED;
			window_invalidate_by_class(WC_WATER);
		}
		return;
	}

	mapTile.x += 16;
	mapTile.y += 16;

	uint8 state_changed = 0;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & (1 << 0))){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
		state_changed++;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) != 4){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		state_changed++;
	}

	sint16 tool_size = RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16);
	if (tool_size == 0)
		tool_size = 1;

	sint16 tool_length = (tool_size - 1) * 32;

	// Move to tool bottom left
	mapTile.x -= (tool_size - 1) * 16;
	mapTile.y -= (tool_size - 1) * 16;
	mapTile.x &= 0xFFE0;
	mapTile.y &= 0xFFE0;

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) != mapTile.x){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = mapTile.x;
		state_changed++;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) != mapTile.y){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = mapTile.y;
		state_changed++;
	}

	mapTile.x += tool_length;
	mapTile.y += tool_length;

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) != mapTile.x){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = mapTile.x;
		state_changed++;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) != mapTile.y){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = mapTile.y;
		state_changed++;
	}

	map_invalidate_selection_rect();
	if (!state_changed)
		return;

	money32 lower_cost = lower_water(
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16),
		0);

	money32 raise_cost = raise_water(
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16),
		0);

	if (RCT2_GLOBAL(RCT2_ADDRESS_WATER_RAISE_COST, money32) != raise_cost ||
		RCT2_GLOBAL(RCT2_ADDRESS_WATER_LOWER_COST, money32) != lower_cost){
		RCT2_GLOBAL(RCT2_ADDRESS_WATER_RAISE_COST, money32) = raise_cost;
		RCT2_GLOBAL(RCT2_ADDRESS_WATER_LOWER_COST, money32) = lower_cost;
		window_invalidate_by_class(WC_WATER);
	}
}

/* rct2: 0x006E24F6 
 * On failure returns MONEY32_UNDEFINED
 * On success places ghost scenery and returns cost to place proper
 */
money32 try_place_ghost_scenery(rct_xy16 map_tile, uint32 parameter_1, uint32 parameter_2, uint32 parameter_3, uint16 selected_tab){
	scenery_remove_ghost_tool_placement();

	uint8 scenery_type = (selected_tab & 0xFF00) >> 8;
	uint8 selected_scenery = selected_tab & 0xFF;
	money32 cost = 0;
	rct_map_element* mapElement;

	switch (scenery_type){
	case 0:
		// Small Scenery
		//6e252b
		cost = game_do_command(
			map_tile.x, 
			parameter_1 | 0x69,
			map_tile.y, 
			parameter_2, 
			GAME_COMMAND_PLACE_SCENERY, 
			parameter_3, 
			RCT2_GLOBAL(0x00F64ED4, sint16));

		if (cost == MONEY32_UNDEFINED)
			return cost;

		RCT2_GLOBAL(0x00F64EC4, sint16) = map_tile.x;
		RCT2_GLOBAL(0x00F64EC6, sint16) = map_tile.y;
		RCT2_GLOBAL(0x00F64EC0, uint16) = (uint16)(parameter_3 & 0xFFFF);
		RCT2_GLOBAL(0x00F64EDA, sint16) = selected_tab;
		
		mapElement = RCT2_GLOBAL(0x00F64EBC, rct_map_element*);
		RCT2_GLOBAL(0x00F64F09, uint8) = mapElement->base_height;
		RCT2_GLOBAL(0x00F64F0C, uint8) = mapElement->type;
		if (RCT2_GLOBAL(0x00F64F14, uint8) & (1 << 1)){
			viewport_set_visibility(4);
		}
		else{
			viewport_set_visibility(5);
		}

		RCT2_GLOBAL(0x00F64F0D, uint8) |= (1 << 0);
		break;
	case 1:
		// Path Bits
		//6e265b
		cost = game_do_command(
			map_tile.x,
			parameter_1 | 0xE9,
			map_tile.y,
			parameter_2,
			GAME_COMMAND_PLACE_PATH,
			parameter_3,
			0);

		if (cost == MONEY32_UNDEFINED)
			return cost;

		RCT2_GLOBAL(0x00F64EC4, sint16) = map_tile.x;
		RCT2_GLOBAL(0x00F64EC6, sint16) = map_tile.y;
		RCT2_GLOBAL(0x00F64F09, uint8) = (parameter_2 & 0xFF);
		RCT2_GLOBAL(0x00F64F0F, uint8) = ((parameter_1 >> 8) & 0xFF);
		RCT2_GLOBAL(0x00F64F10, uint8) = ((parameter_2 >> 8) & 0xFF);
		RCT2_GLOBAL(0x00F64EAC, uint32) = parameter_3;

		RCT2_GLOBAL(0x00F64F0D, uint8) |= (1 << 1);
		break;
	case 2:
		// Walls
		//6e26b0
		cost = game_do_command(
			map_tile.x,
			parameter_1 | 0x69,
			map_tile.y,
			parameter_2,
			GAME_COMMAND_PLACE_FENCE,
			RCT2_GLOBAL(0x00F64ED4, uint16),
			RCT2_GLOBAL(0x00F64F15, uint16));

		if (cost == MONEY32_UNDEFINED)
			return cost;

		RCT2_GLOBAL(0x00F64EC4, sint16) = map_tile.x;
		RCT2_GLOBAL(0x00F64EC6, sint16) = map_tile.y;
		RCT2_GLOBAL(0x00F64F11, uint8) = (parameter_2 & 0xFF);
		
		mapElement = RCT2_GLOBAL(0x00F64EBC, rct_map_element*);
		RCT2_GLOBAL(0x00F64F09, uint8) = mapElement->base_height;

		RCT2_GLOBAL(0x00F64F0D, uint8) |= (1 << 2);
		break;
	case 3:
		// Large Scenery
		//6e25a7
		cost = game_do_command(
			map_tile.x,
			parameter_1 | 0x69,
			map_tile.y,
			parameter_2,
			GAME_COMMAND_PLACE_LARGE_SCENERY,
			parameter_3,
			RCT2_GLOBAL(0x00F64ED4, uint16));

		if (cost == MONEY32_UNDEFINED)
			return cost;

		RCT2_GLOBAL(0x00F64EC4, sint16) = map_tile.x;
		RCT2_GLOBAL(0x00F64EC6, sint16) = map_tile.y;
		RCT2_GLOBAL(0x00F64EC0, uint8) = ((parameter_1 >> 8) & 0xFF);

		mapElement = RCT2_GLOBAL(0x00F64EBC, rct_map_element*);
		RCT2_GLOBAL(0x00F64F09, uint8) = mapElement->base_height;

		if (RCT2_GLOBAL(0x00F64F14, uint8) & (1 << 1)){
			viewport_set_visibility(4);
		}
		else{
			viewport_set_visibility(5);
		}

		RCT2_GLOBAL(0x00F64F0D, uint8) |= (1 << 3);
		break;
	case 4:
		// Banners
		//6e2612
		cost = game_do_command(
			map_tile.x,
			parameter_1 | 0x69,
			map_tile.y,
			parameter_2,
			GAME_COMMAND_PLACE_BANNER,
			parameter_3,
			0);

		if (cost == MONEY32_UNDEFINED)
			return cost;

		RCT2_GLOBAL(0x00F64EC4, sint16) = map_tile.x;
		RCT2_GLOBAL(0x00F64EC6, sint16) = map_tile.y;
		RCT2_GLOBAL(0x00F64F09, uint8) = (parameter_2 & 0xFF);
		RCT2_GLOBAL(0x00F64EC0, uint8) = ((parameter_2 >> 8) & 0xFF);
		RCT2_GLOBAL(0x00F64F0D, uint8) |= (1 << 4);
		break;
	}

	return cost;
}

/**
*
*  rct2: 0x006E287B
*/
void top_toolbar_tool_update_scenery(sint16 x, sint16 y){
	map_invalidate_selection_rect();
	map_invalidate_map_selection_tiles();

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~((1 << 0) | (1 << 1));

	if (window_scenery_is_repaint_scenery_tool_on)
		return;

	sint16 selected_tab = window_scenery_selected_scenery_by_tab[window_scenery_active_tab_index];

	if (selected_tab == -1){
		scenery_remove_ghost_tool_placement();
		return;
	}

	uint8 scenery_type = (selected_tab & 0xFF00) >> 8;
	uint8 selected_scenery = selected_tab & 0xFF;
	rct_xy16 mapTile = { 0 };
	uint32 parameter1, parameter2, parameter3;

	sub_6E1F34(x, y, selected_tab, &mapTile.x, &mapTile.y, &parameter1, &parameter2, &parameter3);

	if (mapTile.x == (sint16)0x8000){
		scenery_remove_ghost_tool_placement();
		return;
	}
	
	rct_scenery_entry* scenery;
	uint8 bl;
	money32 cost;

	switch (scenery_type){
	case 0:
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = mapTile.x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = mapTile.y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = mapTile.x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = mapTile.y;

		scenery = g_smallSceneryEntries[selected_scenery];

		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		if (!(scenery->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE)){
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = ((parameter2 & 0xFF) ^ 2) + 6;
		}

		map_invalidate_selection_rect();

		// If no change in ghost placement
		if ((RCT2_GLOBAL(0x00F64F0D, uint8) & (1 << 0)) &&
			mapTile.x == RCT2_GLOBAL(0x00F64EC4, sint16) &&
			mapTile.y == RCT2_GLOBAL(0x00F64EC6, sint16) &&
			(parameter2 & 0xFF) == RCT2_GLOBAL(0x00F64F0E, uint8)&&
			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) == RCT2_GLOBAL(0x00F64F0A, sint16) &&
			RCT2_GLOBAL(0x00F64EDA, uint16) == selected_tab){
			return;		
		}

		scenery_remove_ghost_tool_placement();

		RCT2_GLOBAL(0x00F64F0E, uint8) = (parameter2 & 0xFF);
		RCT2_GLOBAL(0x00F64F0A, sint16) = RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16);

		bl = 1;
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) != 0 &&
			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
			bl = 20;
		}

		for (; bl != 0; bl--){
			cost = try_place_ghost_scenery(
				mapTile,
				parameter1,
				parameter2,
				parameter3,
				selected_tab);

			if (cost != MONEY32_UNDEFINED)
				break;
			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) += 8;
		}

		RCT2_GLOBAL(0x00F64EB4, money32) = cost;
		break;
	case 1:
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = mapTile.x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = mapTile.y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = mapTile.x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = mapTile.y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;

		map_invalidate_selection_rect();

		// If no change in ghost placement
		if ((RCT2_GLOBAL(0x00F64F0D, uint8) & (1 << 1)) &&
			mapTile.x == RCT2_GLOBAL(0x00F64EC4, sint16) &&
			mapTile.y == RCT2_GLOBAL(0x00F64EC6, sint16) &&
			(parameter2 & 0xFF) == RCT2_GLOBAL(0x00F64F09, uint8)){
			return;
		}

		scenery_remove_ghost_tool_placement();

		cost = try_place_ghost_scenery(
			mapTile,
			parameter1,
			parameter2,
			parameter3,
			selected_tab);

		RCT2_GLOBAL(0x00F64EB4, money32) = cost;
		break;
	case 2:
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = mapTile.x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = mapTile.y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = mapTile.x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = mapTile.y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 10 + parameter2 & 0xFF;

		map_invalidate_selection_rect();

		// If no change in ghost placement
		if ((RCT2_GLOBAL(0x00F64F0D, uint8) & (1 << 2)) &&
			mapTile.x == RCT2_GLOBAL(0x00F64EC4, sint16) &&
			mapTile.y == RCT2_GLOBAL(0x00F64EC6, sint16) &&
			(parameter2 & 0xFF) == RCT2_GLOBAL(0x00F64F11, uint8) &&
			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) == RCT2_GLOBAL(0x00F64F0A, sint16)
			){
			return;
		}

		scenery_remove_ghost_tool_placement();

		RCT2_GLOBAL(0x00F64F11, uint8) = (parameter2 & 0xFF);
		RCT2_GLOBAL(0x00F64F0A, sint16) = RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16);

		bl = 1;
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) != 0 &&
			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
			bl = 20;
		}

		cost = 0;
		for (; bl != 0; bl--){
			cost = try_place_ghost_scenery(
				mapTile,
				parameter1,
				parameter2,
				parameter3,
				selected_tab);

			if (cost != MONEY32_UNDEFINED)
				break;
			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) += 8;
		}

		RCT2_GLOBAL(0x00F64EB4, money32) = cost;
		break;
	case 3:
		scenery = g_largeSceneryEntries[selected_scenery];
		rct_xy16* selectedTile = gMapSelectionTiles;

		for (rct_large_scenery_tile* tile = scenery->large_scenery.tiles; tile->x_offset != (sint16)0xFFFF; tile++){
			rct_xy16 tileLocation = { 
				.x = tile->x_offset, 
				.y = tile->y_offset 
			};

			rotate_map_coordinates(&tileLocation.x, &tileLocation.y, (parameter1 >> 8) & 0xFF);

			tileLocation.x += mapTile.x;
			tileLocation.y += mapTile.y;

			selectedTile->x = tileLocation.x;
			selectedTile->y = tileLocation.y;
			selectedTile++;
		}
		selectedTile->x = 0xFFFF;

		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 1);
		map_invalidate_map_selection_tiles();

		// If no change in ghost placement
		if ((RCT2_GLOBAL(0x00F64F0D, uint8) & (1 << 3)) &&
			mapTile.x == RCT2_GLOBAL(0x00F64EC4, sint16) &&
			mapTile.y == RCT2_GLOBAL(0x00F64EC6, sint16) &&
			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) == RCT2_GLOBAL(0x00F64F0A, sint16) &&
			(parameter3 & 0xFFFF) == RCT2_GLOBAL(0x00F64EDA, uint16)){
			return;
		}

		scenery_remove_ghost_tool_placement();

		RCT2_GLOBAL(0x00F64EDA, uint16) = (parameter3 & 0xFFFF);
		RCT2_GLOBAL(0x00F64F0A, sint16) = RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16);

		bl = 1;
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) != 0 &&
			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_TOOL_SHIFT_PRESSED, uint8) != 0){
			bl = 20;
		}

		cost = 0;
		for (; bl != 0; bl--){
			cost = try_place_ghost_scenery(
				mapTile,
				parameter1,
				parameter2,
				parameter3,
				selected_tab);

			if (cost != MONEY32_UNDEFINED)
				break;
			RCT2_GLOBAL(RCT2_ADDRESS_SCENERY_Z_COORDINATE, sint16) += 8;
		}

		RCT2_GLOBAL(0x00F64EB4, money32) = cost;
		break;
	case 4:
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = mapTile.x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = mapTile.y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = mapTile.x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = mapTile.y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;

		map_invalidate_selection_rect();

		// If no change in ghost placement
		if ((RCT2_GLOBAL(0x00F64F0D, uint8) & (1 << 4)) &&
			mapTile.x == RCT2_GLOBAL(0x00F64EC4, sint16) &&
			mapTile.y == RCT2_GLOBAL(0x00F64EC6, sint16) &&
			(parameter2 & 0xFF) == RCT2_GLOBAL(0x00F64F09, uint8) &&
			((parameter2 >> 8) & 0xFF) == RCT2_GLOBAL(0x00F64EC0, uint8)){
			return;
		}

		scenery_remove_ghost_tool_placement();

		cost = try_place_ghost_scenery(
			mapTile,
			parameter1,
			parameter2,
			parameter3,
			selected_tab);

		RCT2_GLOBAL(0x00F64EB4, money32) = cost;
		break;
	}
}

/**
 *
 *  rct2: 0x0066CB25
 */
static void window_top_toolbar_tool_update(rct_window* w, int widgetIndex, int x, int y)
{
	switch (widgetIndex) {
	case WIDX_CLEAR_SCENERY:
		top_toolbar_tool_update_scenery_clear(x, y);
		break;
	case WIDX_LAND:
		if (LandPaintMode)
			top_toolbar_tool_update_land_paint(x, y);
		else
			top_toolbar_tool_update_land(x, y);
		break;
	case WIDX_WATER:
		top_toolbar_tool_update_water(x, y);
		break;
	case WIDX_SCENERY:
		top_toolbar_tool_update_scenery(x, y);
		break;
	}
}

/**
 * rct2: 0x0066CB73
 */
static void window_top_toolbar_tool_down(rct_window* w, int widgetIndex, int x, int y)
{
	switch (widgetIndex){
	case WIDX_CLEAR_SCENERY:
		if (!RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & (1 << 0))
			break;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 3438;

		game_do_command(
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16),
			1,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16),
			0,
			GAME_COMMAND_CLEAR_SCENERY,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16)
			);
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 12;
		break;
	case WIDX_LAND:
		if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16)&(1 << 0)){
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1387;
			game_do_command(
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16),
				1,
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16),
				RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) | (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) << 8),
				GAME_COMMAND_CHANGE_SURFACE_STYLE,
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16),
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16)
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
*  
*  rct2: 0x006644DD
*/
money32 selection_raise_land(uint8 flags)
{
	int centreX = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) + RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16)) / 2;
	int centreY = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) + RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16)) / 2;
	centreX += 16;
	centreY += 16;

	uint32 xBounds = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) & 0xFFFF) | (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) << 16);
	uint32 yBounds = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) & 0xFFFF) | (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) << 16);

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = STR_CANT_RAISE_LAND_HERE;
	if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) == 0) {
		return game_do_command(centreX, flags, centreY, xBounds, GAME_COMMAND_EDIT_LAND_SMOOTH, 1, yBounds);
	} else {
		return game_do_command(centreX, flags, centreY, xBounds, GAME_COMMAND_RAISE_LAND, RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16), yBounds);
	}
}

/**
*
*  rct2: 0x006645B3
*/
money32 selection_lower_land(uint8 flags)
{
	int centreX = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) + RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16)) / 2;
	int centreY = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) + RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16)) / 2;
	centreX += 16;
	centreY += 16;

	uint32 xBounds = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) & 0xFFFF) | (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) << 16);
	uint32 yBounds = (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) & 0xFFFF) | (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) << 16);

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = STR_CANT_LOWER_LAND_HERE;
	if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) == 0) {
		return game_do_command(centreX, flags, centreY, xBounds, GAME_COMMAND_EDIT_LAND_SMOOTH, 0xFFFF, yBounds);
	} else {
		return game_do_command(centreX, flags, centreY, xBounds, GAME_COMMAND_LOWER_LAND, RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16), yBounds);
	}
}

/**
*  part of window_top_toolbar_tool_drag(0x0066CB4E)
*  rct2: 0x00664454
*/
void window_top_toolbar_land_tool_drag(short x, short y)
{
	rct_window *window = window_find_from_point(x, y);
	if (!window)
		return;
	int widget_index = window_find_widget_from_point(window, x, y);
	if (widget_index == 0xFFFF)
		return;
	rct_widget *widget = &window->widgets[widget_index];
	if (widget->type != WWT_VIEWPORT)
		return;
	rct_viewport *viewport = window->viewport;
	if (!viewport)
		return;

	sint16 tile_height = -16 / (1 << viewport->zoom);

	int y_diff = y - RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, uint16);

	if (y_diff <= tile_height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, uint16) += tile_height;

		selection_raise_land(GAME_COMMAND_FLAG_APPLY);

		RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, uint32) = MONEY32_UNDEFINED;
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, uint32) = MONEY32_UNDEFINED;
		return;
	}

	if (y_diff >= -tile_height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, uint16) -= tile_height;

		selection_lower_land(GAME_COMMAND_FLAG_APPLY);

		RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, uint32) = MONEY32_UNDEFINED;
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, uint32) = MONEY32_UNDEFINED;

		return;
	}
}

/**
*  part of window_top_toolbar_tool_drag(0x0066CB4E)
*  rct2: 0x006E6D4B
*/
void window_top_toolbar_water_tool_drag(short x, short y)
{
	rct_window *window = window_find_from_point(x, y);
	if (!window)
		return;
	int widget_index = window_find_widget_from_point(window, x, y);
	if (widget_index == 0xFFFF)
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
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16),
			1,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16),
			dx,
			GAME_COMMAND_RAISE_WATER,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16)
			);
		RCT2_GLOBAL(RCT2_ADDRESS_WATER_RAISE_COST, uint32) = MONEY32_UNDEFINED;
		RCT2_GLOBAL(RCT2_ADDRESS_WATER_LOWER_COST, uint32) = MONEY32_UNDEFINED;

		return;
	}

	dx = -dx;

	if (y >= dx) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURSOR_DRAG_LAST_Y, uint16) += dx;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = STR_CANT_LOWER_WATER_LEVEL_HERE;

		game_do_command(
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16),
			1,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16),
			dx,
			GAME_COMMAND_LOWER_WATER,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16)
			);
		RCT2_GLOBAL(RCT2_ADDRESS_WATER_RAISE_COST, uint32) = MONEY32_UNDEFINED;
		RCT2_GLOBAL(RCT2_ADDRESS_WATER_LOWER_COST, uint32) = MONEY32_UNDEFINED;

		return;
	}
}

/**
 *
 *  rct2: 0x0066CB4E
 */
static void window_top_toolbar_tool_drag(rct_window* w, int widgetIndex, int x, int y)
{
	switch (widgetIndex){
	case WIDX_CLEAR_SCENERY:
		if (window_find_by_class(WC_ERROR) != NULL)
			break;

		if (!RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & (1 << 0))
			break;

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 3438;

		game_do_command(
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16),
			1,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16),
			0,
			GAME_COMMAND_CLEAR_SCENERY,
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16),
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16)
		);
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 12;
		break;
	case WIDX_LAND:
		// Custom setting to only change land style instead of raising or lowering land
		if (LandPaintMode) {
			if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16)&(1 << 0)){
				RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, rct_string_id) = 1387;
				game_do_command(
					RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16),
					1,
					RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16),
					RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) | (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) << 8),
					GAME_COMMAND_CHANGE_SURFACE_STYLE,
					RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16),
					RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16)
					);
				// The tool is set to 12 here instead of 3 so that the dragging cursor is not the elevation change cursor
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 12;
			}
		} else {
			window_top_toolbar_land_tool_drag(x, y);
		}
		break;
	case WIDX_WATER:
		window_top_toolbar_water_tool_drag(x, y);
		break;
	case WIDX_SCENERY:
		if (window_scenery_is_repaint_scenery_tool_on & 1)
			window_top_toolbar_scenery_tool_down(x, y, w, widgetIndex);
		break;
	}
}

/**
 * 
 *  rct2: 0x0066CC5B
 */
static void window_top_toolbar_tool_up(rct_window* w, int widgetIndex, int x, int y)
{	
	switch (widgetIndex) {
	case WIDX_LAND:
		map_invalidate_selection_rect();
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= 0xFFFE;
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 0x12;
		break;
	case WIDX_WATER:
		map_invalidate_selection_rect();
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= 0xFFFE;
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 0x13;
		break;
	case WIDX_CLEAR_SCENERY:
		map_invalidate_selection_rect();
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= 0xFFFE;
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 0x0C;
		break;
	}
}

/**
 * 
 *  rct2: 0x0066CA58
 */
static void window_top_toolbar_tool_abort(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_LAND:
	case WIDX_WATER:
	case WIDX_CLEAR_SCENERY:
		hide_gridlines();
		break;
	}
}

void top_toolbar_init_fastforward_menu(rct_window* w, rct_widget* widget) {
	int num_items = 4;
	gDropdownItemsFormat[0] = 1156;
	gDropdownItemsFormat[1] = 1156;
	gDropdownItemsFormat[2] = 1156;
	gDropdownItemsFormat[3] = 1156;
	if (gConfigGeneral.debugging_tools) {
		gDropdownItemsFormat[4] = 0;
		gDropdownItemsFormat[5] = 1156;
		gDropdownItemsArgs[5] = 5146;
		num_items = 6;
	}

	gDropdownItemsArgs[0] = 5142;
	gDropdownItemsArgs[1] = 5143;
	gDropdownItemsArgs[2] = 5144;
	gDropdownItemsArgs[3] = 5145;


	window_dropdown_show_text(
		w->x + widget->left,
		w->y + widget->top,
		widget->bottom - widget->top + 1,
		w->colours[0] | 0x80,
		0,
		num_items
		);

	// Set checkmarks
	if (gGameSpeed <= 4)
		gDropdownItemsChecked |= (1 << (gGameSpeed - 1));
	if (gGameSpeed == 8)
		gDropdownItemsChecked |= (1 << 5);

	if (gConfigGeneral.debugging_tools)
		RCT2_GLOBAL(0x9DEBA2, uint16) = (gGameSpeed == 8 ? 0 : gGameSpeed);
	else
		RCT2_GLOBAL(0x9DEBA2, uint16) = (gGameSpeed >= 4 ? 0 : gGameSpeed);
}

void top_toolbar_fastforward_menu_dropdown(short dropdownIndex) {
	if (dropdownIndex == -1) dropdownIndex = RCT2_GLOBAL(0x9DEBA2, uint16);
	rct_window* w = window_get_main();
	if (w) {
		if (dropdownIndex >= 0 && dropdownIndex <= 5) {
			gGameSpeed = dropdownIndex + 1;
			if (gGameSpeed >= 5)
				gGameSpeed = 8;
			window_invalidate(w);
		}
	}
}

void top_toolbar_init_rotate_menu(rct_window* w, rct_widget* widget) {
	gDropdownItemsFormat[0] = STR_ROTATE_CLOCKWISE;
	gDropdownItemsFormat[1] = STR_ROTATE_ANTI_CLOCKWISE;

	window_dropdown_show_text(
		w->x + widget->left,
		w->y + widget->top,
		widget->bottom - widget->top + 1,
		w->colours[1] | 0x80,
		0,
		2
		);

	RCT2_GLOBAL(0x9DEBA2, uint16) = 0;
}

void top_toolbar_rotate_menu_dropdown(short dropdownIndex) {
	if (dropdownIndex == -1) dropdownIndex = RCT2_GLOBAL(0x9DEBA2, uint16);
	rct_window* w = window_get_main();
	if (w) {
		if (dropdownIndex == 0) {
			window_rotate_camera(w, 1);
			window_invalidate(w);
		}
		else if (dropdownIndex == 1){
			window_rotate_camera(w, -1);
			window_invalidate(w);
		}
	}
}

void top_toolbar_init_debug_menu(rct_window* w, rct_widget* widget) {
	gDropdownItemsFormat[0] = STR_DEBUG_DROPDOWN_CONSOLE;
	gDropdownItemsFormat[1] = STR_DEBUG_DROPDOWN_TILE_INSPECTOR;
	gDropdownItemsFormat[2] = STR_DEBUG_DROPDOWN_OBJECT_SELECTION;

	window_dropdown_show_text(
		w->x + widget->left,
		w->y + widget->top,
		widget->bottom - widget->top + 1,
		w->colours[1] | 0x80,
		0,
		3
	);

	RCT2_GLOBAL(0x9DEBA2, uint16) = 0;
}

void top_toolbar_init_network_menu(rct_window* w, rct_widget* widget) {
	gDropdownItemsFormat[0] = STR_PLAYER_LIST;

	window_dropdown_show_text(
		w->x + widget->left,
		w->y + widget->top,
		widget->bottom - widget->top + 1,
		w->colours[1] | 0x80,
		0,
		1
	);

	RCT2_GLOBAL(0x9DEBA2, uint16) = 0;
}

void top_toolbar_debug_menu_dropdown(short dropdownIndex) {
	if (dropdownIndex == -1) dropdownIndex = RCT2_GLOBAL(0x9DEBA2, uint16);
	rct_window* w = window_get_main();
	if (w) {
		switch (dropdownIndex) {
		case DDIDX_CONSOLE:
			console_open();
			break;
		case DDIDX_TILE_INSPECTOR:
			window_tile_inspector_open();
			break;
		case DDIDX_OBJECT_SELECTION:
			window_close_all();
			window_editor_object_selection_open();
			break;
		}
	}
}

void top_toolbar_network_menu_dropdown(short dropdownIndex) {
	if (dropdownIndex == -1) dropdownIndex = RCT2_GLOBAL(0x9DEBA2, uint16);
	rct_window* w = window_get_main();
	if (w) {
		switch (dropdownIndex) {
		case DDIDX_PLAYER_LIST:
			window_player_list_open();
			break;
		}
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

/**
 *
 *  rct2: 0x0066D104
 */
bool land_tool_is_active()
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE))
		return false;
	if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) != WC_TOP_TOOLBAR)
		return false;
	if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, sint16) != WIDX_LAND)
		return false;
	return true;
}
