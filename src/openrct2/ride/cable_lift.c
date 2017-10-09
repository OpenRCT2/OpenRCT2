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

#include "../rct12.h"
#include "../world/sprite.h"
#include "cable_lift.h"
#include "ride.h"
#include "track.h"
#include "ride_data.h"
#include "vehicle_data.h"

static void cable_lift_update_moving_to_end_of_station(rct_vehicle *vehicle);
static void cable_lift_update_waiting_to_depart(rct_vehicle *vehicle);
static void cable_lift_update_departing(rct_vehicle *vehicle);
static void cable_lift_update_travelling(rct_vehicle *vehicle);
static void cable_lift_update_arriving(rct_vehicle *vehicle);

rct_vehicle *cable_lift_segment_create(sint32 rideIndex, sint32 x, sint32 y, sint32 z, sint32 direction, uint16 var_44, sint32 remaining_distance, bool head)
{
    Ride *ride = get_ride(rideIndex);
    rct_vehicle *current = &(create_sprite(1)->vehicle);
    current->sprite_identifier = SPRITE_IDENTIFIER_VEHICLE;
    current->ride = rideIndex;
    current->ride_subtype = RIDE_ENTRY_INDEX_NULL;
    if (head) {
        move_sprite_to_list((rct_sprite*)current, SPRITE_LIST_TRAIN * 2);
        ride->cable_lift = current->sprite_index;
    }
    current->is_child = head ? 0 : 1;
    current->var_44 = var_44;
    current->remaining_distance = remaining_distance;
    current->sprite_width = 10;
    current->sprite_height_negative = 10;
    current->sprite_height_positive = 10;
    current->friction = 100;
    current->num_seats = 0;
    current->speed = 20;
    current->powered_acceleration = 80;
    current->velocity = 0;
    current->acceleration = 0;
    current->var_4A = 0;
    current->swinging_car_var_0 = 0;
    current->var_4E = 0;
    current->restraints_position = 0;
    current->var_BA = 0;
    current->var_B6 = 0;
    current->var_B8 = 0;
    current->sound1_id = RCT12_SOUND_ID_NULL;
    current->sound2_id = RCT12_SOUND_ID_NULL;
    current->var_C4 = 0;
    current->var_C5 = 0;
    current->var_C8 = 0;
    current->var_CA = 0;
    current->scream_sound_id = 0xFF;
    current->vehicle_sprite_type = 0;
    current->bank_rotation = 0;
    for (sint32 j = 0; j < 32; j++) {
        current->peep[j] = SPRITE_INDEX_NULL;
    }
    current->var_CD = 0;
    current->sprite_direction = direction << 3;
    current->track_x = x;
    current->track_y = y;

    z = z * 8;
    current->track_z = z;
    z += RideData5[ride->type].z_offset;

    sprite_move(16, 16, z, (rct_sprite*)current);
    current->track_type = (TRACK_ELEM_CABLE_LIFT_HILL << 2) | (current->sprite_direction >> 3);
    current->track_progress = 164;
    current->update_flags = VEHICLE_UPDATE_FLAG_1;
    current->status = VEHICLE_STATUS_MOVING_TO_END_OF_STATION;
    current->sub_state = 0;
    current->num_peeps = 0;
    current->next_free_seat = 0;
    return current;
}

void cable_lift_update(rct_vehicle *vehicle)
{
    switch (vehicle->status) {
    case VEHICLE_STATUS_MOVING_TO_END_OF_STATION:
        cable_lift_update_moving_to_end_of_station(vehicle);
        break;
    case VEHICLE_STATUS_WAITING_FOR_PASSENGERS:
        // Stays in this state until a train puts it into next state
        break;
    case VEHICLE_STATUS_WAITING_TO_DEPART:
        cable_lift_update_waiting_to_depart(vehicle);
        break;
    case VEHICLE_STATUS_DEPARTING:
        cable_lift_update_departing(vehicle);
        break;
    case VEHICLE_STATUS_TRAVELLING:
        cable_lift_update_travelling(vehicle);
        break;
    case VEHICLE_STATUS_ARRIVING:
        cable_lift_update_arriving(vehicle);
        break;
    }
}

/**
 *
 *  rct2: 0x006DF8A4
 */
static void cable_lift_update_moving_to_end_of_station(rct_vehicle *vehicle)
{
    if (vehicle->velocity >= -439800)
        vehicle->acceleration = -2932;

    if (vehicle->velocity < -439800) {
        vehicle->velocity -= vehicle->velocity / 16;
        vehicle->acceleration = 0;
    }

    if (!(cable_lift_update_track_motion(vehicle) & (1 << 0)))
        return;

    vehicle->velocity = 0;
    vehicle->acceleration = 0;
    vehicle->status = VEHICLE_STATUS_WAITING_FOR_PASSENGERS;
}

/**
 *
 *  rct2: 0x006DF8F1
 */
static void cable_lift_update_waiting_to_depart(rct_vehicle *vehicle)
{
    if (vehicle->velocity >= -58640)
        vehicle->acceleration = -14660;

    if (vehicle->velocity < -58640) {
        vehicle->velocity -= vehicle->velocity / 16;
        vehicle->acceleration = 0;
    }

    cable_lift_update_track_motion(vehicle);

    // Next check to see if the second part of the cable lift
    // is at the front of the passenger vehicle to simulate the
    // cable being attached underneath the train.
    rct_vehicle* passengerVehicle = GET_VEHICLE(vehicle->cable_lift_target);
    rct_vehicle* cableLiftSecondPart = GET_VEHICLE(vehicle->prev_vehicle_on_ride);

    sint16 dist_x = abs(passengerVehicle->x - cableLiftSecondPart->x);
    sint16 dist_y = abs(passengerVehicle->y - cableLiftSecondPart->y);

    if (dist_x + dist_y > 2)
        return;

    vehicle->velocity = 0;
    vehicle->acceleration = 0;
    vehicle->status = VEHICLE_STATUS_DEPARTING;
    vehicle->sub_state = 0;
}

/**
 *
 *  rct2: 0x006DF97A
 */
static void cable_lift_update_departing(rct_vehicle *vehicle)
{
    vehicle->sub_state++;
    if (vehicle->sub_state < 16)
        return;

    rct_vehicle* passengerVehicle = GET_VEHICLE(vehicle->cable_lift_target);
    vehicle->status = VEHICLE_STATUS_TRAVELLING;
    passengerVehicle->status = VEHICLE_STATUS_TRAVELLING_CABLE_LIFT;
}

/**
 *
 *  rct2: 0x006DF99C
 */
static void cable_lift_update_travelling(rct_vehicle *vehicle)
{
    rct_vehicle* passengerVehicle = GET_VEHICLE(vehicle->cable_lift_target);

    vehicle->velocity = min(passengerVehicle->velocity, 439800);
    vehicle->acceleration = 0;
    if (passengerVehicle->update_flags & VEHICLE_UPDATE_FLAG_BROKEN_TRAIN)
        return;

    if (!(cable_lift_update_track_motion(vehicle) & (1 << 1)))
        return;

    vehicle->velocity = 0;
    vehicle->acceleration = 0;
    vehicle->status = VEHICLE_STATUS_ARRIVING;
    vehicle->sub_state = 0;
}

/**
 *
 *  rct2: 0x006DF9F0
 */
static void cable_lift_update_arriving(rct_vehicle *vehicle)
{
    vehicle->sub_state++;
    if (vehicle->sub_state >= 64)
        vehicle->status = VEHICLE_STATUS_MOVING_TO_END_OF_STATION;
}

static bool sub_6DF01A_loop(rct_vehicle* vehicle) {
    Ride* ride = get_ride(vehicle->ride);
    for (; vehicle->remaining_distance >= 13962; _vehicleUnkF64E10++) {
        uint8 trackType = vehicle->track_type >> 2;
        if (trackType == TRACK_ELEM_CABLE_LIFT_HILL &&
            vehicle->track_progress == 160) {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_1;
        }

        uint16 trackProgress = vehicle->track_progress + 1;

        const rct_vehicle_info *moveInfo = vehicle_get_move_info(vehicle->var_CD, vehicle->track_type, 0);
        uint16 trackTotalProgress = vehicle_get_move_info_size(vehicle->var_CD, vehicle->track_type);
        if (trackProgress >= trackTotalProgress) {
            _vehicleVAngleEndF64E36 = TrackDefinitions[trackType].vangle_end;
            _vehicleBankEndF64E37 = TrackDefinitions[trackType].bank_end;
            rct_map_element* trackElement =
                map_get_track_element_at_of_type_seq(
                    vehicle->track_x,
                    vehicle->track_y,
                    vehicle->track_z / 8,
                    trackType,
                    0);

            rct_xy_element input;
            rct_xy_element output;
            sint32 outputZ;
            sint32 outputDirection;

            input.x = vehicle->track_x;
            input.y = vehicle->track_y;
            input.element = trackElement;

            if (!track_block_get_next(&input, &output, &outputZ, &outputDirection))
                return false;

            if (TrackDefinitions[output.element->properties.track.type].vangle_start != _vehicleVAngleEndF64E36 ||
                TrackDefinitions[output.element->properties.track.type].bank_start != _vehicleBankEndF64E37)
                return false;

            vehicle->track_x = output.x;
            vehicle->track_y = output.y;
            vehicle->track_z = outputZ;
            vehicle->track_direction = outputDirection;
            vehicle->track_type |= output.element->properties.track.type << 2;
            trackProgress = 0;
        }

        vehicle->track_progress = trackProgress;
        moveInfo = vehicle_get_move_info(vehicle->var_CD, vehicle->track_type, trackProgress);
        rct_xyz16 unk = {
            moveInfo->x,
            moveInfo->y,
            moveInfo->z
        };

        unk.x += vehicle->track_x;
        unk.y += vehicle->track_y;
        unk.z += vehicle->track_z;

        uint8 bx = 0;
        unk.z += RideData5[ride->type].z_offset;
        if (unk.x != unk_F64E20.x)
            bx |= (1 << 0);
        if (unk.y != unk_F64E20.y)
            bx |= (1 << 1);
        if (unk.z != unk_F64E20.z)
            bx |= (1 << 2);

        vehicle->remaining_distance -= dword_9A2930[bx];
        unk_F64E20.x = unk.x;
        unk_F64E20.y = unk.y;
        unk_F64E20.z = unk.z;

        vehicle->sprite_direction = moveInfo->direction;
        vehicle->bank_rotation = moveInfo->bank_rotation;
        vehicle->vehicle_sprite_type = moveInfo->vehicle_sprite_type;

        if (vehicle->remaining_distance >= 13962) {
            vehicle->acceleration += dword_9A2970[vehicle->vehicle_sprite_type];
        }
    }
    return true;
}

static bool sub_6DF21B_loop(rct_vehicle* vehicle) {
    Ride* ride = get_ride(vehicle->ride);
    for (; vehicle->remaining_distance < 0; _vehicleUnkF64E10++) {
        uint16 trackProgress = vehicle->track_progress - 1;
        const rct_vehicle_info *moveInfo;

        if ((sint16)trackProgress == -1) {
            uint8 trackType = vehicle->track_type >> 2;
            _vehicleVAngleEndF64E36 = TrackDefinitions[trackType].vangle_start;
            _vehicleBankEndF64E37 = TrackDefinitions[trackType].bank_start;
            rct_map_element* trackElement =
                map_get_track_element_at_of_type_seq(
                    vehicle->track_x,
                    vehicle->track_y,
                    vehicle->track_z / 8,
                    trackType,
                    0);

            rct_xy_element input;

            input.x = vehicle->track_x;
            input.y = vehicle->track_y;
            input.element = trackElement;
            track_begin_end output;

            if (!track_block_get_previous(input.x, input.y, input.element, &output))
                return false;

            if (TrackDefinitions[output.begin_element->properties.track.type].vangle_end != _vehicleVAngleEndF64E36 ||
                TrackDefinitions[output.begin_element->properties.track.type].bank_end != _vehicleBankEndF64E37)
                return false;

            vehicle->track_x = output.begin_x;
            vehicle->track_y = output.begin_y;
            vehicle->track_z = output.begin_z;
            vehicle->track_direction = output.begin_direction;
            vehicle->track_type |= output.begin_element->properties.track.type << 2;

            if (output.begin_element->properties.track.type == TRACK_ELEM_END_STATION) {
                _vehicleMotionTrackFlags = VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
            }

            moveInfo = vehicle_get_move_info(vehicle->var_CD, vehicle->track_type, 0);
            uint16 trackTotalProgress = vehicle_get_move_info_size(vehicle->var_CD, vehicle->track_type);
            trackProgress = trackTotalProgress - 1;
        }
        vehicle->track_progress = trackProgress;

        moveInfo = vehicle_get_move_info(vehicle->var_CD, vehicle->track_type, trackProgress);
        rct_xyz16 unk = {
            moveInfo->x,
            moveInfo->y,
            moveInfo->z
        };

        unk.x += vehicle->track_x;
        unk.y += vehicle->track_y;
        unk.z += vehicle->track_z;

        uint8 bx = 0;
        unk.z += RideData5[ride->type].z_offset;
        if (unk.x != unk_F64E20.x)
            bx |= (1 << 0);
        if (unk.y != unk_F64E20.y)
            bx |= (1 << 1);
        if (unk.z != unk_F64E20.z)
            bx |= (1 << 2);

        vehicle->remaining_distance += dword_9A2930[bx];
        unk_F64E20.x = unk.x;
        unk_F64E20.y = unk.y;
        unk_F64E20.z = unk.z;

        vehicle->sprite_direction = moveInfo->direction;
        vehicle->bank_rotation = moveInfo->bank_rotation;
        vehicle->vehicle_sprite_type = moveInfo->vehicle_sprite_type;

        if (vehicle->remaining_distance < 0) {
            vehicle->acceleration += dword_9A2970[vehicle->vehicle_sprite_type];
        }
    }
    return true;
}

/**
 *
 *  rct2: 0x006DEF56
 */
sint32 cable_lift_update_track_motion(rct_vehicle *cableLift)
{
    _vehicleF64E2C = 0;
    gCurrentVehicle = cableLift;
    _vehicleMotionTrackFlags = 0;
    _vehicleStationIndex = 0xFF;

    cableLift->velocity += cableLift->acceleration;
    _vehicleVelocityF64E08 = cableLift->velocity;
    _vehicleVelocityF64E0C = (cableLift->velocity / 1024) * 42;

    rct_vehicle* frontVehicle = cableLift;
    if (cableLift->velocity < 0) {
        frontVehicle = vehicle_get_tail(cableLift);
    }

    _vehicleFrontVehicle = frontVehicle;

    for (rct_vehicle* vehicle = frontVehicle;;) {
        vehicle->acceleration = dword_9A2970[vehicle->vehicle_sprite_type];
        _vehicleUnkF64E10 = 1;
        vehicle->remaining_distance += _vehicleVelocityF64E0C;

        if (vehicle->remaining_distance < 0 || vehicle->remaining_distance >= 13962) {
            unk_F64E20.x = vehicle->x;
            unk_F64E20.y = vehicle->y;
            unk_F64E20.z = vehicle->z;
            invalidate_sprite_2((rct_sprite*)vehicle);

            while (true) {
                if (vehicle->remaining_distance < 0) {
                    if (sub_6DF21B_loop(vehicle) == true) {
                        break;
                    }
                    else {
                        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                        _vehicleVelocityF64E0C -= vehicle->remaining_distance - 13962;
                        vehicle->remaining_distance = 13962;
                        vehicle->acceleration += dword_9A2970[vehicle->vehicle_sprite_type];
                        _vehicleUnkF64E10++;
                        continue;
                    }
                }
                else {
                    if (sub_6DF01A_loop(vehicle) == true) {
                        break;
                    }
                    else {
                        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                        _vehicleVelocityF64E0C -= vehicle->remaining_distance + 1;
                        vehicle->remaining_distance = -1;
                        vehicle->acceleration += dword_9A2970[vehicle->vehicle_sprite_type];
                        _vehicleUnkF64E10++;
                    }
                }
            }
            sprite_move(
                unk_F64E20.x,
                unk_F64E20.y,
                unk_F64E20.z,
                (rct_sprite*)vehicle);

            invalidate_sprite_2((rct_sprite*)vehicle);
        }
        vehicle->acceleration /= _vehicleUnkF64E10;
        if (_vehicleVelocityF64E08 >= 0) {
            if (vehicle->next_vehicle_on_train == SPRITE_INDEX_NULL)
                break;
            vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
        }
        else {
            if (vehicle == cableLift)
                break;
            vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
        }
    }

    uint32 vehicleCount = 0;
    uint16 frictionTotal = 0;
    sint32 var2CTotal = 0;

    for (uint16 spriteId = cableLift->sprite_index; spriteId != SPRITE_INDEX_NULL;) {
        rct_vehicle* vehicle = GET_VEHICLE(spriteId);
        vehicleCount++;
        frictionTotal += vehicle->friction;
        // Prevent over/underflow in var2CTotal
        if ((INT32_MAX - abs(vehicle->acceleration)) < abs(var2CTotal) &&
                (var2CTotal < 0) == (vehicle->acceleration < 0))
        {
            var2CTotal = (var2CTotal < 0) ? INT32_MIN : INT32_MAX ;
        } else {
            var2CTotal += vehicle->acceleration;
        }
        spriteId = vehicle->next_vehicle_on_train;
    }

    sint32 ecx = (var2CTotal / vehicleCount) >> 9;
    ecx -= cableLift->velocity >> 12;

    sint32 edx = cableLift->velocity >> 8;
    edx *= edx;
    if (cableLift->velocity < 0)
        edx = -edx;
    edx >>= 4;
    ecx -= edx / frictionTotal;

    cableLift->acceleration = ecx;
    return _vehicleMotionTrackFlags;
}
