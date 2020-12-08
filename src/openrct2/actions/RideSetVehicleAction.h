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

enum class RideSetVehicleType : uint8_t
{
    NumTrains,
    NumCarsPerTrain,
    RideEntry,
    Count,
};

DEFINE_GAME_ACTION(RideSetVehicleAction, GAME_COMMAND_SET_RIDE_VEHICLES, GameActions::Result)
{
private:
    NetworkRideId_t _rideIndex{ RideIdNewNull };
    RideSetVehicleType _type{};
    uint8_t _value{};
    uint8_t _colour{};

    constexpr static rct_string_id SetVehicleTypeErrorTitle[] = { STR_RIDE_SET_VEHICLE_SET_NUM_TRAINS_FAIL,
                                                                  STR_RIDE_SET_VEHICLE_SET_NUM_CARS_PER_TRAIN_FAIL,
                                                                  STR_RIDE_SET_VEHICLE_TYPE_FAIL };

public:
    RideSetVehicleAction() = default;
    RideSetVehicleAction(ride_id_t rideIndex, RideSetVehicleType type, uint8_t value, uint8_t colour = 0)
        : _rideIndex(rideIndex)
        , _type(type)
        , _value(value)
        , _colour(colour)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    bool ride_is_vehicle_type_valid(Ride * ride) const;
};
