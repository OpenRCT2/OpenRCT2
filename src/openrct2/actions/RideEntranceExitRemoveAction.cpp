/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

void RideEntranceExitRemoveAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("ride", _rideIndex);
    visitor.Visit("station", _stationNum);
    visitor.Visit("isExit", _isExit);
}

void RideEntranceExitRemoveAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_rideIndex) << DS_TAG(_stationNum) << DS_TAG(_isExit);
}

GameActions::Result::Ptr RideEntranceExitRemoveAction::Query() const
{
    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid ride id %d for entrance/exit removal", static_cast<int32_t>(_rideIndex));
        return std::make_unique<GameActions::Result>(GameActions::Status::InvalidParameters, STR_NONE);
    }

    if (ride->status != RIDE_STATUS_CLOSED && ride->status != RIDE_STATUS_SIMULATING)
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_MUST_BE_CLOSED_FIRST);
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_NOT_ALLOWED_TO_MODIFY_STATION);
    }

    if (!LocationValid(_loc))
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_LAND_NOT_OWNED_BY_PARK);
    }

    bool found = false;
    TileElement* tileElement = map_get_first_element_at(_loc);

    do
    {
        if (tileElement == nullptr)
            break;

        if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
            continue;

        if (tileElement->GetRideIndex() != _rideIndex)
            continue;

        if (tileElement->AsEntrance()->GetStationIndex() != _stationNum)
            continue;

        if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) && !(tileElement->IsGhost()))
            continue;

        if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE)
            continue;

        if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE && _isExit)
            continue;

        if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT && !_isExit)
            continue;

        found = true;
        break;
    } while (!(tileElement++)->IsLastForTile());

    if (!found)
    {
        log_warning(
            "Track Element not found. x = %d, y = %d, ride = %d, station = %d", _loc.x, _loc.y,
            static_cast<int32_t>(_rideIndex), _stationNum);
        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
    }

    return MakeResult();
}

GameActions::Result::Ptr RideEntranceExitRemoveAction::Execute() const
{
    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid ride id %d for entrance/exit removal", static_cast<int32_t>(_rideIndex));
        return std::make_unique<GameActions::Result>(GameActions::Status::InvalidParameters, STR_NONE);
    }

    if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
    {
        ride_clear_for_construction(ride);
        ride_remove_peeps(ride);
        invalidate_test_results(ride);
    }

    bool found = false;
    TileElement* tileElement = map_get_first_element_at(_loc);

    do
    {
        if (tileElement == nullptr)
            break;

        if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
            continue;

        if (tileElement->GetRideIndex() != _rideIndex)
            continue;

        if (tileElement->AsEntrance()->GetStationIndex() != _stationNum)
            continue;

        if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) && !tileElement->IsGhost())
            continue;

        if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE)
            continue;

        if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE && _isExit)
            continue;

        if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT && !_isExit)
            continue;

        found = true;
        break;
    } while (!(tileElement++)->IsLastForTile());

    if (!found)
    {
        log_warning(
            "Track Element not found. x = %d, y = %d, ride = %d, station = %d", _loc.x, _loc.y,
            static_cast<int32_t>(_rideIndex), _stationNum);
        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
    }

    auto res = MakeResult();
    res->Position.x = _loc.x + 16;
    res->Position.y = _loc.y + 16;
    res->Position.z = tile_element_height(res->Position);

    footpath_queue_chain_reset();
    maze_entrance_hedge_replacement({ _loc, tileElement });
    footpath_remove_edges_at(_loc, tileElement);

    tile_element_remove(tileElement);

    if (_isExit)
    {
        ride_clear_exit_location(ride, _stationNum);
    }
    else
    {
        ride_clear_entrance_location(ride, _stationNum);
    }

    footpath_update_queue_chains();

    map_invalidate_tile_full(_loc);
    return res;
}
