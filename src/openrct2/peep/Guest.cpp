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

#include "Peep.h"
#include "../world/Sprite.h"

/**
 *
 *  rct2: 0x00691677
 */
static void peep_try_get_up_from_sitting(rct_peep * peep)
{
    // Eats all food first
    if (peep_has_food(peep))
        return;

    peep->time_to_sitdown--;
    if (peep->time_to_sitdown)
        return;

    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_WALKING;
    peep_window_state_update(peep);

    // Set destination to the centre of the tile.
    peep->destination_x         = (peep->x & 0xFFE0) + 16;
    peep->destination_y         = (peep->y & 0xFFE0) + 16;
    peep->destination_tolerance = 5;
    peep_update_current_action_sprite_type(peep);
}

/** rct2: 0x00981F2C, 0x00981F2E */
static constexpr const LocationXY16 _981F2C[] = {
    { 7, 12 }, { 12, 25 }, { 25, 20 }, { 20, 7 }, { 7, 20 }, { 20, 25 }, { 25, 12 }, { 12, 7 },
};

/**
 *
 *  rct2: 0x0069152B
 */
void rct_peep::UpdateSitting()
{
    if (sub_state == PEEP_SITTING_TRYING_TO_SIT)
    {
        if (!CheckForPath())
            return;
        // 691541

        uint8 pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        sint32 ebx = var_37 & 0x7;
        LocationXYZ16 loc = 
        {
            (x & 0xFFE0) + _981F2C[ebx].x,
            (y & 0xFFE0) + _981F2C[ebx].y,
            z
        };

        invalidate_sprite_2((rct_sprite *)this);
        sprite_move(loc.x, loc.y, loc.z, (rct_sprite *)this);

        sprite_direction = ((var_37 + 2) & 3) * 8;
        invalidate_sprite_2((rct_sprite *)this);
        action                  = 254;
        next_action_sprite_type = 7;
        peep_switch_to_next_action_sprite_type(this);

        sub_state = PEEP_SITTING_SAT_DOWN;

        // Sets time to sit on seat
        time_to_sitdown = (129 - energy) * 16 + 50;
    }
    else if (sub_state == PEEP_SITTING_SAT_DOWN)
    {
        if (action < 0xFE)
        {
            sint16 actionX, actionY, xy_distance;
            peep_update_action(&actionX, &actionY, &xy_distance, this);
            if (action != 0xFF)
                return;

            action = 0xFE;
            peep_try_get_up_from_sitting(this);
            return;
        }

        if ((peep_flags & PEEP_FLAGS_LEAVING_PARK))
        {
            SetState(PEEP_STATE_WALKING);

            // Set destination to the centre of the tile
            destination_x         = (x & 0xFFE0) + 16;
            destination_y         = (y & 0xFFE0) + 16;
            destination_tolerance = 5;
            peep_update_current_action_sprite_type(this);
            return;
        }

        if (sprite_type == PEEP_SPRITE_TYPE_UMBRELLA)
        {
            peep_try_get_up_from_sitting(this);
            return;
        }

        if (peep_has_food(this))
        {
            if ((peep_rand() & 0xFFFF) > 1310)
            {
                peep_try_get_up_from_sitting(this);
                return;
            }
            action                     = PEEP_ACTION_SITTING_EAT_FOOD;
            action_frame               = 0;
            action_sprite_image_offset = 0;
            peep_update_current_action_sprite_type(this);
            invalidate_sprite_2((rct_sprite *)this);
            return;
        }

        sint32 rand = peep_rand();
        if ((rand & 0xFFFF) > 131)
        {
            peep_try_get_up_from_sitting(this);
            return;
        }
        if (sprite_type == PEEP_SPRITE_TYPE_BALLOON || sprite_type == PEEP_SPRITE_TYPE_HAT)
        {
            peep_try_get_up_from_sitting(this);
            return;
        }

        action = PEEP_ACTION_SITTING_LOOK_AROUND_LEFT;
        if (rand & 0x80000000)
        {
            action = PEEP_ACTION_SITTING_LOOK_AROUND_RIGHT;
        }

        if (rand & 0x40000000)
        {
            action = PEEP_ACTION_SITTING_CHECK_WATCH;
        }
        action_frame               = 0;
        action_sprite_image_offset = 0;
        peep_update_current_action_sprite_type(this);
        invalidate_sprite_2((rct_sprite *)this);
        return;
    }
}

/**
 *
 *  rct2: 0x006966A9
 */
void remove_peep_from_queue(rct_peep * peep)
{
    Ride * ride = get_ride(peep->current_ride);

    uint8 cur_station = peep->current_ride_station;
    // Make sure we don't underflow, building while paused might reset it to 0 where peeps have
    // not yet left the queue.
    if (ride->queue_length[cur_station] > 0)
    {
        ride->queue_length[cur_station]--;
    }

    if (peep->sprite_index == ride->last_peep_in_queue[cur_station])
    {
        ride->last_peep_in_queue[cur_station] = peep->next_in_queue;
        return;
    }

    uint16 spriteId = ride->last_peep_in_queue[cur_station];
    while (spriteId != SPRITE_INDEX_NULL)
    {
        rct_peep * other_peep = GET_PEEP(spriteId);
        if (peep->sprite_index == other_peep->next_in_queue)
        {
            other_peep->next_in_queue = peep->next_in_queue;
            return;
        }
        spriteId = other_peep->next_in_queue;
    }
}

/**
 *
 *  rct2: 0x00691C6E
 */
static rct_vehicle * peep_choose_car_from_ride(rct_peep * peep, Ride * ride, std::vector<uint8> &car_array)
{
    uint8 chosen_car = peep_rand();
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_G_FORCES) && ((chosen_car & 0xC) != 0xC))
    {
        chosen_car = (peep_rand() & 1) ? 0 : (uint8)car_array.size() - 1;
    }
    else
    {
        chosen_car = (chosen_car * (uint16)car_array.size()) >> 8;
    }

    peep->current_car = car_array[chosen_car];

    rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);

    for (sint32 i = peep->current_car; i > 0; --i)
    {
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    return vehicle;
}

/**
 *
 *  rct2: 0x00691CD1
 */
static void peep_choose_seat_from_car(rct_peep * peep, Ride * ride, rct_vehicle * vehicle)
{
    uint8 chosen_seat = vehicle->next_free_seat;

    if (ride->mode == RIDE_MODE_FORWARD_ROTATION || ride->mode == RIDE_MODE_BACKWARD_ROTATION)
    {

        chosen_seat = (((~vehicle->vehicle_sprite_type + 1) >> 3) & 0xF) * 2;
        if (vehicle->next_free_seat & 1)
        {
            chosen_seat++;
        }
    }
    peep->current_seat = chosen_seat;
    vehicle->next_free_seat++;

    vehicle->peep[peep->current_seat]                = peep->sprite_index;
    vehicle->peep_tshirt_colours[peep->current_seat] = peep->tshirt_colour;
}

/**
 *
 *  rct2: 0x00691D27
 */
static void peep_go_to_ride_entrance(rct_peep * peep, Ride * ride)
{
    TileCoordsXYZD location = ride_get_entrance_location(peep->current_ride, peep->current_ride_station);
    Guard::Assert(!location.isNull());
    sint32 x = location.x;
    sint32 y = location.y;

    uint8 direction = location.direction;

    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    sint16 x_shift = word_981D6C[direction].x;
    sint16 y_shift = word_981D6C[direction].y;

    uint8            shift_multiplier = 21;
    rct_ride_entry * rideEntry        = get_ride_entry(ride->subtype);
    if (rideEntry != nullptr)
    {
        if (rideEntry->vehicles[rideEntry->default_vehicle].flags & VEHICLE_ENTRY_FLAG_MINI_GOLF ||
            rideEntry->vehicles[rideEntry->default_vehicle].flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
        {
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x += x_shift;
    y += y_shift;

    peep->destination_x         = x;
    peep->destination_y         = y;
    peep->destination_tolerance = 2;

    peep_decrement_num_riders(peep);
    peep->state     = PEEP_STATE_ENTERING_RIDE;
    peep->sub_state = PEEP_RIDE_IN_ENTRANCE;
    peep_window_state_update(peep);

    peep->rejoin_queue_timeout       = 0;
    peep->time_on_ride = 0;

    remove_peep_from_queue(peep);
}

static bool peep_find_vehicle_to_enter(rct_peep * peep, Ride * ride, std::vector<uint8> &car_array)
{
    uint8 chosen_train = 0xFF;

    if (ride->mode == RIDE_MODE_BUMPERCAR || ride->mode == RIDE_MODE_RACE)
    {
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
            return false;

        for (sint32 i = 0; i < ride->num_vehicles; ++i)
        {
            rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[i]);

            if (vehicle->next_free_seat >= vehicle->num_seats)
                continue;

            if (vehicle->status != VEHICLE_STATUS_WAITING_FOR_PASSENGERS)
                continue;
            chosen_train = i;
            break;
        }
    }
    else
    {
        chosen_train = ride->train_at_station[peep->current_ride_station];
    }
    if (chosen_train == 0xFF)
    {
        return false;
    }

    peep->current_train = chosen_train;

    sint32 i = 0;

    uint16        vehicle_id = ride->vehicles[chosen_train];
    rct_vehicle * vehicle = GET_VEHICLE(vehicle_id);

    for (; vehicle_id != SPRITE_INDEX_NULL; vehicle_id = vehicle->next_vehicle_on_train, i++)
    {
        vehicle = GET_VEHICLE(vehicle_id);

        uint8 num_seats = vehicle->num_seats;
        if (vehicle_is_used_in_pairs(vehicle))
        {
            num_seats &= VEHICLE_SEAT_NUM_MASK;
            if (vehicle->next_free_seat & 1)
            {
                peep->current_car = i;
                peep_choose_seat_from_car(peep, ride, vehicle);
                peep_go_to_ride_entrance(peep, ride);
                return false;
            }
        }
        if (num_seats == vehicle->next_free_seat)
            continue;

        if (ride->mode == RIDE_MODE_FORWARD_ROTATION || ride->mode == RIDE_MODE_BACKWARD_ROTATION)
        {
            uint8 position = (((~vehicle->vehicle_sprite_type + 1) >> 3) & 0xF) * 2;
            if (vehicle->peep[position] != SPRITE_INDEX_NULL)
                continue;
        }
        car_array.push_back(i);
    }

    return !car_array.empty();
}

static void peep_update_ride_at_entrance_try_leave(rct_peep * peep)
{
    // Destination Tolerance is zero when peep has completely
    // entered entrance
    if (peep->destination_tolerance == 0)
    {
        remove_peep_from_queue(peep);
        peep_decrement_num_riders(peep);
        peep->state = PEEP_STATE_FALLING;
        peep_window_state_update(peep);
    }
}

static bool peep_check_ride_price_at_entrance(rct_peep * peep, Ride * ride, money32 ridePrice)
{
    if ((peep->item_standard_flags & PEEP_ITEM_VOUCHER) &&
        peep->voucher_type == VOUCHER_TYPE_RIDE_FREE &&
        peep->voucher_arguments == peep->current_ride)
        return true;
    
    if (peep->cash_in_pocket <= 0)
    {
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_SPENT_MONEY, PEEP_THOUGHT_ITEM_NONE);
        peep_update_ride_at_entrance_try_leave(peep);
        return false;
    }

    if (ridePrice > peep->cash_in_pocket)
    {
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CANT_AFFORD_0, peep->current_ride);
        peep_update_ride_at_entrance_try_leave(peep);
        return false;
    }

    uint16 value = ride->value;
    if (value != RIDE_VALUE_UNDEFINED)
    {
        if (value * 2 < ridePrice)
        {
            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_BAD_VALUE, peep->current_ride);
            peep_update_ride_at_entrance_try_leave(peep);
            return false;
        }
    }
    return true;
}

/**
 *
 *  rct2: 0x00691A3B
 */
void rct_peep::UpdateRideAtEntrance()
{
    Ride * ride = get_ride(peep->current_ride);

    // The peep will keep advancing in the entranceway
    // whilst in this state. When it has reached the very
    // front of the queue destination tolerance is set to 
    // zero to indicate it is final decision time (try_leave will pass).
    // When a peep has to return to the queue without getting on a ride
    // this is the state it will return to.
    if (peep->destination_tolerance != 0)
    {
        invalidate_sprite_2((rct_sprite *)peep);

        sint16 x, y, xy_distance;

        if (peep_update_action(&x, &y, &xy_distance, peep))
        {
            sint16 z = peep->z;
            if (xy_distance < 16)
            {
                auto entrance = ride_get_entrance_location(ride, peep->current_ride_station);
                z = entrance.z * 8 + 2;
            }
            sprite_move(x, y, z, (rct_sprite *)peep);
            invalidate_sprite_2((rct_sprite *)peep);
        }
        else
        {
            peep->destination_tolerance = 0;
            peep->sprite_direction ^= (1 << 4);
        }
    }

    std::vector<uint8> carArray;

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        if (ride->num_riders >= ride->operation_option)
            return;
    }
    else
    {
        if (!peep_find_vehicle_to_enter(peep, ride, carArray))
            return;
    }

    if (ride->status != RIDE_STATUS_OPEN || ride->vehicle_change_timeout != 0)
    {
        peep_update_ride_at_entrance_try_leave(peep);
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
        return;

    money16 ridePrice = ride_get_price(ride);
    if (ridePrice != 0)
    {
        if (!peep_check_ride_price_at_entrance(peep, ride, ridePrice))
            return;
    }

    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        rct_vehicle * vehicle = peep_choose_car_from_ride(peep, ride, carArray);
        peep_choose_seat_from_car(peep, ride, vehicle);
    }
    peep_go_to_ride_entrance(peep, ride);
}

/** rct2: 0x00981FD4, 0x00981FD6 */
static constexpr const LocationXY16 _981FD4[] = {
    { 8, 8 },
    { 8, 24 },
    { 24, 24 },
    { 24, 8 },
};

static void peep_update_ride_leave_entrance_maze(rct_peep * peep, Ride * ride, TileCoordsXYZD &entrance_loc)
{
    peep->maze_last_edge = entrance_loc.direction + 1;
    entrance_loc.x *= 32;
    entrance_loc.y *= 32;

    entrance_loc.x += TileDirectionDelta[entrance_loc.direction].x;
    entrance_loc.y += TileDirectionDelta[entrance_loc.direction].y;

    uint8 direction = entrance_loc.direction * 4 + 11;
    if (peep_rand() & 0x40)
    {
        direction += 4;
        peep->maze_last_edge += 2;
    }

    direction &= 0xF;
    // Direction is 11, 15, 3, or 7
    peep->var_37 = direction;
    peep->maze_last_edge &= 3;

    entrance_loc.x += _981FD4[direction / 4].x;
    entrance_loc.y += _981FD4[direction / 4].y;

    peep->destination_x = entrance_loc.x;
    peep->destination_y = entrance_loc.y;
    peep->destination_tolerance = 3;

    ride->cur_num_customers++;
    peep_on_enter_or_exit_ride(peep, peep->current_ride, 0);
    peep->sub_state = PEEP_RIDE_MAZE_PATHFINDING;
}

static void peep_update_ride_leave_entrance_spiral_slide(rct_peep * peep, Ride * ride, TileCoordsXYZD &entrance_loc)
{
    entrance_loc.x = ride->station_starts[peep->current_ride_station].x * 32;
    entrance_loc.y = ride->station_starts[peep->current_ride_station].y * 32;

    rct_tile_element * tile_element = ride_get_station_start_track_element(ride, peep->current_ride_station);

    uint8 direction_track = (tile_element == nullptr ? 0 : tile_element_get_direction(tile_element));

    peep->var_37 = (entrance_loc.direction << 2) | (direction_track << 4);

    const CoordsXY slidePlatformDestination = SpiralSlideWalkingPath[peep->var_37];

    entrance_loc.x += slidePlatformDestination.x;
    entrance_loc.y += slidePlatformDestination.y;

    peep->destination_x = entrance_loc.x;
    peep->destination_y = entrance_loc.y;
    peep->current_car = 0;

    ride->cur_num_customers++;
    peep_on_enter_or_exit_ride(peep, peep->current_ride, 0);
    peep->sub_state = PEEP_RIDE_APPROACH_SPIRAL_SLIDE;
}

static uint8 peep_get_waypointed_seat_location(rct_peep * peep, Ride * ride, rct_ride_entry_vehicle * vehicle_type, uint8 track_direction)
{
    // The seatlocation can be split into segments around the ride base
    // to decide the segment first split off the segmentable seat location
    // from the fixed section
    uint8 seatLocationSegment = peep->current_seat & 0x7;
    uint8 seatLocationFixed = peep->current_seat & 0xF8;

    // Enterprise has more segments (8) compared to the normal (4)
    if (ride->type != RIDE_TYPE_ENTERPRISE)
        track_direction *= 2;

    // Type 1 loading doesn't do segments and all peeps go to the same
    // location on the ride
    if (vehicle_type->peep_loading_waypoint_segments == 0)
    {
        track_direction /= 2;
        seatLocationSegment = 0;
        seatLocationFixed = 0;
    }
    seatLocationSegment += track_direction;
    seatLocationSegment &= 0x7;
    return seatLocationSegment + seatLocationFixed;
}

static void peep_update_ride_leave_entrance_waypoints(rct_peep * peep, Ride * ride)
{
    TileCoordsXYZD entranceLocation = ride_get_entrance_location(peep->current_ride, peep->current_ride_station);
    Guard::Assert(!entranceLocation.isNull());
    uint8 direction_entrance = entranceLocation.direction;

    LocationXY16 waypoint;
    waypoint.x = ride->station_starts[peep->current_ride_station].x * 32 + 16;
    waypoint.y = ride->station_starts[peep->current_ride_station].y * 32 + 16;

    rct_tile_element * tile_element = ride_get_station_start_track_element(ride, peep->current_ride_station);

    uint8 direction_track = (tile_element == nullptr ? 0 : tile_element_get_direction(tile_element));

    auto vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
    auto ride_entry = get_ride_entry(vehicle->ride_subtype);
    auto vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

    peep->var_37 = (direction_entrance | peep_get_waypointed_seat_location(peep, ride, vehicle_type, direction_track) * 4) * 4;

    if (ride->type == RIDE_TYPE_ENTERPRISE)
    {
        waypoint.x = vehicle->x;
        waypoint.y = vehicle->y;
    }

    Guard::Assert(vehicle_type->peep_loading_waypoints.size() >= (size_t)(peep->var_37 / 4));
    waypoint.x += vehicle_type->peep_loading_waypoints[peep->var_37 / 4][0].x;
    waypoint.y += vehicle_type->peep_loading_waypoints[peep->var_37 / 4][0].y;

    peep->destination_x = waypoint.x;
    peep->destination_y = waypoint.y;
    peep->sub_state = PEEP_RIDE_APPROACH_VEHICLE_WAYPOINTS;
}

/**
 *
 *  rct2: 0x006921D3
 */
void rct_peep::UpdateRideAdvanceThroughEntrance()
{
    sint16 x, y, z, xy_distance;

    Ride *           ride       = get_ride(peep->current_ride);
    rct_ride_entry * ride_entry = get_ride_entry(ride->subtype);

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        uint16 distanceThreshold = 16;
        if (ride_entry != nullptr)
        {
            uint8 vehicle = ride_entry->default_vehicle;
            if (ride_entry->vehicles[vehicle].flags & VEHICLE_ENTRY_FLAG_MINI_GOLF ||
                ride_entry->vehicles[vehicle].flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
            {
                distanceThreshold = 28;
            }
        }

        if (peep->sub_state == PEEP_RIDE_IN_ENTRANCE && xy_distance < distanceThreshold)
        {
            peep->sub_state = PEEP_RIDE_FREE_VEHICLE_CHECK;
        }

        invalidate_sprite_2((rct_sprite *)peep);

        z = ride->station_heights[peep->current_ride_station] * 8;

        distanceThreshold += 4;
        if (xy_distance < distanceThreshold)
        {
            z += RideData5[ride->type].z;
        }

        sprite_move(x, y, z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
        return;
    }

    Guard::Assert(peep->sub_state == PEEP_RIDE_LEAVE_ENTRANCE, "Peep substate should be LEAVE_ENTRACE");
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        TileCoordsXYZD entranceLocation = ride_get_entrance_location(peep->current_ride, peep->current_ride_station);
        Guard::Assert(!entranceLocation.isNull());

        if (ride->type == RIDE_TYPE_MAZE)
        {
            peep_update_ride_leave_entrance_maze(peep, ride, entranceLocation);
            return;
        }
        Guard::Assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);

        peep_update_ride_leave_entrance_spiral_slide(peep, ride, entranceLocation);
        return;
    }

    rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
    for (sint32 i = peep->current_car; i != 0; --i)
    {
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    ride_entry = get_ride_entry(vehicle->ride_subtype);
    if (ride_entry == nullptr)
    {
        return;
    }

    rct_ride_entry_vehicle * vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

    if (vehicle_type->flags & VEHICLE_ENTRY_FLAG_LOADING_WAYPOINTS)
    {
        peep_update_ride_leave_entrance_waypoints(peep, ride);
        return;
    }

    if (vehicle_type->flags & VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT)
    {
        peep->destination_x         = vehicle->x;
        peep->destination_y         = vehicle->y;
        peep->destination_tolerance = 15;
        peep->sub_state             = PEEP_RIDE_APPROACH_VEHICLE;
        return;
    }

    sint8 load_position = 0;
    // Safe, in case current seat > number of loading positions
    uint16 numSeatPositions = static_cast<uint16>(vehicle_type->peep_loading_positions.size());
    if (numSeatPositions != 0)
    {
        size_t loadPositionIndex = numSeatPositions - 1;
        if (peep->current_seat < numSeatPositions)
        {
            loadPositionIndex = peep->current_seat;
        }
        load_position = vehicle_type->peep_loading_positions[loadPositionIndex];
    }

    switch (vehicle->sprite_direction / 8)
    {
    case 0:
        peep->destination_x = vehicle->x - load_position;
        break;
    case 1:
        peep->destination_y = vehicle->y + load_position;
        break;
    case 2:
        peep->destination_x = vehicle->x + load_position;
        break;
    case 3:
        peep->destination_y = vehicle->y - load_position;
        break;
    }

    peep->sub_state = PEEP_RIDE_APPROACH_VEHICLE;
}

/**
 *
 *  rct2: 0x0069321D
 */
static void peep_go_to_ride_exit(rct_peep * peep, Ride * ride, sint16 x, sint16 y, sint16 z, uint8 exit_direction)
{
    z += RideData5[ride->type].z;

    sprite_move(x, y, z, (rct_sprite *)peep);
    invalidate_sprite_2((rct_sprite *)peep);

    Guard::Assert(peep->current_ride_station < MAX_STATIONS);
    auto exit = ride_get_exit_location(ride, peep->current_ride_station);
    Guard::Assert(!exit.isNull());
    x = exit.x;
    y = exit.y;
    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    sint16 x_shift = word_981D6C[exit_direction].x;
    sint16 y_shift = word_981D6C[exit_direction].y;

    sint16 shift_multiplier = 20;

    rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry != nullptr)
    {
        rct_ride_entry_vehicle * vehicle_entry = &rideEntry->vehicles[rideEntry->default_vehicle];
        if (vehicle_entry->flags & VEHICLE_ENTRY_FLAG_MINI_GOLF ||
            vehicle_entry->flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
        {
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x -= x_shift;
    y -= y_shift;

    peep->destination_x         = x;
    peep->destination_y         = y;
    peep->destination_tolerance = 2;

    peep->sprite_direction = exit_direction * 8;
    peep->sub_state = PEEP_RIDE_APPROACH_EXIT;
}

/**
 *
 *  rct2: 0x006920B4
 */
static void peep_update_ride_free_vehicle_enter_ride(rct_peep * peep, Ride * ride)
{
    money16 ridePrice = ride_get_price(ride);
    if (ridePrice != 0)
    {
        if ((peep->item_standard_flags & PEEP_ITEM_VOUCHER) && (peep->voucher_type == VOUCHER_TYPE_RIDE_FREE) &&
            (peep->voucher_arguments == peep->current_ride))
        {

            peep->item_standard_flags &= ~PEEP_ITEM_VOUCHER;
            peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
        }
        else
        {
            ride->total_profit += ridePrice;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
            gCommandExpenditureType = RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS;
            peep_spend_money(peep, &peep->paid_on_rides, ridePrice);
        }
    }

    peep->sub_state = PEEP_RIDE_LEAVE_ENTRANCE;
    uint8 queue_time = peep->days_in_queue;
    if (queue_time < 253)
        queue_time += 3;

    queue_time /= 2;
    if (queue_time != ride->queue_time[peep->current_ride_station])
    {
        ride->queue_time[peep->current_ride_station] = queue_time;
        window_invalidate_by_number(WC_RIDE, peep->current_ride);
    }

    if (peep->peep_flags & PEEP_FLAGS_TRACKING)
    {
        set_format_arg(0, rct_string_id, peep->name_string_idx);
        set_format_arg(2, uint32, peep->id);
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);

        rct_string_id msg_string;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE))
            msg_string = STR_PEEP_TRACKING_PEEP_IS_IN_X;
        else
            msg_string = STR_PEEP_TRACKING_PEEP_IS_ON_X;

        if (gConfigNotifications.guest_on_ride)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, msg_string, peep->sprite_index);
        }
    }

    if (ride->type == RIDE_TYPE_SPIRAL_SLIDE)
    {
        peep_switch_to_special_sprite(peep, 1);
    }

    UpdateRideAdvanceThroughEntrance(peep);
}

/**
 *
 *  rct2: 0x00691FD4
 */
static void peep_update_ride_no_free_vehicle_rejoin_queue(rct_peep * peep, Ride * ride)
{
    TileCoordsXYZD entranceLocation = ride_get_entrance_location(peep->current_ride, peep->current_ride_station);

    sint32 x = entranceLocation.x * 32;
    sint32 y = entranceLocation.y * 32;
    x += 16 - word_981D6C[entranceLocation.direction].x * 20;
    y += 16 - word_981D6C[entranceLocation.direction].y * 20;

    peep->destination_x         = x;
    peep->destination_y         = y;
    peep->destination_tolerance = 2;

    peep_decrement_num_riders(peep);
    peep->state     = PEEP_STATE_QUEUING_FRONT;
    peep->sub_state = PEEP_RIDE_AT_ENTRANCE;
    peep_window_state_update(peep);

    ride_queue_insert_guest_at_front(ride, peep->current_ride_station, peep);
}

/**
 *
 *  rct2: 0x00691E42
 * Note: Before this was the entry
 * point for sub state 1 and 3. The
 * check has been removed that would
 * branch it out to 1 and 3. Now uses
 * separate functions.
 */
void rct_peep::UpdateRideFreeVehicleCheck()
{
    Ride * ride = get_ride(peep->current_ride);

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        if (ride->status != RIDE_STATUS_OPEN || ride->vehicle_change_timeout != 0 || (++peep->rejoin_queue_timeout) == 0)
        {

            peep_update_ride_no_free_vehicle_rejoin_queue(peep, ride);
            return;
        }

        peep_update_ride_free_vehicle_enter_ride(peep, ride);
        return;
    }

    rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
    for (sint32 i = peep->current_car; i != 0; --i)
    {
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    rct_ride_entry * ride_entry = get_ride_entry(vehicle->ride_subtype);
    if (ride_entry == nullptr)
    {
        return;
    }

    if (ride_entry->vehicles[0].flags & VEHICLE_ENTRY_FLAG_MINI_GOLF)
    {
        vehicle->mini_golf_flags &= ~(1 << 5);

        for (size_t i = 0; i < ride->num_vehicles; ++i)
        {
            if (ride->vehicles[i] == SPRITE_INDEX_NULL)
                continue;

            rct_vehicle * train          = GET_VEHICLE(ride->vehicles[i]);
            rct_vehicle * second_vehicle = GET_VEHICLE(train->next_vehicle_on_train);

            if (second_vehicle->num_peeps == 0)
                continue;

            if (second_vehicle->mini_golf_flags & (1 << 5))
                continue;

            return;
        }
    }

    if (!vehicle_is_used_in_pairs(vehicle))
    {
        peep_update_ride_free_vehicle_enter_ride(peep, ride);
        return;
    }

    if (ride->mode == RIDE_MODE_FORWARD_ROTATION || ride->mode == RIDE_MODE_BACKWARD_ROTATION)
    {
        if (peep->current_seat & 1 || !(vehicle->next_free_seat & 1))
        {
            peep_update_ride_free_vehicle_enter_ride(peep, ride);
            return;
        }
    }
    else
    {
        uint8 current_seat = (peep->current_seat & 0xFE) + 1;
        if (current_seat < vehicle->next_free_seat)
        {
            peep_update_ride_free_vehicle_enter_ride(peep, ride);
            return;
        }
    }

    rct_vehicle * currentTrain = GET_VEHICLE(ride->vehicles[peep->current_train]);
    if (ride->status == RIDE_STATUS_OPEN && ++peep->rejoin_queue_timeout != 0 &&
        !(currentTrain->update_flags & VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART))
    {
        return;
    }

    if (ride->mode != RIDE_MODE_FORWARD_ROTATION && ride->mode != RIDE_MODE_BACKWARD_ROTATION)
    {
        if (vehicle->next_free_seat - 1 != peep->current_seat)
            return;
    }

    vehicle->next_free_seat--;
    vehicle->peep[peep->current_seat] = SPRITE_INDEX_NULL;

    peep_update_ride_no_free_vehicle_rejoin_queue(peep, ride);
}

void rct_peep::UpdateRideApproachVehicle()
{
    sint16 x, y, xy_distance;
    if (!peep_update_action(&x, &y, &xy_distance, peep))
    {
        peep->sub_state = PEEP_RIDE_ENTER_VEHICLE;
        return;
    }

    invalidate_sprite_2((rct_sprite *)peep);
    sprite_move(x, y, peep->z, (rct_sprite *)peep);
    invalidate_sprite_2((rct_sprite *)peep);
}

void rct_peep::UpdateRideEnterVehicle()
{
    Ride * ride = get_ride(peep->current_ride);

    rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);
    for (sint32 i = peep->current_car; i != 0; --i)
    {
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    if (ride->mode != RIDE_MODE_FORWARD_ROTATION && ride->mode != RIDE_MODE_BACKWARD_ROTATION)
    {
        if (peep->current_seat != vehicle->num_peeps)
            return;
    }

    if (vehicle_is_used_in_pairs(vehicle))
    {
        rct_peep * seated_peep = GET_PEEP(vehicle->peep[peep->current_seat ^ 1]);
        if (seated_peep->sub_state != PEEP_RIDE_ENTER_VEHICLE)
            return;

        vehicle->num_peeps++;
        ride->cur_num_customers++;

        vehicle->mass += seated_peep->mass;
        invalidate_sprite_2((rct_sprite *)seated_peep);
        sprite_move(LOCATION_NULL, 0, 0, (rct_sprite *)seated_peep);

        peep_decrement_num_riders(seated_peep);
        seated_peep->state = PEEP_STATE_ON_RIDE;
        peep_window_state_update(seated_peep);
        seated_peep->time_on_ride = 0;
        seated_peep->sub_state    = PEEP_RIDE_ON_RIDE;
        peep_on_enter_or_exit_ride(seated_peep, peep->current_ride, 0);
    }

    vehicle->num_peeps++;
    ride->cur_num_customers++;

    vehicle->mass += peep->mass;
    invalidate_sprite_2((rct_sprite *)vehicle);

    invalidate_sprite_2((rct_sprite *)peep);
    sprite_move(LOCATION_NULL, 0, 0, (rct_sprite *)peep);

    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_ON_RIDE;
    peep_window_state_update(peep);

    peep->time_on_ride = 0;
    peep->sub_state    = PEEP_RIDE_ON_RIDE;

    peep_on_enter_or_exit_ride(peep, peep->current_ride, 0);
}

/**
 *
 *  rct2: 0x00693028
 */
void rct_peep::UpdateRideLeaveVehicle()
{
    Ride * ride = get_ride(peep->current_ride);

    rct_vehicle * vehicle      = GET_VEHICLE(ride->vehicles[peep->current_train]);
    uint8         ride_station = vehicle->current_station;

    for (sint32 i = peep->current_car; i != 0; --i)
    {
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    // Check if ride is NOT Ferris Wheel.
    if (ride->mode != RIDE_MODE_FORWARD_ROTATION && ride->mode != RIDE_MODE_BACKWARD_ROTATION)
    {
        if (vehicle->num_peeps - 1 != peep->current_seat)
            return;
    }

    peep->action_sprite_image_offset++;
    if (peep->action_sprite_image_offset & 3)
        return;

    peep->action_sprite_image_offset = 0;

    vehicle->num_peeps--;
    vehicle->mass -= peep->mass;
    invalidate_sprite_2((rct_sprite *)vehicle);

    if (ride_station >= MAX_STATIONS)
    {
        // HACK #5658: Some parks have hacked rides which end up in this state
        sint8 bestStationIndex = ride_get_first_valid_station_exit(ride);
        if (bestStationIndex == -1)
        {
            bestStationIndex = 0;
        }
        ride_station = bestStationIndex;
    }
    peep->current_ride_station  = ride_station;
    rct_ride_entry * ride_entry = get_ride_entry(vehicle->ride_subtype);
    if (ride_entry == nullptr)
    {
        return;
    }

    rct_ride_entry_vehicle * vehicle_entry = &ride_entry->vehicles[vehicle->vehicle_type];

    if (!(vehicle_entry->flags & VEHICLE_ENTRY_FLAG_LOADING_WAYPOINTS))
    {
        assert(peep->current_ride_station < MAX_STATIONS);
        TileCoordsXYZD exitLocation = ride_get_exit_location(peep->current_ride, peep->current_ride_station);
        sint32 x, y;
        sint32 z = ride->station_heights[peep->current_ride_station];

        uint8 exit_direction = exitLocation.direction;
        exit_direction ^= (1 << 1);

        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_16))
        {

            for (; vehicle->is_child; vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride))
            {
                uint16 trackType = vehicle->track_type >> 2;
                if (trackType == TRACK_ELEM_FLAT || trackType > TRACK_ELEM_MIDDLE_STATION)
                    continue;

                rct_tile_element * inner_map = map_get_first_element_at(vehicle->track_x / 32, vehicle->track_y / 32);
                for (;; inner_map++)
                {
                    if (tile_element_get_type(inner_map) != TILE_ELEMENT_TYPE_TRACK)
                        continue;
                    if (inner_map->base_height == vehicle->track_z / 8)
                        break;
                }

                uint8 stationIndex = tile_element_get_station(inner_map);
                if (stationIndex == peep->current_ride_station)
                    break;
            }

            uint8 shift_multiplier = 12;
            uint8 direction        = exit_direction;

            ride_entry = get_ride_entry(ride->subtype);

            if (ride_entry != nullptr)
            {
                vehicle_entry = &ride_entry->vehicles[ride_entry->default_vehicle];

                if (vehicle_entry->flags & VEHICLE_ENTRY_FLAG_GO_KART)
                {
                    shift_multiplier = 9;
                }

                if (vehicle_entry->flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
                {
                    direction = ((vehicle->sprite_direction + 3) / 8) + 1;
                    direction &= 3;

                    if (vehicle->var_CD == 6)
                        direction ^= (1 << 1);
                }
            }

            sint16 x_shift = word_981D6C[direction].x;
            sint16 y_shift = word_981D6C[direction].y;

            x = vehicle->x + x_shift * shift_multiplier;
            y = vehicle->y + y_shift * shift_multiplier;
            z *= 8;

            peep_go_to_ride_exit(peep, ride, x, y, z, exit_direction);
            return;
        }

        x = vehicle->x + word_981D6C[exit_direction].x * 12;
        y = vehicle->y + word_981D6C[exit_direction].y * 12;

        sint8 load_position = vehicle_entry->peep_loading_positions[peep->current_seat];

        switch (vehicle->sprite_direction / 8)
        {
        case 0:
            x -= load_position;
            break;
        case 1:
            y += load_position;
            break;
        case 2:
            x += load_position;
            break;
        case 3:
            y -= load_position;
            break;
        }

        z = ride->station_heights[peep->current_ride_station] * 8;

        peep_go_to_ride_exit(peep, ride, x, y, z, exit_direction);
        return;
    }

    TileCoordsXYZD exitLocation = ride_get_exit_location(peep->current_ride, peep->current_ride_station);
    Guard::Assert(!exitLocation.isNull());
    sint16 z = (sint16)exitLocation.z * 8 + RideData5[ride->type].z;

    uint8 exit_direction = exitLocation.direction;

    sint16 x = ride->station_starts[peep->current_ride_station].x * 32 + 16;
    sint16 y = ride->station_starts[peep->current_ride_station].y * 32 + 16;

    rct_tile_element * tile_element = ride_get_station_start_track_element(ride, peep->current_ride_station);

    uint8 station_direction = (tile_element == nullptr ? 0 : tile_element_get_direction(tile_element));

    vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);

    ride_entry                            = get_ride_entry(vehicle->ride_subtype);
    rct_ride_entry_vehicle * vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

    peep->var_37 = ((exit_direction | peep_get_waypointed_seat_location(peep, ride, vehicle_type, station_direction) * 4) * 4) | 1;

    if (ride->type == RIDE_TYPE_ENTERPRISE)
    {
        x = vehicle->x;
        y = vehicle->y;
    }

    Guard::Assert(vehicle_type->peep_loading_waypoints.size() >= (size_t)(peep->var_37 / 4));
    sint16 exit_x = x + vehicle_type->peep_loading_waypoints[peep->var_37 / 4][2].x;
    sint16 exit_y = y + vehicle_type->peep_loading_waypoints[peep->var_37 / 4][2].y;

    if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        z += 15;

    sprite_move(exit_x, exit_y, z, (rct_sprite *)peep);
    invalidate_sprite_2((rct_sprite *)peep);

    x += vehicle_type->peep_loading_waypoints[peep->var_37 / 4][1].x;
    y += vehicle_type->peep_loading_waypoints[peep->var_37 / 4][1].x;

    peep->destination_x         = x;
    peep->destination_y         = y;
    peep->destination_tolerance = 2;
    peep->sub_state             = PEEP_RIDE_APPROACH_EXIT_WAYPOINTS;
}

/**
 *
 *  rct2: 0x0069376A
 */
static void peep_update_ride_prepare_for_exit(rct_peep * peep)
{
    Ride * ride = get_ride(peep->current_ride);

    Guard::Assert(peep->current_ride_station < Util::CountOf(ride->exits), GUARD_LINE);
    auto exit = ride_get_exit_location(peep->current_ride, peep->current_ride_station);
    sint16 x = exit.x;
    sint16 y = exit.y;
    uint8 exit_direction = exit.direction;

    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    sint16 x_shift = word_981D6C[exit_direction].x;
    sint16 y_shift = word_981D6C[exit_direction].y;

    sint16 shift_multiplier = 20;

    rct_ride_entry * ride_type = get_ride_entry(ride->subtype);
    if (ride_type != nullptr)
    {
        rct_ride_entry_vehicle * vehicle_entry = &ride_type->vehicles[ride_type->default_vehicle];
        if (vehicle_entry->flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
        {
            shift_multiplier = 32;
        }
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x -= x_shift;
    y -= y_shift;

    peep->destination_x         = x;
    peep->destination_y         = y;
    peep->destination_tolerance = 2;
    peep->sub_state             = PEEP_RIDE_IN_EXIT;
}

/**
 *
 *  rct2: 0x0069374F
 */
void rct_peep::UpdateRideApproachExit()
{
    sint16 x, y, xy_distance;
    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        invalidate_sprite_2((rct_sprite *)peep);
        sprite_move(x, y, peep->z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
        return;
    }

    peep_update_ride_prepare_for_exit(peep);
}

/**
 *
 *  rct2: 0x0069382E
 */
void rct_peep::UpdateRideInExit()
{
    sint16 x, y, xy_distance;
    Ride * ride = get_ride(peep->current_ride);

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        invalidate_sprite_2((rct_sprite *)peep);

        if (xy_distance >= 16)
        {
            sint16 z = ride->station_heights[peep->current_ride_station] * 8;

            z += RideData5[ride->type].z;
            sprite_move(x, y, z, (rct_sprite *)peep);
            invalidate_sprite_2((rct_sprite *)peep);
            return;
        }

        peep_switch_to_special_sprite(peep, 0);
        sprite_move(x, y, peep->z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
    {
        uint8 secondaryItem = RidePhotoItems[ride->type];
        if (peep_decide_and_buy_item(peep, peep->current_ride, secondaryItem, ride->price_secondary))
        {
            ride->no_secondary_items_sold++;
        }
    }
    peep->sub_state = PEEP_RIDE_LEAVE_EXIT;
}

/**
 *
 *  rct2: 0x006926AD
 */
void rct_peep::UpdateRideApproachVehicleWaypoints()
{
    sint16 x, y, xy_distance;
    Ride * ride = get_ride(peep->current_ride);
    uint8 waypoint = peep->var_37 & 3;

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        sint16 z;
        // Motion simulators have steps this moves the peeps up the steps
        if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        {
            z = ride->station_heights[peep->current_ride_station] * 8 + 2;

            if (waypoint == 2)
            {
                xy_distance -= 12;
                if (xy_distance < 0)
                    xy_distance = 0;

                if (xy_distance <= 15)
                {
                    z += 15 - xy_distance;
                }
            }
        }
        else
        {
            z = peep->z;
        }
        invalidate_sprite_2((rct_sprite *)peep);
        sprite_move(x, y, z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
        return;
    }

    if (waypoint == 2)
    {
        peep->sub_state = PEEP_RIDE_ENTER_VEHICLE;
        return;
    }

    waypoint++;
    // This is incrementing the actual peep waypoint
    peep->var_37++;

    rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);

    x = ride->station_starts[peep->current_ride_station].x * 32 + 16;
    y = ride->station_starts[peep->current_ride_station].y * 32 + 16;

    if (ride->type == RIDE_TYPE_ENTERPRISE)
    {
        x = vehicle->x;
        y = vehicle->y;
    }

    rct_ride_entry * ride_entry = get_ride_entry(vehicle->ride_subtype);
    if (ride_entry == nullptr)
    {
        return;
    }

    rct_ride_entry_vehicle * vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];
    Guard::Assert(waypoint < 3);
    x += vehicle_type->peep_loading_waypoints[peep->var_37 / 4][waypoint].x;
    y += vehicle_type->peep_loading_waypoints[peep->var_37 / 4][waypoint].y;

    peep->destination_x = x;
    peep->destination_y = y;
}

/**
 *
 *  rct2: 0x0069357D
 */
void rct_peep::UpdateRideApproachExitWaypoints()
{
    sint16 x, y, xy_distance;
    Ride * ride = get_ride(peep->current_ride);

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        sint16 z;
        if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        {
            z = ride->station_heights[peep->current_ride_station] * 8 + 2;

            if ((peep->var_37 & 3) == 1)
            {

                if (xy_distance > 15)
                    xy_distance = 15;

                z += xy_distance;
            }
        }
        else
        {
            z = peep->z;
        }
        invalidate_sprite_2((rct_sprite *)peep);
        sprite_move(x, y, z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
        return;
    }

    if ((peep->var_37 & 3) != 0)
    {
        if ((peep->var_37 & 3) == 3)
        {
            peep_update_ride_prepare_for_exit(peep);
            return;
        }

        peep->var_37--;
        rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);

        x = ride->station_starts[peep->current_ride_station].x;
        y = ride->station_starts[peep->current_ride_station].y;

        x *= 32;
        y *= 32;
        x += 16;
        y += 16;

        if (ride->type == RIDE_TYPE_ENTERPRISE)
        {
            x = vehicle->x;
            y = vehicle->y;
        }

        rct_ride_entry *         ride_entry   = get_ride_entry(vehicle->ride_subtype);
        rct_ride_entry_vehicle * vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];

        Guard::Assert((peep->var_37 & 3) < 3);
        x += vehicle_type->peep_loading_waypoints[peep->var_37 / 4][peep->var_37 & 3].x;
        y += vehicle_type->peep_loading_waypoints[peep->var_37 / 4][peep->var_37 & 3].y;

        peep->destination_x = x;
        peep->destination_y = y;
        return;
    }

    peep->var_37 |= 3;

    auto exit = ride_get_exit_location(peep->current_ride, peep->current_ride_station);
    x = exit.x;
    y = exit.y;
    uint8 exit_direction = exit.direction ^ 2;

    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    sint16 x_shift = word_981D6C[exit_direction].x;
    sint16 y_shift = word_981D6C[exit_direction].y;

    sint16 shift_multiplier = 20;

    rct_ride_entry *         ride_type     = get_ride_entry(ride->subtype);
    rct_ride_entry_vehicle * vehicle_entry = &ride_type->vehicles[ride_type->default_vehicle];
    if (vehicle_entry->flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
    {
        shift_multiplier = 32;
    }

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x -= x_shift;
    y -= y_shift;

    peep->destination_x = x;
    peep->destination_y = y;
}

/**
 *
 *  rct2: 0x006927B3
 */
void rct_peep::UpdateRideApproachSpiralSlide()
{
    sint16 x, y, xy_distance;
    Ride * ride = get_ride(peep->current_ride);

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        invalidate_sprite_2((rct_sprite *)peep);
        sprite_move(x, y, peep->z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
        return;
    }

    uint8 waypoint = peep->var_37 & 3;

    if (waypoint == 3)
    {
        peep->sub_state     = 15;
        peep->destination_x = 0;
        peep->destination_y = 0;
        peep->var_37        = (peep->var_37 / 4) & 0xC;
        sprite_move(LOCATION_NULL, y, peep->z, (rct_sprite *)peep);
        return;
    }
    else if (waypoint == 2)
    {
        uint8 last_ride = 0;
        if (ride->status != RIDE_STATUS_OPEN)
            last_ride = 1;
        else if (peep->current_car++ != 0)
        {
            if (ride->mode == RIDE_MODE_SINGLE_RIDE_PER_ADMISSION)
                last_ride = 1;
            if ((uint8)(peep->current_car - 1) > (peep_rand() & 0xF))
                last_ride = 1;
        }

        if (last_ride)
        {
            auto exit = ride_get_exit_location(peep->current_ride, peep->current_ride_station);
            uint8 exit_direction = exit.direction;

            waypoint = 1;
            peep->var_37 = (exit_direction * 4) | (peep->var_37 & 0x30) | waypoint;
            x            = ride->station_starts[peep->current_ride_station].x;
            y            = ride->station_starts[peep->current_ride_station].y;

            x *= 32;
            y *= 32;
            assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
            const CoordsXY slidePlatformDestination = SpiralSlideWalkingPath[peep->var_37];

            x += slidePlatformDestination.x;
            y += slidePlatformDestination.y;

            peep->destination_x = x;
            peep->destination_y = y;
            peep->sub_state     = PEEP_RIDE_LEAVE_SPIRAL_SLIDE;
            return;
        }
    }
    waypoint++;
    // Actually increment the real peep waypoint
    peep->var_37++;

    x = ride->station_starts[peep->current_ride_station].x;
    y = ride->station_starts[peep->current_ride_station].y;

    x *= 32;
    y *= 32;

    assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
    const CoordsXY slidePlatformDestination = SpiralSlideWalkingPath[peep->var_37];

    x += slidePlatformDestination.x;
    y += slidePlatformDestination.y;

    peep->destination_x = x;
    peep->destination_y = y;
}

/** rct2: 0x00981F0C, 0x00981F0E */
static constexpr const CoordsXY _981F0C[] = {
    { 25, 56 },
    { 56, 7 },
    { 7, -24 },
    { -24, 25 },
};

/** rct2: 0x00981F1C, 0x00981F1E */
static constexpr const CoordsXY _981F1C[] = {
    { 8, 56 },
    { 56, 24 },
    { 24, -24 },
    { -24, 8 },
};

/**
 *
 *  rct2: 0x00692D83
 */
void rct_peep::UpdateRideOnSpiralSlide()
{
    Ride * ride = get_ride(peep->current_ride);

    if (ride->type != RIDE_TYPE_SPIRAL_SLIDE)
        return;

    if ((peep->var_37 & 3) == 0)
    {
        switch (peep->destination_x)
        {
        case 0:
            peep->destination_y++;
            if (peep->destination_y >= 30)
                peep->destination_x++;
            return;
        case 1:
            if (ride->slide_in_use != 0)
                return;

            ride->slide_in_use++;
            ride->slide_peep                = peep->sprite_index;
            ride->slide_peep_t_shirt_colour = peep->tshirt_colour;
            ride->spiral_slide_progress     = 0;
            peep->destination_x++;
            return;
        case 2:
            return;
        case 3:
        {
            sint16 x = ride->station_starts[peep->current_ride_station].x;
            sint16 y = ride->station_starts[peep->current_ride_station].y;

            x *= 32;
            y *= 32;

            uint8  direction = (peep->var_37 / 4) & 3;
            sint16 dest_x    = x + _981F1C[direction].x;
            sint16 dest_y    = y + _981F1C[direction].y;

            peep->destination_x = dest_x;
            peep->destination_y = dest_y;

            x += _981F0C[direction].x;
            y += _981F0C[direction].y;

            sprite_move(x, y, peep->z, (rct_sprite *)peep);

            peep->sprite_direction = (peep->var_37 & 0xC) * 2;

            invalidate_sprite_2((rct_sprite *)peep);

            peep->var_37++;
            return;
        }
        default:
            return;
        }
    }

    sint16 x, y, xy_distance;

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        invalidate_sprite_2((rct_sprite *)peep);
        sprite_move(x, y, peep->z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
        return;
    }
    uint8 waypoint = 2;
    peep->var_37 = (peep->var_37 * 4 & 0x30) + waypoint;

    x = ride->station_starts[peep->current_ride_station].x;
    y = ride->station_starts[peep->current_ride_station].y;

    x *= 32;
    y *= 32;

    assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
    const CoordsXY slidePlatformDestination = SpiralSlideWalkingPath[peep->var_37];

    x += slidePlatformDestination.x;
    y += slidePlatformDestination.y;

    peep->destination_x = x;
    peep->destination_y = y;
    peep->sub_state     = PEEP_RIDE_APPROACH_SPIRAL_SLIDE;
}

/**
 *
 *  rct2: 0x00692C6B
 */
void rct_peep::UpdateRideLeaveSpiralSlide()
{
    // Iterates through the spiral slide waypoints until it reaches 
    // waypoint 0. Then it readies to leave the ride by the entrance.
    sint16 x, y, xy_distance;

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        invalidate_sprite_2((rct_sprite *)peep);
        sprite_move(x, y, peep->z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
        return;
    }

    Ride * ride = get_ride(peep->current_ride);

    uint8 waypoint = peep->var_37 & 3;

    if (waypoint != 0)
    {
        if (waypoint == 3)
        {
            peep_update_ride_prepare_for_exit(peep);
            return;
        }

        waypoint--;
        // Actually decrement the peep waypoint
        peep->var_37--;
        x = ride->station_starts[peep->current_ride_station].x * 32;
        y = ride->station_starts[peep->current_ride_station].y * 32;

        assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
        const CoordsXY slidePlatformDestination = SpiralSlideWalkingPath[peep->var_37];

        x += slidePlatformDestination.x;
        y += slidePlatformDestination.y;

        peep->destination_x = x;
        peep->destination_y = y;
        return;
    }
    waypoint = 3;
    // Actually force the final waypoint
    peep->var_37 |= 3;

    auto exit = ride_get_exit_location(peep->current_ride, peep->current_ride_station);
    x         = exit.x * 32 + 16;
    y         = exit.y * 32 + 16;

    uint8 exit_direction = exit.direction ^ 2;

    sint16 x_shift = word_981D6C[exit_direction].x;
    sint16 y_shift = word_981D6C[exit_direction].y;

    sint16 shift_multiplier = 20;

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    x -= x_shift;
    y -= y_shift;

    peep->destination_x = x;
    peep->destination_y = y;
}

/** rct2: 0x00981FE4 */
static constexpr const uint8 _981FE4[][4] = {
    { 15, 7, 15, 7 },
    { 11, 3, 11, 3 },
    { 7, 15, 7, 15 },
    { 3, 11, 3, 11 },
};

/** rct2: 0x00981FF4 */
static constexpr const uint8 _981FF4[][4] = {
    { 1, 2, 14, 0 },
    { 4, 5, 6, 2 },
    { 6, 8, 9, 10 },
    { 14, 10, 12, 13 },
};

/**
 *
 *  rct2: 0x00692A83
 */
void rct_peep::UpdateRideMazePathfinding()
{
    sint16 x, y, xy_distance;

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        invalidate_sprite_2((rct_sprite *)peep);
        sprite_move(x, y, peep->z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
        return;
    }

    Ride * ride = get_ride(peep->current_ride);
    if (peep->var_37 == 16)
    {
        peep_update_ride_prepare_for_exit(peep);
        return;
    }

    if (peep->action >= PEEP_ACTION_NONE_1)
    {
        if (peep->energy > 64 && (peep_rand() & 0xFFFF) <= 2427)
        {

            peep->action                     = PEEP_ACTION_JUMP;
            peep->action_frame               = 0;
            peep->action_sprite_image_offset = 0;
            peep_update_current_action_sprite_type(peep);
            invalidate_sprite_2((rct_sprite *)peep);
        }
    }

    x        = peep->destination_x & 0xFFE0;
    y        = peep->destination_y & 0xFFE0;
    sint16 z = ride->station_heights[0];

    // Find the station track element
    rct_tile_element * tileElement = map_get_first_element_at(x / 32, y / 32);
    do
    {
        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_TRACK && z == tileElement->base_height)
            break;

    } while (!tile_element_is_last_for_tile(tileElement++));

    uint16 maze_entry  = track_element_get_maze_entry(tileElement);
    uint16 open_hedges = 0;
    uint8  var_37      = peep->var_37;
    // var_37 is 3, 7, 11 or 15

    if (maze_entry & (1 << _981FF4[var_37 / 4][3]))
    {
        open_hedges = 1;
    }
    open_hedges <<= 1;
    if (maze_entry & (1 << _981FF4[var_37 / 4][2]))
    {
        open_hedges |= 1;
    }
    open_hedges <<= 1;
    if (maze_entry & (1 << _981FF4[var_37 / 4][1]))
    {
        open_hedges |= 1;
    }
    open_hedges <<= 1;
    if (maze_entry & (1 << _981FF4[var_37 / 4][0]))
    {
        open_hedges |= 1;
    }

    open_hedges ^= 0xF;
    if (open_hedges == 0)
        return;

    uint8 maze_last_edge = peep->maze_last_edge ^ (1 << 1);
    open_hedges &= ~(1 << maze_last_edge);
    if (open_hedges == 0)
        open_hedges |= (1 << maze_last_edge);

    uint8 chosen_edge = peep_rand() & 0x3;
    while (!(open_hedges & (1 << chosen_edge)))
    {
        chosen_edge = (chosen_edge + 1) & 3;
    }

    x = TileDirectionDelta[chosen_edge].x / 2;
    y = TileDirectionDelta[chosen_edge].y / 2;

    x += peep->destination_x;
    y += peep->destination_y;

    uint8 type = 0;

    tileElement = map_get_first_element_at(x / 32, y / 32);
    do
    {
        if (z != tileElement->base_height)
            continue;

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_TRACK)
        {
            type = 1;
            break;
        }

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_ENTRANCE &&
            tileElement->properties.entrance.type == ENTRANCE_TYPE_RIDE_EXIT)
        {
            type = 2;
            break;
        }
    } while (!tile_element_is_last_for_tile(tileElement++));

    switch (type)
    {
    case 0:
        peep->maze_last_edge++;
        peep->maze_last_edge &= 3;
        return;
    case 1:
        peep->destination_x = x;
        peep->destination_y = y;

        peep->var_37         = _981FE4[peep->var_37 / 4][chosen_edge];
        peep->maze_last_edge = chosen_edge;
        break;
    case 2:
        x = peep->destination_x;
        y = peep->destination_y;
        if (chosen_edge & 1)
        {
            x &= 0xFFE0;
            x += 16;
        }
        else
        {
            y &= 0xFFE0;
            y += 16;
        }
        peep->destination_x  = x;
        peep->destination_y  = y;
        peep->var_37         = 16;
        peep->maze_last_edge = chosen_edge;
        break;
    }

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        invalidate_sprite_2((rct_sprite *)peep);
        sprite_move(x, y, peep->z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
        return;
    }
}

/**
 *
 *  rct2: 0x006938D2
 */
void rct_peep::UpdateRideLeaveExit()
{
    sint16 x, y, xy_distance;
    Ride * ride = get_ride(peep->current_ride);

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        invalidate_sprite_2((rct_sprite *)peep);
        sprite_move(x, y, ride->station_heights[peep->current_ride_station] * 8, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
        return;
    }

    peep_on_enter_or_exit_ride(peep, peep->current_ride, 1);

    if (peep->peep_flags & PEEP_FLAGS_TRACKING)
    {
        set_format_arg(0, rct_string_id, peep->name_string_idx);
        set_format_arg(2, uint32, peep->id);
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);

        if (gConfigNotifications.guest_left_ride)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, STR_PEEP_TRACKING_LEFT_RIDE_X, peep->sprite_index);
        }
    }

    peep->interaction_ride_index = 0xFF;
    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_FALLING;
    peep_window_state_update(peep);

    x = peep->x & 0xFFE0;
    y = peep->y & 0xFFE0;

    // Find the station track element
    rct_tile_element * tileElement = map_get_first_element_at(x / 32, y / 32);
    do
    {
        if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_PATH)
            continue;

        sint16 z = map_height_from_slope(peep->x, peep->y, tileElement->properties.path.type);
        z += tileElement->base_height * 8;

        sint16 z_diff = peep->z - z;
        if (z_diff > 0 || z_diff < -16)
            continue;

        sprite_move(peep->x, peep->y, z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
        return;
    } while (!tile_element_is_last_for_tile(tileElement++));
}

/**
 *
 *  rct2: 0x0069299C
 */
void rct_peep::UpdateRideShopApproach()
{
    sint16 x, y, xy_distance;

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        invalidate_sprite_2((rct_sprite *)peep);
        sprite_move(x, y, peep->z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);
        return;
    }

    peep->sub_state = PEEP_SHOP_INTERACT;
}

/**
 *
 *  rct2: 0x006929BB
 */
void rct_peep::UpdateRideShopInteract()
{
    sint16 x, y;
    Ride * ride = get_ride(peep->current_ride);

    if (ride->type == RIDE_TYPE_FIRST_AID)
    {
        if (peep->nausea <= 35)
        {
            peep->sub_state = 21;

            x                           = peep->next_x + 16;
            y                           = peep->next_y + 16;
            peep->destination_x         = x;
            peep->destination_y         = y;
            peep->destination_tolerance = 3;
            peep->happiness_target      = Math::Min(peep->happiness_target + 30, PEEP_MAX_HAPPINESS);
            peep->happiness             = peep->happiness_target;
        }
        else
        {
            peep->nausea--;
            peep->nausea_target = peep->nausea;
        }
        return;
    }

    if (peep->toilet != 0)
    {
        peep->toilet--;
        return;
    }

    // Do not play toilet flush sound on title screen as it's considered loud and annoying
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        audio_play_sound_at_location(SOUND_TOILET_FLUSH, peep->x, peep->y, peep->z);
    }

    peep->sub_state = PEEP_SHOP_LEAVE;

    x                           = peep->next_x + 16;
    y                           = peep->next_y + 16;
    peep->destination_x         = x;
    peep->destination_y         = y;
    peep->destination_tolerance = 3;

    peep->happiness_target = Math::Min(peep->happiness_target + 30, PEEP_MAX_HAPPINESS);
    peep->happiness        = peep->happiness_target;

    peep_stop_purchase_thought(peep, ride->type);
}

/**
 *
 *  rct2: 0x00692935
 */
void rct_peep::UpdateRideShopLeave()
{
    sint16 x, y, xy_distance;

    if (peep_update_action(&x, &y, &xy_distance, peep))
    {
        invalidate_sprite_2((rct_sprite *)peep);
        sprite_move(x, y, peep->z, (rct_sprite *)peep);
        invalidate_sprite_2((rct_sprite *)peep);

        x = peep->x & 0xFFE0;
        y = peep->y & 0xFFE0;
        if (x != peep->next_x)
            return;
        if (y != peep->next_y)
            return;
    }

    peep_decrement_num_riders(peep);
    peep->state = PEEP_STATE_WALKING;
    peep_window_state_update(peep);

    Ride * ride = get_ride(peep->current_ride);
    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

    ride_update_satisfaction(ride, peep->happiness / 64);
}

/**
 *
 *  rct2: 0x691A30
 * Used by entering_ride and queueing_front */
void rct_peep::UpdateRide()
{
    switch (sub_state)
    {
    case PEEP_RIDE_AT_ENTRANCE:
        UpdateRideAtEntrance();
        break;
    case PEEP_RIDE_IN_ENTRANCE:
        UpdateRideAdvanceThroughEntrance();
        break;
    case PEEP_RIDE_FREE_VEHICLE_CHECK:
        UpdateRideFreeVehicleCheck();
        break;
    case PEEP_RIDE_LEAVE_ENTRANCE:
        UpdateRideAdvanceThroughEntrance();
        break;
    case PEEP_RIDE_APPROACH_VEHICLE:
        UpdateRideApproachVehicle();
        break;
    case PEEP_RIDE_ENTER_VEHICLE:
        UpdateRideEnterVehicle();
        break;
    case PEEP_RIDE_ON_RIDE:
        // No action, on ride.
        break;
    case PEEP_RIDE_LEAVE_VEHICLE:
        UpdateRideLeaveVehicle();
        break;
    case PEEP_RIDE_APPROACH_EXIT:
        UpdateRideApproachExit();
        break;
    case PEEP_RIDE_IN_EXIT:
        UpdateRideInExit();
        break;
    case 10:
    case 11:
        assert(false);
        break;
    case PEEP_RIDE_APPROACH_VEHICLE_WAYPOINTS:
        UpdateRideApproachVehicleWaypoints();
        break;
    case PEEP_RIDE_APPROACH_EXIT_WAYPOINTS:
        UpdateRideApproachExitWaypoints();
        break;
    case PEEP_RIDE_APPROACH_SPIRAL_SLIDE:
        UpdateRideApproachSpiralSlide();
        break;
    case PEEP_RIDE_ON_SPIRAL_SLIDE:
        UpdateRideOnSpiralSlide();
        break;
    case PEEP_RIDE_LEAVE_SPIRAL_SLIDE:
        UpdateRideLeaveSpiralSlide();
        break;
    case PEEP_RIDE_MAZE_PATHFINDING:
        UpdateRideMazePathfinding();
        break;
    case PEEP_RIDE_LEAVE_EXIT:
        UpdateRideLeaveExit();
        break;
    case PEEP_SHOP_APPROACH:
        UpdateRideShopApproach();
        break;
    case PEEP_SHOP_INTERACT:
        UpdateRideShopInteract();
        break;
    case PEEP_SHOP_LEAVE:
        UpdateRideShopLeave();
        break;
    default:
        // Invalid peep sub-state
        assert(false);
        break;
    }
}
