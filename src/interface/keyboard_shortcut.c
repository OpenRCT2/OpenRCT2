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
#include "../game.h"
#include "../interface/chat.h"
#include "../input.h"
#include "../localisation/localisation.h"
#include "../network/network.h"
#include "keyboard_shortcut.h"
#include "viewport.h"
#include "window.h"
#include "widget.h"

typedef void (*shortcut_action)();

static const shortcut_action shortcut_table[SHORTCUT_COUNT];

/**
 * 
 *  rct2: 0x006E3E91
 */
void keyboard_shortcut_set(int key)
{
	int i;

	// Unmap shortcut that already uses this key
	for (i = 0; i < 32; i++) {
		if (key == gShortcutKeys[i]) {
			gShortcutKeys[i] = 0xFFFF;
			break;
		}
	}

	// Map shortcut to this key
	gShortcutKeys[RCT2_GLOBAL(0x009DE511, uint8)] = key;
	window_close_by_class(WC_CHANGE_KEYBOARD_SHORTCUT);
	window_invalidate_by_class(WC_KEYBOARD_SHORTCUT_LIST);
	config_shortcut_keys_save();
}

/**
 * 
 *  rct2: 0x006E3E68
 */
void keyboard_shortcut_handle(int key)
{
	int i;
	for (i = 0; i < SHORTCUT_COUNT; i++) {
		if (key == gShortcutKeys[i]) {
			keyboard_shortcut_handle_command(i);
			break;
		}
	}
}

void keyboard_shortcut_handle_command(int shortcutIndex)
{
	if (shortcutIndex >= 0 && shortcutIndex < countof(shortcut_table)) {
		shortcut_action action = shortcut_table[shortcutIndex];
		if (action != NULL) {
			action();
		}
	}
}

void keyboard_shortcut_format_string(char *buffer, uint16 shortcutKey)
{
	char formatBuffer[256];

	*buffer = 0;
	if (shortcutKey == 0xFFFF) return;
	if (shortcutKey & 0x100) {
		format_string(formatBuffer, STR_SHIFT_PLUS, NULL);
		strcat(buffer, formatBuffer);
	}
	if (shortcutKey & 0x200) {
		format_string(formatBuffer, STR_CTRL_PLUS, NULL);
		strcat(buffer, formatBuffer);
	}
	if (shortcutKey & 0x400) {
		format_string(formatBuffer, STR_ALT_PLUS, NULL);
		strcat(buffer, formatBuffer);
	}
	strcat(buffer, SDL_GetScancodeName(shortcutKey & 0xFF));
}

#pragma region Shortcut Commands

static void toggle_view_flag(int viewportFlag)
{
	rct_window *window;

	window = window_get_main();
	if (window != NULL) {
		window->viewport->flags ^= viewportFlag;
		window_invalidate(window);
	}
}

static void shortcut_close_top_most_window()
{
	window_close_top();
}

static void shortcut_close_all_floating_windows()
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR))
		window_close_all();
	else if (RCT2_GLOBAL(0x0141F570, uint8) == 1)
		window_close_top();
}

static void shortcut_cancel_construction_mode()
{
	rct_window *window;

	window = window_find_by_class(WC_ERROR);
	if (window != NULL)
		window_close(window);
	else if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE)
		tool_cancel();
}

static void shortcut_pause_game()
{
	rct_window *window;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER))) {
		window = window_find_by_class(WC_TOP_TOOLBAR);
		if (window != NULL) {
			window_invalidate(window);
			window_event_mouse_up_call(window, 0);
		}
	}
}

static void shortcut_zoom_view_out()
{
	rct_window *window;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER)) {
			window = window_find_by_class(WC_TOP_TOOLBAR);
			if (window != NULL) {
				window_invalidate(window);
				window_event_mouse_up_call(window, 2);
			}
		}
	}
}

static void shortcut_zoom_view_in()
{
	rct_window *window;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER)) {
			window = window_find_by_class(WC_TOP_TOOLBAR);
			if (window != NULL) {
				window_invalidate(window);
				window_event_mouse_up_call(window, 3);
			}
		}
	}
}

static void shortcut_rotate_view_clockwise()
{
	rct_window* w = window_get_main();
	window_rotate_camera(w, 1);
}

static void shortcut_rotate_view_anticlockwise()
{
	rct_window* w = window_get_main();
	window_rotate_camera(w, -1);
}

static void shortcut_rotate_construction_object()
{
	rct_window *w;

	// Rotate scenery
	w = window_find_by_class(WC_SCENERY);
	if (w != NULL && !widget_is_disabled(w, 25) && w->widgets[25].type != WWT_EMPTY) {
		window_event_mouse_up_call(w, 25);
		return;
	}

	// Rotate construction track piece
	w = window_find_by_class(WC_RIDE_CONSTRUCTION);
	if (w != NULL && !widget_is_disabled(w, 32) && w->widgets[32].type != WWT_EMPTY) {
		// Check if building a maze...
		if (w->widgets[32].tooltip != 1761) {
			window_event_mouse_up_call(w, 32);
			return;
		}
	}

	// Rotate track design preview
	w = window_find_by_class(WC_TRACK_DESIGN_LIST);
	if (w != NULL && !widget_is_disabled(w, 5) && w->widgets[5].type != WWT_EMPTY) {
		window_event_mouse_up_call(w, 5);
		return;
	}

	// Rotate track design placement
	w = window_find_by_class(WC_TRACK_DESIGN_PLACE);
	if (w != NULL && !widget_is_disabled(w, 3) && w->widgets[3].type != WWT_EMPTY) {
		window_event_mouse_up_call(w, 3);
		return;
	}

	// Rotate park entrance
	w = window_find_by_class(WC_MAP);
	if (w != NULL && !widget_is_disabled(w, 20) && w->widgets[20].type != WWT_EMPTY) {
		window_event_mouse_up_call(w, 20);
		return;
	}
}

static void shortcut_underground_view_toggle()
{
	toggle_view_flag(VIEWPORT_FLAG_UNDERGROUND_INSIDE);
}

static void shortcut_remove_base_land_toggle()
{
	toggle_view_flag(VIEWPORT_FLAG_HIDE_BASE);
}

static void shortcut_remove_vertical_land_toggle()
{
	toggle_view_flag(VIEWPORT_FLAG_HIDE_VERTICAL);
}

static void shortcut_remove_top_bottom_toolbar_toggle()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TITLE_DEMO)
		return;

	if (window_find_by_class(WC_TOP_TOOLBAR) != NULL) {
		window_close(window_find_by_class(WC_TOP_TOOLBAR));
		window_close(window_find_by_class(WC_BOTTOM_TOOLBAR));
	} else {
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) == 0) {
			window_top_toolbar_open();
			window_game_bottom_toolbar_open();
		} else {
			window_top_toolbar_open();
			window_editor_bottom_toolbar_open();
		}
	}
}

static void shortcut_see_through_rides_toggle()
{
	toggle_view_flag(VIEWPORT_FLAG_SEETHROUGH_RIDES);
}

static void shortcut_see_through_scenery_toggle()
{
	toggle_view_flag(VIEWPORT_FLAG_SEETHROUGH_SCENERY);
}

static void shortcut_invisible_supports_toggle()
{
	toggle_view_flag(VIEWPORT_FLAG_INVISIBLE_SUPPORTS);
}

static void shortcut_invisible_people_toggle()
{
	toggle_view_flag(VIEWPORT_FLAG_INVISIBLE_PEEPS);
}

static void shortcut_height_marks_on_land_toggle()
{
	toggle_view_flag(VIEWPORT_FLAG_LAND_HEIGHTS);
}

static void shortcut_height_marks_on_ride_tracks_toggle()
{
	toggle_view_flag(VIEWPORT_FLAG_TRACK_HEIGHTS);
}

static void shortcut_height_marks_on_paths_toggle()
{
	toggle_view_flag(VIEWPORT_FLAG_PATH_HEIGHTS);
}

static void shortcut_adjust_land()
{
	rct_window *window;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
			window = window_find_by_class(WC_TOP_TOOLBAR);
			if (window != NULL) {
				window_invalidate(window);
				window_event_mouse_up_call(window, 7);
			}
		}
	}
}

static void shortcut_adjust_water()
{
	rct_window *window;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
			window = window_find_by_class(WC_TOP_TOOLBAR);
			if (window != NULL) {
				window_invalidate(window);
				window_event_mouse_up_call(window, 8);
			}
		}
	}
}

static void shortcut_build_scenery()
{
	rct_window *window;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
			window = window_find_by_class(WC_TOP_TOOLBAR);
			if (window != NULL) {
				window_invalidate(window);
				window_event_mouse_up_call(window, 9);
			}
		}
	}
}

static void shortcut_build_paths()
{
	rct_window *window;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
			window = window_find_by_class(WC_TOP_TOOLBAR);
			if (window != NULL) {
				window_invalidate(window);
				window_event_mouse_up_call(window, 10);
			}
		}
	}
}

static void shortcut_build_new_ride()
{
	rct_window *window;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || RCT2_GLOBAL(0x0141F570, uint8) == 1) {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
			window = window_find_by_class(WC_TOP_TOOLBAR);
			if (window != NULL) {
				window_invalidate(window);
				window_event_mouse_up_call(window, 11);
			}
		}
	}
}

static void shortcut_show_financial_information()
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY))
			window_finances_open();
}

static void shortcut_show_research_information()
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
		if (gConfigInterface.toolbar_show_research)
			window_research_open();
		else
			window_new_ride_open_research();
	}
}

static void shortcut_show_rides_list()
{
	rct_window *window;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
		window = window_find_by_class(WC_TOP_TOOLBAR);
		if (window != NULL) {
			window_invalidate(window);
			window_event_mouse_up_call(window, 12);
		}
	}
}

static void shortcut_show_park_information()
{
	rct_window *window;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
		window = window_find_by_class(WC_TOP_TOOLBAR);
		if (window != NULL) {
			window_invalidate(window);
			window_event_mouse_up_call(window, 13);
		}
	}
}

static void shortcut_show_guest_list()
{
	rct_window *window;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
		window = window_find_by_class(WC_TOP_TOOLBAR);
		if (window != NULL) {
			window_invalidate(window);
			window_event_mouse_up_call(window, 15);
		}
	}
}

static void shortcut_show_staff_list()
{
	rct_window *window;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
		window = window_find_by_class(WC_TOP_TOOLBAR);
		if (window != NULL) {
			window_invalidate(window);
			window_event_mouse_up_call(window, 14);
		}
	}
}

static void shortcut_show_recent_messages()
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
		window_news_open();
}

static void shortcut_show_map()
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || RCT2_GLOBAL(0x0141F570, uint8) == 1)
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
			window_map_open();
}

static void shortcut_screenshot()
{
	RCT2_GLOBAL(RCT2_ADDRESS_SCREENSHOT_COUNTDOWN, uint8) = 2;
}

static void shortcut_reduce_game_speed()
{
	game_reduce_game_speed();
}

static void shortcut_increase_game_speed()
{
	game_increase_game_speed();
}

static void shortcut_open_cheat_window()
{
	rct_window *window;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) != SCREEN_FLAGS_PLAYING)
		return;

	// Check if window is already open
	window = window_find_by_class(WC_CHEATS);
	if (window != NULL) {
		window_close(window);
		return;
	}
	window_cheats_open();
}

static void shortcut_open_chat_window()
{
	chat_toggle();
}

static void shortcut_quick_save_game()
{
	// Do a quick save in playing mode and a regular save in Scenario Editor mode. In other cases, don't do anything.
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) == SCREEN_FLAGS_PLAYING) {
		tool_cancel();
		save_game();
	}
	else if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) {
		rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;
		window_loadsave_open(LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE, s6Info->name);
	}
}

static const shortcut_action shortcut_table[SHORTCUT_COUNT] = {
	shortcut_close_top_most_window,
	shortcut_close_all_floating_windows,
	shortcut_cancel_construction_mode,
	shortcut_pause_game,
	shortcut_zoom_view_out,
	shortcut_zoom_view_in,
	shortcut_rotate_view_clockwise,
	shortcut_rotate_view_anticlockwise,
	shortcut_rotate_construction_object,
	shortcut_underground_view_toggle,
	shortcut_remove_base_land_toggle,
	shortcut_remove_vertical_land_toggle,
	shortcut_see_through_rides_toggle,
	shortcut_see_through_scenery_toggle,
	shortcut_invisible_supports_toggle,
	shortcut_invisible_people_toggle,
	shortcut_height_marks_on_land_toggle,
	shortcut_height_marks_on_ride_tracks_toggle,
	shortcut_height_marks_on_paths_toggle,
	shortcut_adjust_land,
	shortcut_adjust_water,
	shortcut_build_scenery,
	shortcut_build_paths,
	shortcut_build_new_ride,
	shortcut_show_financial_information,
	shortcut_show_research_information,
	shortcut_show_rides_list,
	shortcut_show_park_information,
	shortcut_show_guest_list,
	shortcut_show_staff_list,
	shortcut_show_recent_messages,
	shortcut_show_map,
	shortcut_screenshot,

	//new
	shortcut_reduce_game_speed,
	shortcut_increase_game_speed,
	shortcut_open_cheat_window,
	shortcut_remove_top_bottom_toolbar_toggle,
	NULL,
	NULL,
	NULL,
	NULL,
	shortcut_open_chat_window,
	shortcut_quick_save_game,
};

#pragma endregion
