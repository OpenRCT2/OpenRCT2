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

enum class RideSetAppearanceType : uint8_t
{
    TrackColourMain,
    TrackColourAdditional,
    TrackColourSupports,
    MazeStyle = TrackColourSupports,
    VehicleColourBody,
    VehicleColourTrim,
    VehicleColourTernary,
    VehicleColourScheme,
    EntranceStyle
};

DEFINE_GAME_ACTION(RideSetAppearanceAction, GAME_COMMAND_SET_RIDE_APPEARANCE, GameActions::Result)
{
private:
    NetworkRideId_t _rideIndex{ RideIdNewNull };
    RideSetAppearanceType _type{};
    uint8_t _value{};
    uint32_t _index{};

public:
    RideSetAppearanceAction() = default;
    RideSetAppearanceAction(ride_id_t rideIndex, RideSetAppearanceType type, uint8_t value, uint32_t index)
        : _rideIndex(rideIndex)
        , _type(type)
        , _value(value)
        , _index(index)
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
};
