#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#pragma once

#include "../core/MemoryStream.h"
#include "../localisation/string_ids.h"
#include "GameAction.h"

#include "../cheats.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../world/park.h"
#include "../ride/track.h"
#include "../ride/track_data.h"

class RideRemoveTrackActionResult final : public GameActionResult
{
public:
    RideRemoveTrackActionResult() : GameActionResult(GA_ERROR::OK, 0) {}
    RideRemoveTrackActionResult(GA_ERROR error, rct_string_id message) : GameActionResult(error, message) {}

    sint32 x;
    sint32 y;
    sint32 z;
    sint32 direction;
    uint8 type;
    uint8 sequence;
};

struct RideRemoveTrackAction : public GameActionBase<GAME_COMMAND_REMOVE_TRACK, RideRemoveTrackActionResult>
{
private:
    sint32 _x;
    sint32 _y;
    sint32 _z;
    sint32 _direction;
    uint8 _type;
    uint8 _sequence;

public:
    RideRemoveTrackAction() {}
    RideRemoveTrackAction(sint32 x, sint32 y, sint32 z, sint32 direction, uint8 type, uint8 sequence)
        : _x(x),
        _y(y),
        _z(z),
        _direction(direction),
        _type(type),
        _sequence(sequence)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _x << _y << _z << _direction << _type << _sequence;
    }

    GameActionResult::Ptr Query() const override
    {
        return std::make_unique<GameActionResult>();
    }

    /*
    static money32 track_remove(uint8 type, uint8 sequence, sint16 originX, sint16 originY, sint16 originZ, uint8 rotation, uint8 flags)
    {
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        gCommandPosition.x = originX + 16;
        gCommandPosition.y = originY + 16;
        gCommandPosition.z = originZ;
        sint16 trackpieceZ = originZ;

        switch (type) {
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            type = TRACK_ELEM_END_STATION;
            break;
        }

        if (!(flags & (1 << 3)) && game_is_paused() && !gCheatsBuildInPauseMode) {
            gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
            return MONEY32_UNDEFINED;
        }

        uint8 found = 0;
        rct_map_element* mapElement = map_get_first_element_at(originX / 32, originY / 32);
        if (mapElement == NULL)
        {
            log_warning("Invalid coordinates for track removal. x = %d, y = %d", originX, originY);
            return MONEY32_UNDEFINED;
        }
        do {
            if (mapElement->base_height * 8 != originZ)
                continue;

            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
                continue;

            if ((map_element_get_direction(mapElement)) != rotation)
                continue;

            if (map_element_get_track_sequence(mapElement) != sequence)
                continue;

            // Probably should add a check for ghost here as well!

            uint8 track_type = mapElement->properties.track.type;
            switch (track_type) {
            case TRACK_ELEM_BEGIN_STATION:
            case TRACK_ELEM_MIDDLE_STATION:
                track_type = TRACK_ELEM_END_STATION;
                break;
            }

            if (track_type != type)
                continue;

            found = 1;
            break;
        } while (!map_element_is_last_for_tile(mapElement++));

        if (!found) {
            return MONEY32_UNDEFINED;
        }

        if (mapElement->flags & MAP_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE) {
            gGameCommandErrorText = STR_YOU_ARE_NOT_ALLOWED_TO_REMOVE_THIS_SECTION;
            return MONEY32_UNDEFINED;
        }

        uint8 rideIndex = mapElement->properties.track.ride_index;
        type = mapElement->properties.track.type;
        bool isLiftHill = track_element_is_lift_hill(mapElement);

        Ride* ride = get_ride(rideIndex);
        const rct_preview_track* trackBlock = get_track_def_from_ride(ride, type);
        trackBlock += map_element_get_track_sequence(mapElement);

        uint8 originDirection = map_element_get_direction(mapElement);
        switch (originDirection) {
        case 0:
            originX -= trackBlock->x;
            originY -= trackBlock->y;
            break;
        case 1:
            originX -= trackBlock->y;
            originY += trackBlock->x;
            break;
        case 2:
            originX += trackBlock->x;
            originY += trackBlock->y;
            break;
        case 3:
            originX += trackBlock->y;
            originY -= trackBlock->x;
            break;
        }

        originZ -= trackBlock->z;

        money32 cost = 0;

        trackBlock = get_track_def_from_ride(ride, type);
        for (; trackBlock->index != 255; trackBlock++) {
            sint16 x = originX, y = originY, z = originZ;

            switch (originDirection) {
            case 0:
                x += trackBlock->x;
                y += trackBlock->y;
                break;
            case 1:
                x += trackBlock->y;
                y -= trackBlock->x;
                break;
            case 2:
                x -= trackBlock->x;
                y -= trackBlock->y;
                break;
            case 3:
                x -= trackBlock->y;
                y += trackBlock->x;
                break;
            }

            z += trackBlock->z;

            map_invalidate_tile_full(x, y);

            trackpieceZ = z;

            found = 0;
            mapElement = map_get_first_element_at(x / 32, y / 32);
            do {
                if (mapElement == NULL)
                    break;

                if (mapElement->base_height != z / 8)
                    continue;

                if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
                    continue;

                if ((map_element_get_direction(mapElement)) != rotation)
                    continue;

                if (map_element_get_track_sequence(mapElement) != trackBlock->index)
                    continue;

                if (mapElement->properties.track.type != type)
                    continue;

                found = 1;
                break;
            } while (!map_element_is_last_for_tile(mapElement++));

            if (!found) {
                log_error("Track map element part not found!");
                return MONEY32_UNDEFINED;
            }

            sint32 entranceDirections;
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
                entranceDirections = FlatRideTrackSequenceProperties[type][0];
            }
            else {
                entranceDirections = TrackSequenceProperties[type][0];
            }

            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (map_element_get_track_sequence(mapElement) == 0)) {
                if (!track_remove_station_element(x, y, z / 8, rotation, rideIndex, 0)) {
                    return MONEY32_UNDEFINED;
                }
            }

            rct_map_element* surfaceElement = map_get_surface_element_at(x / 32, y / 32);
            if (surfaceElement == NULL) {
                return MONEY32_UNDEFINED;
            }

            sint8 _support_height = mapElement->base_height - surfaceElement->base_height;
            if (_support_height < 0) {
                _support_height = 10;
            }

            cost += (_support_height / 2) * RideTrackCosts[ride->type].support_price;

            if (!(flags & GAME_COMMAND_FLAG_APPLY))
                continue;

            if (entranceDirections & (1 << 4) && (map_element_get_track_sequence(mapElement) == 0)) {
                if (!track_remove_station_element(x, y, z / 8, rotation, rideIndex, GAME_COMMAND_FLAG_APPLY)) {
                    return MONEY32_UNDEFINED;
                }
            }

            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER)) {
                surfaceElement->type &= ~(1 << 6);
            }

            invalidate_test_results(rideIndex);
            footpath_queue_chain_reset();
            if (!gCheatsDisableClearanceChecks || !(mapElement->flags & MAP_ELEMENT_FLAG_GHOST)) {
                footpath_remove_edges_at(x, y, mapElement);
            }
            map_element_remove(mapElement);
            if (!(flags & GAME_COMMAND_FLAG_GHOST)) {
                sub_6CB945(rideIndex);
                ride_update_max_vehicles(rideIndex);
            }
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            switch (type) {
            case TRACK_ELEM_ON_RIDE_PHOTO:
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_ON_RIDE_PHOTO;
                break;
            case TRACK_ELEM_CABLE_LIFT_HILL:
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED;
                break;
            case TRACK_ELEM_BLOCK_BRAKES:
                ride->num_block_brakes--;
                if (ride->num_block_brakes == 0) {
                    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_OPERATING;
                    ride->mode = RIDE_MODE_CONTINUOUS_CIRCUIT;
                    if (ride->type == RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER) {
                        ride->mode = RIDE_MODE_POWERED_LAUNCH;
                    }
                }
                break;
            }

            switch (type) {
            case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            case TRACK_ELEM_60_DEG_UP_TO_FLAT:
            case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
            case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
                if (!isLiftHill)
                    break;
                // Fall through
            case TRACK_ELEM_CABLE_LIFT_HILL:
                ride->num_block_brakes--;
                break;
            }
        }

        money32 price = RideTrackCosts[ride->type].track_price;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
            price *= FlatRideTrackPricing[type];
        }
        else {
            price *= TrackPricing[type];
        }
        price >>= 16;
        price = (price + cost) / 2;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
            price *= -7;
        else
            price *= -10;

        if (gGameCommandNestLevel == 1) {
            rct_xyz16 coord;
            coord.x = originX + 16;
            coord.y = originY + 16;
            coord.z = trackpieceZ;
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        if (gParkFlags & PARK_FLAGS_NO_MONEY)
            return 0;
        else
            return price;
    }
    */    

    GameActionResult::Ptr Execute() const override
    {
        auto res = std::make_unique<RideRemoveTrackActionResult>();

        // We need this data in the callback so we feed forward the input.
        res->x = _x;
        res->y = _y;
        res->type = _type;
        res->direction = _direction;
        res->sequence = _sequence;

        uint8 type = _type;
        sint32 originX = _x;
        sint32 originY = _y;
        sint32 originZ = _z;
        sint16 trackpieceZ = originZ;

        res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        res->Position = { originX + 16 , originY + 16, originZ };

        switch (type) 
        {
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            type = TRACK_ELEM_END_STATION;
            break;
        }

        bool foundElem = false;

        rct_map_element* mapElement = map_get_first_element_at(originX / 32, originY / 32);
        if (mapElement == nullptr)
        {
            log_warning("Invalid coordinates for track removal. x = %d, y = %d", originX, originY);
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
            return res;
        }

        do 
        {
            if (mapElement->base_height * 8 != originZ)
                continue;

            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
                continue;

            if ((map_element_get_direction(mapElement)) != _direction)
                continue;

            if (map_element_get_track_sequence(mapElement) != _sequence)
                continue;

            // Probably should add a check for ghost here as well!

            uint8 track_type = mapElement->properties.track.type;

            switch (track_type) 
            {
            case TRACK_ELEM_BEGIN_STATION:
            case TRACK_ELEM_MIDDLE_STATION:
                track_type = TRACK_ELEM_END_STATION;
                break;
            }

            if (track_type != type)
                continue;

            foundElem = true;
            break;
        } while (!map_element_is_last_for_tile(mapElement++));

        if (foundElem == false)
        {
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
            return res;
        }

        if (mapElement->flags & MAP_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE) 
        {
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorMessage = STR_YOU_ARE_NOT_ALLOWED_TO_REMOVE_THIS_SECTION;
            return res;
        }

        uint8 rideIndex = mapElement->properties.track.ride_index;
        type = mapElement->properties.track.type;
        bool isLiftHill = track_element_is_lift_hill(mapElement);

        Ride* ride = get_ride(rideIndex);

        const rct_preview_track* trackBlock = get_track_def_from_ride(ride, type);
        trackBlock += map_element_get_track_sequence(mapElement);

        uint8 originDirection = map_element_get_direction(mapElement);
        switch (originDirection) 
        {
        case 0:
            originX -= trackBlock->x;
            originY -= trackBlock->y;
            break;
        case 1:
            originX -= trackBlock->y;
            originY += trackBlock->x;
            break;
        case 2:
            originX += trackBlock->x;
            originY += trackBlock->y;
            break;
        case 3:
            originX += trackBlock->y;
            originY -= trackBlock->x;
            break;
        }

        originZ -= trackBlock->z;

        trackBlock = get_track_def_from_ride(ride, type);
        for (; trackBlock->index != 255; trackBlock++) 
        {
            sint16 x = originX, y = originY, z = originZ;

            switch (originDirection) {
            case 0:
                x += trackBlock->x;
                y += trackBlock->y;
                break;
            case 1:
                x += trackBlock->y;
                y -= trackBlock->x;
                break;
            case 2:
                x -= trackBlock->x;
                y -= trackBlock->y;
                break;
            case 3:
                x -= trackBlock->y;
                y += trackBlock->x;
                break;
            }

            z += trackBlock->z;

            map_invalidate_tile_full(x, y);

            trackpieceZ = z;

            foundElem = false;
            mapElement = map_get_first_element_at(x / 32, y / 32);

            do 
            {
                if (mapElement == NULL)
                    break;

                if (mapElement->base_height != z / 8)
                    continue;

                if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
                    continue;

                if ((map_element_get_direction(mapElement)) != _direction)
                    continue;

                if (map_element_get_track_sequence(mapElement) != trackBlock->index)
                    continue;

                if (mapElement->properties.track.type != type)
                    continue;

                foundElem = true;
                break;
            } while (!map_element_is_last_for_tile(mapElement++));

            if (!foundElem)
            {
                log_error("Track map element part not found!");
                res->Error = GA_ERROR::INVALID_PARAMETERS;
                res->ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
                return res;
            }

            sint32 entranceDirections;
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) 
            {
                entranceDirections = FlatRideTrackSequenceProperties[type][0];
            }
            else 
            {
                entranceDirections = TrackSequenceProperties[type][0];
            }

            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (map_element_get_track_sequence(mapElement) == 0)) 
            {
                if (!track_remove_station_element(x, y, z / 8, _direction, rideIndex, 0)) 
                {
                    return res;
                }
            }

            rct_map_element* surfaceElement = map_get_surface_element_at(x / 32, y / 32);
            if (surfaceElement == nullptr) 
            {
                return res;
            }

            sint8 _support_height = mapElement->base_height - surfaceElement->base_height;
            if (_support_height < 0) {
                _support_height = 10;
            }

            res->Cost += (_support_height / 2) * RideTrackCosts[ride->type].support_price;

            if (!(GetFlags() & GAME_COMMAND_FLAG_APPLY))
                continue;

            if (entranceDirections & (1 << 4) && (map_element_get_track_sequence(mapElement) == 0)) 
            {
                if (!track_remove_station_element(x, y, z / 8, _direction, rideIndex, GAME_COMMAND_FLAG_APPLY)) 
                {
                    // Error?
                    return res;
                }
            }

            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER)) 
            {
                surfaceElement->type &= ~(1 << 6);
            }

            invalidate_test_results(rideIndex);
            footpath_queue_chain_reset();

            if (!gCheatsDisableClearanceChecks || !(mapElement->flags & MAP_ELEMENT_FLAG_GHOST)) 
            {
                footpath_remove_edges_at(x, y, mapElement);
            }
            map_element_remove(mapElement);

            if (!(GetFlags() & GAME_COMMAND_FLAG_GHOST)) 
            {
                sub_6CB945(rideIndex);
                ride_update_max_vehicles(rideIndex);
            }
        }

        if (GetFlags() & GAME_COMMAND_FLAG_APPLY) 
        {
            switch (type) 
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

            switch (type) 
            {
            case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            case TRACK_ELEM_60_DEG_UP_TO_FLAT:
            case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
            case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
                if (!isLiftHill)
                    break;
                // Fall through
            case TRACK_ELEM_CABLE_LIFT_HILL:
                ride->num_block_brakes--;
                break;
            }
        }

        money32 price = RideTrackCosts[ride->type].track_price;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) 
        {
            price *= FlatRideTrackPricing[type];
        }
        else 
        {
            price *= TrackPricing[type];
        }

        price >>= 16;
        price = (price + res->Cost) / 2;
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
            price *= -7;
        else
            price *= -10;

        res->Cost = price;
        res->Position = { originX + 16, originY + 16, trackpieceZ };

        return res;
    }
};
