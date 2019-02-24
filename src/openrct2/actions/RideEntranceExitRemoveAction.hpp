/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../ride/Ride.h"
#include "../ride/Station.h"
#include "../world/Entrance.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(RideEntranceExitRemoveAction, GAME_COMMAND_REMOVE_RIDE_ENTRANCE_OR_EXIT, GameActionResult)
{
private:
    CoordsXY _loc;
    NetworkRideId_t _rideIndex;
    uint8_t _stationNum;
    bool _isExit;

public:
    RideEntranceExitRemoveAction() = default;

    RideEntranceExitRemoveAction(CoordsXY loc, ride_id_t rideIndex, uint8_t stationNum, bool isExit)
        : _loc(loc)
        , _rideIndex(rideIndex)
        , _stationNum(stationNum)
        , _isExit(isExit)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_rideIndex) << DS_TAG(_stationNum) << DS_TAG(_isExit);
    }

    GameActionResult::Ptr Query() const override
    {
        if (_rideIndex >= MAX_RIDES || _rideIndex == RIDE_ID_NULL)
        {
            log_warning("Invalid game command for ride %u", uint32_t(_rideIndex));
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        Ride* ride = get_ride(_rideIndex);
        if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid ride id %u for entrance/exit removal", _rideIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        if (ride->status != RIDE_STATUS_CLOSED)
        {
            gGameCommandErrorText = STR_MUST_BE_CLOSED_FIRST;
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_MUST_BE_CLOSED_FIRST);
        }

        if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NOT_ALLOWED_TO_MODIFY_STATION);
        }

        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        Ride* ride = get_ride(_rideIndex);
        if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid ride id %u for entrance/exit removal", _rideIndex);
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        ride_clear_for_construction(ride);
        ride_remove_peeps(ride);
        invalidate_test_results(ride);

        bool found = false;
        TileElement* tileElement = map_get_first_element_at(_loc.x / 32, _loc.y / 32);

        do
        {
            if (tileElement == nullptr)
                break;

            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;

            if (tile_element_get_ride_index(tileElement) != _rideIndex)
                continue;

            if (tileElement->AsEntrance()->GetStationIndex() != _stationNum)
                continue;

            if ((GetFlags() & GAME_COMMAND_FLAG_5) && !(tileElement->flags & TILE_ELEMENT_FLAG_GHOST))
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
                "Track Element not found. x = %d, y = %d, ride = %d, station = %d", _loc.x, _loc.y, _rideIndex, _stationNum);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        auto res = MakeResult();
        res->Position.x = _loc.x + 16;
        res->Position.y = _loc.y + 16;
        res->Position.z = tile_element_height(res->Position.x, res->Position.y);

        footpath_queue_chain_reset();
        maze_entrance_hedge_replacement(_loc.x, _loc.y, tileElement);
        footpath_remove_edges_at(_loc.x, _loc.y, tileElement);

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

        map_invalidate_tile_full(_loc.x, _loc.y);
        return res;
    }
};
