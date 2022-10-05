/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ShortcutIds.h"
#include "ShortcutManager.h"

#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/interface/InGameConsole.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Window.h>
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
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackPaint.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/title/TitleScreen.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/windows/tile_inspector.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

#pragma region Shortcut Commands

static void OpenWindow(WindowClass wc)
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
            window_rotate_camera(*window, direction);
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
    rct_window* w = window_find_by_class(WindowClass::Scenery);
    if (w != nullptr && !WidgetIsDisabled(*w, WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON)
        && w->widgets[WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type != WindowWidgetType::Empty)
    {
        window_event_mouse_up_call(w, WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON);
        return;
    }

    // Rotate construction track piece
    w = window_find_by_class(WindowClass::RideConstruction);
    if (w != nullptr && !WidgetIsDisabled(*w, WC_RIDE_CONSTRUCTION__WIDX_ROTATE)
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
    w = window_find_by_class(WindowClass::TrackDesignList);
    if (w != nullptr && !WidgetIsDisabled(*w, WC_TRACK_DESIGN_LIST__WIDX_ROTATE)
        && w->widgets[WC_TRACK_DESIGN_LIST__WIDX_ROTATE].type != WindowWidgetType::Empty)
    {
        window_event_mouse_up_call(w, WC_TRACK_DESIGN_LIST__WIDX_ROTATE);
        return;
    }

    // Rotate track design placement
    w = window_find_by_class(WindowClass::TrackDesignPlace);
    if (w != nullptr && !WidgetIsDisabled(*w, WC_TRACK_DESIGN_PLACE__WIDX_ROTATE)
        && w->widgets[WC_TRACK_DESIGN_PLACE__WIDX_ROTATE].type != WindowWidgetType::Empty)
    {
        window_event_mouse_up_call(w, WC_TRACK_DESIGN_PLACE__WIDX_ROTATE);
        return;
    }

    // Rotate park entrance
    w = window_find_by_class(WindowClass::Map);
    if (w != nullptr && !WidgetIsDisabled(*w, WC_MAP__WIDX_ROTATE_90)
        && w->widgets[WC_MAP__WIDX_ROTATE_90].type != WindowWidgetType::Empty)
    {
        window_event_mouse_up_call(w, WC_MAP__WIDX_ROTATE_90);
        return;
    }

    // Rotate selected element in tile inspector
    w = window_find_by_class(WindowClass::TileInspector);
    if (w != nullptr && !WidgetIsDisabled(*w, WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE)
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
        if (window_find_by_class(WindowClass::TitleLogo) != nullptr)
        {
            window_close(*window_find_by_class(WindowClass::TitleLogo));
            window_close(*window_find_by_class(WindowClass::TitleOptions));
            window_close(*window_find_by_class(WindowClass::TitleMenu));
            window_close(*window_find_by_class(WindowClass::TitleExit));
            title_set_hide_version_info(true);
        }
        else
        {
            title_create_windows();
        }
    }
    else
    {
        if (window_find_by_class(WindowClass::TopToolbar) != nullptr)
        {
            window_close(*window_find_by_class(WindowClass::Dropdown));
            window_close(*window_find_by_class(WindowClass::TopToolbar));
            window_close(*window_find_by_class(WindowClass::BottomToolbar));
        }
        else
        {
            if (gScreenFlags == 0)
            {
                context_open_window(WindowClass::TopToolbar);
                context_open_window(WindowClass::BottomToolbar);
            }
            else
            {
                context_open_window(WindowClass::TopToolbar);
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

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gEditorStep == EditorStep::LandscapeEditor)
    {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            rct_window* window = window_find_by_class(WindowClass::TopToolbar);
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

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gEditorStep == EditorStep::LandscapeEditor)
    {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            rct_window* window = window_find_by_class(WindowClass::TopToolbar);
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

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gEditorStep == EditorStep::LandscapeEditor)
    {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            rct_window* window = window_find_by_class(WindowClass::TopToolbar);
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

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gEditorStep == EditorStep::LandscapeEditor)
    {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            rct_window* window = window_find_by_class(WindowClass::TopToolbar);
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
            context_open_window(WindowClass::ConstructRide);
        }
    }
}

static void ShortcutShowFinancialInformation()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
            context_open_window(WindowClass::Finances);
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
        context_open_window(WindowClass::RideList);
    }
}

static void ShortcutShowParkInformation()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        context_open_window(WindowClass::ParkInformation);
    }
}

static void ShortcutShowGuestList()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        context_open_window(WindowClass::GuestList);
    }
}

static void ShortcutShowStaffList()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        context_open_window(WindowClass::StaffList);
    }
}

static void ShortcutShowRecentMessages()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
        context_open_window(WindowClass::RecentNews);
}

static void ShortcutShowMap()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gEditorStep == EditorStep::LandscapeEditor)
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
            context_open_window(WindowClass::Map);
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
    rct_window* window = window_find_by_class(WindowClass::Cheats);
    if (window != nullptr)
    {
        window_close(*window);
        return;
    }
    context_open_window(WindowClass::Cheats);
}

static void ShortcutOpenTransparencyWindow()
{
    if (gScreenFlags != SCREEN_FLAGS_PLAYING)
        return;

    context_open_window(WindowClass::Transparency);
}

static void ShortcutClearScenery()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gEditorStep == EditorStep::LandscapeEditor)
    {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            rct_window* window = window_find_by_class(WindowClass::TopToolbar);
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
        auto intent = Intent(WindowClass::Loadsave);
        intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE);
        intent.putExtra(INTENT_EXTRA_PATH, gScenarioName);
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
        || (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR && gEditorStep != EditorStep::LandscapeEditor))
        return;

    rct_window* window_scenery = window_find_by_class(WindowClass::Scenery);
    if (window_scenery == nullptr)
    {
        rct_window* window_toolbar = window_find_by_class(WindowClass::TopToolbar);
        if (window_toolbar != nullptr)
        {
            window_toolbar->Invalidate();
            window_event_mouse_up_call(window_toolbar, WC_TOP_TOOLBAR__WIDX_SCENERY);
        }
    }

    window_scenery = window_find_by_class(WindowClass::Scenery);
    if (window_scenery != nullptr && !WidgetIsDisabled(*window_scenery, WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON)
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
static void TileInspectorMouseUp(WidgetIndex widgetIndex)
{
    auto w = window_find_by_class(WindowClass::TileInspector);
    if (w != nullptr && !WidgetIsDisabled(*w, widgetIndex) && w->widgets[widgetIndex].type != WindowWidgetType::Empty)
    {
        window_event_mouse_up_call(w, widgetIndex);
    }
}

static void TileInspectorMouseDown(WidgetIndex widgetIndex)
{
    auto w = window_find_by_class(WindowClass::TileInspector);
    if (w != nullptr && !WidgetIsDisabled(*w, widgetIndex) && w->widgets[widgetIndex].type != WindowWidgetType::Empty)
    {
        window_event_mouse_down_call(w, widgetIndex);
    }
}

static void ShortcutToggleVisibility()
{
    // TODO: Once the tile inspector window has its own class, move this to its own function
    if (windowTileInspectorSelectedIndex < 0)
        return;

    rct_window* w = window_find_by_class(WindowClass::TileInspector);
    if (w == nullptr)
        return;

    extern TileCoordsXY windowTileInspectorTile;
    TileElement* tileElement = map_get_nth_element_at(windowTileInspectorTile.ToCoordsXY(), windowTileInspectorSelectedIndex);
    if (tileElement != nullptr)
    {
        tileElement->SetInvisible(!tileElement->IsInvisible());
        w->Invalidate();
    }
}

static void ShortcutIncreaseElementHeight()
{
    rct_window* w = window_find_by_class(WindowClass::TileInspector);
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
            case TileInspectorPage::Default:
                break;
        }
        if (action != -1 && !WidgetIsDisabled(*w, action) && w->widgets[action].type != WindowWidgetType::Empty)
            window_event_mouse_down_call(w, action);
        return;
    }
}

static void ShortcutDecreaseElementHeight()
{
    rct_window* w = window_find_by_class(WindowClass::TileInspector);
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
            case TileInspectorPage::Default:
                break;
        }
        if (action != -1 && !WidgetIsDisabled(*w, action) && w->widgets[action].type != WindowWidgetType::Empty)
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

static void ShortcutConstructionTurnLeft()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    rct_window* window = window_find_by_class(WindowClass::Footpath);
    if (window != nullptr)
    {
        window_footpath_keyboard_shortcut_turn_left();
    }
    else
    {
        window_ride_construction_keyboard_shortcut_turn_left();
    }
}

static void ShortcutConstructionTurnRight()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;
    rct_window* window = window_find_by_class(WindowClass::Footpath);
    if (window != nullptr)
    {
        window_footpath_keyboard_shortcut_turn_right();
    }
    else
    {
        window_ride_construction_keyboard_shortcut_turn_right();
    }
}

static void ShortcutConstructionSlopeUp()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    rct_window* window = window_find_by_class(WindowClass::Footpath);
    if (window != nullptr)
    {
        window_footpath_keyboard_shortcut_slope_up();
    }
    else
    {
        window_ride_construction_keyboard_shortcut_slope_up();
    }
}

static void ShortcutConstructionBuildCurrent()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    rct_window* window = window_find_by_class(WindowClass::Footpath);
    if (window != nullptr)
    {
        window_footpath_keyboard_shortcut_build_current();
    }
    else
    {
        window_ride_construction_keyboard_shortcut_build_current();
    }
}

static void ShortcutConstructionSlopeDown()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    rct_window* window = window_find_by_class(WindowClass::Footpath);
    if (window != nullptr)
    {
        window_footpath_keyboard_shortcut_slope_down();
    }
    else
    {
        window_ride_construction_keyboard_shortcut_slope_down();
    }
}

static void ShortcutConstructionDemolishCurrent()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    rct_window* window = window_find_by_class(WindowClass::Footpath);
    if (window != nullptr)
    {
        window_footpath_keyboard_shortcut_demolish_current();
    }
    else
    {
        window_ride_construction_keyboard_shortcut_demolish_current();
    }
}

static void ShortcutToggleTransparentWater()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    gConfigGeneral.transparent_water ^= 1;
    config_save_default();
    gfx_invalidate_screen();
}

#pragma endregion

using namespace OpenRCT2::Ui;

void ShortcutManager::RegisterDefaultShortcuts()
{
    // clang-format off

    // Interface
    RegisterShortcut(ShortcutId::InterfaceCloseTop, STR_SHORTCUT_CLOSE_TOP_MOST_WINDOW, "BACKSPACE", []() { window_close_top(); });
    RegisterShortcut(ShortcutId::InterfaceCloseAll, STR_SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS, "SHIFT+BACKSPACE", []() {
        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR))
        {
            window_close_all();
        }
        else if (gEditorStep == EditorStep::LandscapeEditor)
        {
            window_close_top();
        }
    });
    RegisterShortcut(ShortcutId::InterfaceRotateConstruction, STR_SHORTCUT_ROTATE_CONSTRUCTION_OBJECT, "Z", []() { ShortcutRotateConstructionObject(); });
    RegisterShortcut(ShortcutId::InterfaceCancelConstruction, STR_SHORTCUT_CANCEL_CONSTRUCTION_MODE, "ESCAPE", []() {
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            auto window = window_find_by_class(WindowClass::Error);
            if (window != nullptr)
            {
                window_close(*window);
            }
            else if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
            {
                tool_cancel();
            }
        }
    });
    RegisterShortcut(ShortcutId::InterfacePause, STR_SHORTCUT_PAUSE_GAME, "PAUSE", []() {
        if (!(gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            auto window = window_find_by_class(WindowClass::TopToolbar);
            if (window != nullptr)
            {
                window->Invalidate();
                window_event_mouse_up_call(window, WC_TOP_TOOLBAR__WIDX_PAUSE);
            }
        }
    });

    RegisterShortcut(ShortcutId::ScaleToggleWindowMode, STR_SHORTCUT_WINDOWED_MODE_TOGGLE, "ALT+RETURN", []() { ToggleWindowedMode(); });
    RegisterShortcut(ShortcutId::InterfaceScaleIncrease, STR_SHORTCUT_SCALE_UP, []() { ShortcutScaleUp(); });
    RegisterShortcut(ShortcutId::InterfaceScaleDecrease, STR_SHORTCUT_SCALE_DOWN, []() { ShortcutScaleDown(); });

    RegisterShortcut(ShortcutId::InterfaceToggleToolbars, STR_SHORTCUT_TOGGLE_VISIBILITY_OF_TOOLBARS, "T", []() {
        ShortcutRemoveTopBottomToolbarToggle();
    });
    RegisterShortcut(ShortcutId::InterfaceDecreaseSpeed, STR_SHORTCUT_REDUCE_GAME_SPEED , "-", []() { ShortcutReduceGameSpeed(); });
    RegisterShortcut(ShortcutId::InterfaceIncreaseSpeed, STR_SHORTCUT_INCREASE_GAME_SPEED , "=", []() { ShortcutIncreaseGameSpeed(); });
    RegisterShortcut(ShortcutId::InterfaceLoadGame, STR_LOAD_GAME, "CTRL+L", []() { ShortcutLoadGame(); });
    RegisterShortcut(ShortcutId::InterfaceSaveGame, STR_SAVE_GAME, "CTRL+F10", []() { ShortcutQuickSaveGame(); });
    RegisterShortcut(ShortcutId::InterfaceScreenshot, STR_SHORTCUT_SCREENSHOT, "CTRL+S", []() { gScreenshotCountdown = 2; });
    RegisterShortcut(ShortcutId::InterfaceGiantScreenshot, STR_SHORTCUT_GIANT_SCREENSHOT, "CTRL+SHIFT+S", []() { screenshot_giant(); });
    RegisterShortcut(ShortcutId::InterfaceMute, STR_SHORTCUT_MUTE_SOUND, []() { OpenRCT2::Audio::ToggleAllSounds(); });
    RegisterShortcut(ShortcutId::InterfaceDisableClearance, STR_SHORTCUT_TOGGLE_CLEARANCE_CHECKS, []() { ShortcutToggleClearanceChecks(); });
    RegisterShortcut(ShortcutId::MultiplayerChat, STR_SEND_MESSAGE, "C", []() {
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            if (chat_available())
            {
                chat_toggle();
            }
        }
    });
    RegisterShortcut(ShortcutId::InterfaceSceneryPicker, STR_SHORTCUT_OPEN_SCENERY_PICKER, []() { ShortcutOpenSceneryPicker(); });

    RegisterShortcut(ShortcutId::InterfaceShowOptions, STR_SHORTCUT_SHOW_OPTIONS, []() { context_open_window(WindowClass::Options); });
    RegisterShortcut(ShortcutId::InterfaceOpenTransparencyOptions, STR_SHORTCUT_OPEN_TRANSPARENCY_OPTIONS, "CTRL+T", []() { ShortcutOpenTransparencyWindow(); });
    RegisterShortcut(ShortcutId::InterfaceOpenCheats, STR_SHORTCUT_OPEN_CHEATS_WINDOW, "CTRL+ALT+C", []() { ShortcutOpenCheatWindow(); });
    RegisterShortcut(ShortcutId::InterfaceOpenMap, STR_SHORTCUT_SHOW_MAP, "TAB", []() { ShortcutShowMap(); });
    RegisterShortcut(ShortcutId::InterfaceClearScenery, STR_SHORTCUT_CLEAR_SCENERY, "B", []() { ShortcutClearScenery(); });
    RegisterShortcut(ShortcutId::InterfaceOpenLand, STR_SHORTCUT_ADJUST_LAND, "F1", []() { ShortcutAdjustLand(); });
    RegisterShortcut(ShortcutId::InterfaceOpenWater, STR_SHORTCUT_ADJUST_WATER, "F2", []() { ShortcutAdjustWater(); });
    RegisterShortcut(ShortcutId::InterfaceOpenScenery, STR_SHORTCUT_BUILD_SCENERY, "F3", []() { ShortcutBuildScenery(); });
    RegisterShortcut(ShortcutId::InterfaceOpenFootpaths, STR_SHORTCUT_BUILD_PATHS, "F4", []() { ShortcutBuildPaths(); });
    RegisterShortcut(ShortcutId::InterfaceOpenNewRide, STR_SHORTCUT_BUILD_NEW_RIDE, "F5", []() { ShortcutBuildNewRide(); });
    RegisterShortcut(ShortcutId::InterfaceOpenFinances, STR_SHORTCUT_SHOW_FINANCIAL_INFORMATION, "F", []() { ShortcutShowFinancialInformation(); });
    RegisterShortcut(ShortcutId::InterfaceOpenResearch, STR_SHORTCUT_SHOW_RESEARCH_INFORMATION, "D", []() { ShortcutShowResearchInformation(); });
    RegisterShortcut(ShortcutId::InterfaceOpenRides, STR_SHORTCUT_SHOW_RIDES_LIST, "R", []() { ShortcutShowRidesList(); });
    RegisterShortcut(ShortcutId::InterfaceOpenPark, STR_SHORTCUT_SHOW_PARK_INFORMATION, "P", []() { ShortcutShowParkInformation(); });
    RegisterShortcut(ShortcutId::InterfaceOpenGuests, STR_SHORTCUT_SHOW_GUEST_LIST, "G", []() { ShortcutShowGuestList(); });
    RegisterShortcut(ShortcutId::InterfaceOpenStaff, STR_SHORTCUT_SHOW_STAFF_LIST, "S", []() { ShortcutShowStaffList(); });
    RegisterShortcut(ShortcutId::InterfaceOpenMessages, STR_SHORTCUT_SHOW_RECENT_MESSAGES, "M", []() { ShortcutShowRecentMessages(); });
    RegisterShortcut(ShortcutId::MultiplayerShow, STR_SHORTCUT_SHOW_MULTIPLAYER, []() {
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            OpenWindow(WindowClass::Multiplayer);
        }
    });
    RegisterShortcut(ShortcutId::InterfaceOpenTileInspector, STR_SHORTCUT_OPEN_TILE_INSPECTOR, []() {
        if (gConfigInterface.toolbar_show_cheats)
        {
            OpenWindow(WindowClass::TileInspector);
        }
    });

    // View
    RegisterShortcut(ShortcutId::ViewGeneralZoomOut, STR_SHORTCUT_ZOOM_VIEW_OUT, "PAGEUP", []() { main_window_zoom(false, false); });
    RegisterShortcut(ShortcutId::ViewGeneralZoomIn, STR_SHORTCUT_ZOOM_VIEW_IN, "PAGEDOWN", []() { main_window_zoom(true, false); });
    RegisterShortcut(ShortcutId::ViewGeneralRotateClockwise, STR_SHORTCUT_ROTATE_VIEW_CLOCKWISE, "RETURN", "MOUSE 6", []() { RotateCamera(1); });
    RegisterShortcut(ShortcutId::ViewGeneralRotateAnticlockwise, STR_SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE, "SHIFT+RETURN", "MOUSE 5", []() { RotateCamera(-1); });

    RegisterShortcut(ShortcutId::ViewScrollUp, STR_SHORTCUT_SCROLL_MAP_UP, "UP", []() { });
    RegisterShortcut(ShortcutId::ViewScrollLeft, STR_SHORTCUT_SCROLL_MAP_LEFT, "LEFT", []() { });
    RegisterShortcut(ShortcutId::ViewScrollRight, STR_SHORTCUT_SCROLL_MAP_RIGHT, "RIGHT", []() { });
    RegisterShortcut(ShortcutId::ViewScrollDown, STR_SHORTCUT_SCROLL_MAP_DOWN, "DOWN", []() { });

    RegisterShortcut(ShortcutId::ViewToggleUnderground, STR_SHORTCUT_UNDERGROUND_VIEW_TOGGLE, "1", []() { ToggleViewFlag(VIEWPORT_FLAG_UNDERGROUND_INSIDE); });
    RegisterShortcut(ShortcutId::ViewToggleTransparentWater, STR_VIEWPORT_TRANSPARENT_WATER, "2", []() { ShortcutToggleTransparentWater(); });
    RegisterShortcut(ShortcutId::ViewToggleBaseLand, STR_SHORTCUT_REMOVE_BASE_LAND_TOGGLE, "H", []() { ToggleViewFlag(VIEWPORT_FLAG_HIDE_BASE); });
    RegisterShortcut(ShortcutId::ViewToggleVerticalLand, STR_SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE, "V", []() { ToggleViewFlag(VIEWPORT_FLAG_HIDE_VERTICAL); });
    RegisterShortcut(ShortcutId::ViewToggleRides, STR_SHORTCUT_SEE_THROUGH_RIDES_TOGGLE, "3", []() { ToggleViewFlag(VIEWPORT_FLAG_HIDE_RIDES); });
    RegisterShortcut(ShortcutId::ViewToggleVehicles, STR_SHORTCUT_SEE_THROUGH_VEHICLES_TOGGLE, []() { ToggleViewFlag(VIEWPORT_FLAG_HIDE_VEHICLES); });
    RegisterShortcut(ShortcutId::ViewToggleVegetation, STR_SHORTCUT_SEE_THROUGH_VEGETATION_TOGGLE, []() { ToggleViewFlag(VIEWPORT_FLAG_HIDE_VEGETATION); });
    RegisterShortcut(ShortcutId::ViewToggleScenery, STR_SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE, "4", []() { ToggleViewFlag(VIEWPORT_FLAG_HIDE_SCENERY); });
    RegisterShortcut(ShortcutId::ViewToggleFootpaths, STR_SHORTCUT_SEE_THROUGH_PATHS_TOGGLE, []() { ToggleViewFlag(VIEWPORT_FLAG_HIDE_PATHS); });
    RegisterShortcut(ShortcutId::ViewToggleSupports, STR_SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE, "5", []() { ToggleViewFlag(VIEWPORT_FLAG_HIDE_SUPPORTS); });
    RegisterShortcut(ShortcutId::ViewToggleGuests, STR_SHORTCUT_SEE_THROUGH_GUESTS_TOGGLE, "6", []() { ToggleViewFlag(VIEWPORT_FLAG_HIDE_GUESTS); });
    RegisterShortcut(ShortcutId::ViewToggleStaff, STR_SHORTCUT_SEE_THROUGH_STAFF_TOGGLE, []() { ToggleViewFlag(VIEWPORT_FLAG_HIDE_STAFF); });
    RegisterShortcut(ShortcutId::ViewToggleLandHeightMarkers, STR_SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE, "8", []() { ToggleViewFlag(VIEWPORT_FLAG_LAND_HEIGHTS); });
    RegisterShortcut(ShortcutId::ViewToggleTrackHeightMarkers, STR_SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE, "9", []() { ToggleViewFlag(VIEWPORT_FLAG_TRACK_HEIGHTS); });
    RegisterShortcut(ShortcutId::ViewToggleFootpathHeightMarkers, STR_SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE, "0", []() { ToggleViewFlag(VIEWPORT_FLAG_PATH_HEIGHTS); });
    RegisterShortcut(ShortcutId::ViewToggleCutAway, STR_SHORTCUT_VIEW_CLIPPING, []() { OpenWindow(WindowClass::ViewClipping); });
    RegisterShortcut(ShortcutId::ViewToogleFootpathIssues, STR_SHORTCUT_HIGHLIGHT_PATH_ISSUES_TOGGLE, "I", []() { ToggleViewFlag(VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES); });
    RegisterShortcut(ShortcutId::ViewToggleGridlines, STR_SHORTCUT_GRIDLINES_DISPLAY_TOGGLE, "7", []() { ToggleViewFlag(VIEWPORT_FLAG_GRIDLINES); });

    // Window
    RegisterShortcut(ShortcutId::WindowRideConstructionTurnLeft, STR_SHORTCUT_CONSTRUCTION_TURN_LEFT, "NUMPAD 4", []() { ShortcutConstructionTurnLeft(); });
    RegisterShortcut(ShortcutId::WindowRideConstructionTurnRight, STR_SHORTCUT_CONSTRUCTION_TURN_RIGHT, "NUMPAD 6", []() { ShortcutConstructionTurnRight(); });
    RegisterShortcut(ShortcutId::WindowRideConstructionDefault, STR_SHORTCUT_CONSTRUCTION_USE_TRACK_DEFAULT, "NUMPAD 5", []() { window_ride_construction_keyboard_shortcut_use_track_default(); });
    RegisterShortcut(ShortcutId::WindowRideConstructionSlopeDown, STR_SHORTCUT_CONSTRUCTION_SLOPE_DOWN, "NUMPAD 2", []() { ShortcutConstructionSlopeDown(); });
    RegisterShortcut(ShortcutId::WindowRideConstructionSlopeUp, STR_SHORTCUT_CONSTRUCTION_SLOPE_UP, "NUMPAD 8", []() { ShortcutConstructionSlopeUp(); });
    RegisterShortcut(ShortcutId::WindowRideConstructionChainLift, STR_SHORTCUT_CONSTRUCTION_CHAIN_LIFT_TOGGLE, "NUMPAD +", []() { window_ride_construction_keyboard_shortcut_chain_lift_toggle(); });
    RegisterShortcut(ShortcutId::WindowRideConstructionBankLeft, STR_SHORTCUT_CONSTRUCTION_BANK_LEFT, "NUMPAD 1", []() { window_ride_construction_keyboard_shortcut_bank_left(); });
    RegisterShortcut(ShortcutId::WindowRideConstructionBankRight, STR_SHORTCUT_CONSTRUCTION_BANK_RIGHT, "NUMPAD 3", []() { window_ride_construction_keyboard_shortcut_bank_right(); });
    RegisterShortcut(ShortcutId::WindowRideConstructionPrevious, STR_SHORTCUT_CONSTRUCTION_PREVIOUS_TRACK, "NUMPAD 7", []() { window_ride_construction_keyboard_shortcut_previous_track(); });
    RegisterShortcut(ShortcutId::WindowRideConstructionNext, STR_SHORTCUT_CONSTRUCTION_NEXT_TRACK, "NUMPAD 9", []() { window_ride_construction_keyboard_shortcut_next_track(); });
    RegisterShortcut(ShortcutId::WindowRideConstructionBuild, STR_SHORTCUT_CONSTRUCTION_BUILD_CURRENT, "NUMPAD 0", []() { ShortcutConstructionBuildCurrent(); });
    RegisterShortcut(ShortcutId::WindowRideConstructionDemolish, STR_SHORTCUT_CONSTRUCTION_DEMOLISH_CURRENT, "NUMPAD -", []() { ShortcutConstructionDemolishCurrent(); });

    RegisterShortcut(ShortcutId::WindowTileInspectorToggleInvisibility, STR_SHORTCUT_TOGGLE_INVISIBILITY, []() { ShortcutToggleVisibility(); });
    RegisterShortcut(ShortcutId::WindowTileInspectorCopy, STR_SHORTCUT_COPY_ELEMENT, []() { TileInspectorMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_COPY); });
    RegisterShortcut(ShortcutId::WindowTileInspectorPaste, STR_SHORTCUT_PASTE_ELEMENT, []() { TileInspectorMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_PASTE); });
    RegisterShortcut(ShortcutId::WindowTileInspectorRemove, STR_SHORTCUT_REMOVE_ELEMENT, []() { TileInspectorMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_REMOVE); });
    RegisterShortcut(ShortcutId::WindowTileInspectorMoveUp, STR_SHORTCUT_MOVE_ELEMENT_UP, []() { TileInspectorMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_UP); });
    RegisterShortcut(ShortcutId::WindowTileInspectorMoveDown, STR_SHORTCUT_MOVE_ELEMENT_DOWN, []() { TileInspectorMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_DOWN); });
    RegisterShortcut(ShortcutId::WindowTileInspectorIncreaseX, STR_SHORTCUT_INCREASE_X_COORD, []() { TileInspectorMouseDown(WC_TILE_INSPECTOR__WIDX_SPINNER_X_INCREASE); });
    RegisterShortcut(ShortcutId::WindowTileInspectorDecreaseX, STR_SHORTCUT_DECREASE_X_COORD, []() { TileInspectorMouseDown(WC_TILE_INSPECTOR__WIDX_SPINNER_X_DECREASE); });
    RegisterShortcut(ShortcutId::WindowTileInspectorIncreaseY, STR_SHORTCUT_INCREASE_Y_COORD, []() { TileInspectorMouseDown(WC_TILE_INSPECTOR__WIDX_SPINNER_Y_INCREASE); });
    RegisterShortcut(ShortcutId::WindowTileInspectorDecreaseY, STR_SHORTCUT_DECREASE_Y_COORD, []() { TileInspectorMouseDown(WC_TILE_INSPECTOR__WIDX_SPINNER_Y_DECREASE); });
    RegisterShortcut(ShortcutId::WindowTileInspectorIncreaseHeight, STR_SHORTCUT_INCREASE_ELEM_HEIGHT, []() { ShortcutIncreaseElementHeight(); });
    RegisterShortcut(ShortcutId::WindowTileInspectorDecreaseHeight, STR_SHORTCUT_DECREASE_ELEM_HEIGHT, []() { ShortcutDecreaseElementHeight(); });

    // Debug
    RegisterShortcut(ShortcutId::DebugToggleConsole, STR_CONSOLE, "`", []() { ShortcutToggleConsole(); });
    RegisterShortcut(ShortcutId::DebugAdvanceTick, STR_ADVANCE_TO_NEXT_TICK, []() {
        if (!(gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            gDoSingleUpdate = true;
        }
    });
    RegisterShortcut(ShortcutId::DebugTogglePaintDebugWindow, STR_SHORTCUT_DEBUG_PAINT_TOGGLE, []() {
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            auto window = window_find_by_class(WindowClass::DebugPaint);
            if (window != nullptr)
            {
                window_close(*window);
            }
            else
            {
                context_open_window(WindowClass::DebugPaint);
            }
        }
    });
    // clang-format on
}
