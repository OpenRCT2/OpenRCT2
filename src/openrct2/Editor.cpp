/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "audio/audio.h"
#include "core/EnumUtils.hpp"
#include "core/Path.hpp"
#include "core/String.hpp"
#include "entity/EntityList.h"
#include "entity/EntityRegistry.h"
#include "entity/Guest.h"
#include "entity/PatrolArea.h"
#include "entity/Staff.h"
#include "interface/Viewport.h"
#include "interface/Window_internal.h"
#include "localisation/LocalisationService.h"
#include "management/Finance.h"
#include "management/NewsItem.h"
#include "object/DefaultObjects.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "peep/PeepAnimations.h"
#include "rct1/RCT1.h"
#include "scenario/Scenario.h"
#include "ui/WindowManager.h"
#include "windows/Intent.h"
#include "world/Climate.h"
#include "world/Entrance.h"
#include "world/Footpath.h"
#include "world/Park.h"
#include "world/Scenery.h"

#include <array>
#include <cassert>
#include <vector>

using namespace OpenRCT2;

namespace OpenRCT2::Editor
{
    static std::array<std::vector<uint8_t>, EnumValue(ObjectType::Count)> _editorSelectedObjectFlags;

    static void ConvertSaveToScenarioCallback(int32_t result, const utf8* path);
    static void SetAllLandOwned();
    static void FinaliseMainView();
    static void ClearMapForEditing(bool fromSave);

    static void ObjectListLoad()
    {
        auto* context = GetContext();

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
        for (const auto& entry : MinimumRequiredObjects)
        {
            objectManager.LoadObject(entry);
        }
    }

    static WindowBase* OpenEditorWindows()
    {
        auto* main = ContextOpenWindow(WindowClass::MainWindow);
        ContextOpenWindow(WindowClass::TopToolbar);
        ContextOpenWindowView(WV_EDITOR_BOTTOM_TOOLBAR);
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

        auto& gameState = GetGameState();
        Audio::StopAll();
        ObjectListLoad();
        gameStateInitAll(gameState, kDefaultMapSize);
        gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
        gameState.EditorStep = EditorStep::ObjectSelection;
        gameState.Park.Flags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        gameState.ScenarioCategory = SCENARIO_CATEGORY_OTHER;
        ViewportInitAll();
        WindowBase* mainWindow = OpenEditorWindows();
        mainWindow->SetLocation(TileCoordsXYZ{ 75, 75, 14 }.ToCoordsXYZ());
        LoadPalette();
        gScreenAge = 0;
        gameState.ScenarioName = LanguageGetString(STR_MY_NEW_SCENARIO);
    }

    /**
     *
     *  rct2: 0x00672781
     */
    void ConvertSaveToScenario()
    {
        ToolCancel();
        auto intent = Intent(WindowClass::Loadsave);
        intent.PutExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME);
        intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(ConvertSaveToScenarioCallback));
        ContextOpenIntent(&intent);
    }

    static void ConvertSaveToScenarioCallback(int32_t result, const utf8* path)
    {
        if (result != MODAL_RESULT_OK)
        {
            return;
        }

        if (!GetContext()->LoadParkFromFile(path))
        {
            return;
        }

        auto& gameState = GetGameState();
        ScenarioReset(gameState);

        gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
        gameState.EditorStep = EditorStep::ObjectiveSelection;
        gameState.ScenarioCategory = SCENARIO_CATEGORY_OTHER;
        ViewportInitAll();
        OpenEditorWindows();
        FinaliseMainView();
        gScreenAge = 0;
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
        gScreenFlags = SCREEN_FLAGS_TRACK_DESIGNER;
        gScreenAge = 0;

        ObjectManagerUnloadAllObjects();
        ObjectListLoad();
        gameStateInitAll(GetGameState(), kDefaultMapSize);
        SetAllLandOwned();
        GetGameState().EditorStep = EditorStep::ObjectSelection;
        ViewportInitAll();
        WindowBase* mainWindow = OpenEditorWindows();
        mainWindow->SetLocation(TileCoordsXYZ{ 75, 75, 14 }.ToCoordsXYZ());
        LoadPalette();
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
        gScreenFlags = SCREEN_FLAGS_TRACK_MANAGER;
        gScreenAge = 0;

        ObjectManagerUnloadAllObjects();
        ObjectListLoad();
        gameStateInitAll(GetGameState(), kDefaultMapSize);
        SetAllLandOwned();
        GetGameState().EditorStep = EditorStep::ObjectSelection;
        ViewportInitAll();
        WindowBase* mainWindow = OpenEditorWindows();
        mainWindow->SetLocation(TileCoordsXYZ{ 75, 75, 14 }.ToCoordsXYZ());
        LoadPalette();
    }

    /**
     *
     *  rct2: 0x0068ABEC
     */
    static void SetAllLandOwned()
    {
        auto& gameState = GetGameState();
        MapRange range = { 2 * kCoordsXYStep, 2 * kCoordsXYStep, (gameState.MapSize.x - 3) * kCoordsXYStep,
                           (gameState.MapSize.y - 3) * kCoordsXYStep };
        auto landSetRightsAction = LandSetRightsAction(range, LandSetRightSetting::SetForSale);
        landSetRightsAction.SetFlags(GAME_COMMAND_FLAG_NO_SPEND);
        GameActions::Execute(&landSetRightsAction);

        auto landBuyRightsAction = LandBuyRightsAction(range, LandBuyRightSetting::BuyLand);
        landBuyRightsAction.SetFlags(GAME_COMMAND_FLAG_NO_SPEND);
        GameActions::Execute(&landBuyRightsAction);
    }

    static void AfterLoadCleanup(bool loadedFromSave)
    {
        ClearMapForEditing(loadedFromSave);

        // TODO: replace with dedicated scene
        auto* context = GetContext();
        context->SetActiveScene(context->GetGameScene());

        GetGameState().EditorStep = EditorStep::LandscapeEditor;
        gScreenAge = 0;
        gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
        ViewportInitAll();
        OpenEditorWindows();
        FinaliseMainView();
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

        //
        for (auto* guest : EntityList<Guest>())
        {
            guest->SetName({});
        }
        for (auto* staff : EntityList<Staff>())
        {
            staff->SetName({});
        }

        auto& gameState = GetGameState();

        ResetAllEntities();
        UpdateConsolidatedPatrolAreas();
        gameState.NumGuestsInPark = 0;
        gameState.NumGuestsHeadingForPark = 0;
        gameState.NumGuestsInParkLastWeek = 0;
        gameState.GuestChangeModifier = 0;
        if (fromSave)
        {
            gameState.Park.Flags |= PARK_FLAGS_NO_MONEY;

            if (gameState.Park.EntranceFee == 0)
            {
                gameState.Park.Flags |= PARK_FLAGS_PARK_FREE_ENTRY;
            }
            else
            {
                gameState.Park.Flags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
            }

            gameState.Park.Flags &= ~PARK_FLAGS_SPRITES_INITIALISED;

            gameState.GuestInitialCash = std::clamp(gameState.GuestInitialCash, 10.00_GBP, kMaxEntranceFee);

            gameState.InitialCash = std::min<money64>(gameState.InitialCash, 100000);
            FinanceResetCashToInitial();

            gameState.BankLoan = std::clamp<money64>(gameState.BankLoan, 0.00_GBP, 5000000.00_GBP);

            gameState.MaxBankLoan = std::clamp<money64>(gameState.MaxBankLoan, 0.00_GBP, 5000000.00_GBP);

            gameState.BankLoanInterestRate = std::clamp<uint8_t>(gameState.BankLoanInterestRate, 5, MaxBankLoanInterestRate);
        }

        ClimateReset(gameState.Climate);

        News::InitQueue();
    }

    /**
     *
     *  rct2: 0x0067009A
     */
    void OpenWindowsForCurrentStep()
    {
        if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
        {
            return;
        }

        auto* windowMgr = Ui::GetWindowManager();

        switch (GetGameState().EditorStep)
        {
            case EditorStep::ObjectSelection:
                if (windowMgr->FindByClass(WindowClass::EditorObjectSelection) != nullptr)
                {
                    return;
                }

                if (windowMgr->FindByClass(WindowClass::InstallTrack) != nullptr)
                {
                    return;
                }

                if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
                {
                    ObjectManagerUnloadAllObjects();
                }

                ContextOpenWindow(WindowClass::EditorObjectSelection);
                break;
            case EditorStep::InventionsListSetUp:
                if (windowMgr->FindByClass(WindowClass::EditorInventionList) != nullptr)
                {
                    return;
                }

                ContextOpenWindow(WindowClass::EditorInventionList);
                break;
            case EditorStep::OptionsSelection:
                if (windowMgr->FindByClass(WindowClass::EditorScenarioOptions) != nullptr)
                {
                    return;
                }

                ContextOpenWindow(WindowClass::EditorScenarioOptions);
                break;
            case EditorStep::ObjectiveSelection:
                if (windowMgr->FindByClass(WindowClass::EditorObjectiveOptions) != nullptr)
                {
                    return;
                }

                ContextOpenWindow(WindowClass::EditorObjectiveOptions);
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
        auto& gameState = GetGameState();
        windowManager->SetMainView(gameState.SavedView, gameState.SavedViewZoom, gameState.SavedViewRotation);

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
            { ObjectType::Ride, STR_AT_LEAST_ONE_RIDE_OBJECT_MUST_BE_SELECTED },
            { ObjectType::Station, STR_AT_LEAST_ONE_STATION_OBJECT_MUST_BE_SELECTED },
            { ObjectType::TerrainSurface, STR_AT_LEAST_ONE_TERRAIN_SURFACE_OBJECT_MUST_BE_SELECTED },
            { ObjectType::TerrainEdge, STR_AT_LEAST_ONE_TERRAIN_EDGE_OBJECT_MUST_BE_SELECTED },
        };

        for (auto& pair : kBasicCheckPairs)
        {
            if (!EditorCheckObjectGroupAtLeastOneSelected(pair.first))
            {
                return { pair.first, pair.second };
            }
        }

        // No checks beyond this point apply to the track designer or track designs manager.
        const bool isTrackDesignerManager = gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER);
        if (isTrackDesignerManager)
        {
            return { ObjectType::None, kStringIdNone };
        }

        if (!EditorCheckObjectGroupAtLeastOneSurfaceSelected(false))
        {
            return { ObjectType::FootpathSurface, STR_AT_LEAST_ONE_FOOTPATH_NON_QUEUE_SURFACE_OBJECT_MUST_BE_SELECTED };
        }
        if (!EditorCheckObjectGroupAtLeastOneSurfaceSelected(true))
        {
            return { ObjectType::FootpathSurface, STR_AT_LEAST_ONE_FOOTPATH_QUEUE_SURFACE_OBJECT_MUST_BE_SELECTED };
        }

        constexpr std::pair<ObjectType, StringId> kParkCheckPairs[] = {
            { ObjectType::FootpathRailings, STR_AT_LEAST_ONE_FOOTPATH_RAILING_OBJECT_MUST_BE_SELECTED },
            { ObjectType::ParkEntrance, STR_PARK_ENTRANCE_TYPE_MUST_BE_SELECTED },
            { ObjectType::Water, STR_WATER_TYPE_MUST_BE_SELECTED },
            { ObjectType::PeepNames, STR_AT_LEAST_ONE_PEEP_NAMES_OBJECT_MUST_BE_SELECTED },
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
            { AnimationPeepType::Guest, STR_AT_LEAST_ONE_GUEST_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::Handyman, STR_AT_LEAST_ONE_HANDYMAN_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::Mechanic, STR_AT_LEAST_ONE_MECHANIC_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::Security, STR_AT_LEAST_ONE_SECURITY_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::Entertainer, STR_AT_LEAST_ONE_ENTERTAINER_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
        };

        for (auto& pair : kPeepCheckPairs)
        {
            if (!EditorCheckObjectGroupAtLeastOneOfPeepTypeSelected(EnumValue(pair.first)))
            {
                return { ObjectType::PeepAnimations, pair.second };
            }
        }

        return { ObjectType::None, kStringIdNone };
    }

    /**
     *
     *  rct2: 0x0066FEAC
     */
    ResultWithMessage CheckPark()
    {
        auto& gameState = GetGameState();
        int32_t parkSize = Park::UpdateSize(gameState);
        if (parkSize == 0)
        {
            return { false, STR_PARK_MUST_OWN_SOME_LAND };
        }

        if (gameState.Park.Entrances.empty())
        {
            return { false, STR_NO_PARK_ENTRANCES };
        }

        for (const auto& parkEntrance : gameState.Park.Entrances)
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

        if (gameState.PeepSpawns.empty())
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
            assert(static_cast<size_t>(objectType) < getObjectEntryGroupCount(ObjectType::Paths));
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
