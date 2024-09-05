/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

enum class RideSetVisibilityType : uint8_t
{
    Invisible,
    Visible,
};

class RideSetVisibilityAction final : public GameActionBase<GameCommand::SetRideVehicles>
{
private:
    RideId _rideIndex{ RideId::GetNull() };
    RideSetVisibilityType _visibility{};

public:
    RideSetVisibilityAction() = default;
    RideSetVisibilityAction(RideId rideIndex, RideSetVisibilityType visibility);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};
