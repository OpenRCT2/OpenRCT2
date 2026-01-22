/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/audio/Audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/Screenshot.h>
#include <openrct2/network/Network.h>
#include <openrct2/object/WallSceneryEntry.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackPaint.h>
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
    if (gLegacyScene != LegacyScene::titleSequence)
    {
        ContextOpenWindow(wc);
    }
}

static void RotateCamera(int32_t direction)
{
    if (gLegacyScene != LegacyScene::titleSequence)
    {
        ViewportRotateAll(direction);
    }
}

static void ToggleViewFlag(int32_t viewportFlag)
{
    if (gLegacyScene != LegacyScene::titleSequence)
    {
        auto window = WindowGetMain();
        if (window != nullptr)
        {
            window->viewport->flags ^= viewportFlag;
            window->invalidate();
        }
    }
}

static void ShortcutRotateConstructionObject()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    auto* windowMgr = GetWindowManager();

    // Rotate scenery
    WindowBase* w = windowMgr->FindByClass(WindowClass::scenery);
    if (w != nullptr && !widgetIsDisabled(*w, WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON)
        && w->widgets[WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type != WidgetType::empty)
    {
        w->onMouseUp(WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON);
        return;
    }

    // Rotate construction track piece
    w = windowMgr->FindByClass(WindowClass::rideConstruction);
    if (w != nullptr && !widgetIsDisabled(*w, WC_RIDE_CONSTRUCTION__WIDX_ROTATE)
        && w->widgets[WC_RIDE_CONSTRUCTION__WIDX_ROTATE].type != WidgetType::empty)
    {
        // Check if building a maze...
        if (w->widgets[WC_RIDE_CONSTRUCTION__WIDX_ROTATE].tooltip != STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP)
        {
            w->onMouseUp(WC_RIDE_CONSTRUCTION__WIDX_ROTATE);
            return;
        }
    }

    // Rotate track design preview
    w = windowMgr->FindByClass(WindowClass::trackDesignList);
    if (w != nullptr && !widgetIsDisabled(*w, WC_TRACK_DESIGN_LIST__WIDX_ROTATE)
        && w->widgets[WC_TRACK_DESIGN_LIST__WIDX_ROTATE].type != WidgetType::empty)
    {
        w->onMouseUp(WC_TRACK_DESIGN_LIST__WIDX_ROTATE);
        return;
    }

    // Rotate track design placement
    w = windowMgr->FindByClass(WindowClass::trackDesignPlace);
    if (w != nullptr && !widgetIsDisabled(*w, WC_TRACK_DESIGN_PLACE__WIDX_ROTATE)
        && w->widgets[WC_TRACK_DESIGN_PLACE__WIDX_ROTATE].type != WidgetType::empty)
    {
        w->onMouseUp(WC_TRACK_DESIGN_PLACE__WIDX_ROTATE);
        return;
    }

    // Rotate park entrance
    w = windowMgr->FindByClass(WindowClass::editorParkEntrance);
    if (w != nullptr && !widgetIsDisabled(*w, WC_EDITOR_PARK_ENTRANCE__WIDX_ROTATE_ENTRANCE_BUTTON)
        && w->widgets[WC_EDITOR_PARK_ENTRANCE__WIDX_ROTATE_ENTRANCE_BUTTON].type != WidgetType::empty)
    {
        w->onMouseUp(WC_EDITOR_PARK_ENTRANCE__WIDX_ROTATE_ENTRANCE_BUTTON);
        return;
    }

    // Rotate selected element in tile inspector
    w = windowMgr->FindByClass(WindowClass::tileInspector);
    if (w != nullptr && !widgetIsDisabled(*w, WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE)
        && w->widgets[WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE].type != WidgetType::empty)
    {
        w->onMouseUp(WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE);
        return;
    }
}

static void ShortcutRemoveTopBottomToolbarToggle()
{
    auto* windowMgr = GetWindowManager();

    if (gLegacyScene == LegacyScene::titleSequence)
    {
        if (windowMgr->FindByClass(WindowClass::titleLogo) != nullptr)
        {
            windowMgr->CloseByClass(WindowClass::titleLogo);
            windowMgr->CloseByClass(WindowClass::titleOptions);
            windowMgr->CloseByClass(WindowClass::titleMenu);
            windowMgr->CloseByClass(WindowClass::titleExit);
            windowMgr->CloseByClass(WindowClass::titleVersion);
        }
        else
        {
            TitleCreateWindows();
        }
    }
    else
    {
        if (windowMgr->FindByClass(WindowClass::topToolbar) != nullptr)
        {
            windowMgr->CloseByClass(WindowClass::dropdown);
            windowMgr->CloseByClass(WindowClass::topToolbar);
            windowMgr->CloseByClass(WindowClass::bottomToolbar);
        }
        else
        {
            if (gLegacyScene == LegacyScene::playing)
            {
                ContextOpenWindow(WindowClass::topToolbar);
                ContextOpenWindow(WindowClass::bottomToolbar);
            }
            else
            {
                ContextOpenWindow(WindowClass::topToolbar);
                ContextOpenWindowView(WindowView::editorBottomToolbar);
            }
        }
    }
    GfxInvalidateScreen();
}

static void ShortcutAdjustLand()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (gLegacyScene == LegacyScene::scenarioEditor && getGameState().editorStep != EditorStep::LandscapeEditor)
        return;

    if (isInTrackDesignerOrManager())
        return;

    ToggleLandWindow();
}

static void ShortcutAdjustWater()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (gLegacyScene == LegacyScene::scenarioEditor && getGameState().editorStep != EditorStep::LandscapeEditor)
        return;

    if (isInTrackDesignerOrManager())
        return;

    ToggleWaterWindow();
}

static void ShortcutBuildScenery()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (gLegacyScene == LegacyScene::scenarioEditor && getGameState().editorStep != EditorStep::LandscapeEditor)
        return;

    if (isInTrackDesignerOrManager())
        return;

    ToggleSceneryWindow();
}

static void ShortcutBuildPaths()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (gLegacyScene == LegacyScene::scenarioEditor && getGameState().editorStep != EditorStep::LandscapeEditor)
        return;

    if (isInTrackDesignerOrManager())
        return;

    ToggleFootpathWindow();
}

static void ShortcutBuildNewRide()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (gLegacyScene != LegacyScene::scenarioEditor)
    {
        if (!(isInTrackDesignerOrManager()))
        {
            ContextOpenWindow(WindowClass::constructRide);
        }
    }
}

static void ShortcutShowFinancialInformation()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (!(isInTrackDesignerOrManager()))
        if (!(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
            ContextOpenWindow(WindowClass::finances);
}

static void ShortcutShowResearchInformation()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (!isInEditorMode())
    {
        ContextOpenWindowView(WindowView::rideResearch);
    }
}

static void ShortcutShowRidesList()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (!isInEditorMode())
    {
        ContextOpenWindow(WindowClass::rideList);
    }
}

static void ShortcutShowParkInformation()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (!isInEditorMode())
    {
        ContextOpenWindow(WindowClass::parkInformation);
    }
}

static void ShortcutShowGuestList()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (!isInEditorMode())
    {
        ContextOpenWindow(WindowClass::guestList);
    }
}

static void ShortcutShowStaffList()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (!isInEditorMode())
    {
        ContextOpenWindow(WindowClass::staffList);
    }
}

static void ShortcutShowRecentMessages()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (!isInEditorMode())
        ContextOpenWindow(WindowClass::recentNews);
}

static void ShortcutShowMap()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (gLegacyScene != LegacyScene::scenarioEditor || getGameState().editorStep == EditorStep::LandscapeEditor)
        if (!(isInTrackDesignerOrManager()))
            ContextOpenWindow(WindowClass::map);
}

static void ShortcutReduceGameSpeed()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (Network::GetMode() == Network::Mode::none)
        GameReduceGameSpeed();
}

static void ShortcutIncreaseGameSpeed()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (Network::GetMode() == Network::Mode::none)
        GameIncreaseGameSpeed();
}

static void ShortcutOpenCheatWindow()
{
    if (gLegacyScene != LegacyScene::playing)
        return;

    // Check if window is already open
    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::cheats);
    if (window != nullptr)
    {
        windowMgr->Close(*window);
        return;
    }
    ContextOpenWindow(WindowClass::cheats);
}

static void ShortcutOpenKeyboardShortcutsWindow()
{
    ShortcutKeysOpen();
}

static void ShortcutOpenTransparencyWindow()
{
    if (gLegacyScene != LegacyScene::playing)
        return;

    ContextOpenWindow(WindowClass::transparency);
}

static void ShortcutClearScenery()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    if (gLegacyScene == LegacyScene::scenarioEditor && getGameState().editorStep != EditorStep::LandscapeEditor)
        return;

    if (isInTrackDesignerOrManager())
        return;

    ToggleClearSceneryWindow();
}

static void ShortcutQuickSaveGame()
{
    // Do a quick save in playing mode and a regular save in Scenario Editor mode. In other cases, don't do anything.
    if (gLegacyScene == LegacyScene::playing)
    {
        ToolCancel();
        SaveGame();
    }
    else if (gLegacyScene == LegacyScene::scenarioEditor)
    {
        auto intent = Intent(WindowClass::loadsave);
        intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::save);
        intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::landscape);
        intent.PutExtra(INTENT_EXTRA_PATH, getGameState().scenarioOptions.name);
        ContextOpenIntent(&intent);
    }
}

static void ShortcutLoadGame()
{
    if (!(isInTrackDesignerOrManager()))
    {
        auto loadOrQuitAction = GameActions::LoadOrQuitAction(GameActions::LoadOrQuitModes::OpenSavePrompt);
        GameActions::Execute(&loadOrQuitAction, getGameState());
    }
}

static void ShortcutOpenSceneryPicker()
{
    if ((gLegacyScene == LegacyScene::titleSequence || gLegacyScene == LegacyScene::trackDesigner
         || gLegacyScene == LegacyScene::trackDesignsManager)
        || (gLegacyScene == LegacyScene::scenarioEditor && getGameState().editorStep != EditorStep::LandscapeEditor))
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* sceneryWindow = windowMgr->FindByClass(WindowClass::scenery);
    if (sceneryWindow == nullptr)
        ToggleSceneryWindow();

    sceneryWindow = windowMgr->FindByClass(WindowClass::scenery);
    if (sceneryWindow != nullptr && !widgetIsDisabled(*sceneryWindow, WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON)
        && !gWindowSceneryEyedropperEnabled)
    {
        sceneryWindow->onMouseUp(WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON);
        return;
    }
}

static void ShortcutScaleUp()
{
    Config::Get().general.windowScale += 0.25f;
    Config::Save();
    GfxInvalidateScreen();
    ContextTriggerResize();
    ContextUpdateCursorScale();
}

static void ShortcutScaleDown()
{
    Config::Get().general.windowScale -= 0.25f;
    Config::Get().general.windowScale = std::max(0.5f, Config::Get().general.windowScale);
    Config::Save();
    GfxInvalidateScreen();
    ContextTriggerResize();
    ContextUpdateCursorScale();
}

// Tile inspector shortcuts
static void TileInspectorMouseUp(WidgetIndex widgetIndex)
{
    auto* windowMgr = GetWindowManager();
    auto w = windowMgr->FindByClass(WindowClass::tileInspector);
    if (w != nullptr && !widgetIsDisabled(*w, widgetIndex) && w->widgets[widgetIndex].type != WidgetType::empty)
    {
        w->onMouseUp(widgetIndex);
    }
}

static void TileInspectorMouseDown(WidgetIndex widgetIndex)
{
    auto* windowMgr = GetWindowManager();
    auto w = windowMgr->FindByClass(WindowClass::tileInspector);
    if (w != nullptr && !widgetIsDisabled(*w, widgetIndex) && w->widgets[widgetIndex].type != WidgetType::empty)
    {
        w->onMouseDown(widgetIndex);
    }
}

static void ShortcutToggleWallSlope()
{
    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::tileInspector);
    if (window == nullptr)
    {
        return;
    }

    const TileElement* tileElement = TileInspector::GetSelectedElement();

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
    auto modifyTile = GameActions::TileModifyAction(
        windowTileInspectorTile.ToCoordsXY(), GameActions::TileModifyType::WallSetSlope, windowTileInspectorSelectedIndex,
        newSlopeValue);
    GameActions::Execute(&modifyTile, getGameState());
}

static void ShortcutIncreaseElementHeight()
{
    auto* windowMgr = GetWindowManager();
    WindowBase* w = windowMgr->FindByClass(WindowClass::tileInspector);
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
        if (action != -1 && !widgetIsDisabled(*w, action) && w->widgets[action].type != WidgetType::empty)
            w->onMouseDown(action);
        return;
    }
}

static void ShortcutDecreaseElementHeight()
{
    auto* windowMgr = GetWindowManager();
    WindowBase* w = windowMgr->FindByClass(WindowClass::tileInspector);
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
        if (action != -1 && !widgetIsDisabled(*w, action) && w->widgets[action].type != WidgetType::empty)
            w->onMouseDown(action);
        return;
    }
}

static void ShortcutToggleClearanceChecks()
{
    auto& gameState = getGameState();
    auto cheatSetAction = GameActions::CheatSetAction(
        CheatType::disableClearanceChecks, gameState.cheats.disableClearanceChecks ? 0 : 1);
    GameActions::Execute(&cheatSetAction, gameState);
}

static void ShortcutToggleConsole()
{
    auto& console = GetInGameConsole();
    if (console.IsOpen())
    {
        console.Toggle();
    }
    else if (Config::Get().general.debuggingTools && !ContextIsInputActive())
    {
        WindowCancelTextbox();
        console.Toggle();
    }
}

static void ShortcutConstructionTurnLeft()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::footpath);
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
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::footpath);
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
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::footpath);
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
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::footpath);
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
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::footpath);
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
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    auto* windowMgr = GetWindowManager();
    WindowBase* window = windowMgr->FindByClass(WindowClass::footpath);
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
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    Config::Get().general.transparentWater ^= 1;
    Config::Save();
    GfxInvalidateScreen();
}

#pragma endregion

using namespace OpenRCT2::Ui;

void ShortcutManager::registerDefaultShortcuts()
{
    // clang-format off
    // Interface
    registerShortcut(ShortcutId::kInterfaceCloseTop, STR_SHORTCUT_CLOSE_TOP_MOST_WINDOW, "BACKSPACE", []() {
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseTop();
    });
    registerShortcut(ShortcutId::kInterfaceCloseAll, STR_SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS, "SHIFT+BACKSPACE", []() {
        auto* windowMgr = GetWindowManager();
        if (gLegacyScene != LegacyScene::scenarioEditor)
        {
            windowMgr->CloseAll();
        }
        else if (getGameState().editorStep == EditorStep::LandscapeEditor)
        {
            windowMgr->CloseTop();
        }
    });
    registerShortcut(ShortcutId::kInterfaceRotateConstruction, STR_SHORTCUT_ROTATE_CONSTRUCTION_OBJECT, "Z", ShortcutRotateConstructionObject);
    registerShortcut(ShortcutId::kInterfaceCancelConstruction, STR_SHORTCUT_CANCEL_CONSTRUCTION_MODE, "ESCAPE", []() {
        if (gLegacyScene != LegacyScene::titleSequence)
        {
            auto* windowMgr = GetWindowManager();
            auto window = windowMgr->FindByClass(WindowClass::error);
            if (window != nullptr)
            {
                windowMgr->Close(*window);
            }
            else if (gInputFlags.has(InputFlag::toolActive))
            {
                ToolCancel();
            }
        }
    });
    registerShortcut(ShortcutId::kInterfacePause, STR_SHORTCUT_PAUSE_GAME, "PAUSE", []() {
        if (gLegacyScene != LegacyScene::titleSequence && gLegacyScene != LegacyScene::scenarioEditor && gLegacyScene != LegacyScene::trackDesignsManager)
        {
            auto pauseToggleAction = GameActions::PauseToggleAction();
            GameActions::Execute(&pauseToggleAction, getGameState());
        }
    });
    registerShortcut(ShortcutId::kInterfaceDecreaseSpeed, STR_SHORTCUT_REDUCE_GAME_SPEED, "-", ShortcutReduceGameSpeed);
    registerShortcut(ShortcutId::kInterfaceIncreaseSpeed, STR_SHORTCUT_INCREASE_GAME_SPEED, "=", ShortcutIncreaseGameSpeed);
    registerShortcut(ShortcutId::kInterfaceToggleToolbars, STR_SHORTCUT_TOGGLE_VISIBILITY_OF_TOOLBARS, ShortcutRemoveTopBottomToolbarToggle);
    registerShortcut(ShortcutId::kInterfaceScreenshot, STR_SHORTCUT_SCREENSHOT, "CTRL+S", []() { gScreenshotCountdown = 2; });
    registerShortcut(ShortcutId::kInterfaceGiantScreenshot, STR_SHORTCUT_GIANT_SCREENSHOT, "CTRL+SHIFT+S", ScreenshotGiant);
    registerShortcut(ShortcutId::kInterfaceLoadGame, STR_LOAD_GAME, "CTRL+L", ShortcutLoadGame);
    registerShortcut(ShortcutId::kInterfaceSaveGame, STR_SAVE_GAME, "CTRL+F10", ShortcutQuickSaveGame);
    registerShortcut(ShortcutId::kInterfaceMute, STR_SHORTCUT_MUTE_SOUND, Audio::ToggleAllSounds);
    registerShortcut(ShortcutId::kInterfaceSceneryPicker, STR_SHORTCUT_OPEN_SCENERY_PICKER, ShortcutOpenSceneryPicker);
    registerShortcut(
        ShortcutId::kInterfaceDisableClearance, STR_SHORTCUT_TOGGLE_CLEARANCE_CHECKS, ShortcutToggleClearanceChecks);
    registerShortcut(ShortcutId::kInterfaceMultiplayerChat, STR_SHORTCUT_SEND_MESSAGE, "C", []() {
        if (gLegacyScene != LegacyScene::titleSequence && ChatAvailable())
        {
            ChatToggle();
        }
    });
    registerShortcut(ShortcutId::kInterfaceScaleToggleWindowMode, STR_SHORTCUT_WINDOWED_MODE_TOGGLE, "ALT+RETURN", ToggleWindowedMode);
    registerShortcut(ShortcutId::kInterfaceScaleIncrease, STR_SHORTCUT_SCALE_UP, ShortcutScaleUp);
    registerShortcut(ShortcutId::kInterfaceScaleDecrease, STR_SHORTCUT_SCALE_DOWN, ShortcutScaleDown);
    registerShortcut(ShortcutId::kInterfaceOpenLand, STR_SHORTCUT_ADJUST_LAND, "F1", ShortcutAdjustLand);
    registerShortcut(ShortcutId::kInterfaceOpenWater, STR_SHORTCUT_ADJUST_WATER, "F2", ShortcutAdjustWater);
    registerShortcut(ShortcutId::kInterfaceClearScenery, STR_SHORTCUT_CLEAR_SCENERY, "B", ShortcutClearScenery);
    registerShortcut(ShortcutId::kInterfaceOpenScenery, STR_SHORTCUT_BUILD_SCENERY, "F3", ShortcutBuildScenery);
    registerShortcut(ShortcutId::kInterfaceOpenFootpaths, STR_SHORTCUT_BUILD_PATHS, "F4", ShortcutBuildPaths);
    registerShortcut(ShortcutId::kInterfaceOpenNewRide, STR_SHORTCUT_BUILD_NEW_RIDE, "F5", ShortcutBuildNewRide);
    registerShortcut(ShortcutId::kInterfaceOpenFinances, STR_SHORTCUT_SHOW_FINANCIAL_INFORMATION, "F", ShortcutShowFinancialInformation);
    registerShortcut(ShortcutId::kInterfaceOpenResearch, STR_SHORTCUT_SHOW_RESEARCH_INFORMATION, "D", ShortcutShowResearchInformation);
    registerShortcut(ShortcutId::kInterfaceOpenRides, STR_SHORTCUT_SHOW_RIDES_LIST, "R", ShortcutShowRidesList);
    registerShortcut(ShortcutId::kInterfaceOpenPark, STR_SHORTCUT_SHOW_PARK_INFORMATION, "P", ShortcutShowParkInformation);
    registerShortcut(ShortcutId::kInterfaceOpenGuests, STR_SHORTCUT_SHOW_GUEST_LIST, "G", ShortcutShowGuestList);
    registerShortcut(ShortcutId::kInterfaceOpenStaff, STR_SHORTCUT_SHOW_STAFF_LIST, "S", ShortcutShowStaffList);
    registerShortcut(ShortcutId::kInterfaceOpenMessages, STR_SHORTCUT_SHOW_RECENT_MESSAGES, "M", ShortcutShowRecentMessages);
    registerShortcut(ShortcutId::kInterfaceOpenMap, STR_SHORTCUT_SHOW_MAP, "TAB", ShortcutShowMap);
    registerShortcut(ShortcutId::kInterfaceShowOptions, STR_SHORTCUT_SHOW_OPTIONS, std::bind(ContextOpenWindow, WindowClass::options));
    registerShortcut(ShortcutId::kInterfaceOpenKeyboardShortcuts, STR_SHORTCUT_OPEN_KEYBOARD_SHORTCUTS_WINDOW, "SHIFT+/", ShortcutOpenKeyboardShortcutsWindow);
    registerShortcut(ShortcutId::kInterfaceOpenTransparencyOptions, STR_SHORTCUT_OPEN_TRANSPARENCY_OPTIONS, "CTRL+T", ShortcutOpenTransparencyWindow);
    registerShortcut(ShortcutId::kInterfaceOpenCheats, STR_SHORTCUT_OPEN_CHEATS_WINDOW, "CTRL+ALT+C", ShortcutOpenCheatWindow);
    registerShortcut(ShortcutId::kInterfaceOpenTileInspector, STR_SHORTCUT_OPEN_TILE_INSPECTOR, []() {
        if (Config::Get().interface.toolbarShowCheats)
        {
            OpenWindow(WindowClass::tileInspector);
        }
    });
    registerShortcut(ShortcutId::kInterfaceMultiplayerShow, STR_SHORTCUT_SHOW_MULTIPLAYER, []() {
        if (Network::GetMode() != Network::Mode::none)
        {
            OpenWindow(WindowClass::multiplayer);
        }
    });

    // View
    registerShortcut(ShortcutId::kViewGeneralZoomOut, STR_SHORTCUT_ZOOM_VIEW_OUT, "PAGEUP", std::bind(MainWindowZoom, false, false));
    registerShortcut(ShortcutId::kViewGeneralZoomIn, STR_SHORTCUT_ZOOM_VIEW_IN, "PAGEDOWN", std::bind(MainWindowZoom, true, false));
    registerShortcut(ShortcutId::kViewGeneralRotateClockwise, STR_SHORTCUT_ROTATE_VIEW_CLOCKWISE, "RETURN", "MOUSE 6", std::bind(RotateCamera, 1));
    registerShortcut(ShortcutId::kViewGeneralRotateAnticlockwise, STR_SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE, "SHIFT+RETURN", "MOUSE 5", std::bind(RotateCamera, -1));
    registerShortcut(ShortcutId::kViewScrollUp, STR_SHORTCUT_SCROLL_MAP_UP, "UP", []() {});
    registerShortcut(ShortcutId::kViewScrollLeft, STR_SHORTCUT_SCROLL_MAP_LEFT, "LEFT", []() {});
    registerShortcut(ShortcutId::kViewScrollRight, STR_SHORTCUT_SCROLL_MAP_RIGHT, "RIGHT", []() {});
    registerShortcut(ShortcutId::kViewScrollDown, STR_SHORTCUT_SCROLL_MAP_DOWN, "DOWN", []() {});
    registerShortcut(ShortcutId::kViewToggleUnderground, STR_SHORTCUT_UNDERGROUND_VIEW_TOGGLE, "1", std::bind(ToggleViewFlag, VIEWPORT_FLAG_UNDERGROUND_INSIDE));
    registerShortcut(ShortcutId::kViewToggleTransparentWater, STR_VIEWPORT_TRANSPARENT_WATER, "2", ShortcutToggleTransparentWater);
    registerShortcut(ShortcutId::kViewToggleBaseLand, STR_SHORTCUT_REMOVE_BASE_LAND_TOGGLE, "H", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_BASE));
    registerShortcut(ShortcutId::kViewToggleVerticalLand, STR_SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE, "V", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_VERTICAL));
    registerShortcut(ShortcutId::kViewToggleRides, STR_SHORTCUT_SEE_THROUGH_RIDES_TOGGLE, "3", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_RIDES));
    registerShortcut(ShortcutId::kViewToggleVehicles, STR_SHORTCUT_SEE_THROUGH_VEHICLES_TOGGLE, std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_VEHICLES));
    registerShortcut(ShortcutId::kViewToggleVegetation, STR_SHORTCUT_SEE_THROUGH_VEGETATION_TOGGLE, std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_VEGETATION));
    registerShortcut(ShortcutId::kViewToggleScenery, STR_SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE, "4", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_SCENERY));
    registerShortcut(ShortcutId::kViewToggleFootpaths, STR_SHORTCUT_SEE_THROUGH_PATHS_TOGGLE, std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_PATHS));
    registerShortcut(ShortcutId::kViewToggleSupports, STR_SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE, "5", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_SUPPORTS));
    registerShortcut(ShortcutId::kViewToggleGuests, STR_SHORTCUT_SEE_THROUGH_GUESTS_TOGGLE, "6", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_GUESTS));
    registerShortcut(ShortcutId::kViewToggleStaff, STR_SHORTCUT_SEE_THROUGH_STAFF_TOGGLE, std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIDE_STAFF));
    registerShortcut(ShortcutId::kViewToggleLandHeightMarkers, STR_SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE, "8", std::bind(ToggleViewFlag, VIEWPORT_FLAG_LAND_HEIGHTS));
    registerShortcut(ShortcutId::kViewToggleTrackHeightMarkers, STR_SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE, "9", std::bind(ToggleViewFlag, VIEWPORT_FLAG_TRACK_HEIGHTS));
    registerShortcut(ShortcutId::kViewToggleFootpathHeightMarkers, STR_SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE, "0", std::bind(ToggleViewFlag, VIEWPORT_FLAG_PATH_HEIGHTS));
    registerShortcut(ShortcutId::kViewToggleGridlines, STR_SHORTCUT_GRIDLINES_DISPLAY_TOGGLE, "7", std::bind(ToggleViewFlag, VIEWPORT_FLAG_GRIDLINES));
    registerShortcut(ShortcutId::kViewToggleCutAway, STR_SHORTCUT_VIEW_CLIPPING, std::bind(OpenWindow, WindowClass::viewClipping));
    registerShortcut(ShortcutId::kViewToggleFootpathIssues, STR_SHORTCUT_HIGHLIGHT_PATH_ISSUES_TOGGLE, "I", std::bind(ToggleViewFlag, VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES));

    // Window
    registerShortcut(ShortcutId::kWindowRideConstructionTurnLeft, STR_SHORTCUT_CONSTRUCTION_TURN_LEFT, "NUMPAD 4", ShortcutConstructionTurnLeft);
    registerShortcut(ShortcutId::kWindowRideConstructionTurnRight, STR_SHORTCUT_CONSTRUCTION_TURN_RIGHT, "NUMPAD 6", ShortcutConstructionTurnRight);
    registerShortcut(ShortcutId::kWindowRideConstructionDefault, STR_SHORTCUT_CONSTRUCTION_USE_TRACK_DEFAULT, "NUMPAD 5", WindowRideConstructionKeyboardShortcutUseTrackDefault);
    registerShortcut(ShortcutId::kWindowRideConstructionSlopeDown, STR_SHORTCUT_CONSTRUCTION_SLOPE_DOWN, "NUMPAD 2", ShortcutConstructionSlopeDown);
    registerShortcut(ShortcutId::kWindowRideConstructionSlopeUp, STR_SHORTCUT_CONSTRUCTION_SLOPE_UP, "NUMPAD 8", ShortcutConstructionSlopeUp);
    registerShortcut(ShortcutId::kWindowRideConstructionChainLift, STR_SHORTCUT_CONSTRUCTION_CHAIN_LIFT_TOGGLE, "NUMPAD +", WindowRideConstructionKeyboardShortcutChainLiftToggle);
    registerShortcut(ShortcutId::kWindowRideConstructionBankLeft, STR_SHORTCUT_CONSTRUCTION_BANK_LEFT, "NUMPAD 1", WindowRideConstructionKeyboardShortcutBankLeft);
    registerShortcut(ShortcutId::kWindowRideConstructionBankRight, STR_SHORTCUT_CONSTRUCTION_BANK_RIGHT, "NUMPAD 3", WindowRideConstructionKeyboardShortcutBankRight);
    registerShortcut(ShortcutId::kWindowRideConstructionPrevious, STR_SHORTCUT_CONSTRUCTION_PREVIOUS_TRACK, "NUMPAD 7", WindowRideConstructionKeyboardShortcutPreviousTrack);
    registerShortcut(ShortcutId::kWindowRideConstructionNext, STR_SHORTCUT_CONSTRUCTION_NEXT_TRACK, "NUMPAD 9", WindowRideConstructionKeyboardShortcutNextTrack);
    registerShortcut(ShortcutId::kWindowRideConstructionBuild, STR_SHORTCUT_CONSTRUCTION_BUILD_CURRENT, "NUMPAD 0", ShortcutConstructionBuildCurrent);
    registerShortcut(ShortcutId::kWindowRideConstructionDemolish, STR_SHORTCUT_CONSTRUCTION_DEMOLISH_CURRENT, "NUMPAD -", ShortcutConstructionDemolishCurrent);
    registerShortcut(ShortcutId::kWindowTileInspectorToggleInvisibility, STR_SHORTCUT_TOGGLE_INVISIBILITY, WindowTileInspectorKeyboardShortcutToggleInvisibility);
    registerShortcut(ShortcutId::kWindowTileInspectorCopy, STR_SHORTCUT_COPY_ELEMENT, std::bind(TileInspectorMouseUp, WC_TILE_INSPECTOR__WIDX_BUTTON_COPY));
    registerShortcut(ShortcutId::kWindowTileInspectorPaste, STR_SHORTCUT_PASTE_ELEMENT, std::bind(TileInspectorMouseUp, WC_TILE_INSPECTOR__WIDX_BUTTON_PASTE));
    registerShortcut(ShortcutId::kWindowTileInspectorSort, STR_SHORTCUT_SORT_ELEMENTS, std::bind(TileInspectorMouseUp, WC_TILE_INSPECTOR__WIDX_BUTTON_SORT));
    registerShortcut(ShortcutId::kWindowTileInspectorRemove, STR_SHORTCUT_REMOVE_ELEMENT, std::bind(TileInspectorMouseUp, WC_TILE_INSPECTOR__WIDX_BUTTON_REMOVE));
    registerShortcut(ShortcutId::kWindowTileInspectorMoveUp, STR_SHORTCUT_MOVE_ELEMENT_UP, std::bind(TileInspectorMouseUp, WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_UP));
    registerShortcut(ShortcutId::kWindowTileInspectorMoveDown, STR_SHORTCUT_MOVE_ELEMENT_DOWN, std::bind(TileInspectorMouseUp, WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_DOWN));
    registerShortcut(ShortcutId::kWindowTileInspectorIncreaseX, STR_SHORTCUT_INCREASE_X_COORD, std::bind(TileInspectorMouseDown, WC_TILE_INSPECTOR__WIDX_SPINNER_X_INCREASE));
    registerShortcut(ShortcutId::kWindowTileInspectorDecreaseX, STR_SHORTCUT_DECREASE_X_COORD, std::bind(TileInspectorMouseDown, WC_TILE_INSPECTOR__WIDX_SPINNER_X_DECREASE));
    registerShortcut(ShortcutId::kWindowTileInspectorIncreaseY, STR_SHORTCUT_INCREASE_Y_COORD, std::bind(TileInspectorMouseDown, WC_TILE_INSPECTOR__WIDX_SPINNER_Y_INCREASE));
    registerShortcut(ShortcutId::kWindowTileInspectorDecreaseY, STR_SHORTCUT_DECREASE_Y_COORD, std::bind(TileInspectorMouseDown, WC_TILE_INSPECTOR__WIDX_SPINNER_Y_DECREASE));
    registerShortcut(ShortcutId::kWindowTileInspectorIncreaseHeight, STR_SHORTCUT_INCREASE_ELEM_HEIGHT, ShortcutIncreaseElementHeight);
    registerShortcut(ShortcutId::kWindowTileInspectorDecreaseHeight, STR_SHORTCUT_DECREASE_ELEM_HEIGHT, ShortcutDecreaseElementHeight);
    registerShortcut(ShortcutId::kWindowTileInspectorChangeWallSlope, STR_SHORTCUT_TOGGLE_WALL_SLOPE, ShortcutToggleWallSlope);

    // Debug
    registerShortcut(ShortcutId::kDebugToggleConsole, STR_CONSOLE, "`", ShortcutToggleConsole);
    registerShortcut(ShortcutId::kDebugAdvanceTick, STR_SHORTCUT_ADVANCE_TO_NEXT_TICK, []() {
        if (gLegacyScene != LegacyScene::titleSequence && gLegacyScene != LegacyScene::scenarioEditor && gLegacyScene != LegacyScene::trackDesignsManager)
        {
            gDoSingleUpdate = true;
        }
    });
    registerShortcut(ShortcutId::kDebugTogglePaintDebugWindow, STR_SHORTCUT_DEBUG_PAINT_TOGGLE, []() {
        if (gLegacyScene != LegacyScene::titleSequence)
        {
            auto* windowMgr = GetWindowManager();
            auto window = windowMgr->FindByClass(WindowClass::debugPaint);
            if (window != nullptr)
            {
                windowMgr->Close(*window);
            }
            else
            {
                ContextOpenWindow(WindowClass::debugPaint);
            }
        }
    });
    // clang-format on
}
