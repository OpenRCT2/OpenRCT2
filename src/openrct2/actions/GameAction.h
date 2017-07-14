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

template<uint32 _TYPE, uint16 _FLAGS>
struct GameAction : public IGameAction
{
public:
    constexpr static uint32 Type = _TYPE;
    constexpr static uint16 Flags = _FLAGS;

private:
    uint32 _playerId;

public:
    GameAction() : _playerId(0)
    {
    }

    /**
    * Gets the GA_FLAGS flags that are enabled for this game action.
    */
    virtual uint16 GetFlags() const override
    {
        return Flags;
    }

    virtual uint32 GetType() const override
    {
        return Type;
    }

    /**
    * Reads the game action directly from the given stream. Used for
    * sending across the network in multiplayer.
    */
    virtual void Deserialise(IStream * stream)
    {
        stream->Read(&_playerId);
    }

    /**
    * Writes the game action directly to the given stream. Used for
    * sending across the network in multiplayer.
    */
    virtual void Serialise(IStream * stream) const
    {
        stream->Write(&_playerId);
    }

    /**
    * Query the result of the game action without changing the game state.
    */
    virtual GameActionResult Query(uint32 flags = 0) const abstract;

    /**
    * Apply the game action and change the game state.
    */
    virtual GameActionResult Execute(uint32 flags = 0) const abstract;
};

namespace GameActions
{
GameActionFactory   Register(uint32 id, GameActionFactory action);
IGameAction *       Create(uint32 id);
GameActionResult    Query(const IGameAction * action, uint32 flags = 0);
GameActionResult    Execute(const IGameAction * action, uint32 flags = 0, GameActionCallback callback = nullptr);

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