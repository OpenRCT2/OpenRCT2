/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideEntranceExitRemoveAction.h"

#include "../ride/Ride.h"
#include "../ride/Station.h"
#include "../world/Entrance.h"
#include "../world/TileElementsView.h"

using namespace OpenRCT2;

RideEntranceExitRemoveAction::RideEntranceExitRemoveAction(
    const CoordsXY& loc, RideId rideIndex, StationIndex stationNum, bool isExit)
    : _loc(loc)
    , _rideIndex(rideIndex)
    , _stationNum(stationNum)
    , _isExit(isExit)
{
}

void RideEntranceExitRemoveAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("ride", _rideIndex);
    visitor.Visit("station", _stationNum);
    visitor.Visit("isExit", _isExit);
}

uint16_t RideEntranceExitRemoveAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void RideEntranceExitRemoveAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_rideIndex) << DS_TAG(_stationNum) << DS_TAG(_isExit);
}

static TileElement* FindEntranceElement(
    const CoordsXY& loc, RideId rideIndex, StationIndex stationNum, int32_t entranceType, uint32_t flags)
{
    for (auto* entranceElement : TileElementsView<EntranceElement>(loc))
    {
        if (entranceElement->GetRideIndex() != rideIndex)
            continue;

        if (entranceElement->GetStationIndex() != stationNum)
            continue;

        if (entranceElement->GetEntranceType() != entranceType)
            continue;

        return entranceElement->as<TileElement>();
    }
    return nullptr;
}

GameActions::Result RideEntranceExitRemoveAction::Query() const
{
    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid ride id %u for entrance/exit removal", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    if (ride->status != RideStatus::Closed && ride->status != RideStatus::Simulating)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_MUST_BE_CLOSED_FIRST, STR_NONE);
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NOT_ALLOWED_TO_MODIFY_STATION, STR_NONE);
    }

    if (!LocationValid(_loc))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_LAND_NOT_OWNED_BY_PARK, STR_NONE);
    }

    auto* entranceElement = FindEntranceElement(
        _loc, _rideIndex, _stationNum, _isExit ? ENTRANCE_TYPE_RIDE_EXIT : ENTRANCE_TYPE_RIDE_ENTRANCE, GetFlags());

    // If we are trying to remove a ghost and the element we found is real, return an error, but don't log a warning
    if (entranceElement != nullptr && (GetFlags() & GAME_COMMAND_FLAG_GHOST) && !(entranceElement->IsGhost()))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }
    else if (entranceElement == nullptr)
    {
        log_warning(
            "Track Element not found. x = %d, y = %d, ride = %u, station = %u", _loc.x, _loc.y, _rideIndex.ToUnderlying(),
            _stationNum.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    return GameActions::Result();
}

GameActions::Result RideEntranceExitRemoveAction::Execute() const
{
    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid ride id %u for entrance/exit removal", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    const bool isGhost = GetFlags() & GAME_COMMAND_FLAG_GHOST;
    if (!isGhost)
    {
        ride_clear_for_construction(ride);
        ride->RemovePeeps();
        invalidate_test_results(ride);
    }

    auto* entranceElement = FindEntranceElement(
        _loc, _rideIndex, _stationNum, _isExit ? ENTRANCE_TYPE_RIDE_EXIT : ENTRANCE_TYPE_RIDE_ENTRANCE, GetFlags());

    // If we are trying to remove a ghost and the element we found is real, return an error, but don't log a warning
    if (entranceElement != nullptr && isGhost && !(entranceElement->IsGhost()))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }
    else if (entranceElement == nullptr)
    {
        log_warning(
            "Track Element not found. x = %d, y = %d, ride = %u, station = %d", _loc.x, _loc.y, _rideIndex.ToUnderlying(),
            _stationNum);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    auto res = GameActions::Result();
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = TileElementHeight(res.Position);

    FootpathQueueChainReset();
    MazeEntranceHedgeReplacement({ _loc, entranceElement });
    FootpathRemoveEdgesAt(_loc, entranceElement);

    TileElementRemove(entranceElement);

    auto& station = ride->GetStation(_stationNum);
    if (_isExit)
    {
        station.Exit.SetNull();
    }
    else
    {
        station.Entrance.SetNull();
    }

    FootpathUpdateQueueChains();

    MapInvalidateTileFull(_loc);
    return res;
}
