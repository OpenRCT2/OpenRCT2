/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../management/Finance.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(TrackSetBrakeSpeedAction, GAME_COMMAND_SET_BRAKES_SPEED, GameActionResult)
{
private:
    CoordsXYZ _loc;
    track_type_t _trackType;
    uint8_t _brakeSpeed;

public:
    TrackSetBrakeSpeedAction() = default;
    TrackSetBrakeSpeedAction(const CoordsXYZ& loc, track_type_t trackType, uint8_t brakeSpeed)
        : _loc(loc)
        , _trackType(trackType)
        , _brakeSpeed(brakeSpeed)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override
    {
        visitor.Visit(_loc);
        visitor.Visit("trackType", _trackType);
        visitor.Visit("brakeSpeed", _brakeSpeed);
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_loc) << DS_TAG(_trackType) << DS_TAG(_brakeSpeed);
    }

    GameActionResult::Ptr Query() const override
    {
        return QueryExecute(false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return QueryExecute(true);
    }

private:
    GameActionResult::Ptr QueryExecute(bool isExecuting) const
    {
        auto res = MakeResult();

        res->Position = _loc;
        res->Position.x += 16;
        res->Position.y += 16;
        res->Expenditure = ExpenditureType::RideConstruction;

        if (!LocationValid(_loc))
        {
            return MakeResult(GA_ERROR::NOT_OWNED, STR_NONE);
        }

        TileElement* tileElement = map_get_track_element_at_of_type(_loc, _trackType);
        if (tileElement == nullptr)
        {
            log_warning("Invalid game command for setting brakes speed. x = %d, y = %d", _loc.x, _loc.y);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        if (isExecuting)
        {
            tileElement->AsTrack()->SetBrakeBoosterSpeed(_brakeSpeed);
        }
        return res;
    }
};
