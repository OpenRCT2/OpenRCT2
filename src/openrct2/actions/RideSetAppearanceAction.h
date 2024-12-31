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

enum class RideSetAppearanceType : uint8_t
{
    TrackColourMain,
    TrackColourAdditional,
    TrackColourSupports,
    MazeStyle = TrackColourSupports,
    VehicleColourBody,
    VehicleColourTrim,
    VehicleColourTertiary,
    VehicleColourScheme,
    EntranceStyle,
    SellingItemColourIsRandom
};

class RideSetAppearanceAction final : public GameActionBase<GameCommand::SetRideAppearance>
{
private:
    RideId _rideIndex{ RideId::GetNull() };
    RideSetAppearanceType _type{};
    uint16_t _value{};
    uint32_t _index{};

public:
    RideSetAppearanceAction() = default;
    RideSetAppearanceAction(RideId rideIndex, RideSetAppearanceType type, uint16_t value, uint32_t index);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};
