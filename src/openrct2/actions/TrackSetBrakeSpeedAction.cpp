/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackSetBrakeSpeedAction.h"

#include "../Diagnostic.h"
#include "../management/Finance.h"
#include "../world/tile_element/TrackElement.h"

using namespace OpenRCT2;

TrackSetBrakeSpeedAction::TrackSetBrakeSpeedAction(const CoordsXYZ& loc, OpenRCT2::TrackElemType trackType, uint8_t brakeSpeed)
    : _loc(loc)
    , _trackType(trackType)
    , _brakeSpeed(brakeSpeed)
{
}

void TrackSetBrakeSpeedAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("trackType", _trackType);
    visitor.Visit("brakeSpeed", _brakeSpeed);
}

uint16_t TrackSetBrakeSpeedAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void TrackSetBrakeSpeedAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_loc) << DS_TAG(_trackType) << DS_TAG(_brakeSpeed);
}

GameActions::Result TrackSetBrakeSpeedAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result TrackSetBrakeSpeedAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result TrackSetBrakeSpeedAction::QueryExecute(bool isExecuting) const
{
    auto res = GameActions::Result();

    res.Position = _loc;
    res.Position.x += 16;
    res.Position.y += 16;
    res.Expenditure = ExpenditureType::RideConstruction;

    if (!LocationValid(_loc))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_THIS, STR_OFF_EDGE_OF_MAP);
    }

    TileElement* tileElement = MapGetTrackElementAtOfType(_loc, _trackType);
    if (tileElement == nullptr)
    {
        LOG_ERROR("Track element of type %u not found at x = %d, y = %d, z = %d", _trackType, _loc.x, _loc.y, _loc.z);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TILE_ELEMENT_NOT_FOUND);
    }

    if (_brakeSpeed > kMaximumTrackSpeed)
    {
        LOG_WARNING("Invalid speed for track, speed = %d", _brakeSpeed);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_SPEED_TOO_HIGH, kStringIdNone);
    }

    if (isExecuting)
    {
        GetTrackElementOriginAndApplyChanges(
            { _loc, tileElement->GetDirection() }, tileElement->AsTrack()->GetTrackType(), _brakeSpeed, nullptr,
            TRACK_ELEMENT_SET_BRAKE_BOOSTER_SPEED);
    }
    return res;
}
