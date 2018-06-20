/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include "../Context.h"
#include "../core/Guard.hpp"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../core/Util.hpp"
#include "../localisation/Localisation.h"
#include "../network/network.h"
#include "../platform/platform.h"
#include "../scenario/Scenario.h"
#include "../world/Park.h"
#include "GameAction.h"

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

GameActionResult::GameActionResult(GA_ERROR error, rct_string_id title, rct_string_id message, uint8_t * args)
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
        Guard::Assert(id < Util::CountOf(_actions));
        Guard::ArgumentNotNull(factory);

        _actions[id] = factory;
        return factory;
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

        GameAction * result = nullptr;
        if (id < Util::CountOf(_actions))
        {
            GameActionFactory factory = _actions[id];
            if (factory != nullptr)
            {
                result = factory();
            }
        }
        Guard::ArgumentNotNull(result, "Attempting to create unregistered gameaction: %u", id);
        return std::unique_ptr<GameAction>(result);
    }

    static bool CheckActionInPausedMode(uint32_t actionFlags)
    {
        if (gGamePaused == 0) return true;
        if (gCheatsBuildInPauseMode) return true;
        if (actionFlags & GA_FLAGS::ALLOW_WHILE_PAUSED) return true;
        return false;
    }

    static bool CheckActionAffordability(const GameActionResult * result)
    {
        if (gParkFlags & PARK_FLAGS_NO_MONEY) return true;
        if (result->Cost <= 0) return true;
        if (result->Cost <= gCash) return true;
        return false;
    }

    GameActionResult::Ptr Query(const GameAction * action)
    {
        Guard::ArgumentNotNull(action);

        uint16_t actionFlags = action->GetActionFlags();
        if (!CheckActionInPausedMode(actionFlags))
        {
            GameActionResult::Ptr result = std::make_unique<GameActionResult>();

            result->Error = GA_ERROR::GAME_PAUSED;
            result->ErrorMessage = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;

            return result;
        }

        auto result = action->Query();

        gCommandPosition.x = result->Position.x;
        gCommandPosition.y = result->Position.y;
        gCommandPosition.z = result->Position.z;

        if (result->Error == GA_ERROR::OK)
        {
            if (!CheckActionAffordability(result.get()))
            {
                result->Error = GA_ERROR::INSUFFICIENT_FUNDS;
                result->ErrorMessage = STR_NOT_ENOUGH_CASH_REQUIRES;
                set_format_arg_on(result->ErrorMessageArgs.data(), 0, uint32_t, result->Cost);
            }
        }
        return result;
    }

    GameActionResult::Ptr Execute(const GameAction * action)
    {
        Guard::ArgumentNotNull(action);

        uint16_t actionFlags = action->GetActionFlags();
        uint32_t flags = action->GetFlags();

        GameActionResult::Ptr result = Query(action);
        if (result->Error == GA_ERROR::OK)
        {
            // Networked games send actions to the server to be run
            if (network_get_mode() == NETWORK_MODE_CLIENT)
            {
                // As a client we have to wait or send it first.
                if (!(actionFlags & GA_FLAGS::CLIENT_ONLY) && !(flags & GAME_COMMAND_FLAG_NETWORKED))
                {
                    log_verbose("[%s] GameAction::Execute\n", "cl");

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
                    log_verbose("[%s] GameAction::Execute\n", "sv-cl");
                    network_enqueue_game_action(action);

                    return result;
                }
            }

            log_verbose("[%s] GameAction::Execute\n", "sv");

            // Execute the action, changing the game state
            result = action->Execute();

            gCommandPosition.x = result->Position.x;
            gCommandPosition.y = result->Position.y;
            gCommandPosition.z = result->Position.z;

            // Update money balance
            if (!(gParkFlags & PARK_FLAGS_NO_MONEY) && 
                !(flags & GAME_COMMAND_FLAG_GHOST) && 
                !(flags & GAME_COMMAND_FLAG_5) &&
                result->Cost != 0)
            {
                finance_payment(result->Cost, result->ExpenditureType);
                money_effect_create(result->Cost);
            }

            if (!(actionFlags & GA_FLAGS::CLIENT_ONLY))
            {
                if (network_get_mode() == NETWORK_MODE_SERVER && result->Error == GA_ERROR::OK)
                {
                    uint32_t playerId = action->GetPlayer();

                    network_set_player_last_action(network_get_player_index(playerId), action->GetType());
                    if (result->Cost != 0)
                    {
                        network_add_player_money_spent(playerId, result->Cost);
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

        if (result->Error != GA_ERROR::OK && 
            !(flags & GAME_COMMAND_FLAG_GHOST) &&
            !(flags & GAME_COMMAND_FLAG_5))
        {
            // Show the error box
            std::copy(result->ErrorMessageArgs.begin(), result->ErrorMessageArgs.end(), gCommonFormatArgs);
            context_show_error(result->ErrorTitle, result->ErrorMessage);
        }
        return result;
    }
} // namespace GameActions
