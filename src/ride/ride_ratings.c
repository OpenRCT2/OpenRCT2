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
static void ride_ratings_calculate_value(rct_ride *ride);

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
	// TODO test this function
	RCT2_CALLPROC_EBPSAFE(0x006B5C66);
	return;

	rct_ride *ride;
	rct_map_element *mapElement;
	rct_xy_element trackElement, nextTrackElement;
	int x, y, z, trackType, entranceIndex;

	ride = GET_RIDE(_rideRatingsCurrentRide);
	if (ride->type == RIDE_TYPE_NULL || ride->status == RIDE_STATUS_CLOSED) {
		_rideRatingsState = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
		return;
	}

	x = RCT2_GLOBAL(0x0138B584, uint16) / 32;
	y = RCT2_GLOBAL(0x0138B586, uint16) / 32;
	z = RCT2_GLOBAL(0x0138B588, uint16) / 8;

	mapElement = map_get_first_element_at(x, y);
	trackType = RCT2_GLOBAL(0x0138B592, uint8);

	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
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

			trackElement.x = RCT2_GLOBAL(0x0138B584, uint16);
			trackElement.y = RCT2_GLOBAL(0x0138B586, uint16);
			trackElement.element = mapElement;
			if (!track_get_next(&trackElement, &nextTrackElement)) {
				_rideRatingsState = RIDE_RATINGS_STATE_4;
				return;
			}

			x = nextTrackElement.x;
			y = nextTrackElement.y;
			z = nextTrackElement.element->base_height * 8;
			mapElement = nextTrackElement.element;
			if (x == RCT2_GLOBAL(0x0138B58A, uint16) && y == RCT2_GLOBAL(0x0138B58C, uint16) && z == RCT2_GLOBAL(0x0138B58E, uint16)) {
				_rideRatingsState = RIDE_RATINGS_STATE_CALCULATE;
				return;
			}
			RCT2_GLOBAL(0x0138B584, uint16) = x;
			RCT2_GLOBAL(0x0138B586, uint16) = y;
			RCT2_GLOBAL(0x0138B588, uint16) = z;
			RCT2_GLOBAL(0x0138B592, uint8) = mapElement->properties.track.type;
		}
	} while (!map_element_is_last_for_tile(mapElement++));

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
	ride_ratings_calculate_value(ride);

	window_invalidate_by_number(WC_RIDE, _rideRatingsCurrentRide);
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

	mapElement = map_get_first_element_at(x, y);
	trackType = RCT2_GLOBAL(0x0138B592, uint8);

	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
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
	} while (!map_element_is_last_for_tile(mapElement++));

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

static void ride_ratings_calculate_value(rct_ride *ride)
{
	rct_ride *ride2;
	int i, otherRidesOfSameType;

	if (ride->excitement == (ride_rating)0xFFFF)
		return;

	int value =
		(((ride->excitement * RCT2_GLOBAL(0x0097CD1E + (ride->type * 6), sint16)) * 32) >> 15) +
		(((ride->intensity  * RCT2_GLOBAL(0x0097CD20 + (ride->type * 6), sint16)) * 32) >> 15) +
		(((ride->nausea     * RCT2_GLOBAL(0x0097CD22 + (ride->type * 6), sint16)) * 32) >> 15);

	int monthsOld = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16) - ride->build_date;

	// New ride reward
	if (monthsOld <= 12) {
		value += 10;
		if (monthsOld <= 4)
			value += 20;
	}

	// Old ride penalty
	if (monthsOld >= 40) value -= value / 4;
	if (monthsOld >= 64) value -= value / 4;
	if (monthsOld < 200) {
		if (monthsOld >= 88) value -= value / 4;
		if (monthsOld >= 104) value -= value / 4;
		if (monthsOld >= 120) value -= value / 2;
		if (monthsOld >= 128) value -= value / 2;
	}

	// Other ride of same type penalty
	otherRidesOfSameType = 0;
	FOR_ALL_RIDES(i, ride2) {
		if (ride2->type == ride->type)
			otherRidesOfSameType++;
	}
	if (otherRidesOfSameType > 1)
		value -= value / 4;

	ride->value = max(0, value);
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
	} else if (ride->mode == RIDE_MODE_POWERED_LAUNCH_PASSTROUGH) {
		upkeep += 160;
	} else if (ride->mode == RIDE_MODE_LIM_POWERED_LAUNCH) {
		upkeep += 320;
	} else if (ride->mode == RIDE_MODE_POWERED_LAUNCH || 
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
 *  rct2: 0x00655FD6
 */
static void set_unreliability_factor(rct_ride *ride)
{
    // The higher the number, the lower the breakdown
    // possibility. Range is [3, 7]. values are here:
    // https://gist.github.com/kevinburke/123977c4884ccadbec70. Consider
    // inlining this per ride
	uint8 lift_speed_adjustment = RCT2_ADDRESS(0x0097D7C9, uint8)[ride->type * 4];
    ride->unreliability_factor += (ride->lift_hill_speed - lift_speed_adjustment) * 2;
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

static rating_tuple get_var_10E_rating(uint16 var_10E) {
	int al;
	al = var_10E >> 8;
	al = al & 7;
	int excitement = (al * 163840) >> 16;

	al = var_10E >> 5;
	al = al & 7;
	excitement += (al * 196608) >> 16;

	// Low bits, 0x0065DF3A
	al = var_10E & 0x1F;
	excitement += (al * 63421) >> 16;

	al = var_10E >> 8;
	al = al & 7;
	int intensity = (al * 81920) >> 16;

	al = var_10E >> 5;
	al = al & 7;
	intensity += (al * 49152) >> 16;

	al = var_10E & 0x1F;
	intensity += (al * 21140) >> 16;

	al = var_10E >> 8;
	al = al & 7;
	int nausea = (al * 327680) >> 16;

	al = var_10E >> 5;
	al = al & 7;
	nausea += (al * 204800) >> 16;

	al = var_10E & 0x1F;
	nausea += (al * 42281) >> 16;

	rating_tuple rating = { excitement, intensity, nausea };
	return rating;
}

/**
 * rct2: 0x0065DF72
 */
static rating_tuple get_var_110_rating(uint16 var_110) {
	int al;

	al = var_110 >> 8;
	al = al & 7;
	int excitement = (al * 245760) >> 16;

	al = var_110 >> 5;
	al = al & 7;
	excitement += (al * 245760) >> 16;

	al = var_110 & 0x1F;
	excitement += (al * 73992) >> 16;

	al = var_110 >> 8;
	al = al & 7;
	int intensity = (al * 81920) >> 16;

	al = var_110 >> 5;
	al = al & 7;
	intensity += (al * 49152) >> 16;

	al = var_110 & 0x1F;
	intensity += (al * 21140) >> 16;

	al = var_110 >> 8;
	al = al & 7;
	int nausea = (al * 327680) >> 16;

	al = var_110 >> 5;
	al = al & 7;
	nausea += (al * 204800) >> 16;

	al = var_110 & 0x1F;
	nausea += (al * 48623) >> 16;

	rating_tuple rating = { excitement, intensity, nausea };
	return rating;
}

/**
 * rct2: 0x0065E047
 */
static rating_tuple get_var_112_rating(uint16 var_112) {
	int al;

	al = var_112 >> 11;
	al = min(al & 0x3F, 4);
	int excitement = (al * 491520) >> 16;

	al = var_112 >> 11;
	al = min(al & 0x3F, 8);
	int nausea = (al * 491520) >> 16;

	al = var_112 >> 8;
	al = min(al & 7, 6);
	excitement += (al * 273066) >> 16;

	al = var_112 >> 5;
	al = min(al & 7, 6);
	excitement += (al * 240298) >> 16;

	al = min(var_112 & 0x1F, 7);
	excitement += (al * 187245) >> 16;

	rating_tuple rating = { excitement, 0, nausea };
	return rating;
}

/**
 * rct2: 0x0065E0F2
 */
static rating_tuple get_inversions_ratings(uint8 inversions) {
	inversions = inversions & 0x1F;

	int a = min(inversions, 6);
	int excitement = (a * 1747626) >> 16;

	int intensity = (inversions * 3276800) >> 16;
	int nausea = (inversions * 1419946) >> 16;

	rating_tuple rating = { excitement, intensity, nausea };
	return rating;
}

/*
 *
 */
static rating_tuple get_special_track_elements_rating(uint8 type, rct_ride *ride) {
	int excitement = 0, intensity = 0, nausea = 0;
	if (type == RIDE_TYPE_GHOST_TRAIN) {
		if (ride_has_spinning_tunnel(ride)) {
			excitement += 40;
			intensity  += 25;
			nausea     += 55;
		}
	} else if (type == RIDE_TYPE_LOG_FLUME) {
		// Reverser for log flume
		if (ride_has_log_reverser(ride)) {
			excitement += 48;
			intensity  += 55;
			nausea     += 65;
		}
	} else {
		if (ride_has_water_splash(ride)) {
			excitement += 50;
			intensity  += 30;
			nausea     += 20;
		} 
		if (ride_has_waterfall(ride)) {
			excitement += 55;
			intensity  += 30;
		}
		if (ride_has_whirlpool(ride)) {
			excitement += 35;
			intensity  += 20;
			nausea     += 23;
		}
	}
	uint8 helix_sections = ride_get_helix_sections(ride);
	int al = min(helix_sections, 9);
	excitement += (al * 254862) >> 16;

	al = min(helix_sections, 11);
	intensity += (al * 148945) >> 16;

	al = max(helix_sections - 5, 0);
	al = min(al, 10);
	nausea += (al * 1310720) >> 16;

	rating_tuple rating = { excitement, intensity, nausea };
	return rating;
}

/**
 *  rct2: 0x0065DDD1
 */
static rating_tuple sub_65DDD1(rct_ride *ride)
{
	int excitement = 0, intensity = 0, nausea = 0;

	rating_tuple special_track_element_rating = get_special_track_elements_rating(ride->type, ride);
	excitement += special_track_element_rating.excitement;
	intensity  += special_track_element_rating.intensity;
	nausea     += special_track_element_rating.nausea;

	rating_tuple var_10E_rating = get_var_10E_rating(ride->var_10E);
	excitement += var_10E_rating.excitement;
	intensity  += var_10E_rating.intensity;
	nausea     += var_10E_rating.nausea;

	rating_tuple var_110_rating = get_var_110_rating(ride->var_110);
	excitement += var_110_rating.excitement;
	intensity  += var_110_rating.intensity;
	nausea     += var_110_rating.nausea;

	rating_tuple var_112_rating = get_var_112_rating(ride->var_112);
	excitement += var_112_rating.excitement;
	intensity  += var_112_rating.intensity;
	nausea     += var_112_rating.nausea;

	rating_tuple inversions_rating = get_inversions_ratings(ride->inversions);
	excitement += inversions_rating.excitement;
	intensity  += inversions_rating.intensity;
	nausea     += inversions_rating.nausea;

	rating_tuple rating = { excitement, intensity, nausea };
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
	rating_tuple result;
	fixed16_2dp gforce;

	result.excitement = 0;
	result.intensity = 0;
	result.nausea = 0;

	// Apply maximum positive G force factor
	result.excitement += (ride->max_positive_vertical_g * 5242) >> 16;
	result.intensity += (ride->max_positive_vertical_g * 52428) >> 16;
	result.nausea += (ride->max_positive_vertical_g * 17039) >> 16;

	// Apply maximum negative G force factor
	gforce = ride->max_negative_vertical_g;
	result.excitement += (clamp(-FIXED_2DP(2,50), gforce, FIXED_2DP(0,00)) * -15728) >> 16;
	result.intensity += ((gforce - FIXED_2DP(1,00)) * -52428) >> 16;
	result.nausea += ((gforce - FIXED_2DP(1,00)) * -14563) >> 16;

	// Apply lateral G force factor
	result.excitement += (min(FIXED_2DP(1,50), ride->max_lateral_g) * 26214) >> 16;
	result.intensity += (ride->max_lateral_g * 65536) >> 16;
	result.nausea += (ride->max_lateral_g * 21845) >> 16;

	// Very high lateral G force penalty
	if (ride->max_lateral_g > FIXED_2DP(2,80)) {
		result.intensity += FIXED_2DP(3,75);
		result.nausea += FIXED_2DP(2,00);
	}
	if (ride->max_lateral_g > FIXED_2DP(3,10)) {
		result.excitement /= 2;
		result.intensity += FIXED_2DP(8,50);
		result.nausea += FIXED_2DP(4,00);
	}

	return result;
}

/**
 *
 *  rct2: 0x0065E139
 */
static rating_tuple ride_ratings_get_drop_ratings(rct_ride *ride)
{
	rating_tuple result;
	int drops;

	result.excitement = 0;
	result.intensity = 0;
	result.nausea = 0;

	// Apply number of drops factor
	drops = ride->drops & 0x3F;
	result.excitement += (min(9, drops) * 728177) >> 16;
	result.intensity += (drops * 928426) >> 16;
	result.nausea += (drops * 655360) >> 16;

	// Apply highest drop factor
	result.excitement += ((ride->highest_drop_height * 2) * 16000) >> 16;
	result.intensity += ((ride->highest_drop_height * 2) * 32000) >> 16;
	result.nausea += ((ride->highest_drop_height * 2) * 10240) >> 16;

	return result;
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
	for (yy = max(y - 5, 0); yy <= y + 5; yy++) {
		for (xx = max(x - 5, 0); xx <= x + 5; xx++) {
			// Count scenery items on this tile
			mapElement = map_get_first_element_at(xx, yy);
			do {
				if (mapElement->flags & (1 << 4))
					continue;

				type = map_element_get_type(mapElement);
				if (type == MAP_ELEMENT_TYPE_SCENERY || type == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE)
					numSceneryItems++;
			} while (!map_element_is_last_for_tile(mapElement++));
		}
	}

	return min(numSceneryItems, 47) * 5;
}

#pragma region Ride rating calculation functions

static void ride_ratings_calculate_mine_train_coaster(rct_ride *ride)
{
	rating_tuple ratings, subRating;
	int totalLength, time;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	// Base ratings
	ratings.excitement = RIDE_RATING(2,90);
	ratings.intensity  = RIDE_RATING(2,30);
	ratings.nausea     = RIDE_RATING(2,10);

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
	subRating = ride_ratings_get_gforce_ratings(ride);
	ratings.excitement += (subRating.excitement * 40960) >> 16;
	ratings.intensity += (subRating.intensity * 35746) >> 16;
	ratings.nausea += (subRating.nausea * 49648) >> 16;

	// Apply ?
	subRating = sub_65DDD1(ride);
	ratings.excitement += (subRating.excitement * 29721) >> 16;
	ratings.intensity += (subRating.intensity * 34767) >> 16;
	ratings.nausea += (subRating.nausea * 45749) >> 16;

	// Apply drops factor
	subRating = ride_ratings_get_drop_ratings(ride);
	ratings.excitement += (subRating.excitement * 29127) >> 16;
	ratings.intensity += (subRating.intensity * 46811) >> 16;
	ratings.nausea += (subRating.nausea * 49152) >> 16;

	// Apply ?
	subRating = sub_65E1C2(ride);
	ratings.excitement += (subRating.excitement * 19275) >> 16;
	ratings.intensity += (subRating.intensity * 32768) >> 16;
	ratings.nausea += (subRating.nausea * 35108) >> 16;

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
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_maze(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 8;
	set_unreliability_factor(ride);

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
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_spiral_slide(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 8;
	set_unreliability_factor(ride);

	// Base ratings
	ratings.excitement =	RIDE_RATING(1,50);
	ratings.intensity =		RIDE_RATING(1,40);
	ratings.nausea =		RIDE_RATING(0,90);

	// Unlimited slides boost
	if (ride->mode == RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION) {
		ratings.excitement += RIDE_RATING(0, 40);
		ratings.intensity  += RIDE_RATING(0, 20);
		ratings.nausea     += RIDE_RATING(0, 25);
	}

	ratings.excitement += (ride_ratings_get_scenery_score(ride) * 25098) >> 16;

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 2 << 5;
}

static void ride_ratings_calculate_pirate_ship(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 10;
	set_unreliability_factor(ride);

	// Base ratings
	ratings.excitement =	RIDE_RATING(1,50);
	ratings.intensity =		RIDE_RATING(1,90);
	ratings.nausea =		RIDE_RATING(1,41);

	ratings.excitement += ride->var_0D0 * 5;
	ratings.intensity += ride->var_0D0 * 5;
	ratings.nausea += ride->var_0D0 * 10;

	ratings.excitement += (ride_ratings_get_scenery_score(ride) * 16732) >> 16;

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_inverter_ship(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	// Base ratings
	ratings.excitement =	RIDE_RATING(2,50);
	ratings.intensity =		RIDE_RATING(2,70);
	ratings.nausea =		RIDE_RATING(2,74);

	ratings.excitement += ride->var_0D0 * 11;
	ratings.intensity += ride->var_0D0 * 22;
	ratings.nausea += ride->var_0D0 * 22;

	ratings.excitement += (ride_ratings_get_scenery_score(ride) * 11155) >> 16;

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_food_stall(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_drink_stall(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_shop(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_merry_go_round(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	int unk = ride->var_0D0 * 5;
	ratings.excitement	= unk + RIDE_RATING(0,60) + ((ride_ratings_get_scenery_score(ride) * 19521) >> 16);
	ratings.intensity	= unk + RIDE_RATING(0,15);
	ratings.nausea		= unk + RIDE_RATING(0,30);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;
}

static void ride_ratings_calculate_information_kiosk(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_bathroom(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_ferris_wheel(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	int unk = ride->var_0D0 * 25;
	ratings.excitement	= unk + RIDE_RATING(0,60) + ((ride_ratings_get_scenery_score(ride) * 41831) >> 16);
	ratings.intensity	= unk + RIDE_RATING(0,25);
	ratings.nausea		= unk + RIDE_RATING(0,30);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_motion_simulator(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 21;
	set_unreliability_factor(ride);

	// Base ratings
	if (ride->mode == RIDE_MODE_FILM_THRILL_RIDERS) {
		ratings.excitement	= RIDE_RATING(2,90);
		ratings.intensity	= RIDE_RATING(3,50);
		ratings.nausea		= RIDE_RATING(3,00);
	} else {
		ratings.excitement	= RIDE_RATING(3,25);
		ratings.intensity	= RIDE_RATING(4,10);
		ratings.nausea		= RIDE_RATING(3,30);
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;
}

static void ride_ratings_calculate_3d_cinema(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 21;
	set_unreliability_factor(ride);

	// Base ratings
	switch (ride->mode) {
	default:
	case RIDE_MODE_3D_FILM_MOUSE_TAILS:
		ratings.excitement	= RIDE_RATING(3,50);
		ratings.intensity	= RIDE_RATING(2,40);
		ratings.nausea		= RIDE_RATING(1,40);
		break;
	case RIDE_MODE_3D_FILM_STORM_CHASERS:
		ratings.excitement	= RIDE_RATING(4,00);
		ratings.intensity	= RIDE_RATING(2,65);
		ratings.nausea		= RIDE_RATING(1,55);
		break;
	case RIDE_MODE_3D_FILM_SPACE_RAIDERS:
		ratings.excitement	= RIDE_RATING(4,20);
		ratings.intensity	= RIDE_RATING(2,60);
		ratings.nausea		= RIDE_RATING(1,48);
		break;
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;
}

static void ride_ratings_calculate_top_spin(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 19;
	set_unreliability_factor(ride);

	// Base ratings
	switch (ride->mode) {
	default:
	case RIDE_MODE_BEGINNERS:
		ratings.excitement	= RIDE_RATING(2,00);
		ratings.intensity	= RIDE_RATING(4,80);
		ratings.nausea		= RIDE_RATING(5,74);
		break;
	case RIDE_MODE_INTENSE:
		ratings.excitement	= RIDE_RATING(3,00);
		ratings.intensity	= RIDE_RATING(5,75);
		ratings.nausea		= RIDE_RATING(6,64);
		break;
	case RIDE_MODE_BERSERK:
		ratings.excitement	= RIDE_RATING(3,20);
		ratings.intensity	= RIDE_RATING(6,80);
		ratings.nausea		= RIDE_RATING(7,94);
		break;
	}

	ratings.excitement += (ride_ratings_get_scenery_score(ride) * 11155) >> 16;

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_space_rings(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 7;
	set_unreliability_factor(ride);

	// Base ratings
	ratings.excitement	= RIDE_RATING(1,50);
	ratings.intensity	= RIDE_RATING(2,10);
	ratings.nausea		= RIDE_RATING(6,50);

	ratings.excitement += (ride_ratings_get_scenery_score(ride) * 25098) >> 16;

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_elevator(rct_ride *ride)
{
	rating_tuple ratings;
	int totalLength;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

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
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;

	if ((sub_65E72D(ride) >> 8) >= 5)
		ride->excitement /= 4;
}

static void ride_ratings_calculate_atm(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_twist(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	// Base ratings
	ratings.excitement	= RIDE_RATING(1,13);
	ratings.intensity	= RIDE_RATING(0,97);
	ratings.nausea		= RIDE_RATING(1,90);

	ratings.excitement += ride->var_0D0 * 20;
	ratings.intensity += ride->var_0D0 * 20;
	ratings.nausea += ride->var_0D0 * 20;

	ratings.excitement += (ride_ratings_get_scenery_score(ride) * 13943) >> 16;
	
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);
	
	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_haunted_house(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 8;
	set_unreliability_factor(ride);

	ratings.excitement	= RIDE_RATING(3,41);
	ratings.intensity	= RIDE_RATING(1,53);
	ratings.nausea		= RIDE_RATING(0,10);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0xE0;
}

static void ride_ratings_calculate_mini_golf(rct_ride *ride)
{
	rating_tuple ratings, unkRating;

	// RCT2_CALLPROC_X(0x0065BF97, 0, 0, 0, 0, 0, (int)ride, 0); return;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 0;
	set_unreliability_factor(ride);

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
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_first_aid(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_circus_show(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 9;
	set_unreliability_factor(ride);

	ratings.excitement	= RIDE_RATING(2,10);
	ratings.intensity	= RIDE_RATING(0,30);
	ratings.nausea		= RIDE_RATING(0,0);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;
}

static void ride_ratings_calculate_crooked_house(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 5;
	set_unreliability_factor(ride);

	ratings.excitement	= RIDE_RATING(2,15);
	ratings.intensity	= RIDE_RATING(0,62);
	ratings.nausea		= RIDE_RATING(0,34);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0xE0;
}

static void ride_ratings_calculate_magic_carpet(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	// Base ratings
	ratings.excitement	= RIDE_RATING(2,45);
	ratings.intensity	= RIDE_RATING(1,60);
	ratings.nausea		= RIDE_RATING(2,60);

	ratings.excitement += ride->var_0D0 * 10;
	ratings.intensity += ride->var_0D0 * 20;
	ratings.nausea += ride->var_0D0 * 20;

	ratings.excitement += (ride_ratings_get_scenery_score(ride) * 11155) >> 16;
	
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_enterprise(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 22;
	set_unreliability_factor(ride);

	// Base ratings
	ratings.excitement	= RIDE_RATING(3,60);
	ratings.intensity	= RIDE_RATING(4,55);
	ratings.nausea		= RIDE_RATING(5,72);

	ratings.excitement += ride->var_0D0;
	ratings.intensity += ride->var_0D0 * 16;
	ratings.nausea += ride->var_0D0 * 16;

	ratings.excitement += (ride_ratings_get_scenery_score(ride) * 19521) >> 16;
	
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 3 << 5;
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
	ride_ratings_calculate_spiral_slide,			// SPIRAL_SLIDE
	NULL,											// GO_KARTS
	NULL,											// LOG_FLUME
	NULL,											// RIVER_RAPIDS
	NULL,											// BUMPER_CARS
	ride_ratings_calculate_pirate_ship,				// PIRATE_SHIP
	ride_ratings_calculate_inverter_ship,			// SWINGING_INVERTER_SHIP
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
	ride_ratings_calculate_motion_simulator,		// MOTION_SIMULATOR
	ride_ratings_calculate_3d_cinema,				// 3D_CINEMA
	ride_ratings_calculate_top_spin,				// TOP_SPIN
	ride_ratings_calculate_space_rings,				// SPACE_RINGS
	NULL,											// REVERSE_FREEFALL_COASTER
	ride_ratings_calculate_elevator,				// ELEVATOR
	NULL,											// VERTICAL_DROP_ROLLER_COASTER
	ride_ratings_calculate_atm,						// ATM
	ride_ratings_calculate_twist,					// TWIST
	ride_ratings_calculate_haunted_house,			// HAUNTED_HOUSE
	ride_ratings_calculate_first_aid,				// FIRST_AID
	ride_ratings_calculate_circus_show,				// CIRCUS_SHOW
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
	ride_ratings_calculate_magic_carpet,			// MAGIC_CARPET
	NULL,											// SUBMARINE_RIDE
	NULL,											// RIVER_RAFTS
	NULL,											// 50
	ride_ratings_calculate_enterprise,				// ENTERPRISE
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
