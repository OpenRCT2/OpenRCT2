/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

class StaffSetOrdersAction final : public GameActionBase<GameCommand::SetStaffOrders>
{
private:
    EntityId _spriteIndex{ EntityId::GetNull() };
    uint8_t _ordersId{};

public:
    StaffSetOrdersAction() = default;
    StaffSetOrdersAction(EntityId spriteIndex, uint8_t ordersId);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};
