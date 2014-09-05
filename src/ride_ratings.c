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

#include "addresses.h"
#include "ride.h"
#include "ride_data.h"
#include "ride_ratings.h"

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
		upkeep += 3 * ride->var_0C9;
	}

	// slight upkeep boosts for some rides - 5 for mini railroad, 10 for log
	// flume/rapids, 10 for roller coaster, 28 for giga coaster
	upkeep += rideUnknownData3[ride->type] * ride->num_stations;

	if (ride->mode == RIDE_MODE_REVERSE_INCLINED_SHUTTLE) {
		upkeep += 30;
	} else if (ride->mode == RIDE_MODE_POWERED_LAUNCH) {
		upkeep += 160;
	} else if (ride->mode == RIDE_MODE_LIM_POWERED_LAUNCH) {
		upkeep += 320;
	} else if (ride->mode == RIDE_MODE_POWERED_LAUNCH2 || 
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
		uint16 ax = ride->totalAirTime;
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
    uint8 al = ride->var_1CD;
    // No idea what this address is; maybe like compensation of some kind? The
    // maximum possible value?
    // List of ride names/values is here: 
    // https://gist.github.com/kevinburke/5eebcda14d94e6ee99c0
    al -= RCT2_ADDRESS(0x0097D7C9, uint8)[4 * ride->type];
    al = al << 1;
    ride->var_198 += al;
}
