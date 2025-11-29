/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../ReplayManager.h"
#include "../core/Guard.hpp"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../entity/MoneyEffect.h"
#include "../localisation/Formatter.h"
#include "../network/Network.h"
#include "../platform/Platform.h"
#include "../profiling/Profiling.h"
#include "../scenario/Scenario.h"
#include "../scripting/Duktape.hpp"
#include "../scripting/HookEngine.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/WindowManager.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"

#include <iterator>

namespace OpenRCT2::GameActions
{
    struct QueuedGameAction
    {
        uint32_t tick;
        uint32_t uniqueId;
        GameAction::Ptr action;

        explicit QueuedGameAction(uint32_t t, std::unique_ptr<GameAction>&& ga, uint32_t id)
            : tick(t)
            , uniqueId(id)
            , action(std::move(ga))
        {
        }

        bool operator<(const QueuedGameAction& comp) const
        {
            // First sort by tick
            if (tick < comp.tick)
                return true;
            if (tick > comp.tick)
                return false;

            // If the ticks are equal sort by commandIndex
            return uniqueId < comp.uniqueId;
        }
    };

    static std::multiset<QueuedGameAction> _actionQueue;
    static uint32_t _nextUniqueId = 0;
    static bool _suspended = false;

    void SuspendQueue()
    {
        _suspended = true;
    }

    void ResumeQueue()
    {
        _suspended = false;
    }

    void Enqueue(const GameAction* ga, uint32_t tick)
    {
        auto action = Clone(ga);
        Enqueue(std::move(action), tick);
    }

    void Enqueue(GameAction::Ptr&& ga, uint32_t tick)
    {
        if (ga->GetPlayer() == -1 && Network::GetMode() != Network::Mode::none)
        {
            // Server can directly invoke actions and will have no player id assigned
            // as that normally happens when receiving them over network.
            ga->SetPlayer(Network::GetCurrentPlayerId());
        }
        _actionQueue.emplace(tick, std::move(ga), _nextUniqueId++);
    }

    void ProcessQueue()
    {
        if (_suspended)
        {
            // Do nothing if suspended, this is usually the case between connect and map loads.
            return;
        }

        const uint32_t currentTick = getGameState().currentTicks;

        while (_actionQueue.begin() != _actionQueue.end())
        {
            // run all the game commands at the current tick
            const QueuedGameAction& queued = *_actionQueue.begin();

            if (Network::GetMode() == Network::Mode::client)
            {
                if (queued.tick < currentTick)
                {
                    // This should never happen.
                    Guard::Assert(
                        false,
                        "Discarding game action %s (%u) from tick behind current tick, ID: %08X, Action Tick: %08X, Current "
                        "Tick: "
                        "%08X\n",
                        queued.action->GetName(), queued.action->GetType(), queued.uniqueId, queued.tick, currentTick);
                }
                else if (queued.tick > currentTick)
                {
                    return;
                }
            }

            // Remove ghost scenery so it doesn't interfere with incoming network command
            switch (queued.action->GetType())
            {
                case GameCommand::PlaceWall:
                case GameCommand::PlaceLargeScenery:
                case GameCommand::PlaceBanner:
                case GameCommand::PlaceScenery:
                    SceneryRemoveGhostToolPlacement();
                    break;
                default:
                    break;
            }

            GameAction* action = queued.action.get();
            action->SetFlags(action->GetFlags() | GAME_COMMAND_FLAG_NETWORKED);

            Guard::Assert(action != nullptr);

            Result result = Execute(action, getGameState());
            if (result.Error == Status::Ok && Network::GetMode() == Network::Mode::server)
            {
                // Relay this action to all other clients.
                Network::SendGameAction(action);
            }

            _actionQueue.erase(_actionQueue.begin());
        }
    }

    void ClearQueue()
    {
        _actionQueue.clear();
    }

    GameAction::Ptr Clone(const GameAction* action)
    {
        std::unique_ptr<GameAction> ga = Create(action->GetType());
        ga->SetCallback(action->GetCallback());

        // Serialise action data into stream.
        DataSerialiser dsOut(true);
        action->Serialise(dsOut);

        // Serialise into new action.
        IStream& stream = dsOut.GetStream();
        stream.SetPosition(0);

        DataSerialiser dsIn(false, stream);
        ga->Serialise(dsIn);

        return ga;
    }

    static bool CheckActionInPausedMode(uint32_t actionFlags)
    {
        if (gGamePaused == 0)
            return true;
        if (getGameState().cheats.buildInPauseMode)
            return true;
        if (actionFlags & Flags::AllowWhilePaused)
            return true;
        return false;
    }

    static Result QueryInternal(const GameAction* action, GameState_t& gameState, bool topLevel)
    {
        Guard::ArgumentNotNull(action);

        uint16_t actionFlags = action->GetActionFlags();
        if (topLevel && !CheckActionInPausedMode(actionFlags))
        {
            Result result = Result();

            result.Error = Status::GamePaused;
            result.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            result.ErrorMessage = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;

            return result;
        }

        auto result = action->Query(gameState);

        if (result.Error == Status::Ok)
        {
            if (!FinanceCheckAffordability(result.Cost, action->GetFlags()))
            {
                result.Error = Status::InsufficientFunds;
                result.ErrorTitle = STR_CANT_DO_THIS;
                result.ErrorMessage = STR_NOT_ENOUGH_CASH_REQUIRES;
                Formatter(result.ErrorMessageArgs.data()).Add<uint32_t>(result.Cost);
            }
        }
        return result;
    }

    Result Query(const GameAction* action, GameState_t& gameState)
    {
        return QueryInternal(action, gameState, true);
    }

    Result QueryNested(const GameAction* action, GameState_t& gameState)
    {
        return QueryInternal(action, gameState, false);
    }

    static const char* GetRealm()
    {
        if (Network::GetMode() == Network::Mode::client)
            return "cl";
        if (Network::GetMode() == Network::Mode::server)
            return "sv";
        return "sp";
    }

    struct ActionLogContext
    {
        MemoryStream output;
    };

    static void LogActionBegin(ActionLogContext& ctx, const GameAction* action)
    {
        MemoryStream& output = ctx.output;

        char temp[128] = {};
        snprintf(
            temp, sizeof(temp), "[%s] Tick: %u, GA: %s (%08X) (", GetRealm(), getGameState().currentTicks, action->GetName(),
            EnumValue(action->GetType()));

        output.Write(temp, strlen(temp));

        DataSerialiser ds(true, ctx.output, true); // Logging mode.

        // Write all parameters into output as text.
        action->Serialise(ds);
    }

    static void LogActionFinish(ActionLogContext& ctx, const GameAction* action, const Result& result)
    {
        MemoryStream& output = ctx.output;

        char temp[128] = {};

        if (result.Error != Status::Ok)
        {
            snprintf(temp, sizeof(temp), ") Failed, %u", static_cast<uint32_t>(result.Error));
        }
        else
        {
            snprintf(temp, sizeof(temp), ") OK");
        }

        output.Write(temp, strlen(temp) + 1);

        const char* text = static_cast<const char*>(output.GetData());
        LOG_VERBOSE("%s", text);

        Network::AppendServerLog(text);
    }

    static Result ExecuteInternal(const GameAction* action, GameState_t& gameState, bool topLevel)
    {
        Guard::ArgumentNotNull(action);

        uint16_t actionFlags = action->GetActionFlags();
        uint32_t flags = action->GetFlags();

        // Some actions are not recorded in the replay.
        const auto ignoreForReplays = (actionFlags & Flags::IgnoreForReplays) != 0;

        auto* replayManager = OpenRCT2::GetContext()->GetReplayManager();
        if (replayManager != nullptr && (replayManager->IsReplaying() || replayManager->IsNormalising()))
        {
            // We only accept replay commands as long the replay is active.
            if ((flags & GAME_COMMAND_FLAG_REPLAY) == 0 && !ignoreForReplays)
            {
                // TODO: Introduce proper error.
                auto result = Result();
                result.Error = Status::GamePaused;
                result.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                result.ErrorMessage = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;

                return result;
            }
        }

        Result result = QueryInternal(action, gameState, topLevel);
#ifdef ENABLE_SCRIPTING
        if (result.Error == Status::Ok
            && ((Network::GetMode() == Network::Mode::none) || (flags & GAME_COMMAND_FLAG_NETWORKED)))
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.RunGameActionHooks(*action, result, false);
            // Script hooks may now have changed the game action result...
        }
#endif
        if (result.Error  ==  Status::Ok)
        {
            if (topLevel)
            {
                // Networked games send actions to the server to be run
                if (Network::GetMode() == Network::Mode::client)
                {
                    // As a client we have to wait or send it first.
                    if (!(actionFlags & Flags::ClientOnly) && !(flags & GAME_COMMAND_FLAG_NETWORKED))
                    {
                        LOG_VERBOSE("[%s] GameAction::Execute %s (Out)", GetRealm(), action->GetName());
                        Network::SendGameAction(action);

                        return result;
                    }
                }
                else if (Network::GetMode() == Network::Mode::server || !gInUpdateCode)
                {
                    // If player is the server it would execute right away as where clients execute the commands
                    // at the beginning of the frame, so we have to put them into the queue.
                    // This is also the case when its executed from the UI update.
                    if (!(actionFlags & Flags::ClientOnly) && !(flags & GAME_COMMAND_FLAG_NETWORKED))
                    {
                        LOG_VERBOSE("[%s] GameAction::Execute %s (Queue)", GetRealm(), action->GetName());
                        Enqueue(action, getGameState().currentTicks);

                        return result;
                    }
                }
            }

            ActionLogContext logContext;
            LogActionBegin(logContext, action);

            // Execute the action, changing the game state
            result = action->Execute(gameState);
#ifdef ENABLE_SCRIPTING
            if (result.Error == Status::Ok)
            {
                auto& scriptEngine = GetContext()->GetScriptEngine();
                scriptEngine.RunGameActionHooks(*action, result, true);
                // Script hooks may now have changed the game action result...
            }
#endif

            LogActionFinish(logContext, action, result);

            // If not top level just give away the result.
            if (!topLevel)
                return result;

            // Update money balance
            if (result.Error == Status::Ok && FinanceCheckMoneyRequired(flags) && result.Cost != 0)
            {
                FinancePayment(result.Cost, result.Expenditure);
                MoneyEffect::Create(result.Cost, result.Position);
            }

            if (!(actionFlags & Flags::ClientOnly) && result.Error == Status::Ok)
            {
                if (Network::GetMode() != Network::Mode::none)
                {
                    Network::PlayerId_t playerId = action->GetPlayer();

                    int32_t playerIndex = Network::GetPlayerIndex(playerId.id);
                    Guard::Assert(
                        playerIndex != -1, "Unable to find player %u for game action %u", playerId, action->GetType());

                    Network::SetPlayerLastAction(playerIndex, action->GetType());
                    Network::IncrementPlayerNumCommands(playerIndex);
                    if (result.Cost > 0)
                    {
                        Network::AddPlayerMoneySpent(playerIndex, result.Cost);
                    }

                    if (!result.Position.IsNull())
                    {
                        Network::SetPlayerLastActionCoord(playerIndex, result.Position);
                    }
                }
                else
                {
                    bool commandExecutes = (flags & GAME_COMMAND_FLAG_GHOST) == 0 && (flags & GAME_COMMAND_FLAG_NO_SPEND) == 0;

                    bool recordAction = false;
                    if (replayManager != nullptr && !ignoreForReplays)
                    {
                        if (replayManager->IsRecording() && commandExecutes)
                            recordAction = true;
                        else if (replayManager->IsNormalising() && (flags & GAME_COMMAND_FLAG_REPLAY) != 0)
                            recordAction = true; // In normalisation we only feed back actions issued by the replay manager.
                    }
                    if (recordAction)
                    {
                        replayManager->AddGameAction(getGameState().currentTicks, action);
                    }
                }
            }

            // Allow autosave to commence
            if (gLastAutoSaveUpdate == kAutosavePause)
            {
                gLastAutoSaveUpdate = Platform::GetTicks();
            }
        }

        // Call callback for asynchronous events
        auto cb = action->GetCallback();
        if (cb != nullptr)
        {
            cb(action, &result);
        }

        // Only show errors when its not a ghost and not a preview and also top level action.
        bool shouldShowError = !(flags & GAME_COMMAND_FLAG_GHOST) && !(flags & GAME_COMMAND_FLAG_NO_SPEND) && topLevel;

        // In network mode the error should be only shown to the issuer of the action.
        if (Network::GetMode() != Network::Mode::none)
        {
            // If the action was never networked and query fails locally the player id is not assigned.
            // So compare only if the action went into the queue otherwise show errors by default.
            const bool isActionFromNetwork = (action->GetFlags() & GAME_COMMAND_FLAG_NETWORKED) != 0;
            if (isActionFromNetwork && action->GetPlayer() != Network::GetCurrentPlayerId())
            {
                shouldShowError = false;
            }
        }

        if (result.Error != Status::Ok && shouldShowError)
        {
            auto windowManager = Ui::GetWindowManager();
            windowManager->ShowError(result.GetErrorTitle(), result.GetErrorMessage());
        }

        return result;
    }

    Result Execute(const GameAction* action, GameState_t& gameState)
    {
        return ExecuteInternal(action, gameState, true);
    }

    Result ExecuteNested(const GameAction* action, GameState_t& gameState)
    {
        return ExecuteInternal(action, gameState, false);
    }

    const char* GameAction::GetName() const
    {
        return OpenRCT2::GameActions::GetName(_type);
    }

    bool GameAction::LocationValid(const CoordsXY& coords) const
    {
        auto result = MapIsLocationValid(coords);
        if (!result)
            return false;
#ifdef ENABLE_SCRIPTING
        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        if (hookEngine.HasSubscriptions(Scripting::HookType::actionLocation))
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();

            // Create event args object
            auto obj = Scripting::DukObject(ctx);
            obj.Set("x", coords.x);
            obj.Set("y", coords.y);
            obj.Set("player", _playerId);
            obj.Set("type", EnumValue(_type));

            auto flags = GetActionFlags();
            obj.Set("isClientOnly", (flags & GameActions::Flags::ClientOnly) != 0);
            obj.Set("result", true);

            // Call the subscriptions
            auto e = obj.Take();
            hookEngine.Call(Scripting::HookType::actionLocation, e, true);

            auto scriptResult = Scripting::AsOrDefault(e["result"], true);

            return scriptResult;
        }
#endif
        return true;
    }
} // namespace OpenRCT2::GameActions
