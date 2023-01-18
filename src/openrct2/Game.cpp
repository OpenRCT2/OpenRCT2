/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Game.h"

#include "Cheats.h"
#include "Context.h"
#include "Editor.h"
#include "FileClassifier.h"
#include "GameStateSnapshots.h"
#include "Input.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "PlatformEnvironment.h"
#include "ReplayManager.h"
#include "actions/LoadOrQuitAction.h"
#include "audio/audio.h"
#include "config/Config.h"
#include "core/Console.hpp"
#include "core/File.h"
#include "core/FileScanner.h"
#include "core/Path.hpp"
#include "entity/EntityRegistry.h"
#include "entity/PatrolArea.h"
#include "entity/Peep.h"
#include "entity/Staff.h"
#include "interface/Colour.h"
#include "interface/Screenshot.h"
#include "interface/Viewport.h"
#include "interface/Window.h"
#include "localisation/Localisation.h"
#include "management/Finance.h"
#include "management/Marketing.h"
#include "management/Research.h"
#include "network/network.h"
#include "object/Object.h"
#include "object/ObjectList.h"
#include "platform/Platform.h"
#include "ride/Ride.h"
#include "ride/RideRatings.h"
#include "ride/Station.h"
#include "ride/Track.h"
#include "ride/TrackDesign.h"
#include "ride/Vehicle.h"
#include "scenario/Scenario.h"
#include "scripting/ScriptEngine.h"
#include "title/TitleScreen.h"
#include "ui/UiContext.h"
#include "ui/WindowManager.h"
#include "util/SawyerCoding.h"
#include "util/Util.h"
#include "windows/Intent.h"
#include "world/Banner.h"
#include "world/Climate.h"
#include "world/Entrance.h"
#include "world/Footpath.h"
#include "world/Map.h"
#include "world/MapAnimation.h"
#include "world/Park.h"
#include "world/Scenery.h"
#include "world/Surface.h"
#include "world/Water.h"

#include <algorithm>
#include <cstdio>
#include <iterator>
#include <memory>

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

uint32_t gCurrentTicks;
uint32_t gCurrentRealTimeTicks;

#ifdef ENABLE_SCRIPTING
static bool _mapChangedExpected;
#endif

using namespace OpenRCT2;

void GameResetSpeed()
{
    gGameSpeed = 1;
    WindowInvalidateByClass(WindowClass::TopToolbar);
}

void GameIncreaseGameSpeed()
{
    gGameSpeed = std::min(gConfigGeneral.DebuggingTools ? 5 : 4, gGameSpeed + 1);
    if (gGameSpeed == 5)
        gGameSpeed = 8;
    WindowInvalidateByClass(WindowClass::TopToolbar);
}

void GameReduceGameSpeed()
{
    gGameSpeed = std::max(1, gGameSpeed - 1);
    if (gGameSpeed == 7)
        gGameSpeed = 4;
    WindowInvalidateByClass(WindowClass::TopToolbar);
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

enum
{
    SPR_GAME_PALETTE_DEFAULT = 1532,
    SPR_GAME_PALETTE_WATER = 1533,
    SPR_GAME_PALETTE_WATER_DARKER_1 = 1534,
    SPR_GAME_PALETTE_WATER_DARKER_2 = 1535,
    SPR_GAME_PALETTE_3 = 1536,
    SPR_GAME_PALETTE_3_DARKER_1 = 1537,
    SPR_GAME_PALETTE_3_DARKER_2 = 1538,
    SPR_GAME_PALETTE_4 = 1539,
    SPR_GAME_PALETTE_4_DARKER_1 = 1540,
    SPR_GAME_PALETTE_4_DARKER_2 = 1541,
};

/**
 *
 *  rct2: 0x006838BD
 */
void UpdatePaletteEffects()
{
    auto water_type = static_cast<rct_water_type*>(ObjectEntryGetChunk(ObjectType::Water, 0));

    if (gClimateLightningFlash == 1)
    {
        // Change palette to lighter colour during lightning
        int32_t palette = SPR_GAME_PALETTE_DEFAULT;

        if (water_type != nullptr)
        {
            palette = water_type->image_id;
        }
        const G1Element* g1 = GfxGetG1Element(palette);
        if (g1 != nullptr)
        {
            int32_t xoffset = g1->x_offset;
            xoffset = xoffset * 4;
            uint8_t* paletteOffset = gGamePalette + xoffset;
            for (int32_t i = 0; i < g1->width; i++)
            {
                paletteOffset[(i * 4) + 0] = -((0xFF - g1->offset[(i * 3) + 0]) / 2) - 1;
                paletteOffset[(i * 4) + 1] = -((0xFF - g1->offset[(i * 3) + 1]) / 2) - 1;
                paletteOffset[(i * 4) + 2] = -((0xFF - g1->offset[(i * 3) + 2]) / 2) - 1;
            }
            UpdatePalette(gGamePalette, PALETTE_OFFSET_DYNAMIC, PALETTE_LENGTH_DYNAMIC);
        }
        gClimateLightningFlash++;
    }
    else
    {
        if (gClimateLightningFlash == 2)
        {
            // Change palette back to normal after lightning
            int32_t palette = SPR_GAME_PALETTE_DEFAULT;

            if (water_type != nullptr)
            {
                palette = water_type->image_id;
            }

            const G1Element* g1 = GfxGetG1Element(palette);
            if (g1 != nullptr)
            {
                int32_t xoffset = g1->x_offset;
                xoffset = xoffset * 4;
                uint8_t* paletteOffset = gGamePalette + xoffset;
                for (int32_t i = 0; i < g1->width; i++)
                {
                    paletteOffset[(i * 4) + 0] = g1->offset[(i * 3) + 0];
                    paletteOffset[(i * 4) + 1] = g1->offset[(i * 3) + 1];
                    paletteOffset[(i * 4) + 2] = g1->offset[(i * 3) + 2];
                }
            }
        }

        // Animate the water/lava/chain movement palette
        uint32_t shade = 0;
        if (gConfigGeneral.RenderWeatherGloom)
        {
            auto paletteId = ClimateGetWeatherGloomPaletteId(gClimateCurrent);
            if (paletteId != FilterPaletteID::PaletteNull)
            {
                shade = 1;
                if (paletteId != FilterPaletteID::PaletteDarken1)
                {
                    shade = 2;
                }
            }
        }
        uint32_t j = gPaletteEffectFrame;
        j = ((static_cast<uint16_t>((~j / 2) * 128) * 15) >> 16);
        uint32_t waterId = SPR_GAME_PALETTE_WATER;
        if (water_type != nullptr)
        {
            waterId = water_type->palette_index_1;
        }
        const G1Element* g1 = GfxGetG1Element(shade + waterId);
        if (g1 != nullptr)
        {
            uint8_t* vs = &g1->offset[j * 3];
            uint8_t* vd = &gGamePalette[PALETTE_OFFSET_WATER_WAVES * 4];
            int32_t n = PALETTE_LENGTH_WATER_WAVES;
            for (int32_t i = 0; i < n; i++)
            {
                vd[0] = vs[0];
                vd[1] = vs[1];
                vd[2] = vs[2];
                vs += 9;
                if (vs >= &g1->offset[9 * n])
                {
                    vs -= 9 * n;
                }
                vd += 4;
            }
        }

        waterId = SPR_GAME_PALETTE_3;
        if (water_type != nullptr)
        {
            waterId = water_type->palette_index_2;
        }
        g1 = GfxGetG1Element(shade + waterId);
        if (g1 != nullptr)
        {
            uint8_t* vs = &g1->offset[j * 3];
            uint8_t* vd = &gGamePalette[PALETTE_OFFSET_WATER_SPARKLES * 4];
            int32_t n = PALETTE_LENGTH_WATER_SPARKLES;
            for (int32_t i = 0; i < n; i++)
            {
                vd[0] = vs[0];
                vd[1] = vs[1];
                vd[2] = vs[2];
                vs += 9;
                if (vs >= &g1->offset[9 * n])
                {
                    vs -= 9 * n;
                }
                vd += 4;
            }
        }

        j = (static_cast<uint16_t>(gPaletteEffectFrame * -960) * 3) >> 16;
        waterId = SPR_GAME_PALETTE_4;
        g1 = GfxGetG1Element(shade + waterId);
        if (g1 != nullptr)
        {
            uint8_t* vs = &g1->offset[j * 3];
            uint8_t* vd = &gGamePalette[PALETTE_INDEX_243 * 4];
            int32_t n = 3;
            for (int32_t i = 0; i < n; i++)
            {
                vd[0] = vs[0];
                vd[1] = vs[1];
                vd[2] = vs[2];
                vs += 3;
                if (vs >= &g1->offset[3 * n])
                {
                    vs -= 3 * n;
                }
                vd += 4;
            }
        }

        UpdatePalette(gGamePalette, PALETTE_OFFSET_ANIMATED, PALETTE_LENGTH_ANIMATED);
        if (gClimateLightningFlash == 2)
        {
            UpdatePalette(gGamePalette, PALETTE_OFFSET_DYNAMIC, PALETTE_LENGTH_DYNAMIC);
            gClimateLightningFlash = 0;
        }
    }
}

void PauseToggle()
{
    gGamePaused ^= GAME_PAUSED_NORMAL;
    WindowInvalidateByClass(WindowClass::TopToolbar);
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
        safe_strcpy(buffer, temp.data(), length);
    }
}

// OpenRCT2 workaround to recalculate some values which are saved redundantly in the save to fix corrupted files.
// For example recalculate guest count by looking at all the guests instead of trusting the value in the file.
void GameFixSaveVars()
{
    // Recalculates peep count after loading a save to fix corrupted files
    uint32_t guestCount = 0;
    {
        for (auto guest : EntityList<Guest>())
        {
            if (!guest->OutsideOfPark)
            {
                guestCount++;
            }
        }
    }

    gNumGuestsInPark = guestCount;

    // Peeps to remove have to be cached here, as removing them from within the loop breaks iteration
    std::vector<Peep*> peepsToRemove;

    // Fix possibly invalid field values
    for (auto peep : EntityList<Guest>())
    {
        if (peep->CurrentRideStation.ToUnderlying() >= OpenRCT2::Limits::MaxStationsPerRide)
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
                LOG_WARNING("Couldn't find ride %u, resetting ride on peep %u", rideIdx, peep->sprite_index);
                peep->CurrentRide = RideId::GetNull();
                continue;
            }
            auto curName = peep->GetName();
            LOG_WARNING(
                "Peep %u (%s) has invalid ride station = %u for ride %u.", peep->sprite_index, curName.c_str(),
                srcStation.ToUnderlying(), rideIdx);
            auto station = RideGetFirstValidStationExit(*ride);
            if (station.IsNull())
            {
                LOG_WARNING("Couldn't find station, removing peep %u", peep->sprite_index);
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

    // Fixes broken saves where a surface element could be null
    // and broken saves with incorrect invisible map border tiles
    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            auto* surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y }.ToCoordsXY());

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
            if (x == 0 || x == gMapSize.x - 1 || y == 0 || y == gMapSize.y - 1)
            {
                surfaceElement->SetBaseZ(MINIMUM_LAND_HEIGHT_BIG);
                surfaceElement->SetClearanceZ(MINIMUM_LAND_HEIGHT_BIG);
                surfaceElement->SetSlope(0);
                surfaceElement->SetWaterHeight(0);
            }
        }
    }

    ResearchFix();

    // Fix banner list pointing to NULL map elements
    BannerResetBrokenIndex();

    // Fix banners which share their index
    BannerFixDuplicates();

    // Fix invalid vehicle sprite sizes, thus preventing visual corruption of sprites
    FixInvalidVehicleSpriteSizes();

    // Fix gParkEntrance locations for which the tile_element no longer exists
    ParkEntranceFixLocations();

    UpdateConsolidatedPatrolAreas();

    MapCountRemainingLandRights();
}

void GameLoadInit()
{
    IGameStateSnapshots* snapshots = GetContext()->GetGameStateSnapshots();
    snapshots->Reset();

    gScreenFlags = SCREEN_FLAGS_PLAYING;
    OpenRCT2::Audio::StopAll();
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

    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    windowManager->SetMainView(gSavedView, gSavedViewZoom, gSavedViewRotation);

    if (NetworkGetMode() != NETWORK_MODE_CLIENT)
    {
        GameActions::ClearQueue();
    }
    ResetEntitySpatialIndices();
    ResetAllSpriteQuadrantPlacements();
    ScenerySetDefaultPlacementConfiguration();

    auto intent = Intent(INTENT_ACTION_REFRESH_NEW_RIDES);
    ContextBroadcastIntent(&intent);

    gWindowUpdateTicks = 0;

    LoadPalette();

    if (!gOpenRCT2Headless)
    {
        intent = Intent(INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD);
        ContextBroadcastIntent(&intent);
        WindowUpdateAll();
    }

    OpenRCT2::Audio::StopTitleMusic();
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
    for (EntityId::UnderlyingType i = 0; i < MAX_ENTITIES; i++)
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
    if (ScenarioSave(name, gConfigGeneral.SavePluginData ? 1 : 0))
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

    int32_t autosavesToKeep = gConfigGeneral.AutosaveAmount;
    LimitAutosaveCount(autosavesToKeep - 1, (gScreenFlags & SCREEN_FLAGS_EDITOR));

    auto env = GetContext()->GetPlatformEnvironment();
    auto autosaveDir = Path::Combine(env->GetDirectoryPath(DIRBASE::USER, subDirectory), u8"autosave");
    Platform::EnsureDirectoryExists(autosaveDir.c_str());

    auto path = Path::Combine(autosaveDir, timeName);
    auto backupFileName = u8string(u8"autosave") + fileExtension + u8".bak";
    auto backupPath = Path::Combine(autosaveDir, backupFileName);

    if (File::Exists(path))
    {
        File::Copy(path, backupPath, true);
    }

    if (!ScenarioSave(path, saveFlags))
        Console::Error::WriteLine("Could not autosave the scenario. Is the save folder writeable?");
}

static void GameLoadOrQuitNoSavePromptCallback(int32_t result, const utf8* path)
{
    if (result == MODAL_RESULT_OK)
    {
        GameNotifyMapChange();
        GameUnloadScripts();
        WindowCloseByClass(WindowClass::EditorObjectSelection);
        GetContext()->LoadParkFromFile(path);
        GameLoadScripts();
        GameNotifyMapChanged();
        gIsAutosaveLoaded = gIsAutosave;
        gFirstTimeSaving = false;
    }
}

static void NewGameWindowCallback(const utf8* path)
{
    WindowCloseByClass(WindowClass::EditorObjectSelection);
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
                intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<void*>(GameLoadOrQuitNoSavePromptCallback));
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
            gGameSpeed = 1;
            gFirstTimeSaving = true;
            GameNotifyMapChange();
            GameUnloadScripts();
            TitleLoad();
            break;
        }
        case PromptMode::SaveBeforeNewGame:
        {
            auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::CloseSavePrompt);
            GameActions::Execute(&loadOrQuitAction);
            ToolCancel();
            auto intent = Intent(WindowClass::ScenarioSelect);
            intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<void*>(NewGameWindowCallback));
            ContextOpenIntent(&intent);
            break;
        }
        default:
            GameUnloadScripts();
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
