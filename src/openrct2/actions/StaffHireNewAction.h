/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../entity/Staff.h"
#include "GameAction.h"

struct StaffHireNewActionResult
{
    EntityId StaffEntityId = EntityId::GetNull();
};

class StaffHireNewAction final : public GameActionBase<GameCommand::HireNewStaffMember>
{
private:
    bool _autoPosition{};
    uint8_t _staffType{ EnumValue(StaffType::Count) };
    EntertainerCostume _entertainerType{ EntertainerCostume::Count };
    uint32_t _staffOrders{};

public:
    StaffHireNewAction() = default;
    StaffHireNewAction(bool autoPosition, StaffType staffType, EntertainerCostume entertainerType, uint32_t staffOrders);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    GameActions::Result QueryExecute(bool execute) const;
    void AutoPositionNewStaff(Peep* newPeep) const;
};
