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
    bool RideIsModeValid(const Ride& ride) const;
    bool RideIsValidLiftHillSpeed(const Ride& ride) const;
    bool RideIsValidNumCircuits() const;
    bool RideIsValidOperationOption(const Ride& ride) const;
    StringId GetOperationErrorMessage(const Ride& ride) const;
};
