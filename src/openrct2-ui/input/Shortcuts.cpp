/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiStringIds.h"
#include "ShortcutIds.h"
#include "ShortcutManager.h"

#include <functional>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/interface/InGameConsole.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/CheatSetAction.h>
#include <openrct2/actions/LoadOrQuitAction.h>
#include <openrct2/actions/PauseToggleAction.h>
#include <openrct2/actions/TileModifyAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/Screenshot.h>
#include <openrct2/network/network.h>
#include <openrct2/object/WallSceneryEntry.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackPaint.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/scenes/title/TitleScene.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/windows/TileInspectorGlobals.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/TileInspector.h>
#include <openrct2/world/tile_element/WallElement.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;
using namespace OpenRCT2::Ui::Windows;

#pragma region Shortcut Commands

static void OpenWindow(WindowClass wc)
{
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        ContextOpenWindow(wc);
    }
}

static void RotateCamera(int32_t direction)
{
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        ViewportRotateAll(direction);
    }
}

static void ToggleViewFlag(int32_t viewportFlag)
{
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        auto window = WindowGetMain();
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

    auto* windowMgr = GetWindowManager();

    // Rotate scenery
    WindowBase* w = windowMgr->FindByClass(WindowClass::Scenery);
    if (w != nullptr && !WidgetIsDisabled(*w, WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON)
        && w->widgets[WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type != WindowWidgetType::Empty)
    {
        w->OnMouseUp(WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON);
        return;
    }

    // Rotate construction track piece
    w = windowMgr->FindByClass(WindowClass::RideConstruction);
    if (w != nullptr && !WidgetIsDisabled(*w, WC_RIDE_CONSTRUCTION__WIDX_ROTATE)
        && w->widgets[WC_RIDE_CONSTRUCTION__WIDX_ROTATE].type != WindowWidgetType::Empty)
    {
        // Check if building a maze...
        if (w->widgets[WC_RIDE_CONSTRUCTION__WIDX_ROTATE].tooltip != STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP)
        {
            w->OnMouseUp(WC_RIDE_CONSTRUCTION__WIDX_ROTATE);
            return;
        }
    }

    // Rotate track design preview
    w = windowMgr->FindByClass(WindowClass::TrackDesignList);
    if (w != nullptr && !WidgetIsDisabled(*w, WC_TRACK_DESIGN_LIST__WIDX_ROTATE)
        && w->widgets[WC_TRACK_DESIGN_LIST__WIDX_ROTATE].type != WindowWidgetType::Empty)
    {
        w->OnMouseUp(WC_TRACK_DESIGN_LIST__WIDX_ROTATE);
        return;
    }

    // Rotate track design placement
    w = windowMgr->FindByClass(WindowClass::TrackDesignPlace);
    if (w != nullptr && !WidgetIsDisabled(*w, WC_TRACK_DESIGN_PLACE__WIDX_ROTATE)
        && w->widgets[WC_TRACK_DESIGN_PLACE__WIDX_ROTATE].type != WindowWidgetType::Empty)
    {
        w->OnMouseUp(WC_TRACK_DESIGN_PLACE__WIDX_ROTATE);
        return;
    }

    // Rotate park entrance
    w = windowMgr->FindByClass(WindowClass::EditorParkEntrance);
    if (w != nullptr && !WidgetIsDisabled(*w, WC_EDITOR_PARK_ENTRANCE__WIDX_ROTATE_ENTRANCE_BUTTON)
        && w->widgets[WC_EDITOR_PARK_ENTRANCE__WIDX_ROTATE_ENTRANCE_BUTTON].type != WindowWidgetType::Empty)
    {
        w->OnMouseUp(WC_EDITOR_PARK_ENTRANCE__WIDX_ROTATE_ENTRANCE_BUTTON);
        return;
    }

    // Rotate selected element in tile inspector
    w = windowMgr->FindByClass(WindowClass::TileInspector);
    if (w != nullptr && !WidgetIsDisabled(*w, WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE].type != WindowWidgetType::Empty)
    {
        w->OnMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE);
        return;
    }
}

static void ShortcutRemoveTopBottomToolbarToggle()
{
    auto* windowMgr = GetWindowManager();

    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
    {
        if (windowMgr->FindByClass(WindowClass::TitleLogo) != nullptr)
        {
            windowMgr->CloseByClass(WindowClass::TitleLogo);
            windowMgr->CloseByClass(WindowClass::TitleOptions);
            windowMgr->CloseByClass(WindowClass::TitleMenu);
            windowMgr->CloseByClass(WindowClass::TitleExit);
            windowMgr->CloseByClass(WindowClass::TitleVersion);
        }
        else
        {
            TitleCreateWindows();
        }
    }
    else
    {
        if (windowMgr->FindByClass(WindowClass::TopToolbar) != nullptr)
        {
            windowMgr->CloseByClass(WindowClass::Dropdown);
            windowMgr->CloseByClass(WindowClass::TopToolbar);
            windowMgr->CloseByClass(WindowClass::BottomToolbar);
        }
        else
        {
            if (gScreenFlags == 0)
            {
                ContextOpenWindow(WindowClass::TopToolbar);
                ContextOpenWindow(WindowClass::BottomToolbar);
            }
            else
            {
                ContextOpenWindow(WindowClass::TopToolbar);
                ContextOpenWindowView(WV_EDITOR_BOTTOM_TOOLBAR);
            }
        }
    }
    GfxInvalidateScreen();
}

static void ShortcutAdjustLand()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR && GetGameState().EditorStep != EditorStep::LandscapeEditor)
        return;

    if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        return;

    ToggleLandWindow();
}

static void ShortcutAdjustWater()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR && GetGameState().EditorStep != EditorStep::LandscapeEditor)
        return;

    if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        return;

    ToggleWaterWindow();
}

static void ShortcutBuildScenery()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR && GetGameState().EditorStep != EditorStep::LandscapeEditor)
        return;

    if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        return;

    ToggleSceneryWindow();
}

static void ShortcutBuildPaths()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR && GetGameState().EditorStep != EditorStep::LandscapeEditor)
        return;

    if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        return;

    ToggleFootpathWindow();
}

static void ShortcutBuildNewRide()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR))
    {
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            ContextOpenWindow(WindowClass::ConstructRide);
        }
    }
}

static void ShortcutShowFinancialInformation()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
        if (!(GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY))
            ContextOpenWindow(WindowClass::Finances);
}

static void ShortcutShowResearchInformation()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        ContextOpenWindowView(WV_RIDE_RESEARCH);
    }
}

static void ShortcutShowRidesList()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        ContextOpenWindow(WindowClass::RideList);
    }
}

static void ShortcutShowParkInformation()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        ContextOpenWindow(WindowClass::ParkInformation);
    }
}

static void ShortcutShowGuestList()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        ContextOpenWindow(WindowClass::GuestList);
    }
}

static void ShortcutShowStaffList()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        ContextOpenWindow(WindowClass::StaffList);
    }
}

static void ShortcutShowRecentMessages()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
        ContextOpenWindow(WindowClass::RecentNews);
}

static void ShortcutShowMap()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || GetGameState().EditorStep == EditorStep::LandscapeEditor)
        if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
            ContextOpenWindow(WindowClass::Map);
}

static void ShortcutReduceGameSpeed()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (NetworkGetMode() == NETWORK_MODE_NONE)
        GameReduceGameSpeed();
}

static void ShortcutIncreaseGameSpeed()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (NetworkGetMode() == NETWORK_MODE_NONE)
        GameIncreaseGameSpeed();
}

static void ShortcutOpenCheatWindow()
{
    if (gScreenFlags != SCREEN_FLAGS_PLAYING)
        return;

    // Check if window is already open
    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::Cheats);
    if (window != nullptr)
    {
        windowMgr->Close(*window);
        return;
    }
    ContextOpenWindow(WindowClass::Cheats);
}

static void ShortcutOpenKeyboardShortcutsWindow()
{
    ShortcutKeysOpen();
}

static void ShortcutOpenTransparencyWindow()
{
    if (gScreenFlags != SCREEN_FLAGS_PLAYING)
        return;

    ContextOpenWindow(WindowClass::Transparency);
}

static void ShortcutClearScenery()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR && GetGameState().EditorStep != EditorStep::LandscapeEditor)
        return;

    if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        return;

    ToggleClearSceneryWindow();
}

static void ShortcutQuickSaveGame()
{
    // Do a quick save in playing mode and a regular save in Scenario Editor mode. In other cases, don't do anything.
    if (gScreenFlags == SCREEN_FLAGS_PLAYING)
    {
        ToolCancel();
        SaveGame();
    }
    else if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
    {
        auto intent = Intent(WindowClass::Loadsave);
        intent.PutExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE);
        intent.PutExtra(INTENT_EXTRA_PATH, GetGameState().ScenarioName);
        ContextOpenIntent(&intent);
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
        || (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR && GetGameState().EditorStep != EditorStep::LandscapeEditor))
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window_scenery = windowMgr->FindByClass(WindowClass::Scenery);
    if (window_scenery == nullptr)
        ToggleSceneryWindow();

    window_scenery = windowMgr->FindByClass(WindowClass::Scenery);
    if (window_scenery != nullptr && !WidgetIsDisabled(*window_scenery, WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON)
        && !gWindowSceneryEyedropperEnabled)
    {
        window_scenery->OnMouseUp(WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON);
        return;
    }
}

static void ShortcutScaleUp()
{
    Config::Get().general.WindowScale += 0.25f;
    Config::Save();
    GfxInvalidateScreen();
    ContextTriggerResize();
    ContextUpdateCursorScale();
}

static void ShortcutScaleDown()
{
    Config::Get().general.WindowScale -= 0.25f;
    Config::Get().general.WindowScale = std::max(0.5f, Config::Get().general.WindowScale);
    Config::Save();
    GfxInvalidateScreen();
    ContextTriggerResize();
    ContextUpdateCursorScale();
}

// Tile inspector shortcuts
static void TileInspectorMouseUp(WidgetIndex widgetIndex)
{
    auto* windowMgr = GetWindowManager();
    auto w = windowMgr->FindByClass(WindowClass::TileInspector);
    if (w != nullptr && !WidgetIsDisabled(*w, widgetIndex) && w->widgets[widgetIndex].type != WindowWidgetType::Empty)
    {
        w->OnMouseUp(widgetIndex);
    }
}

static void TileInspectorMouseDown(WidgetIndex widgetIndex)
{
    auto* windowMgr = GetWindowManager();
    auto w = windowMgr->FindByClass(WindowClass::TileInspector);
    if (w != nullptr && !WidgetIsDisabled(*w, widgetIndex) && w->widgets[widgetIndex].type != WindowWidgetType::Empty)
    {
        w->OnMouseDown(widgetIndex);
    }
}

static void ShortcutToggleWallSlope()
{
    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::TileInspector);
    if (window == nullptr)
    {
        return;
    }

    const TileElement* tileElement = OpenRCT2::TileInspector::GetSelectedElement();

    // Ensure an element is selected and it's a wall
    if (tileElement == nullptr || tileElement->GetType() != TileElementType::Wall)
    {
        return;
    }

    // Ensure a wall can be built on a slope
    if (tileElement->AsWall()->GetEntry()->flags & WALL_SCENERY_CANT_BUILD_ON_SLOPE)
    {
        return;
    }

    int32_t currSlopeValue = tileElement->AsWall()->GetSlope();
    int32_t newSlopeValue = (currSlopeValue + 1) % 3;

    extern TileCoordsXY windowTileInspectorTile;
    auto modifyTile = TileModifyAction(
        windowTileInspectorTile.ToCoordsXY(), TileModifyType::WallSetSlope, windowTileInspectorSelectedIndex, newSlopeValue);
    GameActions::Execute(&modifyTile);
}

static void ShortcutIncreaseElementHeight()
{
    auto* windowMgr = GetWindowManager();
    WindowBase* w = windowMgr->FindByClass(WindowClass::TileInspector);
    if (w != nullptr)
    {
        int action = -1;
        switch (EnumValue(w->tileInspectorPage))
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
        }
        if (action != -1 && !WidgetIsDisabled(*w, action) && w->widgets[action].type != WindowWidgetType::Empty)
            w->OnMouseDown(action);
        return;
    }
}

static void ShortcutDecreaseElementHeight()
{
    auto* windowMgr = GetWindowManager();
    WindowBase* w = windowMgr->FindByClass(WindowClass::TileInspector);
    if (w != nullptr)
    {
        int action = -1;
        switch (EnumValue(w->tileInspectorPage))
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
        }
        if (action != -1 && !WidgetIsDisabled(*w, action) && w->widgets[action].type != WindowWidgetType::Empty)
            w->OnMouseDown(action);
        return;
    }
}

static void ShortcutToggleClearanceChecks()
{
    auto cheatSetAction = CheatSetAction(
        CheatType::DisableClearanceChecks, GetGameState().Cheats.disableClearanceChecks ? 0 : 1);
    GameActions::Execute(&cheatSetAction);
}

static void ShortcutToggleConsole()
{
    auto& console = GetInGameConsole();
    if (console.IsOpen())
    {
        console.Toggle();
    }
    else if (Config::Get().general.DebuggingTools && !ContextIsInputActive())
    {
        WindowCancelTextbox();
        console.Toggle();
    }
}

static void ShortcutConstructionTurnLeft()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::Footpath);
    if (window != nullptr)
    {
        WindowFootpathKeyboardShortcutTurnLeft();
    }
    else
    {
        WindowRideConstructionKeyboardShortcutTurnLeft();
    }
}

static void ShortcutConstructionTurnRight()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::Footpath);
    if (window != nullptr)
    {
        WindowFootpathKeyboardShortcutTurnRight();
    }
    else
    {
        WindowRideConstructionKeyboardShortcutTurnRight();
    }
}

static void ShortcutConstructionSlopeUp()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::Footpath);
    if (window != nullptr)
    {
        WindowFootpathKeyboardShortcutSlopeUp();
    }
    else
    {
        WindowRideConstructionKeyboardShortcutSlopeUp();
    }
}

static void ShortcutConstructionBuildCurrent()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::Footpath);
    if (window != nullptr)
    {
        WindowFootpathKeyboardShortcutBuildCurrent();
    }
    else
    {
        WindowRideConstructionKeyboardShortcutBuildCurrent();
    }
}

static void ShortcutConstructionSlopeDown()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::Footpath);
    if (window != nullptr)
    {
        WindowFootpathKeyboardShortcutSlopeDown();
    }
    else
    {
        WindowRideConstructionKeyboardShortcutSlopeDown();
    }
}

static void ShortcutConstructionDemolishCurrent()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::Footpath);
    if (window != nullptr)
    {
        WindowFootpathKeyboardShortcutDemolishCurrent();
    }
    else
    {
        WindowRideConstructionKeyboardShortcutDemolishCurrent();
    }
}

static void ShortcutToggleTransparentWater()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    Config::Get().general.TransparentWater ^= 1;
    Config::Save();
    GfxInvalidateScreen();
}

#pragma endregion

using namespace OpenRCT2::Ui;

void ShortcutManager::RegisterDefaultShortcuts()
{
    // clang-format off
    // Interface
    RegisterShortcut(ShortcutId::kInterfaceCloseTop, STR_SHORTCUT_CLOSE_TOP_MOST_WINDOW, "BACKSPACE", []() {
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseTop();
    });
    RegisterShortcut(ShortcutId::kInterfaceCloseAll, STR_SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS, "SHIFT+BACKSPACE", []() {
        auto* windowMgr = GetWindowManager();
        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR))
        {
            windowMgr->CloseAll();
        }
        else if (GetGameState().EditorStep == EditorStep::LandscapeEditor)
        {
            windowMgr->CloseTop();
        }
    });
    RegisterShortcut(ShortcutId::kInterfaceRotateConstruction, STR_SHORTCUT_ROTATE_CONSTRUCTION_OBJECT, "Z", ShortcutRotateConstructionObject);
    RegisterShortcut(ShortcutId::kInterfaceCancelConstruction, STR_SHORTCUT_CANCEL_CONSTRUCTION_MODE, "ESCAPE", []() {
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            auto* windowMgr = GetWindowManager();
            auto window = windowMgr->FindByClass(WindowClass::Error);
            if (window != nullptr)
            {
                windowMgr->Close(*window);
            }
            else if (InputTestFlag(INPUT_FLAG_TOOL_ACTIVE))
            {
                ToolCancel();
            }
        }
    });
    RegisterShortcut(ShortcutId::kInterfacePause, STR_SHORTCUT_PAUSE_GAME, "PAUSE", []() {
        if (!(gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            auto pauseToggleAction = PauseToggleAction();
            GameActions::Execute(&pauseToggleAction);
        }
    });
    RegisterShortcut(ShortcutId::kInterfaceDecreaseSpeed, STR_SHORTCUT_REDUCE_GAME_SPEED, "-", ShortcutReduceGameSpeed);
    RegisterShortcut(ShortcutId::kInterfaceIncreaseSpeed, STR_SHORTCUT_INCREASE_GAME_SPEED, "=", ShortcutIncreaseGameSpeed);
    RegisterShortcut(ShortcutId::kInterfaceToggleToolbars, STR_SHORTCUT_TOGGLE_VISIBILITY_OF_TOOLBARS, ShortcutRemoveTopBottomToolbarToggle);
    RegisterShortcut(ShortcutId::kInterfaceScreenshot, STR_SHORTCUT_SCREENSHOT, "CTRL+S", []() { gScreenshotCountdown = 2; });
    RegisterShortcut(ShortcutId::kInterfaceGiantScreenshot, STR_SHORTCUT_GIANT_SCREENSHOT, "CTRL+SHIFT+S", ScreenshotGiant);
    RegisterShortcut(ShortcutId::kInterfaceLoadGame, STR_LOAD_GAME, "CTRL+L", ShortcutLoadGame);
    RegisterShortcut(ShortcutId::kInterfaceSaveGame, STR_SAVE_GAME, "CTRL+F10", ShortcutQuickSaveGame);
    RegisterShortcut(ShortcutId::kInterfaceMute, STR_SHORTCUT_MUTE_SOUND, OpenRCT2::Audio::ToggleAllSounds);
    RegisterShortcut(ShortcutId::kInterfaceSceneryPicker, STR_SHORTCUT_OPEN_SCENERY_PICKER, ShortcutOpenSceneryPicker);
    RegisterShortcut(
        ShortcutId::kInterfaceDisableClearance, STR_SHORTCUT_TOGGLE_CLEARANCE_CHECKS, ShortcutToggleClearanceChecks);
    RegisterShortcut(ShortcutId::kInterfaceMultiplayerChat, STR_SHORTCUT_SEND_MESSAGE, "C", []() {
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && ChatAvailable())
        {
            ChatToggle();
        }
    });
    RegisterShortcut(ShortcutId::kInterfaceScaleToggleWindowMode, STR_SHORTCUT_WINDOWED_MODE_TOGGLE, "ALT+RETURN", ToggleWindowedMode);
    RegisterShortcut(ShortcutId::kInterfaceScaleIncrease, STR_SHORTCUT_SCALE_UP, ShortcutScaleUp);
    RegisterShortcut(ShortcutId::kInterfaceScaleDecrease, STR_SHORTCUT_SCALE_DOWN, ShortcutScaleDown);
    RegisterShortcut(ShortcutId::kInterfaceOpenLand, STR_SHORTCUT_ADJUST_LAND, "F1", ShortcutAdjustLand);
    RegisterShortcut(ShortcutId::kInterfaceOpenWater, STR_SHORTCUT_ADJUST_WATER, "F2", ShortcutAdjustWater);
    RegisterShortcut(ShortcutId::kInterfaceClearScenery, STR_SHORTCUT_CLEAR_SCENERY, "B", ShortcutClearScenery);
    RegisterShortcut(ShortcutId::kInterfaceOpenScenery, STR_SHORTCUT_BUILD_SCENERY, "F3", ShortcutBuildScenery);
    RegisterShortcut(ShortcutId::kInterfaceOpenFootpaths, STR_SHORTCUT_BUILD_PATHS, "F4", ShortcutBuildPaths);
    RegisterShortcut(ShortcutId::kInterfaceOpenNewRide, STR_SHORTCUT_BUILD_NEW_RIDE, "F5", ShortcutBuildNewRide);
    RegisterShortcut(ShortcutId::kInterfaceOpenFinances, STR_SHORTCUT_SHOW_FINANCIAL_INFORMATION, "F", ShortcutShowFinancialInformation);
    RegisterShortcut(ShortcutId::kInterfaceOpenResearch, STR_SHORTCUT_SHOW_RESEARCH_INFORMATION, "D", ShortcutShowResearchInformation);
    RegisterShortcut(ShortcutId::kInterfaceOpenRides, STR_SHORTCUT_SHOW_RIDES_LIST, "R", ShortcutShowRidesList);
    RegisterShortcut(ShortcutId::kInterfaceOpenPark, STR_SHORTCUT_SHOW_PARK_INFORMATION, "P", ShortcutShowParkInformation);
    RegisterShortcut(ShortcutId::kInterfaceOpenGuests, STR_SHORTCUT_SHOW_GUEST_LIST, "G", ShortcutShowGuestList);
    RegisterShortcut(ShortcutId::kInterfaceOpenStaff, STR_SHORTCUT_SHOW_STAFF_LIST, "S", ShortcutShowStaffList);
    RegisterShortcut(ShortcutId::kInterfaceOpenMessages, STR_SHORTCUT_SHOW_RECENT_MESSAGES, "M", ShortcutShowRecentMessages);
    RegisterShortcut(ShortcutId::kInterfaceOpenMap, STR_SHORTCUT_SHOW_MAP, "TAB", ShortcutShowMap);
    RegisterShortcut(ShortcutId::kInterfaceShowOptions, STR_SHORTCUT_SHOW_OPTIONS, std::bind(ContextOpenWindow, WindowClass::Options));
    RegisterShortcut(ShortcutId::kInterfaceOpenKeyboardShortcuts, STR_SHORTCUT_OPEN_KEYBOARD_SHORTCUTS_WINDOW, "SHIFT+/", ShortcutOpenKeyboardShortcutsWindow);
    RegisterShortcut(ShortcutId::kInterfaceOpenTransparencyOptions, STR_SHORTCUT_OPEN_TRANSPARENCY_OPTIONS, "CTRL+T", ShortcutOpenTransparencyWindow);
    RegisterShortcut(ShortcutId::kInterfaceOpenCheats, STR_SHORTCUT_OPEN_CHEATS_WINDOW, "CTRL+ALT+C", ShortcutOpenCheatWindow);
    RegisterShortcut(ShortcutId::kInterfaceOpenTileInspector, STR_SHORTCUT_OPEN_TILE_INSPECTOR, []() {
        if (Config::Get().interface.ToolbarShowCheats)
        {
            OpenWindow(WindowClass::TileInspector);
        }
    });
    RegisterShortcut(ShortcutId::kInterfaceMultiplayerShow, STR_SHORTCUT_SHOW_MULTIPLAYER, []() {
        if (NetworkGetMode() != NETWORK_MODE_NONE)
        {
            OpenWindow(WindowClass::Multiplayer);
        }
    });

    // View
    RegisterShortcut(ShortcutId::kViewGeneralZoomOut, STR_SHORTCUT_ZOOM_VIEW_OUT, "PAGEUP", std::bind(MainWindowZoom, false, false));
    RegisterShortcut(ShortcutId::kViewGeneralZoomIn, STR_SHORTCUT_ZOOM_VIEW_IN, "PAGEDOWN", std::bind(MainWindowZoom, true, false));
    RegisterShortcut(ShortcutId::kViewGeneralRotateClockwise, STR_SHORTCUT_ROTATE_VIEW_CLOCKWISE, "RETURN", "MOUSE 6", std::bind(RotateCamera, 1));
    RegisterShortcut(ShortcutId::kViewGeneralRotateAnticlockwise, STR_SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE, "SHIFT+RETURN", "MOUSE 5", std::bind(RotateCamera, -1));
    RegisterShortcut(ShortcutId::kViewScrollUp, STR_SHORTCUT_SCROLL_MAP_UP, "UP", []() {});
    RegisterShortcut(ShortcutId::kViewScrollLeft, STR_SHORTCUT_SCROLL_MAP_LEFT, "LEFT", []() {});
    RegisterShortcut(ShortcutId::kViewScrollRight, STR_SHORTCUT_SCROLL_MAP_RIGHT, "RIGHT", []() {});
    RegisterShortcut(ShortcutId::kViewScrollDown, STR_SHORTCUT_SCROLL_MAP_DOWN, "DOWN", []() {});
    RegisterShortcut(ShortcutId::kViewToggleUnderground, STR_SHORTCUT_UNDERGROUND_VIEW_TOGGLE, "1", std::bind(ToggleViewFlag, VIEWPORT_FLAG_UNDERGROUND_INSIDE));
    RegisterShortcut(ShortcutId::kViewToggleTransparentWater, STR_VIEWPORT_TRANSPARENT_WATER, "2", ShortcutToggleTransparentWater);
    RegisterShortcut(ShortcutId::kViewToggleBaseLand, STR_SHORTCUT_REMOVE_BASE_LAND_TOGGLE, "H", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_BASE));
    RegisterShortcut(ShortcutId::kViewToggleVerticalLand, STR_SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE, "V", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_VERTICAL));
    RegisterShortcut(ShortcutId::kViewToggleRides, STR_SHORTCUT_SEE_THROUGH_RIDES_TOGGLE, "3", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_RIDES));
    RegisterShortcut(ShortcutId::kViewToggleVehicles, STR_SHORTCUT_SEE_THROUGH_VEHICLES_TOGGLE, std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_VEHICLES));
    RegisterShortcut(ShortcutId::kViewToggleVegetation, STR_SHORTCUT_SEE_THROUGH_VEGETATION_TOGGLE, std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_VEGETATION));
    RegisterShortcut(ShortcutId::kViewToggleScenery, STR_SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE, "4", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_SCENERY));
    RegisterShortcut(ShortcutId::kViewToggleFootpaths, STR_SHORTCUT_SEE_THROUGH_PATHS_TOGGLE, std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_PATHS));
    RegisterShortcut(ShortcutId::kViewToggleSupports, STR_SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE, "5", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_SUPPORTS));
    RegisterShortcut(ShortcutId::kViewToggleGuests, STR_SHORTCUT_SEE_THROUGH_GUESTS_TOGGLE, "6", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_GUESTS));
    RegisterShortcut(ShortcutId::kViewToggleStaff, STR_SHORTCUT_SEE_THROUGH_STAFF_TOGGLE, std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_STAFF));
    RegisterShortcut(ShortcutId::kViewToggleLandHeightMarkers, STR_SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE, "8", std::bind(ToggleViewFlag, VIEWPORT_FLAG_LAND_HEIGHTS));
    RegisterShortcut(ShortcutId::kViewToggleTrackHeightMarkers, STR_SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE, "9", std::bind(ToggleViewFlag, VIEWPORT_FLAG_TRACK_HEIGHTS));
    RegisterShortcut(ShortcutId::kViewToggleFootpathHeightMarkers, STR_SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE, "0", std::bind(ToggleViewFlag, VIEWPORT_FLAG_PATH_HEIGHTS));
    RegisterShortcut(ShortcutId::kViewToggleGridlines, STR_SHORTCUT_GRIDLINES_DISPLAY_TOGGLE, "7", std::bind(ToggleViewFlag, VIEWPORT_FLAG_GRIDLINES));
    RegisterShortcut(ShortcutId::kViewToggleCutAway, STR_SHORTCUT_VIEW_CLIPPING, std::bind(OpenWindow, WindowClass::ViewClipping));
    RegisterShortcut(ShortcutId::kViewToggleFootpathIssues, STR_SHORTCUT_HIGHLIGHT_PATH_ISSUES_TOGGLE, "I", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES));

    // Window
    RegisterShortcut(ShortcutId::kWindowRideConstructionTurnLeft, STR_SHORTCUT_CONSTRUCTION_TURN_LEFT, "NUMPAD 4", ShortcutConstructionTurnLeft);
    RegisterShortcut(ShortcutId::kWindowRideConstructionTurnRight, STR_SHORTCUT_CONSTRUCTION_TURN_RIGHT, "NUMPAD 6", ShortcutConstructionTurnRight);
    RegisterShortcut(ShortcutId::kWindowRideConstructionDefault, STR_SHORTCUT_CONSTRUCTION_USE_TRACK_DEFAULT, "NUMPAD 5", WindowRideConstructionKeyboardShortcutUseTrackDefault);
    RegisterShortcut(ShortcutId::kWindowRideConstructionSlopeDown, STR_SHORTCUT_CONSTRUCTION_SLOPE_DOWN, "NUMPAD 2", ShortcutConstructionSlopeDown);
    RegisterShortcut(ShortcutId::kWindowRideConstructionSlopeUp, STR_SHORTCUT_CONSTRUCTION_SLOPE_UP, "NUMPAD 8", ShortcutConstructionSlopeUp);
    RegisterShortcut(ShortcutId::kWindowRideConstructionChainLift, STR_SHORTCUT_CONSTRUCTION_CHAIN_LIFT_TOGGLE, "NUMPAD +", WindowRideConstructionKeyboardShortcutChainLiftToggle);
    RegisterShortcut(ShortcutId::kWindowRideConstructionBankLeft, STR_SHORTCUT_CONSTRUCTION_BANK_LEFT, "NUMPAD 1", WindowRideConstructionKeyboardShortcutBankLeft);
    RegisterShortcut(ShortcutId::kWindowRideConstructionBankRight, STR_SHORTCUT_CONSTRUCTION_BANK_RIGHT, "NUMPAD 3", WindowRideConstructionKeyboardShortcutBankRight);
    RegisterShortcut(ShortcutId::kWindowRideConstructionPrevious, STR_SHORTCUT_CONSTRUCTION_PREVIOUS_TRACK, "NUMPAD 7", WindowRideConstructionKeyboardShortcutPreviousTrack);
    RegisterShortcut(ShortcutId::kWindowRideConstructionNext, STR_SHORTCUT_CONSTRUCTION_NEXT_TRACK, "NUMPAD 9", WindowRideConstructionKeyboardShortcutNextTrack);
    RegisterShortcut(ShortcutId::kWindowRideConstructionBuild, STR_SHORTCUT_CONSTRUCTION_BUILD_CURRENT, "NUMPAD 0", ShortcutConstructionBuildCurrent);
    RegisterShortcut(ShortcutId::kWindowRideConstructionDemolish, STR_SHORTCUT_CONSTRUCTION_DEMOLISH_CURRENT, "NUMPAD -", ShortcutConstructionDemolishCurrent);
    RegisterShortcut(ShortcutId::kWindowTileInspectorToggleInvisibility, STR_SHORTCUT_TOGGLE_INVISIBILITY, WindowTileInspectorKeyboardShortcutToggleInvisibility);
    RegisterShortcut(ShortcutId::kWindowTileInspectorCopy, STR_SHORTCUT_COPY_ELEMENT, std::bind(TileInspectorMouseUp, WC_TILE_INSPECTOR__WIDX_BUTTON_COPY));
    RegisterShortcut(ShortcutId::kWindowTileInspectorPaste, STR_SHORTCUT_PASTE_ELEMENT, std::bind(TileInspectorMouseUp, WC_TILE_INSPECTOR__WIDX_BUTTON_PASTE));
    RegisterShortcut(ShortcutId::kWindowTileInspectorSort, STR_SHORTCUT_SORT_ELEMENTS, std::bind(TileInspectorMouseUp, WC_TILE_INSPECTOR__WIDX_BUTTON_SORT));
    RegisterShortcut(ShortcutId::kWindowTileInspectorRemove, STR_SHORTCUT_REMOVE_ELEMENT, std::bind(TileInspectorMouseUp, WC_TILE_INSPECTOR__WIDX_BUTTON_REMOVE));
    RegisterShortcut(ShortcutId::kWindowTileInspectorMoveUp, STR_SHORTCUT_MOVE_ELEMENT_UP, std::bind(TileInspectorMouseUp, WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_UP));
    RegisterShortcut(ShortcutId::kWindowTileInspectorMoveDown, STR_SHORTCUT_MOVE_ELEMENT_DOWN, std::bind(TileInspectorMouseUp, WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_DOWN));
    RegisterShortcut(ShortcutId::kWindowTileInspectorIncreaseX, STR_SHORTCUT_INCREASE_X_COORD, std::bind(TileInspectorMouseDown, WC_TILE_INSPECTOR__WIDX_SPINNER_X_INCREASE));
    RegisterShortcut(ShortcutId::kWindowTileInspectorDecreaseX, STR_SHORTCUT_DECREASE_X_COORD, std::bind(TileInspectorMouseDown, WC_TILE_INSPECTOR__WIDX_SPINNER_X_DECREASE));
    RegisterShortcut(ShortcutId::kWindowTileInspectorIncreaseY, STR_SHORTCUT_INCREASE_Y_COORD, std::bind(TileInspectorMouseDown, WC_TILE_INSPECTOR__WIDX_SPINNER_Y_INCREASE));
    RegisterShortcut(ShortcutId::kWindowTileInspectorDecreaseY, STR_SHORTCUT_DECREASE_Y_COORD, std::bind(TileInspectorMouseDown, WC_TILE_INSPECTOR__WIDX_SPINNER_Y_DECREASE));
    RegisterShortcut(ShortcutId::kWindowTileInspectorIncreaseHeight, STR_SHORTCUT_INCREASE_ELEM_HEIGHT, ShortcutIncreaseElementHeight);
    RegisterShortcut(ShortcutId::kWindowTileInspectorDecreaseHeight, STR_SHORTCUT_DECREASE_ELEM_HEIGHT, ShortcutDecreaseElementHeight);
    RegisterShortcut(ShortcutId::kWindowTileInspectorChangeWallSlope, STR_SHORTCUT_TOGGLE_WALL_SLOPE, ShortcutToggleWallSlope);

    // Debug
    RegisterShortcut(ShortcutId::kDebugToggleConsole, STR_CONSOLE, "`", ShortcutToggleConsole);
    RegisterShortcut(ShortcutId::kDebugAdvanceTick, STR_SHORTCUT_ADVANCE_TO_NEXT_TICK, []() {
        if (!(gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER)))
        {
            gDoSingleUpdate = true;
        }
    });
    RegisterShortcut(ShortcutId::kDebugTogglePaintDebugWindow, STR_SHORTCUT_DEBUG_PAINT_TOGGLE, []() {
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            auto* windowMgr = GetWindowManager();
            auto window = windowMgr->FindByClass(WindowClass::DebugPaint);
            if (window != nullptr)
            {
                windowMgr->Close(*window);
            }
            else
            {
                ContextOpenWindow(WindowClass::DebugPaint);
            }
        }
    });
    // clang-format on
}
