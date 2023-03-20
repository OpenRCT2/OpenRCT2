/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameState.h"

#include "./peep/GuestPathfinding.h"
#include "Context.h"
#include "Date.h"
#include "Editor.h"
#include "Game.h"
#include "GameState.h"
#include "GameStateSnapshots.h"
#include "Input.h"
#include "OpenRCT2.h"
#include "ReplayManager.h"
#include "actions/GameAction.h"
#include "config/Config.h"
#include "entity/EntityRegistry.h"
#include "entity/PatrolArea.h"
#include "entity/Staff.h"
#include "interface/Screenshot.h"
#include "localisation/Date.h"
#include "localisation/Localisation.h"
#include "management/NewsItem.h"
#include "network/network.h"
#include "platform/Platform.h"
#include "profiling/Profiling.h"
#include "ride/Vehicle.h"
#include "scenario/Scenario.h"
#include "scripting/ScriptEngine.h"
#include "title/TitleScreen.h"
#include "title/TitleSequencePlayer.h"
#include "ui/UiContext.h"
#include "windows/Intent.h"
#include "world/Climate.h"
#include "world/MapAnimation.h"
#include "world/Park.h"
#include "world/Scenery.h"

#include <algorithm>
#include <chrono>

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

GameState::GameState()
{
    _park = std::make_unique<Park>();
}

/**
 * Initialises the map, park etc. basically all S6 data.
 */
void GameState::InitAll(const TileCoordsXY& mapSize)
{
    PROFILED_FUNCTION();

    gInMapInitCode = true;
    gCurrentTicks = 0;

    MapInit(mapSize);
    _park->Initialise();
    FinanceInit();
    BannerInit();
    RideInitAll();
    ResetAllEntities();
    UpdateConsolidatedPatrolAreas();
    ResetDate();
    ClimateReset(ClimateType::CoolAndWet);
    News::InitQueue();

    gInMapInitCode = false;

    gNextGuestNumber = 1;

    ContextInit();
    ScenerySetDefaultPlacementConfiguration();

    auto intent = Intent(INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD);
    ContextBroadcastIntent(&intent);

    LoadPalette();

    CheatsReset();
    ClearRestrictedScenery();

#ifdef ENABLE_SCRIPTING
    auto& scriptEngine = GetContext()->GetScriptEngine();
    scriptEngine.ClearParkStorage();
#endif
}

/**
 * Function will be called every GAME_UPDATE_TIME_MS.
 * It has its own loop which might run multiple updates per call such as
 * when operating as a client it may run multiple updates to catch up with the server tick,
 * another influence can be the game speed setting.
 */
void GameState::Tick()
{
    PROFILED_FUNCTION();

    gInUpdateCode = true;

    // Normal game play will update only once every GAME_UPDATE_TIME_MS
    uint32_t numUpdates = 1;

    // 0x006E3AEC // screen_game_process_mouse_input();
    ScreenshotCheck();
    GameHandleKeyboardInput();

    if (GameIsNotPaused() && gPreviewingTitleSequenceInGame)
    {
        auto player = GetContext()->GetUiContext()->GetTitleSequencePlayer();
        if (player != nullptr)
        {
            player->Update();
        }
    }

    NetworkUpdate();

    if (NetworkGetMode() == NETWORK_MODE_CLIENT && NetworkGetStatus() == NETWORK_STATUS_CONNECTED
        && NetworkGetAuthstatus() == NetworkAuth::Ok)
    {
        numUpdates = std::clamp<uint32_t>(NetworkGetServerTick() - gCurrentTicks, 0, 10);
    }
    else
    {
        // Determine how many times we need to update the game
        if (gGameSpeed > 1)
        {
            // Update more often if game speed is above normal.
            numUpdates = 1 << (gGameSpeed - 1);
        }
    }

    bool isPaused = GameIsPaused();
    if (NetworkGetMode() == NETWORK_MODE_SERVER && gConfigNetwork.PauseServerIfNoClients)
    {
        // If we are headless we always have 1 player (host), pause if no one else is around.
        if (gOpenRCT2Headless && NetworkGetNumPlayers() == 1)
        {
            isPaused |= true;
        }
    }

    bool didRunSingleFrame = false;
    if (isPaused)
    {
        if (gDoSingleUpdate && NetworkGetMode() == NETWORK_MODE_NONE)
        {
            didRunSingleFrame = true;
            PauseToggle();
            numUpdates = 1;
        }
        else
        {
            // NOTE: Here are a few special cases that would be normally handled in UpdateLogic.
            // If the game is paused it will not call UpdateLogic at all.
            numUpdates = 0;

            if (NetworkGetMode() == NETWORK_MODE_SERVER)
            {
                // Make sure the client always knows about what tick the host is on.
                NetworkSendTick();
            }

            // Update the animation list. Note this does not
            // increment the map animation.
            MapAnimationInvalidateAll();

            // Post-tick network update
            NetworkProcessPending();

            // Post-tick game actions.
            GameActions::ProcessQueue();
        }
    }

    // Update the game one or more times
    for (uint32_t i = 0; i < numUpdates; i++)
    {
        UpdateLogic();
        if (gGameSpeed == 1)
        {
            if (InputGetState() == InputState::Reset || InputGetState() == InputState::Normal)
            {
                if (InputTestFlag(INPUT_FLAG_VIEWPORT_SCROLLING))
                {
                    InputSetFlag(INPUT_FLAG_VIEWPORT_SCROLLING, false);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    NetworkFlush();

    if (!gOpenRCT2Headless)
    {
        InputSetFlag(INPUT_FLAG_VIEWPORT_SCROLLING, false);

        // the flickering frequency is reduced by 4, compared to the original
        // it was done due to inability to reproduce original frequency
        // and decision that the original one looks too fast
        if (gCurrentRealTimeTicks % 4 == 0)
            gWindowMapFlashingFlags ^= MapFlashingFlags::SwitchColour;

        // Handle guest map flashing
        gWindowMapFlashingFlags &= ~MapFlashingFlags::FlashGuests;
        if (gWindowMapFlashingFlags & MapFlashingFlags::GuestListOpen)
            gWindowMapFlashingFlags |= MapFlashingFlags::FlashGuests;
        gWindowMapFlashingFlags &= ~MapFlashingFlags::GuestListOpen;

        // Handle staff map flashing
        gWindowMapFlashingFlags &= ~MapFlashingFlags::FlashStaff;
        if (gWindowMapFlashingFlags & MapFlashingFlags::StaffListOpen)
            gWindowMapFlashingFlags |= MapFlashingFlags::FlashStaff;
        gWindowMapFlashingFlags &= ~MapFlashingFlags::StaffListOpen;

        ContextUpdateMapTooltip();

        ContextHandleInput();
    }

    // Always perform autosave check, even when paused
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && !(gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
        && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
    {
        ScenarioAutosaveCheck();
    }

    WindowDispatchUpdateAll();

    if (didRunSingleFrame && GameIsNotPaused() && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        PauseToggle();
    }

    gDoSingleUpdate = false;
    gInUpdateCode = false;
}

void GameState::UpdateLogic(LogicTimings* timings)
{
    PROFILED_FUNCTION();

    auto start_time = std::chrono::high_resolution_clock::now();

    auto report_time = [timings, start_time](LogicTimePart part) {
        if (timings != nullptr)
        {
            timings->TimingInfo[part][timings->CurrentIdx] = std::chrono::high_resolution_clock::now() - start_time;
        }
    };

    gScreenAge++;
    if (gScreenAge == 0)
        gScreenAge--;

    GetContext()->GetReplayManager()->Update();

    NetworkUpdate();
    report_time(LogicTimePart::NetworkUpdate);

    if (NetworkGetMode() == NETWORK_MODE_SERVER)
    {
        if (NetworkGamestateSnapshotsEnabled())
        {
            CreateStateSnapshot();
        }

        // Send current tick out.
        NetworkSendTick();
    }
    else if (NetworkGetMode() == NETWORK_MODE_CLIENT)
    {
        // Don't run past the server, this condition can happen during map changes.
        if (NetworkGetServerTick() == gCurrentTicks)
        {
            return;
        }

        // Check desync.
        bool desynced = NetworkCheckDesynchronisation();
        if (desynced)
        {
            // If desync debugging is enabled and we are still connected request the specific game state from server.
            if (NetworkGamestateSnapshotsEnabled() && NetworkGetStatus() == NETWORK_STATUS_CONNECTED)
            {
                // Create snapshot from this tick so we can compare it later
                // as we won't pause the game on this event.
                CreateStateSnapshot();

                NetworkRequestGamestateSnapshot();
            }
        }
    }

#ifdef ENABLE_SCRIPTING
    // Stash the current day number before updating the date so that we
    // know if the day number changes on this tick.
    auto day = _date.GetDay();
#endif

    _date.Update();
    report_time(LogicTimePart::Date);

    ScenarioUpdate();
    report_time(LogicTimePart::Scenario);
    ClimateUpdate();
    report_time(LogicTimePart::Climate);
    MapUpdateTiles();
    report_time(LogicTimePart::MapTiles);
    // Temporarily remove provisional paths to prevent peep from interacting with them
    MapRemoveProvisionalElements();
    report_time(LogicTimePart::MapStashProvisionalElements);
    MapUpdatePathWideFlags();
    report_time(LogicTimePart::MapPathWideFlags);
    PeepUpdateAll();
    report_time(LogicTimePart::Peep);
    MapRestoreProvisionalElements();
    report_time(LogicTimePart::MapRestoreProvisionalElements);
    VehicleUpdateAll();
    report_time(LogicTimePart::Vehicle);
    UpdateAllMiscEntities();
    report_time(LogicTimePart::Misc);
    Ride::UpdateAll();
    report_time(LogicTimePart::Ride);

    if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        _park->Update(_date);
    }
    report_time(LogicTimePart::Park);

    ResearchUpdate();
    report_time(LogicTimePart::Research);
    RideRatingsUpdateAll();
    report_time(LogicTimePart::RideRatings);
    RideMeasurementsUpdate();
    report_time(LogicTimePart::RideMeasurments);
    News::UpdateCurrentItem();
    report_time(LogicTimePart::News);

    MapAnimationInvalidateAll();
    report_time(LogicTimePart::MapAnimation);
    VehicleSoundsUpdate();
    PeepUpdateCrowdNoise();
    ClimateUpdateSound();
    report_time(LogicTimePart::Sounds);
    EditorOpenWindowsForCurrentStep();

    // Update windows
    // WindowDispatchUpdateAll();

    // Start autosave timer after update
    if (gLastAutoSaveUpdate == AUTOSAVE_PAUSE)
    {
        gLastAutoSaveUpdate = Platform::GetTicks();
    }

    GameActions::ProcessQueue();
    report_time(LogicTimePart::GameActions);

    NetworkProcessPending();
    NetworkFlush();
    report_time(LogicTimePart::NetworkFlush);

    gCurrentTicks++;
    gSavedAge++;

#ifdef ENABLE_SCRIPTING
    auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
    hookEngine.Call(HOOK_TYPE::INTERVAL_TICK, true);

    if (day != _date.GetDay())
    {
        hookEngine.Call(HOOK_TYPE::INTERVAL_DAY, true);
    }
    report_time(LogicTimePart::Scripts);
#endif

    if (timings != nullptr)
    {
        timings->CurrentIdx = (timings->CurrentIdx + 1) % LOGIC_UPDATE_MEASUREMENTS_COUNT;
    }
}

void GameState::CreateStateSnapshot()
{
    PROFILED_FUNCTION();

    IGameStateSnapshots* snapshots = GetContext()->GetGameStateSnapshots();

    auto& snapshot = snapshots->CreateSnapshot();
    snapshots->Capture(snapshot);
    snapshots->LinkSnapshot(snapshot, gCurrentTicks, ScenarioRandState().s0);
}

void GameState::SetDate(Date newDate)
{
    _date = newDate;
}

/**
 *
 *  rct2: 0x006C4494
 */
void GameState::ResetDate()
{
    _date = OpenRCT2::Date();
    gCurrentRealTimeTicks = 0;
}
