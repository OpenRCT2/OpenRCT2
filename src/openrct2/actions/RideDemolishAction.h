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

class RideDemolishAction final : public GameActionBase<GameCommand::DemolishRide>
{
private:
    NetworkRideId_t _rideIndex{ RIDE_ID_NULL };
    uint8_t _modifyType{ RIDE_MODIFY_DEMOLISH };

public:
    RideDemolishAction() = default;
    RideDemolishAction(ride_id_t rideIndex, uint8_t modifyType);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint32_t GetCooldownTime() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    GameActions::Result::Ptr DemolishRide(Ride* ride) const;
    money32 MazeRemoveTrack(const CoordsXYZD& coords) const;
    money32 DemolishTracks() const;
    GameActions::Result::Ptr RefurbishRide(Ride* ride) const;
    money32 GetRefurbishPrice(const Ride* ride) const;
    money32 GetRefundPrice(const Ride* ride) const;
};
