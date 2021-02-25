/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

DEFINE_GAME_ACTION(StaffSetPatrolAreaAction, GameCommand::SetStaffPatrol, GameActions::Result)
{
private:
    uint16_t _spriteId{ SPRITE_INDEX_NULL };
    CoordsXY _loc;

public:
    StaffSetPatrolAreaAction() = default;
    StaffSetPatrolAreaAction(uint16_t spriteId, const CoordsXY& loc);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
