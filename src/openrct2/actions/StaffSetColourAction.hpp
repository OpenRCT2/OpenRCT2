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
#include "../localisation/StringIds.h"
#include "../peep/Staff.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "GameAction.h"

using namespace OpenRCT2;

struct StaffSetColourAction : public GameActionBase<GAME_COMMAND_SET_STAFF_COLOUR, GameActionResult>
{
private:
    uint8 _staffType;
    uint8 _colour;

public:
    StaffSetColourAction() {}
    StaffSetColourAction(uint8 staffType, uint8 colour)
        : _staffType(staffType),
          _colour(colour)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);
        stream << _staffType << _colour;
    }

    GameActionResult::Ptr Query() const override
    {
        if (_staffType != STAFF_TYPE_HANDYMAN &&
            _staffType != STAFF_TYPE_MECHANIC &&
            _staffType != STAFF_TYPE_SECURITY)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }
        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        // Update global uniform colour property
        if (!staff_set_colour(_staffType, _colour))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        // Update each staff member's uniform
        sint32 spriteIndex;
        rct_peep * peep;
        FOR_ALL_PEEPS(spriteIndex, peep)
        {
            if (peep->type == PEEP_TYPE_STAFF && peep->staff_type == _staffType)
            {
                peep->tshirt_colour = _colour;
                peep->trousers_colour = _colour;
            }
        }

        gfx_invalidate_screen();
        return MakeResult();
    }
};
