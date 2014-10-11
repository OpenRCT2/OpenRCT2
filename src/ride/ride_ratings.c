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
#include "../interface/window.h"
#include "../world/map.h"
#include "ride.h"
#include "ride_data.h"
#include "ride_ratings.h"

enum {
	RIDE_RATINGS_STATE_FIND_NEXT_RIDE,
	RIDE_RATINGS_STATE_INITIALISE,
	RIDE_RATINGS_STATE_2,
	RIDE_RATINGS_STATE_CALCULATE,
	RIDE_RATINGS_STATE_4,
	RIDE_RATINGS_STATE_5
};

typedef void (*ride_ratings_calculation)(rct_ride *ride);

#define _rideRatingsState				RCT2_GLOBAL(0x0138B591, uint8)
#define _rideRatingsCurrentRide			RCT2_GLOBAL(0x0138B590, uint8)

static const ride_ratings_calculation ride_ratings_calculate_func_table[91];

static void ride_ratings_update_state_0();
static void ride_ratings_update_state_1();
static void ride_ratings_update_state_2();
static void ride_ratings_update_state_3();
static void ride_ratings_update_state_4();
static void ride_ratings_update_state_5();
static void loc_6B5BB2();
static void ride_ratings_calculate(rct_ride *ride);
static void ride_ratings_reliability_calculate(rct_ride *ride);

static int sub_6C6402(rct_map_element *mapElement, int *x, int *y, int *z)
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

static int sub_6C60C2(rct_map_element *mapElement, int *x, int *y, int *z)
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
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR)
		return;

	switch (_rideRatingsState) {
	case RIDE_RATINGS_STATE_FIND_NEXT_RIDE:
		ride_ratings_update_state_0();
		break;
	case RIDE_RATINGS_STATE_INITIALISE:
		ride_ratings_update_state_1();
		break;
	case RIDE_RATINGS_STATE_2:
		ride_ratings_update_state_2();
		break;
	case RIDE_RATINGS_STATE_CALCULATE:
		ride_ratings_update_state_3();
		break;
	case RIDE_RATINGS_STATE_4:
		ride_ratings_update_state_4();
		break;
	case RIDE_RATINGS_STATE_5:
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
	_rideRatingsState = RIDE_RATINGS_STATE_2;
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
		_rideRatingsState = RIDE_RATINGS_STATE_CALCULATE;
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
	// sub_6C6402 returns a carry, CALLFUNC doesn't support this
	// so have to wait for sub_6C60C2 to be decompiled
	RCT2_CALLPROC_EBPSAFE(0x006B5C66);
	return;

	rct_ride *ride;
	rct_map_element *mapElement;
	int x, y, z, trackType, entranceIndex;

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

		if (trackType == 255 || (!(mapElement->properties.track.sequence & 0x0F) && trackType == mapElement->properties.track.type)) {
			if (trackType == 1) {
				entranceIndex = (mapElement->properties.track.sequence >> 4) & 7;
				RCT2_GLOBAL(0x0138B5CE, uint16) &= ~1;
				if (ride->entrances[entranceIndex] == 0xFFFF)
					RCT2_GLOBAL(0x0138B5CE, uint16) |= 1;
			}
			
			RCT2_CALLPROC_X(0x006B5F9D, 0, 0, 0, 0, (int)mapElement, 0, 0);

			x = RCT2_GLOBAL(0x0138B584, uint16);
			y = RCT2_GLOBAL(0x0138B586, uint16);
			if (!sub_6C60C2(mapElement, &x, &y, &z)) {
				_rideRatingsState = RIDE_RATINGS_STATE_4;
				return;
			}

			if (x == RCT2_GLOBAL(0x0138B58A, uint16) && y == RCT2_GLOBAL(0x0138B58C, uint16) && z == RCT2_GLOBAL(0x0138B58E, uint16)) {
				_rideRatingsState = RIDE_RATINGS_STATE_CALCULATE;
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
 *
 *  rct2: 0x006B5E4D
 */
static void ride_ratings_update_state_3()
{
	rct_ride *ride;

	ride = GET_RIDE(_rideRatingsCurrentRide);
	if (ride->type == RIDE_TYPE_NULL || ride->status == RIDE_STATUS_CLOSED) {
		_rideRatingsState = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
		return;
	}

	ride_ratings_calculate(ride);
	RCT2_CALLPROC_X(0x00655F64, 0, 0, 0, 0, 0, (int)ride, 0);
	ride_ratings_reliability_calculate(ride);

	window_invalidate_by_id(WC_RIDE, _rideRatingsCurrentRide);
	_rideRatingsState = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

/**
 *
 *  rct2: 0x006B5BAB
 */
static void ride_ratings_update_state_4()
{
	_rideRatingsState = RIDE_RATINGS_STATE_5;
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
				_rideRatingsState = RIDE_RATINGS_STATE_CALCULATE;
				return;
			}

			x >>= 16;
			y >>= 16;
			if (x == RCT2_GLOBAL(0x0138B58A, uint16) && y == RCT2_GLOBAL(0x0138B58C, uint16) && z == RCT2_GLOBAL(0x0138B58E, uint16)) {
				_rideRatingsState = RIDE_RATINGS_STATE_CALCULATE;
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

static void ride_ratings_calculate(rct_ride *ride)
{
	ride_ratings_calculation calcFunc;

	calcFunc = ride_ratings_calculate_func_table[ride->type];
	if (calcFunc == NULL) {
		calcFunc = RCT2_ADDRESS(0x0097E050, ride_ratings_calculation)[ride->type];
		RCT2_CALLPROC_X((int)calcFunc, 0, 0, 0, 0, 0, (int)ride, 0);
	} else {
		calcFunc(ride);
	}
}

static void ride_ratings_reliability_calculate(rct_ride *ride)
{
	rct_ride *ride2;
	int i, otherRidesOfSameType;

	if (ride->excitement == (ride_rating)0xFFFF)
		return;

	int reliability =
		(((ride->excitement * RCT2_GLOBAL(0x0097CD1E + (ride->type * 6), sint16)) * 32) >> 15) +
		(((ride->intensity  * RCT2_GLOBAL(0x0097CD20 + (ride->type * 6), sint16)) * 32) >> 15) +
		(((ride->nausea     * RCT2_GLOBAL(0x0097CD22 + (ride->type * 6), sint16)) * 32) >> 15);

	int monthsOld = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16) - ride->build_date;

	// New ride reward
	if (monthsOld <= 12) {
		reliability += 10;
		if (monthsOld <= 4)
			reliability += 20;
	}

	// Old ride penalty
	if (monthsOld >= 40) reliability -= reliability / 4;
	if (monthsOld >= 64) reliability -= reliability / 4;
	if (monthsOld < 200) {
		if (monthsOld >= 88) reliability -= reliability / 4;
		if (monthsOld >= 104) reliability -= reliability / 4;
		if (monthsOld >= 120) reliability -= reliability / 2;
		if (monthsOld >= 128) reliability -= reliability / 2;
	}

	// Other ride of same type penalty
	otherRidesOfSameType = 0;
	FOR_ALL_RIDES(i, ride2) {
		if (ride2->type == ride->type)
			otherRidesOfSameType++;
	}
	if (otherRidesOfSameType > 1)
		reliability -= reliability / 4;

	ride->reliability = max(0, reliability);
}

/**
 * rct2: sub_65E621
 *
 * I think this function computes ride upkeep? Though it is weird that the
 *
 * inputs
 * - edi: ride ptr
 */
static uint16 ride_compute_upkeep(rct_ride *ride)
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
static void ride_ratings_apply_adjustments(rct_ride *ride, rating_tuple *ratings)
{
	rct_ride_type *rideEntry;
	
	rideEntry = gRideTypeList[ride->subtype];

	// Apply ride entry multipliers
	ratings->excitement += ((ratings->excitement * rideEntry->excitement_multipler) >> 7);
	ratings->intensity +=  ((ratings->intensity  * rideEntry->intensity_multipler ) >> 7);
	ratings->nausea +=     ((ratings->nausea     * rideEntry->nausea_multipler    ) >> 7);

	// As far as I can tell, this flag detects whether the ride is a roller
	// coaster, or a log flume or rapids. Everything else it's not set.
	// more detail: https://gist.github.com/kevinburke/d951e74e678b235eef3e
	uint16 flags = RCT2_GLOBAL(0x0097D4F2 + ride->type * 8, uint16);
	if (flags & 0x80) {
		uint16 totalAirTime = ride->total_air_time;
		if (rideEntry->var_008 & 0x800) {
			totalAirTime -= 96;
			if (totalAirTime >= 0) {
				ratings->excitement -= totalAirTime / 8;
				ratings->nausea -= totalAirTime / 16;
			}
		} else {
			ratings->excitement += totalAirTime / 8;
			ratings->nausea += totalAirTime / 16;
		}
	}
}

/**
 * Lowers excitment, the higher the intensity.
 *  rct2: 0x0065E7A3
 */
static void ride_ratings_apply_intensity_penalty(rating_tuple *ratings)
{
	static const ride_rating intensityBounds[] = { 1000, 1100, 1200, 1320, 1450 };
	int i;

	ride_rating excitement = ratings->excitement;
	for (i = 0; i < countof(intensityBounds); i++)
		if (ratings->intensity >= intensityBounds[i])
			excitement -= excitement / 4;
	ratings->excitement = excitement;
}

/**
 *
 *  rct2: 0x00655FD6
 */
static void sub_655FD6(rct_ride *ride)
{
    ride->var_198 += (ride->lift_hill_speed - RCT2_ADDRESS(0x0097D7C9, uint8)[ride->type * 4]) * 2;
}

/**
 *
 *  rct2: 0x0065E277
 */
static int sub_65E277()
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	RCT2_CALLFUNC_X(0x0065E277, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return ebx;
}

/**
 *
 *  rct2: 0x0065E72D
 */
static int sub_65E72D(rct_ride *ride)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	edi = (int)ride;
	RCT2_CALLFUNC_X(0x0065E277, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return edx & 0xFFFF;
}

/**
 *
 *  rct2: 0x0065DDD1
 */
static rating_tuple sub_65DDD1(rct_ride *ride)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	edi = (int)ride;
	RCT2_CALLFUNC_X(0x0065DDD1, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	rating_tuple rating = { ebx, ecx, ebp };
	return rating;
}

/**
 *
 *  rct2: 0x0065E1C2
 */
static rating_tuple sub_65E1C2(rct_ride *ride)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	edi = (int)ride;
	RCT2_CALLFUNC_X(0x0065E1C2, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	rating_tuple rating = { ebx, ecx, ebp };
	return rating;
}

/**
 *
 *  rct2: 0x0065DCDC
 */
static rating_tuple ride_ratings_get_gforce_ratings(rct_ride *ride)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	edi = (int)ride;
	RCT2_CALLFUNC_X(0x0065DCDC, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	rating_tuple rating = { ebx, ecx, ebp };
	return rating;
}

/**
 *
 *  rct2: 0x0065E139
 */
static rating_tuple ride_ratings_get_drop_ratings(rct_ride *ride)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	edi = (int)ride;
	RCT2_CALLFUNC_X(0x0065E139, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	rating_tuple rating = { ebx, ecx, ebp };
	return rating;
}

/**
 * Calculates a score based on the surrounding scenery.
 *  rct2: 0x0065E557
 */
static int ride_ratings_get_scenery_score(rct_ride *ride)
{
	int i, x, y, z, xx, yy, type, numSceneryItems;
	uint16 stationXY;
	rct_map_element *mapElement;

	for (i = 0; i < 4; i++) {
		stationXY = ride->station_starts[i];
		if (stationXY != 0xFFFF)
			break;
	}
	if (i == 4)
		return 0;

	if (ride->type == RIDE_TYPE_MAZE)
		stationXY = ride->entrances[0];

	x = stationXY & 0xFF;
	y = stationXY >> 8;
	z = map_element_height(x * 32, y * 32);

	// Check if station is underground, returns a fixed mediocre score since you can't have scenery underground
	if (z > ride->station_heights[i] * 8)
		return 40;

	// Count surrounding scenery items
	numSceneryItems = 0;
	for (yy = y - 5; yy <= y + 5; yy++) {
		for (xx = x - 5; xx <= x + 5; xx++) {
			// Count scenery items on this tile
			mapElement = TILE_MAP_ELEMENT_POINTER(yy * 256 + xx);
			do {
				if (mapElement->flags & 0x10)
					continue;

				type = mapElement->type & MAP_ELEMENT_TYPE_MASK;
				if (type == MAP_ELEMENT_TYPE_SCENERY || type == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE)
					numSceneryItems++;
			} while (!((mapElement++)->flags & MAP_ELEMENT_FLAG_LAST_TILE));
		}
	}

	return min(numSceneryItems, 47) * 5;
}

#pragma region Ride rating calculation functions

static void ride_ratings_calculate_mine_train_coaster(rct_ride *ride)
{
	rating_tuple ratings, unkRating;
	int totalLength, time;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->var_198 = 16;
	sub_655FD6(ride);

	// Base ratings
	ratings.excitement =	RIDE_RATING(2,90);
	ratings.intensity =		RIDE_RATING(2,30);
	ratings.nausea =		RIDE_RATING(2,10);

	// Apply length of ride factor
	totalLength = (ride->length[0] + ride->length[1] + ride->length[2] + ride->length[3]) >> 16;
	ratings.excitement += (min(6000, totalLength) * 764) >> 16;

	// Apply racing coaster factor
	if (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS) {
		ratings.excitement += RIDE_RATING(0,40);
		ratings.intensity += RIDE_RATING(0,05);
	}

	// Apply length of train factor
	ratings.excitement += ((ride->num_cars_per_train - 1) * 187245) >> 16;

	// Apply maximum speed factor
	ratings.excitement += ((ride->max_speed >> 16) * 44281) >> 16;
	ratings.intensity += ((ride->max_speed >> 16) * 88562) >> 16;
	ratings.nausea += ((ride->max_speed >> 16) * 35424) >> 16;

	// Apply average speed factor
	ratings.excitement += ((ride->average_speed >> 16) * 291271) >> 16;
	ratings.intensity += ((ride->average_speed >> 16) * 436906) >> 16;
	
	// Apply ride duration factor
	time = ride->time[0] + ride->time[1] + ride->time[2] + ride->time[3];
	ratings.excitement += (min(150, time) * 26214) >> 16;

	// Apply G forces factor
	unkRating = ride_ratings_get_gforce_ratings(ride);
	ratings.excitement += (unkRating.excitement * 40960) >> 16;
	ratings.intensity += (unkRating.intensity * 35746) >> 16;
	ratings.nausea += (unkRating.nausea * 49648) >> 16;

	// Apply ?
	unkRating = sub_65DDD1(ride);
	ratings.excitement += (unkRating.excitement * 29721) >> 16;
	ratings.intensity += (unkRating.intensity * 34767) >> 16;
	ratings.nausea += (unkRating.nausea * 45749) >> 16;

	// Apply drops factor
	unkRating = ride_ratings_get_drop_ratings(ride);
	ratings.excitement += (unkRating.excitement * 29127) >> 16;
	ratings.intensity += (unkRating.intensity * 46811) >> 16;
	ratings.nausea += (unkRating.nausea * 49152) >> 16;

	// Apply ?
	unkRating = sub_65E1C2(ride);
	ratings.excitement += (unkRating.excitement * 19275) >> 16;
	ratings.intensity += (unkRating.intensity * 32768) >> 16;
	ratings.nausea += (unkRating.nausea * 35108) >> 16;

	// Apply ?
	ratings.excitement += (sub_65E277() * 21472) >> 16;

	ratings.excitement += (ride_ratings_get_scenery_score(ride) * 16732) >> 16;

	// Apply low highest drop penalty
	if (ride->highest_drop_height < 8) {
		ride->excitement /= 2;
		ride->intensity /= 2;
		ride->nausea /= 2;
	}

	// Apply low max speed penalty
	if (ride->max_speed < 0xA0000) {
		ride->excitement /= 2;
		ride->intensity /= 2;
		ride->nausea /= 2;
	}

	// Apply low maximum negative vertical G force penalty
	if (ride->max_negative_vertical_g > FIXED_2DP(0,10)) {
		ride->excitement /= 2;
		ride->intensity /= 2;
		ride->nausea /= 2;
	}

	// Apply short ride penalty
	if (ride->length[0] < 0x1720000) {
		ride->excitement /= 2;
		ride->intensity /= 2;
		ride->nausea /= 2;
	}

	// Apply low number of drops penalty
	if ((ride->drops & 0x3F) < 2) {
		ride->excitement /= 2;
		ride->intensity /= 2;
		ride->nausea /= 2;
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_maze(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->var_198 = 8;
	sub_655FD6(ride);

	// Base ratings
	ratings.excitement =	RIDE_RATING(1,30);
	ratings.intensity =		RIDE_RATING(0,50);
	ratings.nausea =		RIDE_RATING(0,00);

	// Apply size factor
	int unk = min(ride->maze_tiles, 100);
	ratings.excitement += unk;
	ratings.intensity += unk / 2;
	
	ratings.excitement += (ride_ratings_get_scenery_score(ride) * 22310) >> 16;

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_food_stall(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;
}

static void ride_ratings_calculate_drink_stall(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;
}

static void ride_ratings_calculate_shop(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;
}

static void ride_ratings_calculate_merry_go_round(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->var_198 = 16;
	sub_655FD6(ride);

	int unk = ride->var_0D0 * 5;
	ratings.excitement	= unk + RIDE_RATING(0,60) + ((ride_ratings_get_scenery_score(ride) * 19521) >> 16);
	ratings.intensity	= unk + RIDE_RATING(0,15);
	ratings.nausea		= unk + RIDE_RATING(0,30);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;
}

static void ride_ratings_calculate_information_kiosk(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;
}

static void ride_ratings_calculate_bathroom(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;
}

static void ride_ratings_calculate_ferris_wheel(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->var_198 = 16;
	sub_655FD6(ride);

	int unk = ride->var_0D0 * 25;
	ratings.excitement	= unk + RIDE_RATING(0,60) + ((ride_ratings_get_scenery_score(ride) * 41831) >> 16);
	ratings.intensity	= unk + RIDE_RATING(0,25);
	ratings.nausea		= unk + RIDE_RATING(0,30);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_elevator(rct_ride *ride)
{
	rating_tuple ratings;
	int totalLength;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->var_198 = 15;
	sub_655FD6(ride);

	// Base ratings
	ratings.excitement	= RIDE_RATING(1,11);
	ratings.intensity	= RIDE_RATING(0,35);
	ratings.nausea		= RIDE_RATING(0,30);

	// Apply length factor
	totalLength = ride->length[0] + ride->length[1] + ride->length[2] + ride->length[3];
	ratings.excitement += ((totalLength >> 16) * 45875) >> 16;
	ratings.excitement += (sub_65E277() * 11183) >> 16;
	ratings.excitement += (ride_ratings_get_scenery_score(ride) * 83662) >> 16;
	ratings.nausea += ((totalLength >> 16) * 26214) >> 16;

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;

	if ((sub_65E72D(ride) >> 8) >= 5)
		ride->excitement /= 4;
}

static void ride_ratings_calculate_haunted_house(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->var_198 = 8;
	sub_655FD6(ride);

	ratings.excitement	= RIDE_RATING(3,41);
	ratings.intensity	= RIDE_RATING(1,53);
	ratings.nausea		= RIDE_RATING(0,10);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;

	ride->inversions &= 0x1F;
	ride->inversions |= 0xE0;
}

static void ride_ratings_calculate_mini_golf(rct_ride *ride)
{
	rating_tuple ratings, unkRating;

	// RCT2_CALLPROC_X(0x0065BF97, 0, 0, 0, 0, 0, (int)ride, 0); return;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->var_198 = 0;
	sub_655FD6(ride);

	// Base ratings
	ratings.excitement =	RIDE_RATING(1,50);
	ratings.intensity =		RIDE_RATING(0,90);
	ratings.nausea =		RIDE_RATING(0,00);

	// Apply length factor
	int length = (ride->length[0] + ride->length[1] + ride->length[2] + ride->length[3]) >> 16;
	ratings.excitement += (min(6000, length) * 873) >> 16;

	// Apply ?
	unkRating = sub_65DDD1(ride);
	ratings.excitement += (unkRating.excitement * 14860) >> 16;

	// Apply ?
	unkRating = sub_65E1C2(ride);
	ratings.excitement += (unkRating.excitement * 5140) >> 16;
	ratings.intensity += (unkRating.intensity * 6553) >> 16;
	ratings.nausea += (unkRating.nausea * 4681) >> 16;

	// Apply ?
	ratings.excitement += (sub_65E277() * 15657) >> 16;

	ratings.excitement += (ride_ratings_get_scenery_score(ride) * 27887) >> 16;

	// Apply golf holes factor
	ratings.excitement += (ride->holes & 0x1F) * 5;

	// Apply no golf holes penalty
	if ((ride->inversions & 0x1F) == 0) {
		ratings.excitement /= 8;
		ratings.intensity /= 2;
		ratings.nausea /= 2;
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_first_aid(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;
}

static void ride_ratings_calculate_crooked_house(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->var_198 = 5;
	sub_655FD6(ride);

	ratings.excitement	= RIDE_RATING(2,15);
	ratings.intensity	= RIDE_RATING(0,62);
	ratings.nausea		= RIDE_RATING(0,34);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->var_14D |= 2;

	ride->inversions &= 0x1F;
	ride->inversions |= 0xE0;
}

#pragma endregion

#pragma region Ride rating calculation function table

// rct2: 0x0097E050
static const ride_ratings_calculation ride_ratings_calculate_func_table[91] = {
	NULL,											// SPIRAL_ROLLER_COASTER
	NULL,											// STAND_UP_ROLLER_COASTER
	NULL,											// SUSPENDED_SWINGING_COASTER
	NULL,											// INVERTED_ROLLER_COASTER
	NULL,											// JUNIOR_ROLLER_COASTER
	NULL,											// MINIATURE_RAILWAY
	NULL,											// MONORAIL
	NULL,											// MINI_SUSPENDED_COASTER
	NULL,											// BUMPER_BOATS
	NULL,											// WOODEN_WILD_MOUSE
	NULL,											// STEEPLECHASE
	NULL,											// CAR_RIDE
	NULL,											// LAUNCHED_FREEFALL
	NULL,											// BOBSLEIGH_COASTER
	NULL,											// OBSERVATION_TOWER
	NULL,											// LOOPING_ROLLER_COASTER
	NULL,											// DINGHY_SLIDE
	ride_ratings_calculate_mine_train_coaster,		// MINE_TRAIN_COASTER
	NULL,											// CHAIRLIFT
	NULL,											// CORKSCREW_ROLLER_COASTER
	ride_ratings_calculate_maze,					// MAZE
	NULL,											// SPIRAL_SLIDE
	NULL,											// GO_KARTS
	NULL,											// LOG_FLUME
	NULL,											// RIVER_RAPIDS
	NULL,											// BUMPER_CARS
	NULL,											// PIRATE_SHIP
	NULL,											// SWINGING_INVERTER_SHIP
	ride_ratings_calculate_food_stall,				// FOOD_STALL
	NULL,											// 1D
	ride_ratings_calculate_drink_stall,				// DRINK_STALL
	NULL,											// 1F
	ride_ratings_calculate_shop,					// SHOP
	ride_ratings_calculate_merry_go_round,			// MERRY_GO_ROUND
	NULL,											// 22
	ride_ratings_calculate_information_kiosk,		// INFORMATION_KIOSK
	ride_ratings_calculate_bathroom,				// BATHROOM
	ride_ratings_calculate_ferris_wheel,			// FERRIS_WHEEL
	NULL,											// MOTION_SIMULATOR
	NULL,											// 3D_CINEMA
	NULL,											// TOP_SPIN
	NULL,											// SPACE_RINGS
	NULL,											// REVERSE_FREEFALL_COASTER
	ride_ratings_calculate_elevator,				// ELEVATOR
	NULL,											// VERTICAL_DROP_ROLLER_COASTER
	NULL,											// ATM
	NULL,											// TWIST
	ride_ratings_calculate_haunted_house,			// HAUNTED_HOUSE
	ride_ratings_calculate_first_aid,				// FIRST_AID
	NULL,											// CIRCUS_SHOW
	NULL,											// GHOST_TRAIN
	NULL,											// TWISTER_ROLLER_COASTER
	NULL,											// WOODEN_ROLLER_COASTER
	NULL,											// SIDE_FRICTION_ROLLER_COASTER
	NULL,											// WILD_MOUSE
	NULL,											// MULTI_DIMENSION_ROLLER_COASTER
	NULL,											// 38
	NULL,											// FLYING_ROLLER_COASTER
	NULL,											// 3A
	NULL,											// VIRGINIA_REEL
	NULL,											// SPLASH_BOATS
	NULL,											// MINI_HELICOPTERS
	NULL,											// LAY_DOWN_ROLLER_COASTER
	NULL,											// SUSPENDED_MONORAIL
	NULL,											// 40
	NULL,											// REVERSER_ROLLER_COASTER
	NULL,											// HEARTLINE_TWISTER_COASTER
	ride_ratings_calculate_mini_golf,				// MINI_GOLF
	NULL,											// GIGA_COASTER
	NULL,											// ROTO_DROP
	NULL,											// FLYING_SAUCERS
	ride_ratings_calculate_crooked_house,			// CROOKED_HOUSE
	NULL,											// MONORAIL_CYCLES
	NULL,											// COMPACT_INVERTED_COASTER
	NULL,											// WATER_COASTER
	NULL,											// AIR_POWERED_VERTICAL_COASTER
	NULL,											// INVERTED_HAIRPIN_COASTER
	NULL,											// MAGIC_CARPET
	NULL,											// SUBMARINE_RIDE
	NULL,											// RIVER_RAFTS
	NULL,											// 50
	NULL,											// ENTERPRISE
	NULL,											// 52
	NULL,											// 53
	NULL,											// 54
	NULL,											// 55
	NULL,											// INVERTED_IMPULSE_COASTER
	NULL,											// MINI_ROLLER_COASTER
	NULL,											// MINE_RIDE
	NULL,											// LIM_LAUNCHED_ROLLER_COASTER
	NULL,											// 90
};

#pragma endregion