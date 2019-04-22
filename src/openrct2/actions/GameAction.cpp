/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
#include "../localisation/Localisation.h"
#include "../network/network.h"
#include "../platform/platform.h"
#include "../scenario/Scenario.h"
#include "../world/Park.h"

#include <algorithm>
#include <iterator>

GameActionResult::GameActionResult(GA_ERROR error, rct_string_id message)
{
    Error = error;
    ErrorMessage = message;
}

GameActionResult::GameActionResult(GA_ERROR error, rct_string_id title, rct_string_id message)
{
    Error = error;
    ErrorTitle = title;
    ErrorMessage = message;
}

GameActionResult::GameActionResult(GA_ERROR error, rct_string_id title, rct_string_id message, uint8_t* args)
{
    Error = error;
    ErrorTitle = title;
    ErrorMessage = message;
    std::copy_n(args, ErrorMessageArgs.size(), ErrorMessageArgs.begin());
}

namespace GameActions
{
    static GameActionFactory _actions[GAME_COMMAND_COUNT];

    GameActionFactory Register(uint32_t id, GameActionFactory factory)
    {
        Guard::Assert(id < std::size(_actions));
        Guard::ArgumentNotNull(factory);

        _actions[id] = factory;
        return factory;
    }

    bool IsValidId(uint32_t id)
    {
        if (id < std::size(_actions))
        {
            return _actions[id] != nullptr;
        }
        return false;
    }

    void Initialize()
    {
        static bool initialized = false;
        if (initialized)
            return;

        Register();

        initialized = true;
    }

    std::unique_ptr<GameAction> Create(uint32_t id)
    {
        Initialize();

        GameAction* result = nullptr;
        if (id < std::size(_actions))
        {
            GameActionFactory factory = _actions[id];
            if (factory != nullptr)
            {
                result = factory();
            }
        }
#ifdef _DEBUG
        Guard::ArgumentNotNull(result, "Attempting to create unregistered gameaction: %u", id);
#endif
        return std::unique_ptr<GameAction>(result);
    }

    GameAction::Ptr Clone(const GameAction* action)
    {
        std::unique_ptr<GameAction> ga = GameActions::Create(action->GetType());
        ga->SetCallback(action->GetCallback());

        // Serialise action data into stream.
        DataSerialiser dsOut(true);
        action->Serialise(dsOut);

        // Serialise into new action.
        MemoryStream& stream = dsOut.GetStream();
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
        if (actionFlags & GA_FLAGS::ALLOW_WHILE_PAUSED)
            return true;
        return false;
    }

    static GameActionResult::Ptr QueryInternal(const GameAction* action, bool topLevel)
    {
        Guard::ArgumentNotNull(action);

        uint16_t actionFlags = action->GetActionFlags();
        if (topLevel && !CheckActionInPausedMode(actionFlags))
        {
            GameActionResult::Ptr result = std::make_unique<GameActionResult>();

            result->Error = GA_ERROR::GAME_PAUSED;
            result->ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            result->ErrorMessage = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;

            return result;
        }

        auto result = action->Query();

        // Only top level actions affect the command position.
        if (topLevel)
        {
            gCommandPosition.x = result->Position.x;
            gCommandPosition.y = result->Position.y;
            gCommandPosition.z = result->Position.z;
        }

        if (result->Error == GA_ERROR::OK)
        {
            if (finance_check_affordability(result->Cost, action->GetFlags()) == false)
            {
                result->Error = GA_ERROR::INSUFFICIENT_FUNDS;
                result->ErrorMessage = STR_NOT_ENOUGH_CASH_REQUIRES;
                set_format_arg_on(result->ErrorMessageArgs.data(), 0, uint32_t, result->Cost);
            }
        }
        return result;
    }

    GameActionResult::Ptr Query(const GameAction* action)
    {
        return QueryInternal(action, true);
    }

    GameActionResult::Ptr QueryNested(const GameAction* action)
    {
        return QueryInternal(action, false);
    }

    static const char* GetRealm()
    {
        if (network_get_mode() == NETWORK_MODE_CLIENT)
            return "cl";
        else if (network_get_mode() == NETWORK_MODE_SERVER)
            return "sv";
        return "sp";
    }

    struct ActionLogContext_t
    {
        MemoryStream output;
    };

    static void LogActionBegin(ActionLogContext_t& ctx, const GameAction* action)
    {
        MemoryStream& output = ctx.output;

        char temp[128] = {};
        snprintf(temp, sizeof(temp), "[%s] GA: %s (%08X) (", GetRealm(), action->GetName(), action->GetType());

        output.Write(temp, strlen(temp));

        DataSerialiser ds(true, ctx.output, true); // Logging mode.

        // Write all parameters into output as text.
        action->Serialise(ds);
    }

    static void LogActionFinish(ActionLogContext_t& ctx, const GameAction* action, const GameActionResult::Ptr& result)
    {
        MemoryStream& output = ctx.output;

        char temp[128] = {};

        if (result->Error != GA_ERROR::OK)
        {
            snprintf(temp, sizeof(temp), ") Failed, %u", (uint32_t)result->Error);
        }
        else
        {
            snprintf(temp, sizeof(temp), ") OK");
        }

        output.Write(temp, strlen(temp) + 1);

        const char* text = (const char*)output.GetData();
        log_verbose("%s", text);

        network_append_server_log(text);
    }

    static GameActionResult::Ptr ExecuteInternal(const GameAction* action, bool topLevel)
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
                GameActionResult::Ptr result = std::make_unique<GameActionResult>();

                result->Error = GA_ERROR::GAME_PAUSED;
                result->ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                result->ErrorMessage = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;

                return result;
            }
        }

        GameActionResult::Ptr result = QueryInternal(action, topLevel);
        if (result->Error == GA_ERROR::OK)
        {
            if (topLevel)
            {
                // Networked games send actions to the server to be run
                if (network_get_mode() == NETWORK_MODE_CLIENT)
                {
                    // As a client we have to wait or send it first.
                    if (!(actionFlags & GA_FLAGS::CLIENT_ONLY) && !(flags & GAME_COMMAND_FLAG_NETWORKED))
                    {
                        log_verbose("[%s] GameAction::Execute %s (Out)", GetRealm(), action->GetName());
                        network_send_game_action(action);

                        return result;
                    }
                }
                else if (network_get_mode() == NETWORK_MODE_SERVER)
                {
                    // If player is the server it would execute right away as where clients execute the commands
                    // at the beginning of the frame, so we have to put them into the queue.
                    if (!(actionFlags & GA_FLAGS::CLIENT_ONLY) && !(flags & GAME_COMMAND_FLAG_NETWORKED))
                    {
                        log_verbose("[%s] GameAction::Execute %s (Queue)", GetRealm(), action->GetName());
                        network_enqueue_game_action(action);

                        return result;
                    }
                }
            }

            ActionLogContext_t logContext;
            LogActionBegin(logContext, action);

            // Execute the action, changing the game state
            result = action->Execute();

            LogActionFinish(logContext, action, result);

            // If not top level just give away the result.
            if (topLevel == false)
                return result;

            gCommandPosition.x = result->Position.x;
            gCommandPosition.y = result->Position.y;
            gCommandPosition.z = result->Position.z;

            // Update money balance
            if (result->Error == GA_ERROR::OK && finance_check_money_required(flags) && result->Cost != 0)
            {
                finance_payment(result->Cost, result->ExpenditureType);
                money_effect_create(result->Cost);
            }

            if (!(actionFlags & GA_FLAGS::CLIENT_ONLY) && result->Error == GA_ERROR::OK)
            {
                if (network_get_mode() == NETWORK_MODE_SERVER)
                {
                    NetworkPlayerId_t playerId = action->GetPlayer();

                    int32_t playerIndex = network_get_player_index(playerId.id);
                    Guard::Assert(playerIndex != -1);

                    network_set_player_last_action(playerIndex, action->GetType());
                    if (result->Cost != 0)
                    {
                        network_add_player_money_spent(playerIndex, result->Cost);
                    }

                    if (result->Position.x != LOCATION_NULL)
                    {
                        network_set_player_last_action_coord(playerId, gCommandPosition);
                    }
                }
                else if (network_get_mode() == NETWORK_MODE_NONE)
                {
                    bool commandExecutes = (flags & GAME_COMMAND_FLAG_GHOST) == 0 && (flags & GAME_COMMAND_FLAG_NO_SPEND) == 0;

                    bool recordAction = false;
                    if (replayManager)
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
                gLastAutoSaveUpdate = platform_get_ticks();
            }
        }

        // Call callback for asynchronous events
        auto cb = action->GetCallback();
        if (cb != nullptr)
        {
            cb(action, result.get());
        }

        // Only show errors when its not a ghost and not a preview and also top level action.
        bool shouldShowError = !(flags & GAME_COMMAND_FLAG_GHOST) && !(flags & GAME_COMMAND_FLAG_NO_SPEND) && topLevel;

        // In network mode the error should be only shown to the issuer of the action.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (action->GetPlayer() != network_get_current_player_id())
            {
                shouldShowError = false;
            }
        }

        if (result->Error != GA_ERROR::OK && shouldShowError)
        {
            // Show the error box
            std::copy(result->ErrorMessageArgs.begin(), result->ErrorMessageArgs.end(), gCommonFormatArgs);
            context_show_error(result->ErrorTitle, result->ErrorMessage);
        }

        return result;
    }

    GameActionResult::Ptr Execute(const GameAction* action)
    {
        return ExecuteInternal(action, true);
    }

    GameActionResult::Ptr ExecuteNested(const GameAction* action)
    {
        return ExecuteInternal(action, false);
    }

} // namespace GameActions
