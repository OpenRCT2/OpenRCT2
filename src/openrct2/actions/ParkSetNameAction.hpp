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
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../ui/UiContext.h"
#include "../world/Sprite.h"
#include "../world/Park.h"
#include "GameAction.h"

struct ParkSetNameAction : public GameActionBase<GAME_COMMAND_SET_PARK_NAME, GameActionResult>
{
private:
    std::string _name;

public:
    ParkSetNameAction() {}
    ParkSetNameAction(const std::string& name)
        : _name(name)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);
        stream << _name;
    }

    GameActionResult::Ptr Query() const override
    {
        if (_name.empty())
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_INVALID_RIDE_ATTRACTION_NAME);
        }

        // Ensure user string space.
        rct_string_id stringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER, _name.c_str());
        if (stringId != 0)
        {
            user_string_free(stringId);
        }
        else
        {
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_INVALID_NAME_FOR_PARK);
        }

        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        rct_string_id stringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER, _name.c_str());
        if (stringId == 0)
        {
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_INVALID_NAME_FOR_PARK);
        }

        // Free the old park name.
        user_string_free(gParkName);
        gParkName = stringId;

        gfx_invalidate_screen();

        return MakeResult();
    }
};
