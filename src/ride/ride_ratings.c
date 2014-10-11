/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "../addresses.h"
#include "../world/map.h"
#include "ride.h"
#include "ride_data.h"
#include "ride_ratings.h"

enum {
	RIDE_RATINGS_STATE_FIND_NEXT_RIDE,
	RIDE_RATINGS_STATE_INITIALISE
};

#define _rideRatingsState				RCT2_GLOBAL(0x0138B591, uint8)
#define _rideRatingsCurrentRide			RCT2_GLOBAL(0x00138B590, uint8)

static void ride_ratings_update_state_0();
static void ride_ratings_update_state_1();
static void ride_ratings_update_state_2();
static void ride_ratings_update_state_3();
static void ride_ratings_update_state_4();
static void ride_ratings_update_state_5();
static void loc_6B5BB2();

int sub_6C6402(rct_map_element *mapElement, int *x, int *y, int *z)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;

	eax = *x;
	ecx = *y;
	esi = (int)mapElement;
	RCT2_CALLFUNC_X(0x006C6402, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	*x = *((uint16*)&eax);
	*y = *((uint16*)&ecx);
	*z = *((uint8*)&edx);
	return 1;
}

/**
 *
 *  rct2: 0x006B5A2A
 */
void ride_ratings_update_all()
{
	// RCT2_CALLPROC_EBPSAFE(0x006B5A2A);

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR)
		return;

	switch (_rideRatingsState) {
	case RIDE_RATINGS_STATE_FIND_NEXT_RIDE:
		ride_ratings_update_state_0();
		break;
	case RIDE_RATINGS_STATE_INITIALISE:
		ride_ratings_update_state_1();
		break;
	case 2:
		ride_ratings_update_state_2();
		break;
	case 3:
		ride_ratings_update_state_3();
		break;
	case 4:
		ride_ratings_update_state_4();
		break;
	case 5:
		ride_ratings_update_state_5();
		break;
	}
}

/**
 *
 *  rct2: 0x006B5A5C
 */
static void ride_ratings_update_state_0()
{
	rct_ride *ride;

	_rideRatingsCurrentRide += 1;
	if (_rideRatingsCurrentRide == 255)
		_rideRatingsCurrentRide = 0;

	ride = GET_RIDE(_rideRatingsCurrentRide);
	if (ride->type != RIDE_TYPE_NULL && ride->status != RIDE_STATUS_CLOSED)
		_rideRatingsState = RIDE_RATINGS_STATE_INITIALISE;
}

/**
 *
 *  rct2: 0x006B5A94
 */
static void ride_ratings_update_state_1()
{
	RCT2_GLOBAL(0x0138B594, uint16) = 0;
	RCT2_GLOBAL(0x0138B596, uint16) = 0;
	RCT2_GLOBAL(0x0138B598, uint16) = 0;
	RCT2_GLOBAL(0x0138B59A, uint16) = 0;
	RCT2_GLOBAL(0x0138B59C, uint16) = 0;
	RCT2_GLOBAL(0x0138B59E, uint16) = 0;
	RCT2_GLOBAL(0x0138B5A0, uint16) = 0;
	RCT2_GLOBAL(0x0138B5A2, uint16) = 0;
	RCT2_GLOBAL(0x0138B5A4, uint16) = 0;
	RCT2_GLOBAL(0x0138B5A6, uint16) = 0;
	RCT2_GLOBAL(0x0138B5A8, uint16) = 0;
	RCT2_GLOBAL(0x0138B5AA, uint16) = 0;
	RCT2_GLOBAL(0x0138B5AC, uint16) = 0;
	RCT2_GLOBAL(0x0138B5AE, uint16) = 0;
	RCT2_GLOBAL(0x0138B5B0, uint16) = 0;
	RCT2_GLOBAL(0x0138B5B2, uint16) = 0;
	RCT2_GLOBAL(0x0138B5B4, uint16) = 0;
	RCT2_GLOBAL(0x0138B5B6, uint16) = 0;
	RCT2_GLOBAL(0x0138B5B8, uint16) = 0;
	RCT2_GLOBAL(0x0138B5BA, uint16) = 0;
	RCT2_GLOBAL(0x0138B5BC, uint16) = 0;
	RCT2_GLOBAL(0x0138B5BE, uint16) = 0;
	RCT2_GLOBAL(0x0138B5C0, uint16) = 0;
	RCT2_GLOBAL(0x0138B5C2, uint16) = 0;
	RCT2_GLOBAL(0x0138B5C4, uint16) = 0;
	RCT2_GLOBAL(0x0138B5C6, uint16) = 0;
	RCT2_GLOBAL(0x0138B5C8, uint16) = 0;
	RCT2_GLOBAL(0x0138B5CA, uint16) = 0;
	RCT2_GLOBAL(0x0138B5CC, uint16) = 0;
	_rideRatingsState = 2;
	RCT2_GLOBAL(0x0138B5CE, uint16) = 0;
	loc_6B5BB2();
}

/**
 *
 *  rct2: 0x006B5BB2
 */
static void loc_6B5BB2()
{
	rct_ride *ride;
	int i, x, y, z;

	ride = GET_RIDE(_rideRatingsCurrentRide);
	if (ride->type == RIDE_TYPE_NULL || ride->status == RIDE_STATUS_CLOSED) {
		_rideRatingsState = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
		return;
	}

	if (ride->type == RIDE_TYPE_MAZE) {
		_rideRatingsState = 3;
		return;
	}

	for (i = 0; i < 4; i++) {
		if (ride->station_starts[i] != 0xFFFF) {
			RCT2_GLOBAL(0x0138B5CE, uint16) &= ~1;
			if (ride->entrances[i] == 0xFFFF)
				RCT2_GLOBAL(0x0138B5CE, uint16) |= 1;

			x = (ride->station_starts[i] & 0xFF) * 32;
			y = (ride->station_starts[i] >> 8) * 32;
			z = ride->station_heights[i] * 8;

			RCT2_GLOBAL(0x0138B584, uint16) = x;
			RCT2_GLOBAL(0x0138B586, uint16) = y;
			RCT2_GLOBAL(0x0138B588, uint16) = z;
			RCT2_GLOBAL(0x0138B592, uint8) = 255;
			RCT2_GLOBAL(0x0138B58A, uint16) = x;
			RCT2_GLOBAL(0x0138B58C, uint16) = y;
			RCT2_GLOBAL(0x0138B58E, uint16) = z;
			return;
		}
	}

	_rideRatingsState = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

/**
 *
 *  rct2: 0x006B5C66
 */
static void ride_ratings_update_state_2()
{
	RCT2_CALLPROC_EBPSAFE(0x006B5C66);
}

/**
 *
 *  rct2: 0x006B5E4D
 */
static void ride_ratings_update_state_3()
{
	RCT2_CALLPROC_EBPSAFE(0x006B5E4D);
}

/**
 *
 *  rct2: 0x006B5BAB
 */
static void ride_ratings_update_state_4()
{
	_rideRatingsState = 5;
	loc_6B5BB2();
}

/**
 *
 *  rct2: 0x006B5D72
 */
static void ride_ratings_update_state_5()
{
	// sub_6C6402 returns a carry, CALLFUNC doesn't support this
	// so have to wait for sub_6C6402 to be decompiled
	RCT2_CALLPROC_EBPSAFE(0x006B5D72);
	return;

	rct_ride *ride;
	rct_map_element *mapElement;
	int x, y, z, trackType;

	ride = GET_RIDE(_rideRatingsCurrentRide);
	if (ride->type == RIDE_TYPE_NULL || ride->status == RIDE_STATUS_CLOSED) {
		_rideRatingsState = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
		return;
	}

	x = RCT2_GLOBAL(0x0138B584, uint16) / 32;
	y = RCT2_GLOBAL(0x0138B586, uint16) / 32;
	z = RCT2_GLOBAL(0x0138B588, uint16) / 8;

	mapElement = TILE_MAP_ELEMENT_POINTER(y * 256 + x);
	trackType = RCT2_GLOBAL(0x0138B592, uint8);

	do {
		if ((mapElement->type & MAP_ELEMENT_TYPE_MASK) != MAP_ELEMENT_TYPE_TRACK)
			continue;
		if (mapElement->base_height != z)
			continue;

		if (trackType == 255 || trackType == mapElement->properties.track.type) {
			RCT2_CALLPROC_X(0x006B5F9D, 0, 0, 0, 0, (int)mapElement, 0, 0);

			x = RCT2_GLOBAL(0x0138B584, uint16);
			y = RCT2_GLOBAL(0x0138B586, uint16);
			if (!sub_6C6402(mapElement, &x, &y, &z)) {
				_rideRatingsState = 3;
				return;
			}

			x >>= 16;
			y >>= 16;
			if (x == RCT2_GLOBAL(0x0138B58A, uint16) && y == RCT2_GLOBAL(0x0138B58C, uint16) && z == RCT2_GLOBAL(0x0138B58E, uint16)) {
				_rideRatingsState = 3;
				return;
			}
			RCT2_GLOBAL(0x0138B584, uint16) = x;
			RCT2_GLOBAL(0x0138B586, uint16) = y;
			RCT2_GLOBAL(0x0138B588, uint16) = z;
			RCT2_GLOBAL(0x0138B592, uint8) = mapElement->properties.track.type;
		}
	} while (!((mapElement++)->flags & MAP_ELEMENT_FLAG_LAST_TILE));

	_rideRatingsState = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

/**
 * rct2: 0x0065C4D4
 *
 * Compute excitement, intensity, etc. for a crooked house ride.
 */
void crooked_house_excitement(rct_ride *ride)
{
	// Set lifecycle bits
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->var_198 = 5;
	sub_655FD6(ride);

	ride_rating excitement	= RIDE_RATING(2,15);
	ride_rating intensity	= RIDE_RATING(0,62);
	ride_rating nausea		= RIDE_RATING(0,34);

	// NB this should get marked out by the compiler, if it's smart.
	excitement = apply_intensity_penalty(excitement, intensity);
	rating_tuple tup = per_ride_rating_adjustments(ride, excitement, intensity, nausea);
	excitement = tup.excitement;
	intensity = tup.intensity;
	nausea = tup.nausea;

	ride->excitement = excitement;
	ride->intensity = intensity;
	ride->nausea = nausea;

	ride->upkeep_cost = compute_upkeep(ride);
	// Upkeep flag? or a dirtiness flag
	ride->var_14D |= 2;

	// clear all bits except lowest 5
	ride->inversions &= 0x1F;
	// set 6th,7th,8th bits
	ride->inversions |= 0xE0;
}

/**
 * rct2: sub_65E621
 *
 * I think this function computes ride upkeep? Though it is weird that the
 *
 * inputs
 * - edi: ride ptr
 */
uint16 compute_upkeep(rct_ride *ride)
{
	// data stored at 0x0057E3A8, incrementing 18 bytes at a time
	uint16 upkeep = initialUpkeepCosts[ride->type];

	uint16 trackCost = costPerTrackPiece[ride->type];
	uint8 dl = ride->drops;

	dl = dl >> 6;
	dl = dl & 3;
	upkeep += trackCost * dl;

	uint32 totalLength = (ride->length[0] + ride->length[1] + ride->length[2] + ride->length[3]) >> 16;

	// The data originally here was 20's and 0's. The 20's all represented
	// rides that had tracks. The 0's were fixed rides like crooked house or
	// bumper cars.
	// Data source is 0x0097E3AC
	if (hasRunningTrack[ride->type]) {
		totalLength *= 20;
	}
	upkeep += (uint16)(totalLength >> 10);

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) {
		// The original code read from a table starting at 0x0097E3AE and
		// incrementing by 0x12 bytes between values. However, all of these
		// values were 40. I have replaced the table lookup with the constant
		// 40 in this case.
		upkeep += 40;
	}

	// Originally this data was at 0x0097E3B0 and incrementing in 18 byte
	// offsets. The value here for every ride is 80, except for the reverser,
	// which is 10
	uint16 eax;
	if (ride->type == RIDE_TYPE_REVERSER_ROLLER_COASTER) {
		eax = 10;
	} else {
		eax = 80;
	}

	// not sure what this value is; it's only written to in one place, where
	// it's incremented.
	sint16 dx = RCT2_GLOBAL(0x0138B5CC, sint16);
	upkeep += eax * dx;

	dx = RCT2_GLOBAL(0x0138B5CA, sint16);
	// Originally there was a lookup into a table at 0x0097E3B0 and
	// incrementing in 18 byte offsets. The value here for every ride was 20,
	// so it's been replaced here by the constant.
	upkeep += 20 * dx;

	// these seem to be adhoc adjustments to a ride's upkeep/cost, times
	// various variables set on the ride itself.

	// https://gist.github.com/kevinburke/e19b803cd2769d96c540
	upkeep += rideUnknownData1[ride->type] * ride->num_vehicles;

	// either set to 3 or 0, extra boosts for some rides including mini golf
	if (rideUnknownData2[ride->type]) {
		upkeep += 3 * ride->num_cars_per_train;
	}

	// slight upkeep boosts for some rides - 5 for mini railroad, 10 for log
	// flume/rapids, 10 for roller coaster, 28 for giga coaster
	upkeep += rideUnknownData3[ride->type] * ride->num_stations;

	if (ride->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE) {
		upkeep += 30;
	} else if (ride->mode == RIDE_MODE_POWERED_LAUNCH) {
		upkeep += 160;
	} else if (ride->mode == RIDE_MODE_LIM_POWERED_LAUNCH) {
		upkeep += 320;
	} else if (ride->mode == RIDE_MODE_POWERED_LAUNCH_35 || 
			ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED) {
		upkeep += 220;
	}

	// multiply by 5/8
	upkeep = upkeep * 10;
	upkeep = upkeep >> 4;
    return upkeep;
}

/**
 * rct2: 0x0065E7FB
 *
 * inputs
 * - bx: excitement
 * - cx: intensity
 * - bp: nausea
 * - edi: ride ptr
 */
rating_tuple per_ride_rating_adjustments(rct_ride *ride, ride_rating excitement, 
		ride_rating intensity, ride_rating nausea)
{
	// NB: The table here is allocated dynamically. Reading the exe will tell
	// you nothing
	rct_ride_type *rideType = gRideTypeList[ride->subtype];

	// example value here: 12 (?)
	excitement = excitement + ((excitement * rideType->excitement_multipler) >> 7);
	intensity = intensity + ((intensity * rideType->intensity_multipler) >> 7);
	nausea = nausea + ((nausea * rideType->nausea_multipler) >> 7);

	// As far as I can tell, this flag detects whether the ride is a roller
	// coaster, or a log flume or rapids. Everything else it's not set.
	// more detail: https://gist.github.com/kevinburke/d951e74e678b235eef3e
	uint16 ridetype_var = RCT2_GLOBAL(0x0097D4F2 + ride->type * 8, uint16);
	if (ridetype_var & 0x80) {
		uint16 ax = ride->total_air_time;
		if (rideType->var_008 & 0x800) {
			// 65e86e
			ax = ax - 96;
			if (ax >= 0) {
				ax = ax >> 3;
				excitement = excitement - ax;
				ax = ax >> 1;
				nausea = nausea - ax;
			}
		} else {
			ax = ax >> 3;
			excitement = excitement + ax;
			ax = ax >> 1;
			nausea += ax;
		}
	}
	rating_tuple tup = { excitement, intensity, nausea };
	return tup;
}

/**
 * rct2: 0x0065E7A3
 *
 * inputs from x86
 * - bx: excitement
 * - cx: intensity
 * - bp: nausea
 *
 * returns: the excitement level, with intensity penalties applied
 */
ride_rating apply_intensity_penalty(ride_rating excitement, ride_rating intensity)
{
    // intensity penalty
    if (intensity >= 1000) {
        excitement = excitement - (excitement >> 2);
    }
    if (intensity >= 1100) {
        excitement = excitement - (excitement >> 2);
    }
    if (intensity >= 1200) {
        excitement = excitement - (excitement >> 2);
    }
    if (intensity >= 1320) {
        excitement = excitement - (excitement >> 2);
    }
    if (intensity >= 1450) {
        excitement = excitement - (excitement >> 2);
    }
    return excitement;
}

/**
 * rct2: 0x00655FD6
 *
 * Take ride property 1CD, make some modifications, store the modified value in
 * property 198.
 */
void sub_655FD6(rct_ride *ride)
{
    uint8 al = ride->lift_hill_speed;
    // No idea what this address is; maybe like compensation of some kind? The
    // maximum possible value?
    // List of ride names/values is here: 
    // https://gist.github.com/kevinburke/5eebcda14d94e6ee99c0
    al -= RCT2_ADDRESS(0x0097D7C9, uint8)[4 * ride->type];
    al = al << 1;
    ride->var_198 += al;
}
