/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"
namespace OpenRCT2
{
    class RideSetStatusAction final : public GameActionBase<GameCommand::SetRideStatus>
    {
    private:
        RideId _rideIndex{ RideId::GetNull() };
        RideStatus _status{ RideStatus::Closed };

    public:
        RideSetStatusAction() = default;
        RideSetStatusAction(RideId rideIndex, RideStatus status);

        void AcceptParameters(GameActionParameterVisitor& visitor) override;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        GameActions::Result Query() const override;
        GameActions::Result Execute() const override;
    };
} // namespace OpenRCT2
