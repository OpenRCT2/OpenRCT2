/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Game.h"

#include "Cheats.h"
#include "Context.h"
#include "Diagnostic.h"
#include "Editor.h"
#include "FileClassifier.h"
#include "GameState.h"
#include "GameStateSnapshots.h"
#include "Input.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "PlatformEnvironment.h"
#include "ReplayManager.h"
#include "actions/GameSetSpeedAction.h"
#include "actions/LoadOrQuitAction.h"
#include "audio/audio.h"
#include "config/Config.h"
#include "core/Console.hpp"
#include "core/File.h"
#include "core/FileScanner.h"
#include "core/Money.hpp"
#include "core/Path.hpp"
#include "core/SawyerCoding.h"
#include "core/String.hpp"
#include "entity/EntityRegistry.h"
#include "entity/PatrolArea.h"
#include "entity/Peep.h"
#include "entity/Staff.h"
#include "interface/Colour.h"
#include "interface/Screenshot.h"
#include "interface/Viewport.h"
#include "interface/Window.h"
#include "management/Finance.h"
#include "management/Marketing.h"
#include "management/Research.h"
#include "network/network.h"
#include "object/Object.h"
#include "object/ObjectEntryManager.h"
#include "object/ObjectList.h"
#include "object/WaterEntry.h"
#include "platform/Platform.h"
#include "rct12/CSStringConverter.h"
#include "ride/Ride.h"
#include "ride/RideRatings.h"
#include "ride/Station.h"
#include "ride/Track.h"
#include "ride/TrackDesign.h"
#include "ride/Vehicle.h"
#include "scenario/Scenario.h"
#include "scenes/title/TitleScene.h"
#include "scripting/ScriptEngine.h"
#include "ui/UiContext.h"
#include "ui/WindowManager.h"
#include "windows/Intent.h"
#include "world/Banner.h"
#include "world/Climate.h"
#include "world/Entrance.h"
#include "world/Footpath.h"
#include "world/Map.h"
#include "world/MapAnimation.h"
#include "world/Park.h"
#include "world/Scenery.h"
#include "world/tile_element/SurfaceElement.h"

#include <cstdio>
#include <iterator>
#include <memory>

using namespace OpenRCT2;

uint16_t gCurrentDeltaTime;
uint8_t gGamePaused = 0;
int32_t gGameSpeed = 1;
bool gDoSingleUpdate = false;
float gDayNightCycle = 0;
bool gInUpdateCode = false;
bool gInMapInitCode = false;
std::string gCurrentLoadedPath;
bool gIsAutosave = false;
bool gIsAutosaveLoaded = false;

bool gLoadKeepWindowsOpen = false;

uint32_t gCurrentRealTimeTicks;

#ifdef ENABLE_SCRIPTING
static bool _mapChangedExpected;
#endif

using namespace OpenRCT2;

void GameResetSpeed()
{
    auto setSpeedAction = GameSetSpeedAction(1);
    GameActions::Execute(&setSpeedAction);
}

void GameIncreaseGameSpeed()
{
    auto newSpeed = std::min(Config::Get().general.DebuggingTools ? 5 : 4, gGameSpeed + 1);
    if (newSpeed == 5)
        newSpeed = 8;

    auto setSpeedAction = GameSetSpeedAction(newSpeed);
    GameActions::Execute(&setSpeedAction);
}

void GameReduceGameSpeed()
{
    auto newSpeed = std::max(1, gGameSpeed - 1);
    if (newSpeed == 7)
        newSpeed = 4;

    auto setSpeedAction = GameSetSpeedAction(newSpeed);
    GameActions::Execute(&setSpeedAction);
}

/**
 *
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
void GameCreateWindows()
{
    ContextOpenWindow(WindowClass::MainWindow);
    ContextOpenWindow(WindowClass::TopToolbar);
    ContextOpenWindow(WindowClass::BottomToolbar);
    WindowResizeGui(ContextGetWidth(), ContextGetHeight());
}

void PauseToggle()
{
    gGamePaused ^= GAME_PAUSED_NORMAL;

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::TopToolbar);

    if (gGamePaused & GAME_PAUSED_NORMAL)
    {
        OpenRCT2::Audio::StopAll();
    }
}

bool GameIsPaused()
{
    return gGamePaused != 0;
}

bool GameIsNotPaused()
{
    return gGamePaused == 0;
}

/**
 *
 *  rct2: 0x0066DC0F
 */
static void LoadLandscape()
{
    auto intent = Intent(WindowClass::Loadsave);
    intent.PutExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_LANDSCAPE);
    ContextOpenIntent(&intent);
}

void RCT2StringToUTF8Self(char* buffer, size_t length)
{
    if (length > 0)
    {
        auto temp = RCT2StringToUTF8(buffer, RCT2LanguageId::EnglishUK);
        String::safeUtf8Copy(buffer, temp.data(), length);
    }
}

static void FixGuestsHeadingToParkCount()
{
    auto& gameState = GetGameState();

    uint32_t guestsHeadingToPark = 0;

    for (auto* peep : EntityList<Guest>())
    {
        if (peep->OutsideOfPark && peep->State != PeepState::LeavingPark)
        {
            guestsHeadingToPark++;
        }
    }

    if (gameState.NumGuestsHeadingForPark != guestsHeadingToPark)
    {
        LOG_WARNING(
            "Corrected bad amount of guests heading to park: %u -> %u", gameState.NumGuestsHeadingForPark, guestsHeadingToPark);
    }

    gameState.NumGuestsHeadingForPark = guestsHeadingToPark;
}

static void FixGuestCount()
{
    // Recalculates peep count after loading a save to fix corrupted files
    uint32_t guestCount = 0;

    for (auto guest : EntityList<Guest>())
    {
        if (!guest->OutsideOfPark)
        {
            guestCount++;
        }
    }

    auto& gameState = GetGameState();
    if (gameState.NumGuestsInPark != guestCount)
    {
        LOG_WARNING("Corrected bad amount of guests in park: %u -> %u", gameState.NumGuestsInPark, guestCount);
    }

    gameState.NumGuestsInPark = guestCount;
}

static void FixPeepsWithInvalidRideReference()
{
    // Peeps to remove have to be cached here, as removing them from within the loop breaks iteration
    std::vector<Peep*> peepsToRemove;

    // Fix possibly invalid field values
    for (auto peep : EntityList<Guest>())
    {
        if (peep->CurrentRideStation.ToUnderlying() >= OpenRCT2::Limits::kMaxStationsPerRide)
        {
            const auto srcStation = peep->CurrentRideStation;
            const auto rideIdx = peep->CurrentRide;
            if (rideIdx.IsNull())
            {
                continue;
            }
            Ride* ride = GetRide(rideIdx);
            if (ride == nullptr)
            {
                LOG_WARNING("Couldn't find ride %u, resetting ride on peep %u", rideIdx, peep->Id);
                peep->CurrentRide = RideId::GetNull();
                continue;
            }
            auto curName = peep->GetName();
            LOG_WARNING(
                "Peep %u (%s) has invalid ride station = %u for ride %u.", peep->Id, curName.c_str(), srcStation.ToUnderlying(),
                rideIdx);
            auto station = RideGetFirstValidStationExit(*ride);
            if (station.IsNull())
            {
                LOG_WARNING("Couldn't find station, removing peep %u", peep->Id);
                peepsToRemove.push_back(peep);
            }
            else
            {
                LOG_WARNING("Amending ride station to %u.", station);
                peep->CurrentRideStation = station;
            }
        }
    }

    if (!peepsToRemove.empty())
    {
        // Some broken saves have broken spatial indexes
        ResetEntitySpatialIndices();
    }

    for (auto ptr : peepsToRemove)
    {
        ptr->Remove();
    }
}

static void FixInvalidSurfaces()
{
    // Fixes broken saves where a surface element could be null
    // and broken saves with incorrect invisible map border tiles

    for (int32_t y = 0; y < kMaximumMapSizeTechnical; y++)
    {
        for (int32_t x = 0; x < kMaximumMapSizeTechnical; x++)
        {
            auto* surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });

            if (surfaceElement == nullptr)
            {
                LOG_ERROR("Null map element at x = %d and y = %d. Fixing...", x, y);
                surfaceElement = TileElementInsert<SurfaceElement>(TileCoordsXYZ{ x, y, 14 }.ToCoordsXYZ(), 0b0000);
                if (surfaceElement == nullptr)
                {
                    LOG_ERROR("Unable to fix: Map element limit reached.");
                    return;
                }
            }

            // Fix the invisible border tiles.
            // At this point, we can be sure that surfaceElement is not NULL.
            auto& gameState = GetGameState();
            if (x == 0 || x == gameState.MapSize.x - 1 || y == 0 || y == gameState.MapSize.y - 1)
            {
                surfaceElement->SetBaseZ(kMinimumLandZ);
                surfaceElement->SetClearanceZ(kMinimumLandZ);
                surfaceElement->SetSlope(0);
                surfaceElement->SetWaterHeight(0);
            }
        }
    }
}

// OpenRCT2 workaround to recalculate some values which are saved redundantly in the save to fix corrupted files.
// For example recalculate guest count by looking at all the guests instead of trusting the value in the file.
void GameFixSaveVars()
{
    FixGuestsHeadingToParkCount();

    FixGuestCount();

    FixPeepsWithInvalidRideReference();

    FixInvalidSurfaces();

    ResearchFix();

    // Fix banners which share their index
    BannerApplyFixes();

    // Fix invalid vehicle sprite sizes, thus preventing visual corruption of sprites
    FixInvalidVehicleSpriteSizes();

    // Fix gParkEntrance locations for which the tile_element no longer exists
    ParkEntranceFixLocations();

    UpdateConsolidatedPatrolAreas();

    MapCountRemainingLandRights();

    // Update sprite bounds, rather than relying on stored data
    PeepUpdateAllBoundingBoxes();
}

void GameLoadInit()
{
    auto* context = GetContext();

    IGameStateSnapshots* snapshots = context->GetGameStateSnapshots();
    snapshots->Reset();

    context->SetActiveScene(context->GetGameScene());

    if (!gLoadKeepWindowsOpen)
    {
        ViewportInitAll();
        GameCreateWindows();
    }
    else
    {
        auto* mainWindow = WindowGetMain();
        WindowUnfollowSprite(*mainWindow);
    }

    auto windowManager = context->GetUiContext()->GetWindowManager();
    auto& gameState = GetGameState();
    windowManager->SetMainView(gameState.SavedView, gameState.SavedViewZoom, gameState.SavedViewRotation);

    if (NetworkGetMode() != NETWORK_MODE_CLIENT)
    {
        GameActions::ClearQueue();
    }
    ResetEntitySpatialIndices();
    ResetAllSpriteQuadrantPlacements();

    gWindowUpdateTicks = 0;
    gCurrentRealTimeTicks = 0;

    LoadPalette();

    if (!gOpenRCT2Headless)
    {
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG));
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_NEW_RIDES));
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD));
    }

    gGameSpeed = 1;
}

void GameLoadScripts()
{
#ifdef ENABLE_SCRIPTING
    GetContext()->GetScriptEngine().LoadTransientPlugins();
#endif
}

void GameUnloadScripts()
{
#ifdef ENABLE_SCRIPTING
    GetContext()->GetScriptEngine().UnloadTransientPlugins();
#endif
}

void GameNotifyMapChange()
{
#ifdef ENABLE_SCRIPTING
    // Ensure we don't get a two lots of change events
    if (_mapChangedExpected)
        return;

    using namespace OpenRCT2::Scripting;

    auto& scriptEngine = GetContext()->GetScriptEngine();
    auto& hookEngine = scriptEngine.GetHookEngine();
    hookEngine.Call(HOOK_TYPE::MAP_CHANGE, false);
    _mapChangedExpected = true;
#endif
}

void GameNotifyMapChanged()
{
#ifdef ENABLE_SCRIPTING
    using namespace OpenRCT2::Scripting;

    auto& scriptEngine = GetContext()->GetScriptEngine();
    auto& hookEngine = scriptEngine.GetHookEngine();
    hookEngine.Call(HOOK_TYPE::MAP_CHANGED, false);
    _mapChangedExpected = false;
#endif
}

/**
 *
 *  rct2: 0x0069E9A7
 * Call after a rotation or loading of a save to reset sprite quadrants
 */
void ResetAllSpriteQuadrantPlacements()
{
    for (EntityId::UnderlyingType i = 0; i < kMaxEntities; i++)
    {
        auto* spr = GetEntity(EntityId::FromUnderlying(i));
        if (spr != nullptr && spr->Type != EntityType::Null)
        {
            spr->MoveTo(spr->GetLocation());
        }
    }
}

void SaveGame()
{
    if (!gFirstTimeSaving && !gIsAutosaveLoaded)
    {
        const auto savePath = Path::WithExtension(gScenarioSavePath, ".park");
        SaveGameWithName(savePath);
    }
    else
    {
        SaveGameAs();
    }
}

void SaveGameCmd(u8string_view name /* = {} */)
{
    if (name.empty())
    {
        const auto savePath = Path::WithExtension(gScenarioSavePath, ".park");

        SaveGameWithName(savePath);
    }
    else
    {
        auto env = GetContext()->GetPlatformEnvironment();
        auto savePath = Path::Combine(env->GetDirectoryPath(DIRBASE::USER, DIRID::SAVE), u8string(name) + u8".park");
        SaveGameWithName(savePath);
    }
}

void SaveGameWithName(u8string_view name)
{
    LOG_VERBOSE("Saving to %s", u8string(name).c_str());

    auto& gameState = GetGameState();
    if (ScenarioSave(gameState, name, Config::Get().general.SavePluginData ? 1 : 0))
    {
        LOG_VERBOSE("Saved to %s", u8string(name).c_str());
        gCurrentLoadedPath = name;
        gIsAutosaveLoaded = false;
        gScreenAge = 0;
    }
}

std::unique_ptr<Intent> CreateSaveGameAsIntent()
{
    auto name = Path::GetFileNameWithoutExtension(gScenarioSavePath);

    auto intent = std::make_unique<Intent>(WindowClass::Loadsave);
    intent->PutExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_GAME);
    intent->PutExtra(INTENT_EXTRA_PATH, name);

    return intent;
}

void SaveGameAs()
{
    auto intent = CreateSaveGameAsIntent();
    ContextOpenIntent(intent.get());
}

static void LimitAutosaveCount(const size_t numberOfFilesToKeep, bool processLandscapeFolder)
{
    size_t autosavesCount = 0;
    size_t numAutosavesToDelete = 0;

    auto environment = GetContext()->GetPlatformEnvironment();
    auto folderDirectory = environment->GetDirectoryPath(DIRBASE::USER, DIRID::SAVE);
    char const* fileFilter = "autosave_*.park";
    if (processLandscapeFolder)
    {
        folderDirectory = environment->GetDirectoryPath(DIRBASE::USER, DIRID::LANDSCAPE);
        fileFilter = "autosave_*.park";
    }

    const u8string filter = Path::Combine(folderDirectory, "autosave", fileFilter);

    // At first, count how many autosaves there are
    {
        auto scanner = Path::ScanDirectory(filter, false);
        while (scanner->Next())
        {
            autosavesCount++;
        }
    }

    // If there are fewer autosaves than the number of files to keep we don't need to delete anything
    if (autosavesCount <= numberOfFilesToKeep)
    {
        return;
    }

    std::vector<u8string> autosaveFiles;
    {
        auto scanner = Path::ScanDirectory(filter, false);
        for (size_t i = 0; i < autosavesCount; i++)
        {
            if (scanner->Next())
            {
                autosaveFiles.emplace_back(Path::Combine(folderDirectory, "autosave", scanner->GetPathRelative()));
            }
        }
    }

    std::sort(autosaveFiles.begin(), autosaveFiles.end(), [](const auto& saveFile0, const auto& saveFile1) {
        return saveFile0.compare(saveFile1) < 0;
    });

    // Calculate how many saves we need to delete.
    numAutosavesToDelete = autosaveFiles.size() - numberOfFilesToKeep;

    for (size_t i = 0; numAutosavesToDelete > 0; i++, numAutosavesToDelete--)
    {
        if (!File::Delete(autosaveFiles[i]))
        {
            LOG_WARNING("Failed to delete autosave file: %s", autosaveFiles[i].data());
        }
    }
}

void GameAutosave()
{
    auto subDirectory = DIRID::SAVE;
    const char* fileExtension = ".park";
    uint32_t saveFlags = 0x80000000;
    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
    {
        subDirectory = DIRID::LANDSCAPE;
        fileExtension = ".park";
        saveFlags |= 2;
    }

    // Retrieve current time
    auto currentDate = Platform::GetDateLocal();
    auto currentTime = Platform::GetTimeLocal();

    utf8 timeName[44];
    snprintf(
        timeName, sizeof(timeName), "autosave_%04u-%02u-%02u_%02u-%02u-%02u%s", currentDate.year, currentDate.month,
        currentDate.day, currentTime.hour, currentTime.minute, currentTime.second, fileExtension);

    int32_t autosavesToKeep = Config::Get().general.AutosaveAmount;
    LimitAutosaveCount(autosavesToKeep - 1, (gScreenFlags & SCREEN_FLAGS_EDITOR));

    auto env = GetContext()->GetPlatformEnvironment();
    auto autosaveDir = Path::Combine(env->GetDirectoryPath(DIRBASE::USER, subDirectory), u8"autosave");
    Path::CreateDirectory(autosaveDir);

    auto path = Path::Combine(autosaveDir, timeName);
    auto backupFileName = u8string(u8"autosave") + fileExtension + u8".bak";
    auto backupPath = Path::Combine(autosaveDir, backupFileName);

    if (File::Exists(path))
    {
        File::Copy(path, backupPath, true);
    }

    auto& gameState = GetGameState();

    if (!ScenarioSave(gameState, path, saveFlags))
        Console::Error::WriteLine("Could not autosave the scenario. Is the save folder writeable?");
}

static void GameLoadOrQuitNoSavePromptCallback(int32_t result, const utf8* path)
{
    if (result == MODAL_RESULT_OK)
    {
        GameNotifyMapChange();
        GameUnloadScripts();

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::EditorObjectSelection);

        GameLoadScripts();
        GameNotifyMapChanged();
        gIsAutosaveLoaded = gIsAutosave;
        gFirstTimeSaving = false;
    }
}

static void NewGameWindowCallback(const utf8* path)
{
    // Closing this will cause a Ride window to pop up, so we have to do this to ensure that
    // no windows are open (besides the toolbars and LoadSave window).
    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->CloseByClass(WindowClass::RideConstruction);
    windowMgr->CloseAllExceptClass(WindowClass::Loadsave);

    GameNotifyMapChange();
    GetContext()->LoadParkFromFile(path, false, true);
    GameLoadScripts();
    GameNotifyMapChanged();
}

/**
 *
 *  rct2: 0x0066DB79
 */
void GameLoadOrQuitNoSavePrompt()
{
    switch (gSavePromptMode)
    {
        case PromptMode::SaveBeforeLoad:
        {
            auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::CloseSavePrompt);
            GameActions::Execute(&loadOrQuitAction);
            ToolCancel();
            if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
            {
                LoadLandscape();
            }
            else
            {
                auto intent = Intent(WindowClass::Loadsave);
                intent.PutExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME);
                intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(GameLoadOrQuitNoSavePromptCallback));
                ContextOpenIntent(&intent);
            }
            break;
        }
        case PromptMode::SaveBeforeQuit:
        {
            auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::CloseSavePrompt);
            GameActions::Execute(&loadOrQuitAction);
            ToolCancel();
            if (InputTestFlag(INPUT_FLAG_5))
            {
                InputSetFlag(INPUT_FLAG_5, false);
            }
            GameResetSpeed();
            gFirstTimeSaving = true;
            GameNotifyMapChange();
            GameUnloadScripts();

            auto* context = OpenRCT2::GetContext();
            context->SetActiveScene(context->GetTitleScene());
            break;
        }
        case PromptMode::SaveBeforeNewGame:
        {
            auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::CloseSavePrompt);
            GameActions::Execute(&loadOrQuitAction);
            ToolCancel();
            auto intent = Intent(WindowClass::ScenarioSelect);
            intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(NewGameWindowCallback));
            ContextOpenIntent(&intent);
            break;
        }
        default:
            GameUnloadScripts();
            ResetAllEntities();
            OpenRCT2Finish();
            break;
    }
}

void StartSilentRecord()
{
    std::string name = Path::Combine(
        OpenRCT2::GetContext()->GetPlatformEnvironment()->GetDirectoryPath(OpenRCT2::DIRBASE::USER), u8"debug_replay.parkrep");
    auto* replayManager = OpenRCT2::GetContext()->GetReplayManager();
    if (replayManager->StartRecording(name, OpenRCT2::k_MaxReplayTicks, OpenRCT2::IReplayManager::RecordType::SILENT))
    {
        OpenRCT2::ReplayRecordInfo info;
        replayManager->GetCurrentReplayInfo(info);
        gSilentRecordingName = info.FilePath;

        const char* logFmt = "Silent replay recording started: (%s) %s\n";
        Console::WriteLine(logFmt, info.Name.c_str(), info.FilePath.c_str());
    }
}

bool StopSilentRecord()
{
    auto* replayManager = OpenRCT2::GetContext()->GetReplayManager();
    if (!replayManager->IsRecording() && !replayManager->IsNormalising())
    {
        return false;
    }

    OpenRCT2::ReplayRecordInfo info;
    replayManager->GetCurrentReplayInfo(info);

    if (replayManager->StopRecording())
    {
        const char* logFmt = "Replay recording stopped: (%s) %s\n"
                             "  Ticks: %u\n"
                             "  Commands: %u\n"
                             "  Checksums: %u";

        Console::WriteLine(logFmt, info.Name.c_str(), info.FilePath.c_str(), info.Ticks, info.NumCommands, info.NumChecksums);

        return true;
    }

    return false;
}

void PrepareMapForSave()
{
    ViewportSetSavedView();

#ifdef ENABLE_SCRIPTING
    auto& scriptEngine = GetContext()->GetScriptEngine();
    auto& hookEngine = scriptEngine.GetHookEngine();
    if (hookEngine.HasSubscriptions(OpenRCT2::Scripting::HOOK_TYPE::MAP_SAVE))
    {
        hookEngine.Call(OpenRCT2::Scripting::HOOK_TYPE::MAP_SAVE, false);
    }
#endif
}
