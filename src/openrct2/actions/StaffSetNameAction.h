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

DEFINE_GAME_ACTION(StaffSetNameAction, GameCommand::SetStaffName, GameActions::Result)
{
private:
    uint16_t _spriteIndex{ SPRITE_INDEX_NULL };
    std::string _name;

public:
    StaffSetNameAction() = default;
    StaffSetNameAction(uint16_t spriteIndex, const std::string& name);

    uint16_t GetActionFlags() const override;
    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
