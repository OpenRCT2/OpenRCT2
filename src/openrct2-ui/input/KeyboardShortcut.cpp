/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "KeyboardShortcuts.h"

#include <iterator>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/LoadOrQuitAction.hpp>
#include <openrct2/actions/SetCheatAction.hpp>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/Screenshot.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/platform/platform.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackPaint.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/title/TitleScreen.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>

extern bool gWindowSceneryEyedropperEnabled;

using shortcut_action = void (*)();
using namespace OpenRCT2;

Input::Shortcut gKeyboardShortcutChangeId;

namespace
{
    extern const shortcut_action shortcut_table[Input::ShortcutsCount];
}

/**
 *
 *  rct2: 0x006E3E68
 */
using namespace OpenRCT2;
void KeyboardShortcutHandle(int32_t key)
{
    auto shortcut = KeyboardShortcutsGetFromKey(key);
    if (shortcut != Input::Shortcut::Undefined)
    {
        KeyboardShortcutHandleCommand(shortcut);
    }
}

void KeyboardShortcutHandleCommand(Input::Shortcut shortcut)
{
    size_t shortcutIndex = static_cast<size_t>(shortcut);
    if (shortcutIndex < std::size(shortcut_table))
    {
        shortcut_action action = shortcut_table[shortcutIndex];
        if (action != nullptr)
        {
            action();
        }
    }
}

#pragma region Shortcut Commands

static void ToggleViewFlag(int32_t viewportFlag)
{
    rct_window* window;

    window = window_get_main();
    if (window != nullptr)
    {
        window->viewport->flags ^= viewportFlag;
        window->Invalidate();
    }
}

static void ShortcutCloseTopMostWindow()
{
    window_close_top();
}

static void ShortcutCloseAllFloatingWindow()
{
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR))
        window_close_all();
    else if (gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR)
        window_close_top();
}

static void ShortcutCancelConstructionMode()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    rct_window* window = window_find_by_class(WC_ERROR);
    if (window != nullptr)
        window_close(window);
    else if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        tool_cancel();
}

static void ShortcutPauseGame()
{
    if (!(gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER)))
    {
        rct_window* window = window_find_by_class(WC_TOP_TOOLBAR);
        if (window != nullptr)
        {
            window->Invalidate();
            window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_PAUSE);
        }
    }
}

static void ShortcutZoomViewOut()
{
    main_window_zoom(false, false);
}

static void ShortcutZoomViewIn()
{
    main_window_zoom(true, false);
}

static void ShortcutRotateViewClockwise()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    rct_window* w = window_get_main();
    window_rotate_camera(w, 1);
}

static void ShortcutRotateViewAnticlockwise()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    rct_window* w = window_get_main();
    window_rotate_camera(w, -1);
}

static void ShortcutRotateConstructionObject()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    // Rotate scenery
    rct_window* w = window_find_by_class(WC_SCENERY);
    if (w != nullptr && !widget_is_disabled(w, WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON)
        && w->widgets[WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type != WWT_EMPTY)
    {
        window_event_mouse_up_call(w, WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON);
        return;
    }

    // Rotate construction track piece
    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != nullptr && !widget_is_disabled(w, WC_RIDE_CONSTRUCTION__WIDX_ROTATE)
        && w->widgets[WC_RIDE_CONSTRUCTION__WIDX_ROTATE].type != WWT_EMPTY)
    {
        // Check if building a maze...
        if (w->widgets[WC_RIDE_CONSTRUCTION__WIDX_ROTATE].tooltip != STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP)
        {
            window_event_mouse_up_call(w, WC_RIDE_CONSTRUCTION__WIDX_ROTATE);
            return;
        }
    }

    // Rotate track design preview
    w = window_find_by_class(WC_TRACK_DESIGN_LIST);
    if (w != nullptr && !widget_is_disabled(w, WC_TRACK_DESIGN_LIST__WIDX_ROTATE)
        && w->widgets[WC_TRACK_DESIGN_LIST__WIDX_ROTATE].type != WWT_EMPTY)
    {
        window_event_mouse_up_call(w, WC_TRACK_DESIGN_LIST__WIDX_ROTATE);
        return;
    }

    // Rotate track design placement
    w = window_find_by_class(WC_TRACK_DESIGN_PLACE);
    if (w != nullptr && !widget_is_disabled(w, WC_TRACK_DESIGN_PLACE__WIDX_ROTATE)
        && w->widgets[WC_TRACK_DESIGN_PLACE__WIDX_ROTATE].type != WWT_EMPTY)
    {
        window_event_mouse_up_call(w, WC_TRACK_DESIGN_PLACE__WIDX_ROTATE);
        return;
    }

    // Rotate park entrance
    w = window_find_by_class(WC_MAP);
    if (w != nullptr && !widget_is_disabled(w, WC_MAP__WIDX_ROTATE_90) && w->widgets[WC_MAP__WIDX_ROTATE_90].type != WWT_EMPTY)
    {
        window_event_mouse_up_call(w, WC_MAP__WIDX_ROTATE_90);
        return;
    }

    // Rotate selected element in tile inspector
    w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !widget_is_disabled(w, WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE].type != WWT_EMPTY)
    {
        window_event_mouse_up_call(w, WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE);
        return;
    }
}

static void ShortcutUndergroundViewToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_UNDERGROUND_INSIDE);
}

static void ShortcutRemoveBaseLandToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_HIDE_BASE);
}

static void ShortcutRemoveVerticalLandToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_HIDE_VERTICAL);
}

static void ShortcutRemoveTopBottomToolbarToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
    {
        if (window_find_by_class(WC_TITLE_LOGO) != nullptr)
        {
            window_close(window_find_by_class(WC_TITLE_LOGO));
            window_close(window_find_by_class(WC_TITLE_OPTIONS));
            window_close(window_find_by_class(WC_TITLE_MENU));
            window_close(window_find_by_class(WC_TITLE_EXIT));
            title_set_hide_version_info(true);
        }
        else
        {
            title_create_windows();
        }
    }
    else
    {
        if (window_find_by_class(WC_TOP_TOOLBAR) != nullptr)
        {
            window_close(window_find_by_class(WC_DROPDOWN));
            window_close(window_find_by_class(WC_TOP_TOOLBAR));
            window_close(window_find_by_class(WC_BOTTOM_TOOLBAR));
        }
        else
        {
            if (gScreenFlags == 0)
            {
                context_open_window(WC_TOP_TOOLBAR);
                context_open_window(WC_BOTTOM_TOOLBAR);
            }
            else
            {
                context_open_window(WC_TOP_TOOLBAR);
                context_open_window_view(WV_EDITOR_BOTTOM_TOOLBAR);
            }
        }
    }
    gfx_invalidate_screen();
}

static void ShortcutSeeThroughRidesToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_SEETHROUGH_RIDES);
}

static void ShortcutSeeThroughSceneryToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_SEETHROUGH_SCENERY);
}

static void ShortcutSeeThroughPathsToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_SEETHROUGH_PATHS);
}

static void ShortcutInvisibleSupportsToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_INVISIBLE_SUPPORTS);
}

static void ShortcutInvisiblePeopleToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_INVISIBLE_PEEPS);
}

static void ShortcutGridlinesToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_GRIDLINES);
}

static void ShortcutHeightMarksOnLandToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_LAND_HEIGHTS);
}

static void ShortcutHeightMarksOnRideTracksToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_TRACK_HEIGHTS);
}

static void ShortcutHeightMarksOnPathsToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_PATH_HEIGHTS);
}

static void ShortcutAdjustLand()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR)
    {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            rct_window* window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != nullptr)
            {
                window->Invalidate();
                window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_LAND);
            }
        }
    }
}

static void ShortcutAdjustWater()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR)
    {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            rct_window* window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != nullptr)
            {
                window->Invalidate();
                window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_WATER);
            }
        }
    }
}

static void ShortcutBuildScenery()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR)
    {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            rct_window* window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != nullptr)
            {
                window->Invalidate();
                window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_SCENERY);
            }
        }
    }
}

static void ShortcutBuildPaths()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR)
    {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            rct_window* window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != nullptr)
            {
                window->Invalidate();
                window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_PATH);
            }
        }
    }
}

static void ShortcutBuildNewRide()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR))
    {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            context_open_window(WC_CONSTRUCT_RIDE);
        }
    }
}

static void ShortcutShowFinancialInformation()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
            context_open_window(WC_FINANCES);
}

static void ShortcutShowResearchInformation()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        context_open_window_view(WV_RIDE_RESEARCH);
    }
}

static void ShortcutShowRidesList()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        context_open_window(WC_RIDE_LIST);
    }
}

static void ShortcutShowParkInformation()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        context_open_window(WC_PARK_INFORMATION);
    }
}

static void ShortcutShowGuestList()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        context_open_window(WC_GUEST_LIST);
    }
}

static void ShortcutShowStaffList()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        context_open_window(WC_STAFF_LIST);
    }
}

static void ShortcutShowRecentMessages()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
        context_open_window(WC_RECENT_NEWS);
}

static void ShortcutShowMap()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR)
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
            context_open_window(WC_MAP);
}

static void ShortcutScreenshot()
{
    gScreenshotCountdown = 2;
}

static void ShortcutReduceGameSpeed()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (network_get_mode() == NETWORK_MODE_NONE)
        game_reduce_game_speed();
}

static void ShortcutIncreaseGameSpeed()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (network_get_mode() == NETWORK_MODE_NONE)
        game_increase_game_speed();
}

static void ShortcutOpenCheatWindow()
{
    if (gScreenFlags != SCREEN_FLAGS_PLAYING)
        return;

    // Check if window is already open
    rct_window* window = window_find_by_class(WC_CHEATS);
    if (window != nullptr)
    {
        window_close(window);
        return;
    }
    context_open_window(WC_CHEATS);
}

static void ShortcutClearScenery()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EDITOR_STEP_LANDSCAPE_EDITOR)
    {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            rct_window* window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != nullptr)
            {
                window->Invalidate();
                window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_CLEAR_SCENERY);
            }
        }
    }
}

static void ShortcutOpenChatWindow()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    chat_toggle();
}

static void ShortcutQuickSaveGame()
{
    // Do a quick save in playing mode and a regular save in Scenario Editor mode. In other cases, don't do anything.
    if (gScreenFlags == SCREEN_FLAGS_PLAYING)
    {
        tool_cancel();
        save_game();
    }
    else if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
    {
        auto intent = Intent(WC_LOADSAVE);
        intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE);
        intent.putExtra(INTENT_EXTRA_PATH, std::string{ gS6Info.name });
        context_open_intent(&intent);
    }
}

static void ShortcutShowOptions()
{
    context_open_window(WC_OPTIONS);
}

static void ShortcutMuteSound()
{
    OpenRCT2::Audio::ToggleAllSounds();
}

static void ShortcutWindowsModeToggle()
{
    platform_toggle_windowed_mode();
}

static void ShortcutShowMultiplayer()
{
    if (network_get_mode() != NETWORK_MODE_NONE)
        context_open_window(WC_MULTIPLAYER);
}

static void ShortcutDebugPaintToggle()
{
    rct_window* window = window_find_by_class(WC_DEBUG_PAINT);
    if (window != nullptr)
    {
        window_close(window);
    }
    else
    {
        context_open_window(WC_DEBUG_PAINT);
    }
}

static void ShortcutRideConstructionTurnLeft()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_turn_left();
}

static void ShortcutRideConstructionTurnRight()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_turn_right();
}

static void ShortcutRideConstructionUseTrackDefault()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_use_track_default();
}

static void ShortcutRideConstructionSlopeDown()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_slope_down();
}

static void ShortcutRideConstructionSlopeUp()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_slope_up();
}

static void ShortcutRideConstructionChainLiftToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_chain_lift_toggle();
}

static void ShortcutRideConstructionBankLeft()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_bank_left();
}

static void ShortcutRideConstructionBankRight()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_bank_right();
}

static void ShortcutRideConstructionPreviousTrack()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_previous_track();
}

static void ShortcutRideConstructionNextTrack()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_next_track();
}

static void ShortcutRideConstructionBuildCurrent()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_build_current();
}

static void ShortcutRideConstructionDemolishCurrent()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    window_ride_construction_keyboard_shortcut_demolish_current();
}

static void ShortcutLoadGame()
{
    if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
    {
        auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::OpenSavePrompt);
        GameActions::Execute(&loadOrQuitAction);
    }
}

static void ShortcutViewClipping()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    context_open_window(WC_VIEW_CLIPPING);
}

static void ShortcutHighlightPathIssuesToggle()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    ToggleViewFlag(VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES);
}

static void ShortcutOpenTileInspector()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO || !gConfigInterface.toolbar_show_cheats)
        return;

    context_open_window(WC_TILE_INSPECTOR);
}

static void ShortcutAdvanceToNextTick()
{
    if (gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER))
        return;

    gDoSingleUpdate = true;
}

static void ShortcutOpenSceneryPicker()
{
    if ((gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        || (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR && gS6Info.editor_step != EDITOR_STEP_LANDSCAPE_EDITOR))
        return;

    rct_window* window_scenery = window_find_by_class(WC_SCENERY);
    if (window_scenery == nullptr)
    {
        rct_window* window_toolbar = window_find_by_class(WC_TOP_TOOLBAR);
        if (window_toolbar != nullptr)
        {
            window_toolbar->Invalidate();
            window_event_mouse_up_call(window_toolbar, WC_TOP_TOOLBAR__WIDX_SCENERY);
        }
    }

    window_scenery = window_find_by_class(WC_SCENERY);
    if (window_scenery != nullptr && !widget_is_disabled(window_scenery, WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON)
        && window_scenery->widgets[WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON].type != WWT_EMPTY
        && !gWindowSceneryEyedropperEnabled)
    {
        window_event_mouse_up_call(window_scenery, WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON);
        return;
    }
}

static void ShortcutScaleUp()
{
    gConfigGeneral.window_scale += 0.25f;
    config_save_default();
    gfx_invalidate_screen();
    context_trigger_resize();
    context_update_cursor_scale();
}

static void ShortcutScaleDown()
{
    gConfigGeneral.window_scale -= 0.25f;
    gConfigGeneral.window_scale = std::max(0.5f, gConfigGeneral.window_scale);
    config_save_default();
    gfx_invalidate_screen();
    context_trigger_resize();
    context_update_cursor_scale();
}

// Tile inspector shortcuts
static void ShortcutInsertCorruptElement()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !widget_is_disabled(w, WC_TILE_INSPECTOR__WIDX_BUTTON_CORRUPT)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_BUTTON_CORRUPT].type != WWT_EMPTY)
    {
        window_event_mouse_up_call(w, WC_TILE_INSPECTOR__WIDX_BUTTON_CORRUPT);
        return;
    }
}

static void ShortcutCopyElement()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !widget_is_disabled(w, WC_TILE_INSPECTOR__WIDX_BUTTON_COPY)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_BUTTON_COPY].type != WWT_EMPTY)
    {
        window_event_mouse_up_call(w, WC_TILE_INSPECTOR__WIDX_BUTTON_COPY);
        return;
    }
}

static void ShortcutPasteElement()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !widget_is_disabled(w, WC_TILE_INSPECTOR__WIDX_BUTTON_PASTE)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_BUTTON_PASTE].type != WWT_EMPTY)
    {
        window_event_mouse_up_call(w, WC_TILE_INSPECTOR__WIDX_BUTTON_PASTE);
        return;
    }
}

static void ShortcutRemoveElement()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !widget_is_disabled(w, WC_TILE_INSPECTOR__WIDX_BUTTON_REMOVE)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_BUTTON_REMOVE].type != WWT_EMPTY)
    {
        window_event_mouse_up_call(w, WC_TILE_INSPECTOR__WIDX_BUTTON_REMOVE);
        return;
    }
}

static void ShortcutMoveElementUp()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !widget_is_disabled(w, WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_UP)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_UP].type != WWT_EMPTY)
    {
        window_event_mouse_up_call(w, WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_UP);
        return;
    }
}

static void ShortcutMoveElementDown()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !widget_is_disabled(w, WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_DOWN)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_DOWN].type != WWT_EMPTY)
    {
        window_event_mouse_up_call(w, WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_DOWN);
        return;
    }
}

static void ShortcutIncreaseXCoord()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !widget_is_disabled(w, WC_TILE_INSPECTOR__WIDX_SPINNER_X_INCREASE)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_SPINNER_X_INCREASE].type != WWT_EMPTY)
    {
        window_event_mouse_down_call(w, WC_TILE_INSPECTOR__WIDX_SPINNER_X_INCREASE);
        return;
    }
}

static void ShortcutDecreaseXCoord()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !widget_is_disabled(w, WC_TILE_INSPECTOR__WIDX_SPINNER_X_DECREASE)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_SPINNER_X_DECREASE].type != WWT_EMPTY)
    {
        window_event_mouse_down_call(w, WC_TILE_INSPECTOR__WIDX_SPINNER_X_DECREASE);
        return;
    }
}

static void ShortcutIncreaseYCoord()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !widget_is_disabled(w, WC_TILE_INSPECTOR__WIDX_SPINNER_Y_INCREASE)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_SPINNER_Y_INCREASE].type != WWT_EMPTY)
    {
        window_event_mouse_down_call(w, WC_TILE_INSPECTOR__WIDX_SPINNER_Y_INCREASE);
        return;
    }
}

static void ShortcutDecreaseYCoord()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !widget_is_disabled(w, WC_TILE_INSPECTOR__WIDX_SPINNER_Y_DECREASE)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_SPINNER_Y_DECREASE].type != WWT_EMPTY)
    {
        window_event_mouse_down_call(w, WC_TILE_INSPECTOR__WIDX_SPINNER_Y_DECREASE);
        return;
    }
}

static void ShortcutIncreaseElementHeight()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr)
    {
        int action = -1;
        switch (w->page)
        {
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_SURFACE:
                action = WC_TILE_INSPECTOR__WIDX_SURFACE_SPINNER_HEIGHT_INCREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_PATH:
                action = WC_TILE_INSPECTOR__WIDX_PATH_SPINNER_HEIGHT_INCREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_TRACK:
                action = WC_TILE_INSPECTOR__WIDX_TRACK_SPINNER_HEIGHT_INCREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_SCENERY:
                action = WC_TILE_INSPECTOR__WIDX_SCENERY_SPINNER_HEIGHT_INCREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_ENTRANCE:
                action = WC_TILE_INSPECTOR__WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_WALL:
                action = WC_TILE_INSPECTOR__WIDX_WALL_SPINNER_HEIGHT_INCREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_LARGE_SCENERY:
                action = WC_TILE_INSPECTOR__WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_BANNER:
                action = WC_TILE_INSPECTOR__WIDX_BANNER_SPINNER_HEIGHT_INCREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_CORRUPT:
                action = WC_TILE_INSPECTOR__WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE;
                break;
        }
        if (action != -1 && !widget_is_disabled(w, action) && w->widgets[action].type != WWT_EMPTY)
            window_event_mouse_down_call(w, action);
        return;
    }
}

static void ShortcutDecreaseElementHeight()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr)
    {
        int action = -1;
        switch (w->page)
        {
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_SURFACE:
                action = WC_TILE_INSPECTOR__WIDX_SURFACE_SPINNER_HEIGHT_DECREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_PATH:
                action = WC_TILE_INSPECTOR__WIDX_PATH_SPINNER_HEIGHT_DECREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_TRACK:
                action = WC_TILE_INSPECTOR__WIDX_TRACK_SPINNER_HEIGHT_DECREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_SCENERY:
                action = WC_TILE_INSPECTOR__WIDX_SCENERY_SPINNER_HEIGHT_DECREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_ENTRANCE:
                action = WC_TILE_INSPECTOR__WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_WALL:
                action = WC_TILE_INSPECTOR__WIDX_WALL_SPINNER_HEIGHT_DECREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_LARGE_SCENERY:
                action = WC_TILE_INSPECTOR__WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_BANNER:
                action = WC_TILE_INSPECTOR__WIDX_BANNER_SPINNER_HEIGHT_DECREASE;
                break;
            case WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_CORRUPT:
                action = WC_TILE_INSPECTOR__WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE;
                break;
        }
        if (action != -1 && !widget_is_disabled(w, action) && w->widgets[action].type != WWT_EMPTY)
            window_event_mouse_down_call(w, action);
        return;
    }
}

static void ShortcutToggleClearanceChecks()
{
    auto setCheatAction = SetCheatAction(CheatType::DisableClearanceChecks, gCheatsDisableClearanceChecks ? 0 : 1);
    GameActions::Execute(&setCheatAction);
}

namespace
{
    using namespace OpenRCT2::Input;
    const shortcut_action shortcut_table[ShortcutsCount] = {
        ShortcutCloseTopMostWindow,
        ShortcutCloseAllFloatingWindow,
        ShortcutCancelConstructionMode,
        ShortcutPauseGame,
        ShortcutZoomViewOut,
        ShortcutZoomViewIn,
        ShortcutRotateViewClockwise,
        ShortcutRotateViewAnticlockwise,
        ShortcutRotateConstructionObject,
        ShortcutUndergroundViewToggle,
        ShortcutRemoveBaseLandToggle,
        ShortcutRemoveVerticalLandToggle,
        ShortcutSeeThroughRidesToggle,
        ShortcutSeeThroughSceneryToggle,
        ShortcutInvisibleSupportsToggle,
        ShortcutInvisiblePeopleToggle,
        ShortcutHeightMarksOnLandToggle,
        ShortcutHeightMarksOnRideTracksToggle,
        ShortcutHeightMarksOnPathsToggle,
        ShortcutAdjustLand,
        ShortcutAdjustWater,
        ShortcutBuildScenery,
        ShortcutBuildPaths,
        ShortcutBuildNewRide,
        ShortcutShowFinancialInformation,
        ShortcutShowResearchInformation,
        ShortcutShowRidesList,
        ShortcutShowParkInformation,
        ShortcutShowGuestList,
        ShortcutShowStaffList,
        ShortcutShowRecentMessages,
        ShortcutShowMap,
        ShortcutScreenshot,

        // new
        ShortcutReduceGameSpeed,
        ShortcutIncreaseGameSpeed,
        ShortcutOpenCheatWindow,
        ShortcutRemoveTopBottomToolbarToggle,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        ShortcutOpenChatWindow,
        ShortcutQuickSaveGame,
        ShortcutShowOptions,
        ShortcutMuteSound,
        ShortcutWindowsModeToggle,
        ShortcutShowMultiplayer,
        nullptr,
        ShortcutDebugPaintToggle,
        ShortcutSeeThroughPathsToggle,
        ShortcutRideConstructionTurnLeft,
        ShortcutRideConstructionTurnRight,
        ShortcutRideConstructionUseTrackDefault,
        ShortcutRideConstructionSlopeDown,
        ShortcutRideConstructionSlopeUp,
        ShortcutRideConstructionChainLiftToggle,
        ShortcutRideConstructionBankLeft,
        ShortcutRideConstructionBankRight,
        ShortcutRideConstructionPreviousTrack,
        ShortcutRideConstructionNextTrack,
        ShortcutRideConstructionBuildCurrent,
        ShortcutRideConstructionDemolishCurrent,
        ShortcutLoadGame,
        ShortcutClearScenery,
        ShortcutGridlinesToggle,
        ShortcutViewClipping,
        ShortcutHighlightPathIssuesToggle,
        ShortcutOpenTileInspector,
        ShortcutAdvanceToNextTick,
        ShortcutOpenSceneryPicker,
        ShortcutScaleUp,
        ShortcutScaleDown,
        ShortcutInsertCorruptElement,
        ShortcutCopyElement,
        ShortcutPasteElement,
        ShortcutRemoveElement,
        ShortcutMoveElementUp,
        ShortcutMoveElementDown,
        ShortcutIncreaseXCoord,
        ShortcutDecreaseXCoord,
        ShortcutIncreaseYCoord,
        ShortcutDecreaseYCoord,
        ShortcutIncreaseElementHeight,
        ShortcutDecreaseElementHeight,
        ShortcutToggleClearanceChecks,
    };
} // anonymous namespace

#pragma endregion
