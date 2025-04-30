/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
    ObjectEntryIndex _costume = kObjectEntryIndexNull;

public:
    StaffSetCostumeAction() = default;
    StaffSetCostumeAction(EntityId spriteIndex, ObjectEntryIndex costume);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};
