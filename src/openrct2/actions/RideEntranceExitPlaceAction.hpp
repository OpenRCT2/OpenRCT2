/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
#include "../actions/RideEntranceExitRemoveAction.hpp"
#include "GameAction.h"

DEFINE_GAME_ACTION(RideEntranceExitPlaceAction, GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, GameActionResult)
{
private:
    CoordsXYZD _loc;
    NetworkRideId_t _rideIndex;
    uint8_t _stationNum;
    bool _isExit;

public:
    RideEntranceExitPlaceAction() = default;

    RideEntranceExitPlaceAction(CoordsXYZD loc, ride_id_t rideIndex, uint8_t stationNum, bool isExit)
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
        // Remember when in unknown station num mode rideIndex is unknown and z is set
        // When in known station num mode rideIndex is known and z is unknown
        auto errorTitle = _isExit ? STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION:
        STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION;
        if (!map_check_free_elements_and_reorganise(1))
        {
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, errorTitle);
        }

        if (_stationNum == 0xFF)
        {
            auto z = _loc.z * 16;

            if (!gCheatsSandboxMode && !map_is_location_owned(_loc.x, _loc.y, z))
            {
                return MakeResult(GA_ERROR::NOT_OWNED, errorTitle);
            }

            int16_t clear_z = z / 8 + (_isExit ? 5 : 7);
            auto cost = MONEY32_UNDEFINED;
            if (!map_can_construct_with_clear_at(
                    _loc.x, _loc.y, z / 8, clear_z, &map_place_non_scenery_clear_func, { 0b1111, 0 }, GetFlags(), &cost,
                    CREATE_CROSSING_MODE_NONE))
            {
                return MakeResult(GA_ERROR::NO_CLEARANCE, errorTitle, gGameCommandErrorText, gCommonFormatArgs);
            }

            if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
            {
                return MakeResult(GA_ERROR::DISALLOWED, errorTitle, STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
            }

            if (z / 8 > 244)
            {
                return MakeResult(GA_ERROR::DISALLOWED, errorTitle, STR_TOO_HIGH);
            }
            auto res = MakeResult();
            res->Position.x = _loc.x + 16;
            res->Position.y = _loc.y + 16;
            res->Position.z = tile_element_height(_loc.x, _loc.y);
            res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
            return res;
        }
        else
        {
            if (_rideIndex >= MAX_RIDES || _rideIndex == RIDE_ID_NULL)
            {
                log_warning("Invalid game command for ride %u", _rideIndex);
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, errorTitle);
            }

            Ride* ride = get_ride(_rideIndex);
            if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
            {
                log_warning("Invalid game command for ride %u", _rideIndex);
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, errorTitle);
            }

            if (ride->status != RIDE_STATUS_CLOSED)
            {
                return MakeResult(GA_ERROR::NOT_CLOSED, errorTitle, STR_MUST_BE_CLOSED_FIRST);
            }

            if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
            {
                return MakeResult(GA_ERROR::DISALLOWED, errorTitle, STR_NOT_ALLOWED_TO_MODIFY_STATION);
            }

            ride_clear_for_construction(ride);
            ride_remove_peeps(ride);

            bool requiresRemove = false;
            LocationXY16 removeCoord = { 0, 0 };

            if (_isExit)
            {
                const auto exit = ride_get_exit_location(ride, _stationNum);
                if (!exit.isNull())
                {
                    if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
                    {
                        return MakeResult(GA_ERROR::DISALLOWED, errorTitle);
                    }

                    removeCoord.x = exit.x * 32;
                    removeCoord.y = exit.y * 32;
                    requiresRemove = true;
                }
            }
            else
            {
                const auto entrance = ride_get_entrance_location(ride, _stationNum);
                if (!entrance.isNull())
                {
                    if (GetFlags() & GAME_COMMAND_FLAG_GHOST)
                    {
                        return MakeResult(GA_ERROR::DISALLOWED, errorTitle);
                    }

                    removeCoord.x = entrance.x * 32;
                    removeCoord.y = entrance.y * 32;
                    requiresRemove = true;
                }
            }

            if (requiresRemove)
            {
                auto rideEntranceExitRemove = RideEntranceExitRemoveAction(
                    { removeCoord.x, removeCoord.y }, _rideIndex, _stationNum, _isExit);
                rideEntranceExitRemove.SetFlags(GetFlags());

                auto result = GameActions::QueryNested(&rideEntranceExitRemove);
                if (result->Error != GA_ERROR::OK)
                {
                    return result;
                }
            }

            auto z = ride->stations[_stationNum].Height * 8;
            gCommandPosition.z = z;

            if (!gCheatsSandboxMode && !map_is_location_owned(_loc.x, _loc.y, z))
            {
                return MakeResult(GA_ERROR::NOT_OWNED, errorTitle);
            }

            int8_t clear_z = (z / 8) + (_isExit ? 5 : 7);
            auto cost = MONEY32_UNDEFINED;
            if (!map_can_construct_with_clear_at(
                    _loc.x, _loc.y, z / 8, clear_z, &map_place_non_scenery_clear_func, { 0b1111, 0 }, GetFlags(), &cost,
                    CREATE_CROSSING_MODE_NONE))
            {
                return MakeResult(GA_ERROR::NO_CLEARANCE, errorTitle, gGameCommandErrorText, gCommonFormatArgs);
            }

            if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
            {
                return MakeResult(GA_ERROR::DISALLOWED, errorTitle, STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
            }

            if (z / 8 > 244)
            {
                return MakeResult(GA_ERROR::DISALLOWED, errorTitle, STR_TOO_HIGH);
            }

            auto res = MakeResult();
            res->Position.x = _loc.x + 16;
            res->Position.y = _loc.y + 16;
            res->Position.z = tile_element_height(_loc.x, _loc.y);
            res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
            return res;
        }
    }

    GameActionResult::Ptr Execute() const override
    {
        // Remember when in unknown station num mode rideIndex is unknown and z is set
        // When in known station num mode rideIndex is known and z is unknown
        auto errorTitle = _isExit ? STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION
                                  : STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION;
        if (_stationNum == 0xFF)
        {
            return MakeResult(GA_ERROR::UNKNOWN, errorTitle);
        }
        else
        {
            Ride* ride = get_ride(_rideIndex);
            if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
            {
                log_warning("Invalid game command for ride %u", _rideIndex);
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, errorTitle);
            }

            ride_clear_for_construction(ride);
            ride_remove_peeps(ride);

            bool requiresRemove = false;
            LocationXY16 removeCoord = { 0, 0 };

            if (_isExit)
            {
                const auto exit = ride_get_exit_location(ride, _stationNum);
                if (!exit.isNull())
                {
                    removeCoord.x = exit.x * 32;
                    removeCoord.y = exit.y * 32;
                    requiresRemove = true;
                }
            }
            else
            {
                const auto entrance = ride_get_entrance_location(ride, _stationNum);
                if (!entrance.isNull())
                {
                    removeCoord.x = entrance.x * 32;
                    removeCoord.y = entrance.y * 32;
                    requiresRemove = true;
                }
            }

            if (requiresRemove)
            {
                auto rideEntranceExitRemove = RideEntranceExitRemoveAction(
                    { removeCoord.x, removeCoord.y }, _rideIndex, _stationNum, _isExit);
                rideEntranceExitRemove.SetFlags(GetFlags());

                auto result = GameActions::ExecuteNested(&rideEntranceExitRemove);
                if (result->Error != GA_ERROR::OK)
                {
                    return result;
                }
            }

            auto z = ride->stations[_stationNum].Height * 8;
            gCommandPosition.z = z;

            if (!(GetFlags() & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED)
                && !(GetFlags() & GAME_COMMAND_FLAG_GHOST))
            {
                footpath_remove_litter(_loc.x, _loc.y, z);
                wall_remove_at_z(_loc.x, _loc.y, z);
            }

            int8_t clear_z = (z / 8) + (_isExit ? 5 : 7);
            auto cost = MONEY32_UNDEFINED;
            if (!map_can_construct_with_clear_at(
                    _loc.x, _loc.y, z / 8, clear_z, &map_place_non_scenery_clear_func, { 0b1111, 0 }, GetFlags(), &cost,
                    CREATE_CROSSING_MODE_NONE))
            {
                return MakeResult(GA_ERROR::NO_CLEARANCE, errorTitle, gGameCommandErrorText, gCommonFormatArgs);
            }

            auto res = MakeResult();
            res->Position.x = _loc.x + 16;
            res->Position.y = _loc.y + 16;
            res->Position.z = tile_element_height(_loc.x, _loc.y);
            res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;

            TileElement* tileElement = tile_element_insert(_loc.x / 32, _loc.y / 32, z / 8, 0b1111);
            assert(tileElement != nullptr);
            tileElement->SetType(TILE_ELEMENT_TYPE_ENTRANCE);
            tileElement->SetDirection(_loc.direction);
            tileElement->clearance_height = clear_z;
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
                    ride, _stationNum, { _loc.x / 32, _loc.y / 32, z / 8, (uint8_t)tileElement->GetDirection() });
            }
            else
            {
                ride_set_entrance_location(
                    ride, _stationNum, { _loc.x / 32, _loc.y / 32, z / 8, (uint8_t)tileElement->GetDirection() });
                ride->stations[_stationNum].LastPeepInQueue = SPRITE_INDEX_NULL;
                ride->stations[_stationNum].QueueLength = 0;

                map_animation_create(MAP_ANIMATION_TYPE_RIDE_ENTRANCE, _loc.x, _loc.y, z / 8);
            }

            footpath_queue_chain_reset();

            if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
            {
                maze_entrance_hedge_removal(_loc.x, _loc.y, tileElement);
            }

            footpath_connect_edges(_loc.x, _loc.y, tileElement, GetFlags());
            footpath_update_queue_chains();

            map_invalidate_tile_full(_loc.x, _loc.y);
            
            return res;
        }
    }
};
