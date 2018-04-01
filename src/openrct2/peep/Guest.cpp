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

#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../core/Math.hpp"
#include "../core/Util.hpp"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../OpenRCT2.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../world/Footpath.h"
#include "../world/Park.h"
#include "../world/Sprite.h"
#include "Peep.h"

// Locations of the spiral slide platform that a peep walks from the entrance of the ride to the
// entrance of the slide. Up to 4 waypoints for each 4 sides that an ride entrance can be located
// and 4 different rotations of the ride. 4 * 4 * 4 = 64 locations.
static constexpr const CoordsXY SpiralSlideWalkingPath[64] = {
    {  56,   8 },
    {   8,   8 },
    {   8,  32 },
    {  32,  32 },
    {   8,   8 },
    {   8,   8 },
    {   8,  32 },
    {  32,  32 },
    {   8,  32 },
    {   8,  32 },
    {   8,  32 },
    {  32,  32 },
    {   8,  56 },
    {   8,  32 },
    {   8,  32 },
    {  32,  32 },
    {  56,  24 },
    {  32,  24 },
    {  32,  24 },
    {  32,   0 },
    {  56, -24 },
    {  56,  24 },
    {  32,  24 },
    {  32,   0 },
    {   8,  24 },
    {  32,  24 },
    {  32,  24 },
    {  32,   0 },
    {  32,  24 },
    {  32,  24 },
    {  32,  24 },
    {  32,   0 },
    {  24,   0 },
    {  24,   0 },
    {  24,   0 },
    {   0,   0 },
    {  24, -24 },
    {  24,   0 },
    {  24,   0 },
    {   0,   0 },
    { -24, -24 },
    {  24, -24 },
    {  24,   0 },
    {   0,   0 },
    {  24,  24 },
    {  24,   0 },
    {  24,   0 },
    {   0,   0 },
    {  24,   8 },
    {   0,   8 },
    {   0,   8 },
    {   0,  32 },
    {   0,   8 },
    {   0,   8 },
    {   0,   8 },
    {   0,  32 },
    { -24,   8 },
    {   0,   8 },
    {   0,   8 },
    {   0,  32 },
    { -24,  56 },
    { -24,   8 },
    {   0,   8 },
    {   0,  32 },
};

/**
 *
 *  rct2: 0x00691677
 */
void rct_peep::TryGetUpFromSitting()
{
    // Eats all food first
    if (peep_has_food(this))
        return;

    time_to_sitdown--;
    if (time_to_sitdown)
        return;

    SetState(PEEP_STATE_WALKING);

    // Set destination to the centre of the tile.
    destination_x         = (x & 0xFFE0) + 16;
    destination_y         = (y & 0xFFE0) + 16;
    destination_tolerance = 5;
    UpdateCurrentActionSpriteType();
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

        Invalidate();
        sprite_move(loc.x, loc.y, loc.z, (rct_sprite *)this);

        sprite_direction = ((var_37 + 2) & 3) * 8;
        Invalidate();
        action                  = PEEP_ACTION_NONE_1;
        next_action_sprite_type = 7;
        SwitchNextActionSpriteType();

        sub_state = PEEP_SITTING_SAT_DOWN;

        // Sets time to sit on seat
        time_to_sitdown = (129 - energy) * 16 + 50;
    }
    else if (sub_state == PEEP_SITTING_SAT_DOWN)
    {
        if (action < PEEP_ACTION_NONE_1)
        {
            UpdateAction();
            if (action != PEEP_ACTION_NONE_2)
                return;

            action = PEEP_ACTION_NONE_1;
            TryGetUpFromSitting();
            return;
        }

        if ((peep_flags & PEEP_FLAGS_LEAVING_PARK))
        {
            SetState(PEEP_STATE_WALKING);

            // Set destination to the centre of the tile
            destination_x         = (x & 0xFFE0) + 16;
            destination_y         = (y & 0xFFE0) + 16;
            destination_tolerance = 5;
            UpdateCurrentActionSpriteType();
            return;
        }

        if (sprite_type == PEEP_SPRITE_TYPE_UMBRELLA)
        {
            TryGetUpFromSitting();
            return;
        }

        if (peep_has_food(this))
        {
            if ((scenario_rand() & 0xFFFF) > 1310)
            {
                TryGetUpFromSitting();
                return;
            }
            action                     = PEEP_ACTION_SITTING_EAT_FOOD;
            action_frame               = 0;
            action_sprite_image_offset = 0;
            UpdateCurrentActionSpriteType();
            Invalidate();
            return;
        }

        sint32 rand = scenario_rand();
        if ((rand & 0xFFFF) > 131)
        {
            TryGetUpFromSitting();
            return;
        }
        if (sprite_type == PEEP_SPRITE_TYPE_BALLOON || sprite_type == PEEP_SPRITE_TYPE_HAT)
        {
            TryGetUpFromSitting();
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
        UpdateCurrentActionSpriteType();
        Invalidate();
        return;
    }
}

/**
 *
 *  rct2: 0x006966A9
 */
void rct_peep::RemoveFromQueue()
{
    Ride * ride = get_ride(current_ride);

    uint8 cur_station = current_ride_station;
    // Make sure we don't underflow, building while paused might reset it to 0 where peeps have
    // not yet left the queue.
    if (ride->queue_length[cur_station] > 0)
    {
        ride->queue_length[cur_station]--;
    }

    if (sprite_index == ride->last_peep_in_queue[cur_station])
    {
        ride->last_peep_in_queue[cur_station] = next_in_queue;
        return;
    }

    uint16 spriteId = ride->last_peep_in_queue[cur_station];
    while (spriteId != SPRITE_INDEX_NULL)
    {
        rct_peep * other_peep = GET_PEEP(spriteId);
        if (sprite_index == other_peep->next_in_queue)
        {
            other_peep->next_in_queue = next_in_queue;
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
    uint8 chosen_car = scenario_rand();
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_G_FORCES) && ((chosen_car & 0xC) != 0xC))
    {
        chosen_car = (scenario_rand() & 1) ? 0 : (uint8)car_array.size() - 1;
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
 *  rct2: 0x00699FE3
 * Stops peeps that are having thoughts
 * such as "I'm hungry" after visiting a food shop.
 * Works for Thirst/Hungry/Low Money/Bathroom
 */
static void peep_stop_purchase_thought(rct_peep * peep, uint8 ride_type)
{

    uint8 thought_type = PEEP_THOUGHT_TYPE_HUNGRY;

    if (!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_SELLS_FOOD))
    {
        thought_type = PEEP_THOUGHT_TYPE_THIRSTY;
        if (!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_SELLS_DRINKS))
        {
            thought_type = PEEP_THOUGHT_TYPE_RUNNING_OUT;
            if (ride_type != RIDE_TYPE_CASH_MACHINE)
            {
                thought_type = PEEP_THOUGHT_TYPE_BATHROOM;
                if (!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_IS_BATHROOM))
                {
                    return;
                }
            }
        }
    }

    // Remove the related thought
    for (sint32 i = 0; i < PEEP_MAX_THOUGHTS; ++i)
    {
        rct_peep_thought * thought = &peep->thoughts[i];

        if (thought->type == PEEP_THOUGHT_TYPE_NONE)
            break;

        if (thought->type != thought_type)
            continue;

        if (i < PEEP_MAX_THOUGHTS - 1)
        {
            memmove(thought, thought + 1, sizeof(rct_peep_thought) * (PEEP_MAX_THOUGHTS - i - 1));
        }

        peep->thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;

        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
        i--;
    }
}

/**
 *
 *  rct2: 0x0069AEB7
 */
static bool peep_should_use_cash_machine(rct_peep * peep, sint32 rideIndex)
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
        return false;
    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
        return false;
    if (peep->cash_in_pocket > MONEY(20, 00))
        return false;
    if (115 + (scenario_rand() % 128) > peep->happiness)
        return false;
    if (peep->energy < 80)
        return false;

    Ride * ride = get_ride(rideIndex);
    ride_update_satisfaction(ride, peep->happiness >> 6);
    ride->cur_num_customers++;
    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
    return true;
}

/**
 *
 *  rct2: 0x006912A3
 */
void rct_peep::UpdateBuying()
{
    if (!CheckForPath())
        return;

    Ride * ride = get_ride(current_ride);
    if (ride->type == RIDE_TYPE_NULL || ride->status != RIDE_STATUS_OPEN)
    {
        peep_decrement_num_riders(this);
        state = PEEP_STATE_FALLING;
        peep_window_state_update(this);
        return;
    }

    if (sub_state == 1)
    {
        if (action != 0xFF)
        {
            sint16 actionX;
            sint16 actionY;
            sint16 xy_distance;
            UpdateAction(&actionX, &actionY, &xy_distance);
            return;
        }

        if (ride->type == RIDE_TYPE_CASH_MACHINE)
        {
            if (current_ride != previous_ride)
            {
                cash_in_pocket += MONEY(50, 00);
            }
            window_invalidate_by_number(WC_PEEP, sprite_index);
        }
        sprite_direction ^= 0x10;
        destination_x = next_x + 16;
        destination_y = next_y + 16;
        direction ^= 2;

        peep_decrement_num_riders(this);
        state = PEEP_STATE_WALKING;
        peep_window_state_update(this);
        return;
    }

    bool item_bought = false;

    if (current_ride != previous_ride)
    {
        if (ride->type == RIDE_TYPE_CASH_MACHINE)
        {
            item_bought = peep_should_use_cash_machine(this, current_ride);
            if (!item_bought)
            {
                previous_ride = current_ride;
                previous_ride_time_out = 0;
            }
            else
            {
                action = PEEP_ACTION_WITHDRAW_MONEY;
                action_frame = 0;
                action_sprite_image_offset = 0;

                UpdateCurrentActionSpriteType();
                invalidate_sprite_2((rct_sprite *)this);

                ride->no_primary_items_sold++;
            }
        }
        else
        {
            rct_ride_entry * ride_type = get_ride_entry(ride->subtype);
            if (ride_type == nullptr)
            {
                return;
            }
            if (ride_type->shop_item_secondary != SHOP_ITEM_NONE)
            {
                money16 price = ride->price_secondary;

                item_bought = DecideAndBuyItem(current_ride, ride_type->shop_item_secondary, price);
                if (item_bought)
                {
                    ride->no_secondary_items_sold++;
                }
            }

            if (!item_bought && ride_type->shop_item != SHOP_ITEM_NONE)
            {
                money16 price = ride->price;

                item_bought = DecideAndBuyItem(current_ride, ride_type->shop_item, price);
                if (item_bought)
                {
                    ride->no_primary_items_sold++;
                }
            }
        }
    }

    if (item_bought)
    {
        ride_update_popularity(ride, 1);

        peep_stop_purchase_thought(this, ride->type);
    }
    else
    {
        ride_update_popularity(ride, 0);
    }
    sub_state = 1;
}

/**
 *
 *  rct2: 0x00691A3B
 */
void rct_peep::UpdateRideAtEntrance()
{
    Ride * ride = get_ride(current_ride);

    // The peep will keep advancing in the entranceway
    // whilst in this state. When it has reached the very
    // front of the queue destination tolerance is set to 
    // zero to indicate it is final decision time (try_leave will pass).
    // When a peep has to return to the queue without getting on a ride
    // this is the state it will return to.
    if (destination_tolerance != 0)
    {
        Invalidate();

        sint16 actionX, actionY, xy_distance;

        if (UpdateAction(&actionX, &actionY, &xy_distance))
        {
            sint16 actionZ = z;
            if (xy_distance < 16)
            {
                auto entrance = ride_get_entrance_location(ride, current_ride_station);
                actionZ = entrance.z * 8 + 2;
            }
            sprite_move(actionX, actionY, actionZ, (rct_sprite *)this);
            Invalidate();
        }
        else
        {
            destination_tolerance = 0;
            sprite_direction ^= (1 << 4);
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
        if (!peep_find_vehicle_to_enter(this, ride, carArray))
            return;
    }

    if (ride->status != RIDE_STATUS_OPEN || ride->vehicle_change_timeout != 0)
    {
        peep_update_ride_at_entrance_try_leave(this);
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
        return;

    money16 ridePrice = ride_get_price(ride);
    if (ridePrice != 0)
    {
        if (!peep_check_ride_price_at_entrance(this, ride, ridePrice))
            return;
    }

    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        rct_vehicle * vehicle = peep_choose_car_from_ride(this, ride, carArray);
        peep_choose_seat_from_car(this, ride, vehicle);
    }
    peep_go_to_ride_entrance(this, ride);
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
    if (scenario_rand() & 0x40)
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
    peep->OnEnterRide(peep->current_ride);
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
    peep->OnEnterRide(peep->current_ride);
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
    sint16 actionX, actionY, actionZ, xy_distance;

    Ride *           ride       = get_ride(current_ride);
    rct_ride_entry * ride_entry = get_ride_entry(ride->subtype);

    if (UpdateAction(&actionX, &actionY, &xy_distance))
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

        if (sub_state == PEEP_RIDE_IN_ENTRANCE && xy_distance < distanceThreshold)
        {
            sub_state = PEEP_RIDE_FREE_VEHICLE_CHECK;
        }

        Invalidate();

        actionZ = ride->station_heights[current_ride_station] * 8;

        distanceThreshold += 4;
        if (xy_distance < distanceThreshold)
        {
            actionZ += RideData5[ride->type].z;
        }

        sprite_move(actionX, actionY, actionZ, (rct_sprite *)this);
        Invalidate();
        return;
    }

    Guard::Assert(sub_state == PEEP_RIDE_LEAVE_ENTRANCE, "Peep substate should be LEAVE_ENTRACE");
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        TileCoordsXYZD entranceLocation = ride_get_entrance_location(current_ride, current_ride_station);
        Guard::Assert(!entranceLocation.isNull());

        if (ride->type == RIDE_TYPE_MAZE)
        {
            peep_update_ride_leave_entrance_maze(this, ride, entranceLocation);
            return;
        }
        Guard::Assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);

        peep_update_ride_leave_entrance_spiral_slide(this, ride, entranceLocation);
        return;
    }

    rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[current_train]);
    for (sint32 i = current_car; i != 0; --i)
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
        peep_update_ride_leave_entrance_waypoints(this, ride);
        return;
    }

    if (vehicle_type->flags & VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT)
    {
        destination_x         = vehicle->x;
        destination_y         = vehicle->y;
        destination_tolerance = 15;
        sub_state             = PEEP_RIDE_APPROACH_VEHICLE;
        return;
    }

    sint8 load_position = 0;
    // Safe, in case current seat > number of loading positions
    uint16 numSeatPositions = static_cast<uint16>(vehicle_type->peep_loading_positions.size());
    if (numSeatPositions != 0)
    {
        size_t loadPositionIndex = numSeatPositions - 1;
        if (current_seat < numSeatPositions)
        {
            loadPositionIndex = current_seat;
        }
        load_position = vehicle_type->peep_loading_positions[loadPositionIndex];
    }

    switch (vehicle->sprite_direction / 8)
    {
    case 0:
        destination_x = vehicle->x - load_position;
        break;
    case 1:
        destination_y = vehicle->y + load_position;
        break;
    case 2:
        destination_x = vehicle->x + load_position;
        break;
    case 3:
        destination_y = vehicle->y - load_position;
        break;
    }

    sub_state = PEEP_RIDE_APPROACH_VEHICLE;
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
void rct_peep::UpdateRideFreeVehicleEnterRide(Ride * ride)
{
    money16 ridePrice = ride_get_price(ride);
    if (ridePrice != 0)
    {
        if ((item_standard_flags & PEEP_ITEM_VOUCHER) && (voucher_type == VOUCHER_TYPE_RIDE_FREE) &&
            (voucher_arguments == current_ride))
        {

            item_standard_flags &= ~PEEP_ITEM_VOUCHER;
            window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
        }
        else
        {
            ride->total_profit += ridePrice;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
            gCommandExpenditureType = RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS;
            SpendMoney(paid_on_rides, ridePrice);
        }
    }

    sub_state = PEEP_RIDE_LEAVE_ENTRANCE;
    uint8 queueTime = days_in_queue;
    if (queueTime < 253)
        queueTime += 3;

    queueTime /= 2;
    if (queueTime != ride->queue_time[current_ride_station])
    {
        ride->queue_time[current_ride_station] = queueTime;
        window_invalidate_by_number(WC_RIDE, current_ride);
    }

    if (peep_flags & PEEP_FLAGS_TRACKING)
    {
        set_format_arg(0, rct_string_id, name_string_idx);
        set_format_arg(2, uint32, id);
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);

        rct_string_id msg_string;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE))
            msg_string = STR_PEEP_TRACKING_PEEP_IS_IN_X;
        else
            msg_string = STR_PEEP_TRACKING_PEEP_IS_ON_X;

        if (gConfigNotifications.guest_on_ride)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, msg_string, sprite_index);
        }
    }

    if (ride->type == RIDE_TYPE_SPIRAL_SLIDE)
    {
        SwitchToSpecialSprite(1);
    }

    UpdateRideAdvanceThroughEntrance();
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
    Ride * ride = get_ride(current_ride);

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        if (ride->status != RIDE_STATUS_OPEN || ride->vehicle_change_timeout != 0 || (++rejoin_queue_timeout) == 0)
        {

            peep_update_ride_no_free_vehicle_rejoin_queue(this, ride);
            return;
        }

        UpdateRideFreeVehicleEnterRide(ride);
        return;
    }

    rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[current_train]);
    for (sint32 i = current_car; i != 0; --i)
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
        UpdateRideFreeVehicleEnterRide(ride);
        return;
    }

    if (ride->mode == RIDE_MODE_FORWARD_ROTATION || ride->mode == RIDE_MODE_BACKWARD_ROTATION)
    {
        if (current_seat & 1 || !(vehicle->next_free_seat & 1))
        {
            UpdateRideFreeVehicleEnterRide(ride);
            return;
        }
    }
    else
    {
        uint8 seat = current_seat | 1;
        if (seat < vehicle->next_free_seat)
        {
            UpdateRideFreeVehicleEnterRide(ride);
            return;
        }
    }

    rct_vehicle * currentTrain = GET_VEHICLE(ride->vehicles[current_train]);
    if (ride->status == RIDE_STATUS_OPEN && ++rejoin_queue_timeout != 0 &&
        !(currentTrain->update_flags & VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART))
    {
        return;
    }

    if (ride->mode != RIDE_MODE_FORWARD_ROTATION && ride->mode != RIDE_MODE_BACKWARD_ROTATION)
    {
        if (vehicle->next_free_seat - 1 != current_seat)
            return;
    }

    vehicle->next_free_seat--;
    vehicle->peep[current_seat] = SPRITE_INDEX_NULL;

    peep_update_ride_no_free_vehicle_rejoin_queue(this, ride);
}

void rct_peep::UpdateRideApproachVehicle()
{
    sint16 actionX, actionY, xy_distance;
    if (!UpdateAction(&actionX, &actionY, &xy_distance))
    {
        sub_state = PEEP_RIDE_ENTER_VEHICLE;
        return;
    }

    Invalidate();
    sprite_move(actionX, actionY, z, (rct_sprite *)this);
    Invalidate();
}

void rct_peep::UpdateRideEnterVehicle()
{
    Ride * ride = get_ride(current_ride);

    rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[current_train]);
    for (sint32 i = current_car; i != 0; --i)
    {
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    if (ride->mode != RIDE_MODE_FORWARD_ROTATION && ride->mode != RIDE_MODE_BACKWARD_ROTATION)
    {
        if (current_seat != vehicle->num_peeps)
            return;
    }

    if (vehicle_is_used_in_pairs(vehicle))
    {
        rct_peep * seated_peep = GET_PEEP(vehicle->peep[current_seat ^ 1]);
        if (seated_peep->sub_state != PEEP_RIDE_ENTER_VEHICLE)
            return;

        vehicle->num_peeps++;
        ride->cur_num_customers++;

        vehicle->mass += seated_peep->mass;
        seated_peep->Invalidate();
        sprite_move(LOCATION_NULL, 0, 0, (rct_sprite *)seated_peep);

        seated_peep->SetState(PEEP_STATE_ON_RIDE);
        seated_peep->time_on_ride = 0;
        seated_peep->sub_state    = PEEP_RIDE_ON_RIDE;
        seated_peep->OnEnterRide(current_ride);
    }

    vehicle->num_peeps++;
    ride->cur_num_customers++;

    vehicle->mass += mass;
    invalidate_sprite_2((rct_sprite *)vehicle);

    Invalidate();
    sprite_move(LOCATION_NULL, 0, 0, (rct_sprite *)this);

    SetState(PEEP_STATE_ON_RIDE);

    time_on_ride = 0;
    sub_state    = PEEP_RIDE_ON_RIDE;
    OnEnterRide(current_ride);
}

/**
 *
 *  rct2: 0x00693028
 */
void rct_peep::UpdateRideLeaveVehicle()
{
    Ride * ride = get_ride(current_ride);

    rct_vehicle * vehicle      = GET_VEHICLE(ride->vehicles[current_train]);
    uint8         ride_station = vehicle->current_station;

    for (sint32 i = current_car; i != 0; --i)
    {
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    // Check if ride is NOT Ferris Wheel.
    if (ride->mode != RIDE_MODE_FORWARD_ROTATION && ride->mode != RIDE_MODE_BACKWARD_ROTATION)
    {
        if (vehicle->num_peeps - 1 != current_seat)
            return;
    }

    action_sprite_image_offset++;
    if (action_sprite_image_offset & 3)
        return;

    action_sprite_image_offset = 0;

    vehicle->num_peeps--;
    vehicle->mass -= mass;
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
    current_ride_station  = ride_station;
    rct_ride_entry * rideEntry = get_ride_entry(vehicle->ride_subtype);
    if (rideEntry == nullptr)
    {
        return;
    }

    rct_ride_entry_vehicle * vehicle_entry = &rideEntry->vehicles[vehicle->vehicle_type];

    if (!(vehicle_entry->flags & VEHICLE_ENTRY_FLAG_LOADING_WAYPOINTS))
    {
        assert(current_ride_station < MAX_STATIONS);
        TileCoordsXYZD exitLocation = ride_get_exit_location(current_ride, current_ride_station);
        CoordsXYZD platformLocation;
        platformLocation.z = ride->station_heights[current_ride_station];

        platformLocation.direction = exitLocation.direction ^ (1 << 1);

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
                if (stationIndex == current_ride_station)
                    break;
            }

            uint8 shiftMultiplier = 12;
            uint8 specialDirection        = platformLocation.direction;

            rideEntry = get_ride_entry(ride->subtype);

            if (rideEntry != nullptr)
            {
                vehicle_entry = &rideEntry->vehicles[rideEntry->default_vehicle];

                if (vehicle_entry->flags & VEHICLE_ENTRY_FLAG_GO_KART)
                {
                    shiftMultiplier = 9;
                }

                if (vehicle_entry->flags & (VEHICLE_ENTRY_FLAG_CHAIRLIFT | VEHICLE_ENTRY_FLAG_GO_KART))
                {
                    specialDirection = ((vehicle->sprite_direction + 3) / 8) + 1;
                    specialDirection &= 3;

                    if (vehicle->var_CD == 6)
                        specialDirection ^= (1 << 1);
                }
            }

            sint16 xShift = word_981D6C[specialDirection].x;
            sint16 yShift = word_981D6C[specialDirection].y;

            platformLocation.x = vehicle->x + xShift * shiftMultiplier;
            platformLocation.y = vehicle->y + yShift * shiftMultiplier;
            platformLocation.z *= 8;

            peep_go_to_ride_exit(this, ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
            return;
        }

        platformLocation.x = vehicle->x + word_981D6C[platformLocation.direction].x * 12;
        platformLocation.y = vehicle->y + word_981D6C[platformLocation.direction].y * 12;

        sint8 loadPosition = vehicle_entry->peep_loading_positions[current_seat];

        switch (vehicle->sprite_direction / 8)
        {
        case 0:
            platformLocation.x -= loadPosition;
            break;
        case 1:
            platformLocation.y += loadPosition;
            break;
        case 2:
            platformLocation.x += loadPosition;
            break;
        case 3:
            platformLocation.y -= loadPosition;
            break;
        }

        platformLocation.z = ride->station_heights[current_ride_station] * 8;

        peep_go_to_ride_exit(this, ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
        return;
    }

    TileCoordsXYZD exitLocation = ride_get_exit_location(current_ride, current_ride_station);
    Guard::Assert(!exitLocation.isNull());
    CoordsXYZ waypointLoc;

    waypointLoc.z = (sint16)exitLocation.z * 8 + RideData5[ride->type].z;
    waypointLoc.x = ride->station_starts[current_ride_station].x * 32 + 16;
    waypointLoc.y = ride->station_starts[current_ride_station].y * 32 + 16;

    rct_tile_element * trackElement = ride_get_station_start_track_element(ride, current_ride_station);

    uint8 station_direction = (trackElement == nullptr ? 0 : tile_element_get_direction(trackElement));

    vehicle = GET_VEHICLE(ride->vehicles[current_train]);

    rideEntry                            = get_ride_entry(vehicle->ride_subtype);
    rct_ride_entry_vehicle * vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

    var_37 = ((exitLocation.direction | peep_get_waypointed_seat_location(this, ride, vehicleEntry, station_direction) * 4) * 4) | 1;

    if (ride->type == RIDE_TYPE_ENTERPRISE)
    {
        waypointLoc.x = vehicle->x;
        waypointLoc.y = vehicle->y;
    }

    Guard::Assert(vehicleEntry->peep_loading_waypoints.size() >= (size_t)(var_37 / 4));
    CoordsXYZ exitWaypointLoc = waypointLoc;

    exitWaypointLoc.x += vehicleEntry->peep_loading_waypoints[var_37 / 4][2].x;
    exitWaypointLoc.y += vehicleEntry->peep_loading_waypoints[var_37 / 4][2].y;

    if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        exitWaypointLoc.z += 15;

    sprite_move(exitWaypointLoc.x, exitWaypointLoc.y, exitWaypointLoc.z, (rct_sprite *)this);
    Invalidate();

    waypointLoc.x += vehicleEntry->peep_loading_waypoints[var_37 / 4][1].x;
    waypointLoc.y += vehicleEntry->peep_loading_waypoints[var_37 / 4][1].x;

    destination_x         = waypointLoc.x;
    destination_y         = waypointLoc.y;
    destination_tolerance = 2;
    sub_state             = PEEP_RIDE_APPROACH_EXIT_WAYPOINTS;
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
    sint16 actionX, actionY, xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        Invalidate();
        return;
    }

    peep_update_ride_prepare_for_exit(this);
}

/**
 *
 *  rct2: 0x0069382E
 */
void rct_peep::UpdateRideInExit()
{
    sint16 actionX, actionY, xy_distance;
    Ride * ride = get_ride(current_ride);

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();

        if (xy_distance >= 16)
        {
            sint16 actionZ = ride->station_heights[current_ride_station] * 8;

            actionZ += RideData5[ride->type].z;
            sprite_move(actionX, actionY, actionZ, (rct_sprite *)this);
            Invalidate();
            return;
        }

        SwitchToSpecialSprite(0);
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        Invalidate();
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
    {
        uint8 secondaryItem = RidePhotoItems[ride->type];
        if (DecideAndBuyItem(current_ride, secondaryItem, ride->price_secondary))
        {
            ride->no_secondary_items_sold++;
        }
    }
    sub_state = PEEP_RIDE_LEAVE_EXIT;
}

/**
 *
 *  rct2: 0x006926AD
 */
void rct_peep::UpdateRideApproachVehicleWaypoints()
{
    sint16 actionX, actionY, xy_distance;
    Ride * ride = get_ride(current_ride);
    uint8 waypoint = var_37 & 3;

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        sint16 actionZ;
        // Motion simulators have steps this moves the peeps up the steps
        if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        {
            actionZ = ride->station_heights[current_ride_station] * 8 + 2;

            if (waypoint == 2)
            {
                xy_distance -= 12;
                if (xy_distance < 0)
                    xy_distance = 0;

                if (xy_distance <= 15)
                {
                    actionZ += 15 - xy_distance;
                }
            }
        }
        else
        {
            actionZ = z;
        }
        Invalidate();
        sprite_move(actionX, actionY, actionZ, (rct_sprite *)this);
        Invalidate();
        return;
    }

    if (waypoint == 2)
    {
        sub_state = PEEP_RIDE_ENTER_VEHICLE;
        return;
    }

    waypoint++;
    // This is incrementing the actual peep waypoint
    var_37++;

    rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[current_train]);

    actionX = ride->station_starts[current_ride_station].x * 32 + 16;
    actionY = ride->station_starts[current_ride_station].y * 32 + 16;

    if (ride->type == RIDE_TYPE_ENTERPRISE)
    {
        actionX = vehicle->x;
        actionY = vehicle->y;
    }

    rct_ride_entry * ride_entry = get_ride_entry(vehicle->ride_subtype);
    if (ride_entry == nullptr)
    {
        return;
    }

    rct_ride_entry_vehicle * vehicle_type = &ride_entry->vehicles[vehicle->vehicle_type];
    Guard::Assert(waypoint < 3);
    actionX += vehicle_type->peep_loading_waypoints[var_37 / 4][waypoint].x;
    actionY += vehicle_type->peep_loading_waypoints[var_37 / 4][waypoint].y;

    destination_x = actionX;
    destination_y = actionY;
}

/**
 *
 *  rct2: 0x0069357D
 */
void rct_peep::UpdateRideApproachExitWaypoints()
{
    sint16 actionX, actionY, xy_distance;
    Ride * ride = get_ride(current_ride);

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        sint16 actionZ;
        if (ride->type == RIDE_TYPE_MOTION_SIMULATOR)
        {
            actionZ = ride->station_heights[current_ride_station] * 8 + 2;

            if ((var_37 & 3) == 1)
            {

                if (xy_distance > 15)
                    xy_distance = 15;

                actionZ += xy_distance;
            }
        }
        else
        {
            actionZ = z;
        }
        Invalidate();
        sprite_move(actionX, actionY, actionZ, (rct_sprite *)this);
        Invalidate();
        return;
    }

    if ((var_37 & 3) != 0)
    {
        if ((var_37 & 3) == 3)
        {
            peep_update_ride_prepare_for_exit(this);
            return;
        }

        var_37--;
        rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[current_train]);

        actionX = ride->station_starts[current_ride_station].x * 32 + 16;
        actionY = ride->station_starts[current_ride_station].y * 32 + 16;

        if (ride->type == RIDE_TYPE_ENTERPRISE)
        {
            actionX = vehicle->x;
            actionY = vehicle->y;
        }

        rct_ride_entry *         rideEntry   = get_ride_entry(vehicle->ride_subtype);
        rct_ride_entry_vehicle * vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

        Guard::Assert((var_37 & 3) < 3);
        actionX += vehicleEntry->peep_loading_waypoints[var_37 / 4][var_37 & 3].x;
        actionY += vehicleEntry->peep_loading_waypoints[var_37 / 4][var_37 & 3].y;

        destination_x = actionX;
        destination_y = actionY;
        return;
    }

    var_37 |= 3;

    auto exit = ride_get_exit_location(current_ride, current_ride_station);
    actionX = exit.x;
    actionY = exit.y;
    uint8 exit_direction = exit.direction ^ 2;

    actionX *= 32;
    actionY *= 32;
    actionX += 16;
    actionY += 16;

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

    actionX -= x_shift;
    actionY -= y_shift;

    destination_x = actionX;
    destination_y = actionY;
}

/**
 *
 *  rct2: 0x006927B3
 */
void rct_peep::UpdateRideApproachSpiralSlide()
{
    Ride * ride = get_ride(current_ride);

    sint16 actionX, actionY, xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        invalidate_sprite_2((rct_sprite *)this);
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        invalidate_sprite_2((rct_sprite *)this);
        return;
    }

    uint8 waypoint = var_37 & 3;

    if (waypoint == 3)
    {
        sub_state     = 15;
        destination_x = 0;
        destination_y = 0;
        var_37        = (var_37 / 4) & 0xC;
        sprite_move(LOCATION_NULL, actionY, z, (rct_sprite *)this);
        return;
    }
    else if (waypoint == 2)
    {
        uint8 last_ride = 0;
        if (ride->status != RIDE_STATUS_OPEN)
            last_ride = 1;
        else if (current_car++ != 0)
        {
            if (ride->mode == RIDE_MODE_SINGLE_RIDE_PER_ADMISSION)
                last_ride = 1;
            if ((uint8)(current_car - 1) > (scenario_rand() & 0xF))
                last_ride = 1;
        }

        if (last_ride)
        {
            auto exit = ride_get_exit_location(current_ride, current_ride_station);
            uint8 exit_direction = exit.direction;

            waypoint = 1;
            var_37   = (exit_direction * 4) | (var_37 & 0x30) | waypoint;
            actionX  = ride->station_starts[current_ride_station].x * 32;
            actionY  = ride->station_starts[current_ride_station].y * 32;

            assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
            const CoordsXY slidePlatformDestination = SpiralSlideWalkingPath[var_37];

            actionX += slidePlatformDestination.x;
            actionY += slidePlatformDestination.y;

            destination_x = actionX;
            destination_y = actionY;
            sub_state     = PEEP_RIDE_LEAVE_SPIRAL_SLIDE;
            return;
        }
    }
    waypoint++;
    // Actually increment the real peep waypoint
    var_37++;

    actionX = ride->station_starts[current_ride_station].x;
    actionY = ride->station_starts[current_ride_station].y;

    actionX *= 32;
    actionY *= 32;

    assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
    const CoordsXY slidePlatformDestination = SpiralSlideWalkingPath[var_37];

    actionX += slidePlatformDestination.x;
    actionY += slidePlatformDestination.y;

    destination_x = actionX;
    destination_y = actionY;
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
    Ride * ride = get_ride(current_ride);

    if (ride->type != RIDE_TYPE_SPIRAL_SLIDE)
        return;

    if ((var_37 & 3) == 0)
    {
        switch (destination_x)
        {
        case 0:
            destination_y++;
            if (destination_y >= 30)
                destination_x++;
            return;
        case 1:
            if (ride->slide_in_use != 0)
                return;

            ride->slide_in_use++;
            ride->slide_peep                = sprite_index;
            ride->slide_peep_t_shirt_colour = tshirt_colour;
            ride->spiral_slide_progress     = 0;
            destination_x++;
            return;
        case 2:
            return;
        case 3:
        {
            sint16 newX  = ride->station_starts[current_ride_station].x * 32;
            sint16 newY  = ride->station_starts[current_ride_station].y * 32;
            uint8  dir   = (var_37 / 4) & 3;
            sint16 destX = newX + _981F1C[dir].x;
            sint16 destY = newY + _981F1C[dir].y;

            destination_x = destX;
            destination_y = destY;

            newX += _981F0C[dir].x;
            newY += _981F0C[dir].y;

            sprite_move(newX, newY, z, (rct_sprite *)this);

            sprite_direction = (var_37 & 0xC) * 2;

            invalidate_sprite_2((rct_sprite *)this);

            var_37++;
            return;
        }
        default:
            return;
        }
    }

    sint16 actionX, actionY, xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        invalidate_sprite_2((rct_sprite *)this);
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        invalidate_sprite_2((rct_sprite *)this);
        return;
    }
    uint8 waypoint = 2;
    var_37 = (var_37 * 4 & 0x30) + waypoint;

    actionX = ride->station_starts[current_ride_station].x;
    actionY = ride->station_starts[current_ride_station].y;

    actionX *= 32;
    actionY *= 32;

    assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
    const CoordsXY slidePlatformDestination = SpiralSlideWalkingPath[var_37];

    actionX += slidePlatformDestination.x;
    actionY += slidePlatformDestination.y;

    destination_x = actionX;
    destination_y = actionY;
    sub_state     = PEEP_RIDE_APPROACH_SPIRAL_SLIDE;
}

/**
 *
 *  rct2: 0x00692C6B
 */
void rct_peep::UpdateRideLeaveSpiralSlide()
{
    // Iterates through the spiral slide waypoints until it reaches 
    // waypoint 0. Then it readies to leave the ride by the entrance.
    sint16 actionX, acitonY, xy_distance;
    if (UpdateAction(&actionX, &acitonY, &xy_distance))
    {
        invalidate_sprite_2((rct_sprite *)this);
        sprite_move(actionX, acitonY, z, (rct_sprite *)this);
        invalidate_sprite_2((rct_sprite *)this);
        return;
    }

    Ride * ride = get_ride(current_ride);

    uint8 waypoint = var_37 & 3;

    if (waypoint != 0)
    {
        if (waypoint == 3)
        {
            peep_update_ride_prepare_for_exit(this);
            return;
        }

        waypoint--;
        // Actually decrement the peep waypoint
        var_37--;
        actionX = ride->station_starts[current_ride_station].x * 32;
        acitonY = ride->station_starts[current_ride_station].y * 32;

        assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
        const CoordsXY slidePlatformDestination = SpiralSlideWalkingPath[var_37];

        actionX += slidePlatformDestination.x;
        acitonY += slidePlatformDestination.y;

        destination_x = actionX;
        destination_y = acitonY;
        return;
    }
    waypoint = 3;
    // Actually force the final waypoint
    var_37 |= 3;

    auto exit = ride_get_exit_location(current_ride, current_ride_station);
    actionX         = exit.x * 32 + 16;
    acitonY         = exit.y * 32 + 16;

    uint8 exit_direction = exit.direction ^ 2;

    sint16 x_shift = word_981D6C[exit_direction].x;
    sint16 y_shift = word_981D6C[exit_direction].y;

    sint16 shift_multiplier = 20;

    x_shift *= shift_multiplier;
    y_shift *= shift_multiplier;

    actionX -= x_shift;
    acitonY -= y_shift;

    destination_x = actionX;
    destination_y = acitonY;
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
    sint16 actionX, actionY, xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        invalidate_sprite_2((rct_sprite *)this);
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        invalidate_sprite_2((rct_sprite *)this);
        return;
    }

    Ride * ride = get_ride(current_ride);
    if (var_37 == 16)
    {
        peep_update_ride_prepare_for_exit(this);
        return;
    }

    if (action >= PEEP_ACTION_NONE_1)
    {
        if (energy > 64 && (scenario_rand() & 0xFFFF) <= 2427)
        {

            action                     = PEEP_ACTION_JUMP;
            action_frame               = 0;
            action_sprite_image_offset = 0;
            UpdateCurrentActionSpriteType();
            invalidate_sprite_2((rct_sprite *)this);
        }
    }

    actionX        = destination_x & 0xFFE0;
    actionY        = destination_y & 0xFFE0;
    sint16 stationHeight = ride->station_heights[0];

    // Find the station track element
    rct_tile_element * tileElement = map_get_first_element_at(actionX / 32, actionY / 32);
    do
    {
        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_TRACK && stationHeight == tileElement->base_height)
            break;

    } while (!tile_element_is_last_for_tile(tileElement++));

    uint16 maze_entry  = track_element_get_maze_entry(tileElement);
    uint16 open_hedges = 0;
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

    uint8 mazeLastEdgeTmp = maze_last_edge ^ (1 << 1);
    open_hedges &= ~(1 << mazeLastEdgeTmp);
    if (open_hedges == 0)
        open_hedges |= (1 << mazeLastEdgeTmp);

    uint8 chosen_edge = scenario_rand() & 0x3;
    while (!(open_hedges & (1 << chosen_edge)))
    {
        chosen_edge = (chosen_edge + 1) & 3;
    }

    actionX = TileDirectionDelta[chosen_edge].x / 2;
    actionY = TileDirectionDelta[chosen_edge].y / 2;

    actionX += destination_x;
    actionY += destination_y;

    uint8 edgeType = 0;

    tileElement = map_get_first_element_at(actionX / 32, actionY / 32);
    do
    {
        if (stationHeight != tileElement->base_height)
            continue;

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_TRACK)
        {
            edgeType = 1;
            break;
        }

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_ENTRANCE &&
            tileElement->properties.entrance.type == ENTRANCE_TYPE_RIDE_EXIT)
        {
            edgeType = 2;
            break;
        }
    } while (!tile_element_is_last_for_tile(tileElement++));

    switch (edgeType)
    {
    case 0:
        maze_last_edge++;
        maze_last_edge &= 3;
        return;
    case 1:
        destination_x = actionX;
        destination_y = actionY;

        var_37         = _981FE4[var_37 / 4][chosen_edge];
        maze_last_edge = chosen_edge;
        break;
    case 2:
        actionX = destination_x;
        actionY = destination_y;
        if (chosen_edge & 1)
        {
            actionX &= 0xFFE0;
            actionX += 16;
        }
        else
        {
            actionY &= 0xFFE0;
            actionY += 16;
        }
        destination_x  = actionX;
        destination_y  = actionY;
        var_37         = 16;
        maze_last_edge = chosen_edge;
        break;
    }

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        invalidate_sprite_2((rct_sprite *)this);
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        invalidate_sprite_2((rct_sprite *)this);
        return;
    }
}

/**
 *
 *  rct2: 0x006938D2
 */
void rct_peep::UpdateRideLeaveExit()
{
    sint16 actionX, actionY, xy_distance;
    Ride * ride = get_ride(current_ride);

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        sprite_move(actionX, actionY, ride->station_heights[current_ride_station] * 8, (rct_sprite *)this);
        Invalidate();
        return;
    }

    OnExitRide(current_ride);

    if (peep_flags & PEEP_FLAGS_TRACKING)
    {
        set_format_arg(0, rct_string_id, name_string_idx);
        set_format_arg(2, uint32, id);
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);

        if (gConfigNotifications.guest_left_ride)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, STR_PEEP_TRACKING_LEFT_RIDE_X, sprite_index);
        }
    }

    interaction_ride_index = 0xFF;
    SetState(PEEP_STATE_FALLING);

    actionX = x & 0xFFE0;
    actionY = y & 0xFFE0;

    // Find the station track element
    rct_tile_element * tileElement = map_get_first_element_at(actionX / 32, actionY / 32);
    do
    {
        if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_PATH)
            continue;

        sint16 height = map_height_from_slope(x, y, tileElement->properties.path.type);
        height += tileElement->base_height * 8;

        sint16 z_diff = z - height;
        if (z_diff > 0 || z_diff < -16)
            continue;

        sprite_move(x, y, height, (rct_sprite *)this);
        Invalidate();
        return;
    } while (!tile_element_is_last_for_tile(tileElement++));
}

/**
 *
 *  rct2: 0x0069299C
 */
void rct_peep::UpdateRideShopApproach()
{
    sint16 actionX, actionY, xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        invalidate_sprite_2((rct_sprite *)this);
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        invalidate_sprite_2((rct_sprite *)this);
        return;
    }

    sub_state = PEEP_SHOP_INTERACT;
}

/**
 *
 *  rct2: 0x006929BB
 */
void rct_peep::UpdateRideShopInteract()
{
    const sint16 tileCenterX = next_x + 16;
    const sint16 tileCenterY = next_y + 16;
    Ride * ride = get_ride(current_ride);

    if (ride->type == RIDE_TYPE_FIRST_AID)
    {
        if (nausea <= 35)
        {
            sub_state = 21;

            destination_x         = tileCenterX;
            destination_y         = tileCenterY;
            destination_tolerance = 3;
            happiness_target      = Math::Min(happiness_target + 30, PEEP_MAX_HAPPINESS);
            happiness             = happiness_target;
        }
        else
        {
            nausea--;
            nausea_target = nausea;
        }
        return;
    }

    if (toilet != 0)
    {
        toilet--;
        return;
    }

    // Do not play toilet flush sound on title screen as it's considered loud and annoying
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        audio_play_sound_at_location(SOUND_TOILET_FLUSH, x, y, z);
    }

    sub_state = PEEP_SHOP_LEAVE;

    destination_x         = tileCenterX;
    destination_y         = tileCenterY;
    destination_tolerance = 3;

    happiness_target = Math::Min(happiness_target + 30, PEEP_MAX_HAPPINESS);
    happiness        = happiness_target;

    peep_stop_purchase_thought(this, ride->type);
}

/**
 *
 *  rct2: 0x00692935
 */
void rct_peep::UpdateRideShopLeave()
{
    sint16 actionX, actionY, xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        invalidate_sprite_2((rct_sprite *)this);
        sprite_move(actionX, actionY, this->z, (rct_sprite *)this);
        invalidate_sprite_2((rct_sprite *)this);

        actionX = this->x & 0xFFE0;
        actionY = this->y & 0xFFE0;
        if (actionX != this->next_x)
            return;
        if (actionY != this->next_y)
            return;
    }

    peep_decrement_num_riders(this);
    this->state = PEEP_STATE_WALKING;
    peep_window_state_update(this);

    Ride * ride = get_ride(this->current_ride);
    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

    ride_update_satisfaction(ride, this->happiness / 64);
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
