/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../interface/Window.h"
#include "../peep/Peep.h"
#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(StaffFireAction, GAME_COMMAND_FIRE_STAFF_MEMBER, GameActionResult)
{
private:
    uint16_t _spriteId{ SPRITE_INDEX_NULL };

public:
    StaffFireAction()
    {
    }
    StaffFireAction(uint16_t spriteId)
        : _spriteId(spriteId)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_spriteId);
    }

    GameActionResult::Ptr Query() const override
    {
        if (_spriteId >= MAX_SPRITES)
        {
            log_error("Invalid spriteId. spriteId = %u", _spriteId);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        auto staff = TryGetEntity<Staff>(_spriteId);
        if (staff == nullptr)
        {
            log_error("Invalid spriteId. spriteId = %u", _spriteId);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto staff = TryGetEntity<Staff>(_spriteId);
        if (staff == nullptr)
        {
            log_error("Invalid spriteId. spriteId = %u", _spriteId);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }
        window_close_by_class(WC_FIRE_PROMPT);
        peep_sprite_remove(staff);
        return MakeResult();
    }
};
