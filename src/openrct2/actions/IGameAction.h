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

#include <functional>
#include "../common.h"
#include "../core/IStream.hpp"

extern "C"
{
#include "../game.h"
#include "../world/map.h"
}

/**
* Common error codes for game actions.
*/
enum class GA_ERROR : uint16
{
    OK,
    INVALID_PARAMETERS,
    DISALLOWED,
    GAME_PAUSED,
    INSUFFICIENT_FUNDS,
    NOT_IN_EDITOR_MODE,

    NOT_OWNED,
    TOO_LOW,
    TOO_HIGH,
    NO_CLEARANCE,
    ITEM_ALREADY_PLACED,

    NO_FREE_ELEMENTS,

    UNKNOWN = UINT16_MAX,
};

namespace GA_FLAGS
{
constexpr uint16 ALLOW_WHILE_PAUSED = 1 << 0;
constexpr uint16 CLIENT_ONLY = 1 << 1;
constexpr uint16 EDITOR_ONLY = 1 << 2;
}

/**
* Represents the result of a game action query or execution.
*/
struct GameActionResult
{
    GA_ERROR        Error = GA_ERROR::OK;
    rct_string_id   ErrorTitle = (rct_string_id)-1;
    rct_string_id   ErrorMessage = (rct_string_id)-1;
    uint8           ErrorMessageArgs[8] = { 0 };
    rct_xyz32       Position = { 0 };
    money32         Cost = 0;
    uint16          ExpenditureType = 0;
    void *          Tag = nullptr;

    GameActionResult();
    GameActionResult(GA_ERROR error, rct_string_id message);
};

/**
* Represents an action that changes the state of the game. Can be serialised and
* deserialised into a stream.
*/
struct IGameAction
{
public:
    /**
    * Gets the GA_FLAGS flags that are enabled for this game action.
    */
    virtual uint16 GetFlags() const abstract;

    virtual uint32 GetType() const abstract;
    /**
    * Reads the game action directly from the given stream. Used for
    * sending across the network in multiplayer.
    */
    virtual void Deserialise(IStream * stream) abstract;

    /**
    * Writes the game action directly to the given stream. Used for
    * sending across the network in multiplayer.
    */
    virtual void Serialise(IStream * stream) const abstract;

    /**
    * Query the result of the game action without changing the game state.
    */
    virtual GameActionResult Query(uint32 flags = 0) const abstract;

    /**
    * Apply the game action and change the game state.
    */
    virtual GameActionResult Execute(uint32 flags = 0) const abstract;
    
    virtual ~IGameAction() {};
};
