/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../ride/TrackDesign.h"
#include "GameAction.h"

class TrackDesignActionResult final : public GameActions::Result
{
public:
    TrackDesignActionResult();
    TrackDesignActionResult(GameActions::Status error);
    TrackDesignActionResult(GameActions::Status error, rct_string_id title, rct_string_id message);
    TrackDesignActionResult(GameActions::Status error, rct_string_id message);

    ride_id_t rideIndex = RIDE_ID_NULL;
};

DEFINE_GAME_ACTION(TrackDesignAction, GameCommand::PlaceTrackDesign, TrackDesignActionResult)
{
private:
    CoordsXYZD _loc;
    TrackDesign _td;

public:
    TrackDesignAction() = default;
    TrackDesignAction(const CoordsXYZD& location, const TrackDesign& td);

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;

    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
