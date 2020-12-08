/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(StaffSetOrdersAction, GAME_COMMAND_SET_STAFF_ORDERS, GameActions::Result)
{
private:
    uint16_t _spriteIndex{ SPRITE_INDEX_NULL };
    uint8_t _ordersId{};

public:
    StaffSetOrdersAction() = default;
    StaffSetOrdersAction(uint16_t spriteIndex, uint8_t ordersId)
        : _spriteIndex(spriteIndex)
        , _ordersId(ordersId)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
