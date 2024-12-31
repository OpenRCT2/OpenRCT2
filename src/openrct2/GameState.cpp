/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameState.h"

#include "Game.h"
#include "GameStateSnapshots.h"
#include "Input.h"
#include "OpenRCT2.h"
#include "ReplayManager.h"
#include "actions/GameAction.h"
#include "config/Config.h"
#include "entity/EntityTweener.h"
#include "entity/PatrolArea.h"
#include "interface/Screenshot.h"
#include "platform/Platform.h"
#include "profiling/Profiling.h"
#include "ride/Vehicle.h"
#include "scenes/title/TitleScene.h"
#include "scenes/title/TitleSequencePlayer.h"
#include "scripting/ScriptEngine.h"
#include "ui/UiContext.h"
#include "windows/Intent.h"
#include "world/Scenery.h"

using namespace OpenRCT2::Scripting;

namespace OpenRCT2
{
    static auto _gameState = std::make_unique<GameState_t>();

    GameState_t& GetGameState()
    {
        return *_gameState;
    }

    void SwapGameState(std::unique_ptr<GameState_t>& otherState)
    {
        _gameState.swap(otherState);
    }

    /**
     * Initialises the map, park etc. basically all S6 data.
     */
    void gameStateInitAll(GameState_t& gameState, const TileCoordsXY& mapSize)
    {
        PROFILED_FUNCTION();

        gInMapInitCode = true;
        gameState.CurrentTicks = 0;

        MapInit(mapSize);
        Park::Initialise(gameState);
        FinanceInit();
        BannerInit(gameState);
        RideInitAll();
        ResetAllEntities();
        UpdateConsolidatedPatrolAreas();
        ResetDate();
        ClimateReset(ClimateType::CoolAndWet);
        News::InitQueue();

        gInMapInitCode = false;

        gameState.NextGuestNumber = 1;

        ContextInit();

        auto sceneryIntent = Intent(INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG);
        ContextBroadcastIntent(&sceneryIntent);

        auto clipboardIntent = Intent(INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD);
        ContextBroadcastIntent(&clipboardIntent);

        LoadPalette();

        CheatsReset();
        ClearRestrictedScenery();

#ifdef ENABLE_SCRIPTING
        auto& scriptEngine = GetContext()->GetScriptEngine();
        scriptEngine.ClearParkStorage();
#endif

        EntityTweener::Get().Reset();
    }

    /**
     * Function will be called every kGameUpdateTimeMS.
     * It has its own loop which might run multiple updates per call such as
     * when operating as a client it may run multiple updates to catch up with the server tick,
     * another influence can be the game speed setting.
     */
    void gameStateTick()
    {
        PROFILED_FUNCTION();

        // Normal game play will update only once every kGameUpdateTimeMS
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
            numUpdates = std::clamp<uint32_t>(NetworkGetServerTick() - GetGameState().CurrentTicks, 0, 10);
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
        if (NetworkGetMode() == NETWORK_MODE_SERVER && Config::Get().network.PauseServerIfNoClients)
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

                // Keep updating the money effect even when paused.
                UpdateMoneyEffect();

                // Update the animation list. Note this does not
                // increment the map animation.
                MapAnimationInvalidateAll();

                // Post-tick network update
                NetworkProcessPending();

                // Post-tick game actions.
                GameActions::ProcessQueue();
                UpdateEntitiesSpatialIndex();
            }
        }

        // Update the game one or more times
        for (uint32_t i = 0; i < numUpdates; i++)
        {
            gameStateUpdateLogic();
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
            // Don't call UpdateLogic again if the game was just paused.
            isPaused |= GameIsPaused();
            if (isPaused)
                break;
        }

        NetworkFlush();

        if (!gOpenRCT2Headless)
        {
            InputSetFlag(INPUT_FLAG_VIEWPORT_SCROLLING, false);
        }

        // Always perform autosave check, even when paused
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && !(gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
            && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            ScenarioAutosaveCheck();
        }

        if (didRunSingleFrame && GameIsNotPaused() && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            PauseToggle();
        }

        gDoSingleUpdate = false;
    }

    static void gameStateCreateStateSnapshot()
    {
        PROFILED_FUNCTION();

        IGameStateSnapshots* snapshots = GetContext()->GetGameStateSnapshots();

        auto& snapshot = snapshots->CreateSnapshot();
        snapshots->Capture(snapshot);
        snapshots->LinkSnapshot(snapshot, GetGameState().CurrentTicks, ScenarioRandState().s0);
    }

    void gameStateUpdateLogic()
    {
        PROFILED_FUNCTION();

        gInUpdateCode = true;

        gScreenAge++;
        if (gScreenAge == 0)
            gScreenAge--;

        GetContext()->GetReplayManager()->Update();

        NetworkUpdate();

        auto& gameState = GetGameState();

        if (NetworkGetMode() == NETWORK_MODE_SERVER)
        {
            if (NetworkGamestateSnapshotsEnabled())
            {
                gameStateCreateStateSnapshot();
            }

            // Send current tick out.
            NetworkSendTick();
        }
        else if (NetworkGetMode() == NETWORK_MODE_CLIENT)
        {
            // Don't run past the server, this condition can happen during map changes.
            if (NetworkGetServerTick() == gameState.CurrentTicks)
            {
                gInUpdateCode = false;
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
                    gameStateCreateStateSnapshot();

                    NetworkRequestGamestateSnapshot();
                }
            }
        }

#ifdef ENABLE_SCRIPTING
        // Stash the current day number before updating the date so that we
        // know if the day number changes on this tick.
        auto day = gameState.Date.GetDay();
#endif

        DateUpdate(gameState);

        ScenarioUpdate(gameState);
        ClimateUpdate();
        MapUpdateTiles();

        // Temporarily remove provisional paths to prevent peep from interacting with them
        auto removeProvisionalIntent = Intent(INTENT_ACTION_REMOVE_PROVISIONAL_ELEMENTS);
        ContextBroadcastIntent(&removeProvisionalIntent);

        MapUpdatePathWideFlags();
        PeepUpdateAll();
        auto restoreProvisionalIntent = Intent(INTENT_ACTION_RESTORE_PROVISIONAL_ELEMENTS);
        ContextBroadcastIntent(&restoreProvisionalIntent);
        VehicleUpdateAll();
        UpdateAllMiscEntities();
        Ride::UpdateAll();

        if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
        {
            Park::Update(gameState, gameState.Date);
        }

        ResearchUpdate();
        RideRatingsUpdateAll();
        RideMeasurementsUpdate();
        News::UpdateCurrentItem();

        MapAnimationInvalidateAll();
        VehicleSoundsUpdate();
        PeepUpdateCrowdNoise();
        ClimateUpdateSound();
        EditorOpenWindowsForCurrentStep();

        // Update windows
        // WindowDispatchUpdateAll();

        UpdateEntitiesSpatialIndex();

        // Start autosave timer after update
        if (gLastAutoSaveUpdate == kAutosavePause)
        {
            gLastAutoSaveUpdate = Platform::GetTicks();
        }

        GameActions::ProcessQueue();

        NetworkProcessPending();
        NetworkFlush();

        gameState.CurrentTicks++;

#ifdef ENABLE_SCRIPTING
        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        hookEngine.Call(HOOK_TYPE::INTERVAL_TICK, true);

        if (day != gameState.Date.GetDay())
        {
            hookEngine.Call(HOOK_TYPE::INTERVAL_DAY, true);
        }
#endif

        gInUpdateCode = false;
    }
} // namespace OpenRCT2
