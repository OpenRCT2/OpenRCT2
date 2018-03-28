#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../localisation/StringIds.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Location.hpp"
#include "GameAction.h"

using namespace OpenRCT2;

using ClearableItems = uint8;
namespace CLEARABLE_ITEMS
{
    constexpr ClearableItems SCENERY_SMALL = 1 << 0;
    constexpr ClearableItems SCENERY_LARGE = 1 << 1;
    constexpr ClearableItems SCENERY_FOOTPATH = 1 << 2;
}

struct ClearAction : public GameActionBase<GAME_COMMAND_CLEAR_SCENERY, GameActionResult>
{
private:
    MapRange _range;
    ClearableItems _itemsToClear;

public:
    ClearAction() { }
    ClearAction(MapRange range, ClearableItems itemsToClear)
        : _range(range),
          _itemsToClear(itemsToClear)
    {
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);
        stream << _range << _itemsToClear;
    }

    GameActionResult::Ptr Query() const override
    {
        return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_UNABLE_TO_REMOVE_ALL_SCENERY_FROM_HERE, STR_NONE);
    }

    GameActionResult::Ptr Execute() const override
    {
        return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_UNABLE_TO_REMOVE_ALL_SCENERY_FROM_HERE, STR_NONE);
    }
};
