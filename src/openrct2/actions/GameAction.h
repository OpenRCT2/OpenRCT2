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

extern "C" {
#include "../platform/platform.h"
}

#include <memory>
#include "IGameAction.h"

typedef IGameAction *(*GameActionFactory)();

template<uint32 _TYPE, uint16 _ACTFLAGS>
struct GameAction : public IGameAction
{    
public:
    constexpr static uint32 Type = _TYPE;
    constexpr static uint16 ActionFlags = _ACTFLAGS;

private:
    uint32 _playerId;   // Callee
    uint32 _flags;      // GAME_COMMAND_FLAGS
    uint32 _networkId;
    GameActionCallback_t _callback;

public:
    GameAction() : _playerId(0), _flags(0), _networkId(0)
    {
    }

    virtual void SetPlayer(uint32 playerId) override final
    {
        _playerId = playerId;
    }

    virtual uint32 GetPlayer() const override final
    {
        return _playerId;
    }

    /**
    * Gets the GA_FLAGS flags that are enabled for this game action.
    */
    virtual uint16 GetActionFlags() const override final
    {
        return ActionFlags;
    }

    /**
    * Currently used for GAME_COMMAND_FLAGS, needs refactoring once everything is replaced.
    */
    virtual uint32 GetFlags() const override final
    {
        return _flags;
    }

    virtual uint32 SetFlags(uint32 flags) override final
    {
        return _flags = flags;
    }

    virtual uint32 GetType() const override final
    {
        return Type;
    }

    virtual void SetCallback(const GameActionCallback_t& cb) override final
    {
        _callback = cb;
    }

    virtual const GameActionCallback_t& GetCallback() const override final
    {
        return _callback;
    }

    virtual void SetNetworkId(uint32_t id) override final
    {
        _networkId = id;
    }

    virtual uint32 GetNetworkId() const override final
    {
        return _networkId;
    }

    virtual void Serialise(DataSerialiser& stream) override
    {
        stream << _networkId;
        stream << _flags;
        stream << _playerId;
    }

    /**
    * Query the result of the game action without changing the game state.
    */
    virtual GameActionResult::Ptr Query() const override abstract;

    /**
    * Apply the game action and change the game state.
    */
    virtual GameActionResult::Ptr Execute() const override abstract;
};

namespace GameActions
{
void                Initialize();
void                Register();
IGameAction::Ptr       Create(uint32 id);
GameActionResult::Ptr    Query(const IGameAction * action);
GameActionResult::Ptr    Execute(const IGameAction * action);

GameActionFactory   Register(uint32 id, GameActionFactory action);

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
