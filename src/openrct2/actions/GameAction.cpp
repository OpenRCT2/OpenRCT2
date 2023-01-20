/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameAction.h"

#include "../Context.h"
#include "../ReplayManager.h"
#include "../core/Guard.hpp"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../entity/MoneyEffect.h"
#include "../localisation/Formatter.h"
#include "../localisation/Localisation.h"
#include "../network/network.h"
#include "../platform/Platform.h"
#include "../profiling/Profiling.h"
#include "../scenario/Scenario.h"
#include "../scripting/Duktape.hpp"
#include "../scripting/HookEngine.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"
#include "../world/Scenery.h"

#include <algorithm>
#include <iterator>

using namespace OpenRCT2;

namespace GameActions
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
        if (ga->GetPlayer() == -1 && NetworkGetMode() != NETWORK_MODE_NONE)
        {
            // Server can directly invoke actions and will have no player id assigned
            // as that normally happens when receiving them over network.
            ga->SetPlayer(NetworkGetCurrentPlayerId());
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

        const uint32_t currentTick = gCurrentTicks;

        while (_actionQueue.begin() != _actionQueue.end())
        {
            // run all the game commands at the current tick
            const QueuedGameAction& queued = *_actionQueue.begin();

            if (NetworkGetMode() == NETWORK_MODE_CLIENT)
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

            GameActions::Result result = Execute(action);
            if (result.Error == GameActions::Status::Ok && NetworkGetMode() == NETWORK_MODE_SERVER)
            {
                // Relay this action to all other clients.
                NetworkSendGameAction(action);
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
        std::unique_ptr<GameAction> ga = GameActions::Create(action->GetType());
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
        if (gCheatsBuildInPauseMode)
            return true;
        if (actionFlags & GameActions::Flags::AllowWhilePaused)
            return true;
        return false;
    }

    static GameActions::Result QueryInternal(const GameAction* action, bool topLevel)
    {
        Guard::ArgumentNotNull(action);

        uint16_t actionFlags = action->GetActionFlags();
        if (topLevel && !CheckActionInPausedMode(actionFlags))
        {
            GameActions::Result result = GameActions::Result();

            result.Error = GameActions::Status::GamePaused;
            result.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            result.ErrorMessage = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;

            return result;
        }

        auto result = action->Query();

        if (result.Error == GameActions::Status::Ok)
        {
            if (!FinanceCheckAffordability(result.Cost, action->GetFlags()))
            {
                result.Error = GameActions::Status::InsufficientFunds;
                result.ErrorTitle = STR_CANT_DO_THIS;
                result.ErrorMessage = STR_NOT_ENOUGH_CASH_REQUIRES;
                Formatter(result.ErrorMessageArgs.data()).Add<uint32_t>(result.Cost);
            }
        }
        return result;
    }

    GameActions::Result Query(const GameAction* action)
    {
        return QueryInternal(action, true);
    }

    GameActions::Result QueryNested(const GameAction* action)
    {
        return QueryInternal(action, false);
    }

    static const char* GetRealm()
    {
        if (NetworkGetMode() == NETWORK_MODE_CLIENT)
            return "cl";
        if (NetworkGetMode() == NETWORK_MODE_SERVER)
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
            temp, sizeof(temp), "[%s] Tick: %u, GA: %s (%08X) (", GetRealm(), gCurrentTicks, action->GetName(),
            EnumValue(action->GetType()));

        output.Write(temp, strlen(temp));

        DataSerialiser ds(true, ctx.output, true); // Logging mode.

        // Write all parameters into output as text.
        action->Serialise(ds);
    }

    static void LogActionFinish(ActionLogContext& ctx, const GameAction* action, const GameActions::Result& result)
    {
        MemoryStream& output = ctx.output;

        char temp[128] = {};

        if (result.Error != GameActions::Status::Ok)
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

        NetworkAppendServerLog(text);
    }

    static GameActions::Result ExecuteInternal(const GameAction* action, bool topLevel)
    {
        Guard::ArgumentNotNull(action);

        uint16_t actionFlags = action->GetActionFlags();
        uint32_t flags = action->GetFlags();

        auto* replayManager = OpenRCT2::GetContext()->GetReplayManager();
        if (replayManager != nullptr && (replayManager->IsReplaying() || replayManager->IsNormalising()))
        {
            // We only accept replay commands as long the replay is active.
            if ((flags & GAME_COMMAND_FLAG_REPLAY) == 0)
            {
                // TODO: Introduce proper error.
                auto result = GameActions::Result();
                result.Error = GameActions::Status::GamePaused;
                result.ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                result.ErrorMessage = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;

                return result;
            }
        }

        GameActions::Result result = QueryInternal(action, topLevel);
#ifdef ENABLE_SCRIPTING
        if (result.Error == GameActions::Status::Ok
            && ((NetworkGetMode() == NETWORK_MODE_NONE) || (flags & GAME_COMMAND_FLAG_NETWORKED)))
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.RunGameActionHooks(*action, result, false);
            // Script hooks may now have changed the game action result...
        }
#endif
        if (result.Error == GameActions::Status::Ok)
        {
            if (topLevel)
            {
                // Networked games send actions to the server to be run
                if (NetworkGetMode() == NETWORK_MODE_CLIENT)
                {
                    // As a client we have to wait or send it first.
                    if (!(actionFlags & GameActions::Flags::ClientOnly) && !(flags & GAME_COMMAND_FLAG_NETWORKED))
                    {
                        LOG_VERBOSE("[%s] GameAction::Execute %s (Out)", GetRealm(), action->GetName());
                        NetworkSendGameAction(action);

                        return result;
                    }
                }
                else if (NetworkGetMode() == NETWORK_MODE_SERVER)
                {
                    // If player is the server it would execute right away as where clients execute the commands
                    // at the beginning of the frame, so we have to put them into the queue.
                    if (!(actionFlags & GameActions::Flags::ClientOnly) && !(flags & GAME_COMMAND_FLAG_NETWORKED))
                    {
                        LOG_VERBOSE("[%s] GameAction::Execute %s (Queue)", GetRealm(), action->GetName());
                        Enqueue(action, gCurrentTicks);

                        return result;
                    }
                }
            }

            ActionLogContext logContext;
            LogActionBegin(logContext, action);

            // Execute the action, changing the game state
            result = action->Execute();
#ifdef ENABLE_SCRIPTING
            if (result.Error == GameActions::Status::Ok)
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
            if (result.Error == GameActions::Status::Ok && FinanceCheckMoneyRequired(flags) && result.Cost != 0)
            {
                FinancePayment(result.Cost, result.Expenditure);
                MoneyEffect::Create(result.Cost, result.Position);
            }

            if (!(actionFlags & GameActions::Flags::ClientOnly) && result.Error == GameActions::Status::Ok)
            {
                if (NetworkGetMode() != NETWORK_MODE_NONE)
                {
                    NetworkPlayerId_t playerId = action->GetPlayer();

                    int32_t playerIndex = NetworkGetPlayerIndex(playerId.id);
                    Guard::Assert(
                        playerIndex != -1, "Unable to find player %u for game action %u", playerId, action->GetType());

                    NetworkSetPlayerLastAction(playerIndex, action->GetType());
                    if (result.Cost != 0)
                    {
                        NetworkAddPlayerMoneySpent(playerIndex, result.Cost);
                    }

                    if (!result.Position.IsNull())
                    {
                        NetworkSetPlayerLastActionCoord(playerIndex, result.Position);
                    }
                }
                else
                {
                    bool commandExecutes = (flags & GAME_COMMAND_FLAG_GHOST) == 0 && (flags & GAME_COMMAND_FLAG_NO_SPEND) == 0;

                    bool recordAction = false;
                    if (replayManager != nullptr)
                    {
                        if (replayManager->IsRecording() && commandExecutes)
                            recordAction = true;
                        else if (replayManager->IsNormalising() && (flags & GAME_COMMAND_FLAG_REPLAY) != 0)
                            recordAction = true; // In normalisation we only feed back actions issued by the replay manager.
                    }
                    if (recordAction)
                    {
                        replayManager->AddGameAction(gCurrentTicks, action);
                    }
                }
            }

            // Allow autosave to commence
            if (gLastAutoSaveUpdate == AUTOSAVE_PAUSE)
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
        if (NetworkGetMode() != NETWORK_MODE_NONE)
        {
            // If the action was never networked and query fails locally the player id is not assigned.
            // So compare only if the action went into the queue otherwise show errors by default.
            const bool isActionFromNetwork = (action->GetFlags() & GAME_COMMAND_FLAG_NETWORKED) != 0;
            if (isActionFromNetwork && action->GetPlayer() != NetworkGetCurrentPlayerId())
            {
                shouldShowError = false;
            }
        }

        if (result.Error != GameActions::Status::Ok && shouldShowError)
        {
            auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
            windowManager->ShowError(result.GetErrorTitle(), result.GetErrorMessage());
        }

        return result;
    }

    GameActions::Result Execute(const GameAction* action)
    {
        return ExecuteInternal(action, true);
    }

    GameActions::Result ExecuteNested(const GameAction* action)
    {
        return ExecuteInternal(action, false);
    }
} // namespace GameActions

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
    if (hookEngine.HasSubscriptions(OpenRCT2::Scripting::HOOK_TYPE::ACTION_LOCATION))
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        // Create event args object
        auto obj = OpenRCT2::Scripting::DukObject(ctx);
        obj.Set("x", coords.x);
        obj.Set("y", coords.y);
        obj.Set("player", _playerId);
        obj.Set("type", EnumValue(_type));

        auto flags = GetActionFlags();
        obj.Set("isClientOnly", (flags & GameActions::Flags::ClientOnly) != 0);
        obj.Set("result", true);

        // Call the subscriptions
        auto e = obj.Take();
        hookEngine.Call(OpenRCT2::Scripting::HOOK_TYPE::ACTION_LOCATION, e, true);

        auto scriptResult = OpenRCT2::Scripting::AsOrDefault(e["result"], true);

        return scriptResult;
    }
#endif
    return true;
}
