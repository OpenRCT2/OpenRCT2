/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../world/Location.hpp"
#include "RideTypes.h"

using ride_rating = fixed16_2dp;

// Convenience function for writing ride ratings. The result is a 16 bit signed
// integer. To create the ride rating 3.65 type RIDE_RATING(3,65)
#define RIDE_RATING(whole, fraction) FIXED_2DP(whole, fraction)
#define RIDE_RATING_UNDEFINED static_cast<ride_rating>(static_cast<uint16_t>(0xFFFF))

#pragma pack(push, 1)

// Used for return values, for functions that modify all three.
struct RatingTuple
{
    ride_rating Excitement;
    ride_rating Intensity;
    ride_rating Nausea;
};
assert_struct_size(RatingTuple, 6);

#pragma pack(pop)

enum
{
    RIDE_RATING_STATION_FLAG_NO_ENTRANCE = 1 << 0
};

struct RideRatingCalculationData
{
    CoordsXYZ proximity;
    CoordsXYZ proximity_start;
    ride_id_t current_ride;
    uint8_t state;
    uint8_t proximity_track_type;
    uint8_t proximity_base_height;
    uint16_t proximity_total;
    uint16_t proximity_scores[26];
    uint16_t num_brakes;
    uint16_t num_reversers;
    uint16_t station_flags;
};

extern RideRatingCalculationData gRideRatingsCalcData;

void ride_ratings_update_ride(const Ride& ride);
void ride_ratings_update_all();

using ride_ratings_calculation = void (*)(Ride* ride);
ride_ratings_calculation ride_ratings_get_calculate_func(uint8_t rideType);

void ride_ratings_calculate_spiral_roller_coaster(Ride* ride);
void ride_ratings_calculate_stand_up_roller_coaster(Ride* ride);
void ride_ratings_calculate_suspended_swinging_coaster(Ride* ride);
void ride_ratings_calculate_inverted_roller_coaster(Ride* ride);
void ride_ratings_calculate_junior_roller_coaster(Ride* ride);
void ride_ratings_calculate_miniature_railway(Ride* ride);
void ride_ratings_calculate_monorail(Ride* ride);
void ride_ratings_calculate_mini_suspended_coaster(Ride* ride);
void ride_ratings_calculate_boat_hire(Ride* ride);
void ride_ratings_calculate_wooden_wild_mouse(Ride* ride);
void ride_ratings_calculate_steeplechase(Ride* ride);
void ride_ratings_calculate_car_ride(Ride* ride);
void ride_ratings_calculate_launched_freefall(Ride* ride);
void ride_ratings_calculate_bobsleigh_coaster(Ride* ride);
void ride_ratings_calculate_observation_tower(Ride* ride);
void ride_ratings_calculate_looping_roller_coaster(Ride* ride);
void ride_ratings_calculate_dinghy_slide(Ride* ride);
void ride_ratings_calculate_mine_train_coaster(Ride* ride);
void ride_ratings_calculate_chairlift(Ride* ride);
void ride_ratings_calculate_corkscrew_roller_coaster(Ride* ride);
void ride_ratings_calculate_maze(Ride* ride);
void ride_ratings_calculate_spiral_slide(Ride* ride);
void ride_ratings_calculate_go_karts(Ride* ride);
void ride_ratings_calculate_log_flume(Ride* ride);
void ride_ratings_calculate_river_rapids(Ride* ride);
void ride_ratings_calculate_dodgems(Ride* ride);
void ride_ratings_calculate_pirate_ship(Ride* ride);
void ride_ratings_calculate_inverter_ship(Ride* ride);
void ride_ratings_calculate_food_stall(Ride* ride);
void ride_ratings_calculate_shop(Ride* ride);
void ride_ratings_calculate_merry_go_round(Ride* ride);
void ride_ratings_calculate_information_kiosk(Ride* ride);
void ride_ratings_calculate_toilets(Ride* ride);
void ride_ratings_calculate_ferris_wheel(Ride* ride);
void ride_ratings_calculate_motion_simulator(Ride* ride);
void ride_ratings_calculate_3d_cinema(Ride* ride);
void ride_ratings_calculate_top_spin(Ride* ride);
void ride_ratings_calculate_space_rings(Ride* ride);
void ride_ratings_calculate_reverse_freefall_coaster(Ride* ride);
void ride_ratings_calculate_lift(Ride* ride);
void ride_ratings_calculate_vertical_drop_roller_coaster(Ride* ride);
void ride_ratings_calculate_cash_machine(Ride* ride);
void ride_ratings_calculate_twist(Ride* ride);
void ride_ratings_calculate_haunted_house(Ride* ride);
void ride_ratings_calculate_first_aid(Ride* ride);
void ride_ratings_calculate_circus_show(Ride* ride);
void ride_ratings_calculate_ghost_train(Ride* ride);
void ride_ratings_calculate_twister_roller_coaster(Ride* ride);
void ride_ratings_calculate_wooden_roller_coaster(Ride* ride);
void ride_ratings_calculate_side_friction_roller_coaster(Ride* ride);
void ride_ratings_calculate_wild_mouse(Ride* ride);
void ride_ratings_calculate_multi_dimension_roller_coaster(Ride* ride);
void ride_ratings_calculate_flying_roller_coaster(Ride* ride);
void ride_ratings_calculate_virginia_reel(Ride* ride);
void ride_ratings_calculate_splash_boats(Ride* ride);
void ride_ratings_calculate_mini_helicopters(Ride* ride);
void ride_ratings_calculate_lay_down_roller_coaster(Ride* ride);
void ride_ratings_calculate_suspended_monorail(Ride* ride);
void ride_ratings_calculate_reverser_roller_coaster(Ride* ride);
void ride_ratings_calculate_heartline_twister_coaster(Ride* ride);
void ride_ratings_calculate_mini_golf(Ride* ride);
void ride_ratings_calculate_giga_coaster(Ride* ride);
void ride_ratings_calculate_roto_drop(Ride* ride);
void ride_ratings_calculate_flying_saucers(Ride* ride);
void ride_ratings_calculate_crooked_house(Ride* ride);
void ride_ratings_calculate_monorail_cycles(Ride* ride);
void ride_ratings_calculate_compact_inverted_coaster(Ride* ride);
void ride_ratings_calculate_water_coaster(Ride* ride);
void ride_ratings_calculate_air_powered_vertical_coaster(Ride* ride);
void ride_ratings_calculate_inverted_hairpin_coaster(Ride* ride);
void ride_ratings_calculate_magic_carpet(Ride* ride);
void ride_ratings_calculate_submarine_ride(Ride* ride);
void ride_ratings_calculate_river_rafts(Ride* ride);
void ride_ratings_calculate_enterprise(Ride* ride);
void ride_ratings_calculate_inverted_impulse_coaster(Ride* ride);
void ride_ratings_calculate_mini_roller_coaster(Ride* ride);
void ride_ratings_calculate_mine_ride(Ride* ride);
void ride_ratings_calculate_lim_launched_roller_coaster(Ride* ride);
void ride_ratings_calculate_drink_stall(Ride* ride);
