/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

    void ProcessQueue(GameState_t& gameState)
    {
        if (_suspended)
        {
            // Do nothing if suspended, this is usually the case between connect and map loads.
            return;
        }

        const uint32_t currentTick = gameState.currentTicks;

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
            action->SetFlags(action->GetFlags().with(CommandFlag::networked));

            Guard::Assert(action != nullptr);

            Result result = Execute(action, gameState);
            if (result.error == Status::ok && Network::GetMode() == Network::Mode::server)
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

    static bool CheckActionInPausedMode(const GameState_t& gameState, uint32_t actionFlags)
    {
        if (gGamePaused == 0)
            return true;
        if (gameState.cheats.buildInPauseMode)
            return true;
        if (actionFlags & Flags::AllowWhilePaused)
            return true;
        return false;
    }

    static Result QueryInternal(const GameAction* action, GameState_t& gameState, bool topLevel)
    {
        Guard::ArgumentNotNull(action);

        uint16_t actionFlags = action->GetActionFlags();
        if (topLevel && !CheckActionInPausedMode(gameState, actionFlags))
        {
            Result result = Result();

            result.error = Status::gamePaused;
            result.errorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            result.errorMessage = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;

            return result;
        }

        auto result = action->Query(gameState);

        if (result.error == Status::ok)
        {
            if (!FinanceCheckAffordability(result.cost, action->GetFlags()))
            {
                result.error = Status::insufficientFunds;
                result.errorTitle = STR_CANT_DO_THIS;
                result.errorMessage = STR_NOT_ENOUGH_CASH_REQUIRES;
                Formatter(result.errorMessageArgs.data()).Add<uint32_t>(result.cost);
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

    static void LogActionBegin(const GameState_t& gameState, ActionLogContext& ctx, const GameAction* action)
    {
        MemoryStream& output = ctx.output;

        char temp[128] = {};
        snprintf(
            temp, sizeof(temp), "[%s] Tick: %u, GA: %s (%08X) (", GetRealm(), gameState.currentTicks, action->GetName(),
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

        if (result.error != Status::ok)
        {
            snprintf(temp, sizeof(temp), ") Failed, %u", static_cast<uint32_t>(result.error));
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
        auto flags = action->GetFlags();

        // Some actions are not recorded in the replay.
        const auto ignoreForReplays = (actionFlags & Flags::IgnoreForReplays) != 0;

        auto* replayManager = GetContext()->GetReplayManager();
        if (replayManager != nullptr && (replayManager->IsReplaying() || replayManager->IsNormalising()))
        {
            // We only accept replay commands as long the replay is active.
            if (!flags.has(CommandFlag::replay) && !ignoreForReplays)
            {
                // TODO: Introduce proper error.
                auto result = Result();
                result.error = Status::gamePaused;
                result.errorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                result.errorMessage = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;

                return result;
            }
        }

        Result result = QueryInternal(action, gameState, topLevel);
#ifdef ENABLE_SCRIPTING
        if (result.error == Status::ok && ((Network::GetMode() == Network::Mode::none) || flags.has(CommandFlag::networked)))
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.RunGameActionHooks(*action, result, false);
            // Script hooks may now have changed the game action result...
        }
#endif
        if (result.error == Status::ok)
        {
            if (topLevel)
            {
                // Networked games send actions to the server to be run
                if (Network::GetMode() == Network::Mode::client)
                {
                    // As a client we have to wait or send it first.
                    if (!(actionFlags & Flags::ClientOnly) && !flags.has(CommandFlag::networked))
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
                    if (!(actionFlags & Flags::ClientOnly) && !flags.has(CommandFlag::networked))
                    {
                        LOG_VERBOSE("[%s] GameAction::Execute %s (Queue)", GetRealm(), action->GetName());
                        Enqueue(action, gameState.currentTicks);

                        return result;
                    }
                }
            }

            ActionLogContext logContext;
            if (topLevel && !flags.has(CommandFlag::ghost))
            {
                LogActionBegin(gameState, logContext, action);
            }

            // Execute the action, changing the game state
            result = action->Execute(gameState);
#ifdef ENABLE_SCRIPTING
            if (result.error == Status::ok)
            {
                auto& scriptEngine = GetContext()->GetScriptEngine();
                scriptEngine.RunGameActionHooks(*action, result, true);
                // Script hooks may now have changed the game action result...
            }
#endif
            if (topLevel && !flags.has(CommandFlag::ghost))
            {
                LogActionFinish(logContext, action, result);
            }

            // If not top level just give away the result.
            if (!topLevel)
                return result;

            // Update money balance
            if (result.error == Status::ok && FinanceCheckMoneyRequired(flags) && result.cost != 0)
            {
                FinancePayment(result.cost, result.expenditure);
                MoneyEffect::Create(result.cost, result.position);
            }

            if (!(actionFlags & Flags::ClientOnly) && result.error == Status::ok)
            {
                if (Network::GetMode() != Network::Mode::none)
                {
                    Network::PlayerId_t playerId = action->GetPlayer();

                    int32_t playerIndex = Network::GetPlayerIndex(playerId.id);
                    Guard::Assert(
                        playerIndex != -1, "Unable to find player %u for game action %u", playerId, action->GetType());

                    Network::SetPlayerLastAction(playerIndex, action->GetType());
                    Network::IncrementPlayerNumCommands(playerIndex);
                    if (result.cost > 0)
                    {
                        Network::AddPlayerMoneySpent(playerIndex, result.cost);
                    }

                    if (!result.position.IsNull())
                    {
                        Network::SetPlayerLastActionCoord(playerIndex, result.position);
                    }
                }
                else
                {
                    bool commandExecutes = !flags.hasAny(CommandFlag::ghost, CommandFlag::noSpend);

                    bool recordAction = false;
                    if (replayManager != nullptr && !ignoreForReplays)
                    {
                        if (replayManager->IsRecording() && commandExecutes)
                            recordAction = true;
                        else if (replayManager->IsNormalising() && flags.has(CommandFlag::replay))
                            recordAction = true; // In normalisation we only feed back actions issued by the replay manager.
                    }
                    if (recordAction)
                    {
                        replayManager->AddGameAction(gameState.currentTicks, action);
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
        bool shouldShowError = !flags.has(CommandFlag::ghost) && !flags.has(CommandFlag::noSpend) && topLevel;

        // In network mode the error should be only shown to the issuer of the action.
        if (Network::GetMode() != Network::Mode::none)
        {
            // If the action was never networked and query fails locally the player id is not assigned.
            // So compare only if the action went into the queue otherwise show errors by default.
            const bool isActionFromNetwork = action->GetFlags().has(CommandFlag::networked);
            if (isActionFromNetwork && action->GetPlayer() != Network::GetCurrentPlayerId())
            {
                shouldShowError = false;
            }
        }

        if (result.error != Status::ok && shouldShowError)
        {
            auto windowManager = Ui::GetWindowManager();
            windowManager->ShowError(result.getErrorTitle(), result.getErrorMessage());
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
        return GameActions::GetName(_type);
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
            obj.Set("isClientOnly", (flags & Flags::ClientOnly) != 0);
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
