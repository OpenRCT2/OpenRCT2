/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EditorScene.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../Game.h"
#include "../../GameState.h"
#include "../../OpenRCT2.h"
#include "../../ParkImporter.h"
#include "../../actions/GameActionRunner.h"
#include "../../actions/ResultWithMessage.h"
#include "../../actions/park/LandBuyRightsAction.h"
#include "../../actions/park/LandSetRightsAction.h"
#include "../../audio/Audio.h"
#include "../../core/Path.hpp"
#include "../../drawing/Drawing.h"
#include "../../entity/EntityList.h"
#include "../../entity/Guest.h"
#include "../../entity/PatrolArea.h"
#include "../../entity/Staff.h"
#include "../../interface/WindowBase.h"
#include "../../localisation/Language.h"
#include "../../object/ObjectManager.h"
#include "../../scenario/Scenario.h"
#include "../../scenes/editor/EditorController.h"
#include "../../scripting/ScriptEngine.h"
#include "../../ui/WindowManager.h"
#include "../../windows/Intent.h"
#include "../../world/Map.h"
#include "../SceneManager.h"

using namespace OpenRCT2;
using OpenRCT2::GameActions::CommandFlag;

void EditorScene::Load()
{
    LOG_VERBOSE("EditorScene::Load()");

    auto& gameState = getGameState();
    Audio::StopAll();
    gameStateInitAll(gameState, kDefaultMapSize);

    gLegacyScene = LegacyScene::scenarioEditor;
    gameState.editorStep = Editor::Step::objectSelection;
    gameState.park.flags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
    gameState.scenarioOptions.category = Scenario::Category::other;

    Editor::ObjectListLoad();
    ContextResetSubsystems();

    OpenEditorWindows();
    resetMainViewport();

    LoadPalette();
    gScreenAge = 0;
    gameState.scenarioOptions.name = LanguageGetString(STR_MY_NEW_SCENARIO);

    GameLoadScripts();
    GameNotifyMapChanged();

    LOG_VERBOSE("EditorScene::Load() finished");
}

void EditorScene::Tick()
{
    gameStateTick();
}

void EditorScene::Stop()
{
    Audio::StopAll();

    // Force closure of any object selection windows, regardless of valid state.
    // NB: this is relevant for both in-game scenes and editors, as the window
    // may be opened in-game using cheats.
    ContextForceCloseWindowByClass(WindowClass::editorObjectSelection);
}

static void ConvertSaveToScenarioCallback(ModalResult result, const utf8* path)
{
    if (result != ModalResult::ok)
    {
        return;
    }

    auto& gameState = getGameState();
    ScenarioReset(gameState);

    gLegacyScene = LegacyScene::scenarioEditor;
    gameState.editorStep = Editor::Step::optionsSelection;
    gameState.scenarioOptions.category = Scenario::Category::other;
    ContextResetSubsystems();

    auto* sceneMgr = GetContext()->GetSceneManager();
    auto* scene = static_cast<EditorScene*>(sceneMgr->getEditorScene());

    scene->OpenEditorWindows();
    scene->FinaliseMainView();
    gScreenAge = 0;

    GameLoadScripts();
    GameNotifyMapChanged();

#ifdef ENABLE_SCRIPTING
    // Clear the plugin storage before saving
    auto& scriptEngine = GetContext()->GetScriptEngine();
    scriptEngine.ClearParkStorage();
#endif
}

/**
 *
 *  rct2: 0x00672781
 */
void EditorScene::ConvertSaveToScenario()
{
    ToolCancel();
    auto intent = Intent(WindowClass::loadsave);
    intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::load);
    intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::park);
    intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(ConvertSaveToScenarioCallback));
    ContextOpenIntent(&intent);
}

bool EditorScene::LoadLandscape(const utf8* path)
{
    // #4996: Make sure the object selection window closes here to prevent unload objects
    //        after we have loaded a new park.
    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->CloseAll();

    if (!_context.LoadParkFromFile(path))
        return false;

    clearMapForEditing();

    auto extension = Path::GetExtension(path);
    bool wasScenario = ParkImporter::ExtensionIsScenario(extension);
    if (!wasScenario)
        clearFinances();

    AfterLoadCleanup();
    return true;
}

// NB: only called by LoadLandscape
void EditorScene::AfterLoadCleanup()
{
    // TODO: replace with dedicated scene
    auto* sceneMgr = _context.GetSceneManager();
    sceneMgr->setActiveScene(sceneMgr->getEditorScene());

    getGameState().editorStep = Editor::Step::landscapeEditor;
    gScreenAge = 0;
    gLegacyScene = LegacyScene::scenarioEditor;
    ContextResetSubsystems();
    OpenEditorWindows();
    FinaliseMainView();

    GameLoadScripts();
    GameNotifyMapChanged();
}

// NB: only called by LoadLandscape
void EditorScene::clearMapForEditing()
{
    MapRemoveAllRides();
    UnlinkAllRideBanners();

    RideInitAll();

    for (auto* guest : EntityList<Guest>())
    {
        guest->SetName({});
    }
    for (auto* staff : EntityList<Staff>())
    {
        staff->SetName({});
    }

    auto& gameState = getGameState();
    gameState.entities.ResetAllEntities();

    UpdateConsolidatedPatrolAreas();

    auto& park = gameState.park;
    park.numGuestsInPark = 0;
    park.numGuestsHeadingForPark = 0;
    park.numGuestsInParkLastWeek = 0;
    park.guestChangeModifier = 0;

    Weather::reset();

    News::InitQueue(gameState);
}

// NB: only called by LoadLandscape
void EditorScene::clearFinances()
{
    auto& gameState = getGameState();
    auto& park = gameState.park;

    park.flags |= PARK_FLAGS_NO_MONEY;

    if (park.entranceFee == 0)
    {
        park.flags |= PARK_FLAGS_PARK_FREE_ENTRY;
    }
    else
    {
        park.flags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
    }

    park.flags &= ~PARK_FLAGS_SPRITES_INITIALISED;

    auto& scenarioOptions = gameState.scenarioOptions;
    scenarioOptions.guestInitialCash = std::clamp(scenarioOptions.guestInitialCash, 10.00_GBP, kMaxEntranceFee);
    scenarioOptions.initialCash = std::min<money64>(scenarioOptions.initialCash, 100000);
    FinanceResetCashToInitial();

    park.bankLoan = std::clamp<money64>(park.bankLoan, 0.00_GBP, 5000000.00_GBP);
    park.maxBankLoan = std::clamp<money64>(park.maxBankLoan, 0.00_GBP, 5000000.00_GBP);
    park.bankLoanInterestRate = std::clamp<uint8_t>(park.bankLoanInterestRate, 5, kMaxBankLoanInterestRate);
}

void EditorScene::OpenEditorWindows()
{
    ContextOpenWindow(WindowClass::mainWindow);
    ContextOpenWindow(WindowClass::topToolbar);
    ContextOpenWindowView(WindowView::editorBottomToolbar);
}

/**
 *
 *  rct2: 0x0067009A
 */
void EditorScene::OpenWindowsForCurrentStep()
{
    if (!isInEditorMode())
    {
        return;
    }

    auto* windowMgr = Ui::GetWindowManager();

    switch (::getGameState().editorStep)
    {
        case Editor::Step::objectSelection:
            if (windowMgr->FindByClass(WindowClass::editorObjectSelection) != nullptr)
            {
                return;
            }

            if (windowMgr->FindByClass(WindowClass::installTrack) != nullptr)
            {
                return;
            }

            if (gLegacyScene == LegacyScene::trackDesignsManager)
            {
                ObjectManagerUnloadAllObjects();
            }

            ContextOpenWindow(WindowClass::editorObjectSelection);
            break;
        case Editor::Step::inventionsListSetUp:
            if (windowMgr->FindByClass(WindowClass::editorInventionList) != nullptr)
            {
                return;
            }

            ContextOpenWindow(WindowClass::editorInventionList);
            break;
        case Editor::Step::optionsSelection:
        case Editor::Step::objectiveSelection:
        case Editor::Step::scenarioDetails:
            if (windowMgr->FindByClass(WindowClass::editorScenarioOptions) != nullptr)
            {
                return;
            }

            ContextOpenWindow(WindowClass::editorScenarioOptions);
            break;
        case Editor::Step::landscapeEditor:
        case Editor::Step::saveScenario:
        case Editor::Step::rollerCoasterDesigner:
        case Editor::Step::designsManager:
        case Editor::Step::invalid:
            break;
    }
}

void EditorScene::FinaliseMainView()
{
    auto windowManager = Ui::GetWindowManager();
    auto& gameState = ::getGameState();
    windowManager->SetMainView(gameState.savedView, gameState.savedViewZoom, gameState.savedViewRotation);

    ResetAllSpriteQuadrantPlacements();

    windowManager->BroadcastIntent(Intent(INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG));
    windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_NEW_RIDES));
    windowManager->BroadcastIntent(Intent(INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD));

    gWindowUpdateTicks = 0;
    LoadPalette();
}

void EditorScene::resetMainViewport()
{
    auto* mainWindow = WindowGetMain();
    mainWindow->setViewportLocation(TileCoordsXYZ{ 75, 75, 14 }.ToCoordsXYZ());
}

/**
 *
 *  rct2: 0x0068ABEC
 */
void EditorScene::SetAllLandOwned()
{
    auto& gameState = getGameState();

    MapRange range = { 2 * kCoordsXYStep, 2 * kCoordsXYStep, (gameState.mapSize.x - 3) * kCoordsXYStep,
                       (gameState.mapSize.y - 3) * kCoordsXYStep };

    auto landSetRightsAction = GameActions::LandSetRightsAction(range, GameActions::LandSetRightSetting::setForSale);
    landSetRightsAction.SetFlags({ CommandFlag::noSpend });
    GameActions::Execute(&landSetRightsAction, gameState);

    auto landBuyRightsAction = GameActions::LandBuyRightsAction(range, GameActions::LandBuyRightSetting::buyLand);
    landBuyRightsAction.SetFlags({ CommandFlag::noSpend });
    GameActions::Execute(&landBuyRightsAction, gameState);
}
