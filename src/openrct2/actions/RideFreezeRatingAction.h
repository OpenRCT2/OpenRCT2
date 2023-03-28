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

enum class RideRatingType : uint8_t
{
    Excitement,
    Intensity,
    Nausea,
};
//
class RideFreezeRatingAction final : public GameActionBase<GameCommand::FreezeRideRating>
{
private:
    RideId _rideIndex{ RideId::GetNull() };
    RideRatingType _type{};
    ride_rating _value{};

public:
    RideFreezeRatingAction() = default;
    RideFreezeRatingAction(RideId rideIndex, RideRatingType type, ride_rating value);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;
};
