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

#pragma once

#include "IGameAction.h"

typedef IGameAction *(*GameActionFactory)();
using GameActionCallback = std::function<void(GameActionResult)>;

template<uint32 _TYPE, uint16 _ACTFLAGS>
struct GameAction : public IGameAction
{
public:
    constexpr static uint32 Type = _TYPE;
    constexpr static uint16 ActionFlags = _ACTFLAGS;

private:
    uint32 _playerId;   // Callee
    uint32 _flags;      // GAME_COMMAND_FLAGS

public:
    GameAction() : _playerId(0), _flags(0)
    {
    }

    virtual void SetPlayer(uint32 playerId) override
    {
        _playerId = playerId;
    }

    virtual uint32 GetPlayer() const override
    {
        return _playerId;
    }

    /**
    * Gets the GA_FLAGS flags that are enabled for this game action.
    */
    virtual uint16 GetActionFlags() const override
    {
        return ActionFlags;
    }

    /**
    * Currently used for GAME_COMMAND_FLAGS, needs refactoring once everything is replaced.
    */
    virtual uint32 GetFlags() const override
    {
        return _flags;
    }

    virtual uint32 SetFlags(uint32 flags) override
    {
        return _flags = flags;
    }

    virtual uint32 GetType() const override
    {
        return Type;
    }

    virtual void Serialise(DataSerialiser& stream)
    {
        stream << _flags;
        stream << _playerId;
    }

    /**
    * Query the result of the game action without changing the game state.
    */
    virtual GameActionResult Query() const abstract;

    /**
    * Apply the game action and change the game state.
    */
    virtual GameActionResult Execute() const abstract;
};

namespace GameActions
{
GameActionFactory   Register(uint32 id, GameActionFactory action);
IGameAction *       Create(uint32 id);
GameActionResult    Query(const IGameAction * action);
GameActionResult    Execute(IGameAction * action);

template<typename T>
static GameActionFactory Register()
{
    GameActionFactory factory = []() -> IGameAction *
    {
        return new T();
    };
    Register(T::Type, factory);
    return factory;
}
}
