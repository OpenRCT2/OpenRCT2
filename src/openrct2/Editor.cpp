/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Editor.h"

#include "Context.h"
#include "EditorObjectSelectionSession.h"
#include "FileClassifier.h"
#include "Game.h"
#include "GameState.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "actions/LandBuyRightsAction.h"
#include "actions/LandSetRightsAction.h"
#include "actions/ResultWithMessage.h"
#include "audio/Audio.h"
#include "core/EnumUtils.hpp"
#include "core/Path.hpp"
#include "core/String.hpp"
#include "drawing/Drawing.h"
#include "entity/EntityList.h"
#include "entity/EntityRegistry.h"
#include "entity/Guest.h"
#include "entity/PatrolArea.h"
#include "entity/Staff.h"
#include "interface/WindowBase.h"
#include "localisation/LocalisationService.h"
#include "management/Finance.h"
#include "management/NewsItem.h"
#include "object/DefaultObjects.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "peep/PeepAnimations.h"
#include "rct1/RCT1.h"
#include "scenario/Scenario.h"
#include "scripting/ScriptEngine.h"
#include "ui/WindowManager.h"
#include "windows/Intent.h"
#include "world/Climate.h"
#include "world/Entrance.h"
#include "world/Footpath.h"
#include "world/Map.h"
#include "world/MapLimits.h"
#include "world/Park.h"
#include "world/Scenery.h"

#include <array>
#include <cassert>
#include <vector>

using namespace OpenRCT2;
using OpenRCT2::GameActions::CommandFlag;

namespace OpenRCT2::Editor
{
    static std::array<std::vector<uint8_t>, EnumValue(ObjectType::count)> _editorSelectedObjectFlags;

    static void ConvertSaveToScenarioCallback(ModalResult result, const utf8* path);
    static void SetAllLandOwned();
    static void FinaliseMainView();
    static void ClearMapForEditing(bool fromSave);

    static void ObjectListLoad()
    {
        auto* context = GetContext();
        context->OpenProgress(STR_LOADING_GENERIC);

        // Unload objects first, the repository is re-populated which owns the objects.
        auto& objectManager = context->GetObjectManager();
        objectManager.UnloadAll();

        // Scan objects if necessary
        const auto& localisationService = context->GetLocalisationService();
        auto& objectRepository = context->GetObjectRepository();
        objectRepository.LoadOrConstruct(localisationService.GetCurrentLanguage());

        Audio::LoadAudioObjects();

        // Reset loaded objects to just defaults
        // Load minimum required objects (like surface and edge)
        for (const auto& entry : kMinimumRequiredObjects)
        {
            objectManager.LoadObject(entry);
        }

        context->CloseProgress();
    }

    static WindowBase* OpenEditorWindows()
    {
        auto* main = ContextOpenWindow(WindowClass::mainWindow);
        ContextOpenWindow(WindowClass::topToolbar);
        ContextOpenWindowView(WindowView::editorBottomToolbar);
        return main;
    }

    /**
     *
     *  rct2: 0x0066FFE1
     */
    void Load()
    {
        // TODO: replace with dedicated scene
        auto* context = GetContext();
        context->SetActiveScene(context->GetGameScene());

        auto& gameState = getGameState();
        Audio::StopAll();
        gameStateInitAll(gameState, kDefaultMapSize);
        gLegacyScene = LegacyScene::scenarioEditor;
        gameState.editorStep = EditorStep::ObjectSelection;
        gameState.park.flags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        gameState.scenarioOptions.category = Scenario::Category::other;
        ObjectListLoad();
        ContextResetSubsystems();
        WindowBase* mainWindow = OpenEditorWindows();
        mainWindow->setViewportLocation(TileCoordsXYZ{ 75, 75, 14 }.ToCoordsXYZ());
        LoadPalette();
        gScreenAge = 0;
        gameState.scenarioOptions.name = LanguageGetString(STR_MY_NEW_SCENARIO);

        GameLoadScripts();
        GameNotifyMapChanged();
    }

    /**
     *
     *  rct2: 0x00672781
     */
    void ConvertSaveToScenario()
    {
        ToolCancel();
        auto intent = Intent(WindowClass::loadsave);
        intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::load);
        intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::park);
        intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(ConvertSaveToScenarioCallback));
        ContextOpenIntent(&intent);
    }

    static void ConvertSaveToScenarioCallback(ModalResult result, const utf8* path)
    {
        if (result != ModalResult::ok)
        {
            return;
        }

        if (!GetContext()->LoadParkFromFile(path))
        {
            return;
        }

        auto& gameState = getGameState();
        ScenarioReset(gameState);

        gLegacyScene = LegacyScene::scenarioEditor;
        gameState.editorStep = EditorStep::OptionsSelection;
        gameState.scenarioOptions.category = Scenario::Category::other;
        ContextResetSubsystems();
        OpenEditorWindows();
        FinaliseMainView();
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
     *  rct2: 0x00672957
     */
    void LoadTrackDesigner()
    {
        // TODO: replace with dedicated scene
        auto* context = GetContext();
        context->SetActiveScene(context->GetGameScene());

        Audio::StopAll();
        gLegacyScene = LegacyScene::trackDesigner;
        gScreenAge = 0;

        auto& gameState = getGameState();
        gameStateInitAll(gameState, kDefaultMapSize);
        gameState.editorStep = EditorStep::ObjectSelection;
        SetAllLandOwned();
        ObjectListLoad();
        ContextResetSubsystems();
        WindowBase* mainWindow = OpenEditorWindows();
        mainWindow->setViewportLocation(TileCoordsXYZ{ 75, 75, 14 }.ToCoordsXYZ());
        LoadPalette();

        GameLoadScripts();
        GameNotifyMapChanged();
    }

    /**
     *
     *  rct2: 0x006729FD
     */
    void LoadTrackManager()
    {
        // TODO: replace with dedicated scene
        auto* context = GetContext();
        context->SetActiveScene(context->GetGameScene());

        Audio::StopAll();
        gLegacyScene = LegacyScene::trackDesignsManager;
        gScreenAge = 0;

        auto& gameState = getGameState();
        gameStateInitAll(gameState, kDefaultMapSize);
        SetAllLandOwned();
        gameState.editorStep = EditorStep::ObjectSelection;
        ObjectListLoad();
        ContextResetSubsystems();
        WindowBase* mainWindow = OpenEditorWindows();
        mainWindow->setViewportLocation(TileCoordsXYZ{ 75, 75, 14 }.ToCoordsXYZ());
        LoadPalette();

        GameLoadScripts();
        GameNotifyMapChanged();
    }

    /**
     *
     *  rct2: 0x0068ABEC
     */
    static void SetAllLandOwned()
    {
        auto& gameState = getGameState();

        MapRange range = { 2 * kCoordsXYStep, 2 * kCoordsXYStep, (gameState.mapSize.x - 3) * kCoordsXYStep,
                           (gameState.mapSize.y - 3) * kCoordsXYStep };

        auto landSetRightsAction = GameActions::LandSetRightsAction(range, GameActions::LandSetRightSetting::SetForSale);
        landSetRightsAction.SetFlags({ CommandFlag::noSpend });
        GameActions::Execute(&landSetRightsAction, gameState);

        auto landBuyRightsAction = GameActions::LandBuyRightsAction(range, GameActions::LandBuyRightSetting::BuyLand);
        landBuyRightsAction.SetFlags({ CommandFlag::noSpend });
        GameActions::Execute(&landBuyRightsAction, gameState);
    }

    static void AfterLoadCleanup(bool loadedFromSave)
    {
        ClearMapForEditing(loadedFromSave);

        // TODO: replace with dedicated scene
        auto* context = GetContext();
        context->SetActiveScene(context->GetGameScene());

        getGameState().editorStep = EditorStep::LandscapeEditor;
        gScreenAge = 0;
        gLegacyScene = LegacyScene::scenarioEditor;
        ContextResetSubsystems();
        OpenEditorWindows();
        FinaliseMainView();

        GameLoadScripts();
        GameNotifyMapChanged();
    }

    bool LoadLandscape(const utf8* path)
    {
        // #4996: Make sure the object selection window closes here to prevent unload objects
        //        after we have loaded a new park.
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseAll();

        if (!GetContext()->LoadParkFromFile(path))
            return false;

        auto extension = Path::GetExtension(path);
        bool loadedFromSave = !ParkImporter::ExtensionIsScenario(extension);

        AfterLoadCleanup(loadedFromSave);
        return true;
    }

    static void ClearMapForEditing(bool fromSave)
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

        getGameState().entities.ResetAllEntities();
        UpdateConsolidatedPatrolAreas();

        auto& gameState = getGameState();
        auto& park = gameState.park;
        auto& scenarioOptions = gameState.scenarioOptions;

        park.numGuestsInPark = 0;
        park.numGuestsHeadingForPark = 0;
        park.numGuestsInParkLastWeek = 0;
        park.guestChangeModifier = 0;

        if (fromSave)
        {
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

            scenarioOptions.guestInitialCash = std::clamp(scenarioOptions.guestInitialCash, 10.00_GBP, kMaxEntranceFee);
            scenarioOptions.initialCash = std::min<money64>(scenarioOptions.initialCash, 100000);
            FinanceResetCashToInitial();

            park.bankLoan = std::clamp<money64>(park.bankLoan, 0.00_GBP, 5000000.00_GBP);
            park.maxBankLoan = std::clamp<money64>(park.maxBankLoan, 0.00_GBP, 5000000.00_GBP);
            park.bankLoanInterestRate = std::clamp<uint8_t>(park.bankLoanInterestRate, 5, kMaxBankLoanInterestRate);
        }

        ClimateReset();

        News::InitQueue(gameState);
    }

    /**
     *
     *  rct2: 0x0067009A
     */
    void OpenWindowsForCurrentStep()
    {
        if (!isInEditorMode())
        {
            return;
        }

        auto* windowMgr = Ui::GetWindowManager();

        switch (getGameState().editorStep)
        {
            case EditorStep::ObjectSelection:
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
            case EditorStep::InventionsListSetUp:
                if (windowMgr->FindByClass(WindowClass::editorInventionList) != nullptr)
                {
                    return;
                }

                ContextOpenWindow(WindowClass::editorInventionList);
                break;
            case EditorStep::OptionsSelection:
            case EditorStep::ObjectiveSelection:
            case EditorStep::ScenarioDetails:
                if (windowMgr->FindByClass(WindowClass::editorScenarioOptions) != nullptr)
                {
                    return;
                }

                ContextOpenWindow(WindowClass::editorScenarioOptions);
                break;
            case EditorStep::LandscapeEditor:
            case EditorStep::SaveScenario:
            case EditorStep::RollercoasterDesigner:
            case EditorStep::DesignsManager:
            case EditorStep::Invalid:
                break;
        }
    }

    static void FinaliseMainView()
    {
        auto windowManager = Ui::GetWindowManager();
        auto& gameState = getGameState();
        windowManager->SetMainView(gameState.savedView, gameState.savedViewZoom, gameState.savedViewRotation);

        ResetAllSpriteQuadrantPlacements();

        windowManager->BroadcastIntent(Intent(INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG));
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_NEW_RIDES));
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD));

        gWindowUpdateTicks = 0;
        LoadPalette();
    }

    /**
     *
     *  rct2: 0x006AB9B8
     */
    std::pair<ObjectType, StringId> CheckObjectSelection()
    {
        constexpr std::pair<ObjectType, StringId> kBasicCheckPairs[] = {
            { ObjectType::ride, STR_AT_LEAST_ONE_RIDE_OBJECT_MUST_BE_SELECTED },
            { ObjectType::station, STR_AT_LEAST_ONE_STATION_OBJECT_MUST_BE_SELECTED },
            { ObjectType::terrainSurface, STR_AT_LEAST_ONE_TERRAIN_SURFACE_OBJECT_MUST_BE_SELECTED },
            { ObjectType::terrainEdge, STR_AT_LEAST_ONE_TERRAIN_EDGE_OBJECT_MUST_BE_SELECTED },
        };

        for (auto& pair : kBasicCheckPairs)
        {
            if (!EditorCheckObjectGroupAtLeastOneSelected(pair.first))
            {
                return { pair.first, pair.second };
            }
        }

        // No checks beyond this point apply to the track designer or track designs manager.
        const bool isTrackDesignerManager = isInTrackDesignerOrManager();
        if (isTrackDesignerManager)
        {
            return { ObjectType::none, kStringIdNone };
        }

        if (!EditorCheckObjectGroupAtLeastOneSurfaceSelected(false))
        {
            return { ObjectType::footpathSurface, STR_AT_LEAST_ONE_FOOTPATH_NON_QUEUE_SURFACE_OBJECT_MUST_BE_SELECTED };
        }
        if (!EditorCheckObjectGroupAtLeastOneSurfaceSelected(true))
        {
            return { ObjectType::footpathSurface, STR_AT_LEAST_ONE_FOOTPATH_QUEUE_SURFACE_OBJECT_MUST_BE_SELECTED };
        }

        constexpr std::pair<ObjectType, StringId> kParkCheckPairs[] = {
            { ObjectType::footpathRailings, STR_AT_LEAST_ONE_FOOTPATH_RAILING_OBJECT_MUST_BE_SELECTED },
            { ObjectType::parkEntrance, STR_PARK_ENTRANCE_TYPE_MUST_BE_SELECTED },
            { ObjectType::water, STR_WATER_TYPE_MUST_BE_SELECTED },
            { ObjectType::peepNames, STR_AT_LEAST_ONE_PEEP_NAMES_OBJECT_MUST_BE_SELECTED },
        };

        for (auto& pair : kParkCheckPairs)
        {
            if (!EditorCheckObjectGroupAtLeastOneSelected(pair.first))
            {
                return { pair.first, pair.second };
            }
        }

        using OpenRCT2::AnimationPeepType;
        constexpr std::pair<AnimationPeepType, StringId> kPeepCheckPairs[] = {
            { AnimationPeepType::guest, STR_AT_LEAST_ONE_GUEST_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::handyman, STR_AT_LEAST_ONE_HANDYMAN_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::mechanic, STR_AT_LEAST_ONE_MECHANIC_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::security, STR_AT_LEAST_ONE_SECURITY_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::entertainer, STR_AT_LEAST_ONE_ENTERTAINER_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
        };

        for (auto& pair : kPeepCheckPairs)
        {
            if (!EditorCheckObjectGroupAtLeastOneOfPeepTypeSelected(EnumValue(pair.first)))
            {
                return { ObjectType::peepAnimations, pair.second };
            }
        }

        return { ObjectType::none, kStringIdNone };
    }

    /**
     *
     *  rct2: 0x0066FEAC
     */
    ResultWithMessage CheckPark()
    {
        auto& gameState = getGameState();
        auto& park = gameState.park;
        int32_t parkSize = Park::UpdateSize(park);
        if (parkSize == 0)
        {
            return { false, STR_PARK_MUST_OWN_SOME_LAND };
        }

        if (gameState.park.entrances.empty())
        {
            return { false, STR_NO_PARK_ENTRANCES };
        }

        for (const auto& parkEntrance : gameState.park.entrances)
        {
            int32_t direction = DirectionReverse(parkEntrance.direction);

            switch (FootpathIsConnectedToMapEdge(parkEntrance, direction, 0))
            {
                case FOOTPATH_SEARCH_NOT_FOUND:
                    return { false, STR_PARK_ENTRANCE_WRONG_DIRECTION_OR_NO_PATH };
                case FOOTPATH_SEARCH_INCOMPLETE:
                case FOOTPATH_SEARCH_TOO_COMPLEX:
                    return { false, STR_PARK_ENTRANCE_PATH_INCOMPLETE_OR_COMPLEX };
                case FOOTPATH_SEARCH_SUCCESS:
                    // Run the search again and unown the path
                    FootpathIsConnectedToMapEdge(parkEntrance, direction, (1 << 5));
                    break;
            }
        }

        if (gameState.peepSpawns.empty())
        {
            return { false, STR_PEEP_SPAWNS_NOT_SET };
        }

        return { true, kStringIdNone };
    }

    uint8_t GetSelectedObjectFlags(ObjectType objectType, size_t index)
    {
        uint8_t result = 0;
        auto& list = _editorSelectedObjectFlags[EnumValue(objectType)];
        if (list.size() > index)
        {
            result = list[index];
        }
        return result;
    }

    void ClearSelectedObject(ObjectType objectType, size_t index, uint32_t flags)
    {
        auto& list = _editorSelectedObjectFlags[EnumValue(objectType)];
        if (list.size() <= index)
        {
            list.resize(index + 1);
        }
        list[index] &= ~flags;
    }

    void SetSelectedObject(ObjectType objectType, size_t index, uint32_t flags)
    {
        if (index != kObjectEntryIndexNull)
        {
            assert(static_cast<size_t>(objectType) < getObjectEntryGroupCount(ObjectType::paths));
            auto& list = _editorSelectedObjectFlags[EnumValue(objectType)];
            if (list.size() <= index)
            {
                list.resize(index + 1);
            }
            list[index] |= flags;
        }
    }
} // namespace OpenRCT2::Editor

void EditorOpenWindowsForCurrentStep()
{
    Editor::OpenWindowsForCurrentStep();
}
