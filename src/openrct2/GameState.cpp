/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "drawing/Drawing.h"
#include "entity/EntityTweener.h"
#include "entity/PatrolArea.h"
#include "interface/Screenshot.h"
#include "platform/Platform.h"
#include "profiling/Profiling.h"
#include "ride/Vehicle.h"
#include "scenario/Scenario.h"
#include "scenes/title/TitleScene.h"
#include "scenes/title/TitleSequencePlayer.h"
#include "scripting/ScriptEngine.h"
#include "ui/UiContext.h"
#include "windows/Intent.h"
#include "world/Map.h"
#include "world/MapAnimation.h"
#include "world/Park.h"
#include "world/Scenery.h"

using namespace OpenRCT2::Scripting;

namespace OpenRCT2
{
    static auto _gameState = std::make_unique<GameState_t>();

    GameState_t& getGameState()
    {
        return *_gameState;
    }

    void swapGameState(std::unique_ptr<GameState_t>& otherState)
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
        gameState.currentTicks = 0;

        MapInit(mapSize);
        Park::Initialise(gameState);
        FinanceInit();
        BannerInit(gameState);
        RideInitAll();
        gameState.entities.ResetAllEntities();
        UpdateConsolidatedPatrolAreas();
        ResetDate();
        ClimateReset();
        News::InitQueue(gameState);

        gInMapInitCode = false;

        gameState.nextGuestNumber = 1;

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
            auto player = GetContext()->GetUiContext().GetTitleSequencePlayer();
            if (player != nullptr)
            {
                player->Update();
            }
        }

        Network::Update();

        if (Network::GetMode() == Network::Mode::client && Network::GetStatus() == Network::Status::connected
            && Network::GetAuthstatus() == Network::Auth::ok)
        {
            numUpdates = std::clamp<uint32_t>(Network::GetServerTick() - getGameState().currentTicks, 0, 10);
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
        if (Network::GetMode() == Network::Mode::server && Config::Get().network.pauseServerIfNoClients)
        {
            // If we are headless we always have 1 player (host), pause if no one else is around.
            if (gOpenRCT2Headless && Network::GetNumPlayers() == 1)
            {
                isPaused |= true;
            }
        }

        bool didRunSingleFrame = false;
        if (isPaused)
        {
            if (gDoSingleUpdate && Network::GetMode() == Network::Mode::none)
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

                if (Network::GetMode() == Network::Mode::server)
                {
                    // Make sure the client always knows about what tick the host is on.
                    Network::SendTick();
                }

                // Keep updating the money effect even when paused.
                auto& gameState = getGameState();
                gameState.entities.UpdateMoneyEffect();

                // Post-tick network update
                Network::PostTick();

                // Post-tick game actions.
                GameActions::ProcessQueue(gameState);
                gameState.entities.UpdateEntitiesSpatialIndex();
            }
        }

        // Network has to always tick.
        if (numUpdates == 0)
        {
            Network::Tick();
        }

        // Update the game one or more times
        for (uint32_t i = 0; i < numUpdates; i++)
        {
            gameStateUpdateLogic();
            if (gGameSpeed == 1)
            {
                if (InputGetState() == InputState::Reset || InputGetState() == InputState::Normal)
                {
                    if (gInputFlags.has(InputFlag::viewportScrolling))
                    {
                        gInputFlags.unset(InputFlag::viewportScrolling);
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

        Network::Flush();

        if (!gOpenRCT2Headless)
        {
            gInputFlags.unset(InputFlag::viewportScrolling);
        }

        // Always perform autosave check, even when paused
        if (gLegacyScene != LegacyScene::titleSequence && gLegacyScene != LegacyScene::trackDesigner
            && gLegacyScene != LegacyScene::trackDesignsManager)
        {
            ScenarioAutosaveCheck();
        }

        if (didRunSingleFrame && GameIsNotPaused() && gLegacyScene != LegacyScene::titleSequence)
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
        snapshots->LinkSnapshot(snapshot, getGameState().currentTicks, ScenarioRandState().s0);
    }

    void gameStateUpdateLogic()
    {
        PROFILED_FUNCTION();

        gInUpdateCode = true;

        gScreenAge++;
        if (gScreenAge == 0)
            gScreenAge--;

        GetContext()->GetReplayManager()->Update();

        Network::Tick();

        auto& gameState = getGameState();

        if (Network::GetMode() == Network::Mode::server)
        {
            if (Network::GamestateSnapshotsEnabled())
            {
                gameStateCreateStateSnapshot();
            }

            // Send current tick out.
            Network::SendTick();
        }
        else if (Network::GetMode() == Network::Mode::client)
        {
            // Don't run past the server, this condition can happen during map changes.
            if (Network::GetServerTick() == gameState.currentTicks)
            {
                gInUpdateCode = false;
                return;
            }

            // Check desync.
            bool desynced = Network::CheckDesynchronisation();
            if (desynced)
            {
                // If desync debugging is enabled and we are still connected request the specific game state from server.
                if (Network::GamestateSnapshotsEnabled() && Network::GetStatus() == Network::Status::connected)
                {
                    // Create snapshot from this tick so we can compare it later
                    // as we won't pause the game on this event.
                    gameStateCreateStateSnapshot();

                    Network::RequestGamestateSnapshot();
                }
            }
        }

#ifdef ENABLE_SCRIPTING
        // Stash the current day number before updating the date so that we
        // know if the day number changes on this tick.
        auto day = gameState.date.GetDay();
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
        gameState.entities.UpdateAllMiscEntities();
        Ride::updateAll();

        if (!isInEditorMode())
        {
            auto& park = gameState.park;
            Park::Update(park, gameState);
        }

        ResearchUpdate();
        RideRating::UpdateAll();
        RideMeasurementsUpdate();
        News::UpdateCurrentItem();

        MapAnimations::InvalidateAndUpdateAll();
        VehicleSoundsUpdate();
        PeepUpdateCrowdNoise();
        ClimateUpdateSound();
        EditorOpenWindowsForCurrentStep();

        // Update windows
        // WindowDispatchUpdateAll();

        gameState.entities.UpdateEntitiesSpatialIndex();

        // Start autosave timer after update
        if (gLastAutoSaveUpdate == kAutosavePause)
        {
            gLastAutoSaveUpdate = Platform::GetTicks();
        }

        GameActions::ProcessQueue(gameState);

        Network::PostTick();
        Network::Flush();

        gameState.currentTicks++;

#ifdef ENABLE_SCRIPTING
        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        hookEngine.Call(HookType::intervalTick, true);

        if (day != gameState.date.GetDay())
        {
            hookEngine.Call(HookType::intervalDay, true);
        }
#endif

        gInUpdateCode = false;
    }
} // namespace OpenRCT2
