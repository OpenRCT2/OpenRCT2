/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../actions/RideEntranceExitRemoveAction.hpp"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/Station.h"
#include "../world/Entrance.h"
#include "../world/MapAnimation.h"
#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(RideEntranceExitPlaceAction, GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, GameActionResult)
{
private:
    CoordsXY _loc;
    Direction _direction;
    NetworkRideId_t _rideIndex;
    uint8_t _stationNum;
    bool _isExit;

public:
    RideEntranceExitPlaceAction() = default;

    RideEntranceExitPlaceAction(const CoordsXY& loc, Direction direction, ride_id_t rideIndex, uint8_t stationNum, bool isExit)
        : _loc(loc)
        , _direction(direction)
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

        stream << DS_TAG(_loc) << DS_TAG(_direction) << DS_TAG(_rideIndex) << DS_TAG(_stationNum) << DS_TAG(_isExit);
    }

    GameActionResult::Ptr Query() const override
    {
        auto errorTitle = _isExit ? STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION
                                  : STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION;
        if (!map_check_free_elements_and_reorganise(1))
        {
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, errorTitle);
        }

        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            log_warning("Invalid game command for ride %d", (int32_t)_rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, errorTitle);
        }

        if (_stationNum >= MAX_STATIONS)
        {
            log_warning("Invalid station number for ride. stationNum: %u", _stationNum);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, errorTitle);
        }

        if (ride->status != RIDE_STATUS_CLOSED && ride->status != RIDE_STATUS_SIMULATING)
        {
            return MakeResult(GA_ERROR::NOT_CLOSED, errorTitle, STR_MUST_BE_CLOSED_FIRST);
        }

        if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
        {
            return MakeResult(GA_ERROR::DISALLOWED, errorTitle, STR_NOT_ALLOWED_TO_MODIFY_STATION);
        }

        const auto location = _isExit ? ride_get_exit_location(ride, _stationNum)
                                      : ride_get_entrance_location(ride, _stationNum);

        if (!location.isNull())
        {
            auto rideEntranceExitRemove = RideEntranceExitRemoveAction(location.ToCoordsXY(), _rideIndex, _stationNum, _isExit);
            rideEntranceExitRemove.SetFlags(GetFlags());

            auto result = GameActions::QueryNested(&rideEntranceExitRemove);
            if (result->Error != GA_ERROR::OK)
            {
                return result;
            }
        }

        auto z = ride->stations[_stationNum].GetBaseZ();
        if (!gCheatsSandboxMode && !map_is_location_owned({ _loc, z }))
        {
            return MakeResult(GA_ERROR::NOT_OWNED, errorTitle);
        }

        auto clear_z = z + (_isExit ? RideExitHeight : RideEntranceHeight);
        auto cost = MONEY32_UNDEFINED;
        if (!map_can_construct_with_clear_at(
                { _loc, z, clear_z }, &map_place_non_scenery_clear_func, { 0b1111, 0 }, GetFlags(), &cost,
                CREATE_CROSSING_MODE_NONE))
        {
            return MakeResult(GA_ERROR::NO_CLEARANCE, errorTitle, gGameCommandErrorText, gCommonFormatArgs);
        }

        if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
        {
            return MakeResult(GA_ERROR::DISALLOWED, errorTitle, STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
        }

        if (z > MaxRideEntranceOrExitHeight)
        {
            return MakeResult(GA_ERROR::DISALLOWED, errorTitle, STR_TOO_HIGH);
        }

        auto res = MakeResult();
        res->Position = { _loc.ToTileCentre(), z };
        res->Expenditure = ExpenditureType::RideConstruction;
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        // Remember when in unknown station num mode rideIndex is unknown and z is set
        // When in known station num mode rideIndex is known and z is unknown
        auto errorTitle = _isExit ? STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION
                                  : STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION;
        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            log_warning("Invalid game command for ride %d", (int32_t)_rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, errorTitle);
        }

        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            ride_clear_for_construction(ride);
            ride_remove_peeps(ride);
        }

        const auto location = _isExit ? ride_get_exit_location(ride, _stationNum)
                                      : ride_get_entrance_location(ride, _stationNum);
        if (!location.isNull())
        {
            auto rideEntranceExitRemove = RideEntranceExitRemoveAction(location.ToCoordsXY(), _rideIndex, _stationNum, _isExit);
            rideEntranceExitRemove.SetFlags(GetFlags());

            auto result = GameActions::ExecuteNested(&rideEntranceExitRemove);
            if (result->Error != GA_ERROR::OK)
            {
                return result;
            }
        }

        auto z = ride->stations[_stationNum].GetBaseZ();
        if (!(GetFlags() & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED) && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            footpath_remove_litter({ _loc, z });
            wall_remove_at_z({ _loc, z });
        }

        auto clear_z = z + (_isExit ? RideExitHeight : RideEntranceHeight);
        auto cost = MONEY32_UNDEFINED;
        if (!map_can_construct_with_clear_at(
                { _loc, z, clear_z }, &map_place_non_scenery_clear_func, { 0b1111, 0 }, GetFlags() | GAME_COMMAND_FLAG_APPLY,
                &cost, CREATE_CROSSING_MODE_NONE))
        {
            return MakeResult(GA_ERROR::NO_CLEARANCE, errorTitle, gGameCommandErrorText, gCommonFormatArgs);
        }

        auto res = MakeResult();
        res->Position = { _loc.ToTileCentre(), z };
        res->Expenditure = ExpenditureType::RideConstruction;

        TileElement* tileElement = tile_element_insert(CoordsXYZ{ _loc, z }, 0b1111);
        assert(tileElement != nullptr);
        tileElement->SetType(TILE_ELEMENT_TYPE_ENTRANCE);
        tileElement->SetDirection(_direction);
        tileElement->SetClearanceZ(clear_z);
        tileElement->AsEntrance()->SetEntranceType(_isExit ? ENTRANCE_TYPE_RIDE_EXIT : ENTRANCE_TYPE_RIDE_ENTRANCE);
        tileElement->AsEntrance()->SetStationIndex(_stationNum);
        tileElement->AsEntrance()->SetRideIndex(_rideIndex);

        if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
        {
            tileElement->SetGhost(true);
        }

        if (_isExit)
        {
            ride_set_exit_location(
                ride, _stationNum, TileCoordsXYZD(CoordsXYZD{ _loc, z, (uint8_t)tileElement->GetDirection() }));
        }
        else
        {
            ride_set_entrance_location(
                ride, _stationNum, TileCoordsXYZD(CoordsXYZD{ _loc, z, (uint8_t)tileElement->GetDirection() }));
            ride->stations[_stationNum].LastPeepInQueue = SPRITE_INDEX_NULL;
            ride->stations[_stationNum].QueueLength = 0;

            map_animation_create(MAP_ANIMATION_TYPE_RIDE_ENTRANCE, { _loc, z });
        }

        footpath_queue_chain_reset();

        if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
        {
            maze_entrance_hedge_removal({ _loc, tileElement });
        }

        footpath_connect_edges(_loc, tileElement, GetFlags());
        footpath_update_queue_chains();

        map_invalidate_tile_full(_loc);

        return res;
    }

    static GameActionResult::Ptr TrackPlaceQuery(const CoordsXYZ& loc, const bool isExit)
    {
        auto errorTitle = isExit ? STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION
                                 : STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION;
        if (!map_check_free_elements_and_reorganise(1))
        {
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, errorTitle);
        }

        if (!gCheatsSandboxMode && !map_is_location_owned(loc))
        {
            return MakeResult(GA_ERROR::NOT_OWNED, errorTitle);
        }

        int16_t baseZ = loc.z;
        int16_t clearZ = baseZ + (isExit ? RideExitHeight : RideEntranceHeight);
        auto cost = MONEY32_UNDEFINED;
        if (!map_can_construct_with_clear_at(
                { loc, baseZ, clearZ }, &map_place_non_scenery_clear_func, { 0b1111, 0 }, 0, &cost, CREATE_CROSSING_MODE_NONE))
        {
            return MakeResult(GA_ERROR::NO_CLEARANCE, errorTitle, gGameCommandErrorText, gCommonFormatArgs);
        }

        if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
        {
            return MakeResult(GA_ERROR::DISALLOWED, errorTitle, STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
        }

        if (baseZ > MaxRideEntranceOrExitHeight)
        {
            return MakeResult(GA_ERROR::DISALLOWED, errorTitle, STR_TOO_HIGH);
        }
        auto res = MakeResult();
        res->Position = { loc.ToTileCentre(), tile_element_height(loc) };
        res->Expenditure = ExpenditureType::RideConstruction;
        return res;
    }
};
