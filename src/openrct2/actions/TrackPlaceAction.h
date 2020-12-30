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

class TrackPlaceActionResult final : public GameActions::Result
{
public:
    TrackPlaceActionResult();
    TrackPlaceActionResult(GameActions::Status error);
    TrackPlaceActionResult(GameActions::Status error, rct_string_id message);
    TrackPlaceActionResult(GameActions::Status error, rct_string_id message, uint8_t* args);

    uint8_t GroundFlags{ 0 };
};

DEFINE_GAME_ACTION(TrackPlaceAction, GameCommand::PlaceTrack, TrackPlaceActionResult)
{
private:
    NetworkRideId_t _rideIndex{ RideIdNewNull };
    int32_t _trackType{};
    CoordsXYZD _origin;
    int32_t _brakeSpeed{};
    int32_t _colour{};
    int32_t _seatRotation{};
    int32_t _trackPlaceFlags{};
    bool _fromTrackDesign{};

public:
    TrackPlaceAction() = default;
    TrackPlaceAction(
        NetworkRideId_t rideIndex, int32_t trackType, const CoordsXYZD& origin, int32_t brakeSpeed, int32_t colour,
        int32_t seatRotation, int32_t liftHillAndAlternativeState, bool fromTrackDesign);

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override final;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
