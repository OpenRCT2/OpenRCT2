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

#ifndef _RIDE_RATINGS_H_
#define _RIDE_RATINGS_H_

#include "../common.h"

typedef fixed16_2dp ride_rating;

// Convenience function for writing ride ratings. The result is a 16 bit signed
// integer. To create the ride rating 3.65 type RIDE_RATING(3,65)
#define RIDE_RATING(whole, fraction)    FIXED_2DP(whole, fraction)
#define RIDE_RATING_UNDEFINED           (ride_rating)(uint16)0xFFFF

#pragma pack(push, 1)

// Used for return values, for functions that modify all three.
typedef struct rating_tuple {
    ride_rating excitement;
    ride_rating intensity;
    ride_rating nausea;
} rating_tuple;
assert_struct_size(rating_tuple, 6);

#pragma pack(pop)

enum {
    RIDE_RATING_STATION_FLAG_NO_ENTRANCE = 1 << 0
};

typedef struct rct_ride_rating_calc_data {
    uint16  proximity_x;
    uint16  proximity_y;
    uint16  proximity_z;
    uint16  proximity_start_x;
    uint16  proximity_start_y;
    uint16  proximity_start_z;
    uint8   current_ride;
    uint8   state;
    uint8   proximity_track_type;
    uint8   proximity_base_height;
    uint16  proximity_total;
    uint16  proximity_scores[26];
    uint16  num_brakes;
    uint16  num_reversers;
    uint16  station_flags;
} rct_ride_rating_calc_data;

#ifdef __cplusplus
extern "C" {
#endif

extern rct_ride_rating_calc_data gRideRatingsCalcData;

void ride_ratings_update_ride(int rideIndex);
void ride_ratings_update_all();

#ifdef __cplusplus
}
#endif

#endif
