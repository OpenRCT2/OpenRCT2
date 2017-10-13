#pragma region Copyright(c) 2014 - 2017 OpenRCT2 Developers
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
#include "../ride/track.h"
#include "../ride/track_data.h"
#include "../world/park.h"
#include "../util/util.h"
#include "../config/Config.h"
#include "../ride/RideGroupManager.h"

struct RidePlaceTrackAction : public GameActionBase<GAME_COMMAND_PLACE_TRACK, GameActionResult>
{
private:
    sint32 _rideIndex;
    sint32 _type;
    sint32 _originX;
    sint32 _originY;
    sint32 _originZ;
    sint32 _direction;
    sint32 _brakeSpeed;
    sint32 _colour;
    sint32 _seatRotation;
    sint32 _liftHillAndAlternativeState;

public:
    RidePlaceTrackAction() {}
    RidePlaceTrackAction(sint32 rideIndex, sint32 type, sint32 originX, sint32 originY, sint32 originZ, sint32 direction,
                         sint32 brakeSpeed, sint32 colour, sint32 seatRotation, sint32 liftHillAndAlternativeState)
        : _rideIndex(rideIndex), _type(type), _originX(originX), _originY(originY), _originZ(originZ), _direction(direction),
          _brakeSpeed(brakeSpeed), _colour(colour), _seatRotation(seatRotation),
          _liftHillAndAlternativeState(liftHillAndAlternativeState)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << _rideIndex << _type << _originX << _originY << _originZ << _direction << _brakeSpeed << _colour
               << _seatRotation << _liftHillAndAlternativeState;
    }

    GameActionResult::Ptr Query() const override
    {
        return std::make_unique<GameActionResult>();
    }

    /*
    money32 track_place(sint32 rideIndex, sint32 type, sint32 originX, sint32 originY, sint32 originZ, sint32 direction, sint32
    brakeSpeed, sint32 colour, sint32 seatRotation, sint32 liftHillAndAlternativeState, sint32 flags)
    {
        Ride *ride = get_ride(rideIndex);
        if (ride == NULL)
        {
            log_warning("Invalid ride for track placement, rideIndex = %d", rideIndex);
            return MONEY32_UNDEFINED;
        }
        if (ride->type == RIDE_TYPE_NULL) {
            log_warning("Invalid ride type, rideIndex = %d", rideIndex);
            return MONEY32_UNDEFINED;
        }
        rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
        if (rideEntry == NULL)
        {
            log_warning("Invalid ride type for track placement, rideIndex = %d", rideIndex);
            return MONEY32_UNDEFINED;
        }
        rct_map_element *mapElement;

        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        gCommandPosition.x = originX + 16;
        gCommandPosition.y = originY + 16;
        gCommandPosition.z = originZ;
        sint16 trackpieceZ = originZ;
        direction &= 3;
        gTrackGroundFlags = 0;

        uint64 enabledTrackPieces = rideEntry->enabledTrackPieces & RideTypePossibleTrackConfigurations[ride->type];
        uint32 rideTypeFlags = RideProperties[ride->type].flags;

        if ((ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK) && type == TRACK_ELEM_END_STATION) {
            gGameCommandErrorText = STR_NOT_ALLOWED_TO_MODIFY_STATION;
            return MONEY32_UNDEFINED;
        }

        if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED)) {
            if (game_is_paused() && !gCheatsBuildInPauseMode) {
                gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
                return MONEY32_UNDEFINED;
            }
        }

        const uint8(*wallEdges)[16];
        if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) {
            wallEdges = &FlatRideTrackSequenceElementAllowedWallEdges[type];
        }
        else {
            if (type == TRACK_ELEM_ON_RIDE_PHOTO) {
                if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) {
                    gGameCommandErrorText = STR_ONLY_ONE_ON_RIDE_PHOTO_PER_RIDE;
                    return MONEY32_UNDEFINED;
                }
            }
            else if (type == TRACK_ELEM_CABLE_LIFT_HILL) {
                if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED) {
                    gGameCommandErrorText = STR_ONLY_ONE_CABLE_LIFT_HILL_PER_RIDE;
                    return MONEY32_UNDEFINED;
                }
            }
            // Backwards steep lift hills are allowed, even on roller coasters that do not support forwards steep lift hills.
            if ((liftHillAndAlternativeState & CONSTRUCTION_LIFT_HILL_SELECTED) && !(enabledTrackPieces & (1ULL <<
    TRACK_LIFT_HILL_STEEP)) && !gCheatsEnableChainLiftOnAllTrack) { if (TrackFlags[type] & TRACK_ELEM_FLAG_IS_STEEP_UP) {
                    gGameCommandErrorText = STR_TOO_STEEP_FOR_LIFT_HILL;
                    return MONEY32_UNDEFINED;
                }
            }

            wallEdges = &TrackSequenceElementAllowedWallEdges[type];
        }

        money32 cost = 0;
        const rct_preview_track *trackBlock = get_track_def_from_ride(ride, type);
        uint32 num_elements = 0;
        // First check if any of the track pieces are outside the park
        for (; trackBlock->index != 0xFF; trackBlock++) {
            sint32 offsetX = 0;
            sint32 offsetY = 0;

            switch (direction) {
            case 0:
                offsetX = trackBlock->x;
                offsetY = trackBlock->y;
                break;
            case 1:
                offsetX = trackBlock->y;
                offsetY = -trackBlock->x;
                break;
            case 2:
                offsetX = -trackBlock->x;
                offsetY = -trackBlock->y;
                break;
            case 3:
                offsetX = -trackBlock->y;
                offsetY = trackBlock->x;
                break;
            }

            sint32 x = originX + offsetX;
            sint32 y = originY + offsetY;
            sint32 z = originZ + trackBlock->z;

            if (!map_is_location_owned(x, y, z) && !gCheatsSandboxMode) {
                gGameCommandErrorText = STR_LAND_NOT_OWNED_BY_PARK;
                return MONEY32_UNDEFINED;
            }
            num_elements++;
        }

        if (!map_check_free_elements_and_reorganise(num_elements)) {
            return MONEY32_UNDEFINED;
        }
        const uint16 *trackFlags = (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) ?
            FlatTrackFlags :
            TrackFlags;
        if (trackFlags[type] & TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT) {
            if ((originZ & 0x0F) != 8) {
                gGameCommandErrorText = STR_CONSTRUCTION_ERR_UNKNOWN;
                return MONEY32_UNDEFINED;
            }
        }
        else {
            if ((originZ & 0x0F) != 0) {
                gGameCommandErrorText = STR_CONSTRUCTION_ERR_UNKNOWN;
                return MONEY32_UNDEFINED;
            }
        }

        // If that is not the case, then perform the remaining checks
        trackBlock = get_track_def_from_ride(ride, type);

        for (sint32 blockIndex = 0; trackBlock->index != 0xFF; trackBlock++, blockIndex++) {
            sint32 offsetX = 0;
            sint32 offsetY = 0;
            sint32 bl = trackBlock->var_08;
            sint32 bh;
            switch (direction) {
            case 0:
                offsetX = trackBlock->x;
                offsetY = trackBlock->y;
                break;
            case 1:
                offsetX = trackBlock->y;
                offsetY = -trackBlock->x;
                bl = rol8(bl, 1);
                bh = bl;
                bh = ror8(bh, 4);
                bl &= 0xEE;
                bh &= 0x11;
                bl |= bh;
                break;
            case 2:
                offsetX = -trackBlock->x;
                offsetY = -trackBlock->y;
                bl = rol8(bl, 2);
                bh = bl;
                bh = ror8(bh, 4);
                bl &= 0xCC;
                bh &= 0x33;
                bl |= bh;
                break;
            case 3:
                offsetX = -trackBlock->y;
                offsetY = trackBlock->x;
                bl = rol8(bl, 3);
                bh = bl;
                bh = ror8(bh, 4);
                bl &= 0x88;
                bh &= 0x77;
                bl |= bh;
                break;
            }
            sint32 x = originX + offsetX;
            sint32 y = originY + offsetY;
            sint32 z = originZ + trackBlock->z;

            trackpieceZ = z;

            if (z < 16) {
                gGameCommandErrorText = STR_TOO_LOW;
                return MONEY32_UNDEFINED;
            }

            sint32 baseZ = (originZ + trackBlock->z) / 8;

            sint32 clearanceZ = trackBlock->var_07;
            if (trackBlock->var_09 & (1 << 2) && RideData5[ride->type].clearance_height > 24) {
                clearanceZ += 24;
            }
            else {
                clearanceZ += RideData5[ride->type].clearance_height;
            }

            clearanceZ = (clearanceZ / 8) + baseZ;

            if (clearanceZ >= 255) {
                gGameCommandErrorText = STR_TOO_HIGH;
                return MONEY32_UNDEFINED;
            }

            _currentTrackEndX = x;
            _currentTrackEndY = y;

            if (!gCheatsDisableClearanceChecks || flags & GAME_COMMAND_FLAG_GHOST) {
                if (!map_can_construct_with_clear_at(x, y, baseZ, clearanceZ, &map_place_non_scenery_clear_func, bl, flags,
    &cost)) return MONEY32_UNDEFINED;
            }

            //6c53dc

            if ((flags & GAME_COMMAND_FLAG_APPLY) && !(flags & GAME_COMMAND_FLAG_GHOST) && !gCheatsDisableClearanceChecks) {
                footpath_remove_litter(x, y, z);
                if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_NO_WALLS) {
                    wall_remove_at(x, y, baseZ * 8, clearanceZ * 8);
                }
                else {
                    // Remove walls in the directions this track intersects
                    uint8 intersectingDirections = (*wallEdges)[blockIndex];
                    intersectingDirections ^= 0x0F;
                    for (sint32 i = 0; i < 4; i++) {
                        if (intersectingDirections & (1 << i)) {
                            wall_remove_intersecting_walls(x, y, baseZ, clearanceZ, i);
                        }
                    }
                }
            }

            bh = gMapGroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
            if (gTrackGroundFlags != 0 && (gTrackGroundFlags & bh) == 0) {
                gGameCommandErrorText = STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND;
                return MONEY32_UNDEFINED;
            }

            gTrackGroundFlags = bh;
            if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) {
                if (FlatTrackFlags[type] & TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND) {
                    if (gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND) {
                        gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                        return MONEY32_UNDEFINED;
                    }
                }
            }
            else {
                if (TrackFlags[type] & TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND) {
                    if (gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND) {
                        gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                        return MONEY32_UNDEFINED;
                    }
                }
            }

            if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) {
                if (FlatTrackFlags[type] & TRACK_ELEM_FLAG_ONLY_UNDERWATER) {
                    if (!(gMapGroundFlags & ELEMENT_IS_UNDERWATER)) {
                        gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_UNDERWATER;
                        return MONEY32_UNDEFINED;
                    }
                }
            }
            else {
                if (TrackFlags[type] & TRACK_ELEM_FLAG_ONLY_UNDERWATER) { // No element has this flag
                    if (gMapGroundFlags & ELEMENT_IS_UNDERWATER) {
                        gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_UNDERWATER;
                        return MONEY32_UNDEFINED;
                    }
                }
            }

            if (gMapGroundFlags & ELEMENT_IS_UNDERWATER) {
                gGameCommandErrorText = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
                return MONEY32_UNDEFINED;
            }

            if ((rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER) && !byte_9D8150) {
                mapElement = map_get_surface_element_at(x / 32, y / 32);

                uint8 water_height = map_get_water_height(mapElement) * 2;
                if (water_height == 0) {
                    gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
                    return MONEY32_UNDEFINED;
                }

                if (water_height != baseZ) {
                    gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
                    return MONEY32_UNDEFINED;
                }
                water_height -= 2;
                if (water_height == mapElement->base_height) {
                    bh = mapElement->properties.surface.slope & 0x0F;
                    if (bh == 7 || bh == 11 || bh == 13 || bh == 14) {
                        gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
                        return MONEY32_UNDEFINED;
                    }
                }
            }

            sint32 entranceDirections;
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
                entranceDirections = FlatRideTrackSequenceProperties[type][0];
            }
            else {
                entranceDirections = TrackSequenceProperties[type][0];
            }
            if ((entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN) && trackBlock->index == 0) {
                if (!track_add_station_element(x, y, baseZ, direction, rideIndex, 0)) {
                    return MONEY32_UNDEFINED;
                }
            }
            //6c55be
            if (entranceDirections & TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH) {
                entranceDirections &= 0x0F;
                if (entranceDirections != 0) {
                    if (!(flags & GAME_COMMAND_FLAG_APPLY) && !(flags & GAME_COMMAND_FLAG_GHOST) &&
    !gCheatsDisableClearanceChecks) { uint8 _bl = entranceDirections; for (sint32 dl = bitscanforward(_bl); dl != -1; dl =
    bitscanforward(_bl)) { _bl &= ~(1 << dl); sint32 temp_x = x, temp_y = y; sint32 temp_direction = (direction + dl) & 3;
                            temp_x += TileDirectionDelta[temp_direction].x;
                            temp_y += TileDirectionDelta[temp_direction].y;
                            temp_direction ^= (1 << 1);
                            wall_remove_intersecting_walls(temp_x, temp_y, baseZ, clearanceZ, temp_direction & 3);
                        }
                    }
                }
            }
            //6c5648 12 push
            mapElement = map_get_surface_element_at(x / 32, y / 32);
            if (!gCheatsDisableSupportLimits) {
                sint32 ride_height = clearanceZ - mapElement->base_height;
                if (ride_height >= 0) {

                    uint16 maxHeight;
                    if (gConfigInterface.select_by_track_type) {
                        if (ride_type_has_ride_groups(ride->type)) {
                            const RideGroup * rideGroup = get_ride_group(ride->type, rideEntry);
                            maxHeight = rideGroup->MaximumHeight;
                        }
                        else {
                            maxHeight = RideData5[ride->type].max_height;
                        }
                    }
                    else {
                        maxHeight = rideEntry->max_height;
                        // If a rideEntry specifies 0 as max height, it means OpenRCT2
                        // should fall back on the default for the track type.
                        if (maxHeight == 0) {
                            maxHeight = RideData5[ride->type].max_height;
                        }
                    }

                    ride_height /= 2;
                    if (ride_height > maxHeight && !byte_9D8150) {
                        gGameCommandErrorText = STR_TOO_HIGH_FOR_SUPPORTS;
                        return MONEY32_UNDEFINED;
                    }
                }
            }

            sint32 _support_height = baseZ - mapElement->base_height;
            if (_support_height < 0) {
                _support_height = 10;
            }

            cost += ((_support_height / 2) * RideTrackCosts[ride->type].support_price) * 5;

            //6c56d3

            if (!(flags & GAME_COMMAND_FLAG_APPLY))
                continue;

            invalidate_test_results(rideIndex);
            switch (type) {
            case TRACK_ELEM_ON_RIDE_PHOTO:
                ride->lifecycle_flags |= RIDE_LIFECYCLE_ON_RIDE_PHOTO;
                break;
            case TRACK_ELEM_CABLE_LIFT_HILL:
                if (trackBlock->index != 0)
                    break;
                ride->lifecycle_flags |= RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED;
                ride->cable_lift_x = x;
                ride->cable_lift_y = y;
                ride->cable_lift_z = baseZ;
                break;
            case TRACK_ELEM_BLOCK_BRAKES:
                ride->num_block_brakes++;
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_OPERATING;
                ride->mode = RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED;
                if (ride->type == RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER)
                    ride->mode = RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED;
                break;
            }

            if (trackBlock->index == 0) {
                switch (type) {
                case TRACK_ELEM_25_DEG_UP_TO_FLAT:
                case TRACK_ELEM_60_DEG_UP_TO_FLAT:
                case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
                case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
                    if (!(liftHillAndAlternativeState & CONSTRUCTION_LIFT_HILL_SELECTED))
                        break;
                    //Fall Through
                case TRACK_ELEM_CABLE_LIFT_HILL:
                    ride->num_block_brakes++;
                    break;
                }
            }

            entranceDirections = 0;
            if (ride->overall_view.xy != RCT_XY8_UNDEFINED) {
                if (!(flags & GAME_COMMAND_FLAG_5)) {
                    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
                        entranceDirections = FlatRideTrackSequenceProperties[type][0];
                    }
                    else {
                        entranceDirections = TrackSequenceProperties[type][0];
                    }
                }
            }
            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN || ride->overall_view.xy == RCT_XY8_UNDEFINED) {
                ride->overall_view.x = x / 32;
                ride->overall_view.y = y / 32;
            }

            mapElement = map_element_insert(x / 32, y / 32, baseZ, bl & 0xF);
            assert(mapElement != NULL);
            mapElement->clearance_height = clearanceZ;

            uint8 map_type = direction;
            map_type |= MAP_ELEMENT_TYPE_TRACK;
            if (liftHillAndAlternativeState & CONSTRUCTION_LIFT_HILL_SELECTED) {
                map_type |= (1 << 7);
            }
            mapElement->type = map_type;

            map_element_set_track_sequence(mapElement, trackBlock->index);
            mapElement->properties.track.ride_index = rideIndex;
            mapElement->properties.track.type = type;
            mapElement->properties.track.colour = 0;
            if (flags & GAME_COMMAND_FLAG_GHOST)
            {
                mapElement->flags |= MAP_ELEMENT_FLAG_GHOST;
            }

            switch (type) {
            case TRACK_ELEM_WATERFALL:
                map_animation_create(MAP_ANIMATION_TYPE_TRACK_WATERFALL, x, y, mapElement->base_height);
                break;
            case TRACK_ELEM_RAPIDS:
                map_animation_create(MAP_ANIMATION_TYPE_TRACK_RAPIDS, x, y, mapElement->base_height);
                break;
            case TRACK_ELEM_WHIRLPOOL:
                map_animation_create(MAP_ANIMATION_TYPE_TRACK_WHIRLPOOL, x, y, mapElement->base_height);
                break;
            case TRACK_ELEM_SPINNING_TUNNEL:
                map_animation_create(MAP_ANIMATION_TYPE_TRACK_SPINNINGTUNNEL, x, y, mapElement->base_height);
                break;
            }
            if (track_element_has_speed_setting(type))
            {
                map_element_set_brake_booster_speed(mapElement, brakeSpeed);
            }
            else
            {
                track_element_set_seat_rotation(mapElement, seatRotation);
            }

            if (liftHillAndAlternativeState & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE)
            {
                track_element_set_inverted(mapElement, true);
            }
            track_element_set_colour_scheme(mapElement, colour);

            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
                entranceDirections = FlatRideTrackSequenceProperties[type][0];
            }
            else {
                entranceDirections = TrackSequenceProperties[type][0];
            }

            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN) {
                if (trackBlock->index == 0) {
                    track_add_station_element(x, y, baseZ, direction, rideIndex, GAME_COMMAND_FLAG_APPLY);
                }
                sub_6CB945(rideIndex);
                ride_update_max_vehicles(rideIndex);
            }

            if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER) {
                rct_map_element* surfaceElement = map_get_surface_element_at(x / 32, y / 32);
                surfaceElement->type |= (1 << 6);
                mapElement = surfaceElement;
            }

            if (!gCheatsDisableClearanceChecks || !(flags & GAME_COMMAND_FLAG_GHOST)) {
                footpath_connect_edges(x, y, mapElement, flags);
            }
            map_invalidate_tile_full(x, y);
        }

        if (gGameCommandNestLevel == 1) {
            rct_xyz16 coord;
            coord.x = originX + 16;
            coord.y = originY + 16;
            coord.z = trackpieceZ;
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        money32 price = RideTrackCosts[ride->type].track_price;
        price *= (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) ?
            FlatRideTrackPricing[type] :
            TrackPricing[type];

        price >>= 16;
        price = cost + ((price / 2) * 10);

        if (gParkFlags & PARK_FLAGS_NO_MONEY) {
            return 0;
        }
        else {
            return price;
        }
    }
    */

    GameActionResult::Ptr Execute() const override
    {
        auto res = std::make_unique<RideRemoveTrackActionResult>();

        uint32 flags = GetFlags();

        Ride * ride = get_ride(_rideIndex);
        if (ride == NULL)
        {
            log_warning("Invalid ride for track placement, rideIndex = %d", _rideIndex);
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorMessage = STR_EMPTY;
            return res;
        }

        if (ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid ride type, rideIndex = %d", _rideIndex);
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorMessage = STR_EMPTY;
            return res;
        }

        rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
        if (rideEntry == NULL)
        {
            log_warning("Invalid ride type for track placement, rideIndex = %d", _rideIndex);
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorMessage = STR_EMPTY;
            return res;
        }

        rct_map_element * mapElement;

        res->ExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        res->Position = { _originX + 16, _originY + 16, _originZ };

        sint16 trackpieceZ = _originZ;
        sint16 direction = _direction & 3;

        gTrackGroundFlags = 0;

        uint64 enabledTrackPieces = rideEntry->enabledTrackPieces & RideTypePossibleTrackConfigurations[ride->type];
        uint32 rideTypeFlags      = RideProperties[ride->type].flags;

        if ((ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK) && _type == TRACK_ELEM_END_STATION)
        {
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorMessage = STR_NOT_ALLOWED_TO_MODIFY_STATION;
            return res;
        }

        const uint8(*wallEdges)[16];
        if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE)
        {
            wallEdges = &FlatRideTrackSequenceElementAllowedWallEdges[_type];
        }
        else
        {
            if (_type == TRACK_ELEM_ON_RIDE_PHOTO)
            {
                if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
                {
                    res->Error = GA_ERROR::INVALID_PARAMETERS;
                    res->ErrorMessage = STR_ONLY_ONE_ON_RIDE_PHOTO_PER_RIDE;
                    return res;
                }
            }
            else if (_type == TRACK_ELEM_CABLE_LIFT_HILL)
            {
                if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED)
                {
                    res->Error = GA_ERROR::INVALID_PARAMETERS;
                    res->ErrorMessage = STR_ONLY_ONE_CABLE_LIFT_HILL_PER_RIDE;
                    return res;
                }
            }
            // Backwards steep lift hills are allowed, even on roller coasters that do not support forwards steep lift hills.
            if ((_liftHillAndAlternativeState & CONSTRUCTION_LIFT_HILL_SELECTED) &&
                !(enabledTrackPieces & (1ULL << TRACK_LIFT_HILL_STEEP)) && !gCheatsEnableChainLiftOnAllTrack)
            {
                if (TrackFlags[_type] & TRACK_ELEM_FLAG_IS_STEEP_UP)
                {
                    res->Error = GA_ERROR::INVALID_PARAMETERS;
                    res->ErrorMessage = STR_TOO_STEEP_FOR_LIFT_HILL;
                    return res;
                }
            }

            wallEdges = &TrackSequenceElementAllowedWallEdges[_type];
        }

        money32                   cost         = 0;
        const rct_preview_track * trackBlock   = get_track_def_from_ride(ride, _type);
        uint32                    num_elements = 0;
        // First check if any of the track pieces are outside the park
        for (; trackBlock->index != 0xFF; trackBlock++)
        {
            sint32 offsetX = 0;
            sint32 offsetY = 0;

            switch (direction)
            {
            case 0:
                offsetX = trackBlock->x;
                offsetY = trackBlock->y;
                break;
            case 1:
                offsetX = trackBlock->y;
                offsetY = -trackBlock->x;
                break;
            case 2:
                offsetX = -trackBlock->x;
                offsetY = -trackBlock->y;
                break;
            case 3:
                offsetX = -trackBlock->y;
                offsetY = trackBlock->x;
                break;
            }

            sint32 x = _originX + offsetX;
            sint32 y = _originY + offsetY;
            sint32 z = _originZ + trackBlock->z;

            if (!map_is_location_owned(x, y, z) && !gCheatsSandboxMode)
            {
                res->Error = GA_ERROR::NOT_OWNED;
                res->ErrorMessage = STR_LAND_NOT_OWNED_BY_PARK;
                return res;
            }
            num_elements++;
        }

        if (!map_check_free_elements_and_reorganise(num_elements))
        {
            res->Error = GA_ERROR::UNKNOWN;
            return res;
        }

        const uint16 * trackFlags = (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) ? FlatTrackFlags : TrackFlags;
        if (trackFlags[_type] & TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT)
        {
            if ((_originZ & 0x0F) != 8)
            {
                res->Error = GA_ERROR::UNKNOWN;
                return res;
            }
        }
        else
        {
            if ((_originZ & 0x0F) != 0)
            {
                res->Error = GA_ERROR::UNKNOWN;
                return res;
            }
        }

        // If that is not the case, then perform the remaining checks
        trackBlock = get_track_def_from_ride(ride, _type);

        for (sint32 blockIndex = 0; trackBlock->index != 0xFF; trackBlock++, blockIndex++)
        {
            sint32 offsetX = 0;
            sint32 offsetY = 0;
            sint32 bl      = trackBlock->var_08;
            sint32 bh;
            switch (direction)
            {
            case 0:
                offsetX = trackBlock->x;
                offsetY = trackBlock->y;
                break;
            case 1:
                offsetX = trackBlock->y;
                offsetY = -trackBlock->x;
                bl      = rol8(bl, 1);
                bh      = bl;
                bh      = ror8(bh, 4);
                bl &= 0xEE;
                bh &= 0x11;
                bl |= bh;
                break;
            case 2:
                offsetX = -trackBlock->x;
                offsetY = -trackBlock->y;
                bl      = rol8(bl, 2);
                bh      = bl;
                bh      = ror8(bh, 4);
                bl &= 0xCC;
                bh &= 0x33;
                bl |= bh;
                break;
            case 3:
                offsetX = -trackBlock->y;
                offsetY = trackBlock->x;
                bl      = rol8(bl, 3);
                bh      = bl;
                bh      = ror8(bh, 4);
                bl &= 0x88;
                bh &= 0x77;
                bl |= bh;
                break;
            }
            sint32 x = _originX + offsetX;
            sint32 y = _originY + offsetY;
            sint32 z = _originZ + trackBlock->z;

            trackpieceZ = z;

            if (z < 16)
            {
                res->Error = GA_ERROR::INVALID_PARAMETERS;
                res->ErrorMessage = STR_TOO_LOW;
                return res;
            }

            sint32 baseZ = (_originZ + trackBlock->z) / 8;

            sint32 clearanceZ = trackBlock->var_07;
            if (trackBlock->var_09 & (1 << 2) && RideData5[ride->type].clearance_height > 24)
            {
                clearanceZ += 24;
            }
            else
            {
                clearanceZ += RideData5[ride->type].clearance_height;
            }

            clearanceZ = (clearanceZ / 8) + baseZ;

            if (clearanceZ >= 255)
            {
                res->Error = GA_ERROR::INVALID_PARAMETERS;
                res->ErrorMessage = STR_TOO_HIGH;
                return res;
            }

            _currentTrackEndX = x;
            _currentTrackEndY = y;

            if (!gCheatsDisableClearanceChecks || flags & GAME_COMMAND_FLAG_GHOST)
            {
                if (!map_can_construct_with_clear_at(x, y, baseZ, clearanceZ, &map_place_non_scenery_clear_func, bl, flags,
                    &cost))
                {
                    res->Error = GA_ERROR::UNKNOWN;
                    return res;
                }
            }

            // 6c53dc

            if ((flags & GAME_COMMAND_FLAG_APPLY) && !(flags & GAME_COMMAND_FLAG_GHOST) && !gCheatsDisableClearanceChecks)
            {
                footpath_remove_litter(x, y, z);
                if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_NO_WALLS)
                {
                    wall_remove_at(x, y, baseZ * 8, clearanceZ * 8);
                }
                else
                {
                    // Remove walls in the directions this track intersects
                    uint8 intersectingDirections = (*wallEdges)[blockIndex];
                    intersectingDirections ^= 0x0F;
                    for (sint32 i = 0; i < 4; i++)
                    {
                        if (intersectingDirections & (1 << i))
                        {
                            wall_remove_intersecting_walls(x, y, baseZ, clearanceZ, i);
                        }
                    }
                }
            }

            bh = gMapGroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
            if (gTrackGroundFlags != 0 && (gTrackGroundFlags & bh) == 0)
            {
                res->Error = GA_ERROR::INVALID_PARAMETERS;
                res->ErrorMessage = STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND;
                return res;
            }

            gTrackGroundFlags = bh;
            if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE)
            {
                if (FlatTrackFlags[_type] & TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND)
                {
                    if (gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND)
                    {
                        res->Error = GA_ERROR::INVALID_PARAMETERS;
                        res->ErrorMessage = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                        return res;
                    }
                }
            }
            else
            {
                if (TrackFlags[_type] & TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND)
                {
                    if (gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND)
                    {
                        res->Error = GA_ERROR::INVALID_PARAMETERS;
                        res->ErrorMessage = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                        return res;
                    }
                }
            }

            if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE)
            {
                if (FlatTrackFlags[_type] & TRACK_ELEM_FLAG_ONLY_UNDERWATER)
                {
                    if (!(gMapGroundFlags & ELEMENT_IS_UNDERWATER))
                    {
                        res->Error = GA_ERROR::INVALID_PARAMETERS;
                        res->ErrorMessage = STR_CAN_ONLY_BUILD_THIS_UNDERWATER;
                        return res;
                    }
                }
            }
            else
            {
                if (TrackFlags[_type] & TRACK_ELEM_FLAG_ONLY_UNDERWATER)
                { // No element has this flag
                    if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
                    {
                        res->Error = GA_ERROR::INVALID_PARAMETERS;
                        res->ErrorMessage = STR_CAN_ONLY_BUILD_THIS_UNDERWATER;
                        return res;
                    }
                }
            }

            if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
            {
                res->Error = GA_ERROR::INVALID_PARAMETERS;
                res->ErrorMessage = STR_CAN_ONLY_BUILD_THIS_UNDERWATER;
                return res;
            }

            if ((rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER) && !byte_9D8150)
            {
                mapElement = map_get_surface_element_at(x / 32, y / 32);

                uint8 water_height = map_get_water_height(mapElement) * 2;
                if (water_height == 0)
                {
                    res->Error = GA_ERROR::INVALID_PARAMETERS;
                    res->ErrorMessage = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
                    return res;
                }

                if (water_height != baseZ)
                {
                    res->Error = GA_ERROR::INVALID_PARAMETERS;
                    res->ErrorMessage = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
                    return res;
                }

                water_height -= 2;
                if (water_height == mapElement->base_height)
                {
                    bh = mapElement->properties.surface.slope & 0x0F;
                    if (bh == 7 || bh == 11 || bh == 13 || bh == 14)
                    {
                        res->Error = GA_ERROR::INVALID_PARAMETERS;
                        res->ErrorMessage = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
                        return res;
                    }
                }
            }

            sint32 entranceDirections;
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                entranceDirections = FlatRideTrackSequenceProperties[_type][0];
            }
            else
            {
                entranceDirections = TrackSequenceProperties[_type][0];
            }
            if ((entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN) && trackBlock->index == 0)
            {
                if (!track_add_station_element(x, y, baseZ, direction, _rideIndex, 0))
                {
                    res->Error = GA_ERROR::UNKNOWN;
                    return res;
                }
            }
            // 6c55be
            if (entranceDirections & TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH)
            {
                entranceDirections &= 0x0F;
                if (entranceDirections != 0)
                {
                    if (!(flags & GAME_COMMAND_FLAG_APPLY) && !(flags & GAME_COMMAND_FLAG_GHOST) &&
                        !gCheatsDisableClearanceChecks)
                    {
                        uint8 _bl = entranceDirections;
                        for (sint32 dl = bitscanforward(_bl); dl != -1; dl = bitscanforward(_bl))
                        {
                            _bl &= ~(1 << dl);
                            sint32 temp_x = x, temp_y = y;
                            sint32 temp_direction = (direction + dl) & 3;
                            temp_x += TileDirectionDelta[temp_direction].x;
                            temp_y += TileDirectionDelta[temp_direction].y;
                            temp_direction ^= (1 << 1);
                            wall_remove_intersecting_walls(temp_x, temp_y, baseZ, clearanceZ, temp_direction & 3);
                        }
                    }
                }
            }
            // 6c5648 12 push
            mapElement = map_get_surface_element_at(x / 32, y / 32);
            if (!gCheatsDisableSupportLimits)
            {
                sint32 ride_height = clearanceZ - mapElement->base_height;
                if (ride_height >= 0)
                {

                    uint16 maxHeight;
                    if (gConfigInterface.select_by_track_type)
                    {
                        if (ride_type_has_ride_groups(ride->type))
                        {
                            const RideGroup * rideGroup = get_ride_group(ride->type, rideEntry);
                            maxHeight                   = rideGroup->MaximumHeight;
                        }
                        else
                        {
                            maxHeight = RideData5[ride->type].max_height;
                        }
                    }
                    else
                    {
                        maxHeight = rideEntry->max_height;
                        // If a rideEntry specifies 0 as max height, it means OpenRCT2
                        // should fall back on the default for the track type.
                        if (maxHeight == 0)
                        {
                            maxHeight = RideData5[ride->type].max_height;
                        }
                    }

                    ride_height /= 2;
                    if (ride_height > maxHeight && !byte_9D8150)
                    {
                        res->Error = GA_ERROR::INVALID_PARAMETERS;
                        res->ErrorMessage = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
                        return res;
                    }
                }
            }

            sint32 _support_height = baseZ - mapElement->base_height;
            if (_support_height < 0)
            {
                _support_height = 10;
            }

            cost += ((_support_height / 2) * RideTrackCosts[ride->type].support_price) * 5;

            // 6c56d3

            if (!(flags & GAME_COMMAND_FLAG_APPLY))
                continue;

            invalidate_test_results(_rideIndex);
            switch (_type)
            {
            case TRACK_ELEM_ON_RIDE_PHOTO:
                ride->lifecycle_flags |= RIDE_LIFECYCLE_ON_RIDE_PHOTO;
                break;
            case TRACK_ELEM_CABLE_LIFT_HILL:
                if (trackBlock->index != 0)
                    break;
                ride->lifecycle_flags |= RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED;
                ride->cable_lift_x = x;
                ride->cable_lift_y = y;
                ride->cable_lift_z = baseZ;
                break;
            case TRACK_ELEM_BLOCK_BRAKES:
                ride->num_block_brakes++;
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_OPERATING;
                ride->mode = RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED;
                if (ride->type == RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER)
                    ride->mode = RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED;
                break;
            }

            if (trackBlock->index == 0)
            {
                switch (_type)
                {
                case TRACK_ELEM_25_DEG_UP_TO_FLAT:
                case TRACK_ELEM_60_DEG_UP_TO_FLAT:
                case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
                case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
                    if (!(_liftHillAndAlternativeState & CONSTRUCTION_LIFT_HILL_SELECTED))
                        break;
                    // Fall Through
                case TRACK_ELEM_CABLE_LIFT_HILL:
                    ride->num_block_brakes++;
                    break;
                }
            }

            entranceDirections = 0;
            if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
            {
                if (!(flags & GAME_COMMAND_FLAG_5))
                {
                    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
                    {
                        entranceDirections = FlatRideTrackSequenceProperties[_type][0];
                    }
                    else
                    {
                        entranceDirections = TrackSequenceProperties[_type][0];
                    }
                }
            }
            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN || ride->overall_view.xy == RCT_XY8_UNDEFINED)
            {
                ride->overall_view.x = x / 32;
                ride->overall_view.y = y / 32;
            }

            mapElement = map_element_insert(x / 32, y / 32, baseZ, bl & 0xF);
            assert(mapElement != NULL);
            mapElement->clearance_height = clearanceZ;

            uint8 map_type = direction;
            map_type |= MAP_ELEMENT_TYPE_TRACK;
            if (_liftHillAndAlternativeState & CONSTRUCTION_LIFT_HILL_SELECTED)
            {
                map_type |= (1 << 7);
            }
            mapElement->type = map_type;

            map_element_set_track_sequence(mapElement, trackBlock->index);
            mapElement->properties.track.ride_index = _rideIndex;
            mapElement->properties.track.type       = _type;
            mapElement->properties.track.colour     = 0;
            if (flags & GAME_COMMAND_FLAG_GHOST)
            {
                mapElement->flags |= MAP_ELEMENT_FLAG_GHOST;
            }

            switch (_type)
            {
            case TRACK_ELEM_WATERFALL:
                map_animation_create(MAP_ANIMATION_TYPE_TRACK_WATERFALL, x, y, mapElement->base_height);
                break;
            case TRACK_ELEM_RAPIDS:
                map_animation_create(MAP_ANIMATION_TYPE_TRACK_RAPIDS, x, y, mapElement->base_height);
                break;
            case TRACK_ELEM_WHIRLPOOL:
                map_animation_create(MAP_ANIMATION_TYPE_TRACK_WHIRLPOOL, x, y, mapElement->base_height);
                break;
            case TRACK_ELEM_SPINNING_TUNNEL:
                map_animation_create(MAP_ANIMATION_TYPE_TRACK_SPINNINGTUNNEL, x, y, mapElement->base_height);
                break;
            }
            if (track_element_has_speed_setting(_type))
            {
                map_element_set_brake_booster_speed(mapElement, _brakeSpeed);
            }
            else
            {
                track_element_set_seat_rotation(mapElement, _seatRotation);
            }

            if (_liftHillAndAlternativeState & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE)
            {
                track_element_set_inverted(mapElement, true);
            }
            track_element_set_colour_scheme(mapElement, _colour);

            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                entranceDirections = FlatRideTrackSequenceProperties[_type][0];
            }
            else
            {
                entranceDirections = TrackSequenceProperties[_type][0];
            }

            if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN)
            {
                if (trackBlock->index == 0)
                {
                    track_add_station_element(x, y, baseZ, direction, _rideIndex, GAME_COMMAND_FLAG_APPLY);
                }
                sub_6CB945(_rideIndex);
                ride_update_max_vehicles(_rideIndex);
            }

            if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER)
            {
                rct_map_element * surfaceElement = map_get_surface_element_at(x / 32, y / 32);
                surfaceElement->type |= (1 << 6);
                mapElement = surfaceElement;
            }

            if (!gCheatsDisableClearanceChecks || !(flags & GAME_COMMAND_FLAG_GHOST))
            {
                footpath_connect_edges(x, y, mapElement, flags);
            }
            map_invalidate_tile_full(x, y);
        }

        res->Position.z = trackpieceZ;

        money32 price = RideTrackCosts[ride->type].track_price;
        price *= (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) ? FlatRideTrackPricing[_type] : TrackPricing[_type];

        price >>= 16;
        price = cost + ((price / 2) * 10);

        res->Cost = price;

        return res;
    }
};
