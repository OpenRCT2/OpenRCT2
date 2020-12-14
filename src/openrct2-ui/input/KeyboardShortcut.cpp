/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "KeyboardShortcuts.h"
#include "ShortcutManager.h"

#include <iterator>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/interface/InGameConsole.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/LoadOrQuitAction.h>
#include <openrct2/actions/SetCheatAction.h>
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
#include <openrct2/windows/tile_inspector.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>

extern bool gWindowSceneryEyedropperEnabled;

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

#pragma region Shortcut Commands

static void OpenWindow(rct_windowclass wc)
{
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        context_open_window(wc);
    }
}

static void RotateCamera(int32_t direction)
{
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        auto window = window_get_main();
        if (window != nullptr)
        {
            window_rotate_camera(window, direction);
        }
    }
}

static void ToggleViewFlag(int32_t viewportFlag)
{
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        auto window = window_get_main();
        if (window != nullptr)
        {
            window->viewport->flags ^= viewportFlag;
            window->Invalidate();
        }
    }
}

static void ShortcutRotateConstructionObject()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    // Rotate scenery
    rct_window* w = window_find_by_class(WC_SCENERY);
    if (w != nullptr && !WidgetIsDisabled(w, WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON)
        && w->widgets[WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type != WindowWidgetType::Empty)
    {
        window_event_mouse_up_call(w, WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON);
        return;
    }

    // Rotate construction track piece
    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != nullptr && !WidgetIsDisabled(w, WC_RIDE_CONSTRUCTION__WIDX_ROTATE)
        && w->widgets[WC_RIDE_CONSTRUCTION__WIDX_ROTATE].type != WindowWidgetType::Empty)
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
    if (w != nullptr && !WidgetIsDisabled(w, WC_TRACK_DESIGN_LIST__WIDX_ROTATE)
        && w->widgets[WC_TRACK_DESIGN_LIST__WIDX_ROTATE].type != WindowWidgetType::Empty)
    {
        window_event_mouse_up_call(w, WC_TRACK_DESIGN_LIST__WIDX_ROTATE);
        return;
    }

    // Rotate track design placement
    w = window_find_by_class(WC_TRACK_DESIGN_PLACE);
    if (w != nullptr && !WidgetIsDisabled(w, WC_TRACK_DESIGN_PLACE__WIDX_ROTATE)
        && w->widgets[WC_TRACK_DESIGN_PLACE__WIDX_ROTATE].type != WindowWidgetType::Empty)
    {
        window_event_mouse_up_call(w, WC_TRACK_DESIGN_PLACE__WIDX_ROTATE);
        return;
    }

    // Rotate park entrance
    w = window_find_by_class(WC_MAP);
    if (w != nullptr && !WidgetIsDisabled(w, WC_MAP__WIDX_ROTATE_90)
        && w->widgets[WC_MAP__WIDX_ROTATE_90].type != WindowWidgetType::Empty)
    {
        window_event_mouse_up_call(w, WC_MAP__WIDX_ROTATE_90);
        return;
    }

    // Rotate selected element in tile inspector
    w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !WidgetIsDisabled(w, WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE].type != WindowWidgetType::Empty)
    {
        window_event_mouse_up_call(w, WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE);
        return;
    }
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

static void ShortcutAdjustLand()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EditorStep::LandscapeEditor)
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

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EditorStep::LandscapeEditor)
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

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EditorStep::LandscapeEditor)
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

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EditorStep::LandscapeEditor)
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

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EditorStep::LandscapeEditor)
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
            context_open_window(WC_MAP);
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

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gS6Info.editor_step == EditorStep::LandscapeEditor)
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

static void ShortcutLoadGame()
{
    if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
    {
        auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::OpenSavePrompt);
        GameActions::Execute(&loadOrQuitAction);
    }
}

static void ShortcutOpenSceneryPicker()
{
    if ((gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        || (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR && gS6Info.editor_step != EditorStep::LandscapeEditor))
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
    if (window_scenery != nullptr && !WidgetIsDisabled(window_scenery, WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON)
        && window_scenery->widgets[WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON].type != WindowWidgetType::Empty
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
static void TileInspectorMouseUp(rct_widgetindex widgetIndex)
{
    auto w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !WidgetIsDisabled(w, widgetIndex) && w->widgets[widgetIndex].type != WindowWidgetType::Empty)
    {
        window_event_mouse_up_call(w, widgetIndex);
    }
}

static void TileInspectorMouseDown(rct_widgetindex widgetIndex)
{
    auto w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr && !WidgetIsDisabled(w, widgetIndex) && w->widgets[widgetIndex].type != WindowWidgetType::Empty)
    {
        window_event_mouse_down_call(w, widgetIndex);
    }
}

static void ShortcutIncreaseElementHeight()
{
    rct_window* w = window_find_by_class(WC_TILE_INSPECTOR);
    if (w != nullptr)
    {
        int action = -1;
        switch (w->tileInspectorPage)
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
            case TileInspectorPage::Default:
                break;
        }
        if (action != -1 && !WidgetIsDisabled(w, action) && w->widgets[action].type != WindowWidgetType::Empty)
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
        switch (w->tileInspectorPage)
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
            case TileInspectorPage::Default:
                break;
        }
        if (action != -1 && !WidgetIsDisabled(w, action) && w->widgets[action].type != WindowWidgetType::Empty)
            window_event_mouse_down_call(w, action);
        return;
    }
}

static void ShortcutToggleClearanceChecks()
{
    auto setCheatAction = SetCheatAction(CheatType::DisableClearanceChecks, gCheatsDisableClearanceChecks ? 0 : 1);
    GameActions::Execute(&setCheatAction);
}

static void ShortcutToggleConsole()
{
    auto& console = GetInGameConsole();
    if (console.IsOpen())
    {
        console.Toggle();
    }
    else if (gConfigGeneral.debugging_tools && !context_is_input_active())
    {
        window_cancel_textbox();
        console.Toggle();
    }
}

#pragma endregion

using namespace OpenRCT2::Ui;

void ShortcutManager::RegisterDefaultShortcuts()
{
    // clang-format off

    // Interface 0
    RegisterShortcut("interface0.close_top", STR_SHORTCUT_CLOSE_TOP_MOST_WINDOW, "BACKSPACE", []() { window_close_top(); });
    RegisterShortcut("interface0.close_all", STR_SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS, "SHIFT+BACKSPACE", []() {
        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR))
        {
            window_close_all();
        }
        else if (gS6Info.editor_step == EditorStep::LandscapeEditor)
        {
            window_close_top();
        }
    });
    RegisterShortcut("interface0.cancel_construction", STR_SHORTCUT_CANCEL_CONSTRUCTION_MODE, "ESCAPE", []() {
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            auto window = window_find_by_class(WC_ERROR);
            if (window != nullptr)
            {
                window_close(window);
            }
            else if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
            {
                tool_cancel();
            }
        }
    });
    RegisterShortcut("interface0.toggle_toolbars", STR_SHORTCUT_TOGGLE_VISIBILITY_OF_TOOLBARS, "T", []() {
        ShortcutRemoveTopBottomToolbarToggle();
    });

    // Interface 1
    RegisterShortcut("interface1.pause", STR_SHORTCUT_PAUSE_GAME, "PAUSE", []() {
        if (!(gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            auto window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != nullptr)
            {
                window->Invalidate();
                window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_PAUSE);
            }
        }
    });
    RegisterShortcut("interface1.decrease_speed", STR_SHORTCUT_REDUCE_GAME_SPEED , "-", []() { ShortcutReduceGameSpeed(); });
    RegisterShortcut("interface1.increase_speed", STR_SHORTCUT_INCREASE_GAME_SPEED , "=", []() { ShortcutIncreaseGameSpeed(); });
    RegisterShortcut("interface1.load_game", STR_LOAD_GAME, "CTRL+L", []() { ShortcutLoadGame(); });
    RegisterShortcut("interface1.save_game", STR_LOAD_GAME, "CTRL+F10", []() { ShortcutQuickSaveGame(); });
    RegisterShortcut("interface1.show_options", STR_SHORTCUT_SHOW_OPTIONS, []() { context_open_window(WC_OPTIONS); });
    RegisterShortcut("interface1.screenshot", STR_SHORTCUT_SCREENSHOT, "CTRL+S", []() { gScreenshotCountdown = 2; });
    RegisterShortcut("interface1.mute", STR_SHORTCUT_MUTE_SOUND, []() { OpenRCT2::Audio::ToggleAllSounds(); });

    // Interface 2
    RegisterShortcut("interface2.open_cheats", STR_SHORTCUT_OPEN_CHEATS_WINDOW, "CTRL+ALT+C", []() { ShortcutOpenCheatWindow(); });
    RegisterShortcut("interface2.disable_clearance", STR_SHORTCUT_TOGGLE_CLEARANCE_CHECKS, []() { ShortcutToggleClearanceChecks(); });

    // Interface 3
    RegisterShortcut("interface3.zoom_out", STR_SHORTCUT_ZOOM_VIEW_OUT, "PAGEUP", []() { main_window_zoom(false, false); });
    RegisterShortcut("interface3.zoom_in", STR_SHORTCUT_ZOOM_VIEW_IN, "PAGEDOWN", []() { main_window_zoom(true, false); });
    RegisterShortcut("interface3.rotate_clockwise", STR_SHORTCUT_ROTATE_VIEW_CLOCKWISE, "RETURN", "MOUSE:5", []() { RotateCamera(1); });
    RegisterShortcut("interface3.rotate_anticlockwise", STR_SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE, "SHIFT+RETURN", "MOUSE:4", []() { RotateCamera(-1); });
    RegisterShortcut("interface3.show_map", STR_SHORTCUT_TOGGLE_CLEARANCE_CHECKS, "TAB", []() { ShortcutShowMap(); });

    // Interface 4
    RegisterShortcut("interface4.clear_scenery", STR_SHORTCUT_CLEAR_SCENERY, "B", []() { ShortcutClearScenery(); });
    RegisterShortcut("interface4.adjust_land", STR_SHORTCUT_ADJUST_LAND, "F1", []() { ShortcutAdjustLand(); });
    RegisterShortcut("interface4.adjust_water", STR_SHORTCUT_ADJUST_WATER, "F2", []() { ShortcutAdjustWater(); });
    RegisterShortcut("interface4.build_scenery", STR_SHORTCUT_BUILD_SCENERY, "F3", []() { ShortcutBuildScenery(); });
    RegisterShortcut("interface4.build_footpaths", STR_SHORTCUT_BUILD_PATHS, "F4", []() { ShortcutBuildPaths(); });
    RegisterShortcut("interface4.build_new_ride", STR_SHORTCUT_BUILD_NEW_RIDE, "F5", []() { ShortcutBuildNewRide(); });

    // Interface 5
    RegisterShortcut("interface5.show_finances", STR_SHORTCUT_SHOW_FINANCIAL_INFORMATION, "F", []() { ShortcutShowFinancialInformation(); });
    RegisterShortcut("interface5.show_research", STR_SHORTCUT_SHOW_RESEARCH_INFORMATION, "D", []() { ShortcutShowResearchInformation(); });
    RegisterShortcut("interface5.show_rides", STR_SHORTCUT_SHOW_RIDES_LIST, "R", []() { ShortcutShowRidesList(); });
    RegisterShortcut("interface5.show_park", STR_SHORTCUT_SHOW_PARK_INFORMATION, "P", []() { ShortcutShowParkInformation(); });
    RegisterShortcut("interface5.show_guests", STR_SHORTCUT_SHOW_GUEST_LIST, "G", []() { ShortcutShowGuestList(); });
    RegisterShortcut("interface5.show_staff", STR_SHORTCUT_SHOW_STAFF_LIST, "S", []() { ShortcutShowStaffList(); });
    RegisterShortcut("interface5.show_messages", STR_SHORTCUT_SHOW_RECENT_MESSAGES, "M", []() { ShortcutShowRecentMessages(); });

    // Multiplayer
    RegisterShortcut("multiplayer.show", STR_SHORTCUT_SHOW_MULTIPLAYER, []() {
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            OpenWindow(WC_MULTIPLAYER);
        }
    });
    RegisterShortcut("multiplayer.chat", STR_SEND_MESSAGE, "C", []() {
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            chat_toggle();
        }
    });

    // View
    RegisterShortcut("view.show_underground", STR_SHORTCUT_UNDERGROUND_VIEW_TOGGLE, "1", []() { ToggleViewFlag(VIEWPORT_FLAG_UNDERGROUND_INSIDE); });
    RegisterShortcut("view.hide_base_land", STR_SHORTCUT_REMOVE_BASE_LAND_TOGGLE, "H", []() { ToggleViewFlag(VIEWPORT_FLAG_HIDE_BASE); });
    RegisterShortcut("view.hide_vertical_land", STR_SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE, "V", []() { ToggleViewFlag(VIEWPORT_FLAG_HIDE_BASE); });
    RegisterShortcut("view.transparent_rides", STR_SHORTCUT_SEE_THROUGH_RIDES_TOGGLE, "3", []() { ToggleViewFlag(VIEWPORT_FLAG_SEETHROUGH_RIDES); });
    RegisterShortcut("view.transparent_scenery", STR_SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE, "4", []() { ToggleViewFlag(VIEWPORT_FLAG_SEETHROUGH_SCENERY); });
    RegisterShortcut("view.transparent_scenery", STR_SHORTCUT_SEE_THROUGH_PATHS_TOGGLE, []() { ToggleViewFlag(VIEWPORT_FLAG_SEETHROUGH_PATHS); });
    RegisterShortcut("view.hide_supports", STR_SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE, "5", []() { ToggleViewFlag(VIEWPORT_FLAG_INVISIBLE_SUPPORTS); });
    RegisterShortcut("view.hide_peeps", STR_SHORTCUT_INVISIBLE_PEOPLE_TOGGLE, "6", []() { ToggleViewFlag(VIEWPORT_FLAG_INVISIBLE_PEEPS); });
    RegisterShortcut("view.show_land_height", STR_SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE, "8", []() { ToggleViewFlag(VIEWPORT_FLAG_LAND_HEIGHTS); });
    RegisterShortcut("view.show_track_height", STR_SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE, "9", []() { ToggleViewFlag(VIEWPORT_FLAG_TRACK_HEIGHTS); });
    RegisterShortcut("view.show_footpath_height", STR_SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE, "0", []() { ToggleViewFlag(VIEWPORT_FLAG_PATH_HEIGHTS); });
    RegisterShortcut("view.toggle_cut_away", STR_SHORTCUT_VIEW_CLIPPING, []() { OpenWindow(WC_VIEW_CLIPPING); });
    RegisterShortcut("view.highlight_path_issues", STR_SHORTCUT_HIGHLIGHT_PATH_ISSUES_TOGGLE, "I", []() { ToggleViewFlag(VIEWPORT_FLAG_PATH_HEIGHTS); });
    RegisterShortcut("view.show_gridlines", STR_SHORTCUT_GRIDLINES_DISPLAY_TOGGLE, "7", []() { ToggleViewFlag(VIEWPORT_FLAG_GRIDLINES); });

    // Interface 6
    RegisterShortcut("interface6.scenery_picker", STR_SHORTCUT_OPEN_SCENERY_PICKER, []() { ShortcutOpenSceneryPicker(); });
    RegisterShortcut("interface6.rotate_construction", STR_SHORTCUT_ROTATE_CONSTRUCTION_OBJECT, "Z", []() { ShortcutRotateConstructionObject(); });

    // Ride construction

    // Map scrolling
    RegisterShortcut("scroll.up", STR_SHORTCUT_SCROLL_MAP_UP, "UP", []() { });
    RegisterShortcut("scroll.left", STR_SHORTCUT_SCROLL_MAP_LEFT, "LEFT", []() { });
    RegisterShortcut("scroll.right", STR_SHORTCUT_SCROLL_MAP_RIGHT, "RIGHT", []() { });
    RegisterShortcut("scroll.down", STR_SHORTCUT_SCROLL_MAP_DOWN, "DOWN", []() { });

    // Window scale
    RegisterShortcut("scale.toggle_window_mode", STR_SHORTCUT_WINDOWED_MODE_TOGGLE, "ALT+RETURN", []() { platform_toggle_windowed_mode(); });
    RegisterShortcut("scale.increase", STR_SHORTCUT_SCALE_UP, []() { ShortcutScaleUp(); });
    RegisterShortcut("scale.decrease", STR_SHORTCUT_SCALE_DOWN, []() { ShortcutScaleDown(); });

    // Ride construction
    RegisterShortcut("rideconstruction.turn_left", STR_SHORTCUT_RIDE_CONSTRUCTION_TURN_LEFT, "KEYPAD 4", []() { window_ride_construction_keyboard_shortcut_turn_left(); });
    RegisterShortcut("rideconstruction.turn_right", STR_SHORTCUT_RIDE_CONSTRUCTION_TURN_RIGHT, "KEYPAD 6", []() { window_ride_construction_keyboard_shortcut_turn_right(); });
    RegisterShortcut("rideconstruction.default", STR_SHORTCUT_RIDE_CONSTRUCTION_USE_TRACK_DEFAULT, "KEYPAD 5", []() { window_ride_construction_keyboard_shortcut_use_track_default(); });
    RegisterShortcut("rideconstruction.slope_down", STR_SHORTCUT_RIDE_CONSTRUCTION_SLOPE_DOWN, "KEYPAD 2", []() { window_ride_construction_keyboard_shortcut_slope_down(); });
    RegisterShortcut("rideconstruction.slope_up", STR_SHORTCUT_RIDE_CONSTRUCTION_SLOPE_UP, "KEYPAD 8", []() { window_ride_construction_keyboard_shortcut_slope_up(); });
    RegisterShortcut("rideconstruction.chain_lift", STR_SHORTCUT_RIDE_CONSTRUCTION_CHAIN_LIFT_TOGGLE, "KEYPAD +", []() { window_ride_construction_keyboard_shortcut_chain_lift_toggle(); });
    RegisterShortcut("rideconstruction.bank_left", STR_SHORTCUT_RIDE_CONSTRUCTION_BANK_LEFT, "KEYPAD 1", []() { window_ride_construction_keyboard_shortcut_bank_left(); });
    RegisterShortcut("rideconstruction.bank_right", STR_SHORTCUT_RIDE_CONSTRUCTION_BANK_RIGHT, "KEYPAD 3", []() { window_ride_construction_keyboard_shortcut_bank_right(); });
    RegisterShortcut("rideconstruction.previous", STR_SHORTCUT_RIDE_CONSTRUCTION_PREVIOUS_TRACK, "KEYPAD 7", []() { window_ride_construction_keyboard_shortcut_previous_track(); });
    RegisterShortcut("rideconstruction.next", STR_SHORTCUT_RIDE_CONSTRUCTION_NEXT_TRACK, "KEYPAD 9", []() { window_ride_construction_keyboard_shortcut_next_track(); });
    RegisterShortcut("rideconstruction.build", STR_SHORTCUT_RIDE_CONSTRUCTION_BUILD_CURRENT, "KEYPAD 0", []() { window_ride_construction_keyboard_shortcut_build_current(); });
    RegisterShortcut("rideconstruction.demolish", STR_SHORTCUT_RIDE_CONSTRUCTION_DEMOLISH_CURRENT, "KEYPAD -", []() { window_ride_construction_keyboard_shortcut_demolish_current(); });

    // Tile inspector
    RegisterShortcut("tileinspector.open", STR_SHORTCUT_OPEN_TILE_INSPECTOR, []() {
        if (gConfigInterface.toolbar_show_cheats)
        {
            OpenWindow(WC_TILE_INSPECTOR);
        }
    });
    RegisterShortcut("tileinspector.insert_corrupt", STR_SHORTCUT_INSERT_CORRPUT_ELEMENT, []() { TileInspectorMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_CORRUPT); });
    RegisterShortcut("tileinspector.copy", STR_SHORTCUT_COPY_ELEMENT, []() { TileInspectorMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_COPY); });
    RegisterShortcut("tileinspector.paste", STR_SHORTCUT_PASTE_ELEMENT, []() { TileInspectorMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_PASTE); });
    RegisterShortcut("tileinspector.remove", STR_SHORTCUT_REMOVE_ELEMENT, []() { TileInspectorMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_REMOVE); });
    RegisterShortcut("tileinspector.move_up", STR_SHORTCUT_MOVE_ELEMENT_UP, []() { TileInspectorMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_UP); });
    RegisterShortcut("tileinspector.move_down", STR_SHORTCUT_MOVE_ELEMENT_DOWN, []() { TileInspectorMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_DOWN); });
    RegisterShortcut("tileinspector.increase_x", STR_SHORTCUT_INCREASE_X_COORD, []() { TileInspectorMouseDown(WC_TILE_INSPECTOR__WIDX_SPINNER_X_INCREASE); });
    RegisterShortcut("tileinspector.decrease_x", STR_SHORTCUT_DECREASE_X_COORD, []() { TileInspectorMouseDown(WC_TILE_INSPECTOR__WIDX_SPINNER_X_DECREASE); });
    RegisterShortcut("tileinspector.increase_y", STR_SHORTCUT_INCREASE_Y_COORD, []() { TileInspectorMouseDown(WC_TILE_INSPECTOR__WIDX_SPINNER_Y_INCREASE); });
    RegisterShortcut("tileinspector.decrease_y", STR_SHORTCUT_DECREASE_Y_COORD, []() { TileInspectorMouseDown(WC_TILE_INSPECTOR__WIDX_SPINNER_Y_DECREASE); });
    RegisterShortcut("tileinspector.increase_height", STR_SHORTCUT_INCREASE_ELEM_HEIGHT, []() { ShortcutIncreaseElementHeight(); });
    RegisterShortcut("tileinspector.decrease_height", STR_SHORTCUT_DECREASE_ELEM_HEIGHT, []() { ShortcutDecreaseElementHeight(); });

    // Debug
    RegisterShortcut(SHORTCUT_ID_DEBUG_CONSOLE, STR_CONSOLE, "`", []() { ShortcutToggleConsole(); });
    RegisterShortcut("debug.advance_tick", STR_ADVANCE_TO_NEXT_TICK, []() {
        if (!(gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            gDoSingleUpdate = true;
        }
    });
    RegisterShortcut("debug.toggle_paint_debug_window", STR_SHORTCUT_DEBUG_PAINT_TOGGLE, []() {
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            auto window = window_find_by_class(WC_DEBUG_PAINT);
            if (window != nullptr)
            {
                window_close(window);
            }
            else
            {
                context_open_window(WC_DEBUG_PAINT);
            }
        }
    });
    // clang-format on
}
