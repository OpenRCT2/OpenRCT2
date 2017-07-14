#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../core/Guard.hpp"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../core/Util.hpp"
#include "../network/network.h"
#include "GameAction.h"

extern "C"
{
    #include "../platform/platform.h"
    #include "../localisation/localisation.h"
    #include "../windows/error.h"
    #include "../world/park.h"
}

GameActionResult::GameActionResult()
{
}

GameActionResult::GameActionResult(GA_ERROR error, rct_string_id message)
{
    Error = error;
    ErrorMessage = message;
}

namespace GameActions
{
    static GameActionFactory _actions[GAME_COMMAND_COUNT];

    GameActionFactory Register(uint32 id, GameActionFactory factory)
    {
        Guard::Assert(id < sizeof(_actions));
        Guard::ArgumentNotNull(factory);

        _actions[id] = factory;
        return factory;
    }

    IGameAction * Create(uint32 id)
    {
        IGameAction * result = nullptr;
        if (id < Util::CountOf(_actions))
        {
            GameActionFactory factory = _actions[id];
            if (factory != nullptr)
            {
                result = factory();
            }
        }
        return result;
    }

    static bool CheckActionInPausedMode(uint32 actionFlags)
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
        if (result->Cost <= DECRYPT_MONEY(gCashEncrypted)) return true;
        return false;
    }

    GameActionResult Query(const IGameAction * action, uint32 flags)
    {
        Guard::ArgumentNotNull(action);

        GameActionResult result;
        uint16 actionFlags = action->GetFlags();
        if (!CheckActionInPausedMode(actionFlags))
        {
            result.Error = GA_ERROR::GAME_PAUSED;
            result.ErrorMessage = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        }
        else
        {
            result = action->Query(flags);
            if (result.Error == GA_ERROR::OK)
            {
                if (!CheckActionAffordability(&result))
                {
                    result.Error = GA_ERROR::INSUFFICIENT_FUNDS;
                    result.ErrorMessage = STR_NOT_ENOUGH_CASH_REQUIRES;
                    set_format_arg_on(result.ErrorMessageArgs, 0, uint32, result.Cost);
                }
            }
        }
        return result;
    }

    GameActionResult Execute(const IGameAction * action, uint32 flags, GameActionCallback callback)
    {
        log_info("[%s] GameAction::Execute\n", network_get_mode() == NETWORK_MODE_CLIENT ? "cl" : "sv");

        Guard::ArgumentNotNull(action);

        uint16 actionFlags = action->GetFlags();
        GameActionResult result = Query(action, flags);
        if (result.Error == GA_ERROR::OK)
        {
            // Networked games send actions to the server to be run
            if (network_get_mode() != NETWORK_MODE_NONE)
            {
                // Action has come from server.
                if (!(actionFlags & GA_FLAGS::CLIENT_ONLY) && !(flags & GAME_COMMAND_FLAG_GHOST) && !(flags & GAME_COMMAND_FLAG_NETWORKED))
                {
                    network_send_game_action(action, flags);
                    if (network_get_mode() == NETWORK_MODE_CLIENT) {
                        // Client sent the command to the server, do not run it locally, just return.  It will run when server sends it
                        //game_command_callback = 0;
                        // Decrement nest count
                        //gGameCommandNestLevel--;
                        return result;
                    }
                }
            }

            // Execute the action, changing the game state
            result = action->Execute(flags);

            // Update money balance
            if (!(gParkFlags & PARK_FLAGS_NO_MONEY) && result.Cost != 0)
            {
                finance_payment(result.Cost, result.ExpenditureType);
                money_effect_create(result.Cost);
            }

            if (!(actionFlags & GA_FLAGS::CLIENT_ONLY))
            {
                if (network_get_mode() == NETWORK_MODE_SERVER)
                {
                    // network_set_player_last_action(network_get_player_index(network_get_current_player_id()), command);
                    // network_add_player_money_spent(network_get_current_player_id(), cost);
                }
            }

            // Allow autosave to commence
            if (gLastAutoSaveUpdate == AUTOSAVE_PAUSE)
            {
                gLastAutoSaveUpdate = platform_get_ticks();
            }
        }

        // Call callback for asynchronous events
        if (callback != nullptr)
        {
            callback(result);
        }

        if (result.Error != GA_ERROR::OK && !(flags & GAME_COMMAND_FLAG_GHOST))
        {
            // Show the error box
            Memory::Copy(gCommonFormatArgs, result.ErrorMessageArgs, sizeof(result.ErrorMessageArgs));
            window_error_open(result.ErrorTitle, result.ErrorMessage);
        }
        return result;
    }
}
