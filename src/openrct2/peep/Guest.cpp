/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Footpath.h"
#include "../world/LargeScenery.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
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
static constexpr const LocationXY16 _WatchingPositionOffsets[] = {
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

static constexpr const ride_rating NauseaMaximumThresholds[] = {
    300, 600, 800, 1000
};

/** rct2: 0x0097EFCC */
static constexpr const uint8_t item_standard_litter[32] = {
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
static constexpr const uint8_t item_extra_litter[32] = {
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

/** rct2: 0x009822F4, 0x00982310 */
static constexpr const uint8_t item_consumption_time[] = {
    0,      // SHOP_ITEM_BALLOON
    0,      // SHOP_ITEM_TOY
    0,      // SHOP_ITEM_MAP
    0,      // SHOP_ITEM_PHOTO
    0,      // SHOP_ITEM_UMBRELLA
    100,    // SHOP_ITEM_DRINK
    150,    // SHOP_ITEM_BURGER
    120,    // SHOP_ITEM_CHIPS
    60,     // SHOP_ITEM_ICE_CREAM
    50,     // SHOP_ITEM_CANDYFLOSS
    0,      // SHOP_ITEM_EMPTY_CAN
    0,      // SHOP_ITEM_RUBBISH
    0,      // SHOP_ITEM_EMPTY_BURGER_BOX
    150,    // SHOP_ITEM_PIZZA
    0,      // SHOP_ITEM_VOUCHER
    75,     // SHOP_ITEM_POPCORN
    133,    // SHOP_ITEM_HOT_DOG
    110,    // SHOP_ITEM_TENTACLE
    0,      // SHOP_ITEM_HAT
    50,     // SHOP_ITEM_TOFFEE_APPLE
    0,      // SHOP_ITEM_TSHIRT
    80,     // SHOP_ITEM_DOUGHNUT
    90,     // SHOP_ITEM_COFFEE
    0,      // SHOP_ITEM_EMPTY_CUP
    170,    // SHOP_ITEM_CHICKEN
    115,    // SHOP_ITEM_LEMONADE
    0,      // SHOP_ITEM_EMPTY_BOX
    0,      // SHOP_ITEM_EMPTY_BOTTLE
    0xFF,   // UNUSED
    0xFF,   // UNUSED
    0xFF,   // UNUSED
    0xFF,   // UNUSED
    0,      // SHOP_ITEM_PHOTO2
    0,      // SHOP_ITEM_PHOTO3
    0,      // SHOP_ITEM_PHOTO4
    70,     // SHOP_ITEM_PRETZEL
    85,     // SHOP_ITEM_CHOCOLATE
    95,     // SHOP_ITEM_ICED_TEA
    90,     // SHOP_ITEM_FUNNEL_CAKE
    0,      // SHOP_ITEM_SUNGLASSES
    130,    // SHOP_ITEM_BEEF_NOODLES
    120,    // SHOP_ITEM_FRIED_RICE_NOODLES
    100,    // SHOP_ITEM_WONTON_SOUP
    110,    // SHOP_ITEM_MEATBALL_SOUP
    110,    // SHOP_ITEM_FRUIT_JUICE
    90,     // SHOP_ITEM_SOYBEAN_MILK
    100,    // SHOP_ITEM_SU_JONGKWA
    130,    // SHOP_ITEM_SUB_SANDWICH
    75,     // SHOP_ITEM_COOKIE
    0,      // SHOP_ITEM_EMPTY_BOWL_RED
    0,      // SHOP_ITEM_EMPTY_DRINK_CARTON
    0,      // SHOP_ITEM_EMPTY_JUICE_CUP
    115,    // SHOP_ITEM_ROAST_SAUSAGE
    0       // SHOP_ITEM_EMPTY_BOWL_BLUE
};

/** rct2: 009823AC */
static constexpr const uint8_t crowded_thoughts[] = {
    PEEP_THOUGHT_TYPE_LOST,
    PEEP_THOUGHT_TYPE_TIRED,
    PEEP_THOUGHT_TYPE_BAD_LITTER,
    PEEP_THOUGHT_TYPE_HUNGRY,
    PEEP_THOUGHT_TYPE_THIRSTY,
    PEEP_THOUGHT_TYPE_VERY_CLEAN,
    PEEP_THOUGHT_TYPE_CROWDED,
    PEEP_THOUGHT_TYPE_SCENERY,
    PEEP_THOUGHT_TYPE_VERY_CLEAN,
    PEEP_THOUGHT_TYPE_MUSIC,
    PEEP_THOUGHT_TYPE_WATCHED,
    PEEP_THOUGHT_TYPE_NOT_HUNGRY,
    PEEP_THOUGHT_TYPE_NOT_THIRSTY,
    PEEP_THOUGHT_TYPE_BATHROOM,
    PEEP_THOUGHT_TYPE_NONE,
    PEEP_THOUGHT_TYPE_NONE,
};

/** rct2: 0x00982326 */
static constexpr const uint8_t peep_item_containers[] = {
    0xFF,                           // PEEP_ITEM_BALLOON
    0xFF,                           // PEEP_ITEM_TOY
    0xFF,                           // PEEP_ITEM_MAP
    0xFF,                           // PEEP_ITEM_PHOTO
    0xFF,                           // PEEP_ITEM_UMBRELLA
    SHOP_ITEM_EMPTY_CAN,            // PEEP_ITEM_DRINK
    SHOP_ITEM_EMPTY_BURGER_BOX,     // PEEP_ITEM_BURGER
    SHOP_ITEM_RUBBISH,              // PEEP_ITEM_CHIPS
    0xFF,                           // PEEP_ITEM_ICE_CREAM
    0xFF,                           // PEEP_ITEM_CANDYFLOSS
    0xFF,                           // PEEP_ITEM_EMPTY_CAN
    0xFF,                           // PEEP_ITEM_RUBBISH
    0xFF,                           // PEEP_ITEM_EMPTY_BURGER_BOX
    SHOP_ITEM_RUBBISH,              // PEEP_ITEM_PIZZA
    0xFF,                           // PEEP_ITEM_VOUCHER
    SHOP_ITEM_RUBBISH,              // PEEP_ITEM_POPCORN
    0xFF,                           // PEEP_ITEM_HOT_DOG
    0xFF,                           // PEEP_ITEM_TENTACLE
    0xFF,                           // PEEP_ITEM_HAT
    0xFF,                           // PEEP_ITEM_TOFFEE_APPLE
    0xFF,                           // PEEP_ITEM_TSHIRT
    0xFF,                           // PEEP_ITEM_DOUGHNUT
    SHOP_ITEM_EMPTY_CUP,            // PEEP_ITEM_COFFEE
    0xFF,                           // PEEP_ITEM_EMPTY_CUP
    SHOP_ITEM_EMPTY_BOX,            // PEEP_ITEM_CHICKEN
    SHOP_ITEM_EMPTY_BOTTLE,         // PEEP_ITEM_LEMONADE
    0xFF,                           // PEEP_ITEM_EMPTY_BOX
    0xFF,                           // PEEP_ITEM_EMPTY_BOTTLE
};

/** rct2: 0x00982342 */
static constexpr const uint8_t peep_extra_item_containers[] = {
    0xFF,                           // PEEP_ITEM_PHOTO2
    0xFF,                           // PEEP_ITEM_PHOTO3
    0xFF,                           // PEEP_ITEM_PHOTO4
    0xFF,                           // PEEP_ITEM_PRETZEL
    SHOP_ITEM_EMPTY_CUP,            // PEEP_ITEM_CHOCOLATE
    SHOP_ITEM_EMPTY_CUP,            // PEEP_ITEM_ICED_TEA
    0xFF,                           // PEEP_ITEM_FUNNEL_CAKE
    0xFF,                           // PEEP_ITEM_SUNGLASSES
    SHOP_ITEM_EMPTY_BOWL_BLUE,      // PEEP_ITEM_BEEF_NOODLES
    SHOP_ITEM_EMPTY_BOWL_BLUE,      // PEEP_ITEM_FRIED_RICE_NOODLES
    SHOP_ITEM_EMPTY_BOWL_RED,       // PEEP_ITEM_WONTON_SOUP
    SHOP_ITEM_EMPTY_BOWL_RED,       // PEEP_ITEM_MEATBALL_SOUP
    SHOP_ITEM_EMPTY_JUICE_CUP,      // PEEP_ITEM_FRUIT_JUICE
    SHOP_ITEM_EMPTY_DRINK_CARTON,   // PEEP_ITEM_SOYBEAN_MILK
    SHOP_ITEM_EMPTY_DRINK_CARTON,   // PEEP_ITEM_SU_JONGKWA
    0xFF,                           // PEEP_ITEM_SUB_SANDWICH
    0xFF,                           // PEEP_ITEM_COOKIE
    0xFF,                           // PEEP_ITEM_EMPTY_BOWL_RED
    0xFF,                           // PEEP_ITEM_EMPTY_DRINK_CARTON
    0xFF,                           // PEEP_ITEM_EMPTY_JUICE_CUP
    0xFF,                           // PEEP_ITEM_ROAST_SAUSAGE
    0xFF,                           // PEEP_ITEM_EMPTY_BOWL_BLUE
};

// These arrays contain the base minimum and maximum nausea ratings for peeps, based on their nausea tolerance level.
static constexpr const ride_rating NauseaMinimumThresholds[] = {
    0, 0, 200, 400
};
// clang-format on

static bool   peep_has_voucher_for_free_ride(rct_peep * peep, int32_t rideIndex);
static void   peep_ride_is_too_intense(rct_peep * peep, int32_t rideIndex, bool peepAtRide);
static void   peep_reset_ride_heading(rct_peep * peep);
static void   peep_tried_to_enter_full_queue(rct_peep * peep, int32_t rideIndex);
static int16_t peep_calculate_ride_satisfaction(rct_peep * peep, Ride * ride);
static void   peep_update_favourite_ride(rct_peep * peep, Ride * ride);
static int16_t peep_calculate_ride_value_satisfaction(rct_peep * peep, Ride * ride);
static int16_t peep_calculate_ride_intensity_nausea_satisfaction(rct_peep * peep, Ride * ride);
static void   peep_update_ride_nausea_growth(rct_peep * peep, Ride * ride);
static bool   peep_should_go_on_ride_again(rct_peep * peep, Ride * ride);
static bool   peep_should_preferred_intensity_increase(rct_peep * peep);
static bool   peep_really_liked_ride(rct_peep * peep, Ride * ride);
static uint8_t  peep_assess_surroundings(int16_t centre_x, int16_t centre_y, int16_t centre_z);
static void   peep_update_hunger(rct_peep * peep);
static void   peep_decide_whether_to_leave_park(rct_peep * peep);
static void   peep_leave_park(rct_peep * peep);
static void   peep_head_for_nearest_ride_type(rct_peep * peep, int32_t rideType);
static void   peep_head_for_nearest_ride_with_flags(rct_peep * peep, int32_t rideTypeFlags);
bool loc_690FD0(rct_peep * peep, uint8_t * rideToView, uint8_t * rideSeatToView, rct_tile_element * tileElement);

void rct_peep::Tick128UpdateGuest(int32_t index)
{
    if ((uint32_t)(index & 0x1FF) == (gCurrentTicks & 0x1FF))
    {
        /* Effect of masking with 0x1FF here vs mask 0x7F,
         * which is the condition for calling this function, is
         * to reduce how often the content in this conditional
         * is executed to once every four calls. */
        if (peep_flags & PEEP_FLAGS_CROWDED)
        {
            uint8_t thought_type = crowded_thoughts[scenario_rand() & 0xF];
            if (thought_type != PEEP_THOUGHT_TYPE_NONE)
            {
                peep_insert_new_thought(this, thought_type, PEEP_THOUGHT_ITEM_NONE);
            }
        }

        if (peep_flags & PEEP_FLAGS_EXPLODE && x != LOCATION_NULL)
        {
            if (state == PEEP_STATE_WALKING || state == PEEP_STATE_SITTING)
            {
                audio_play_sound_at_location(SOUND_CRASH, x, y, z);

                sprite_misc_explosion_cloud_create(x, y, z + 16);
                sprite_misc_explosion_flare_create(x, y, z + 16);

                Remove();
                return;
            }
            else
            {
                peep_flags &= ~PEEP_FLAGS_EXPLODE;
            }
        }

        if (peep_flags & PEEP_FLAGS_HUNGER)
        {
            if (hunger >= 15)
                hunger -= 15;
        }

        if (peep_flags & PEEP_FLAGS_BATHROOM)
        {
            if (toilet <= 180)
                toilet += 50;
        }

        if (peep_flags & PEEP_FLAGS_HAPPINESS)
        {
            happiness_target = 5;
        }

        if (peep_flags & PEEP_FLAGS_NAUSEA)
        {
            nausea_target = 200;
            if (nausea <= 130)
                nausea = 130;
        }

        if (angriness != 0)
            angriness--;

        if (state == PEEP_STATE_WALKING || state == PEEP_STATE_SITTING)
        {
            surroundings_thought_timeout++;
            if (surroundings_thought_timeout >= 18)
            {
                surroundings_thought_timeout = 0;
                if (x != LOCATION_NULL)
                {

                    uint8_t thought_type = peep_assess_surroundings(x & 0xFFE0, y & 0xFFE0, z);

                    if (thought_type != PEEP_THOUGHT_TYPE_NONE)
                    {
                        peep_insert_new_thought(this, thought_type, PEEP_THOUGHT_ITEM_NONE);
                        happiness_target = std::min(PEEP_MAX_HAPPINESS, happiness_target + 45);
                    }
                }
            }
        }

        UpdateSpriteType();

        if (state == PEEP_STATE_ON_RIDE || state == PEEP_STATE_ENTERING_RIDE)
        {
            time_on_ride = std::min(255, time_on_ride + 1);

            if (peep_flags & PEEP_FLAGS_WOW)
            {
                peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_WOW2, PEEP_THOUGHT_ITEM_NONE);
            }

            if (time_on_ride > 15)
            {
                happiness_target = std::max(0, happiness_target - 5);

                if (time_on_ride > 22)
                {
                    Ride * ride = get_ride(current_ride);

                    uint8_t thought_type = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE) ? PEEP_THOUGHT_TYPE_GET_OUT
                                                                                                : PEEP_THOUGHT_TYPE_GET_OFF;

                    peep_insert_new_thought(this, thought_type, current_ride);
                }
            }
        }

        if (state == PEEP_STATE_WALKING && outside_of_park == 0 && !(peep_flags & PEEP_FLAGS_LEAVING_PARK) &&
            no_of_rides == 0 && guest_heading_to_ride_id == 0xFF)
        {

            uint32_t time_duration = gScenarioTicks - time_in_park;
            time_duration /= 2048;

            if (time_duration >= 5)
            {
                PickRideToGoOn();

                if (guest_heading_to_ride_id == 0xFF)
                {
                    happiness_target = std::max(happiness_target - 128, 0);
                    peep_leave_park(this);
                    peep_update_hunger(this);
                    goto loc_68F9F3;
                }
            }
        }

        if ((scenario_rand() & 0xFFFF) <= ((item_standard_flags & PEEP_ITEM_MAP) ? 8192U : 2184U))
        {
            PickRideToGoOn();
        }

        if ((uint32_t)(index & 0x3FF) == (gCurrentTicks & 0x3FF))
        {
            /* Effect of masking with 0x3FF here vs mask 0x1FF,
             * which is used in the encompassing conditional, is
             * to reduce how often the content in this conditional
             * is executed to once every second time the encompassing
             * conditional executes. */

            if (outside_of_park == 0 && (state == PEEP_STATE_WALKING || state == PEEP_STATE_SITTING))
            {

                uint8_t num_thoughts         = 0;
                uint8_t possible_thoughts[5] = { 0 };

                if (peep_flags & PEEP_FLAGS_LEAVING_PARK)
                {
                    possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_GO_HOME;
                }
                else
                {
                    if (energy <= 70 && happiness < 128)
                    {
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_TIRED;
                    }

                    if (hunger <= 10 && !HasFood())
                    {
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_HUNGRY;
                    }

                    if (thirst <= 25 && !HasFood())
                    {
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_THIRSTY;
                    }

                    if (toilet >= 160)
                    {
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_BATHROOM;
                    }

                    if (!(gParkFlags & PARK_FLAGS_NO_MONEY) && cash_in_pocket <= MONEY(9, 00) && happiness >= 105 &&
                        energy >= 70)
                    {
                        /* The energy check was originally a second check on happiness.
                         * This was superfluous so should probably check something else.
                         * Guessed that this should really be checking energy, since
                         * the addresses for happiness and energy are quite close,
                         * 70 is also the threshold for tired thoughts (see above) and
                         * it makes sense that a tired peep might not think about getting
                         * more money. */
                        possible_thoughts[num_thoughts++] = PEEP_THOUGHT_TYPE_RUNNING_OUT;
                    }
                }

                if (num_thoughts != 0)
                {
                    uint8_t chosen_thought = possible_thoughts[scenario_rand() % num_thoughts];

                    peep_insert_new_thought(this, chosen_thought, PEEP_THOUGHT_ITEM_NONE);

                    switch (chosen_thought)
                    {
                    case PEEP_THOUGHT_TYPE_HUNGRY:
                        peep_head_for_nearest_ride_with_flags(this, RIDE_TYPE_FLAG_SELLS_FOOD);
                        break;
                    case PEEP_THOUGHT_TYPE_THIRSTY:
                        peep_head_for_nearest_ride_with_flags(this, RIDE_TYPE_FLAG_SELLS_DRINKS);
                        break;
                    case PEEP_THOUGHT_TYPE_BATHROOM:
                        peep_head_for_nearest_ride_with_flags(this, RIDE_TYPE_FLAG_IS_BATHROOM);
                        break;
                    case PEEP_THOUGHT_TYPE_RUNNING_OUT:
                        peep_head_for_nearest_ride_type(this, RIDE_TYPE_CASH_MACHINE);
                        break;
                    }
                }
            }
        }
        else
        {
            /* This branch of the conditional is executed on the
             * remaining times the encompassing conditional is
             * executed (which is also every second time, but
             * the alternate time to the true branch). */
            if (nausea >= 140)
            {
                uint8_t thought_type = PEEP_THOUGHT_TYPE_SICK;
                if (nausea >= 200)
                {
                    thought_type = PEEP_THOUGHT_TYPE_VERY_SICK;
                    peep_head_for_nearest_ride_type(this, RIDE_TYPE_FIRST_AID);
                }
                peep_insert_new_thought(this, thought_type, PEEP_THOUGHT_ITEM_NONE);
            }
        }

        switch (state)
        {
        case PEEP_STATE_WALKING:
        case PEEP_STATE_LEAVING_PARK:
        case PEEP_STATE_ENTERING_PARK:
            peep_decide_whether_to_leave_park(this);
            peep_update_hunger(this);
            break;

        case PEEP_STATE_SITTING:
            if (energy_target <= 135)
                energy_target += 5;

            if (thirst >= 5)
            {
                thirst -= 4;
                toilet = std::min(255, toilet + 3);
            }

            if (nausea_target >= 50)
                nausea_target -= 6;

            // In the original this branched differently
            // but it would mean setting the peep happiness from
            // a thought type entry which i think is incorrect.
            peep_update_hunger(this);
            break;

        case PEEP_STATE_QUEUING:
            if (time_in_queue >= 2000)
            {
                /* Peep happiness is affected once the peep has been waiting
                 * too long in a queue. */
                rct_tile_element * tileElement = map_get_first_element_at(next_x / 32, next_y / 32);
                bool               found       = false;
                do
                {
                    if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
                        continue;
                    if (tileElement->base_height != next_z)
                        continue;

                    // Check if the footpath has a queue line TV monitor on it
                    if (footpath_element_has_path_scenery(tileElement) && !footpath_element_path_scenery_is_ghost(tileElement))
                    {
                        uint8_t               pathSceneryIndex = footpath_element_get_path_scenery_index(tileElement);
                        rct_scenery_entry * sceneryEntry     = get_footpath_item_entry(pathSceneryIndex);
                        if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_QUEUE_SCREEN)
                        {
                            found = true;
                        }
                    }
                    break;
                } while (!(tileElement++)->IsLastForTile());

                if (found)
                {
                    /* Queue line TV monitors make the peeps waiting in the queue
                     * slowly happier, up to a certain level. */
                    /* Why don't queue line TV monitors start affecting the peeps
                     * as soon as they join the queue?? */
                    if (happiness_target < 90)
                        happiness_target = 90;

                    if (happiness_target < 165)
                        happiness_target += 2;
                }
                else
                {
                    /* Without a queue line TV monitor peeps waiting too long
                     * in a queue get less happy. */
                    happiness_target = std::max(happiness_target - 4, 0);
                }
            }
            peep_update_hunger(this);
            break;
        case PEEP_STATE_ENTERING_RIDE:
            if (sub_state == 17 || sub_state == 15)
            {
                peep_decide_whether_to_leave_park(this);
            }
            peep_update_hunger(this);
            break;
        }

    loc_68F9F3:
        // Idle peep happiness tends towards 127 (50%).
        if (happiness_target >= 128)
            happiness_target--;
        else
            happiness_target++;

        nausea_target = std::max(nausea_target - 2, 0);

        if (energy <= 50)
        {
            energy = std::max(energy - 2, 0);
        }

        if (hunger < 10)
        {
            hunger = std::max(hunger - 1, 0);
        }

        if (thirst < 10)
        {
            thirst = std::max(thirst - 1, 0);
        }

        if (toilet >= 195)
        {
            toilet--;
        }

        if (state == PEEP_STATE_WALKING && nausea_target >= 128)
        {

            if ((scenario_rand() & 0xFF) <= (uint8_t)((nausea - 128) / 2))
            {
                if (action >= PEEP_ACTION_NONE_1)
                {
                    action                     = PEEP_ACTION_THROW_UP;
                    action_frame               = 0;
                    action_sprite_image_offset = 0;
                    UpdateCurrentActionSpriteType();
                    invalidate_sprite_2((rct_sprite *)this);
                }
            }
        }
    }

    // Remaining content is executed every call.

    // 68FA89
    if (time_to_consume == 0 && HasFood())
    {
        time_to_consume += 3;
    }

    if (time_to_consume != 0 && state != PEEP_STATE_ON_RIDE)
    {

        time_to_consume = std::max(time_to_consume - 3, 0);

        if (HasDrink())
        {
            thirst = std::min(thirst + 7, 255);
        }
        else
        {
            hunger = std::min(hunger + 7, 255);
            thirst = std::max(thirst - 3, 0);
            toilet = std::min(toilet + 2, 255);
        }

        if (time_to_consume == 0)
        {
            int32_t chosen_food = bitscanforward(HasFoodStandardFlag());
            if (chosen_food != -1)
            {
                item_standard_flags &= ~(1 << chosen_food);

                uint8_t discard_container = peep_item_containers[chosen_food];
                if (discard_container != 0xFF)
                {
                    item_standard_flags |= (1 << discard_container);
                }

                window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                UpdateSpriteType();
            }
            else
            {
                chosen_food = bitscanforward(HasFoodExtraFlag());
                if (chosen_food != -1)
                {
                    item_extra_flags &= ~(1 << chosen_food);
                    uint8_t discard_container = peep_extra_item_containers[chosen_food];
                    if (discard_container != 0xFF)
                    {
                        if (discard_container >= 32)
                            item_extra_flags |= (1 << (discard_container - 32));
                        else
                            item_standard_flags |= (1 << discard_container);
                    }

                    window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                    UpdateSpriteType();
                }
            }
        }
    }

    uint8_t newEnergy       = energy;
    uint8_t newTargetEnergy = energy_target;
    if (newEnergy >= newTargetEnergy)
    {
        newEnergy -= 2;
        if (newEnergy < newTargetEnergy)
            newEnergy = newTargetEnergy;
    }
    else
    {
        newEnergy = std::min(PEEP_MAX_ENERGY_TARGET, newEnergy + 4);
        if (newEnergy > newTargetEnergy)
            newEnergy = newTargetEnergy;
    }

    if (newEnergy < PEEP_MIN_ENERGY)
        newEnergy = PEEP_MIN_ENERGY;

    /* Previous code here suggested maximum energy is 128. */
    newEnergy = std::min(static_cast<uint8_t>(PEEP_MAX_ENERGY), newEnergy);

    if (newEnergy != energy)
    {
        energy = newEnergy;
        window_invalidate_flags |= PEEP_INVALIDATE_PEEP_2;
    }

    uint8_t newHappiness       = happiness;
    uint8_t newHappinessGrowth = happiness_target;
    if (newHappiness >= newHappinessGrowth)
    {
        newHappiness = std::max(newHappiness - 4, 0);
        if (newHappiness < newHappinessGrowth)
            newHappiness = newHappinessGrowth;
    }
    else
    {
        newHappiness = std::min(255, newHappiness + 4);
        if (newHappiness > newHappinessGrowth)
            newHappiness = newHappinessGrowth;
    }

    if (newHappiness != happiness)
    {
        happiness = newHappiness;
        window_invalidate_flags |= PEEP_INVALIDATE_PEEP_2;
    }

    uint8_t newNausea       = nausea;
    uint8_t newNauseaGrowth = nausea_target;
    if (newNausea >= newNauseaGrowth)
    {
        newNausea = std::max(newNausea - 4, 0);
        if (newNausea < newNauseaGrowth)
            newNausea = newNauseaGrowth;
    }
    else
    {
        newNausea = std::min(255, newNausea + 4);
        if (newNausea > newNauseaGrowth)
            newNausea = newNauseaGrowth;
    }

    if (newNausea != nausea)
    {
        nausea = newNausea;
        window_invalidate_flags |= PEEP_INVALIDATE_PEEP_2;
    }
}

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

        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        LocationXYZ16 loc = 
        {
            (int16_t)((x & 0xFFE0) + BenchUseOffsets[var_37 & 0x7].x),
            (int16_t)((y & 0xFFE0) + BenchUseOffsets[var_37 & 0x7].y),
            z
        };

        Invalidate();
        MoveTo(loc.x, loc.y, loc.z);

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

        int32_t rand = scenario_rand();
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

    uint8_t cur_station = current_ride_station;
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

    uint16_t spriteId = ride->last_peep_in_queue[cur_station];
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

bool rct_peep::HasItem(int32_t peepItem) const
{
    if (peepItem < 32)
    {
        return item_standard_flags & (1u << peepItem);
    }
    else
    {
        return item_extra_flags & (1u << (peepItem - 32));
    }
}

int32_t rct_peep::HasFoodStandardFlag() const
{
    return item_standard_flags &
           (PEEP_ITEM_DRINK | PEEP_ITEM_BURGER | PEEP_ITEM_CHIPS | PEEP_ITEM_ICE_CREAM | PEEP_ITEM_CANDYFLOSS |
            PEEP_ITEM_PIZZA | PEEP_ITEM_POPCORN | PEEP_ITEM_HOT_DOG | PEEP_ITEM_TENTACLE | PEEP_ITEM_TOFFEE_APPLE |
            PEEP_ITEM_DOUGHNUT | PEEP_ITEM_COFFEE | PEEP_ITEM_CHICKEN | PEEP_ITEM_LEMONADE);
}

int32_t rct_peep::HasFoodExtraFlag() const
{
    return item_extra_flags & (PEEP_ITEM_PRETZEL | PEEP_ITEM_CHOCOLATE | PEEP_ITEM_ICED_TEA | PEEP_ITEM_FUNNEL_CAKE |
                               PEEP_ITEM_BEEF_NOODLES | PEEP_ITEM_FRIED_RICE_NOODLES | PEEP_ITEM_WONTON_SOUP |
                               PEEP_ITEM_MEATBALL_SOUP | PEEP_ITEM_FRUIT_JUICE | PEEP_ITEM_SOYBEAN_MILK | PEEP_ITEM_SU_JONGKWA |
                               PEEP_ITEM_SUB_SANDWICH | PEEP_ITEM_COOKIE | PEEP_ITEM_ROAST_SAUSAGE);
}

bool rct_peep::HasDrinkStandardFlag() const
{
    return item_standard_flags & (PEEP_ITEM_DRINK | PEEP_ITEM_COFFEE | PEEP_ITEM_LEMONADE);
}

bool rct_peep::HasDrinkExtraFlag() const
{
    return item_extra_flags &
           (PEEP_ITEM_CHOCOLATE | PEEP_ITEM_ICED_TEA | PEEP_ITEM_FRUIT_JUICE | PEEP_ITEM_SOYBEAN_MILK | PEEP_ITEM_SU_JONGKWA);
}

/**
 * To simplify check of NOT(0x12BA3C0 and 0x118F48)
 * returns 0 on no food.
 */
bool rct_peep::HasDrink() const
{
    return HasDrinkStandardFlag() || HasDrinkExtraFlag();
}

int32_t rct_peep::HasEmptyContainerStandardFlag() const
{
    return item_standard_flags & (PEEP_ITEM_EMPTY_CAN | PEEP_ITEM_EMPTY_BURGER_BOX | PEEP_ITEM_EMPTY_CUP | PEEP_ITEM_RUBBISH |
                                  PEEP_ITEM_EMPTY_BOX | PEEP_ITEM_EMPTY_BOTTLE);
}

int32_t rct_peep::HasEmptyContainerExtraFlag() const
{
    return item_extra_flags &
           (PEEP_ITEM_EMPTY_BOWL_RED | PEEP_ITEM_EMPTY_DRINK_CARTON | PEEP_ITEM_EMPTY_JUICE_CUP | PEEP_ITEM_EMPTY_BOWL_BLUE);
}

bool rct_peep::HasEmptyContainer() const
{
    return HasEmptyContainerStandardFlag() || HasEmptyContainerExtraFlag();
}

/**
 *
 *  rct2: 0x69C308
 * Check if lost.
 */
void rct_peep::CheckIfLost()
{
    if (!(peep_flags & PEEP_FLAGS_LOST))
    {
        if (gRideCount < 2)
            return;
        peep_flags ^= PEEP_FLAGS_21;

        if (!(peep_flags & PEEP_FLAGS_21))
            return;

        time_lost++;
        if (time_lost != 254)
            return;
        time_lost = 230;
    }
    peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_LOST, PEEP_THOUGHT_ITEM_NONE);

    happiness_target = std::max(happiness_target - 30, 0);
}

/**
 *
 *  rct2: 0x69C26B
 * Check if cant find ride.
 */
void rct_peep::CheckCantFindRide()
{
    if (guest_heading_to_ride_id == 0xFF)
        return;

    // Peeps will think "I can't find ride X" twice before giving up completely.
    if (peep_is_lost_countdown == 30 || peep_is_lost_countdown == 60)
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_CANT_FIND, guest_heading_to_ride_id);
        happiness_target = std::max(happiness_target - 30, 0);
    }

    peep_is_lost_countdown--;
    if (peep_is_lost_countdown != 0)
        return;

    guest_heading_to_ride_id = 0xFF;
    rct_window * w           = window_find_by_number(WC_PEEP, sprite_index);

    if (w)
    {
        window_event_invalidate_call(w);
    }

    window_invalidate_by_number(WC_PEEP, sprite_index);
}

/**
 *
 *  rct2: 0x69C2D0
 * Check if cant find exit.
 */
void rct_peep::CheckCantFindExit()
{
    if (!(peep_flags & PEEP_FLAGS_LEAVING_PARK))
        return;

    // Peeps who can't find the park exit will continue to get less happy until they find it.
    if (peep_is_lost_countdown == 1)
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_CANT_FIND_EXIT, PEEP_THOUGHT_ITEM_NONE);
        happiness_target = std::max(happiness_target - 30, 0);
    }

    if (--peep_is_lost_countdown == 0)
        peep_is_lost_countdown = 90;
}

/** Main logic to decide whether a peep should buy an item in question
 *
 * Also handles the purchase as well, so once it returns, the peep will have the
 * item and the money will have been deducted.
 *
 * eax: shopItem | (rideIndex << 8)
 * ecx: price
 * esi: *peep
 *
 * Returns 0 or 1 depending on if the peep decided to buy the item
 *
 *  rct2: 0x0069AF1E
 */
bool rct_peep::DecideAndBuyItem(uint8_t rideIndex, int32_t shopItem, money32 price)
{
    Ride *  ride = get_ride(rideIndex);
    money32 itemValue;

    bool hasVoucher = false;

    if ((item_standard_flags & PEEP_ITEM_VOUCHER) && (voucher_type == VOUCHER_TYPE_FOOD_OR_DRINK_FREE) &&
        (voucher_arguments == shopItem))
    {
        hasVoucher = true;
    }

    if (HasItem(shopItem))
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_ALREADY_GOT, shopItem);
        return false;
    }

    if (shop_item_is_food_or_drink(shopItem))
    {
        int32_t food = -1;
        if ((food = HasFoodStandardFlag()) != 0)
        {
            peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_HAVENT_FINISHED, bitscanforward(food));
            return false;
        }
        else if ((food = HasFoodExtraFlag()) != 0)
        {
            peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_HAVENT_FINISHED, bitscanforward(food) + 32);
            return false;
        }
        else if (nausea >= 145)
            return false;
    }

    if ((shopItem == SHOP_ITEM_BALLOON) || (shopItem == SHOP_ITEM_ICE_CREAM) || (shopItem == SHOP_ITEM_CANDYFLOSS) ||
        (shopItem == SHOP_ITEM_SUNGLASSES))
    {
        if (climate_is_raining())
            return false;
    }

    if ((shopItem == SHOP_ITEM_SUNGLASSES) || (shopItem == SHOP_ITEM_ICE_CREAM))
    {
        if (gClimateCurrent.Temperature < 12)
            return false;
    }

    if (shop_item_is_food(shopItem) && (hunger > 75))
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_NOT_HUNGRY, PEEP_THOUGHT_ITEM_NONE);
        return false;
    }

    if (shop_item_is_drink(shopItem) && (thirst > 75))
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_NOT_THIRSTY, PEEP_THOUGHT_ITEM_NONE);
        return false;
    }

    if (shopItem == SHOP_ITEM_UMBRELLA && climate_is_raining())
        goto loc_69B119;

    if ((shopItem != SHOP_ITEM_MAP) && shop_item_is_souvenir(shopItem) && !hasVoucher)
    {
        if (((scenario_rand() & 0x7F) + 0x73) > happiness)
            return false;
        else if (no_of_rides < 3)
            return false;
    }

loc_69B119:
    if (!hasVoucher)
    {
        if (price != 0)
        {
            if (cash_in_pocket == 0)
            {
                peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_SPENT_MONEY, PEEP_THOUGHT_ITEM_NONE);
                return false;
            }
            if (price > cash_in_pocket)
            {
                peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_CANT_AFFORD, shopItem);
                return false;
            }
        }

        if (gClimateCurrent.Temperature >= 21)
            itemValue = get_shop_hot_value(shopItem);
        else if (gClimateCurrent.Temperature <= 11)
            itemValue = get_shop_cold_value(shopItem);
        else
            itemValue = get_shop_base_value(shopItem);

        if (itemValue < price)
        {
            itemValue -= price;
            if (shopItem == SHOP_ITEM_UMBRELLA)
            {
                if (climate_is_raining())
                    goto loc_69B221;
            }

            itemValue = -itemValue;
            if (happiness >= 128)
                itemValue /= 2;

            if (happiness >= 180)
                itemValue /= 2;

            if (itemValue > ((money16)(scenario_rand() & 0x07)))
            {
                // "I'm not paying that much for x"
                uint8_t thought_type =
                    (shopItem >= 32 ? (PEEP_THOUGHT_TYPE_PHOTO2_MUCH + (shopItem - 32))
                                    : (PEEP_THOUGHT_TYPE_BALLOON_MUCH + shopItem));
                peep_insert_new_thought(this, thought_type, rideIndex);
                return false;
            }
        }
        else
        {
            itemValue -= price;
            itemValue = std::max(8, itemValue);

            if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
            {
                if (itemValue >= (money32)(scenario_rand() & 0x07))
                {
                    // "This x is a really good value"
                    uint8_t thought_item =
                        (shopItem >= 32 ? (PEEP_THOUGHT_TYPE_PHOTO2 + (shopItem - 32))
                                        : (PEEP_THOUGHT_TYPE_BALLOON + shopItem));
                    peep_insert_new_thought(this, thought_item, rideIndex);
                }
            }

            int32_t happinessGrowth = itemValue * 4;
            happiness_target       = std::min((happiness_target + happinessGrowth), PEEP_MAX_HAPPINESS);
            happiness              = std::min((happiness + happinessGrowth), PEEP_MAX_HAPPINESS);
        }
    }

loc_69B221:
    if (!hasVoucher)
    {
        if (gClimateCurrent.Temperature >= 21)
            itemValue = get_shop_hot_value(shopItem);
        else if (gClimateCurrent.Temperature <= 11)
            itemValue = get_shop_cold_value(shopItem);
        else
            itemValue = get_shop_base_value(shopItem);

        itemValue -= price;
        uint8_t satisfaction = 0;
        if (itemValue > -8)
        {
            satisfaction++;
            if (itemValue > -3)
            {
                satisfaction++;
                if (itemValue > 3)
                    satisfaction++;
            }
        }

        ride_update_satisfaction(ride, satisfaction);
    }

    // The peep has now decided to buy the item (or, specifically, has not been
    // dissuaded so far).
    if (shopItem >= 32)
        item_extra_flags |= (1u << (shopItem - 32));
    else
        item_standard_flags |= (1u << shopItem);

    if (shopItem == SHOP_ITEM_TSHIRT)
        tshirt_colour = ride->track_colour_main[0];

    if (shopItem == SHOP_ITEM_HAT)
        hat_colour = ride->track_colour_main[0];

    if (shopItem == SHOP_ITEM_BALLOON)
        balloon_colour = ride->track_colour_main[0];

    if (shopItem == SHOP_ITEM_UMBRELLA)
        umbrella_colour = ride->track_colour_main[0];

    if (shopItem == SHOP_ITEM_MAP)
        peep_reset_pathfind_goal(this);

    uint16_t consumptionTime = item_consumption_time[shopItem];
    time_to_consume        = std::min((time_to_consume + consumptionTime), 255);

    if (shopItem == SHOP_ITEM_PHOTO)
        photo1_ride_ref = rideIndex;

    if (shopItem == SHOP_ITEM_PHOTO2)
        photo2_ride_ref = rideIndex;

    if (shopItem == SHOP_ITEM_PHOTO3)
        photo3_ride_ref = rideIndex;

    if (shopItem == SHOP_ITEM_PHOTO4)
        photo4_ride_ref = rideIndex;

    window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    UpdateSpriteType();
    if (peep_flags & PEEP_FLAGS_TRACKING)
    {
        set_format_arg(0, rct_string_id, name_string_idx);
        set_format_arg(2, uint32_t, id);
        set_format_arg(6, rct_string_id, ShopItemStringIds[shopItem].indefinite);
        if (gConfigNotifications.guest_bought_item)
        {
            news_item_add_to_queue(2, STR_PEEP_TRACKING_NOTIFICATION_BOUGHT_X, sprite_index);
        }
    }

    if (shop_item_is_food(shopItem))
        no_of_food++;

    if (shop_item_is_drink(shopItem))
        no_of_drinks++;

    if (shop_item_is_souvenir(shopItem))
        no_of_souvenirs++;

    money16 * expend_type   = &paid_on_souvenirs;
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_SHOP_STOCK;

    if (shop_item_is_food(shopItem))
    {
        expend_type             = &paid_on_food;
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_FOODDRINK_STOCK;
    }

    if (shop_item_is_drink(shopItem))
    {
        expend_type             = &paid_on_drink;
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_FOODDRINK_STOCK;
    }

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        finance_payment(get_shop_item_cost(shopItem), gCommandExpenditureType);

    // Sets the expenditure type to *_FOODDRINK_SALES or *_SHOP_SALES appropriately.
    gCommandExpenditureType--;
    if (hasVoucher)
    {
        item_standard_flags &= ~PEEP_ITEM_VOUCHER;
        window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    }
    else if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        SpendMoney(*expend_type, price);
    }
    ride->total_profit += (price - get_shop_item_cost(shopItem));
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
    ride->cur_num_customers++;
    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

    return true;
}

/**
 * Updates various peep stats upon entering a ride, as well as updating the
 * ride's satisfaction value.
 *  rct2: 0x0069545B
 */
void rct_peep::OnEnterRide(uint8_t rideIndex)
{
    Ride * ride = get_ride(rideIndex);

    // Calculate how satisfying the ride is for the peep. Can range from -140 to +105.
    int16_t satisfaction = peep_calculate_ride_satisfaction(this, ride);

    // Update the satisfaction stat of the ride.
    uint8_t rideSatisfaction = 0;
    if (satisfaction >= 40)
        rideSatisfaction = 3;
    else if (satisfaction >= 20)
        rideSatisfaction = 2;
    else if (satisfaction >= 0)
        rideSatisfaction = 1;

    ride_update_satisfaction(ride, rideSatisfaction);

    // Update various peep stats.
    if (no_of_rides < 255)
        no_of_rides++;

    SetHasRidden(current_ride);
    peep_update_favourite_ride(this, ride);
    happiness_target = Math::Clamp(0, happiness_target + satisfaction, PEEP_MAX_HAPPINESS);
    peep_update_ride_nausea_growth(this, ride);
}

/**
 *
 *  rct2: 0x0069576E
 */
void rct_peep::OnExitRide(uint8_t rideIndex)
{
    Ride * ride = get_ride(rideIndex);

    if (peep_flags & PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE)
    {
        peep_flags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        favourite_ride = rideIndex;
        // TODO fix this flag name or add another one
        window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
    }
    happiness = happiness_target;
    nausea    = nausea_target;
    window_invalidate_flags |= PEEP_INVALIDATE_PEEP_STATS;

    if (peep_flags & PEEP_FLAGS_LEAVING_PARK)
        peep_flags &= ~(PEEP_FLAGS_PARK_ENTRANCE_CHOSEN);

    if (peep_should_go_on_ride_again(this, ride))
    {
        guest_heading_to_ride_id = rideIndex;
        peep_is_lost_countdown   = 200;
        peep_reset_pathfind_goal(this);

        rct_window * w = window_find_by_number(WC_PEEP, sprite_index);
        if (w != nullptr)
        {
            window_event_invalidate_call(w);
            widget_invalidate(w, WC_PEEP__WIDX_ACTION_LBL);
        }
    }

    if (peep_should_preferred_intensity_increase(this))
    {
        if (intensity <= 255 - 16)
        {
            intensity += 16;
        }
    }

    if (peep_really_liked_ride(this, ride))
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_WAS_GREAT, rideIndex);

        int32_t laugh = scenario_rand() & 7;
        if (laugh < 3)
        {
            audio_play_sound_at_location(SOUND_LAUGH_1 + laugh, x, y, z);
        }
    }

    ride->total_customers++;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
}

/**
 *
 *  rct2: 0x0069A512
 */
void rct_peep::RemoveFromRide()
{
    if (state == PEEP_STATE_QUEUING)
    {
        RemoveFromQueue();
    }
    StateReset();
}

/**
 * To simplify check of 0x36BA3E0 and 0x11FF78
 * returns false on no food.
 */
bool rct_peep::HasFood() const
{
    return HasFoodStandardFlag() || HasFoodExtraFlag();
}

/**
*
*  rct2: 0x00695DD2
*/
void rct_peep::PickRideToGoOn()
{
    Ride * ride;

    if (state != PEEP_STATE_WALKING)
        return;
    if (guest_heading_to_ride_id != 255)
        return;
    if (peep_flags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (HasFood())
        return;
    if (x == LOCATION_NULL)
        return;

    uint32_t rideConsideration[8]{};

    // FIX  Originally checked for a toy, likely a mistake and should be a map,
    //      but then again this seems to only allow the peep to go on
    //      rides they haven't been on before.
    if (item_standard_flags & PEEP_ITEM_MAP)
    {
        // Consider rides that peep hasn't been on yet
        int32_t i;
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
        int32_t cx = floor2(x, 32);
        int32_t cy = floor2(y, 32);
        for (int32_t tileX = cx - 320; tileX <= cx + 320; tileX += 32)
        {
            for (int32_t tileY = cy - 320; tileY <= cy + 320; tileY += 32)
            {
                if (tileX >= 0 && tileY >= 0 && tileX < (256 * 32) && tileY < (256 * 32))
                {
                    rct_tile_element * tileElement = map_get_first_element_at(tileX >> 5, tileY >> 5);
                    do
                    {
                        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                            continue;

                        int32_t rideIndex = track_element_get_ride_index(tileElement);
                        rideConsideration[rideIndex >> 5] |= (1u << (rideIndex & 0x1F));
                    } while (!(tileElement++)->IsLastForTile());
                }
            }
        }

        // Always take the tall rides into consideration (realistic as you can usually see them from anywhere in the park)
        int32_t i;
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
    uint8_t potentialRides[256];
    uint8_t * nextPotentialRide = &potentialRides[0];
    int32_t  numPotentialRides = 0;
    for (int32_t i = 0; i < MAX_RIDES; i++)
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
    int32_t      mostExcitingRideIndex = -1;
    ride_rating mostExcitingRideRating = 0;
    for (int32_t i = 0; i < numPotentialRides; i++)
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
    guest_heading_to_ride_id = mostExcitingRideIndex;
    peep_is_lost_countdown = 200;
    peep_reset_pathfind_goal(this);

    // Invalidate windows
    rct_window * w = window_find_by_number(WC_PEEP, sprite_index);
    if (w != nullptr)
    {
        window_event_invalidate_call(w);
        widget_invalidate(w, WC_PEEP__WIDX_ACTION_LBL);
    }

    // Make peep look at their map if they have one
    if (item_standard_flags & PEEP_ITEM_MAP)
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
bool rct_peep::ShouldGoOnRide(int32_t rideIndex, int32_t entranceNum, bool atQueue, bool thinking)
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
            if (peep_flags & PEEP_FLAGS_LEAVING_PARK)
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
                    if (abs(lastPeepInQueue->z - z) <= 6)
                    {
                        int32_t dx = abs(lastPeepInQueue->x - x);
                        int32_t dy = abs(lastPeepInQueue->y - y);
                        int32_t maxD = std::max(dx, dy);

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
            if (previous_ride == rideIndex)
            {
                ChoseNotToGoOnRide(rideIndex, peepAtRide, false);
                return false;
            }

            // Basic price checks
            if (ridePrice != 0 && !peep_has_voucher_for_free_ride(this, rideIndex))
            {

                if (ridePrice > cash_in_pocket)
                {
                    if (peepAtRide)
                    {
                        if (cash_in_pocket <= 0)
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
            if (ride->last_crash_type != RIDE_CRASH_TYPE_NONE && happiness < 225)
            {
                if (peepAtRide)
                {
                    peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_NOT_SAFE, rideIndex);
                    if (happiness_target >= 64)
                    {
                        happiness_target -= 8;
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
                if (rideIndex == guest_heading_to_ride_id)
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
                        if (happiness_target >= 64)
                        {
                            happiness_target -= 8;
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
                    ride_rating maxIntensity = std::min((intensity >> 4) * 100, 1000) + happiness;
                    ride_rating minIntensity = ((intensity & 0x0F) * 100) - happiness;
                    if (ride->intensity < minIntensity)
                    {
                        if (peepAtRide)
                        {
                            peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_MORE_THRILLING, rideIndex);
                            if (happiness_target >= 64)
                            {
                                happiness_target -= 8;
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
                    ride_rating maxNausea = NauseaMaximumThresholds[(nausea_tolerance & 3)] + happiness;

                    if (ride->nausea > maxNausea)
                    {
                        if (peepAtRide)
                        {
                            peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_SICKENING, rideIndex);
                            if (happiness_target >= 64)
                            {
                                happiness_target -= 8;
                            }
                            ride_update_popularity(ride, 0);
                        }
                        ChoseNotToGoOnRide(rideIndex, peepAtRide, true);
                        return false;
                    }

                    // Very nauseous peeps will only go on very gentle rides.
                    if (ride->nausea >= FIXED_2DP(1, 40) && nausea > 160)
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

            uint32_t value = ride->value;

            // If the value of the ride hasn't yet been calculated, peeps will be willing to pay any amount for the ride.
            if (value != 0xFFFF && !peep_has_voucher_for_free_ride(this, rideIndex) && !(gParkFlags & PARK_FLAGS_NO_MONEY))
            {

                // The amount peeps are willing to pay is decreased by 75% if they had to pay to enter the park.
                if (peep_flags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY)
                    value /= 4;

                // Peeps won't pay more than twice the value of the ride.
                ridePrice = ride_get_price(ride);
                if (ridePrice > (money16)(value * 2))
                {
                    if (peepAtRide)
                    {
                        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_BAD_VALUE, rideIndex);
                        if (happiness_target >= 60)
                        {
                            happiness_target -= 16;
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
                        if (!(peep_flags & PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY))
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

        if (rideIndex == guest_heading_to_ride_id)
        {
            peep_reset_ride_heading(this);
        }

        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_QUEUE_FULL;
        return true;
    }

    ChoseNotToGoOnRide(rideIndex, peepAtRide, false);
    return false;
}

bool rct_peep::ShouldGoToShop(int32_t rideIndex, bool peepAtShop)
{
    Ride * ride = get_ride(rideIndex);

    // Peeps won't go to the same shop twice in a row.
    if (rideIndex == previous_ride)
    {
        ChoseNotToGoOnRide(rideIndex, peepAtShop, true);
        return false;
    }

    if (ride->type == RIDE_TYPE_TOILETS)
    {
        if (toilet < 70)
        {
            ChoseNotToGoOnRide(rideIndex, peepAtShop, true);
            return false;
        }

        // The amount that peeps are willing to pay to use the Toilets scales with their bathroom stat.
        // It effectively has a minimum of $0.10 (due to the check above) and a maximum of $0.60.
        if (ride->price * 40 > toilet)
        {
            if (peepAtShop)
            {
                peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_NOT_PAYING, rideIndex);
                if (happiness_target >= 60)
                {
                    happiness_target -= 16;
                }
                ride_update_popularity(ride, 0);
            }
            ChoseNotToGoOnRide(rideIndex, peepAtShop, true);
            return false;
        }
    }

    if (ride->type == RIDE_TYPE_FIRST_AID)
    {
        if (nausea < 128)
        {
            ChoseNotToGoOnRide(rideIndex, peepAtShop, true);
            return false;
        }
    }

    // Basic price checks
    if (ride->price != 0 && ride->price > cash_in_pocket)
    {
        if (peepAtShop)
        {
            if (cash_in_pocket <= 0)
            {
                peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_SPENT_MONEY, PEEP_THOUGHT_ITEM_NONE);
            }
            else
            {
                peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_CANT_AFFORD_0, rideIndex);
            }
        }
        ChoseNotToGoOnRide(rideIndex, peepAtShop, true);
        return false;
    }

    if (peepAtShop)
    {
        ride_update_popularity(ride, 1);
        if (rideIndex == guest_heading_to_ride_id)
        {
            peep_reset_ride_heading(this);
        }
    }
    return true;
}

// Used when no logging to an expend type required
void rct_peep::SpendMoney(money32 amount)
{
    money16 unused;
    SpendMoney(unused, amount);
}

/**
 *
 *  rct2: 0x0069926C
 * Expend type was previously an offset saved in 0x00F1AEC0
 */
void rct_peep::SpendMoney(money16 & peep_expend_type, money32 amount)
{
    assert(!(gParkFlags & PARK_FLAGS_NO_MONEY));

    cash_in_pocket = std::max(0, cash_in_pocket - amount);
    cash_spent += amount;

    peep_expend_type += (money16)amount;

    window_invalidate_by_number(WC_PEEP, sprite_index);

    gUnk141F568 = gUnk13CA740;
    finance_payment(-amount, gCommandExpenditureType);

    if (gConfigGeneral.show_guest_purchases && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        // HACK Currently disabled for multiplayer due to limitation of all sprites
        //      needing to be synchronised
        if (network_get_mode() == NETWORK_MODE_NONE && !gOpenRCT2Headless)
        {
            money_effect_create_at(amount, x, y, z, true);
        }
    }

    audio_play_sound_at_location(SOUND_PURCHASE, x, y, z);
}

void rct_peep::SetHasRidden(int32_t rideIndex)
{
    rides_been_on[rideIndex / 8] |= 1 << (rideIndex % 8);
    Ride * ride = get_ride(rideIndex);
    SetHasRiddenRideType(ride->type);
}

bool rct_peep::HasRidden(int32_t rideIndex) const
{
    return rides_been_on[rideIndex / 8] & (1 << (rideIndex % 8));
}

void rct_peep::SetHasRiddenRideType(int32_t rideType)
{
    ride_types_been_on[rideType / 8] |= 1 << (rideType % 8);
}

bool rct_peep::HasRiddenRideType(int32_t rideType) const
{
    return ride_types_been_on[rideType / 8] & (1 << (rideType % 8));
}

void rct_peep::ChoseNotToGoOnRide(int32_t rideIndex, bool peepAtRide, bool updateLastRide)
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

static bool peep_has_voucher_for_free_ride(rct_peep * peep, int32_t rideIndex)
{
    return peep->item_standard_flags & PEEP_ITEM_VOUCHER && peep->voucher_type == VOUCHER_TYPE_RIDE_FREE &&
           peep->voucher_arguments == rideIndex;
}

/**
 * When the queue is full, peeps will ignore the ride when thinking about what to go on next.
 * Does not effect peeps that walk up to the queue entrance.
 * This flag is reset the next time a peep successfully joins the queue.
 */
static void peep_tried_to_enter_full_queue(rct_peep * peep, int32_t rideIndex)
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

static void peep_ride_is_too_intense(rct_peep * peep, int32_t rideIndex, bool peepAtRide)
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
static rct_vehicle * peep_choose_car_from_ride(rct_peep * peep, Ride * ride, std::vector<uint8_t> &car_array)
{
    uint8_t chosen_car = scenario_rand();
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_G_FORCES) && ((chosen_car & 0xC) != 0xC))
    {
        chosen_car = (scenario_rand() & 1) ? 0 : (uint8_t)car_array.size() - 1;
    }
    else
    {
        chosen_car = (chosen_car * (uint16_t)car_array.size()) >> 8;
    }

    peep->current_car = car_array[chosen_car];

    rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[peep->current_train]);

    for (int32_t i = peep->current_car; i > 0; --i)
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
    uint8_t chosen_seat = vehicle->next_free_seat;

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
    int32_t x = location.x;
    int32_t y = location.y;

    uint8_t direction = location.direction;

    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    int16_t x_shift = word_981D6C[direction].x;
    int16_t y_shift = word_981D6C[direction].y;

    uint8_t            shift_multiplier = 21;
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

static bool peep_find_vehicle_to_enter(rct_peep * peep, Ride * ride, std::vector<uint8_t> &car_array)
{
    uint8_t chosen_train = 0xFF;

    if (ride->mode == RIDE_MODE_BUMPERCAR || ride->mode == RIDE_MODE_RACE)
    {
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
            return false;

        for (int32_t i = 0; i < ride->num_vehicles; ++i)
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

    int32_t i = 0;

    uint16_t        vehicle_id = ride->vehicles[chosen_train];
    rct_vehicle * vehicle = GET_VEHICLE(vehicle_id);

    for (; vehicle_id != SPRITE_INDEX_NULL; vehicle_id = vehicle->next_vehicle_on_train, i++)
    {
        vehicle = GET_VEHICLE(vehicle_id);

        uint8_t num_seats = vehicle->num_seats;
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
            uint8_t position = (((~vehicle->vehicle_sprite_type + 1) >> 3) & 0xF) * 2;
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

    uint16_t value = ride->value;
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
 * The satisfaction values calculated here are used to determine how happy the peep is with the ride,
 * and also affects the satisfaction stat of the ride itself. The factors that affect satisfaction include:
 * - The price of the ride compared to the ride's value
 * - How closely the intensity and nausea of the ride matches the peep's preferences
 * - How long the peep was waiting in the queue
 * - If the peep has been on the ride before, or on another ride of the same type
 */
static int16_t peep_calculate_ride_satisfaction(rct_peep * peep, Ride * ride)
{
    int16_t satisfaction = peep_calculate_ride_value_satisfaction(peep, ride);
    satisfaction += peep_calculate_ride_intensity_nausea_satisfaction(peep, ride);

    // Calculate satisfaction based on how long the peep has been in the queue for.
    // (For comparison: peeps start thinking "I've been queueing for a long time" at 3500 and
    // start leaving the queue at 4300.)
    if (peep->time_in_queue >= 4500)
        satisfaction -= 35;
    else if (peep->time_in_queue >= 2250)
        satisfaction -= 10;
    else if (peep->time_in_queue <= 750)
        satisfaction += 10;

    // Peeps get a small boost in satisfaction if they've been on a ride of the same type before,
    // and this boost is doubled if they've already been on this particular ride.
    if (peep->HasRiddenRideType(ride->type))
        satisfaction += 10;

    if (peep->HasRidden(peep->current_ride))
        satisfaction += 10;

    return satisfaction;
}

/**
 * Check to see if the specified ride should become the peep's favourite.
 * For this, a "ride rating" is calculated based on the excitement of the ride and the peep's current happiness.
 * As this value cannot exceed 255, the happier the peep is, the more irrelevant the ride's excitement becomes.
 * Due to the minimum happiness requirement, an excitement rating of more than 3.8 has no further effect.
 *
 * If the ride rating is higher than any ride the peep has already been on and the happiness criteria is met,
 * the ride becomes the peep's favourite. (This doesn't happen right away, but will be updated once the peep
 * exits the ride.)
 */
static void peep_update_favourite_ride(rct_peep * peep, Ride * ride)
{
    peep->peep_flags &= ~PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
    uint8_t peepRideRating = Math::Clamp(0, (ride->excitement / 4) + peep->happiness, PEEP_MAX_HAPPINESS);
    if (peepRideRating >= peep->favourite_ride_rating)
    {
        if (peep->happiness >= 160 && peep->happiness_target >= 160)
        {
            peep->favourite_ride_rating = peepRideRating;
            peep->peep_flags |= PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE;
        }
    }
}

/* rct2: 0x00695555 */
static int16_t peep_calculate_ride_value_satisfaction(rct_peep * peep, Ride * ride)
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return -30;
    }

    if (ride->value == 0xFFFF)
    {
        return -30;
    }

    money16 ridePrice = ride_get_price(ride);
    if (ride->value >= ridePrice)
    {
        return -5;
    }

    if ((ride->value + ((ride->value * peep->happiness) / 256)) >= ridePrice)
    {
        return -30;
    }

    return 0;
}

/**
 * Calculate satisfaction based on the intensity and nausea of the ride.
 * The best possible score from this section is achieved by having the intensity and nausea
 * of the ride fall exactly within the peep's preferences, but lower scores can still be achieved
 * if the peep's happiness is enough to offset it.
 */
static int16_t peep_calculate_ride_intensity_nausea_satisfaction(rct_peep * peep, Ride * ride)
{
    if (!ride_has_ratings(ride))
    {
        return 70;
    }

    uint8_t       intensitySatisfaction = 3;
    uint8_t       nauseaSatisfaction    = 3;
    ride_rating maxIntensity          = (peep->intensity >> 4) * 100;
    ride_rating minIntensity          = (peep->intensity & 0xF) * 100;
    if (minIntensity <= ride->intensity && maxIntensity >= ride->intensity)
    {
        intensitySatisfaction--;
    }
    minIntensity -= peep->happiness * 2;
    maxIntensity += peep->happiness;
    if (minIntensity <= ride->intensity && maxIntensity >= ride->intensity)
    {
        intensitySatisfaction--;
    }
    minIntensity -= peep->happiness * 2;
    maxIntensity += peep->happiness;
    if (minIntensity <= ride->intensity && maxIntensity >= ride->intensity)
    {
        intensitySatisfaction--;
    }

    // Although it's not shown in the interface, a peep with Average or High nausea tolerance
    // has a minimum preferred nausea value. (For peeps with None or Low, this is set to zero.)
    ride_rating minNausea = NauseaMinimumThresholds[(peep->nausea_tolerance & 3)];
    ride_rating maxNausea = NauseaMaximumThresholds[(peep->nausea_tolerance & 3)];
    if (minNausea <= ride->nausea && maxNausea >= ride->nausea)
    {
        nauseaSatisfaction--;
    }
    minNausea -= peep->happiness * 2;
    maxNausea += peep->happiness;
    if (minNausea <= ride->nausea && maxNausea >= ride->nausea)
    {
        nauseaSatisfaction--;
    }
    minNausea -= peep->happiness * 2;
    maxNausea += peep->happiness;
    if (minNausea <= ride->nausea && maxNausea >= ride->nausea)
    {
        nauseaSatisfaction--;
    }

    uint8_t highestSatisfaction = std::max(intensitySatisfaction, nauseaSatisfaction);
    uint8_t lowestSatisfaction  = std::min(intensitySatisfaction, nauseaSatisfaction);

    switch (highestSatisfaction)
    {
    default:
    case 0:
        return 70;
    case 1:
        switch (lowestSatisfaction)
        {
        default:
        case 0:
            return 50;
        case 1:
            return 35;
        }
    case 2:
        switch (lowestSatisfaction)
        {
        default:
        case 0:
            return 35;
        case 1:
            return 20;
        case 2:
            return 10;
        }
    case 3:
        switch (lowestSatisfaction)
        {
        default:
        case 0:
            return -35;
        case 1:
            return -50;
        case 2:
            return -60;
        case 3:
            return -60;
        }
    }
}

/**
 * Update the nausea growth of the peep based on a ride. This is calculated based on:
 * - The nausea rating of the ride
 * - Their new happiness growth rate (the higher, the less nauseous)
 * - How hungry the peep is (+0% nausea at 50% hunger up to +100% nausea at 100% hunger)
 * - The peep's nausea tolerance (Final modifier: none: 100%, low: 50%, average: 25%, high: 12.5%)
 */
static void peep_update_ride_nausea_growth(rct_peep * peep, Ride * ride)
{
    uint32_t nauseaMultiplier       = Math::Clamp(64, 256 - peep->happiness_target, 200);
    uint32_t nauseaGrowthRateChange = (ride->nausea * nauseaMultiplier) / 512;
    nauseaGrowthRateChange *= std::max(static_cast<uint8_t>(128), peep->hunger) / 64;
    nauseaGrowthRateChange >>= (peep->nausea_tolerance & 3);
    peep->nausea_target = (uint8_t)std::min(peep->nausea_target + nauseaGrowthRateChange, 255u);
}

static bool peep_should_go_on_ride_again(rct_peep * peep, Ride * ride)
{
    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN))
        return false;
    if (!ride_has_ratings(ride))
        return false;
    if (ride->intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity)
        return false;
    if (peep->happiness < 180)
        return false;
    if (peep->energy < 100)
        return false;
    if (peep->nausea > 160)
        return false;
    if (peep->hunger < 30)
        return false;
    if (peep->thirst < 20)
        return false;
    if (peep->toilet > 170)
        return false;

    uint8_t r = (scenario_rand() & 0xFF);
    if (r <= 128)
    {
        if (peep->no_of_rides > 7)
            return false;
        if (r > 64)
            return false;
    }

    return true;
}

static bool peep_should_preferred_intensity_increase(rct_peep * peep)
{
    if (gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES)
        return false;
    if (peep->happiness < 200)
        return false;

    return (scenario_rand() & 0xFF) >= peep->intensity;
}

static bool peep_really_liked_ride(rct_peep * peep, Ride * ride)
{
    if (peep->happiness < 215)
        return false;
    if (peep->nausea > 120)
        return false;
    if (!ride_has_ratings(ride))
        return false;
    if (ride->intensity > RIDE_RATING(10, 00) && !gCheatsIgnoreRideIntensity)
        return false;
    return true;
}

/**
 *
 *  rct2: 0x0069BC9A
 */
static uint8_t peep_assess_surroundings(int16_t centre_x, int16_t centre_y, int16_t centre_z)
{
    if ((tile_element_height(centre_x, centre_y) & 0xFFFF) > centre_z)
        return PEEP_THOUGHT_TYPE_NONE;

    uint16_t num_scenery   = 0;
    uint16_t num_fountains = 0;
    uint16_t nearby_music  = 0;
    uint16_t num_rubbish   = 0;

    int16_t initial_x = std::max(centre_x - 160, 0);
    int16_t initial_y = std::max(centre_y - 160, 0);
    int16_t final_x   = std::min(centre_x + 160, 8192);
    int16_t final_y   = std::min(centre_y + 160, 8192);

    for (int16_t x = initial_x; x < final_x; x += 32)
    {
        for (int16_t y = initial_y; y < final_y; y += 32)
        {
            rct_tile_element * tileElement = map_get_first_element_at(x / 32, y / 32);

            do
            {
                Ride *              ride;
                rct_scenery_entry * scenery;

                switch (tileElement->GetType())
                {
                case TILE_ELEMENT_TYPE_PATH:
                    if (!footpath_element_has_path_scenery(tileElement))
                        break;

                    scenery = get_footpath_item_entry(footpath_element_get_path_scenery_index(tileElement));
                    if (scenery == nullptr)
                    {
                        return PEEP_THOUGHT_TYPE_NONE;
                    }
                    if (footpath_element_path_scenery_is_ghost(tileElement))
                        break;

                    if (scenery->path_bit.flags & (PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER | PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW))
                    {
                        num_fountains++;
                        break;
                    }
                    if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
                    {
                        num_rubbish++;
                    }
                    break;
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                    num_scenery++;
                    break;
                case TILE_ELEMENT_TYPE_TRACK:
                    ride = get_ride(track_element_get_ride_index(tileElement));
                    if (ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC && ride->status != RIDE_STATUS_CLOSED &&
                        !(ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)))
                    {

                        if (ride->type == RIDE_TYPE_MERRY_GO_ROUND)
                        {
                            nearby_music |= 1;
                            break;
                        }

                        if (ride->music == MUSIC_STYLE_ORGAN)
                        {
                            nearby_music |= 1;
                            break;
                        }

                        if (ride->type == RIDE_TYPE_DODGEMS)
                        {
                            // Dodgems drown out music?
                            nearby_music |= 2;
                        }
                    }
                    break;
                }
            } while (!(tileElement++)->IsLastForTile());
        }
    }

    rct_litter * litter;
    for (uint16_t sprite_idx = gSpriteListHead[SPRITE_LIST_LITTER]; sprite_idx != SPRITE_INDEX_NULL; sprite_idx = litter->next)
    {
        litter = &(get_sprite(sprite_idx)->litter);

        int16_t dist_x = abs(litter->x - centre_x);
        int16_t dist_y = abs(litter->y - centre_y);
        if (std::max(dist_x, dist_y) <= 160)
        {
            num_rubbish++;
        }
    }

    if (num_fountains >= 5 && num_rubbish < 20)
        return PEEP_THOUGHT_TYPE_FOUNTAINS;

    if (num_scenery >= 40 && num_rubbish < 8)
        return PEEP_THOUGHT_TYPE_SCENERY;

    if (nearby_music == 1 && num_rubbish < 20)
        return PEEP_THOUGHT_TYPE_MUSIC;

    if (num_rubbish < 2 && !gCheatsDisableLittering)
        // if disable littering cheat is enabled, peeps will not have the "clean and tidy park" thought
        return PEEP_THOUGHT_TYPE_VERY_CLEAN;

    return PEEP_THOUGHT_TYPE_NONE;
}

/**
 *
 *  rct2: 0x0068F9A9
 */
static void peep_update_hunger(rct_peep * peep)
{
    if (peep->hunger >= 3)
    {
        peep->hunger -= 2;

        peep->energy_target = std::min(peep->energy_target + 2, PEEP_MAX_ENERGY_TARGET);
        peep->toilet        = std::min(peep->toilet + 1, 255);
    }
}

/**
 * Main purpose is to decide when peeps leave the park due to
 * low happiness, low energy and (if appropriate) low money.
 *
 *  rct2: 0x0068F8CD
 */
static void peep_decide_whether_to_leave_park(rct_peep * peep)
{
    if (peep->energy_target >= 33)
    {
        peep->energy_target -= 2;
    }

    if (gClimateCurrent.Temperature >= 21 && peep->thirst >= 5)
    {
        peep->thirst--;
    }

    if (peep->outside_of_park != 0)
    {
        return;
    }

    /* Peeps that are happy enough, have enough energy and
     * (if appropriate) have enough money will always stay
     * in the park. */
    if (!(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK))
    {
        if (gParkFlags & PARK_FLAGS_NO_MONEY)
        {
            if (peep->energy >= 70 && peep->happiness >= 60)
            {
                return;
            }
        }
        else
        {
            if (peep->energy >= 55 && peep->happiness >= 45 && peep->cash_in_pocket >= MONEY(5, 00))
            {
                return;
            }
        }
    }

    // Approx 95% chance of staying in the park
    if ((scenario_rand() & 0xFFFF) > 3276)
    {
        return;
    }

    // In the remaining 5% chance the peep leaves the park.
    peep_leave_park(peep);
}

/**
 *
 *  rct2: 0x0068F93E
 */
static void peep_leave_park(rct_peep * peep)
{
    peep->guest_heading_to_ride_id = 0xFF;
    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
    {
        if (peep->peep_is_lost_countdown < 60)
        {
            return;
        }
    }
    else
    {
        peep->peep_is_lost_countdown = 254;
        peep->peep_flags |= PEEP_FLAGS_LEAVING_PARK;
        peep->peep_flags &= ~PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
    }

    peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_GO_HOME, PEEP_THOUGHT_ITEM_NONE);

    rct_window * w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != nullptr)
        window_event_invalidate_call(w);
    window_invalidate_by_number(WC_PEEP, peep->sprite_index);
}

/**
 *
 *  rct2: 0x00695B70
 */
static void peep_head_for_nearest_ride_type(rct_peep * peep, int32_t rideType)
{
    Ride * ride;

    if (peep->state != PEEP_STATE_SITTING && peep->state != PEEP_STATE_WATCHING && peep->state != PEEP_STATE_WALKING)
    {
        return;
    }
    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (peep->x == LOCATION_NULL)
        return;
    if (peep->guest_heading_to_ride_id != 255)
    {
        ride = get_ride(peep->guest_heading_to_ride_id);
        if (ride->type == rideType)
        {
            return;
        }
    }

    uint32_t rideConsideration[8]{};

    // FIX Originally checked for a toy,.likely a mistake and should be a map
    if ((peep->item_standard_flags & PEEP_ITEM_MAP) && rideType != RIDE_TYPE_FIRST_AID)
    {
        // Consider all rides in the park
        int32_t i;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride->type == rideType)
            {
                rideConsideration[i >> 5] |= (1u << (i & 0x1F));
            }
        }
    }
    else
    {
        // Take nearby rides into consideration
        int32_t cx = floor2(peep->x, 32);
        int32_t cy = floor2(peep->y, 32);
        for (int32_t x = cx - 320; x <= cx + 320; x += 32)
        {
            for (int32_t y = cy - 320; y <= cy + 320; y += 32)
            {
                if (x >= 0 && y >= 0 && x < (256 * 32) && y < (256 * 32))
                {
                    rct_tile_element * tileElement = map_get_first_element_at(x >> 5, y >> 5);
                    do
                    {
                        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                            continue;

                        int32_t rideIndex = track_element_get_ride_index(tileElement);
                        ride             = get_ride(rideIndex);
                        if (ride->type == rideType)
                        {
                            rideConsideration[rideIndex >> 5] |= (1u << (rideIndex & 0x1F));
                        }
                    } while (!(tileElement++)->IsLastForTile());
                }
            }
        }
    }

    // Filter the considered rides
    uint8_t   potentialRides[256];
    uint8_t * nextPotentialRide = &potentialRides[0];
    int32_t  numPotentialRides = 0;
    for (int32_t i = 0; i < MAX_RIDES; i++)
    {
        if (!(rideConsideration[i >> 5] & (1u << (i & 0x1F))))
            continue;

        ride = get_ride(i);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL))
        {
            if (peep->ShouldGoOnRide(i, 0, false, true))
            {
                *nextPotentialRide++ = i;
                numPotentialRides++;
            }
        }
    }

    // Pick the closest ride
    int32_t closestRideIndex    = -1;
    int32_t closestRideDistance = std::numeric_limits<int32_t>::max();
    for (int32_t i = 0; i < numPotentialRides; i++)
    {
        ride            = get_ride(potentialRides[i]);
        int32_t rideX    = ride->station_starts[0].x * 32;
        int32_t rideY    = ride->station_starts[0].y * 32;
        int32_t distance = abs(rideX - peep->x) + abs(rideY - peep->y);
        if (distance < closestRideDistance)
        {
            closestRideIndex    = potentialRides[i];
            closestRideDistance = distance;
        }
    }
    if (closestRideIndex == -1)
        return;

    // Head to that ride
    peep->guest_heading_to_ride_id = closestRideIndex;
    peep->peep_is_lost_countdown   = 200;
    peep_reset_pathfind_goal(peep);

    // Invalidate windows
    rct_window * w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != nullptr)
    {
        window_event_invalidate_call(w);
        widget_invalidate(w, WC_PEEP__WIDX_ACTION_LBL);
    }

    peep->time_lost = 0;
}

/**
 *
 *  rct2: 0x006958D0
 */
static void peep_head_for_nearest_ride_with_flags(rct_peep * peep, int32_t rideTypeFlags)
{
    Ride * ride;

    if (peep->state != PEEP_STATE_SITTING && peep->state != PEEP_STATE_WATCHING && peep->state != PEEP_STATE_WALKING)
    {
        return;
    }
    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
        return;
    if (peep->x == LOCATION_NULL)
        return;
    if (peep->guest_heading_to_ride_id != 255)
    {
        ride = get_ride(peep->guest_heading_to_ride_id);
        if (ride_type_has_flag(
                ride->type, RIDE_TYPE_FLAG_IS_BATHROOM | RIDE_TYPE_FLAG_SELLS_DRINKS | RIDE_TYPE_FLAG_SELLS_FOOD))
        {
            return;
        }
    }

    if ((rideTypeFlags & RIDE_TYPE_FLAG_IS_BATHROOM) && peep->HasFood())
    {
        return;
    }

    uint32_t rideConsideration[8]{};

    // FIX Originally checked for a toy,.likely a mistake and should be a map
    if (peep->item_standard_flags & PEEP_ITEM_MAP)
    {
        // Consider all rides in the park
        int32_t i;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride_type_has_flag(ride->type, rideTypeFlags))
            {
                rideConsideration[i >> 5] |= (1u << (i & 0x1F));
            }
        }
    }
    else
    {
        // Take nearby rides into consideration
        int32_t cx = floor2(peep->x, 32);
        int32_t cy = floor2(peep->y, 32);
        for (int32_t x = cx - 320; x <= cx + 320; x += 32)
        {
            for (int32_t y = cy - 320; y <= cy + 320; y += 32)
            {
                if (x >= 0 && y >= 0 && x < (256 * 32) && y < (256 * 32))
                {
                    rct_tile_element * tileElement = map_get_first_element_at(x >> 5, y >> 5);
                    do
                    {
                        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                            continue;

                        int32_t rideIndex = track_element_get_ride_index(tileElement);
                        ride             = get_ride(rideIndex);
                        if (ride_type_has_flag(ride->type, rideTypeFlags))
                        {
                            rideConsideration[rideIndex >> 5] |= (1u << (rideIndex & 0x1F));
                        }
                    } while (!(tileElement++)->IsLastForTile());
                }
            }
        }
    }

    // Filter the considered rides
    uint8_t   potentialRides[256];
    uint8_t * nextPotentialRide = &potentialRides[0];
    int32_t  numPotentialRides = 0;
    for (int32_t i = 0; i < MAX_RIDES; i++)
    {
        if (!(rideConsideration[i >> 5] & (1u << (i & 0x1F))))
            continue;

        ride = get_ride(i);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_QUEUE_FULL))
        {
            if (peep->ShouldGoOnRide(i, 0, false, true))
            {
                *nextPotentialRide++ = i;
                numPotentialRides++;
            }
        }
    }

    // Pick the closest ride
    int32_t closestRideIndex    = -1;
    int32_t closestRideDistance = std::numeric_limits<int32_t>::max();
    for (int32_t i = 0; i < numPotentialRides; i++)
    {
        ride            = get_ride(potentialRides[i]);
        int32_t rideX    = ride->station_starts[0].x * 32;
        int32_t rideY    = ride->station_starts[0].y * 32;
        int32_t distance = abs(rideX - peep->x) + abs(rideY - peep->y);
        if (distance < closestRideDistance)
        {
            closestRideIndex    = potentialRides[i];
            closestRideDistance = distance;
        }
    }
    if (closestRideIndex == -1)
        return;

    // Head to that ride
    peep->guest_heading_to_ride_id = closestRideIndex;
    peep->peep_is_lost_countdown   = 200;
    peep_reset_pathfind_goal(peep);

    // Invalidate windows
    rct_window * w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != nullptr)
    {
        window_event_invalidate_call(w);
        window_invalidate(w);
    }

    peep->time_lost = 0;
}

/**
 *
 *  rct2: 0x00699FE3
 * Stops peeps that are having thoughts
 * such as "I'm hungry" after visiting a food shop.
 * Works for Thirst/Hungry/Low Money/Bathroom
 */
void rct_peep::StopPurchaseThought(uint8_t ride_type)
{

    uint8_t thoughtType = PEEP_THOUGHT_TYPE_HUNGRY;

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
    for (int32_t i = 0; i < PEEP_MAX_THOUGHTS; ++i)
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
static bool peep_should_use_cash_machine(rct_peep * peep, int32_t rideIndex)
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
            int16_t actionX;
            int16_t actionY;
            int16_t xy_distance;
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
                Invalidate();

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

        int16_t actionX, actionY, xy_distance;

        if (UpdateAction(&actionX, &actionY, &xy_distance))
        {
            int16_t actionZ = z;
            if (xy_distance < 16)
            {
                auto entrance = ride_get_entrance_location(ride, current_ride_station);
                actionZ = entrance.z * 8 + 2;
            }
            MoveTo(actionX, actionY, actionZ);
            Invalidate();
        }
        else
        {
            destination_tolerance = 0;
            sprite_direction ^= (1 << 4);
        }
    }

    std::vector<uint8_t> carArray;

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
static constexpr const LocationXY16 _MazeEntranceStart[] = {
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

    entrance_loc.x += CoordsDirectionDelta[entrance_loc.direction].x;
    entrance_loc.y += CoordsDirectionDelta[entrance_loc.direction].y;

    uint8_t direction = entrance_loc.direction * 4 + 11;
    if (scenario_rand() & 0x40)
    {
        direction += 4;
        peep->maze_last_edge += 2;
    }

    direction &= 0xF;
    // Direction is 11, 15, 3, or 7
    peep->var_37 = direction;
    peep->maze_last_edge &= 3;

    entrance_loc.x += _MazeEntranceStart[direction / 4].x;
    entrance_loc.y += _MazeEntranceStart[direction / 4].y;

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

    uint8_t direction_track = (tile_element == nullptr ? 0 : tile_element_get_direction(tile_element));

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

static uint8_t peep_get_waypointed_seat_location(rct_peep * peep, Ride * ride, rct_ride_entry_vehicle * vehicle_type, uint8_t track_direction)
{
    // The seatlocation can be split into segments around the ride base
    // to decide the segment first split off the segmentable seat location
    // from the fixed section
    uint8_t seatLocationSegment = peep->current_seat & 0x7;
    uint8_t seatLocationFixed = peep->current_seat & 0xF8;

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
    uint8_t direction_entrance = entranceLocation.direction;

    LocationXY16 waypoint;
    waypoint.x = ride->station_starts[peep->current_ride_station].x * 32 + 16;
    waypoint.y = ride->station_starts[peep->current_ride_station].y * 32 + 16;

    rct_tile_element * tile_element = ride_get_station_start_track_element(ride, peep->current_ride_station);

    uint8_t direction_track = (tile_element == nullptr ? 0 : tile_element_get_direction(tile_element));

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
    int16_t actionX, actionY, actionZ, xy_distance;

    Ride *           ride       = get_ride(current_ride);
    rct_ride_entry * ride_entry = get_ride_entry(ride->subtype);

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        uint16_t distanceThreshold = 16;
        if (ride_entry != nullptr)
        {
            uint8_t vehicle = ride_entry->default_vehicle;
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

        MoveTo(actionX, actionY, actionZ);
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
    for (int32_t i = current_car; i != 0; --i)
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

    int8_t load_position = 0;
    // Safe, in case current seat > number of loading positions
    uint16_t numSeatPositions = static_cast<uint16_t>(vehicle_type->peep_loading_positions.size());
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
static void peep_go_to_ride_exit(rct_peep * peep, Ride * ride, int16_t x, int16_t y, int16_t z, uint8_t exit_direction)
{
    z += RideData5[ride->type].z;

    sprite_move(x, y, z, (rct_sprite *)peep);
    peep->Invalidate();

    Guard::Assert(peep->current_ride_station < MAX_STATIONS);
    auto exit = ride_get_exit_location(ride, peep->current_ride_station);
    Guard::Assert(!exit.isNull());
    x = exit.x;
    y = exit.y;
    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    int16_t x_shift = word_981D6C[exit_direction].x;
    int16_t y_shift = word_981D6C[exit_direction].y;

    int16_t shift_multiplier = 20;

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
    uint8_t queueTime = days_in_queue;
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
        set_format_arg(2, uint32_t, id);
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32_t, ride->name_arguments);

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

    int32_t x = entranceLocation.x * 32;
    int32_t y = entranceLocation.y * 32;
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
    for (int32_t i = current_car; i != 0; --i)
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
        uint8_t seat = current_seat | 1;
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
    int16_t actionX, actionY, xy_distance;
    if (!UpdateAction(&actionX, &actionY, &xy_distance))
    {
        sub_state = PEEP_RIDE_ENTER_VEHICLE;
        return;
    }

    Invalidate();
    MoveTo(actionX, actionY, z);
    Invalidate();
}

void rct_peep::UpdateRideEnterVehicle()
{
    Ride * ride = get_ride(current_ride);

    rct_vehicle * vehicle = GET_VEHICLE(ride->vehicles[current_train]);
    for (int32_t i = current_car; i != 0; --i)
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
    MoveTo(LOCATION_NULL, 0, 0);

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
    uint8_t         ride_station = vehicle->current_station;

    for (int32_t i = current_car; i != 0; --i)
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
        int8_t bestStationIndex = ride_get_first_valid_station_exit(ride);
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
                uint16_t trackType = vehicle->track_type >> 2;
                if (trackType == TRACK_ELEM_FLAT || trackType > TRACK_ELEM_MIDDLE_STATION)
                    continue;

                rct_tile_element * inner_map = map_get_first_element_at(vehicle->track_x / 32, vehicle->track_y / 32);
                for (;; inner_map++)
                {
                    if (inner_map->GetType() != TILE_ELEMENT_TYPE_TRACK)
                        continue;
                    if (inner_map->base_height == vehicle->track_z / 8)
                        break;
                }

                uint8_t stationIndex = tile_element_get_station(inner_map);
                if (stationIndex == current_ride_station)
                    break;
            }

            uint8_t shiftMultiplier = 12;
            uint8_t specialDirection        = platformLocation.direction;

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

            int16_t xShift = word_981D6C[specialDirection].x;
            int16_t yShift = word_981D6C[specialDirection].y;

            platformLocation.x = vehicle->x + xShift * shiftMultiplier;
            platformLocation.y = vehicle->y + yShift * shiftMultiplier;
            platformLocation.z *= 8;

            peep_go_to_ride_exit(this, ride, platformLocation.x, platformLocation.y, platformLocation.z, platformLocation.direction);
            return;
        }

        platformLocation.x = vehicle->x + word_981D6C[platformLocation.direction].x * 12;
        platformLocation.y = vehicle->y + word_981D6C[platformLocation.direction].y * 12;

        int8_t loadPosition = vehicle_entry->peep_loading_positions[current_seat];

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

    waypointLoc.z = (int16_t)exitLocation.z * 8 + RideData5[ride->type].z;
    waypointLoc.x = ride->station_starts[current_ride_station].x * 32 + 16;
    waypointLoc.y = ride->station_starts[current_ride_station].y * 32 + 16;

    rct_tile_element * trackElement = ride_get_station_start_track_element(ride, current_ride_station);

    uint8_t station_direction = (trackElement == nullptr ? 0 : tile_element_get_direction(trackElement));

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

    MoveTo(exitWaypointLoc.x, exitWaypointLoc.y, exitWaypointLoc.z);
    Invalidate();

    waypointLoc.x += vehicleEntry->peep_loading_waypoints[var_37 / 4][1].x;
    waypointLoc.y += vehicleEntry->peep_loading_waypoints[var_37 / 4][1].y;

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
    int16_t x = exit.x;
    int16_t y = exit.y;
    uint8_t exit_direction = exit.direction;

    x *= 32;
    y *= 32;
    x += 16;
    y += 16;

    int16_t x_shift = word_981D6C[exit_direction].x;
    int16_t y_shift = word_981D6C[exit_direction].y;

    int16_t shift_multiplier = 20;

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
    int16_t actionX, actionY, xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        MoveTo(actionX, actionY, z);
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
    int16_t actionX, actionY, xy_distance;
    Ride * ride = get_ride(current_ride);

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();

        if (xy_distance >= 16)
        {
            int16_t actionZ = ride->station_heights[current_ride_station] * 8;

            actionZ += RideData5[ride->type].z;
            MoveTo(actionX, actionY, actionZ);
            Invalidate();
            return;
        }

        SwitchToSpecialSprite(0);
        MoveTo(actionX, actionY, z);
        Invalidate();
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
    {
        uint8_t secondaryItem = RidePhotoItems[ride->type];
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
    int16_t actionX, actionY, xy_distance;
    Ride * ride = get_ride(current_ride);
    uint8_t waypoint = var_37 & 3;

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        int16_t actionZ;
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
        MoveTo(actionX, actionY, actionZ);
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
    int16_t actionX, actionY, xy_distance;
    Ride * ride = get_ride(current_ride);

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        int16_t actionZ;
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
        MoveTo(actionX, actionY, actionZ);
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
    uint8_t exit_direction = exit.direction ^ 2;

    actionX *= 32;
    actionY *= 32;
    actionX += 16;
    actionY += 16;

    int16_t x_shift = word_981D6C[exit_direction].x;
    int16_t y_shift = word_981D6C[exit_direction].y;

    int16_t shift_multiplier = 20;

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
    int16_t actionX, actionY, xy_distance;
    Ride * ride = get_ride(current_ride);

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        MoveTo(actionX, actionY, z);
        Invalidate();
        return;
    }

    uint8_t waypoint = var_37 & 3;

    if (waypoint == 3)
    {
        sub_state     = 15;
        destination_x = 0;
        destination_y = 0;
        var_37        = (var_37 / 4) & 0xC;
        MoveTo(LOCATION_NULL, actionY, z);
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
            if ((uint8_t)(current_car - 1) > (scenario_rand() & 0xF))
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
static constexpr const CoordsXY _SpiralSlideEnd[] = {
    { 25, 56 },
    { 56, 7 },
    { 7, -24 },
    { -24, 25 },
};

/** rct2: 0x00981F1C, 0x00981F1E */
static constexpr const CoordsXY _SpiralSlideEndWaypoint[] = {
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
            int16_t newX  = ride->station_starts[current_ride_station].x * 32;
            int16_t newY  = ride->station_starts[current_ride_station].y * 32;
            uint8_t  dir   = (var_37 / 4) & 3;

            // Set the location that the peep walks to go on slide again
            destination_x = newX + _SpiralSlideEndWaypoint[dir].x;
            destination_y = newY + _SpiralSlideEndWaypoint[dir].y;

            // Move the peep sprite to just at the end of the slide
            newX += _SpiralSlideEnd[dir].x;
            newY += _SpiralSlideEnd[dir].y;

            MoveTo(newX, newY, z);

            sprite_direction = (var_37 & 0xC) * 2;
            Invalidate();

            var_37++;
            return;
        }
        default:
            return;
        }
    }

    int16_t actionX, actionY, xy_distance;

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        MoveTo(actionX, actionY, z);
        Invalidate();
        return;
    }
    uint8_t waypoint = 2;
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
    int16_t actionX, actionY, xy_distance;

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        MoveTo(actionX, actionY, z);
        Invalidate();
        return;
    }

    Ride * ride = get_ride(current_ride);

    uint8_t waypoint = var_37 & 3;

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

    int16_t xShift = word_981D6C[exit.direction].x;
    int16_t yShift = word_981D6C[exit.direction].y;

    int16_t shiftMultiplier = 20;

    xShift *= shiftMultiplier;
    yShift *= shiftMultiplier;

    actionX -= xShift;
    actionY -= yShift;

    destination_x = actionX;
    destination_y = actionY;
}

/** rct2: 0x00981FE4 */
static constexpr const uint8_t _MazeGetNewDirectionFromEdge[][4] = {
    { 15, 7, 15, 7 },
    { 11, 3, 11, 3 },
    { 7, 15, 7, 15 },
    { 3, 11, 3, 11 },
};

/** rct2: 0x00981FF4 */
static constexpr const uint8_t _MazeCurrentDirectionToOpenHedge[][4] = {
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
    int16_t actionX, actionY, xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        MoveTo(actionX, actionY, z);
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
    int16_t stationHeight = ride->station_heights[0];

    // Find the station track element
    rct_tile_element * tileElement = map_get_first_element_at(actionX / 32, actionY / 32);
    do
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK && stationHeight == tileElement->base_height)
            break;

    } while (!(tileElement++)->IsLastForTile());

    uint16_t mazeEntry  = track_element_get_maze_entry(tileElement);
    uint16_t openHedges = 0;
    // var_37 is 3, 7, 11 or 15

    if (mazeEntry & (1 << _MazeCurrentDirectionToOpenHedge[var_37 / 4][3]))
    {
        openHedges = 1;
    }
    openHedges <<= 1;
    if (mazeEntry & (1 << _MazeCurrentDirectionToOpenHedge[var_37 / 4][2]))
    {
        openHedges |= 1;
    }
    openHedges <<= 1;
    if (mazeEntry & (1 << _MazeCurrentDirectionToOpenHedge[var_37 / 4][1]))
    {
        openHedges |= 1;
    }
    openHedges <<= 1;
    if (mazeEntry & (1 << _MazeCurrentDirectionToOpenHedge[var_37 / 4][0]))
    {
        openHedges |= 1;
    }

    openHedges ^= 0xF;
    if (openHedges == 0)
        return;

    uint8_t mazeLastEdge = maze_last_edge ^ (1 << 1);
    openHedges &= ~(1 << mazeLastEdge);
    if (openHedges == 0)
        openHedges |= (1 << mazeLastEdge);

    uint8_t chosenEdge = scenario_rand() & 0x3;
    while (!(openHedges & (1 << chosenEdge)))
    {
        chosenEdge = (chosenEdge + 1) & 3;
    }

    actionX = CoordsDirectionDelta[chosenEdge].x / 2;
    actionY = CoordsDirectionDelta[chosenEdge].y / 2;

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

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
        {
            mazeType = maze_type::hedge;
            break;
        }

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE &&
            tileElement->properties.entrance.type == ENTRANCE_TYPE_RIDE_EXIT)
        {
            mazeType = maze_type::entrance_or_exit;
            break;
        }
    } while (!(tileElement++)->IsLastForTile());

    switch (mazeType)
    {
    case maze_type::invalid:
        maze_last_edge++;
        maze_last_edge &= 3;
        return;
    case maze_type::hedge:
        destination_x = actionX;
        destination_y = actionY;

        var_37         = _MazeGetNewDirectionFromEdge[var_37 / 4][chosenEdge];
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
        MoveTo(actionX, actionY, z);
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
    int16_t actionX, actionY, xy_distance;
    Ride * ride = get_ride(current_ride);

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        MoveTo(actionX, actionY, ride->station_heights[current_ride_station] * 8);
        Invalidate();
        return;
    }

    OnExitRide(current_ride);

    if (peep_flags & PEEP_FLAGS_TRACKING)
    {
        set_format_arg(0, rct_string_id, name_string_idx);
        set_format_arg(2, uint32_t, id);
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32_t, ride->name_arguments);

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
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        int16_t height = map_height_from_slope(x, y, tileElement->properties.path.type);
        height += tileElement->base_height * 8;

        int16_t z_diff = z - height;
        if (z_diff > 0 || z_diff < -16)
            continue;

        MoveTo(x, y, height);
        Invalidate();
        return;
    } while (!(tileElement++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x0069299C
 */
void rct_peep::UpdateRideShopApproach()
{
    int16_t actionX, actionY, xy_distance;

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        MoveTo(actionX, actionY, z);
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
    const int16_t tileCenterX = next_x + 16;
    const int16_t tileCenterY = next_y + 16;
    Ride * ride = get_ride(current_ride);

    if (ride->type == RIDE_TYPE_FIRST_AID)
    {
        if (nausea <= 35)
        {
            sub_state = PEEP_SHOP_LEAVE;

            destination_x         = tileCenterX;
            destination_y         = tileCenterY;
            destination_tolerance = 3;
            happiness_target      = std::min(happiness_target + 30, PEEP_MAX_HAPPINESS);
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

    happiness_target = std::min(happiness_target + 30, PEEP_MAX_HAPPINESS);
    happiness        = happiness_target;
    StopPurchaseThought(ride->type);
}

/**
 *
 *  rct2: 0x00692935
 */
void rct_peep::UpdateRideShopLeave()
{
    int16_t actionX, actionY, xy_distance;

    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        MoveTo(actionX, actionY, z);
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
    next_flags &= ~PEEP_NEXT_FLAG_IS_SLOPED;

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

static void peep_update_walking_break_scenery(rct_peep * peep);
static bool peep_find_ride_to_look_at(rct_peep * peep, uint8_t edge, uint8_t * rideToView, uint8_t * rideSeatToView);

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
                Invalidate();

                action                     = PEEP_ACTION_WAVE_2;
                action_frame               = 0;
                action_sprite_image_offset = 0;

                UpdateCurrentActionSpriteType();
                Invalidate();
            }
        }
    }

    if (peep_flags & PEEP_FLAGS_PHOTO)
    {
        if (action >= PEEP_ACTION_NONE_1)
        {
            if ((0xFFFF & scenario_rand()) < 936)
            {
                Invalidate();

                action                     = PEEP_ACTION_TAKE_PHOTO;
                action_frame               = 0;
                action_sprite_image_offset = 0;

                UpdateCurrentActionSpriteType();
                Invalidate();
            }
        }
    }

    if (peep_flags & PEEP_FLAGS_PAINTING)
    {
        if (action >= PEEP_ACTION_NONE_1)
        {
            if ((0xFFFF & scenario_rand()) < 936)
            {
                Invalidate();

                action                     = PEEP_ACTION_DRAW_PICTURE;
                action_frame               = 0;
                action_sprite_image_offset = 0;

                UpdateCurrentActionSpriteType();
                Invalidate();
            }
        }
    }

    if (peep_flags & PEEP_FLAGS_LITTER)
    {
        if (!GetNextIsSurface())
        {
            if ((0xFFFF & scenario_rand()) <= 4096)
            {
                static constexpr const uint8_t litter_types[] = {
                    LITTER_TYPE_EMPTY_CAN,
                    LITTER_TYPE_RUBBISH,
                    LITTER_TYPE_EMPTY_BURGER_BOX,
                    LITTER_TYPE_EMPTY_CUP,
                };
                int32_t ebp       = litter_types[scenario_rand() & 0x3];
                int32_t litterX   = x + (scenario_rand() & 0x7) - 3;
                int32_t litterY   = y + (scenario_rand() & 0x7) - 3;
                int32_t litterDirection = (scenario_rand() & 0x3);

                litter_create(litterX, litterY, z, litterDirection, ebp);
            }
        }
    }
    else if (HasEmptyContainer())
    {
        if ((!GetNextIsSurface()) && ((uint32_t)(sprite_index & 0x1FF) == (gCurrentTicks & 0x1FF)) &&
            ((0xFFFF & scenario_rand()) <= 4096))
        {

            uint8_t pos_stnd = 0;
            for (int32_t container = HasEmptyContainerStandardFlag(); pos_stnd < 32; pos_stnd++)
                if (container & (1u << pos_stnd))
                    break;

            int32_t bp = 0;

            if (pos_stnd != 32)
            {
                item_standard_flags &= ~(1u << pos_stnd);
                bp = item_standard_litter[pos_stnd];
            }
            else
            {
                uint8_t pos_extr = 0;
                for (int32_t container = HasEmptyContainerExtraFlag(); pos_extr < 32; pos_extr++)
                    if (container & (1u << pos_extr))
                        break;
                item_extra_flags &= ~(1u << pos_extr);
                bp = item_extra_litter[pos_extr];
            }

            window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
            UpdateSpriteType();

            int32_t litterX   = x + (scenario_rand() & 0x7) - 3;
            int32_t litterY   = y + (scenario_rand() & 0x7) - 3;
            int32_t litterDirection = (scenario_rand() & 0x3);

            litter_create(litterX, litterY, z, litterDirection, bp);
        }
    }

    // Check if vehicle is blocking the destination tile
    auto curPos = TileCoordsXYZ(CoordsXYZ { x, y, z });
    auto dstPos = TileCoordsXYZ(CoordsXY { destination_x, destination_y }, next_z);
    if (curPos.x != dstPos.x || curPos.y != dstPos.y)
    {
        if (footpath_is_blocked_by_vehicle(dstPos))
        {
            // Wait for vehicle to pass
            return;
        }
    }

    uint8_t pathingResult;
    PerformNextAction(pathingResult);
    if (!(pathingResult & PATHING_DESTINATION_REACHED))
        return;

    if (GetNextIsSurface())
    {
        rct_tile_element * tile_element = map_get_surface_element_at({next_x, next_y});

        int32_t water_height = surface_get_water_height(tile_element);
        if (water_height)
        {
            Invalidate();
            water_height *= 16;
            MoveTo(x, y, water_height);
            Invalidate();

            SetState(PEEP_STATE_FALLING);
            return;
        }
    }

    CheckIfLost();
    CheckCantFindRide();
    CheckCantFindExit();

    if (UpdateWalkingFindBench())
        return;

    if (UpdateWalkingFindBin())
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

    uint16_t chance = HasFood() ? 13107 : 2849;

    if ((scenario_rand() & 0xFFFF) > chance)
        return;

    if (GetNextIsSurface() || GetNextIsSloped())
        return;

    rct_tile_element * tileElement = map_get_first_element_at(next_x / 32, next_y / 32);

    for (;; tileElement++)
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            if (next_z == tileElement->base_height)
                break;
        }
        if (tileElement->IsLastForTile())
        {
            return;
        }
    }

    int32_t positions_free = 15;

    if (footpath_element_has_path_scenery(tileElement))
    {
        if (!footpath_element_path_scenery_is_ghost(tileElement))
        {
            rct_scenery_entry * sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tileElement));
            if (sceneryEntry == nullptr)
            {
                return;
            }

            if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BENCH))
                positions_free = 9;
        }
    }

    int32_t edges = (tileElement->properties.path.edges & 0xF) ^ 0xF;
    if (edges == 0)
        return;

    uint8_t chosen_edge = scenario_rand() & 0x3;

    for (; !(edges & (1 << chosen_edge));)
        chosen_edge = (chosen_edge + 1) & 3;

    uint8_t ride_to_view, ride_seat_to_view;
    if (!peep_find_ride_to_look_at(this, chosen_edge, &ride_to_view, &ride_seat_to_view))
        return;

    // Check if there is a peep watching (and if there is place for us)
    uint16_t sprite_id = sprite_get_first_in_quadrant(x, y);
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

    uint8_t chosen_position = scenario_rand() & 0x3;

    for (; !(positions_free & (1 << chosen_position));)
        chosen_position = (chosen_position + 1) & 3;

    current_ride = ride_to_view;
    current_seat = ride_seat_to_view;
    var_37       = chosen_edge | (chosen_position << 2);

    SetState(PEEP_STATE_WATCHING);
    sub_state = 0;

    int32_t destX = (x & 0xFFE0) + _WatchingPositionOffsets[var_37 & 0x1F].x;
    int32_t destY = (y & 0xFFE0) + _WatchingPositionOffsets[var_37 & 0x1F].y;

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
        Invalidate();
        RemoveFromQueue();
        SetState(PEEP_STATE_1);
        return;
    }

    uint8_t pathingResult;
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
            Invalidate();
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
                Invalidate();
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
        Invalidate();
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
        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if ((pathingResult & PATHING_OUTSIDE_PARK))
        {
            decrement_guests_heading_for_park();
            peep_sprite_remove(this);
        }
        return;
    }
    int16_t actionX = 0;
    int16_t actionY = 0;
    int16_t xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        MoveTo(actionX, actionY, z);
        Invalidate();
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
        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_OUTSIDE_PARK))
            return;
        peep_sprite_remove(this);
        return;
    }

    int16_t actionX = 0;
    int16_t actionY = 0;
    int16_t xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        Invalidate();
        MoveTo(actionX, actionY, z);
        Invalidate();
        return;
    }

    outside_of_park       = 1;
    destination_tolerance = 5;
    decrement_guests_in_park();
    auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
    context_broadcast_intent(&intent);
    var_37 = 1;

    window_invalidate_by_class(WC_GUEST_LIST);
    uint8_t pathingResult;
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
        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        destination_x = x;
        destination_y = y;

        sprite_direction = (var_37 & 3) * 8;
        Invalidate();

        action                  = 0xFE;
        next_action_sprite_type = 2;

        SwitchNextActionSpriteType();

        sub_state++;

        time_to_stand = Math::Clamp(0, ((129 - energy) * 16 + 50) / 2, 255);
        UpdateSpriteType();
    }
    else if (sub_state == 1)
    {
        if (action < 0xFE)
        {
            // 6917F6
            int16_t actionX = 0;
            int16_t actionY = 0;
            int16_t xy_distance;
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
                    Invalidate();
                    return;
                }
            }

            if ((scenario_rand() & 0xFFFF) <= 655)
            {
                action                     = PEEP_ACTION_TAKE_PHOTO;
                action_frame               = 0;
                action_sprite_image_offset = 0;
                UpdateCurrentActionSpriteType();
                Invalidate();
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
                    Invalidate();
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
        UpdateSpriteType();
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
    switch (sub_state)
    {
    case PEEP_USING_BIN_WALKING_TO_BIN:
    {
        if (!CheckForPath())
            return;

        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if (pathingResult & PATHING_DESTINATION_REACHED)
        {
            sub_state = PEEP_USING_BIN_GOING_BACK;
        }
        break;
    }
    case PEEP_USING_BIN_GOING_BACK:
    {
        if (action != PEEP_ACTION_NONE_2)
        {
            int16_t actionX, actionY, xy_distance;
            UpdateAction(&actionX, &actionY, &xy_distance);
            return;
        }

        rct_tile_element * tileElement = map_get_first_element_at(next_x / 32, next_y / 32);

        for (;; tileElement++)
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            {
                continue;
            }

            if (tileElement->base_height == next_z)
                break;

            if (tileElement->IsLastForTile())
            {
                StateReset();
                return;
            }
        }

        if (!footpath_element_has_path_scenery(tileElement))
        {
            StateReset();
            return;
        }

        rct_scenery_entry * sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tileElement));
        if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BIN))
        {
            StateReset();
            return;
        }

        if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
        {
            StateReset();
            return;
        }

        if (footpath_element_path_scenery_is_ghost(tileElement))
        {
            StateReset();
            return;
        }

        // Bin selection is one of 4 corners
        uint8_t selected_bin = var_37 * 2;

        // This counts down 2 = No rubbish, 0 = full
        uint8_t  space_left_in_bin = 0x3 & (tileElement->properties.path.addition_status >> selected_bin);
        uint32_t empty_containers  = HasEmptyContainerStandardFlag();

        for (uint8_t cur_container = 0; cur_container < 32; cur_container++)
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
                UpdateSpriteType();
                continue;
            }
            uint8_t bp = item_standard_litter[cur_container];

            int32_t litterX = x + (scenario_rand() & 7) - 3;
            int32_t litterY = y + (scenario_rand() & 7) - 3;

            litter_create(litterX, litterY, z, scenario_rand() & 3, bp);
            item_standard_flags &= ~(1 << cur_container);
            window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;

            UpdateSpriteType();
        }

        // Original bug: This would clear any rubbish placed by the previous function
        // space_left_in_bin = 0x3 & (tile_element->properties.path.addition_status >> selected_bin);
        empty_containers = HasEmptyContainerExtraFlag();

        for (uint8_t cur_container = 0; cur_container < 32; cur_container++)
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

                UpdateSpriteType();
                continue;
            }
            uint8_t bp = item_extra_litter[cur_container];

            int32_t litterX = x + (scenario_rand() & 7) - 3;
            int32_t litterY = y + (scenario_rand() & 7) - 3;

            litter_create(litterX, litterY, z, scenario_rand() & 3, bp);
            item_extra_flags &= ~(1 << cur_container);
            window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;

            UpdateSpriteType();
        }

        // Place new amount in bin by first clearing the value
        tileElement->properties.path.addition_status &= ~(3 << selected_bin);
        // Then placing the new value.
        tileElement->properties.path.addition_status |= space_left_in_bin << selected_bin;

        map_invalidate_tile_zoom0(next_x, next_y, tileElement->base_height << 3, tileElement->clearance_height << 3);
        StateReset();
        break;
    }
    default:
        Guard::Assert(false, "Invalid sub state");
        break;
    }
}

/* Simplifies 0x690582. Returns true if should find bench*/
bool rct_peep::ShouldFindBench()
{
    if (peep_flags & PEEP_FLAGS_LEAVING_PARK)
    {
        return false;
    }

    if (HasFood())
    {
        if (hunger < 128 || happiness < 128)
        {
            if (!GetNextIsSurface() && !GetNextIsSloped())
            {
                return true;
            }
        }
    }

    if (nausea <= 170 && energy > 50)
    {
        return false;
    }

    if (!GetNextIsSurface() && !GetNextIsSloped())
    {
        return true;
    }

    return false;
}

/**
 *
 *  rct2: 0x00690582
 * Returns true when the guest wants to sit down and has found a bench to sit on
 */
bool rct_peep::UpdateWalkingFindBench()
{
    if (!ShouldFindBench())
        return false;

    rct_tile_element * tileElement = map_get_first_element_at(next_x / 32, next_y / 32);

    for (;; tileElement++)
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            if (next_z == tileElement->base_height)
                break;
        }
        if (tileElement->IsLastForTile())
        {
            return false;
        }
    }

    if (!footpath_element_has_path_scenery(tileElement))
        return false;
    rct_scenery_entry * sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tileElement));

    if (sceneryEntry == nullptr || !(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BENCH))
        return false;

    if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
        return false;

    if (footpath_element_path_scenery_is_ghost(tileElement))
        return false;

    int32_t edges = (tileElement->properties.path.edges & 0xF) ^ 0xF;
    if (edges == 0)
        return false;

    uint8_t chosen_edge = scenario_rand() & 0x3;

    for (; !(edges & (1 << chosen_edge));)
        chosen_edge = (chosen_edge + 1) & 0x3;

    uint16_t sprite_id = sprite_get_first_in_quadrant(x, y);
    uint8_t  free_edge = 3;

    // Check if there is no peep sitting in chosen_edge
    for (rct_sprite * sprite; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->unknown.next_in_quadrant)
    {
        sprite = get_sprite(sprite_id);

        if (sprite->unknown.linked_list_type_offset != SPRITE_LIST_PEEP * 2)
            continue;

        if (sprite->peep.state != PEEP_STATE_SITTING)
            continue;

        if (z != sprite->peep.z)
            continue;

        if ((sprite->peep.var_37 & 0x3) != chosen_edge)
            continue;

        free_edge &= ~(1 << ((sprite->peep.var_37 & 0x4) >> 2));
    }

    if (!free_edge)
        return false;

    free_edge ^= 0x3;
    if (!free_edge)
    {
        if (scenario_rand() & 0x8000000)
            free_edge = 1;
    }

    var_37 = ((free_edge & 1) << 2) | chosen_edge;

    SetState(PEEP_STATE_SITTING);

    sub_state = PEEP_SITTING_TRYING_TO_SIT;

    int32_t benchX = (x & 0xFFE0) + BenchUseOffsets[var_37 & 0x7].x;
    int32_t benchY = (y & 0xFFE0) + BenchUseOffsets[var_37 & 0x7].y;

    destination_x         = benchX;
    destination_y         = benchY;
    destination_tolerance = 3;

    return true;
}

bool rct_peep::UpdateWalkingFindBin()
{
    auto peep = this;
    if (!peep->HasEmptyContainer())
        return false;

    if (peep->GetNextIsSurface())
        return false;

    rct_tile_element * tileElement = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

    for (;; tileElement++)
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            if (peep->next_z == tileElement->base_height)
                break;
        }
        if (tileElement->IsLastForTile())
        {
            return false;
        }
    }

    if (!footpath_element_has_path_scenery(tileElement))
        return false;
    rct_scenery_entry * sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tileElement));
    if (sceneryEntry == nullptr)
    {
        return false;
    }

    if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BIN))
        return false;

    if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
        return false;

    if (footpath_element_path_scenery_is_ghost(tileElement))
        return false;

    int32_t edges = (tileElement->properties.path.edges & 0xF) ^ 0xF;
    if (edges == 0)
        return false;

    uint8_t chosen_edge = scenario_rand() & 0x3;

    // Note: Bin quantity is inverted 0 = full, 3 = empty
    uint8_t bin_quantities = tileElement->properties.path.addition_status;

    // Rotate the bin to the correct edge. Makes it easier for next calc.
    bin_quantities = ror8(ror8(bin_quantities, chosen_edge), chosen_edge);

    for (uint8_t free_edge = 4; free_edge != 0; free_edge--)
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
    peep->sub_state = PEEP_USING_BIN_WALKING_TO_BIN;

    int32_t binX = (peep->x & 0xFFE0) + BinUseOffsets[peep->var_37 & 0x3].x;
    int32_t binY = (peep->y & 0xFFE0) + BinUseOffsets[peep->var_37 & 0x3].y;

    peep->destination_x         = binX;
    peep->destination_y         = binY;
    peep->destination_tolerance = 3;

    return true;
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

    if (peep->GetNextIsSurface())
        return;

    rct_tile_element * tileElement = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);

    for (;; tileElement++)
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            if (peep->next_z == tileElement->base_height)
                break;
        }
        if (tileElement->IsLastForTile())
        {
            return;
        }
    }

    if (!footpath_element_has_path_scenery(tileElement))
        return;
    rct_scenery_entry * sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tileElement));

    if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_BREAKABLE))
        return;

    if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
        return;

    if (footpath_element_path_scenery_is_ghost(tileElement))
        return;

    int32_t edges = tileElement->properties.path.edges & 0xF;
    if (edges == 0xF)
        return;

    uint16_t sprite_id = sprite_get_first_in_quadrant(peep->x, peep->y);

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
    uint16_t     sprite_index;

    FOR_ALL_STAFF(sprite_index, inner_peep)
    {
        if (inner_peep->staff_type != STAFF_TYPE_SECURITY)
            continue;

        if (inner_peep->x == LOCATION_NULL)
            continue;

        int32_t x_diff = abs(inner_peep->x - peep->x);
        int32_t y_diff = abs(inner_peep->y - peep->y);

        if (std::max(x_diff, y_diff) < 224)
            return;
    }

    tileElement->flags |= TILE_ELEMENT_FLAG_BROKEN;

    map_invalidate_tile_zoom1(peep->next_x, peep->next_y, (tileElement->base_height << 3) + 32, tileElement->base_height << 3);

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

bool loc_690FD0(rct_peep * peep, uint8_t * rideToView, uint8_t * rideSeatToView, rct_tile_element * tileElement)
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
static bool peep_find_ride_to_look_at(rct_peep * peep, uint8_t edge, uint8_t * rideToView, uint8_t * rideSeatToView)
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
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
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
    } while (!(tileElement++)->IsLastForTile());

    uint16_t x = peep->next_x + CoordsDirectionDelta[edge].x;
    uint16_t y = peep->next_y + CoordsDirectionDelta[edge].y;
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
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
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
    } while (!(tileElement++)->IsLastForTile());

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

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
        {
            if (peep_should_watch_ride(tileElement))
            {
                return loc_690FD0(peep, rideToView, rideSeatToView, tileElement);
            }
        }

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_LARGE_SCENERY)
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
    } while (!(tileElement++)->IsLastForTile());

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
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_SURFACE)
            continue;
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
            continue;

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_WALL)
        {
            auto wallEntry = get_wall_entry(tileElement->properties.wall.type);
            if (wallEntry == nullptr || (wallEntry->wall.flags2 & WALL_SCENERY_2_IS_OPAQUE))
            {
                continue;
            }
        }

        return false;
    } while (!(tileElement++)->IsLastForTile());

    x += CoordsDirectionDelta[edge].x;
    y += CoordsDirectionDelta[edge].y;
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
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
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
    } while (!(tileElement++)->IsLastForTile());

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

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
        {
            if (peep_should_watch_ride(tileElement))
            {
                return loc_690FD0(peep, rideToView, rideSeatToView, tileElement);
            }
        }

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_LARGE_SCENERY)
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
    } while (!(tileElement++)->IsLastForTile());

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
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_SURFACE)
            continue;
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
            continue;

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_WALL)
        {
            auto wallEntry = get_wall_entry(tileElement->properties.wall.type);
            if (wallEntry == nullptr || (wallEntry->wall.flags2 & WALL_SCENERY_2_IS_OPAQUE))
            {
                continue;
            }
        }

        return false;
    } while (!(tileElement++)->IsLastForTile());

    x += CoordsDirectionDelta[edge].x;
    y += CoordsDirectionDelta[edge].y;
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
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_WALL)
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
    } while (!(tileElement++)->IsLastForTile());

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

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
        {
            if (peep_should_watch_ride(tileElement))
            {
                return loc_690FD0(peep, rideToView, rideSeatToView, tileElement);
            }
        }

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_LARGE_SCENERY)
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
    } while (!(tileElement++)->IsLastForTile());

    return false;
}

/* Part of 0x0069B8CC rct2: 0x0069BC31 */
void rct_peep::SetSpriteType(uint8_t new_sprite_type)
{
    if (sprite_type == new_sprite_type)
        return;

    sprite_type                = new_sprite_type;
    action_sprite_image_offset = 0;
    no_action_frame_num        = 0;

    if (action >= PEEP_ACTION_NONE_1)
        action = PEEP_ACTION_NONE_2;

    peep_flags &= ~PEEP_FLAGS_SLOW_WALK;
    Guard::Assert(new_sprite_type < Util::CountOf(gSpriteTypeToSlowWalkMap));
    if (gSpriteTypeToSlowWalkMap[new_sprite_type])
    {
        peep_flags |= PEEP_FLAGS_SLOW_WALK;
    }

    action_sprite_type = 0xFF;
    UpdateCurrentActionSpriteType();

    if (state == PEEP_STATE_SITTING)
    {
        action                  = PEEP_ACTION_NONE_1;
        next_action_sprite_type = 7;
        SwitchNextActionSpriteType();
    }
    if (state == PEEP_STATE_WATCHING)
    {
        action                  = PEEP_ACTION_NONE_1;
        next_action_sprite_type = 2;
        SwitchNextActionSpriteType();
    }
}

struct item_pref_t
{
    uint8_t  type; // 0 for standard, 1 for extra
    uint32_t item; // And this with the relevant flags
    uint8_t  sprite_type;
};

// clang-format off
static item_pref_t item_order_preference[] = {
        { 0, PEEP_ITEM_ICE_CREAM, PEEP_SPRITE_TYPE_ICE_CREAM },
        { 0, PEEP_ITEM_CHIPS, PEEP_SPRITE_TYPE_CHIPS },
        { 0, PEEP_ITEM_PIZZA, PEEP_SPRITE_TYPE_PIZZA },
        { 0, PEEP_ITEM_BURGER, PEEP_SPRITE_TYPE_BURGER },
        { 0, PEEP_ITEM_DRINK, PEEP_SPRITE_TYPE_DRINK },
        { 0, PEEP_ITEM_COFFEE, PEEP_SPRITE_TYPE_COFFEE },
        { 0, PEEP_ITEM_CHICKEN, PEEP_SPRITE_TYPE_CHICKEN },
        { 0, PEEP_ITEM_LEMONADE, PEEP_SPRITE_TYPE_LEMONADE },
        { 0, PEEP_ITEM_CANDYFLOSS, PEEP_SPRITE_TYPE_CANDYFLOSS },
        { 0, PEEP_ITEM_POPCORN, PEEP_SPRITE_TYPE_PIZZA },
        { 0, PEEP_ITEM_HOT_DOG, PEEP_SPRITE_TYPE_HOT_DOG  },
        { 0, PEEP_ITEM_TENTACLE, PEEP_SPRITE_TYPE_TENTACLE },
        { 0, PEEP_ITEM_TOFFEE_APPLE, PEEP_SPRITE_TYPE_TOFFEE_APPLE },
        { 0, PEEP_ITEM_DOUGHNUT, PEEP_SPRITE_TYPE_DOUGHNUT },
        { 1, PEEP_ITEM_PRETZEL, PEEP_SPRITE_TYPE_PRETZEL },
        { 1, PEEP_ITEM_COOKIE, PEEP_SPRITE_TYPE_PRETZEL },
        { 1, PEEP_ITEM_CHOCOLATE, PEEP_SPRITE_TYPE_COFFEE },
        { 1, PEEP_ITEM_ICED_TEA, PEEP_SPRITE_TYPE_COFFEE },
        { 1, PEEP_ITEM_FUNNEL_CAKE, PEEP_SPRITE_TYPE_FUNNEL_CAKE },
        { 1, PEEP_ITEM_BEEF_NOODLES, PEEP_SPRITE_TYPE_NOODLES },
        { 1, PEEP_ITEM_FRIED_RICE_NOODLES, PEEP_SPRITE_TYPE_NOODLES },
        { 1, PEEP_ITEM_WONTON_SOUP, PEEP_SPRITE_TYPE_SOUP },
        { 1, PEEP_ITEM_MEATBALL_SOUP, PEEP_SPRITE_TYPE_SOUP },
        { 1, PEEP_ITEM_FRUIT_JUICE, PEEP_SPRITE_TYPE_JUICE },
        { 1, PEEP_ITEM_SOYBEAN_MILK, PEEP_SPRITE_TYPE_SU_JONGKWA },
        { 1, PEEP_ITEM_SU_JONGKWA, PEEP_SPRITE_TYPE_SU_JONGKWA },
        { 1, PEEP_ITEM_SUB_SANDWICH, PEEP_SPRITE_TYPE_SANDWICH },
        { 1, PEEP_ITEM_ROAST_SAUSAGE, PEEP_SPRITE_TYPE_SAUSAGE },
        { 0, PEEP_ITEM_BALLOON, PEEP_SPRITE_TYPE_BALLOON },
        { 0, PEEP_ITEM_HAT, PEEP_SPRITE_TYPE_HAT },
        { 1, PEEP_ITEM_SUNGLASSES, PEEP_SPRITE_TYPE_SUNGLASSES },
        { 0xFF, 0xFFFFFFFF, 0xFF}
};
// clang-format on

/**
 *
 *  rct2: 0x0069B8CC
 */
void rct_peep::UpdateSpriteType()
{
    if (sprite_type == PEEP_SPRITE_TYPE_BALLOON && (scenario_rand() & 0xFFFF) <= 327)
    {
        bool isBalloonPopped = false;
        if (x != LOCATION_NULL)
        {
            if ((scenario_rand() & 0xFFFF) <= 13107)
            {
                isBalloonPopped = true;
                audio_play_sound_at_location(SOUND_BALLOON_POP, x, y, z);
            }
            create_balloon(x, y, z + 9, balloon_colour, isBalloonPopped);
        }
        item_standard_flags &= ~PEEP_ITEM_BALLOON;
        window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
    }

    if (climate_is_raining() && (item_standard_flags & PEEP_ITEM_UMBRELLA) && x != LOCATION_NULL)
    {
        if ((x & 0xFFE0) < 0x1FFF && (y & 0xFFE0) < 0x1FFF)
        {
            rct_tile_element * tileElement = map_get_first_element_at(x / 32, y / 32);
            while (true)
            {
                if ((z / 8) < tileElement->base_height)
                    break;

                if (tileElement->IsLastForTile())
                {
                    SetSpriteType(PEEP_SPRITE_TYPE_UMBRELLA);
                    return;
                }
                tileElement++;
            }
        }
    }

    for (item_pref_t * item_pref = item_order_preference; item_pref->type != 0xFF; item_pref++)
    {
        if (item_pref->type == 0)
        {
            if (item_standard_flags & item_pref->item)
            {
                SetSpriteType(item_pref->sprite_type);
                return;
            }
        }
        else
        {
            if (item_extra_flags & item_pref->item)
            {
                SetSpriteType(item_pref->sprite_type);
                return;
            }
        }
    }

    if (state == PEEP_STATE_WATCHING && standing_flags & (1 << 1))
    {
        SetSpriteType(PEEP_SPRITE_TYPE_WATCHING);
        return;
    }

    if (nausea > 170)
    {
        SetSpriteType(PEEP_SPRITE_TYPE_VERY_NAUSEOUS);
        return;
    }

    if (nausea > 140)
    {
        SetSpriteType(PEEP_SPRITE_TYPE_NAUSEOUS);
        return;
    }

    if (energy <= 64 && happiness < 128)
    {
        SetSpriteType(PEEP_SPRITE_TYPE_HEAD_DOWN);
        return;
    }

    if (energy <= 80 && happiness < 128)
    {
        SetSpriteType(PEEP_SPRITE_TYPE_ARMS_CROSSED);
        return;
    }

    if (toilet > 220)
    {
        SetSpriteType(PEEP_SPRITE_TYPE_REQUIRE_BATHROOM);
        return;
    }

    SetSpriteType(PEEP_SPRITE_TYPE_NORMAL);
}
