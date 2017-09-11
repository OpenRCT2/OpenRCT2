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

#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/Editor.h>
#include <openrct2/game.h>
#include <openrct2/input.h>
#include <openrct2/interface/chat.h>
#include <openrct2/interface/Screenshot.h>
#include <openrct2/localisation/localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/platform/platform.h>
#include <openrct2/ride/track.h>
#include <openrct2/ride/track_paint.h>
#include <openrct2/title/TitleScreen.h>
#include <openrct2/util/util.h>
#include <openrct2/interface/viewport.h>
#include <openrct2/interface/widget.h>
#include <openrct2/interface/window.h>
#include <openrct2/Context.h>
#include "KeyboardShortcuts.h"

uint8 gKeyboardShortcutChangeId;

typedef void (*shortcut_action)();

static const shortcut_action shortcut_table[SHORTCUT_COUNT];

/**
 *
 *  rct2: 0x006E3E68
 */
void keyboard_shortcut_handle(sint32 key)
{
    sint32 shortcut = keyboard_shortcuts_get_from_key(key);
    if (shortcut != -1) {
        keyboard_shortcut_handle_command(shortcut);
    }
}

void keyboard_shortcut_handle_command(sint32 shortcutIndex)
{
    if (shortcutIndex >= 0 && shortcutIndex < countof(shortcut_table)) {
        shortcut_action action = shortcut_table[shortcutIndex];
        if (action != NULL) {
            action();
        }
    }
}

#pragma region Shortcut Commands

static void toggle_view_flag(sint32 viewportFlag)
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
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR))
        window_close_all();
    else if (gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR)
        window_close_top();
}

static void shortcut_cancel_construction_mode()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    rct_window *window = window_find_by_class(WC_ERROR);
    if (window != NULL)
        window_close(window);
    else if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        tool_cancel();
}

static void shortcut_pause_game()
{
    if (!(gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER))) {
        rct_window *window = window_find_by_class(WC_TOP_TOOLBAR);
        if (window != NULL) {
            window_invalidate(window);
            window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_PAUSE);
        }
    }
}

static void shortcut_zoom_view_out()
{
    main_window_zoom(false, false);
}

static void shortcut_zoom_view_in()
{
    main_window_zoom(true, false);
}

static void shortcut_rotate_view_clockwise()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    rct_window* w = window_get_main();
    window_rotate_camera(w, 1);
}

static void shortcut_rotate_view_anticlockwise()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    rct_window* w = window_get_main();
    window_rotate_camera(w, -1);
}

static void shortcut_rotate_construction_object()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    // Rotate scenery
    rct_window *w = window_find_by_class(WC_SCENERY);
    if (w != NULL && !widget_is_disabled(w, WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON) && w->widgets[WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type != WWT_EMPTY) {
        window_event_mouse_up_call(w, WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON);
        return;
    }

    // Rotate construction track piece
    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != NULL && !widget_is_disabled(w, WC_RIDE_CONSTRUCTION__WIDX_ROTATE) && w->widgets[WC_RIDE_CONSTRUCTION__WIDX_ROTATE].type != WWT_EMPTY) {
        // Check if building a maze...
        if (w->widgets[WC_RIDE_CONSTRUCTION__WIDX_ROTATE].tooltip != STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP) {
            window_event_mouse_up_call(w, WC_RIDE_CONSTRUCTION__WIDX_ROTATE);
            return;
        }
    }

    // Rotate track design preview
    w = window_find_by_class(WC_TRACK_DESIGN_LIST);
    if (w != NULL && !widget_is_disabled(w, WC_TRACK_DESIGN_LIST__WIDX_ROTATE) && w->widgets[WC_TRACK_DESIGN_LIST__WIDX_ROTATE].type != WWT_EMPTY) {
        window_event_mouse_up_call(w, WC_TRACK_DESIGN_LIST__WIDX_ROTATE);
        return;
    }

    // Rotate track design placement
    w = window_find_by_class(WC_TRACK_DESIGN_PLACE);
    if (w != NULL && !widget_is_disabled(w, WC_TRACK_DESIGN_PLACE__WIDX_ROTATE) && w->widgets[WC_TRACK_DESIGN_PLACE__WIDX_ROTATE].type != WWT_EMPTY) {
        window_event_mouse_up_call(w, WC_TRACK_DESIGN_PLACE__WIDX_ROTATE);
        return;
    }

    // Rotate park entrance
    w = window_find_by_class(WC_MAP);
    if (w != NULL && !widget_is_disabled(w, WC_MAP__WIDX_ROTATE_90) && w->widgets[WC_MAP__WIDX_ROTATE_90].type != WWT_EMPTY) {
        window_event_mouse_up_call(w, WC_MAP__WIDX_ROTATE_90);
        return;
    }
}

static void shortcut_underground_view_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    toggle_view_flag(VIEWPORT_FLAG_UNDERGROUND_INSIDE);
}

static void shortcut_remove_base_land_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    toggle_view_flag(VIEWPORT_FLAG_HIDE_BASE);
}

static void shortcut_remove_vertical_land_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    toggle_view_flag(VIEWPORT_FLAG_HIDE_VERTICAL);
}

static void shortcut_remove_top_bottom_toolbar_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) {
        if (window_find_by_class(WC_TITLE_LOGO) != NULL) {
            window_close(window_find_by_class(WC_TITLE_LOGO));
            window_close(window_find_by_class(WC_TITLE_OPTIONS));
            window_close(window_find_by_class(WC_TITLE_MENU));
            window_close(window_find_by_class(WC_TITLE_EXIT));
            title_set_hide_version_info(true);
        } else {
            title_create_windows();
        }
    } else {
        if (window_find_by_class(WC_TOP_TOOLBAR) != NULL) {
            window_close(window_find_by_class(WC_DROPDOWN));
            window_close(window_find_by_class(WC_TOP_TOOLBAR));
            window_close(window_find_by_class(WC_BOTTOM_TOOLBAR));
        } else {
            if (gScreenFlags == 0) {
                window_top_toolbar_open();
                window_game_bottom_toolbar_open();
            } else {
                window_top_toolbar_open();
                window_editor_bottom_toolbar_open();
            }
        }
    }
    gfx_invalidate_screen();
}

static void shortcut_see_through_rides_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    toggle_view_flag(VIEWPORT_FLAG_SEETHROUGH_RIDES);
}

static void shortcut_see_through_scenery_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    toggle_view_flag(VIEWPORT_FLAG_SEETHROUGH_SCENERY);
}

static void shortcut_see_through_paths_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    toggle_view_flag(VIEWPORT_FLAG_SEETHROUGH_PATHS);
}

static void shortcut_invisible_supports_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    toggle_view_flag(VIEWPORT_FLAG_INVISIBLE_SUPPORTS);
}

static void shortcut_invisible_people_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    toggle_view_flag(VIEWPORT_FLAG_INVISIBLE_PEEPS);
}

static void shortcut_height_marks_on_land_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    toggle_view_flag(VIEWPORT_FLAG_LAND_HEIGHTS);
}

static void shortcut_height_marks_on_ride_tracks_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    toggle_view_flag(VIEWPORT_FLAG_TRACK_HEIGHTS);
}

static void shortcut_height_marks_on_paths_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    toggle_view_flag(VIEWPORT_FLAG_PATH_HEIGHTS);
}

static void shortcut_adjust_land()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR) {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
            rct_window *window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != NULL) {
                window_invalidate(window);
                window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_LAND);
            }
        }
    }
}

static void shortcut_adjust_water()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR) {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
            rct_window *window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != NULL) {
                window_invalidate(window);
                window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_WATER);
            }
        }
    }
}

static void shortcut_build_scenery()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR) {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
            rct_window *window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != NULL) {
                window_invalidate(window);
                window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_SCENERY);
            }
        }
    }
}

static void shortcut_build_paths()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR) {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
            rct_window *window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != NULL) {
                window_invalidate(window);
                window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_PATH);
            }
        }
    }
}

static void shortcut_build_new_ride()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)) {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
            window_new_ride_open();
        }
    }
}

static void shortcut_show_financial_information()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
            context_open_window(WC_FINANCES);
}

static void shortcut_show_research_information()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
        if (gConfigInterface.toolbar_show_research)
            window_research_open();
        else
            window_new_ride_open_research();
    }
}

static void shortcut_show_rides_list()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
        window_ride_list_open();
    }
}

static void shortcut_show_park_information()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
        context_open_window(WC_PARK_INFORMATION);
    }
}

static void shortcut_show_guest_list()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
        context_open_window(WC_GUEST_LIST);
    }
}

static void shortcut_show_staff_list()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
        context_open_window(WC_STAFF_LIST);
    }
}

static void shortcut_show_recent_messages()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        context_open_window(WC_RECENT_NEWS);
}

static void shortcut_show_map()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR)
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
            window_map_open();
}

static void shortcut_screenshot()
{
    gScreenshotCountdown = 2;
}

static void shortcut_reduce_game_speed()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (network_get_mode() == NETWORK_MODE_NONE)
        game_reduce_game_speed();
}

static void shortcut_increase_game_speed()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (network_get_mode() == NETWORK_MODE_NONE)
        game_increase_game_speed();
}

static void shortcut_open_cheat_window()
{
    if (gScreenFlags != SCREEN_FLAGS_PLAYING)
        return;

    // Check if window is already open
    rct_window *window = window_find_by_class(WC_CHEATS);
    if (window != NULL) {
        window_close(window);
        return;
    }
    context_open_window(WC_CHEATS);
}

static void shortcut_clear_scenery()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;
    
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR) {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
            rct_window *window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != NULL) {
                window_invalidate(window);
                window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_CLEAR_SCENERY);
            }
        }
    }
}


static void shortcut_open_chat_window()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    chat_toggle();
}

static void shortcut_quick_save_game()
{
    // Do a quick save in playing mode and a regular save in Scenario Editor mode. In other cases, don't do anything.
    if (gScreenFlags == SCREEN_FLAGS_PLAYING) {
        tool_cancel();
        save_game();
    }
    else if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) {
        Intent * intent = intent_create(WC_LOADSAVE);
        intent_set_uint(intent, INTENT_EXTRA_4, LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE);
        intent_set_string(intent, INTENT_EXTRA_5, gS6Info.name);
        context_open_intent(intent);
        intent_release(intent);
    }
}

static void shortcut_show_options()
{
    context_open_window(WC_OPTIONS);
}

static void shortcut_mute_sound()
{
    audio_toggle_all_sounds();
}

static void shortcut_windowed_mode_toggle()
{
    platform_toggle_windowed_mode();
}

static void shortcut_show_multiplayer()
{
    if (network_get_mode() != NETWORK_MODE_NONE)
        context_open_window(WC_MULTIPLAYER);
}

static void shortcut_orginal_painting_toggle()
{
    gUseOriginalRidePaint = !gUseOriginalRidePaint;
    window_invalidate_by_class(WC_DEBUG_PAINT);
    gfx_invalidate_screen();
}

static void shortcut_debug_paint_toggle()
{
    rct_window * window = window_find_by_class(WC_DEBUG_PAINT);
    if (window != NULL) {
        window_close(window);
    } else {
        context_open_window(WC_DEBUG_PAINT);
    }
}

static void shortcut_ride_construction_turn_left()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_turn_left();
}

static void shortcut_ride_construction_turn_right()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_turn_right();
}

static void shortcut_ride_construction_use_track_default()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_use_track_default();
}

static void shortcut_ride_construction_slope_down()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_slope_down();
}

static void shortcut_ride_construction_slope_up()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_slope_up();
}

static void shortcut_ride_construction_chain_lift_toggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_chain_lift_toggle();
}

static void shortcut_ride_construction_bank_left()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_bank_left();
}

static void shortcut_ride_construction_bank_right()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_bank_right();
}

static void shortcut_ride_construction_previous_track()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_previous_track();
}

static void shortcut_ride_construction_next_track()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_next_track();
}

static void shortcut_ride_construction_build_current()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_build_current();
}

static void shortcut_ride_construction_demolish_current()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_demolish_current();
}

static void shortcut_load_game()
{
    if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
        game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 0, 0);
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
    shortcut_show_options,
    shortcut_mute_sound,
    shortcut_windowed_mode_toggle,
    shortcut_show_multiplayer,
    shortcut_orginal_painting_toggle,
    shortcut_debug_paint_toggle,
    shortcut_see_through_paths_toggle,
    shortcut_ride_construction_turn_left,
    shortcut_ride_construction_turn_right,
    shortcut_ride_construction_use_track_default,
    shortcut_ride_construction_slope_down,
    shortcut_ride_construction_slope_up,
    shortcut_ride_construction_chain_lift_toggle,
    shortcut_ride_construction_bank_left,
    shortcut_ride_construction_bank_right,
    shortcut_ride_construction_previous_track,
    shortcut_ride_construction_next_track,
    shortcut_ride_construction_build_current,
    shortcut_ride_construction_demolish_current,
    shortcut_load_game,
    shortcut_clear_scenery,
};

#pragma endregion
