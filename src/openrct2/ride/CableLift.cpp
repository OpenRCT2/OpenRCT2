/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/Math.hpp"
#include "../rct12/RCT12.h"
#include "../util/Util.h"
#include "../world/Sprite.h"
#include "CableLift.h"
#include "Ride.h"
#include "Track.h"
#include "RideData.h"
#include "VehicleData.h"

static void cable_lift_update_moving_to_end_of_station(rct_vehicle * vehicle);
static void cable_lift_update_waiting_to_depart(rct_vehicle * vehicle);
static void cable_lift_update_departing(rct_vehicle * vehicle);
static void cable_lift_update_travelling(rct_vehicle * vehicle);
static void cable_lift_update_arriving(rct_vehicle * vehicle);

rct_vehicle * cable_lift_segment_create(int32_t rideIndex,
                                        int32_t x,
                                        int32_t y,
                                        int32_t z,
                                        int32_t direction,
                                        uint16_t var_44,
                                        int32_t remaining_distance,
                                        bool head)
{
    Ride        * ride    = get_ride(rideIndex);
    rct_vehicle * current = &(create_sprite(1)->vehicle);
    current->sprite_identifier = SPRITE_IDENTIFIER_VEHICLE;
    current->ride              = rideIndex;
    current->ride_subtype      = RIDE_ENTRY_INDEX_NULL;
    if (head)
    {
        move_sprite_to_list((rct_sprite *) current, SPRITE_LIST_TRAIN * 2);
        ride->cable_lift = current->sprite_index;
    }
    current->is_child               = head ? 0 : 1;
    current->var_44                 = var_44;
    current->remaining_distance     = remaining_distance;
    current->sprite_width           = 10;
    current->sprite_height_negative = 10;
    current->sprite_height_positive = 10;
    current->mass                   = 100;
    current->num_seats              = 0;
    current->speed                  = 20;
    current->powered_acceleration   = 80;
    current->velocity               = 0;
    current->acceleration           = 0;
    current->swing_sprite           = 0;
    current->swinging_car_var_0     = 0;
    current->var_4E                 = 0;
    current->restraints_position    = 0;
    current->spin_sprite            = 0;
    current->spin_speed             = 0;
    current->sound2_flags           = 0;
    current->sound1_id              = RCT12_SOUND_ID_NULL;
    current->sound2_id              = RCT12_SOUND_ID_NULL;
    current->var_C4                 = 0;
    current->animation_frame        = 0;
    current->var_C8                 = 0;
    current->var_CA                 = 0;
    current->scream_sound_id        = 0xFF;
    current->vehicle_sprite_type    = 0;
    current->bank_rotation          = 0;
    for (auto &peep : current->peep)
    {
        peep = SPRITE_INDEX_NULL;
    }
    current->var_CD           = 0;
    current->sprite_direction = direction << 3;
    current->track_x          = x;
    current->track_y          = y;

    z = z * 8;
    current->track_z = z;
    z += RideData5[ride->type].z_offset;

    sprite_move(16, 16, z, (rct_sprite *) current);
    current->track_type     = (TRACK_ELEM_CABLE_LIFT_HILL << 2) | (current->sprite_direction >> 3);
    current->track_progress = 164;
    current->update_flags   = VEHICLE_UPDATE_FLAG_1;
    current->status         = VEHICLE_STATUS_MOVING_TO_END_OF_STATION;
    current->sub_state      = 0;
    current->num_peeps      = 0;
    current->next_free_seat = 0;
    return current;
}

void cable_lift_update(rct_vehicle * vehicle)
{
    switch (vehicle->status)
    {
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
static void cable_lift_update_moving_to_end_of_station(rct_vehicle * vehicle)
{
    if (vehicle->velocity >= -439800)
        vehicle->acceleration = -2932;

    if (vehicle->velocity < -439800)
    {
        vehicle->velocity -= vehicle->velocity / 16;
        vehicle->acceleration = 0;
    }

    if (!(cable_lift_update_track_motion(vehicle) & (1 << 0)))
        return;

    vehicle->velocity     = 0;
    vehicle->acceleration = 0;
    vehicle->status       = VEHICLE_STATUS_WAITING_FOR_PASSENGERS;
}

/**
 *
 *  rct2: 0x006DF8F1
 */
static void cable_lift_update_waiting_to_depart(rct_vehicle * vehicle)
{
    if (vehicle->velocity >= -58640)
        vehicle->acceleration = -14660;

    if (vehicle->velocity < -58640)
    {
        vehicle->velocity -= vehicle->velocity / 16;
        vehicle->acceleration = 0;
    }

    cable_lift_update_track_motion(vehicle);

    // Next check to see if the second part of the cable lift
    // is at the front of the passenger vehicle to simulate the
    // cable being attached underneath the train.
    rct_vehicle * passengerVehicle    = GET_VEHICLE(vehicle->cable_lift_target);
    rct_vehicle * cableLiftSecondPart = GET_VEHICLE(vehicle->prev_vehicle_on_ride);

    int16_t dist_x = abs(passengerVehicle->x - cableLiftSecondPart->x);
    int16_t dist_y = abs(passengerVehicle->y - cableLiftSecondPart->y);

    if (dist_x + dist_y > 2)
        return;

    vehicle->velocity     = 0;
    vehicle->acceleration = 0;
    vehicle->status       = VEHICLE_STATUS_DEPARTING;
    vehicle->sub_state    = 0;
}

/**
 *
 *  rct2: 0x006DF97A
 */
static void cable_lift_update_departing(rct_vehicle * vehicle)
{
    vehicle->sub_state++;
    if (vehicle->sub_state < 16)
        return;

    rct_vehicle * passengerVehicle = GET_VEHICLE(vehicle->cable_lift_target);
    vehicle->status                = VEHICLE_STATUS_TRAVELLING;
    passengerVehicle->status       = VEHICLE_STATUS_TRAVELLING_CABLE_LIFT;
}

/**
 *
 *  rct2: 0x006DF99C
 */
static void cable_lift_update_travelling(rct_vehicle * vehicle)
{
    rct_vehicle * passengerVehicle = GET_VEHICLE(vehicle->cable_lift_target);

    vehicle->velocity     = std::min(passengerVehicle->velocity, 439800);
    vehicle->acceleration = 0;
    if (passengerVehicle->update_flags & VEHICLE_UPDATE_FLAG_BROKEN_TRAIN)
        return;

    if (!(cable_lift_update_track_motion(vehicle) & (1 << 1)))
        return;

    vehicle->velocity     = 0;
    vehicle->acceleration = 0;
    vehicle->status       = VEHICLE_STATUS_ARRIVING;
    vehicle->sub_state    = 0;
}

/**
 *
 *  rct2: 0x006DF9F0
 */
static void cable_lift_update_arriving(rct_vehicle * vehicle)
{
    vehicle->sub_state++;
    if (vehicle->sub_state >= 64)
        vehicle->status = VEHICLE_STATUS_MOVING_TO_END_OF_STATION;
}

static bool sub_6DF01A_loop(rct_vehicle * vehicle)
{
    Ride * ride = get_ride(vehicle->ride);
    for (; vehicle->remaining_distance >= 13962; _vehicleUnkF64E10++)
    {
        uint8_t trackType = vehicle->track_type >> 2;
        if (trackType == TRACK_ELEM_CABLE_LIFT_HILL &&
            vehicle->track_progress == 160)
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_1;
        }

        uint16_t trackProgress = vehicle->track_progress + 1;

        const rct_vehicle_info * moveInfo           = vehicle_get_move_info(vehicle->var_CD, vehicle->track_type, 0);
        uint16_t                   trackTotalProgress = vehicle_get_move_info_size(vehicle->var_CD, vehicle->track_type);
        if (trackProgress >= trackTotalProgress)
        {
            _vehicleVAngleEndF64E36 = TrackDefinitions[trackType].vangle_end;
            _vehicleBankEndF64E37   = TrackDefinitions[trackType].bank_end;
            rct_tile_element * trackElement = map_get_track_element_at_of_type_seq(
                vehicle->track_x,
                vehicle->track_y,
                vehicle->track_z / 8,
                trackType,
                0
            );

            CoordsXYE   input;
            CoordsXYE   output;
            int32_t         outputZ;
            int32_t         outputDirection;

            input.x       = vehicle->track_x;
            input.y       = vehicle->track_y;
            input.element = trackElement;

            if (!track_block_get_next(&input, &output, &outputZ, &outputDirection))
                return false;

            if (TrackDefinitions[track_element_get_type(output.element)].vangle_start != _vehicleVAngleEndF64E36 ||
                TrackDefinitions[track_element_get_type(output.element)].bank_start != _vehicleBankEndF64E37)
                return false;

            vehicle->track_x         = output.x;
            vehicle->track_y         = output.y;
            vehicle->track_z         = outputZ;
            vehicle->track_direction = outputDirection;
            vehicle->track_type     |= track_element_get_type(output.element) << 2;
            trackProgress = 0;
        }

        vehicle->track_progress = trackProgress;
        moveInfo = vehicle_get_move_info(vehicle->var_CD, vehicle->track_type, trackProgress);
        LocationXYZ16 unk = {
            moveInfo->x,
            moveInfo->y,
            moveInfo->z
        };

        unk.x += vehicle->track_x;
        unk.y += vehicle->track_y;
        unk.z += vehicle->track_z;

        uint8_t bx = 0;
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

        vehicle->sprite_direction    = moveInfo->direction;
        vehicle->bank_rotation       = moveInfo->bank_rotation;
        vehicle->vehicle_sprite_type = moveInfo->vehicle_sprite_type;

        if (vehicle->remaining_distance >= 13962)
        {
            vehicle->acceleration += dword_9A2970[vehicle->vehicle_sprite_type];
        }
    }
    return true;
}

static bool sub_6DF21B_loop(rct_vehicle * vehicle)
{
    Ride * ride = get_ride(vehicle->ride);
    for (; vehicle->remaining_distance < 0; _vehicleUnkF64E10++)
    {
        uint16_t                 trackProgress = vehicle->track_progress - 1;
        const rct_vehicle_info * moveInfo;

        if ((int16_t) trackProgress == -1)
        {
            uint8_t trackType = vehicle->track_type >> 2;
            _vehicleVAngleEndF64E36 = TrackDefinitions[trackType].vangle_start;
            _vehicleBankEndF64E37   = TrackDefinitions[trackType].bank_start;

            rct_tile_element * trackElement = map_get_track_element_at_of_type_seq(
                vehicle->track_x,
                vehicle->track_y,
                vehicle->track_z / 8,
                trackType,
                0
            );

            CoordsXYE    input;

            input.x       = vehicle->track_x;
            input.y       = vehicle->track_y;
            input.element = trackElement;
            track_begin_end output;

            if (!track_block_get_previous(input.x, input.y, input.element, &output))
                return false;

            if (TrackDefinitions[track_element_get_type(output.begin_element)].vangle_end != _vehicleVAngleEndF64E36 ||
                TrackDefinitions[track_element_get_type(output.begin_element)].bank_end != _vehicleBankEndF64E37)
                return false;

            vehicle->track_x         = output.begin_x;
            vehicle->track_y         = output.begin_y;
            vehicle->track_z         = output.begin_z;
            vehicle->track_direction = output.begin_direction;
            vehicle->track_type |= track_element_get_type(output.begin_element) << 2;

            if (track_element_get_type(output.begin_element) == TRACK_ELEM_END_STATION)
            {
                _vehicleMotionTrackFlags = VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
            }

            moveInfo = vehicle_get_move_info(vehicle->var_CD, vehicle->track_type, 0);
            uint16_t trackTotalProgress = vehicle_get_move_info_size(vehicle->var_CD, vehicle->track_type);
            trackProgress = trackTotalProgress - 1;
        }
        vehicle->track_progress = trackProgress;

        moveInfo = vehicle_get_move_info(vehicle->var_CD, vehicle->track_type, trackProgress);
        LocationXYZ16 unk = { moveInfo->x, moveInfo->y, moveInfo->z };

        unk.x += vehicle->track_x;
        unk.y += vehicle->track_y;
        unk.z += vehicle->track_z;

        uint8_t bx = 0;
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

        vehicle->sprite_direction    = moveInfo->direction;
        vehicle->bank_rotation       = moveInfo->bank_rotation;
        vehicle->vehicle_sprite_type = moveInfo->vehicle_sprite_type;

        if (vehicle->remaining_distance < 0)
        {
            vehicle->acceleration += dword_9A2970[vehicle->vehicle_sprite_type];
        }
    }
    return true;
}

/**
 *
 *  rct2: 0x006DEF56
 */
int32_t cable_lift_update_track_motion(rct_vehicle * cableLift)
{
    _vehicleF64E2C           = 0;
    gCurrentVehicle          = cableLift;
    _vehicleMotionTrackFlags = 0;
    _vehicleStationIndex     = 0xFF;

    cableLift->velocity += cableLift->acceleration;
    _vehicleVelocityF64E08 = cableLift->velocity;
    _vehicleVelocityF64E0C = (cableLift->velocity / 1024) * 42;

    rct_vehicle * frontVehicle = cableLift;
    if (cableLift->velocity < 0)
    {
        frontVehicle = vehicle_get_tail(cableLift);
    }

    _vehicleFrontVehicle = frontVehicle;

    for (rct_vehicle * vehicle = frontVehicle;;)
    {
        vehicle->acceleration = dword_9A2970[vehicle->vehicle_sprite_type];
        _vehicleUnkF64E10 = 1;
        vehicle->remaining_distance += _vehicleVelocityF64E0C;

        if (vehicle->remaining_distance < 0 || vehicle->remaining_distance >= 13962)
        {
            unk_F64E20.x = vehicle->x;
            unk_F64E20.y = vehicle->y;
            unk_F64E20.z = vehicle->z;
            invalidate_sprite_2((rct_sprite *) vehicle);

            while (true)
            {
                if (vehicle->remaining_distance < 0)
                {
                    if (sub_6DF21B_loop(vehicle) == true)
                    {
                        break;
                    }
                    else
                    {
                        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
                        _vehicleVelocityF64E0C -= vehicle->remaining_distance - 13962;
                        vehicle->remaining_distance = 13962;
                        vehicle->acceleration += dword_9A2970[vehicle->vehicle_sprite_type];
                        _vehicleUnkF64E10++;
                        continue;
                    }
                }
                else
                {
                    if (sub_6DF01A_loop(vehicle) == true)
                    {
                        break;
                    }
                    else
                    {
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
                (rct_sprite *) vehicle
            );

            invalidate_sprite_2((rct_sprite *) vehicle);
        }
        vehicle->acceleration /= _vehicleUnkF64E10;
        if (_vehicleVelocityF64E08 >= 0)
        {
            if (vehicle->next_vehicle_on_train == SPRITE_INDEX_NULL)
                break;
            vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
        }
        else
        {
            if (vehicle == cableLift)
                break;
            vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
        }
    }

    uint32_t vehicleCount      = 0;
    uint16_t massTotal         = 0;
    int32_t accelerationTotal = 0;

    for (uint16_t spriteId = cableLift->sprite_index; spriteId != SPRITE_INDEX_NULL;)
    {
        rct_vehicle * vehicle = GET_VEHICLE(spriteId);
        vehicleCount++;

        massTotal         += vehicle->mass;
        accelerationTotal  = add_clamp_int32_t(accelerationTotal, vehicle->acceleration);

        spriteId = vehicle->next_vehicle_on_train;
    }

    int32_t newAcceleration = (accelerationTotal / vehicleCount) >> 9;
    newAcceleration -= cableLift->velocity >> 12;

    int32_t edx = cableLift->velocity >> 8;
    edx *= edx;
    if (cableLift->velocity < 0)
    {
        edx = -edx;
    }
    edx >>= 4;
    newAcceleration -= edx / massTotal;

    cableLift->acceleration = newAcceleration;
    return _vehicleMotionTrackFlags;
}
