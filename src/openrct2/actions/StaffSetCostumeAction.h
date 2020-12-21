/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../peep/Staff.h"
#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(StaffSetCostumeAction, GAME_COMMAND_SET_STAFF_COSTUME, GameActions::Result)
{
private:
    uint16_t _spriteIndex{ SPRITE_INDEX_NULL };
    EntertainerCostume _costume = EntertainerCostume::Count;

public:
    StaffSetCostumeAction() = default;
    StaffSetCostumeAction(uint16_t spriteIndex, EntertainerCostume costume);

    void AcceptParameters(GameActionParameterVisitor & visitor);
    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
