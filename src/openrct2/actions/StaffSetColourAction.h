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

class StaffSetColourAction final : public GameActionBase<GameCommand::SetStaffColour>
{
private:
    uint8_t _staffType{};
    uint8_t _colour{};

public:
    StaffSetColourAction() = default;
    StaffSetColourAction(StaffType staffType, uint8_t colour);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};
