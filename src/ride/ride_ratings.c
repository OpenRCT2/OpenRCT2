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
			if (!track_get_next(&trackElement, &nextTrackElement, NULL, NULL)) {
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
			if (!track_get_previous(x, y, mapElement, NULL)) {
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
	if (calcFunc != NULL)
		calcFunc(ride);

	// Original ride calculation
	// calcFunc = RCT2_ADDRESS(0x0097E050, ride_ratings_calculation)[ride->type];
	// RCT2_CALLPROC_X((int)calcFunc, 0, 0, 0, 0, 0, (int)ride, 0);
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

	uint32 totalLength = ride_get_total_length(ride) >> 16;

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
		if (rideEntry->flags & RIDE_ENTRY_FLAG_11) {
			if (totalAirTime >= 96) {
				totalAirTime -= 96;
				ratings->excitement -= totalAirTime / 8;
				ratings->nausea += totalAirTime / 16;
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
	uint8 lift_speed_adjustment = RideLiftHillAdjustments[ride->type];
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
 * Seems to calculate how much of the track is sheltered in eighths.
 *  rct2: 0x0065E72D
 */
static int sub_65E72D(rct_ride *ride)
{
	int totalLength = ride_get_total_length(ride);
	int shelteredLength = ride->sheltered_length;
	int lengthEighth = totalLength / 8;
	int lengthCounter = lengthEighth;
	int numShelteredEighths = 0;
	for (int i = 0; i < 7; i++) {
		if (shelteredLength >= lengthCounter) {
			lengthCounter += lengthEighth;
			numShelteredEighths++;
		}
	}

	int dh = numShelteredEighths;
	rct_ride_type *rideType = GET_RIDE_ENTRY(ride->subtype);
	if (rideType->flags & RIDE_ENTRY_FLAG_COVERED_RIDE)
		numShelteredEighths = 7;

	return (dh << 8) | numShelteredEighths;
}

static rating_tuple get_var_10E_rating(rct_ride* ride) {
	int var_10E_unk_1 = get_var_10E_unk_1(ride);
	int var_10E_unk_2 = get_var_10E_unk_2(ride);
	int var_10E_unk_3 = get_var_10E_unk_3(ride);

	int excitement = (var_10E_unk_1 * 0x28000) >> 16;
	excitement += (var_10E_unk_2 * 0x30000) >> 16;
	excitement += (var_10E_unk_3 * 63421) >> 16;

	int intensity = (var_10E_unk_1 * 81920) >> 16;
	intensity += (var_10E_unk_2 * 49152) >> 16;
	intensity += (var_10E_unk_3 * 21140) >> 16;

	int nausea = (var_10E_unk_1 * 0x50000) >> 16;
	nausea += (var_10E_unk_2 * 0x32000) >> 16;
	nausea += (var_10E_unk_3 * 42281) >> 16;

	rating_tuple rating = { excitement, intensity, nausea };
	return rating;
}

/**
 * rct2: 0x0065DF72
 */
static rating_tuple get_var_110_rating(rct_ride* ride) {
	int var_110_unk_1 = get_var_110_unk_1(ride);
	int var_110_unk_2 = get_var_110_unk_2(ride);
	int var_110_unk_3 = get_var_110_unk_3(ride);

	int excitement = (var_110_unk_1 * 0x3C000) >> 16;
	excitement += (var_110_unk_2 * 0x3C000) >> 16;
	excitement += (var_110_unk_3 * 73992) >> 16;

	int intensity = (var_110_unk_1 * 0x14000) >> 16;
	intensity += (var_110_unk_2 * 49152) >> 16;
	intensity += (var_110_unk_3 * 21140) >> 16;

	int nausea = (var_110_unk_1 * 0x50000) >> 16;
	nausea += (var_110_unk_2 * 0x32000) >> 16;
	nausea += (var_110_unk_3 * 48623) >> 16;

	rating_tuple rating = { excitement, intensity, nausea };
	return rating;
}

/**
 * rct2: 0x0065E047
 */
static rating_tuple get_var_112_rating(rct_ride *ride) {
	int al;

	al = get_var_112_unk_1(ride);
	al = min(al, 4);
	int excitement = (al * 0x78000) >> 16;

	al = get_var_112_unk_1(ride);
	al = min(al, 8);
	int nausea = (al * 0x78000) >> 16;

	al = get_var_112_unk_2(ride);
	al = min(al, 6);
	excitement += (al * 273066) >> 16;

	al = get_var_112_unk_3(ride);
	al = min(al, 6);
	excitement += (al * 0x3AAAA) >> 16;

	al = get_var_112_unk_4(ride);
	al = min(al, 7);
	excitement += (al * 187245) >> 16;

	rating_tuple rating = { excitement, 0, nausea };
	return rating;
}

/**
 * rct2: 0x0065E0F2
 */
static rating_tuple get_inversions_ratings(uint8 inversions) {
	int excitement = (min(inversions, 6) * 0x1AAAAA) >> 16;
	int intensity = (inversions * 0x320000) >> 16;
	int nausea = (inversions * 0x15AAAA) >> 16;

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
	nausea += (al * 0x140000) >> 16;

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

	rating_tuple var_10E_rating = get_var_10E_rating(ride);
	excitement += var_10E_rating.excitement;
	intensity  += var_10E_rating.intensity;
	nausea     += var_10E_rating.nausea;

	rating_tuple var_110_rating = get_var_110_rating(ride);
	excitement += var_110_rating.excitement;
	intensity  += var_110_rating.intensity;
	nausea     += var_110_rating.nausea;

	rating_tuple var_112_rating = get_var_112_rating(ride);
	excitement += var_112_rating.excitement;
	intensity  += var_112_rating.intensity;
	nausea     += var_112_rating.nausea;

	rating_tuple inversions_rating = get_inversions_ratings(ride->inversions & 0x1F);
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
	int sheltered_length_shifted = (ride->sheltered_length) >> 16;
	uint32 eax = min(sheltered_length_shifted, 1000);
	int excitement = (eax * 9175) >> 16;

	eax = min(sheltered_length_shifted, 2000);
	int intensity = (eax * 0x2666) >> 16;

	eax = min(sheltered_length_shifted, 1000);
	int nausea = (eax * 0x4000) >> 16;

	/*eax = (ride->var_11C * 30340) >> 16;*/
	/*nausea += eax;*/

	if (ride->num_sheltered_sections & 0x40) {
		excitement += 20;
		nausea     += 15;
	}

	if (ride->num_sheltered_sections & 0x20) {
		excitement += 20;
		nausea     += 15;
	}

	uint8 lowerval = ride->num_sheltered_sections & 0x1F;
	lowerval = min(lowerval, 11);
	excitement += (lowerval * 774516) >> 16;

	rating_tuple rating = { excitement, intensity, nausea };
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
	result.intensity += ride->max_lateral_g;
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
	z = map_element_height(x * 32, y * 32) & 0xFFFF;

	// Check if station is underground, returns a fixed mediocre score since you can't have scenery underground
	if (z > ride->station_heights[i] * 8)
		return 40;

	// Count surrounding scenery items
	numSceneryItems = 0;
	for (yy = max(y - 5, 0); yy <= min(y + 5, 255); yy++) {
		for (xx = max(x - 5, 0); xx <= min(x + 5, 255); xx++) {
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

#pragma region Ride rating calculation helpers

static void ride_ratings_set(rating_tuple *ratings, int excitement, int intensity, int nausea)
{
	ratings->excitement = excitement;
	ratings->intensity = intensity;
	ratings->nausea = nausea;
}

static void ride_ratings_apply_length(rating_tuple *ratings, rct_ride *ride, int maxLength, int excitementMultiplier)
{
	ratings->excitement += (min(ride_get_total_length(ride) >> 16, maxLength) * excitementMultiplier) >> 16;
}

static void ride_ratings_apply_synchronisation(rating_tuple *ratings, rct_ride *ride, int excitement, int intensity)
{
	if (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS) {
		ratings->excitement += excitement;
		ratings->intensity += intensity;
	}
}

static void ride_ratings_apply_train_length(rating_tuple *ratings, rct_ride *ride, int excitementMultiplier)
{
	ratings->excitement += ((ride->num_cars_per_train - 1) * excitementMultiplier) >> 16;
}

static void ride_ratings_apply_max_speed(rating_tuple *ratings, rct_ride *ride, int excitementMultiplier, int intensityMultiplier, int nauseaMultiplier)
{
	ratings->excitement += ((ride->max_speed >> 16) * excitementMultiplier) >> 16;
	ratings->intensity += ((ride->max_speed >> 16) * intensityMultiplier) >> 16;
	ratings->nausea += ((ride->max_speed >> 16) * nauseaMultiplier) >> 16;
}

static void ride_ratings_apply_average_speed(rating_tuple *ratings, rct_ride *ride, int excitementMultiplier, int intensityMultiplier)
{
	ratings->excitement += ((ride->average_speed >> 16) * excitementMultiplier) >> 16;
	ratings->intensity += ((ride->average_speed >> 16) * intensityMultiplier) >> 16;
}

static void ride_ratings_apply_duration(rating_tuple *ratings, rct_ride *ride, int maxDuration, int excitementMultiplier)
{
	ratings->excitement += (min(ride_get_total_time(ride), maxDuration) * excitementMultiplier) >> 16;
}

static void ride_ratings_apply_gforces(rating_tuple *ratings, rct_ride *ride, int excitementMultiplier, int intensityMultiplier, int nauseaMultiplier)
{
	rating_tuple subRating = ride_ratings_get_gforce_ratings(ride);
	ratings->excitement += (subRating.excitement * excitementMultiplier) >> 16;
	ratings->intensity += (subRating.intensity * intensityMultiplier) >> 16;
	ratings->nausea += (subRating.nausea * nauseaMultiplier) >> 16;
}

static void ride_ratings_apply_65DDD1(rating_tuple *ratings, rct_ride *ride, int excitementMultiplier, int intensityMultiplier, int nauseaMultiplier)
{
	rating_tuple subRating = sub_65DDD1(ride);
	ratings->excitement += (subRating.excitement * excitementMultiplier) >> 16;
	ratings->intensity += (subRating.intensity * intensityMultiplier) >> 16;
	ratings->nausea += (subRating.nausea * nauseaMultiplier) >> 16;
}

static void ride_ratings_apply_drops(rating_tuple *ratings, rct_ride *ride, int excitementMultiplier, int intensityMultiplier, int nauseaMultiplier)
{
	rating_tuple subRating = ride_ratings_get_drop_ratings(ride);
	ratings->excitement += (subRating.excitement * excitementMultiplier) >> 16;
	ratings->intensity += (subRating.intensity * intensityMultiplier) >> 16;
	ratings->nausea += (subRating.nausea * nauseaMultiplier) >> 16;
}

static void ride_ratings_apply_65E1C2(rating_tuple *ratings, rct_ride *ride, int excitementMultiplier, int intensityMultiplier, int nauseaMultiplier)
{
	rating_tuple subRating = sub_65E1C2(ride);
	ratings->excitement += (subRating.excitement * excitementMultiplier) >> 16;
	ratings->intensity += (subRating.intensity * intensityMultiplier) >> 16;
	ratings->nausea += (subRating.nausea * nauseaMultiplier) >> 16;
}

static void ride_ratings_apply_operation_option(rating_tuple *ratings, rct_ride *ride, int excitementMultiplier, int intensityMultiplier, int nauseaMultiplier)
{
	ratings->excitement += (ride->operation_option * excitementMultiplier) >> 16;
	ratings->intensity += (ride->operation_option * intensityMultiplier) >> 16;
	ratings->nausea += (ride->operation_option * nauseaMultiplier) >> 16;
}

static void ride_ratings_apply_65E277(rating_tuple *ratings, rct_ride *ride, int excitementMultiplier)
{
	ratings->excitement += (sub_65E277() * excitementMultiplier) >> 16;
}

static void ride_ratings_apply_scenery(rating_tuple *ratings, rct_ride *ride, int excitementMultiplier)
{
	ratings->excitement += (ride_ratings_get_scenery_score(ride) * excitementMultiplier) >> 16;
}

static void ride_ratings_apply_highest_drop_height_penalty(rating_tuple *ratings, rct_ride *ride, int minHighestDropHeight, int excitementPenalty, int intensityPenalty, int nauseaPenalty)
{
	if (ride->highest_drop_height < minHighestDropHeight) {
		ratings->excitement /= excitementPenalty;
		ratings->intensity /= intensityPenalty;
		ratings->nausea /= nauseaPenalty;
	}
}

static void ride_ratings_apply_max_speed_penalty(rating_tuple *ratings, rct_ride *ride, int minMaxSpeed, int excitementPenalty, int intensityPenalty, int nauseaPenalty)
{
	if (ride->max_speed < minMaxSpeed) {
		ratings->excitement /= excitementPenalty;
		ratings->intensity /= intensityPenalty;
		ratings->nausea /= nauseaPenalty;
	}
}

static void ride_ratings_apply_num_drops_penalty(rating_tuple *ratings, rct_ride *ride, int minNumDrops, int excitementPenalty, int intensityPenalty, int nauseaPenalty)
{
	if ((ride->drops & 0x3F) < minNumDrops) {
		ratings->excitement /= excitementPenalty;
		ratings->intensity /= intensityPenalty;
		ratings->nausea /= nauseaPenalty;
	}
}

static void ride_ratings_apply_max_negative_g_penalty(rating_tuple *ratings, rct_ride *ride, int maxMaxNegativeVerticalG, int excitementPenalty, int intensityPenalty, int nauseaPenalty)
{
	if (ride->max_negative_vertical_g >= maxMaxNegativeVerticalG) {
		ratings->excitement /= excitementPenalty;
		ratings->intensity /= intensityPenalty;
		ratings->nausea /= nauseaPenalty;
	}
}

static void ride_ratings_apply_max_lateral_g_penalty(rating_tuple *ratings, rct_ride *ride, int minMaxLateralG, int excitementPenalty, int intensityPenalty, int nauseaPenalty)
{
	if (ride->max_lateral_g < minMaxLateralG) {
		ratings->excitement /= excitementPenalty;
		ratings->intensity /= intensityPenalty;
		ratings->nausea /= nauseaPenalty;
	}
}

static void ride_ratings_apply_first_length_penalty(rating_tuple *ratings, rct_ride *ride, int minFirstLength, int excitementPenalty, int intensityPenalty, int nauseaPenalty)
{
	if (ride->length[0] < minFirstLength) {
		ratings->excitement /= excitementPenalty;
		ratings->intensity /= intensityPenalty;
		ratings->nausea /= nauseaPenalty;
	}
}

#pragma endregion

#pragma region Ride rating calculation functions

static void ride_ratings_calculate_spiral_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,30), RIDE_RATING(0,30), RIDE_RATING(0,30));
	ride_ratings_apply_length(&ratings, ride, 6000, 819);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 140434);
	ride_ratings_apply_max_speed(&ratings, ride, 51366, 85019, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 400497);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 36864, 30384, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 28235, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 43690, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);
	
	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
	
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
	
	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_stand_up_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 17;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,50), RIDE_RATING(3,00), RIDE_RATING(3,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,10));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 123987, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 59578);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 34952, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 12850, 28398, 30427);
	ride_ratings_apply_65E277(&ratings, ride, 17893);
	ride_ratings_apply_scenery(&ratings, ride, 5577);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,50), 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_suspended_swinging_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 18;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,30), RIDE_RATING(2,90), RIDE_RATING(3,50));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,10));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 32768, 23831, 79437);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 48036);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6971);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xC0000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 60), 2, 2, 2);
	ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1, 50), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_inverted_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 17;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,60), RIDE_RATING(2,80), RIDE_RATING(3,20));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,42), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 29789, 55606);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 29552, 57186);
	ride_ratings_apply_drops(&ratings, ride, 29127, 39009, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 15291, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 15657);
	ride_ratings_apply_scenery(&ratings, ride, 8366);
	
	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
	
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
	
	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,30), 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_junior_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 13;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,40), RIDE_RATING(2,50), RIDE_RATING(1,80));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 20480, 23831, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 25700, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 9760);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 1, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_miniature_railway(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 11;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,50), RIDE_RATING(0,00), RIDE_RATING(0,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_train_length(&ratings, ride, 140434);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_65E1C2(&ratings, ride, 4294960871, 6553, 23405);
	ride_ratings_apply_65E277(&ratings, ride, 8946);
	ride_ratings_apply_scenery(&ratings, ride, 20915);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	int edx = sub_65E72D(ride);
	if (((edx >> 8) & 0xFF) >= 4)
		ride->excitement /= 4;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_monorail(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,00), RIDE_RATING(0,00), RIDE_RATING(0,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_train_length(&ratings, ride, 93622);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 70849, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 218453);
	ride_ratings_apply_duration(&ratings, ride, 150, 21845);
	ride_ratings_apply_65E1C2(&ratings, ride, 5140, 6553, 18724);
	ride_ratings_apply_65E277(&ratings, ride, 8946);
	ride_ratings_apply_scenery(&ratings, ride, 16732);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	int edx = sub_65E72D(ride);
	if (((edx >> 8) & 0xFF) >= 4)
		ride->excitement /= 4;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_mini_suspended_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,80), RIDE_RATING(2,50), RIDE_RATING(2,70));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,45), RIDE_RATING(0,15));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 34179, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 58254, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 19275, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 13943);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x80000, 2, 2, 2);
	ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1,30), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_boat_ride(rct_ride *ride)
{
	rating_tuple ratings;

	ride->unreliability_factor = 7;
	set_unreliability_factor(ride);

	// NOTE In the original game, the ratings were zeroed before calling set_unreliability_factor which is unusual as rest of
	// the calculation functions do this before hand. This is because set_unreliability_factor alters the value of
	// ebx (excitement). This is assumed to be a bug and therefore fixed.

	ride_ratings_set(&ratings, RIDE_RATING(1,90), RIDE_RATING(0,80), RIDE_RATING(0,90));

	// Most likely checking if the ride has does not have a circuit
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		ratings.excitement += RIDE_RATING(0,20);

	ride_ratings_apply_65E277(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 22310);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_wooden_wild_mouse(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,90), RIDE_RATING(2,90), RIDE_RATING(2,10));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0, 8));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 102400, 35746, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 29721, 43458, 45749);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 17893);
	ride_ratings_apply_scenery(&ratings, ride, 5577);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,10), 2, 2, 2);
	ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1,50), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 3, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_steeplechase(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,70), RIDE_RATING(2,40), RIDE_RATING(1,80));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,75), RIDE_RATING(0, 9));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 20480, 20852, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 25700, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 9760);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 4, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x80000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,50), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xF00000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_car_ride(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 12;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,00), RIDE_RATING(0,50), RIDE_RATING(0,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,15), RIDE_RATING(0,00));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_65DDD1(&ratings, ride, 14860, 0, 11437);
	ride_ratings_apply_drops(&ratings, ride, 8738, 0, 0);
	ride_ratings_apply_65E1C2(&ratings, ride, 12850, 6553, 4681);
	ride_ratings_apply_65E277(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 8366);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 8, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_launched_freefall(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,70), RIDE_RATING(3,00), RIDE_RATING(3,50));

	if (ride->mode == RIDE_MODE_DOWNWARD_LAUNCH) {
		ratings.excitement	+= RIDE_RATING(0,30);
		ratings.intensity	+= RIDE_RATING(0,65);
		ratings.nausea		+= RIDE_RATING(0,45);
	}

	ratings.excitement += ((ride_get_total_length(ride) >> 16) * 32768) >> 16;
	ride_ratings_apply_operation_option(&ratings, ride, 0, 1355917, 451972);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 25098);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_bobsleigh_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,80), RIDE_RATING(3,20), RIDE_RATING(2,50));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,20), RIDE_RATING(0,00));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 65536, 23831, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 5577);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xC0000, 2, 2, 2);
	ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1,20), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_observation_tower(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,50), RIDE_RATING(0,00), RIDE_RATING(0,10));
	ratings.excitement += ((ride_get_total_length(ride) >> 16) * 45875) >> 16;
	ratings.nausea += ((ride_get_total_length(ride) >> 16) * 26214) >> 16;
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 83662);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;

	int edx = sub_65E72D(ride);
	if (((edx >> 8) & 0xFF) >= 5)
		ride->excitement /= 4;
}

static void ride_ratings_calculate_looping_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = ride_is_powered_launched(ride) ? 20 : 15;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,00), RIDE_RATING(0,50), RIDE_RATING(0,20));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 14, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
	
	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 10), 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_dinghy_slide(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 13;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,70), RIDE_RATING(2,00), RIDE_RATING(1,50));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,50), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 65536, 29789, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 5577);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x8C0000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_mine_train_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,90), RIDE_RATING(2,30), RIDE_RATING(2,10));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 40960, 35746, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 29721, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 19275, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 21472);
	ride_ratings_apply_scenery(&ratings, ride, 16732);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,10), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_chairlift(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14 + (ride->speed * 2);
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,60), RIDE_RATING(0,40), RIDE_RATING(0,50));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_65DDD1(&ratings, ride, 7430, 3476, 4574);
	ride_ratings_apply_65E1C2(&ratings, ride, 4294948021, 21845, 23405);
	ride_ratings_apply_65E277(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 25098);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x960000, 2, 2, 2);
	
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	if (ride->num_stations <= 1) {
		ratings.excitement = 0;
		ratings.intensity /= 2;
	}

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	int edx = sub_65E72D(ride);
	if (((edx >> 8) & 0xFF) >= 4)
		ride->excitement /= 4;

	ride->inversions &= 0x1F;
	ride->inversions |= edx << 5;
}

static void ride_ratings_calculate_corkscrew_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,00), RIDE_RATING(0,50), RIDE_RATING(0,20));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
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

	ride_ratings_set(&ratings, RIDE_RATING(1,30), RIDE_RATING(0,50), RIDE_RATING(0,00));

	int size = min(ride->maze_tiles, 100);
	ratings.excitement += size;
	ratings.intensity += size * 2;
	
	ride_ratings_apply_scenery(&ratings, ride, 22310);

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

	ride_ratings_set(&ratings, RIDE_RATING(1,50), RIDE_RATING(1,40), RIDE_RATING(0,90));

	// Unlimited slides boost
	if (ride->mode == RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION) {
		ratings.excitement += RIDE_RATING(0, 40);
		ratings.intensity  += RIDE_RATING(0, 20);
		ratings.nausea     += RIDE_RATING(0, 25);
	}

	ride_ratings_apply_scenery(&ratings, ride, 25098);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 2 << 5;
}

static void ride_ratings_calculate_go_karts(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,42), RIDE_RATING(1,73), RIDE_RATING(0,40));
	ride_ratings_apply_length(&ratings, ride, 700, 32768);
	
	if (ride->mode == RIDE_MODE_RACE && ride->num_vehicles >= 4) {
		ratings.excitement +=	RIDE_RATING(1,40);
		ratings.intensity +=	RIDE_RATING(0,50);

		int lapsFactor = (ride->num_laps - 1) * 30;
		ratings.excitement += lapsFactor;
		ratings.intensity += lapsFactor / 2;
	}

	ride_ratings_apply_65DDD1(&ratings, ride, 4458, 3476, 5718);
	ride_ratings_apply_drops(&ratings, ride, 8738, 5461, 6553);
	ride_ratings_apply_65E1C2(&ratings, ride, 2570, 8738, 2340);
	ride_ratings_apply_65E277(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 16732);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	int edx = sub_65E72D(ride);

	ride->inversions &= 0x1F;
	ride->inversions |= edx << 5;

	if (((edx >> 8) & 0xFF) >= 6)
		ride->excitement /= 2;
}

static void ride_ratings_calculate_log_flume(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,50), RIDE_RATING(0,55), RIDE_RATING(0,30));
	ride_ratings_apply_length(&ratings, ride, 2000, 7208);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_max_speed(&ratings, ride, 531372, 655360, 301111);
	ride_ratings_apply_duration(&ratings, ride, 300, 13107);
	ride_ratings_apply_65DDD1(&ratings, ride, 22291, 20860, 4574);
	ride_ratings_apply_drops(&ratings, ride, 69905, 62415, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 22367);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_river_rapids(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,20), RIDE_RATING(0,70), RIDE_RATING(0,50));
	ride_ratings_apply_length(&ratings, ride, 2000, 6225);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,30), RIDE_RATING(0,05));
	ride_ratings_apply_max_speed(&ratings, ride, 115130, 159411, 106274);
	ride_ratings_apply_duration(&ratings, ride, 500, 13107);
	ride_ratings_apply_65DDD1(&ratings, ride, 29721, 22598, 5718);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 31314);
	ride_ratings_apply_scenery(&ratings, ride, 13943);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 2, 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_dodgems(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,30), RIDE_RATING(0,50), RIDE_RATING(0,35));

	if (ride->num_vehicles >= 4)
		ratings.excitement += RIDE_RATING(0,40);

	ratings.excitement += ride->operation_option;
	ratings.intensity += ride->operation_option / 2;

	if (ride->num_vehicles >= 4)
		ratings.excitement += RIDE_RATING(0,40);

	ride_ratings_apply_scenery(&ratings, ride, 5577);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;
}

static void ride_ratings_calculate_pirate_ship(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 10;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,50), RIDE_RATING(1,90), RIDE_RATING(1,41));

	ratings.excitement += ride->operation_option * 5;
	ratings.intensity += ride->operation_option * 5;
	ratings.nausea += ride->operation_option * 10;

	ride_ratings_apply_scenery(&ratings, ride, 16732);

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

	ride_ratings_set(&ratings, RIDE_RATING(2,50), RIDE_RATING(2,70), RIDE_RATING(2,74));

	ratings.excitement += ride->operation_option * 11;
	ratings.intensity += ride->operation_option * 22;
	ratings.nausea += ride->operation_option * 22;

	ride_ratings_apply_scenery(&ratings, ride, 11155);

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

	ride_ratings_set(&ratings, RIDE_RATING(0,60), RIDE_RATING(0,15), RIDE_RATING(0,30));

	int rotationsFactor = ride->rotations * 5;
	ratings.excitement += rotationsFactor;
	ratings.intensity += rotationsFactor;
	ratings.nausea += rotationsFactor;

	ride_ratings_apply_scenery(&ratings, ride, 19521);

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

static void ride_ratings_calculate_toilets(rct_ride *ride)
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

	ride_ratings_set(&ratings, RIDE_RATING(0,60), RIDE_RATING(0,25), RIDE_RATING(0,30));

	int rotationsFactor = ride->rotations * 25;
	ratings.excitement += rotationsFactor;
	ratings.intensity += rotationsFactor;
	ratings.nausea += rotationsFactor;

	ride_ratings_apply_scenery(&ratings, ride, 41831);

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
		ratings.excitement	= RIDE_RATING(3,25);
		ratings.intensity	= RIDE_RATING(4,10);
		ratings.nausea		= RIDE_RATING(3,30);
	} else {
		ratings.excitement	= RIDE_RATING(2,90);
		ratings.intensity	= RIDE_RATING(3,50);
		ratings.nausea		= RIDE_RATING(3,00);
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

	ride_ratings_apply_scenery(&ratings, ride, 11155);

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

	ride_ratings_set(&ratings, RIDE_RATING(1,50), RIDE_RATING(2,10), RIDE_RATING(6,50));
	ride_ratings_apply_scenery(&ratings, ride, 25098);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_reverse_freefall_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 25;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,00), RIDE_RATING(3,20), RIDE_RATING(2,80));
	ride_ratings_apply_length(&ratings, ride, 6000, 327);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,60), RIDE_RATING(0,15));
	ride_ratings_apply_max_speed(&ratings, ride, 436906, 436906, 320398);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 41704, 59578);
	ride_ratings_apply_65E1C2(&ratings, ride, 12850, 28398, 11702);
	ride_ratings_apply_65E277(&ratings, ride, 17893);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 34, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_lift(rct_ride *ride)
{
	rating_tuple ratings;
	int totalLength;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,11), RIDE_RATING(0,35), RIDE_RATING(0,30));

	totalLength = ride_get_total_length(ride) >> 16;
	ratings.excitement += (totalLength * 45875) >> 16;
	ratings.nausea += (totalLength * 26214) >> 16;

	ride_ratings_apply_65E277(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 83662);

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

static void ride_ratings_calculate_vertical_drop_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,20), RIDE_RATING(0,80), RIDE_RATING(0,30));
	ride_ratings_apply_length(&ratings, ride, 4000, 1146);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_max_speed(&ratings, ride, 97418, 141699, 70849);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 40960, 35746, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 58254, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 20, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,10), 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 1, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_cash_machine(rct_ride *ride)
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

	ride_ratings_set(&ratings, RIDE_RATING(1,13), RIDE_RATING(0,97), RIDE_RATING(1,90));

	ratings.excitement += ride->rotations * 20;
	ratings.intensity += ride->rotations * 20;
	ratings.nausea += ride->rotations * 20;

	ride_ratings_apply_scenery(&ratings, ride, 13943);
	
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

static void ride_ratings_calculate_flying_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 17;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(4,35), RIDE_RATING(1,85), RIDE_RATING(4,33));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 38130, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ratings.excitement /= 2;

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 1, 1);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,40), 2, 1, 1);

	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 1, 1);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_virginia_reel(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 19;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,10), RIDE_RATING(1,90), RIDE_RATING(3,70));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 110592, 29789, 59578);
	ride_ratings_apply_65DDD1(&ratings, ride, 52012, 26075, 45749);
	ride_ratings_apply_drops(&ratings, ride, 43690, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 22367);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xD20000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_splash_boats(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,46), RIDE_RATING(0,35), RIDE_RATING(0,30));
	ride_ratings_apply_length(&ratings, ride, 2000, 7208);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_max_speed(&ratings, ride, 797059, 655360, 301111);
	ride_ratings_apply_duration(&ratings, ride, 500, 13107);
	ride_ratings_apply_65DDD1(&ratings, ride, 22291, 20860, 4574);
	ride_ratings_apply_drops(&ratings, ride, 87381, 93622, 62259);
	ride_ratings_apply_65E1C2(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 22367);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_mini_helicopters(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 12;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,60), RIDE_RATING(0,40), RIDE_RATING(0,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,15), RIDE_RATING(0,00));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_65DDD1(&ratings, ride, 14860, 0, 4574);
	ride_ratings_apply_drops(&ratings, ride, 8738, 0, 0);
	ride_ratings_apply_65E1C2(&ratings, ride, 12850, 6553, 4681);
	ride_ratings_apply_65E277(&ratings, ride, 8946);
	ride_ratings_apply_scenery(&ratings, ride, 8366);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xA00000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 6 << 5;
}

static void ride_ratings_calculate_lay_down_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 18;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,85), RIDE_RATING(1,15), RIDE_RATING(2,75));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 38130, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0) {
		ratings.excitement /= 4;
		ratings.intensity /= 2;
		ratings.nausea /= 2;
	}

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_suspended_monorail(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,15), RIDE_RATING(0,23), RIDE_RATING(0, 8));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_train_length(&ratings, ride, 93622);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 70849, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 218453);
	ride_ratings_apply_duration(&ratings, ride, 150, 21845);
	ride_ratings_apply_65E1C2(&ratings, ride, 5140, 6553, 18724);
	ride_ratings_apply_65E277(&ratings, ride, 12525);
	ride_ratings_apply_scenery(&ratings, ride, 25098);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	int edx = sub_65E72D(ride);
	if (((edx >> 8) & 0xFF) >= 4)
		ride->excitement /= 4;

	ride->inversions &= 0x1F;
	ride->inversions |= edx << 5;
}

static void ride_ratings_calculate_reverser_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 19;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,40), RIDE_RATING(1,80), RIDE_RATING(1,70));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);

	int unk = min(RCT2_GLOBAL(0x0138B5CC, uint16), 6) * 20;
	ratings.excitement += unk;
	ratings.intensity += unk;
	ratings.nausea += unk;

	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 28672, 23831, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 43458, 45749);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 22367);
	ride_ratings_apply_scenery(&ratings, ride, 11155);

	if (RCT2_GLOBAL(0x0138B5CC, uint16) < 1)
		ratings.excitement /= 8;

	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 2, 1, 1);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 1, 1);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_heartline_twister_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 18;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,40), RIDE_RATING(1,70), RIDE_RATING(1,65));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,20), RIDE_RATING(0,04));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 97418, 123987, 70849);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 44683, 89367);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 52150, 57186);
	ride_ratings_apply_drops(&ratings, ride, 29127, 53052, 55705);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 34952, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 9841);
	ride_ratings_apply_scenery(&ratings, ride, 3904);

	if ((ride->inversions & 0x1F) == 0)
		ratings.excitement /= 4;

	ride_ratings_apply_num_drops_penalty(&ratings, ride, 1, 4, 1, 1);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_mini_golf(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 0;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,50), RIDE_RATING(0,90), RIDE_RATING(0,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_65DDD1(&ratings, ride, 14860, 0, 0);
	ride_ratings_apply_65E1C2(&ratings, ride, 5140, 6553, 4681);
	ride_ratings_apply_65E277(&ratings, ride, 15657);
	ride_ratings_apply_scenery(&ratings, ride, 27887);

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

static void ride_ratings_calculate_ghost_train(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 12;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,00), RIDE_RATING(0,20), RIDE_RATING(0,03));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,15), RIDE_RATING(0,00));
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_65DDD1(&ratings, ride, 14860, 0, 11437);
	ride_ratings_apply_drops(&ratings, ride, 8738, 0, 0);
	ride_ratings_apply_65E1C2(&ratings, ride, 25700, 6553, 4681);
	ride_ratings_apply_65E277(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 8366);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xB40000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_twister_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,50), RIDE_RATING(0,40), RIDE_RATING(0,30));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 32768, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_wooden_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 19;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,20), RIDE_RATING(2,60), RIDE_RATING(2,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 40960, 34555, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 43458, 45749);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 22367);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,10), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_side_friction_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 19;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,50), RIDE_RATING(2,00), RIDE_RATING(1,50));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 28672, 35746, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 43458, 45749);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 22367);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x50000, 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xFA0000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_wild_mouse(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,80), RIDE_RATING(2,50), RIDE_RATING(2,10));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0, 8));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 102400, 35746, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 29721, 43458, 45749);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 17893);
	ride_ratings_apply_scenery(&ratings, ride, 5577);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1,50), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_multi_dimension_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 18;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,75), RIDE_RATING(1,95), RIDE_RATING(4,79));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 38130, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ratings.excitement /= 4;

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 1, 1);
	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,40), 2, 1, 1);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 1, 1);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_giga_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;
	
	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,85), RIDE_RATING(0,40), RIDE_RATING(0,35));
	ride_ratings_apply_length(&ratings, ride, 6000, 819);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 140434);
	ride_ratings_apply_max_speed(&ratings, ride, 51366, 85019, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 400497);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 36864, 30384, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 28235, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 43690, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 16, 2, 2, 2);
	
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
	
	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_roto_drop(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 24;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,80), RIDE_RATING(3,50), RIDE_RATING(3,50));

	int lengthFactor = ((ride_get_total_length(ride) >> 16) * 209715) >> 16;
	ratings.excitement += lengthFactor;
	ratings.intensity += lengthFactor * 2;
	ratings.nausea += lengthFactor * 2;

	ride_ratings_apply_65E277(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 25098);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_flying_saucers(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 32;
	set_unreliability_factor(ride);

	ratings.excitement	= RIDE_RATING(2,40);
	ratings.intensity	= RIDE_RATING(0,55);
	ratings.nausea		= RIDE_RATING(0,39);

	if (ride->num_vehicles >= 4)
		ratings.excitement += RIDE_RATING(0,40);

	ratings.excitement += ride->time_limit;
	ratings.intensity += ride->time_limit / 2;

	if (ride->num_vehicles >= 4)
		ratings.excitement += RIDE_RATING(0,40);

	ride_ratings_apply_scenery(&ratings, ride, 5577);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
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

static void ride_ratings_calculate_monorail_cycles(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 4;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,40), RIDE_RATING(0,20), RIDE_RATING(0,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,15), RIDE_RATING(0,00));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_65DDD1(&ratings, ride, 14860, 0, 4574);
	ride_ratings_apply_drops(&ratings, ride, 8738, 0, 0);
	ride_ratings_apply_65E1C2(&ratings, ride, 5140, 6553, 2340);
	ride_ratings_apply_65E277(&ratings, ride, 8946);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x8C0000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_compact_inverted_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = ride->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE ? 31 : 21;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,15), RIDE_RATING(2,80), RIDE_RATING(3,20));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,42), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 30980, 55606);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 29552, 57186);
	ride_ratings_apply_drops(&ratings, ride, 29127, 39009, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 15291, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 15657);
	ride_ratings_apply_scenery(&ratings, ride, 8366);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,30), 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_water_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,70), RIDE_RATING(2,80), RIDE_RATING(2,10));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 20480, 23831, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 25700, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 9760);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 1, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	if (!(ride->special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS))
		ratings.excitement /= 8;

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_air_powered_vertical_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 28;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(4,13), RIDE_RATING(2,50), RIDE_RATING(2,80));
	ride_ratings_apply_length(&ratings, ride, 6000, 327);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,60), RIDE_RATING(0,05));
	ride_ratings_apply_max_speed(&ratings, ride, 509724, 364088, 320398);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 59578);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 21845, 11702);
	ride_ratings_apply_65E277(&ratings, ride, 17893);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 34, 2, 1, 1);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_inverted_hairpin_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(3,00), RIDE_RATING(2,65), RIDE_RATING(2,25));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0, 8));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 102400, 35746, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 29721, 43458, 45749);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 17893);
	ride_ratings_apply_scenery(&ratings, ride, 5577);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,10), 2, 2, 2);
	ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1,50), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 3, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_magic_carpet(rct_ride *ride)
{
	rating_tuple ratings;

	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,45), RIDE_RATING(1,60), RIDE_RATING(2,60));

	ratings.excitement += ride->operation_option * 10;
	ratings.intensity += ride->operation_option * 20;
	ratings.nausea += ride->operation_option * 20;

	ride_ratings_apply_scenery(&ratings, ride, 11155);
	
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_submarine_ride(rct_ride *ride)
{
	rating_tuple ratings;

	ride->unreliability_factor = 7;
	set_unreliability_factor(ride);

	// NOTE Fixed bug from original game, see boat ride.

	ride_ratings_set(&ratings, RIDE_RATING(2,20), RIDE_RATING(1,80), RIDE_RATING(1,40));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_65E277(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 22310);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_river_rafts(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 12;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(1,45), RIDE_RATING(0,25), RIDE_RATING(0,34));
	ride_ratings_apply_length(&ratings, ride, 2000, 7208);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_max_speed(&ratings, ride, 531372, 655360, 301111);
	ride_ratings_apply_duration(&ratings, ride, 500, 13107);
	ride_ratings_apply_65DDD1(&ratings, ride, 22291, 20860, 4574);
	ride_ratings_apply_drops(&ratings, ride, 78643, 93622, 62259);
	ride_ratings_apply_65E277(&ratings, ride, 13420);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
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

	ratings.excitement += ride->operation_option;
	ratings.intensity += ride->operation_option * 16;
	ratings.nausea += ride->operation_option * 16;

	ride_ratings_apply_scenery(&ratings, ride, 19521);
	
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 3 << 5;
}

static void ride_ratings_calculate_inverted_impulse_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 20;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(4,00), RIDE_RATING(3,00), RIDE_RATING(3,20));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,42), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 29789, 55606);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 29552, 57186);
	ride_ratings_apply_drops(&ratings, ride, 29127, 39009, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 15291, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 15657);
	ride_ratings_apply_scenery(&ratings, ride, 9760);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 20, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_mini_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 13;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,55), RIDE_RATING(2,40), RIDE_RATING(1,85));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 20480, 23831, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 25700, 30583, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 9760);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,50), 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_mine_ride(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,75), RIDE_RATING(1,00), RIDE_RATING(1,80));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 40960, 29789, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 29721, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 19275, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 21472);
	ride_ratings_apply_scenery(&ratings, ride, 16732);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x10E0000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_lim_launched_roller_coaster(rct_ride *ride)
{
	rating_tuple ratings;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 25;
	set_unreliability_factor(ride);

	ride_ratings_set(&ratings, RIDE_RATING(2,90), RIDE_RATING(1,50), RIDE_RATING(2,20));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 49648);
	ride_ratings_apply_65DDD1(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_65E1C2(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_65E277(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 10, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, 10, 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

#pragma endregion

#pragma region Ride rating calculation function table

// rct2: 0x0097E050
static const ride_ratings_calculation ride_ratings_calculate_func_table[91] = {
	ride_ratings_calculate_spiral_roller_coaster,				// SPIRAL_ROLLER_COASTER
	ride_ratings_calculate_stand_up_roller_coaster,				// STAND_UP_ROLLER_COASTER
	ride_ratings_calculate_suspended_swinging_coaster,			// SUSPENDED_SWINGING_COASTER
	ride_ratings_calculate_inverted_roller_coaster,				// INVERTED_ROLLER_COASTER
	ride_ratings_calculate_junior_roller_coaster,				// JUNIOR_ROLLER_COASTER
	ride_ratings_calculate_miniature_railway,					// MINIATURE_RAILWAY
	ride_ratings_calculate_monorail,							// MONORAIL
	ride_ratings_calculate_mini_suspended_coaster,				// MINI_SUSPENDED_COASTER
	ride_ratings_calculate_boat_ride,							// BOAT_RIDE
	ride_ratings_calculate_wooden_wild_mouse,					// WOODEN_WILD_MOUSE
	ride_ratings_calculate_steeplechase,						// STEEPLECHASE
	ride_ratings_calculate_car_ride,							// CAR_RIDE
	ride_ratings_calculate_launched_freefall,					// LAUNCHED_FREEFALL
	ride_ratings_calculate_bobsleigh_coaster,					// BOBSLEIGH_COASTER
	ride_ratings_calculate_observation_tower,					// OBSERVATION_TOWER
	ride_ratings_calculate_looping_roller_coaster,				// LOOPING_ROLLER_COASTER
	ride_ratings_calculate_dinghy_slide,						// DINGHY_SLIDE
	ride_ratings_calculate_mine_train_coaster,					// MINE_TRAIN_COASTER
	ride_ratings_calculate_chairlift,							// CHAIRLIFT
	ride_ratings_calculate_corkscrew_roller_coaster,			// CORKSCREW_ROLLER_COASTER
	ride_ratings_calculate_maze,								// MAZE
	ride_ratings_calculate_spiral_slide,						// SPIRAL_SLIDE
	ride_ratings_calculate_go_karts,							// GO_KARTS
	ride_ratings_calculate_log_flume,							// LOG_FLUME
	ride_ratings_calculate_river_rapids,						// RIVER_RAPIDS
	ride_ratings_calculate_dodgems,								// DODGEMS
	ride_ratings_calculate_pirate_ship,							// PIRATE_SHIP
	ride_ratings_calculate_inverter_ship,						// SWINGING_INVERTER_SHIP
	ride_ratings_calculate_food_stall,							// FOOD_STALL
	ride_ratings_calculate_food_stall,							// 1D
	ride_ratings_calculate_drink_stall,							// DRINK_STALL
	ride_ratings_calculate_drink_stall,							// 1F
	ride_ratings_calculate_shop,								// SHOP
	ride_ratings_calculate_merry_go_round,						// MERRY_GO_ROUND
	ride_ratings_calculate_shop,								// 22
	ride_ratings_calculate_information_kiosk,					// INFORMATION_KIOSK
	ride_ratings_calculate_toilets,								// TOILETS
	ride_ratings_calculate_ferris_wheel,						// FERRIS_WHEEL
	ride_ratings_calculate_motion_simulator,					// MOTION_SIMULATOR
	ride_ratings_calculate_3d_cinema,							// 3D_CINEMA
	ride_ratings_calculate_top_spin,							// TOP_SPIN
	ride_ratings_calculate_space_rings,							// SPACE_RINGS
	ride_ratings_calculate_reverse_freefall_coaster,			// REVERSE_FREEFALL_COASTER
	ride_ratings_calculate_lift,								// LIFT
	ride_ratings_calculate_vertical_drop_roller_coaster,		// VERTICAL_DROP_ROLLER_COASTER
	ride_ratings_calculate_cash_machine,						// CASH_MACHINE
	ride_ratings_calculate_twist,								// TWIST
	ride_ratings_calculate_haunted_house,						// HAUNTED_HOUSE
	ride_ratings_calculate_first_aid,							// FIRST_AID
	ride_ratings_calculate_circus_show,							// CIRCUS_SHOW
	ride_ratings_calculate_ghost_train,							// GHOST_TRAIN
	ride_ratings_calculate_twister_roller_coaster,				// TWISTER_ROLLER_COASTER
	ride_ratings_calculate_wooden_roller_coaster,				// WOODEN_ROLLER_COASTER
	ride_ratings_calculate_side_friction_roller_coaster,		// SIDE_FRICTION_ROLLER_COASTER
	ride_ratings_calculate_wild_mouse,							// WILD_MOUSE
	ride_ratings_calculate_multi_dimension_roller_coaster,		// MULTI_DIMENSION_ROLLER_COASTER
	ride_ratings_calculate_multi_dimension_roller_coaster,		// 38
	ride_ratings_calculate_flying_roller_coaster,				// FLYING_ROLLER_COASTER
	ride_ratings_calculate_flying_roller_coaster,				// 3A
	ride_ratings_calculate_virginia_reel,						// VIRGINIA_REEL
	ride_ratings_calculate_splash_boats,						// SPLASH_BOATS
	ride_ratings_calculate_mini_helicopters,					// MINI_HELICOPTERS
	ride_ratings_calculate_lay_down_roller_coaster,				// LAY_DOWN_ROLLER_COASTER
	ride_ratings_calculate_suspended_monorail,					// SUSPENDED_MONORAIL
	ride_ratings_calculate_lay_down_roller_coaster,				// 40
	ride_ratings_calculate_reverser_roller_coaster,				// REVERSER_ROLLER_COASTER
	ride_ratings_calculate_heartline_twister_coaster,			// HEARTLINE_TWISTER_COASTER
	ride_ratings_calculate_mini_golf,							// MINI_GOLF
	ride_ratings_calculate_giga_coaster,						// GIGA_COASTER
	ride_ratings_calculate_roto_drop,							// ROTO_DROP
	ride_ratings_calculate_flying_saucers,						// FLYING_SAUCERS
	ride_ratings_calculate_crooked_house,						// CROOKED_HOUSE
	ride_ratings_calculate_monorail_cycles,						// MONORAIL_CYCLES
	ride_ratings_calculate_compact_inverted_coaster,			// COMPACT_INVERTED_COASTER
	ride_ratings_calculate_water_coaster,						// WATER_COASTER
	ride_ratings_calculate_air_powered_vertical_coaster,		// AIR_POWERED_VERTICAL_COASTER
	ride_ratings_calculate_inverted_hairpin_coaster,			// INVERTED_HAIRPIN_COASTER
	ride_ratings_calculate_magic_carpet,						// MAGIC_CARPET
	ride_ratings_calculate_submarine_ride,						// SUBMARINE_RIDE
	ride_ratings_calculate_river_rafts,							// RIVER_RAFTS
	NULL,														// 50
	ride_ratings_calculate_enterprise,							// ENTERPRISE
	NULL,														// 52
	NULL,														// 53
	NULL,														// 54
	NULL,														// 55
	ride_ratings_calculate_inverted_impulse_coaster,			// INVERTED_IMPULSE_COASTER
	ride_ratings_calculate_mini_roller_coaster,					// MINI_ROLLER_COASTER
	ride_ratings_calculate_mine_ride,							// MINE_RIDE
	NULL,														// 59
	ride_ratings_calculate_lim_launched_roller_coaster,			// LIM_LAUNCHED_ROLLER_COASTER
};

#pragma endregion
