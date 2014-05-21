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

#include "ride.h"

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

    excitement = 215;
    intensity = 62;
    nausea = 34;

    // NB this should get marked out by the compiler, if it's smart.
    excitement = apply_intensity_penalty(excitement, intensity);
	rating_tuple tup = per_ride_rating_adjustments(ride, excitement, intensity, nausea);
	excitement = tup.excitement;
	intensity = tup.intensity;
	nausea = tup.nausea;

	ride->excitement = excitement;
	ride->intensity = intensity;
	ride->nausea = nausea;
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
rating_tuple per_ride_rating_adjustments(rct_ride *ride, sint16 excitement, 
		sint16 intensity, sint16 nausea)
{
	// NB: The table here is allocated dynamically. Reading the exe will tell
	// you nothing
	subtype_p = RCT2_GLOBAL(0x009ACFA4 + ride->subtype * 4, uint32);

	// example value here: 12 (?)
	sint16 ctr = RCT2_GLOBAL(subtype_p + 0x1b2);
	excitement = excitement + ((excitement * ctr) >> 7);

	sint16 ctr = RCT2_GLOBAL(subtype_p + 0x1b3);
	intensity = intensity + ((intensity * ctr) >> 7);

	sint16 ctr = RCT2_GLOBAL(subtype_p + 0x1b4);
	nausea = nausea + ((nausea * ctr) >> 7);

	// As far as I can tell, this flag detects whether the ride is a roller
	// coaster, or a log flume or rapids. Everything else it's not set.
	// more detail: https://gist.github.com/kevinburke/d951e74e678b235eef3e
	uint16 ridetype_var = RCT2_GLOBAL(ride->type * 8, uint16);
	if (ridetype_var & 0x80) {
		uint16 ax = ride->var_1F4;
		if (RCT2_GLOBAL(subtype_p + 8, uint32) & 0x800) {
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
	rating_tuple tup = {excitement, intensity, nausea};
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
sint16 apply_intensity_penalty(sint16 excitement, sint16 intensity)
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
    ride->var198 += al;
}
