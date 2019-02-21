/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../management/Finance.h"
#include "../ride/RideGroupManager.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/TrackDesign.h"
#include "../util/Util.h"
#include "../world/MapAnimation.h"
#include "../world/Surface.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(TrackRemoveAction, GAME_COMMAND_REMOVE_TRACK, GameActionResult)
{
private:
    int32_t _trackType{ 0 };
    int32_t _sequence{ 0 };
    CoordsXYZD _origin;

public:
    TrackRemoveAction()
    {
    }

    TrackRemoveAction(int32_t trackType, int32_t sequence, CoordsXYZD origin)
        : _trackType(trackType)
        , _sequence(sequence)
        , _origin(origin)
    {
        _origin.direction &= 3;
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_trackType) << DS_TAG(_sequence) << DS_TAG(_origin);
    }

    GameActionResult::Ptr Query() const override
    {
        auto res = MakeResult();
        res->Position.x = _origin.x + 16;
        res->Position.y = _origin.y + 16;
        res->Position.z = _origin.z;
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;

        int16_t trackpieceZ = _origin.z;

        // Stations require some massaging of the track type for comparing
        auto comparableTrackType = _trackType;
        switch (_trackType)
        {
            case TRACK_ELEM_BEGIN_STATION:
            case TRACK_ELEM_MIDDLE_STATION:
                comparableTrackType = TRACK_ELEM_END_STATION;
                break;
        }

        bool found = false;
        bool isGhost = GetFlags() & GAME_COMMAND_FLAG_GHOST;
        TileElement* tileElement = map_get_first_element_at(_origin.x / 32, _origin.y / 32);

        do
        {
            if (tileElement == nullptr)
                break;

            if (tileElement->base_height * 8 != _origin.z)
                continue;

            if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                continue;

            if ((tileElement->GetDirection()) != _origin.direction)
                continue;

            if (tileElement->AsTrack()->GetSequenceIndex() != _sequence)
                continue;

            if (tileElement->IsGhost() != isGhost)
                continue;

            uint8_t tileTrackType = tileElement->AsTrack()->GetTrackType();
            switch (tileTrackType)
            {
                case TRACK_ELEM_BEGIN_STATION:
                case TRACK_ELEM_MIDDLE_STATION:
                    tileTrackType = TRACK_ELEM_END_STATION;
                    break;
            }

            if (tileTrackType != comparableTrackType)
                continue;

            found = true;
            break;
        } while (!(tileElement++)->IsLastForTile());

        if (!found)
        {
            log_warning(
                "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", _origin.x, _origin.y, _origin.z,
                _origin.direction, _sequence);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
        }

        if (tileElement->flags & TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE)
        {
            return MakeResult(
                GA_ERROR::DISALLOWED, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, STR_YOU_ARE_NOT_ALLOWED_TO_REMOVE_THIS_SECTION);
        }

        ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
        auto trackType = tileElement->AsTrack()->GetTrackType();

        Ride* ride = get_ride(rideIndex);
        if (ride == nullptr)
        {
            log_warning("Ride not found. ride index = %d.", rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
        }
        const rct_preview_track* trackBlock = get_track_def_from_ride(ride, trackType);
        trackBlock += tileElement->AsTrack()->GetSequenceIndex();

        auto startLoc = _origin;
        startLoc.direction = tileElement->GetDirection();

        LocationXY16 trackLoc = { trackBlock->x, trackBlock->y };
        rotate_map_coordinates(&trackLoc.x, &trackLoc.y, startLoc.direction);
        startLoc.x -= trackLoc.x;
        startLoc.y -= trackLoc.y;
        startLoc.z -= trackBlock->z;

        money32 cost = 0;

        trackBlock = get_track_def_from_ride(ride, trackType);
        for (; trackBlock->index != 255; trackBlock++)
        {
            CoordsXYZ mapLoc{ startLoc.x, startLoc.y, startLoc.z };
            trackLoc = { trackBlock->x, trackBlock->y };
            rotate_map_coordinates(&trackLoc.x, &trackLoc.y, startLoc.direction);
            mapLoc.x += trackLoc.x;
            mapLoc.y += trackLoc.y;
            mapLoc.z += trackBlock->z;

            map_invalidate_tile_full(mapLoc.x, mapLoc.y);

            trackpieceZ = mapLoc.z;

            found = false;
            tileElement = map_get_first_element_at(mapLoc.x / 32, mapLoc.y / 32);
            do
            {
                if (tileElement == nullptr)
                    break;

                if (tileElement->base_height != mapLoc.z / 8)
                    continue;

                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;

                if (tileElement->GetDirection() != _origin.direction)
                    continue;

                if (tileElement->AsTrack()->GetSequenceIndex() != trackBlock->index)
                    continue;

                if (tileElement->AsTrack()->GetTrackType() != trackType)
                    continue;

                if (tileElement->IsGhost() != isGhost)
                    continue;

                found = true;
                break;
            } while (!(tileElement++)->IsLastForTile());

            if (!found)
            {
                log_warning(
                    "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", mapLoc.x, mapLoc.y, mapLoc.z,
                    _origin.direction, trackBlock->index);
                return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
            }

            int32_t entranceDirections;
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                entranceDirections = FlatRideTrackSequenceProperties[trackType][0];
            }
            else
            {
                entranceDirections = TrackSequenceProperties[trackType][0];
            }

            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (tileElement->AsTrack()->GetSequenceIndex() == 0))
            {
                if (!track_remove_station_element(mapLoc.x, mapLoc.y, mapLoc.z / 8, _origin.direction, rideIndex, 0))
                {
                    return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, gGameCommandErrorText);
                }
            }

            TileElement* surfaceElement = map_get_surface_element_at({ mapLoc.x, mapLoc.y });
            if (surfaceElement == nullptr)
            {
                log_warning("Surface Element not found. x = %d, y = %d", mapLoc.x, mapLoc.y);
                return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
            }

            int8_t _support_height = tileElement->base_height - surfaceElement->base_height;
            if (_support_height < 0)
            {
                _support_height = 10;
            }

            cost += (_support_height / 2) * RideTrackCosts[ride->type].support_price;
        }

        money32 price = RideTrackCosts[ride->type].track_price;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            price *= FlatRideTrackPricing[trackType];
        }
        else
        {
            price *= TrackPricing[trackType];
        }
        price >>= 16;
        price = (price + cost) / 2;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
            price *= -7;
        else
            price *= -10;

        LocationXYZ16 coord;
        coord.x = startLoc.x + 16;
        coord.y = startLoc.y + 16;
        coord.z = trackpieceZ;
        network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);

        res->Cost = price;
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        auto res = MakeResult();
        res->Position.x = _origin.x + 16;
        res->Position.y = _origin.y + 16;
        res->Position.z = _origin.z;
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;

        int16_t trackpieceZ = _origin.z;

        // Stations require some massaging of the track type for comparing
        auto comparableTrackType = _trackType;
        switch (_trackType)
        {
            case TRACK_ELEM_BEGIN_STATION:
            case TRACK_ELEM_MIDDLE_STATION:
                comparableTrackType = TRACK_ELEM_END_STATION;
                break;
        }

        bool found = false;
        bool isGhost = GetFlags() & GAME_COMMAND_FLAG_GHOST;
        TileElement* tileElement = map_get_first_element_at(_origin.x / 32, _origin.y / 32);

        do
        {
            if (tileElement == nullptr)
                break;

            if (tileElement->base_height * 8 != _origin.z)
                continue;

            if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                continue;

            if ((tileElement->GetDirection()) != _origin.direction)
                continue;

            if (tileElement->AsTrack()->GetSequenceIndex() != _sequence)
                continue;

            if (tileElement->IsGhost() != isGhost)
                continue;

            uint8_t tileTrackType = tileElement->AsTrack()->GetTrackType();
            switch (tileTrackType)
            {
                case TRACK_ELEM_BEGIN_STATION:
                case TRACK_ELEM_MIDDLE_STATION:
                    tileTrackType = TRACK_ELEM_END_STATION;
                    break;
            }

            if (tileTrackType != comparableTrackType)
                continue;

            found = true;
            break;
        } while (!(tileElement++)->IsLastForTile());

        if (!found)
        {
            log_warning(
                "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", _origin.x, _origin.y, _origin.z,
                _origin.direction, _sequence);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
        }

        ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
        auto trackType = tileElement->AsTrack()->GetTrackType();
        bool isLiftHill = tileElement->AsTrack()->HasChain();

        Ride* ride = get_ride(rideIndex);
        if (ride == nullptr)
        {
            log_warning("Ride not found. ride index = %d.", rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
        }
        const rct_preview_track* trackBlock = get_track_def_from_ride(ride, trackType);
        trackBlock += tileElement->AsTrack()->GetSequenceIndex();

        auto startLoc = _origin;
        startLoc.direction = tileElement->GetDirection();

        LocationXY16 trackLoc = { trackBlock->x, trackBlock->y };
        rotate_map_coordinates(&trackLoc.x, &trackLoc.y, startLoc.direction);
        startLoc.x -= trackLoc.x;
        startLoc.y -= trackLoc.y;
        startLoc.z -= trackBlock->z;

        money32 cost = 0;

        trackBlock = get_track_def_from_ride(ride, trackType);
        for (; trackBlock->index != 255; trackBlock++)
        {
            CoordsXYZ mapLoc{ startLoc.x, startLoc.y, startLoc.z };
            trackLoc = { trackBlock->x, trackBlock->y };
            rotate_map_coordinates(&trackLoc.x, &trackLoc.y, startLoc.direction);
            mapLoc.x += trackLoc.x;
            mapLoc.y += trackLoc.y;
            mapLoc.z += trackBlock->z;

            map_invalidate_tile_full(mapLoc.x, mapLoc.y);

            trackpieceZ = mapLoc.z;

            found = false;
            tileElement = map_get_first_element_at(mapLoc.x / 32, mapLoc.y / 32);
            do
            {
                if (tileElement == nullptr)
                    break;

                if (tileElement->base_height != mapLoc.z / 8)
                    continue;

                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;

                if (tileElement->GetDirection() != _origin.direction)
                    continue;

                if (tileElement->AsTrack()->GetSequenceIndex() != trackBlock->index)
                    continue;

                if (tileElement->AsTrack()->GetTrackType() != trackType)
                    continue;

                if (tileElement->IsGhost() != isGhost)
                    continue;

                found = true;
                break;
            } while (!(tileElement++)->IsLastForTile());

            if (!found)
            {
                log_warning(
                    "Track Element not found. x = %d, y = %d, z = %d, d = %d, seq = %d.", mapLoc.x, mapLoc.y, mapLoc.z,
                    _origin.direction, trackBlock->index);
                return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
            }

            int32_t entranceDirections;
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                entranceDirections = FlatRideTrackSequenceProperties[trackType][0];
            }
            else
            {
                entranceDirections = TrackSequenceProperties[trackType][0];
            }

            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (tileElement->AsTrack()->GetSequenceIndex() == 0))
            {
                if (!track_remove_station_element(mapLoc.x, mapLoc.y, mapLoc.z / 8, _origin.direction, rideIndex, 0))
                {
                    return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, gGameCommandErrorText);
                }
            }

            TileElement* surfaceElement = map_get_surface_element_at({ mapLoc.x, mapLoc.y });
            if (surfaceElement == nullptr)
            {
                log_warning("Surface Element not found. x = %d, y = %d", mapLoc.x, mapLoc.y);
                return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS);
            }

            int8_t _support_height = tileElement->base_height - surfaceElement->base_height;
            if (_support_height < 0)
            {
                _support_height = 10;
            }

            cost += (_support_height / 2) * RideTrackCosts[ride->type].support_price;

            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (tileElement->AsTrack()->GetSequenceIndex() == 0))
            {
                if (!track_remove_station_element(
                        mapLoc.x, mapLoc.y, mapLoc.z / 8, _origin.direction, rideIndex, GAME_COMMAND_FLAG_APPLY))
                {
                    return MakeResult(GA_ERROR::UNKNOWN, STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS, gGameCommandErrorText);
                }
            }

            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER))
            {
                surfaceElement->AsSurface()->SetHasTrackThatNeedsWater(false);
            }

            invalidate_test_results(ride);
            footpath_queue_chain_reset();
            if (!gCheatsDisableClearanceChecks || !(tileElement->flags & TILE_ELEMENT_FLAG_GHOST))
            {
                footpath_remove_edges_at(mapLoc.x, mapLoc.y, tileElement);
            }
            tile_element_remove(tileElement);
            sub_6CB945(ride);
            if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST))
            {
                ride_update_max_vehicles(ride);
            }
        }

        switch (trackType)
        {
            case TRACK_ELEM_ON_RIDE_PHOTO:
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_ON_RIDE_PHOTO;
                break;
            case TRACK_ELEM_CABLE_LIFT_HILL:
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED;
                break;
            case TRACK_ELEM_BLOCK_BRAKES:
                ride->num_block_brakes--;
                if (ride->num_block_brakes == 0)
                {
                    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_OPERATING;
                    ride->mode = RIDE_MODE_CONTINUOUS_CIRCUIT;
                    if (ride->type == RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER)
                    {
                        ride->mode = RIDE_MODE_POWERED_LAUNCH;
                    }
                }
                break;
        }

        switch (trackType)
        {
            case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            case TRACK_ELEM_60_DEG_UP_TO_FLAT:
            case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
            case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
                if (!isLiftHill)
                    break;
                [[fallthrough]];
            case TRACK_ELEM_CABLE_LIFT_HILL:
                ride->num_block_brakes--;
                break;
        }

        money32 price = RideTrackCosts[ride->type].track_price;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            price *= FlatRideTrackPricing[trackType];
        }
        else
        {
            price *= TrackPricing[trackType];
        }
        price >>= 16;
        price = (price + cost) / 2;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
            price *= -7;
        else
            price *= -10;

        LocationXYZ16 coord;
        coord.x = startLoc.x + 16;
        coord.y = startLoc.y + 16;
        coord.z = trackpieceZ;
        network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);

        res->Cost = price;
        return res;
    }
};
