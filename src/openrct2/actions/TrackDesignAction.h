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

class TrackDesignActionResult final : public GameActionResult
{
public:
    TrackDesignActionResult()
        : GameActionResult(GA_ERROR::OK, STR_NONE)
    {
    }
    TrackDesignActionResult(GA_ERROR error)
        : GameActionResult(error, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, STR_NONE)
    {
    }
    TrackDesignActionResult(GA_ERROR error, rct_string_id title, rct_string_id message)
        : GameActionResult(error, title, message)
    {
    }
    TrackDesignActionResult(GA_ERROR error, rct_string_id message)
        : GameActionResult(error, STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE, message)
    {
    }

    ride_id_t rideIndex = RIDE_ID_NULL;
};

DEFINE_GAME_ACTION(TrackDesignAction, GAME_COMMAND_PLACE_TRACK_DESIGN, TrackDesignActionResult)
{
private:
    CoordsXYZD _loc;
    TrackDesign _td;

public:
    TrackDesignAction() = default;

    TrackDesignAction(const CoordsXYZD& location, const TrackDesign& td)
        : _loc(location)
        , _td(td)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override
    {
        visitor.Visit(_loc);
        // TODO visit the track design (it has a lot of sub fields)
    }

    uint16_t GetActionFlags() const override
    {
        return GameActionBase::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc);
        _td.Serialise(stream);
    }

    GameActionResult::Ptr Query() const override;
    GameActionResult::Ptr Execute() const override;
};
