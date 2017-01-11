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
#include "../core/Util.hpp"
#include "GameAction.h"

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

    GameActionResult Query(const IGameAction * action)
    {
        Guard::ArgumentNotNull(action);

        return action->Query();
    }

    void Execute(const IGameAction * action, GameActionCallback callback)
    {
        Guard::ArgumentNotNull(action);

        GameActionResult result = action->Query();
        if (result.Error == GA_ERROR::OK)
        {
            result = action->Execute();
        }
        if (callback != nullptr)
        {
            callback(result);
        }
    }
}
