/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(GuestSetFlagsAction, GAME_COMMAND_GUEST_SET_FLAGS, GameActionResult)
{
private:
    uint16_t _peepId = SPRITE_INDEX_NULL;
    uint32_t _newFlags = 0;

public:
    GuestSetFlagsAction()
    {
    }

    GuestSetFlagsAction(uint16_t peepId, uint32_t flags)
        : _peepId(peepId)
        , _newFlags(flags)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_peepId) << DS_TAG(_newFlags);
    }

    GameActionResult::Ptr Query() const override
    {
        Peep* peep = TryGetEntity<Peep>(_peepId);
        if (peep == nullptr)
        {
            log_error("Used invalid sprite index for peep: %u", static_cast<uint32_t>(_peepId));
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_THIS);
        }
        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        Peep* peep = TryGetEntity<Peep>(_peepId);
        if (peep == nullptr)
        {
            log_error("Used invalid sprite index for peep: %u", static_cast<uint32_t>(_peepId));
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_CHANGE_THIS);
        }

        peep->PeepFlags = _newFlags;

        return std::make_unique<GameActionResult>();
    }
};
