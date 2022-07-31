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

enum class RideSetSetting : uint8_t
{
    Mode,
    Departure,
    MinWaitingTime,
    MaxWaitingTime,
    Operation,
    InspectionInterval,
    Music,
    MusicType,
    LiftHillSpeed,
    NumCircuits,
    RideType,
};

class RideSetSettingAction final : public GameActionBase<GameCommand::SetRideSetting>
{
private:
    RideId _rideIndex{ RideId::GetNull() };
    RideSetSetting _setting{};
    uint8_t _value{};

public:
    RideSetSettingAction() = default;
    RideSetSettingAction(RideId rideIndex, RideSetSetting setting, uint8_t value);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    bool ride_is_mode_valid(Ride* ride) const;
    bool ride_is_valid_lift_hill_speed(Ride* ride) const;
    bool ride_is_valid_num_circuits() const;
    bool ride_is_valid_operation_option(Ride* ride) const;
    StringId GetOperationErrorMessage(Ride* ride) const;
};
