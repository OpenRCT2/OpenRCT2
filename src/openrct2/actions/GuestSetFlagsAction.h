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

class GuestSetFlagsAction final : public GameActionBase<GameCommand::GuestSetFlags>
{
private:
    EntityId _peepId{ EntityId::GetNull() };
    uint32_t _newFlags{};

public:
    GuestSetFlagsAction() = default;
    GuestSetFlagsAction(EntityId peepId, uint32_t flags);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};
