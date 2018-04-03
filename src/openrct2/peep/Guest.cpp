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
#include "../network/network.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../Context.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../world/Climate.h"
#include "../world/Footpath.h"
#include "../world/LargeScenery.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "../windows/Intent.h"
#include "Peep.h"

// Locations of the spiral slide platform that a peep walks from the entrance of the ride to the
// entrance of the slide. Up to 4 waypoints for each 4 sides that an ride entrance can be located
// and 4 different rotations of the ride. 4 * 4 * 4 = 64 locations.
// clang-format off
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

/** rct2: 0x00981F4C, 0x00981F4E */
static constexpr const LocationXY16 _981F4C[] = {
    {  7,  5 },
    {  5, 25 },
    { 25,  5 },
    {  5,  7 },
    {  7,  9 },
    {  9, 25 },
    { 25,  9 },
    {  9,  7 },
    {  7, 23 },
    { 23, 25 },
    { 25, 23 },
    { 23,  7 },
    {  7, 27 },
    { 27, 25 },
    { 25, 27 },
    { 27,  7 },
    {  7,  0 },
    {  0, 25 },
    { 25,  0 },
    {  0,  7 },
    {  7,  0 },
    {  0, 25 },
    { 25,  0 },
    {  0,  7 },
    {  7,  0 },
    {  0, 25 },
    { 25,  0 },
    {  0,  7 },
    {  7,  0 },
    {  0, 25 },
    { 25,  0 },
    {  0,  7 },
};

// TODO: Remove duplicate (there's one in peep.cpp too)
static constexpr const ride_rating NauseaMaximumThresholds[] = {
    300, 600, 800, 1000
};

/** rct2: 0x0097EFCC */
static constexpr const uint8 item_standard_litter[32] = {
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_BALLOON
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_TOY
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_MAP
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_PHOTO
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_UMBRELLA
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_DRINK
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_BURGER
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_CHIPS
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_ICE_CREAM
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_CANDYFLOSS
    LITTER_TYPE_EMPTY_CAN,        // PEEP_ITEM_EMPTY_CAN
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_RUBBISH
    LITTER_TYPE_EMPTY_BURGER_BOX, // PEEP_ITEM_EMPTY_BURGER_BOX
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_PIZZA
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_VOUCHER
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_POPCORN
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_HOT_DOG
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_TENTACLE
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_HAT
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_TOFFEE_APPLE
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_TSHIRT
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_DOUGHNUT
    LITTER_TYPE_RUBBISH,          // PEEP_ITEM_COFFEE
    LITTER_TYPE_EMPTY_CUP,        // PEEP_ITEM_EMPTY_CUP
    LITTER_TYPE_EMPTY_BOX,        // PEEP_ITEM_CHICKEN
    LITTER_TYPE_EMPTY_BOTTLE,     // PEEP_ITEM_LEMONADE
    LITTER_TYPE_EMPTY_BOX,        // PEEP_ITEM_EMPTY_BOX
    LITTER_TYPE_EMPTY_BOTTLE,     // PEEP_ITEM_EMPTY_BOTTLE
};

/** rct2: 0x0097EFE8 */
static constexpr const uint8 item_extra_litter[32] = {
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_PHOTO2
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_PHOTO3
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_PHOTO4
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_PRETZEL
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_CHOCOLATE
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_ICED_TEA
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_FUNNEL_CAKE
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_SUNGLASSES
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_BEEF_NOODLES
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_FRIED_RICE_NOODLES
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_WONTON_SOUP
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_MEATBALL_SOUP
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_FRUIT_JUICE
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_SOYBEAN_MILK
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_SU_JONGKWA
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_SUB_SANDWICH
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_COOKIE
    LITTER_TYPE_EMPTY_BOWL_RED,     // PEEP_ITEM_EMPTY_BOWL_RED
    LITTER_TYPE_EMPTY_DRINK_CARTON, // PEEP_ITEM_EMPTY_DRINK_CARTON
    LITTER_TYPE_EMPTY_JUICE_CUP,    // PEEP_ITEM_EMPTY_JUICE_CUP
    LITTER_TYPE_RUBBISH,            // PEEP_ITEM_ROAST_SAUSAGE
    LITTER_TYPE_EMPTY_BOWL_BLUE,    // PEEP_ITEM_EMPTY_BOWL_BLUE
};
// clang-format on

static bool peep_has_voucher_for_free_ride(rct_peep * peep, sint32 rideIndex);
static void peep_ride_is_too_intense(rct_peep * peep, sint32 rideIndex, bool peepAtRide);
static void peep_reset_ride_heading(rct_peep * peep);
static void peep_tried_to_enter_full_queue(rct_peep * peep, sint32 rideIndex);

/**
 *
 *  rct2: 0x00691677
 */
void rct_peep::TryGetUpFromSitting()
{
    // Eats all food first
    if (HasFood())
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

        if (HasFood())
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
*  rct2: 0x00695DD2
*/
void rct_peep::PickRideToGoOn()
{
    Ride * ride;

    if (this->state != PEEP_STATE_WALKING)
        return;
    if (this->guest_heading_to_ride_id != 255)
        return;
    if (this->peep_flags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (HasFood())
        return;
    if (this->x == LOCATION_NULL)
        return;

    uint32 rideConsideration[8]{};

    // FIX  Originally checked for a toy, likely a mistake and should be a map,
    //      but then again this seems to only allow the peep to go on
    //      rides they haven't been on before.
    if (this->item_standard_flags & PEEP_ITEM_MAP)
    {
        // Consider rides that peep hasn't been on yet
        sint32 i;
        FOR_ALL_RIDES(i, ride)
        {
            if (!HasRidden(i))
            {
                rideConsideration[i >> 5] |= (1u << (i & 0x1F));
            }
        }
    }
    else
    {
        // Take nearby rides into consideration
        sint32 cx = floor2(this->x, 32);
        sint32 cy = floor2(this->y, 32);
        for (sint32 tileX = cx - 320; tileX <= cx + 320; tileX += 32)
        {
            for (sint32 tileY = cy - 320; tileY <= cy + 320; tileY += 32)
            {
                if (tileX >= 0 && tileY >= 0 && tileX < (256 * 32) && tileY < (256 * 32))
                {
                    rct_tile_element * tileElement = map_get_first_element_at(tileX >> 5, tileY >> 5);
                    do
                    {
                        if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_TRACK)
                            continue;

                        sint32 rideIndex = track_element_get_ride_index(tileElement);
                        rideConsideration[rideIndex >> 5] |= (1u << (rideIndex & 0x1F));
                    } while (!tile_element_is_last_for_tile(tileElement++));
                }
            }
        }

        // Always take the tall rides into consideration (realistic as you can usually see them from anywhere in the park)
        sint32 i;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride->status != RIDE_STATUS_OPEN)
                continue;
            if (!ride_has_ratings(ride))
                continue;
            if (ride->highest_drop_height <= 66 && ride->excitement < RIDE_RATING(8, 00))
                continue;

            rideConsideration[i >> 5] |= (1u << (i & 0x1F));
        }
    }

    // Filter the considered rides
    uint8 potentialRides[256];
    uint8 * nextPotentialRide = &potentialRides[0];
    sint32  numPotentialRides = 0;
    for (sint32 i = 0; i < MAX_RIDES; i++)
    {
        if (!(rideConsideration[i >> 5] & (1u << (i & 0x1F))))
            continue;

        ride = get_ride(i);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL))
        {
            if (ShouldGoOnRide(i, 0, false, true))
            {
                *nextPotentialRide++ = i;
                numPotentialRides++;
            }
        }
    }

    // Pick the most exciting ride
    sint32      mostExcitingRideIndex = -1;
    ride_rating mostExcitingRideRating = 0;
    for (sint32 i = 0; i < numPotentialRides; i++)
    {
        ride = get_ride(potentialRides[i]);
        if (!ride_has_ratings(ride))
            continue;
        if (ride->excitement > mostExcitingRideRating)
        {
            mostExcitingRideIndex = potentialRides[i];
            mostExcitingRideRating = ride->excitement;
        }
    }
    if (mostExcitingRideIndex == -1)
        return;

    // Head to that ride
    this->guest_heading_to_ride_id = mostExcitingRideIndex;
    this->peep_is_lost_countdown = 200;
    peep_reset_pathfind_goal(this);

    // Invalidate windows
    rct_window * w = window_find_by_number(WC_PEEP, this->sprite_index);
    if (w != nullptr)
    {
        window_event_invalidate_call(w);
        widget_invalidate(w, WC_PEEP__WIDX_ACTION_LBL);
    }

    // Make peep look at their map if they have one
    if (this->item_standard_flags & PEEP_ITEM_MAP)
    {
        ReadMap();
    }
}

/**
* This function is called whenever a peep is deciding whether or not they want
* to go on a ride or visit a shop. They may be physically present at the
* ride/shop, or they may just be thinking about it.
*  rct2: 0x006960AB
*/
bool rct_peep::ShouldGoOnRide(sint32 rideIndex, sint32 entranceNum, bool atQueue, bool thinking)
{
    Ride * ride = get_ride(rideIndex);

    // Indicates whether a peep is physically at the ride, or is just thinking about going on the ride.
    bool peepAtRide = !thinking;

    if (ride->status == RIDE_STATUS_OPEN && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
    {

        // Peeps that are leaving the park will refuse to go on any rides, with the exception of free transport rides.
        if (!(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_TRANSPORT_RIDE) || ride->value == 0xFFFF ||
            ride_get_price(ride) != 0)
        {
            if (this->peep_flags & PEEP_FLAGS_LEAVING_PARK)
            {
                ChoseNotToGoOnRide(rideIndex, peepAtRide, false);
                return false;
            }
        }

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
        {
            return ShouldGoToShop(rideIndex, peepAtRide);
        }

        // This used to check !(flags & 2), but the function is only ever called with flags = 0, 1 or 6.
        // This means we can use the existing !(flags & 4) check.
        if (peepAtRide)
        {
            // Peeps won't join a queue that has 1000 peeps already in it.
            if (ride->queue_length[entranceNum] >= 1000)
            {
                peep_tried_to_enter_full_queue(this, rideIndex);
                return false;
            }

            // Rides without queues can only have one peep waiting at a time.
            if (!atQueue)
            {
                if (ride->last_peep_in_queue[entranceNum] != 0xFFFF)
                {
                    peep_tried_to_enter_full_queue(this, rideIndex);
                    return false;
                }
            }
            else
            {
                // Check if there's room in the queue for the peep to enter.
                if (ride->last_peep_in_queue[entranceNum] != 0xFFFF)
                {
                    rct_peep * lastPeepInQueue = GET_PEEP(ride->last_peep_in_queue[entranceNum]);
                    if (abs(lastPeepInQueue->z - this->z) <= 6)
                    {
                        sint32 dx = abs(lastPeepInQueue->x - this->x);
                        sint32 dy = abs(lastPeepInQueue->y - this->y);
                        sint32 maxD = Math::Max(dx, dy);

                        // Unlike normal paths, peeps cannot overlap when queueing for a ride.
                        // This check enforces a minimum distance between peeps entering the queue.
                        if (maxD < 8)
                        {
                            peep_tried_to_enter_full_queue(this, rideIndex);
                            return false;
                        }

                        // This checks if there's a peep standing still at the very end of the queue.
                        if (maxD <= 13 && lastPeepInQueue->time_in_queue > 10)
                        {
                            peep_tried_to_enter_full_queue(this, rideIndex);
                            return false;
                        }
                    }
                }
            }
        }

        // Assuming the queue conditions are met, peeps will always go on free transport rides.
        // Ride ratings, recent crashes and weather will all be ignored.
        money16 ridePrice = ride_get_price(ride);
        if (!(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_TRANSPORT_RIDE) || ride->value == 0xFFFF || ridePrice != 0)
        {
            if (this->previous_ride == rideIndex)
            {
                ChoseNotToGoOnRide(rideIndex, peepAtRide, false);
                return false;
            }

            // Basic price checks
            if (ridePrice != 0 && !peep_has_voucher_for_free_ride(this, rideIndex))
            {

                if (ridePrice > this->cash_in_pocket)
                {
                    if (peepAtRide)
                    {
                        if (this->cash_in_pocket <= 0)
                        {
                            peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_SPENT_MONEY, PEEP_THOUGHT_ITEM_NONE);
                        }
                        else
                        {
                            peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_CANT_AFFORD_0, rideIndex);
                        }
                    }
                    ChoseNotToGoOnRide(rideIndex, peepAtRide, true);
                    return false;
                }
            }

            // If happy enough, peeps will ignore the fact that a ride has recently crashed.
            if (ride->last_crash_type != RIDE_CRASH_TYPE_NONE && this->happiness < 225)
            {
                if (peepAtRide)
                {
                    peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_NOT_SAFE, rideIndex);
                    if (this->happiness_target >= 64)
                    {
                        this->happiness_target -= 8;
                    }
                    ride_update_popularity(ride, 0);
                }
                ChoseNotToGoOnRide(rideIndex, peepAtRide, true);
                return false;
            }

            if (ride_has_ratings(ride))
            {
                // If a peep has already decided that they're going to go on a ride, they'll skip the weather and
                // excitement check and will only do a basic intensity check when they arrive at the ride itself.
                if (rideIndex == this->guest_heading_to_ride_id)
                {
                    if (ride->intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity)
                    {
                        peep_ride_is_too_intense(this, rideIndex, peepAtRide);
                        return false;
                    }
                }

                // Peeps won't go on rides that aren't sufficiently undercover while it's raining.
                // The threshold is fairly low and only requires about 10-15% of the ride to be undercover.
                if (climate_is_raining() && (ride->sheltered_eighths >> 5) < 3)
                {
                    if (peepAtRide)
                    {
                        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_NOT_WHILE_RAINING, rideIndex);
                        if (this->happiness_target >= 64)
                        {
                            this->happiness_target -= 8;
                        }
                        ride_update_popularity(ride, 0);
                    }
                    ChoseNotToGoOnRide(rideIndex, peepAtRide, true);
                    return false;
                }

                if (!gCheatsIgnoreRideIntensity)
                {
                    // Intensity calculations. Even though the max intensity can go up to 15, it's capped
                    // at 10.0 (before happiness calculations). A full happiness bar will increase the max
                    // intensity and decrease the min intensity by about 2.5.
                    ride_rating maxIntensity = Math::Min((this->intensity >> 4) * 100, 1000) + this->happiness;
                    ride_rating minIntensity = ((this->intensity & 0x0F) * 100) - this->happiness;
                    if (ride->intensity < minIntensity)
                    {
                        if (peepAtRide)
                        {
                            peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_MORE_THRILLING, rideIndex);
                            if (this->happiness_target >= 64)
                            {
                                this->happiness_target -= 8;
                            }
                            ride_update_popularity(ride, 0);
                        }
                        ChoseNotToGoOnRide(rideIndex, peepAtRide, true);
                        return false;
                    }
                    if (ride->intensity > maxIntensity)
                    {
                        peep_ride_is_too_intense(this, rideIndex, peepAtRide);
                        return false;
                    }

                    // Nausea calculations.
                    ride_rating maxNausea = NauseaMaximumThresholds[(this->nausea_tolerance & 3)] + this->happiness;

                    if (ride->nausea > maxNausea)
                    {
                        if (peepAtRide)
                        {
                            peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_SICKENING, rideIndex);
                            if (this->happiness_target >= 64)
                            {
                                this->happiness_target -= 8;
                            }
                            ride_update_popularity(ride, 0);
                        }
                        ChoseNotToGoOnRide(rideIndex, peepAtRide, true);
                        return false;
                    }

                    // Very nauseous peeps will only go on very gentle rides.
                    if (ride->nausea >= FIXED_2DP(1, 40) && this->nausea > 160)
                    {
                        ChoseNotToGoOnRide(rideIndex, peepAtRide, false);
                        return false;
                    }
                }
            }

            // If the ride has not yet been rated and is capable of having g-forces,
            // there's a 90% chance that the peep will ignore it.
            if (!ride_has_ratings(ride) && (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_PEEP_CHECK_GFORCES))
            {
                if ((scenario_rand() & 0xFFFF) > 0x1999U)
                {
                    ChoseNotToGoOnRide(rideIndex, peepAtRide, false);
                    return false;
                }

                if (!gCheatsIgnoreRideIntensity)
                {
                    if (ride->max_positive_vertical_g > FIXED_2DP(5, 00) || ride->max_negative_vertical_g < FIXED_2DP(-4, 00) ||
                        ride->max_lateral_g > FIXED_2DP(4, 00))
                    {
                        ChoseNotToGoOnRide(rideIndex, peepAtRide, false);
                        return false;
                    }
                }
            }

            uint32 value = ride->value;

            // If the value of the ride hasn't yet been calculated, peeps will be willing to pay any amount for the ride.
            if (value != 0xFFFF && !peep_has_voucher_for_free_ride(this, rideIndex) && !(gParkFlags & PARK_FLAGS_NO_MONEY))
            {

                // The amount peeps are willing to pay is decreased by 75% if they had to pay to enter the park.
                if (this->peep_flags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY)
                    value /= 4;

                // Peeps won't pay more than twice the value of the ride.
                ridePrice = ride_get_price(ride);
                if (ridePrice > (money16)(value * 2))
                {
                    if (peepAtRide)
                    {
                        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_BAD_VALUE, rideIndex);
                        if (this->happiness_target >= 60)
                        {
                            this->happiness_target -= 16;
                        }
                        ride_update_popularity(ride, 0);
                    }
                    ChoseNotToGoOnRide(rideIndex, peepAtRide, true);
                    return false;
                }

                // A ride is good value if the price is 50% or less of the ride value and the peep didn't pay to enter the park.
                if (ridePrice <= (money16)(value / 2) && peepAtRide)
                {
                    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
                    {
                        if (!(this->peep_flags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY))
                        {
                            peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_GOOD_VALUE, rideIndex);
                        }
                    }
                }
            }
        }

        // At this point, the peep has decided to go on the ride.
        if (peepAtRide)
        {
            ride_update_popularity(ride, 1);
        }

        if (rideIndex == this->guest_heading_to_ride_id)
        {
            peep_reset_ride_heading(this);
        }

        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_QUEUE_FULL;
        return true;
    }

    ChoseNotToGoOnRide(rideIndex, peepAtRide, false);
    return false;
}

bool rct_peep::ShouldGoToShop(sint32 rideIndex, bool peepAtShop)
{
    Ride * ride = get_ride(rideIndex);

    // Peeps won't go to the same shop twice in a row.
    if (rideIndex == this->previous_ride)
    {
        this->ChoseNotToGoOnRide(rideIndex, peepAtShop, true);
        return false;
    }

    if (ride->type == RIDE_TYPE_TOILETS)
    {
        if (this->toilet < 70)
        {
            this->ChoseNotToGoOnRide(rideIndex, peepAtShop, true);
            return false;
        }

        // The amount that peeps are willing to pay to use the Toilets scales with their bathroom stat.
        // It effectively has a minimum of $0.10 (due to the check above) and a maximum of $0.60.
        if (ride->price * 40 > this->toilet)
        {
            if (peepAtShop)
            {
                peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_NOT_PAYING, rideIndex);
                if (this->happiness_target >= 60)
                {
                    this->happiness_target -= 16;
                }
                ride_update_popularity(ride, 0);
            }
            this->ChoseNotToGoOnRide(rideIndex, peepAtShop, true);
            return false;
        }
    }

    if (ride->type == RIDE_TYPE_FIRST_AID)
    {
        if (this->nausea < 128)
        {
            this->ChoseNotToGoOnRide(rideIndex, peepAtShop, true);
            return false;
        }
    }

    // Basic price checks
    if (ride->price != 0 && ride->price > this->cash_in_pocket)
    {
        if (peepAtShop)
        {
            if (this->cash_in_pocket <= 0)
            {
                peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_SPENT_MONEY, PEEP_THOUGHT_ITEM_NONE);
            }
            else
            {
                peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_CANT_AFFORD_0, rideIndex);
            }
        }
        this->ChoseNotToGoOnRide(rideIndex, peepAtShop, true);
        return false;
    }

    if (peepAtShop)
    {
        ride_update_popularity(ride, 1);
        if (rideIndex == this->guest_heading_to_ride_id)
        {
            peep_reset_ride_heading(this);
        }
    }
    return true;
}

void rct_peep::ChoseNotToGoOnRide(sint32 rideIndex, bool peepAtRide, bool updateLastRide)
{
    if (peepAtRide && updateLastRide)
    {
        previous_ride = rideIndex;
        previous_ride_time_out = 0;
    }

    if (rideIndex == guest_heading_to_ride_id)
    {
        peep_reset_ride_heading(this);
    }
}

void rct_peep::ReadMap()
{
    if (action == PEEP_ACTION_NONE_1 || action == PEEP_ACTION_NONE_2)
    {
        action = PEEP_ACTION_READ_MAP;
        action_frame = 0;
        action_sprite_image_offset = 0;
        UpdateCurrentActionSpriteType();
        Invalidate();
    }
}

static bool peep_has_voucher_for_free_ride(rct_peep * peep, sint32 rideIndex)
{
    return peep->item_standard_flags & PEEP_ITEM_VOUCHER && peep->voucher_type == VOUCHER_TYPE_RIDE_FREE &&
           peep->voucher_arguments == rideIndex;
}

/**
 * When the queue is full, peeps will ignore the ride when thinking about what to go on next.
 * Does not effect peeps that walk up to the queue entrance.
 * This flag is reset the next time a peep successfully joins the queue.
 */
static void peep_tried_to_enter_full_queue(rct_peep * peep, sint32 rideIndex)
{
    Ride * ride = get_ride(rideIndex);

    ride->lifecycle_flags |= RIDE_LIFECYCLE_QUEUE_FULL;
    peep->previous_ride          = rideIndex;
    peep->previous_ride_time_out = 0;
    // Change status "Heading to" to "Walking" if queue is full
    if (rideIndex == peep->guest_heading_to_ride_id)
    {
        peep_reset_ride_heading(peep);
    }
}

static void peep_reset_ride_heading(rct_peep * peep)
{
    rct_window * w;

    peep->guest_heading_to_ride_id = 255;
    w                              = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != nullptr)
    {
        window_event_invalidate_call(w);
        widget_invalidate(w, WC_PEEP__WIDX_ACTION_LBL);
    }
}

static void peep_ride_is_too_intense(rct_peep * peep, sint32 rideIndex, bool peepAtRide)
{
    Ride * ride = get_ride(rideIndex);

    if (peepAtRide)
    {
        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_INTENSE, rideIndex);
        if (peep->happiness_target >= 64)
        {
            peep->happiness_target -= 8;
        }
        ride_update_popularity(ride, 0);
    }
    peep->ChoseNotToGoOnRide(rideIndex, peepAtRide, true);
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

    peep->SetState(PEEP_STATE_ENTERING_RIDE);
    peep->sub_state = PEEP_RIDE_IN_ENTRANCE;

    peep->rejoin_queue_timeout       = 0;
    peep->time_on_ride = 0;

    peep->RemoveFromQueue();
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
        peep->RemoveFromQueue();
        peep->SetState(PEEP_STATE_FALLING);
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
void rct_peep::StopPurchaseThought(uint8 ride_type)
{

    uint8 thoughtType = PEEP_THOUGHT_TYPE_HUNGRY;

    if (!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_SELLS_FOOD))
    {
        thoughtType = PEEP_THOUGHT_TYPE_THIRSTY;
        if (!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_SELLS_DRINKS))
        {
            thoughtType = PEEP_THOUGHT_TYPE_RUNNING_OUT;
            if (ride_type != RIDE_TYPE_CASH_MACHINE)
            {
                thoughtType = PEEP_THOUGHT_TYPE_BATHROOM;
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
        rct_peep_thought * thought = &thoughts[i];

        if (thought->type == PEEP_THOUGHT_TYPE_NONE)
            break;

        if (thought->type != thoughtType)
            continue;

        if (i < PEEP_MAX_THOUGHTS - 1)
        {
            memmove(thought, thought + 1, sizeof(rct_peep_thought) * (PEEP_MAX_THOUGHTS - i - 1));
        }

        thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;

        window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
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
        SetState(PEEP_STATE_FALLING);
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

        SetState(PEEP_STATE_WALKING);
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

        StopPurchaseThought(ride->type);
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

    peep->SetState(PEEP_STATE_QUEUING_FRONT);
    peep->sub_state = PEEP_RIDE_AT_ENTRANCE;

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
    sint16 actionX, actionY, xy_distance;
    Ride * ride = get_ride(current_ride);

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        Invalidate();
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
        bool lastRide = false;
        if (ride->status != RIDE_STATUS_OPEN)
            lastRide = true;
        else if (current_car++ != 0)
        {
            if (ride->mode == RIDE_MODE_SINGLE_RIDE_PER_ADMISSION)
                lastRide = true;
            if ((uint8)(current_car - 1) > (scenario_rand() & 0xF))
                lastRide = true;
        }

        if (lastRide)
        {
            auto exit = ride_get_exit_location(current_ride, current_ride_station);
            waypoint = 1;
            var_37 = (exit.direction * 4) | (var_37 & 0x30) | waypoint;
            actionX            = ride->station_starts[current_ride_station].x * 32;
            actionY            = ride->station_starts[current_ride_station].y * 32;

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

    actionX = ride->station_starts[current_ride_station].x * 32;
    actionY = ride->station_starts[current_ride_station].y * 32;

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
            Invalidate();

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
        Invalidate();
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        Invalidate();
        return;
    }
    uint8 waypoint = 2;
    var_37 = (var_37 * 4 & 0x30) + waypoint;

    actionX = ride->station_starts[current_ride_station].x * 32;
    actionY = ride->station_starts[current_ride_station].y * 32;

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
    sint16 actionX, actionY, xy_distance;

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        Invalidate();
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
        actionY = ride->station_starts[current_ride_station].y * 32;

        assert(ride->type == RIDE_TYPE_SPIRAL_SLIDE);
        const CoordsXY slidePlatformDestination = SpiralSlideWalkingPath[var_37];

        actionX += slidePlatformDestination.x;
        actionY += slidePlatformDestination.y;

        destination_x = actionX;
        destination_y = actionY;
        return;
    }
    waypoint = 3;
    // Actually force the final waypoint
    var_37 |= 3;

    auto exit = ride_get_exit_location(current_ride, current_ride_station);
    actionX         = exit.x * 32 + 16;
    actionY         = exit.y * 32 + 16;

    exit.direction ^= 2;

    sint16 xShift = word_981D6C[exit.direction].x;
    sint16 yShift = word_981D6C[exit.direction].y;

    sint16 shiftMultiplier = 20;

    xShift *= shiftMultiplier;
    yShift *= shiftMultiplier;

    actionX -= xShift;
    actionY -= yShift;

    destination_x = actionX;
    destination_y = actionY;
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
        Invalidate();
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        Invalidate();
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
            Invalidate();
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

    uint16 mazeEntry  = track_element_get_maze_entry(tileElement);
    uint16 openHedges = 0;
    // var_37 is 3, 7, 11 or 15

    if (mazeEntry & (1 << _981FF4[var_37 / 4][3]))
    {
        openHedges = 1;
    }
    openHedges <<= 1;
    if (mazeEntry & (1 << _981FF4[var_37 / 4][2]))
    {
        openHedges |= 1;
    }
    openHedges <<= 1;
    if (mazeEntry & (1 << _981FF4[var_37 / 4][1]))
    {
        openHedges |= 1;
    }
    openHedges <<= 1;
    if (mazeEntry & (1 << _981FF4[var_37 / 4][0]))
    {
        openHedges |= 1;
    }

    openHedges ^= 0xF;
    if (openHedges == 0)
        return;

    uint8 mazeLastEdge = mazeLastEdge ^ (1 << 1);
    openHedges &= ~(1 << mazeLastEdge);
    if (openHedges == 0)
        openHedges |= (1 << mazeLastEdge);

    uint8 chosenEdge = scenario_rand() & 0x3;
    while (!(openHedges & (1 << chosenEdge)))
    {
        chosenEdge = (chosenEdge + 1) & 3;
    }

    actionX = TileDirectionDelta[chosenEdge].x / 2;
    actionY = TileDirectionDelta[chosenEdge].y / 2;

    actionX += destination_x;
    actionY += destination_y;

    enum class maze_type
    {
        invalid,
        hedge,
        entrance_or_exit
    };
    maze_type mazeType = maze_type::invalid;

    tileElement = map_get_first_element_at(actionX / 32, actionY / 32);
    do
    {
        if (stationHeight != tileElement->base_height)
            continue;

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_TRACK)
        {
            mazeType = maze_type::hedge;
            break;
        }

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_ENTRANCE &&
            tileElement->properties.entrance.type == ENTRANCE_TYPE_RIDE_EXIT)
        {
            mazeType = maze_type::entrance_or_exit;
            break;
        }
    } while (!tile_element_is_last_for_tile(tileElement++));

    switch (mazeType)
    {
    case maze_type::invalid:
        maze_last_edge++;
        maze_last_edge &= 3;
        return;
    case maze_type::hedge:
        destination_x = actionX;
        destination_y = actionY;

        var_37         = _981FE4[var_37 / 4][chosenEdge];
        maze_last_edge = chosenEdge;
        break;
    case maze_type::entrance_or_exit:
        actionX = destination_x;
        actionY = destination_y;
        if (chosenEdge & 1)
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
        maze_last_edge = chosenEdge;
        break;
    }

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        Invalidate();
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
        Invalidate();
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        Invalidate();
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
    StopPurchaseThought(ride->type);
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
        Invalidate();
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        Invalidate();

        actionX = x & 0xFFE0;
        actionY = y & 0xFFE0;
        if (actionX != next_x)
            return;
        if (actionY != next_y)
            return;
    }

    SetState(PEEP_STATE_WALKING);

    Ride * ride = get_ride(current_ride);
    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

    ride_update_satisfaction(ride, happiness / 64);
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

static sint32 peep_update_walking_find_bench(rct_peep * peep);
static sint32 peep_update_walking_find_bin(rct_peep * peep);
static void peep_update_walking_break_scenery(rct_peep * peep);
static bool peep_find_ride_to_look_at(rct_peep * peep, uint8 edge, uint8 * rideToView, uint8 * rideSeatToView);

/**
 *
 *  rct2: 0x0069030A
 */
void rct_peep::UpdateWalking()
{
    if (!CheckForPath())
        return;

    if (peep_flags & PEEP_FLAGS_WAVING)
    {
        if (action >= PEEP_ACTION_NONE_1)
        {
            if ((0xFFFF & scenario_rand()) < 936)
            {
                invalidate_sprite_2((rct_sprite *)this);

                action                     = PEEP_ACTION_WAVE_2;
                action_frame               = 0;
                action_sprite_image_offset = 0;

                UpdateCurrentActionSpriteType();
                invalidate_sprite_2((rct_sprite *)this);
            }
        }
    }

    if (peep_flags & PEEP_FLAGS_PHOTO)
    {
        if (action >= PEEP_ACTION_NONE_1)
        {
            if ((0xFFFF & scenario_rand()) < 936)
            {
                invalidate_sprite_2((rct_sprite *)this);

                action                     = PEEP_ACTION_TAKE_PHOTO;
                action_frame               = 0;
                action_sprite_image_offset = 0;

                UpdateCurrentActionSpriteType();
                invalidate_sprite_2((rct_sprite *)this);
            }
        }
    }

    if (peep_flags & PEEP_FLAGS_PAINTING)
    {
        if (action >= PEEP_ACTION_NONE_1)
        {
            if ((0xFFFF & scenario_rand()) < 936)
            {
                invalidate_sprite_2((rct_sprite *)this);

                action                     = PEEP_ACTION_DRAW_PICTURE;
                action_frame               = 0;
                action_sprite_image_offset = 0;

                UpdateCurrentActionSpriteType();
                invalidate_sprite_2((rct_sprite *)this);
            }
        }
    }

    if (peep_flags & PEEP_FLAGS_LITTER)
    {
        if (!(next_var_29 & 0x18))
        {
            if ((0xFFFF & scenario_rand()) <= 4096)
            {
                static constexpr const uint8 litter_types[] = {
                    LITTER_TYPE_EMPTY_CAN,
                    LITTER_TYPE_RUBBISH,
                    LITTER_TYPE_EMPTY_BURGER_BOX,
                    LITTER_TYPE_EMPTY_CUP,
                };
                sint32 ebp       = litter_types[scenario_rand() & 0x3];
                sint32 litterX   = x + (scenario_rand() & 0x7) - 3;
                sint32 litterY   = y + (scenario_rand() & 0x7) - 3;
                sint32 litterDirection = (scenario_rand() & 0x3);

                litter_create(litterX, litterY, z, litterDirection, ebp);
            }
        }
    }
    else if (HasEmptyContainer())
    {
        if ((!(next_var_29 & 0x18)) && ((uint32)(sprite_index & 0x1FF) == (gCurrentTicks & 0x1FF)) &&
            ((0xFFFF & scenario_rand()) <= 4096))
        {

            uint8 pos_stnd = 0;
            for (sint32 container = HasEmptyContainerStandardFlag(); pos_stnd < 32; pos_stnd++)
                if (container & (1u << pos_stnd))
                    break;

            sint32 bp = 0;

            if (pos_stnd != 32)
            {
                item_standard_flags &= ~(1u << pos_stnd);
                bp = item_standard_litter[pos_stnd];
            }
            else
            {
                uint8 pos_extr = 0;
                for (sint32 container = HasEmptyContainerExtraFlag(); pos_extr < 32; pos_extr++)
                    if (container & (1u << pos_extr))
                        break;
                item_extra_flags &= ~(1u << pos_extr);
                bp = item_extra_litter[pos_extr];
            }

            window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
            peep_update_sprite_type(this);

            sint32 litterX   = x + (scenario_rand() & 0x7) - 3;
            sint32 litterY   = y + (scenario_rand() & 0x7) - 3;
            sint32 litterDirection = (scenario_rand() & 0x3);

            litter_create(litterX, litterY, z, litterDirection, bp);
        }
    }

    uint8 pathingResult;
    PerformNextAction(pathingResult);
    if (!(pathingResult & PATHING_DESTINATION_REACHED))
        return;

    if ((next_var_29 & 0x18) == 8)
    {
        rct_tile_element * tile_element = map_get_surface_element_at({next_x, next_y});

        sint32 water_height = map_get_water_height(tile_element);
        if (water_height)
        {
            invalidate_sprite_2((rct_sprite *)this);
            water_height *= 16;
            sprite_move(x, y, water_height, (rct_sprite *)this);
            invalidate_sprite_2((rct_sprite *)this);

            SetState(PEEP_STATE_FALLING);
            return;
        }
    }

    CheckIfLost();
    CheckCantFindRide();
    CheckCantFindExit();

    if (peep_update_walking_find_bench(this))
        return;

    if (peep_update_walking_find_bin(this))
        return;

    peep_update_walking_break_scenery(this);

    if (state != PEEP_STATE_WALKING)
        return;

    if (peep_flags & PEEP_FLAGS_LEAVING_PARK)
        return;

    if (nausea > 140)
        return;

    if (happiness < 120)
        return;

    if (toilet > 140)
        return;

    uint16 chance = HasFoodExtraFlag() ? 13107 : 2849;

    if ((scenario_rand() & 0xFFFF) > chance)
        return;

    if (next_var_29 & 0x1C)
        return;

    rct_tile_element * tile_element = map_get_first_element_at(next_x / 32, next_y / 32);

    for (;; tile_element++)
    {
        if (tile_element_get_type(tile_element) == TILE_ELEMENT_TYPE_PATH)
        {
            if (next_z == tile_element->base_height)
                break;
        }
        if (tile_element_is_last_for_tile(tile_element))
        {
            return;
        }
    }

    sint32 positions_free = 15;

    if (footpath_element_has_path_scenery(tile_element))
    {
        if (!footpath_element_path_scenery_is_ghost(tile_element))
        {
            rct_scenery_entry * sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tile_element));
            if (sceneryEntry == nullptr)
            {
                return;
            }

            if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BENCH))
                positions_free = 9;
        }
    }

    sint32 edges = (tile_element->properties.path.edges & 0xF) ^ 0xF;
    if (edges == 0)
        return;

    uint8 chosen_edge = scenario_rand() & 0x3;

    for (; !(edges & (1 << chosen_edge));)
        chosen_edge = (chosen_edge + 1) & 3;

    uint8 ride_to_view, ride_seat_to_view;
    if (!peep_find_ride_to_look_at(this, chosen_edge, &ride_to_view, &ride_seat_to_view))
        return;

    // Check if there is a peep watching (and if there is place for us)
    uint16 sprite_id = sprite_get_first_in_quadrant(x, y);
    for (rct_sprite * sprite; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->unknown.next_in_quadrant)
    {
        sprite = get_sprite(sprite_id);

        if (sprite->unknown.linked_list_type_offset != SPRITE_LIST_PEEP * 2)
            continue;

        if (sprite->peep.state != PEEP_STATE_WATCHING)
            continue;

        if (z != sprite->peep.z)
            continue;

        if ((sprite->peep.var_37 & 0x3) != chosen_edge)
            continue;

        positions_free &= ~(1 << ((sprite->peep.var_37 & 0x1C) >> 2));
    }

    if (!positions_free)
        return;

    uint8 chosen_position = scenario_rand() & 0x3;

    for (; !(positions_free & (1 << chosen_position));)
        chosen_position = (chosen_position + 1) & 3;

    current_ride = ride_to_view;
    current_seat = ride_seat_to_view;
    var_37       = chosen_edge | (chosen_position << 2);

    SetState(PEEP_STATE_WATCHING);
    sub_state = 0;

    sint32 ebx = var_37 & 0x1F;
    sint32 destX = (x & 0xFFE0) + _981F4C[ebx].x;
    sint32 destY = (y & 0xFFE0) + _981F4C[ebx].y;

    destination_x         = destX;
    destination_y         = destY;
    destination_tolerance = 3;

    if (current_seat & 1)
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_NEW_RIDE, PEEP_THOUGHT_ITEM_NONE);
    }
    if (current_ride == 0xFF)
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_SCENERY, PEEP_THOUGHT_ITEM_NONE);
    }
}

/**
 *
 *  rct2: 0x69185D
 */
void rct_peep::UpdateQueuing()
{
    if (!CheckForPath())
    {
        RemoveFromQueue();
        return;
    }
    Ride * ride = get_ride(current_ride);
    if (ride->status == RIDE_STATUS_CLOSED || ride->status == RIDE_STATUS_TESTING)
    {
        RemoveFromQueue();
        SetState(PEEP_STATE_1);
        return;
    }

    if (sub_state != 10)
    {
        bool is_front = true;
        if (next_in_queue != SPRITE_INDEX_NULL)
        {
            // Fix #4819: Occasionally the peep->next_in_queue is incorrectly set
            // to prevent this from causing the peeps to enter a loop
            // first check if the next in queue is actually nearby
            // if they are not then it's safe to assume that this is
            // the front of the queue.
            rct_peep * next_peep = GET_PEEP(next_in_queue);
            if (abs(next_peep->x - x) < 32 && abs(next_peep->y - y) < 32)
            {
                is_front = false;
            }
        }

        if (is_front)
        {
            // Happens every time peep goes onto ride.
            destination_tolerance = 0;
            SetState(PEEP_STATE_QUEUING_FRONT);
            sub_state = PEEP_RIDE_AT_ENTRANCE;
            return;
        }

        // Give up queueing for the ride
        sprite_direction ^= (1 << 4);
        invalidate_sprite_2((rct_sprite *)this);
        RemoveFromQueue();
        SetState(PEEP_STATE_1);
        return;
    }

    uint8 pathingResult;
    PerformNextAction(pathingResult);
    if (action < 0xFE)
        return;
    if (sprite_type == PEEP_SPRITE_TYPE_NORMAL)
    {
        if (time_in_queue >= 2000 && (0xFFFF & scenario_rand()) <= 119)
        {
            // Eat Food/Look at watch
            action                     = PEEP_ACTION_EAT_FOOD;
            action_frame               = 0;
            action_sprite_image_offset = 0;
            UpdateCurrentActionSpriteType();
            invalidate_sprite_2((rct_sprite *)this);
        }
        if (time_in_queue >= 3500 && (0xFFFF & scenario_rand()) <= 93)
        {
            // Create the I have been waiting in line ages thought
            peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_QUEUING_AGES, current_ride);
        }
    }
    else
    {
        if (!(time_in_queue & 0x3F) && action == 0xFE && next_action_sprite_type == 2)
        {
            switch (sprite_type)
            {
            case PEEP_SPRITE_TYPE_ICE_CREAM:
            case PEEP_SPRITE_TYPE_CHIPS:
            case PEEP_SPRITE_TYPE_BURGER:
            case PEEP_SPRITE_TYPE_DRINK:
            case PEEP_SPRITE_TYPE_CANDYFLOSS:
            case PEEP_SPRITE_TYPE_PIZZA:
            case PEEP_SPRITE_TYPE_POPCORN:
            case PEEP_SPRITE_TYPE_HOT_DOG:
            case PEEP_SPRITE_TYPE_TENTACLE:
            case PEEP_SPRITE_TYPE_TOFFEE_APPLE:
            case PEEP_SPRITE_TYPE_DOUGHNUT:
            case PEEP_SPRITE_TYPE_COFFEE:
            case PEEP_SPRITE_TYPE_CHICKEN:
            case PEEP_SPRITE_TYPE_LEMONADE:
            case PEEP_SPRITE_TYPE_PRETZEL:
            case PEEP_SPRITE_TYPE_SU_JONGKWA:
            case PEEP_SPRITE_TYPE_JUICE:
            case PEEP_SPRITE_TYPE_FUNNEL_CAKE:
            case PEEP_SPRITE_TYPE_NOODLES:
            case PEEP_SPRITE_TYPE_SAUSAGE:
            case PEEP_SPRITE_TYPE_SOUP:
            case PEEP_SPRITE_TYPE_SANDWICH:
                // Eat food
                action                     = PEEP_ACTION_EAT_FOOD;
                action_frame               = 0;
                action_sprite_image_offset = 0;
                UpdateCurrentActionSpriteType();
                invalidate_sprite_2((rct_sprite *)this);
                break;
            }
        }
    }
    if (time_in_queue < 4300)
        return;

    if (happiness <= 65 && (0xFFFF & scenario_rand()) < 2184)
    {
        // Give up queueing for the ride
        sprite_direction ^= (1 << 4);
        invalidate_sprite_2((rct_sprite *)this);
        RemoveFromQueue();
        SetState(PEEP_STATE_1);
    }
}

/**
 * rct2: 0x691451
 */
void rct_peep::UpdateEnteringPark()
{
    if (var_37 != 1)
    {
        uint8 pathingResult;
        PerformNextAction(pathingResult);
        if ((pathingResult & PATHING_OUTSIDE_PARK))
        {
            decrement_guests_heading_for_park();
            peep_sprite_remove(this);
        }
        return;
    }
    sint16 actionX = 0;
    sint16 actionY = 0;
    sint16 xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        invalidate_sprite_2((rct_sprite *)this);
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        invalidate_sprite_2((rct_sprite *)this);
        return;
    }
    SetState(PEEP_STATE_FALLING);

    outside_of_park = 0;
    time_in_park    = gScenarioTicks;
    increment_guests_in_park();
    decrement_guests_heading_for_park();
    auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
    context_broadcast_intent(&intent);
}

/**
 *
 *  rct2: 0x6914CD
 */
void rct_peep::UpdateLeavingPark()
{
    if (var_37 != 0)
    {
        uint8 pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_OUTSIDE_PARK))
            return;
        peep_sprite_remove(this);
        return;
    }

    sint16 actionX = 0;
    sint16 actionY = 0;
    sint16 xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        invalidate_sprite_2((rct_sprite *)this);
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        invalidate_sprite_2((rct_sprite *)this);
        return;
    }

    outside_of_park       = 1;
    destination_tolerance = 5;
    decrement_guests_in_park();
    auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
    context_broadcast_intent(&intent);
    var_37 = 1;

    window_invalidate_by_class(WC_GUEST_LIST);
    uint8 pathingResult;
    PerformNextAction(pathingResult);
    if (!(pathingResult & PATHING_OUTSIDE_PARK))
        return;
    peep_sprite_remove(this);
}

/**
 *
 *  rct2: 0x6916D6
 */
void rct_peep::UpdateWatching()
{
    if (sub_state == 0)
    {
        if (!CheckForPath())
            return;
        uint8 pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        destination_x = x;
        destination_y = y;

        sprite_direction = (var_37 & 3) * 8;
        invalidate_sprite_2((rct_sprite *)this);

        action                  = 0xFE;
        next_action_sprite_type = 2;

        SwitchNextActionSpriteType();

        sub_state++;

        time_to_stand = Math::Clamp(0, ((129 - energy) * 16 + 50) / 2, 255);
        peep_update_sprite_type(this);
    }
    else if (sub_state == 1)
    {
        if (action < 0xFE)
        {
            // 6917F6
            sint16 actionX = 0;
            sint16 actionY = 0;
            sint16 xy_distance;
            UpdateAction(&actionX, &actionY, &xy_distance);

            if (action != 0xFF)
                return;
            action = 0xFE;
        }
        else
        {
            if (HasFood())
            {
                if ((scenario_rand() & 0xFFFF) <= 1310)
                {
                    action                     = PEEP_ACTION_EAT_FOOD;
                    action_frame               = 0;
                    action_sprite_image_offset = 0;
                    UpdateCurrentActionSpriteType();
                    invalidate_sprite_2((rct_sprite *)this);
                    return;
                }
            }

            if ((scenario_rand() & 0xFFFF) <= 655)
            {
                action                     = PEEP_ACTION_TAKE_PHOTO;
                action_frame               = 0;
                action_sprite_image_offset = 0;
                UpdateCurrentActionSpriteType();
                invalidate_sprite_2((rct_sprite *)this);
                return;
            }

            if ((standing_flags & 1))
            {
                if ((scenario_rand() & 0xFFFF) <= 655)
                {
                    action                     = PEEP_ACTION_WAVE;
                    action_frame               = 0;
                    action_sprite_image_offset = 0;
                    UpdateCurrentActionSpriteType();
                    invalidate_sprite_2((rct_sprite *)this);
                    return;
                }
            }
        }

        standing_flags ^= (1 << 7);
        if (!(standing_flags & (1 << 7)))
            return;

        time_to_stand--;
        if (time_to_stand != 0)
            return;

        SetState(PEEP_STATE_WALKING);
        peep_update_sprite_type(this);
        // Send peep to the centre of current tile.
        destination_x         = (x & 0xFFE0) + 16;
        destination_y         = (y & 0xFFE0) + 16;
        destination_tolerance = 5;
        UpdateCurrentActionSpriteType();
    }
}

/**
 *
 *  rct2: 0x00691089
 */
void rct_peep::UpdateUsingBin()
{
    if (sub_state == 0)
    {
        if (!CheckForPath())
            return;

        uint8 pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        sub_state = 1;
    }
    else if (sub_state == 1)
    {

        if (action != PEEP_ACTION_NONE_2)
        {
            sint16 actionX, actionY, xy_distance;
            UpdateAction(&actionX, &actionY, &xy_distance);
            return;
        }

        rct_tile_element * tile_element = map_get_first_element_at(next_x / 32, next_y / 32);

        for (;; tile_element++)
        {
            if (tile_element_get_type(tile_element) != TILE_ELEMENT_TYPE_PATH)
            {
                continue;
            }

            if (tile_element->base_height == next_z)
                break;

            if (tile_element_is_last_for_tile(tile_element))
            {
                StateReset();
                return;
            }
        }

        if (!footpath_element_has_path_scenery(tile_element))
        {
            StateReset();
            return;
        }

        rct_scenery_entry * sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tile_element));
        if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BIN))
        {
            StateReset();
            return;
        }

        if (tile_element->flags & TILE_ELEMENT_FLAG_BROKEN)
        {
            StateReset();
            return;
        }

        if (footpath_element_path_scenery_is_ghost(tile_element))
        {
            StateReset();
            return;
        }

        // Bin selection is one of 4 corners
        uint8 selected_bin = var_37 * 2;

        // This counts down 2 = No rubbish, 0 = full
        uint8  space_left_in_bin = 0x3 & (tile_element->properties.path.addition_status >> selected_bin);
        uint32 empty_containers  = HasEmptyContainerStandardFlag();

        for (uint8 cur_container = 0; cur_container < 32; cur_container++)
        {
            if (!(empty_containers & (1u << cur_container)))
                continue;

            if (space_left_in_bin != 0)
            {
                // OpenRCT2 modification: This previously used
                // the tick count as a simple random function
                // switched to scenario_rand as it is more reliable
                if ((scenario_rand() & 7) == 0)
                    space_left_in_bin--;
                item_standard_flags &= ~(1 << cur_container);
                window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                peep_update_sprite_type(this);
                continue;
            }
            uint8 bp = item_standard_litter[cur_container];

            sint32 litterX = x + (scenario_rand() & 7) - 3;
            sint32 litterY = y + (scenario_rand() & 7) - 3;

            litter_create(litterX, litterY, z, scenario_rand() & 3, bp);
            item_standard_flags &= ~(1 << cur_container);
            window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;

            peep_update_sprite_type(this);
        }

        // Original bug: This would clear any rubbish placed by the previous function
        // space_left_in_bin = 0x3 & (tile_element->properties.path.addition_status >> selected_bin);
        empty_containers = HasEmptyContainerExtraFlag();

        for (uint8 cur_container = 0; cur_container < 32; cur_container++)
        {
            if (!(empty_containers & (1u << cur_container)))
                continue;

            if (space_left_in_bin != 0)
            {
                // OpenRCT2 modification: This previously used
                // the tick count as a simple random function
                // switched to scenario_rand as it is more reliable
                if ((scenario_rand() & 7) == 0)
                    space_left_in_bin--;
                item_extra_flags &= ~(1 << cur_container);
                window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;

                peep_update_sprite_type(this);
                continue;
            }
            uint8 bp = item_extra_litter[cur_container];

            sint32 litterX = x + (scenario_rand() & 7) - 3;
            sint32 litterY = y + (scenario_rand() & 7) - 3;

            litter_create(litterX, litterY, z, scenario_rand() & 3, bp);
            item_extra_flags &= ~(1 << cur_container);
            window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;

            peep_update_sprite_type(this);
        }

        // Place new amount in bin by first clearing the value
        tile_element->properties.path.addition_status &= ~(3 << selected_bin);
        // Then placing the new value.
        tile_element->properties.path.addition_status |= space_left_in_bin << selected_bin;

        map_invalidate_tile_zoom0(next_x, next_y, tile_element->base_height << 3,
                                  tile_element->clearance_height << 3);
        StateReset();
    }
}

/* Simplifies 0x690582. Returns 1 if should find bench*/
static bool peep_should_find_bench(rct_peep * peep)
{
    if (!(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK))
    {
        if (peep->HasFood())
        {
            if (peep->hunger < 128 || peep->happiness < 128)
            {
                if (!(peep->next_var_29 & 0x1C))
                {
                    return true;
                }
            }
        }
        if (peep->nausea <= 170 && peep->energy > 50)
        {
            return false;
        }

        if (!(peep->next_var_29 & 0x1C))
        {
            return true;
        }
    }
    return false;
}

/**
 *
 *  rct2: 0x00690582
 */
static sint32 peep_update_walking_find_bench(rct_peep * peep)
{
    if (!peep_should_find_bench(peep))
        return 0;

    rct_tile_element * tile_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

    for (;; tile_element++)
    {
        if (tile_element_get_type(tile_element) == TILE_ELEMENT_TYPE_PATH)
        {
            if (peep->next_z == tile_element->base_height)
                break;
        }
        if (tile_element_is_last_for_tile(tile_element))
        {
            return 0;
        }
    }

    if (!footpath_element_has_path_scenery(tile_element))
        return 0;
    rct_scenery_entry * sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tile_element));

    if (sceneryEntry == nullptr || !(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BENCH))
        return 0;

    if (tile_element->flags & TILE_ELEMENT_FLAG_BROKEN)
        return 0;

    if (footpath_element_path_scenery_is_ghost(tile_element))
        return 0;

    sint32 edges = (tile_element->properties.path.edges & 0xF) ^ 0xF;
    if (edges == 0)
        return 0;

    uint8 chosen_edge = scenario_rand() & 0x3;

    for (; !(edges & (1 << chosen_edge));)
        chosen_edge = (chosen_edge + 1) & 0x3;

    uint16 sprite_id = sprite_get_first_in_quadrant(peep->x, peep->y);
    uint8  free_edge = 3;

    // Check if there is no peep sitting in chosen_edge
    for (rct_sprite * sprite; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->unknown.next_in_quadrant)
    {
        sprite = get_sprite(sprite_id);

        if (sprite->unknown.linked_list_type_offset != SPRITE_LIST_PEEP * 2)
            continue;

        if (sprite->peep.state != PEEP_STATE_SITTING)
            continue;

        if (peep->z != sprite->peep.z)
            continue;

        if ((sprite->peep.var_37 & 0x3) != chosen_edge)
            continue;

        free_edge &= ~(1 << ((sprite->peep.var_37 & 0x4) >> 2));
    }

    if (!free_edge)
        return 0;

    free_edge ^= 0x3;
    if (!free_edge)
    {
        if (scenario_rand() & 0x8000000)
            free_edge = 1;
    }

    peep->var_37 = ((free_edge & 1) << 2) | chosen_edge;

    peep->SetState(PEEP_STATE_SITTING);

    peep->sub_state = PEEP_SITTING_TRYING_TO_SIT;

    sint32 ebx = peep->var_37 & 0x7;
    sint32 x   = (peep->x & 0xFFE0) + _981F2C[ebx].x;
    sint32 y   = (peep->y & 0xFFE0) + _981F2C[ebx].y;

    peep->destination_x         = x;
    peep->destination_y         = y;
    peep->destination_tolerance = 3;

    return 1;
}

static sint32 peep_update_walking_find_bin(rct_peep * peep)
{
    if (!peep->HasEmptyContainer())
        return 0;

    if (peep->next_var_29 & 0x18)
        return 0;

    rct_tile_element * tile_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

    for (;; tile_element++)
    {
        if (tile_element_get_type(tile_element) == TILE_ELEMENT_TYPE_PATH)
        {
            if (peep->next_z == tile_element->base_height)
                break;
        }
        if (tile_element_is_last_for_tile(tile_element))
        {
            return 0;
        }
    }

    if (!footpath_element_has_path_scenery(tile_element))
        return 0;
    rct_scenery_entry * sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tile_element));
    if (sceneryEntry == nullptr)
    {
        return 0;
    }

    if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BIN))
        return 0;

    if (tile_element->flags & TILE_ELEMENT_FLAG_BROKEN)
        return 0;

    if (footpath_element_path_scenery_is_ghost(tile_element))
        return 0;

    sint32 edges = (tile_element->properties.path.edges & 0xF) ^ 0xF;
    if (edges == 0)
        return 0;

    uint8 chosen_edge = scenario_rand() & 0x3;

    // Note: Bin quantity is inverted 0 = full, 3 = empty
    uint8 bin_quantities = tile_element->properties.path.addition_status;

    // Rotate the bin to the correct edge. Makes it easier for next calc.
    bin_quantities = ror8(ror8(bin_quantities, chosen_edge), chosen_edge);

    for (uint8 free_edge = 4; free_edge != 0; free_edge--)
    {
        // If not full
        if (bin_quantities & 0x3)
        {
            if (edges & (1 << chosen_edge))
                break;
        }
        chosen_edge    = (chosen_edge + 1) & 0x3;
        bin_quantities = ror8(bin_quantities, 2);
        if ((free_edge - 1) == 0)
            return 0;
    }

    peep->var_37 = chosen_edge;

    peep->SetState(PEEP_STATE_USING_BIN);
    peep->sub_state = 0;

    sint32 ebx = peep->var_37 & 0x3;
    sint32 x   = (peep->x & 0xFFE0) + BinUseOffsets[ebx].x;
    sint32 y   = (peep->y & 0xFFE0) + BinUseOffsets[ebx].y;

    peep->destination_x         = x;
    peep->destination_y         = y;
    peep->destination_tolerance = 3;

    return 1;
}

/**
 *
 *  rct2: 0x00690848
 */
static void peep_update_walking_break_scenery(rct_peep * peep)
{
    if (gCheatsDisableVandalism)
        return;

    if (!(peep->peep_flags & PEEP_FLAGS_ANGRY))
    {
        if (peep->happiness >= 48)
            return;
        if (peep->energy < 85)
            return;
        if (peep->state != PEEP_STATE_WALKING)
            return;

        if ((peep->litter_count & 0xC0) != 0xC0 && (peep->disgusting_count & 0xC0) != 0xC0)
            return;

        if ((scenario_rand() & 0xFFFF) > 3276)
            return;
    }

    if (peep->next_var_29 & 0x18)
        return;

    rct_tile_element * tile_element = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

    for (;; tile_element++)
    {
        if (tile_element_get_type(tile_element) == TILE_ELEMENT_TYPE_PATH)
        {
            if (peep->next_z == tile_element->base_height)
                break;
        }
        if (tile_element_is_last_for_tile(tile_element))
        {
            return;
        }
    }

    if (!footpath_element_has_path_scenery(tile_element))
        return;
    rct_scenery_entry * sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tile_element));

    if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_BREAKABLE))
        return;

    if (tile_element->flags & TILE_ELEMENT_FLAG_BROKEN)
        return;

    if (footpath_element_path_scenery_is_ghost(tile_element))
        return;

    sint32 edges = tile_element->properties.path.edges & 0xF;
    if (edges == 0xF)
        return;

    uint16 sprite_id = sprite_get_first_in_quadrant(peep->x, peep->y);

    // Check if a peep is already sitting on the bench. If so, do not vandalise it.
    for (rct_sprite * sprite; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->unknown.next_in_quadrant)
    {
        sprite = get_sprite(sprite_id);

        if ((sprite->unknown.linked_list_type_offset != SPRITE_LIST_PEEP * 2) || (sprite->peep.state != PEEP_STATE_SITTING) ||
            (peep->z != sprite->peep.z))
        {
            continue;
        }

        return;
    }

    rct_peep * inner_peep;
    uint16     sprite_index;

    FOR_ALL_STAFF(sprite_index, inner_peep)
    {
        if (inner_peep->staff_type != STAFF_TYPE_SECURITY)
            continue;

        if (inner_peep->x == LOCATION_NULL)
            continue;

        sint32 x_diff = abs(inner_peep->x - peep->x);
        sint32 y_diff = abs(inner_peep->y - peep->y);

        if (Math::Max(x_diff, y_diff) < 224)
            return;
    }

    tile_element->flags |= TILE_ELEMENT_FLAG_BROKEN;

    map_invalidate_tile_zoom1(peep->next_x, peep->next_y, (tile_element->base_height << 3) + 32, tile_element->base_height << 3);

    peep->angriness = 16;
}

/**
 * rct2: 0x0069101A
 *
 * @return (CF)
 */
static bool peep_should_watch_ride(rct_tile_element * tileElement)
{
    Ride * ride = get_ride(track_element_get_ride_index(tileElement));

    // Ghosts are purely this-client-side and should not cause any interaction,
    // as that may lead to a desync.
    if (network_get_mode() != NETWORK_MODE_NONE)
    {
        if (tile_element_is_ghost(tileElement))
            return false;
    }

    if (gRideClassifications[ride->type] != RIDE_CLASS_RIDE)
    {
        return false;
    }

    // This is most likely to have peeps watch new rides
    if (ride->excitement == RIDE_RATING_UNDEFINED)
    {
        return true;
    }

    if (ride->excitement >= RIDE_RATING(4, 70))
    {
        return true;
    }

    if (ride->intensity >= RIDE_RATING(4, 50))
    {
        return true;
    }

    if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_INTERESTING_TO_LOOK_AT)
    {
        if ((scenario_rand() & 0xFFFF) > 0x3333)
        {
            return false;
        }
    }
    else if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_SLIGHTLY_INTERESTING_TO_LOOK_AT)
    {
        if ((scenario_rand() & 0xFFFF) > 0x1000)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool loc_690FD0(rct_peep * peep, uint8 * rideToView, uint8 * rideSeatToView, rct_tile_element * tileElement)
{
    Ride * ride = get_ride(track_element_get_ride_index(tileElement));

    *rideToView = track_element_get_ride_index(tileElement);
    if (ride->excitement == RIDE_RATING_UNDEFINED)
    {
        *rideSeatToView = 1;
        if (ride->status != RIDE_STATUS_OPEN)
        {
            if (tileElement->clearance_height > peep->next_z + 8)
            {
                *rideSeatToView |= (1 << 1);
            }

            return true;
        }
    }
    else
    {
        *rideSeatToView = 0;
        if (ride->status == RIDE_STATUS_OPEN && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
        {
            if (tileElement->clearance_height > peep->next_z + 8)
            {
                *rideSeatToView = 0x02;
            }

            return true;
        }
    }

    return false;
}

/**
 *
 *  rct2: 0x00690B99
 *
 * @param edge (eax)
 * @param peep (esi)
 * @param[out] rideToView (cl)
 * @param[out] rideSeatToView (ch)
 * @return !CF
 */
static bool peep_find_ride_to_look_at(rct_peep * peep, uint8 edge, uint8 * rideToView, uint8 * rideSeatToView)
{
    rct_tile_element *tileElement, *surfaceElement;

    surfaceElement = map_get_surface_element_at({peep->next_x, peep->next_y});

    tileElement = surfaceElement;
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tile_element_is_ghost(tileElement))
                continue;
        }
        if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_WALL)
            continue;
        if (tile_element_get_direction(tileElement) != edge)
            continue;
        auto wallEntry = get_wall_entry(tileElement->properties.wall.type);
        if (wallEntry == nullptr || (wallEntry->wall.flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        if (peep->next_z + 4 <= tileElement->base_height)
            continue;
        if (peep->next_z + 1 >= tileElement->clearance_height)
            continue;

        return false;
    } while (!tile_element_is_last_for_tile(tileElement++));

    uint16 x = peep->next_x + TileDirectionDelta[edge].x;
    uint16 y = peep->next_y + TileDirectionDelta[edge].y;
    if (x > 255 * 32 || y > 255 * 32)
    {
        return false;
    }

    surfaceElement = map_get_surface_element_at({x, y});

    tileElement = surfaceElement;
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tile_element_is_ghost(tileElement))
                continue;
        }
        if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_WALL)
            continue;
        if (tile_element_get_direction_with_offset(tileElement, 2) != edge)
            continue;
        auto wallEntry = get_wall_entry(tileElement->properties.wall.type);
        if (wallEntry == nullptr || (wallEntry->wall.flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        // TODO: Check whether this shouldn't be <=, as the other loops use. If so, also extract as loop A.
        if (peep->next_z + 4 >= tileElement->base_height)
            continue;
        if (peep->next_z + 1 >= tileElement->clearance_height)
            continue;

        return false;
    } while (!tile_element_is_last_for_tile(tileElement++));

    // TODO: Extract loop B
    tileElement = surfaceElement;
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tile_element_is_ghost(tileElement))
                continue;
        }

        if (tileElement->clearance_height + 1 < peep->next_z)
            continue;
        if (peep->next_z + 6 < tileElement->base_height)
            continue;

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_TRACK)
        {
            if (peep_should_watch_ride(tileElement))
            {
                return loc_690FD0(peep, rideToView, rideSeatToView, tileElement);
            }
        }

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_LARGE_SCENERY)
        {
            if (!(get_large_scenery_entry(scenery_large_get_type(tileElement))->large_scenery.flags &
                  LARGE_SCENERY_FLAG_PHOTOGENIC))
            {
                continue;
            }

            *rideSeatToView = 0;
            if (tileElement->clearance_height >= peep->next_z + 8)
            {
                *rideSeatToView = 0x02;
            }

            *rideToView = 0xFF;

            return true;
        }
    } while (!tile_element_is_last_for_tile(tileElement++));

    // TODO: Extract loop C
    tileElement = surfaceElement;
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tile_element_is_ghost(tileElement))
                continue;
        }
        if (tileElement->clearance_height + 1 < peep->next_z)
            continue;
        if (peep->next_z + 6 < tileElement->base_height)
            continue;
        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_SURFACE)
            continue;
        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_PATH)
            continue;

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_WALL)
        {
            auto wallEntry = get_wall_entry(tileElement->properties.wall.type);
            if (wallEntry == nullptr || (wallEntry->wall.flags2 & WALL_SCENERY_2_IS_OPAQUE))
            {
                continue;
            }
        }

        return false;
    } while (!tile_element_is_last_for_tile(tileElement++));

    x += TileDirectionDelta[edge].x;
    y += TileDirectionDelta[edge].y;
    if (x > 255 * 32 || y > 255 * 32)
    {
        return false;
    }

    surfaceElement = map_get_surface_element_at({x, y});

    // TODO: extract loop A
    tileElement = surfaceElement;
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tile_element_is_ghost(tileElement))
                continue;
        }
        if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_WALL)
            continue;
        if (tile_element_get_direction_with_offset(tileElement, 2) != edge)
            continue;
        auto wallEntry = get_wall_entry(tileElement->properties.wall.type);
        if (wallEntry == nullptr || (wallEntry->wall.flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        if (peep->next_z + 6 <= tileElement->base_height)
            continue;
        if (peep->next_z >= tileElement->clearance_height)
            continue;

        return false;
    } while (!tile_element_is_last_for_tile(tileElement++));

    // TODO: Extract loop B
    tileElement = surfaceElement;
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tile_element_is_ghost(tileElement))
                continue;
        }
        if (tileElement->clearance_height + 1 < peep->next_z)
            continue;
        if (peep->next_z + 8 < tileElement->base_height)
            continue;

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_TRACK)
        {
            if (peep_should_watch_ride(tileElement))
            {
                return loc_690FD0(peep, rideToView, rideSeatToView, tileElement);
            }
        }

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_LARGE_SCENERY)
        {
            auto sceneryEntry = get_large_scenery_entry(scenery_large_get_type(tileElement));
            if (!(sceneryEntry == nullptr || sceneryEntry->large_scenery.flags &
                  LARGE_SCENERY_FLAG_PHOTOGENIC))
            {
                continue;
            }

            *rideSeatToView = 0;
            if (tileElement->clearance_height >= peep->next_z + 8)
            {
                *rideSeatToView = 0x02;
            }

            *rideToView = 0xFF;

            return true;
        }
    } while (!tile_element_is_last_for_tile(tileElement++));

    // TODO: Extract loop C
    tileElement = surfaceElement;
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tile_element_is_ghost(tileElement))
                continue;
        }
        if (tileElement->clearance_height + 1 < peep->next_z)
            continue;
        if (peep->next_z + 8 < tileElement->base_height)
            continue;
        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_SURFACE)
            continue;
        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_PATH)
            continue;

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_WALL)
        {
            auto wallEntry = get_wall_entry(tileElement->properties.wall.type);
            if (wallEntry == nullptr || (wallEntry->wall.flags2 & WALL_SCENERY_2_IS_OPAQUE))
            {
                continue;
            }
        }

        return false;
    } while (!tile_element_is_last_for_tile(tileElement++));

    x += TileDirectionDelta[edge].x;
    y += TileDirectionDelta[edge].y;
    if (x > 255 * 32 || y > 255 * 32)
    {
        return false;
    }

    surfaceElement = map_get_surface_element_at({x, y});

    // TODO: extract loop A
    tileElement = surfaceElement;
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tile_element_is_ghost(tileElement))
                continue;
        }
        if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_WALL)
            continue;
        if (tile_element_get_direction_with_offset(tileElement, 2) != edge)
            continue;
        auto wallEntry = get_wall_entry(tileElement->properties.wall.type);
        if (wallEntry == nullptr || (wallEntry->wall.flags2 & WALL_SCENERY_2_IS_OPAQUE))
            continue;
        if (peep->next_z + 8 <= tileElement->base_height)
            continue;
        if (peep->next_z >= tileElement->clearance_height)
            continue;

        return false;
    } while (!tile_element_is_last_for_tile(tileElement++));

    // TODO: Extract loop B
    tileElement = surfaceElement;
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            if (tile_element_is_ghost(tileElement))
                continue;
        }
        if (tileElement->clearance_height + 1 < peep->next_z)
            continue;
        if (peep->next_z + 10 < tileElement->base_height)
            continue;

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_TRACK)
        {
            if (peep_should_watch_ride(tileElement))
            {
                return loc_690FD0(peep, rideToView, rideSeatToView, tileElement);
            }
        }

        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_LARGE_SCENERY)
        {
            if (!(get_large_scenery_entry(scenery_large_get_type(tileElement))->large_scenery.flags &
                  LARGE_SCENERY_FLAG_PHOTOGENIC))
            {
                continue;
            }

            *rideSeatToView = 0;
            if (tileElement->clearance_height >= peep->next_z + 8)
            {
                *rideSeatToView = 0x02;
            }

            *rideToView = 0xFF;

            return true;
        }
    } while (!tile_element_is_last_for_tile(tileElement++));

    return false;
}
