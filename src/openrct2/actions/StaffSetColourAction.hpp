/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../localisation/StringIds.h"
#include "../peep/Staff.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Sprite.h"
#include "GameAction.h"

struct StaffSetColourAction : public GameActionBase<GAME_COMMAND_SET_STAFF_COLOUR, GameActionResult>
{
private:
    uint8_t _staffType;
    uint8_t _colour;

public:
    StaffSetColourAction() {}
    StaffSetColourAction(uint8_t staffType, uint8_t colour)
        : _staffType(staffType),
          _colour(colour)
    {
    }

    uint16_t GetActionFlags() const override
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
        int32_t spriteIndex;
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
