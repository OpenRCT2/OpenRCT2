/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../entity/Staff.h"
#include "GameAction.h"

class StaffSetCostumeAction final : public GameActionBase<GameCommand::SetStaffCostume>
{
private:
    EntityId _spriteIndex{ EntityId::GetNull() };
    EntertainerCostume _costume = EntertainerCostume::Count;

public:
    StaffSetCostumeAction() = default;
    StaffSetCostumeAction(EntityId spriteIndex, EntertainerCostume costume);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;
};
