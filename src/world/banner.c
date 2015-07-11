/*****************************************************************************
 * Copyright (c) 2014 Ted John, Matthias Lanzinger
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
#include "../game.h"
#include "../localisation/localisation.h"
#include "../ride/ride.h"
#include "banner.h"
#include "map.h"

rct_banner *gBanners = (rct_banner*)0x0135A124;

/**
 *
 * rct2: 0x006B9CB0
 */
void banner_init()
{
	for (int i = 0; i < MAX_BANNERS; i++) {
		gBanners[i].type = BANNER_NULL;
	}
}

/**
 * Creates a new banner and returns the index of the banner
 * If the flag GAME_COMMAND_FLAG_APPLY is NOT set then returns
 * the first unused index but does NOT mark the banner as created.
 * returns 0xFF on failure.
 *
 *  rct2: 0x006BA278
 */
int create_new_banner(uint8 flags)
{
	int banner_index = 0;
	for (; banner_index < MAX_BANNERS; banner_index++){
		if (gBanners[banner_index].type == BANNER_NULL){
			break;
		}
	}

	if (banner_index == MAX_BANNERS){
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_TOO_MANY_BANNERS_IN_GAME;
		return BANNER_NULL;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY){
		rct_banner* banner = &gBanners[banner_index];

		banner->flags = 0;
		banner->type = 0;
		banner->string_idx = 778;
		banner->colour = 2;
		banner->text_colour = 2;
	}
	return banner_index;
}

rct_map_element *banner_get_map_element(int bannerIndex)
{
	rct_banner *banner = &gBanners[bannerIndex];
	rct_map_element *mapElement = map_get_first_element_at(banner->x, banner->y);
	do {
		if (map_element_get_banner_index(mapElement) == bannerIndex) {
			return mapElement;
		}
	} while (!map_element_is_last_for_tile(mapElement++));
	return NULL;
}

/**
 *
 *  rct2: 0x006B7EAB
 */
static int banner_get_ride_index_at(int x, int y, int z)
{
	rct_map_element *mapElement;
	rct_ride *ride;
	int rideIndex, resultRideIndex;

	resultRideIndex = -1;
	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		rideIndex = mapElement->properties.track.ride_index;
		ride = GET_RIDE(rideIndex);
		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
			continue;

		if ((mapElement->clearance_height * 8) + 32 <= z)
			continue;

		resultRideIndex = rideIndex;
	} while (!map_element_is_last_for_tile(mapElement++));

	return resultRideIndex;
}

/**
 *
 *  rct2: 0x006B7D86
 */
int banner_get_closest_ride_index(int x, int y, int z)
{
	int i, rideIndex;
	rct_ride *ride;

	static const rct_xy16 NeighbourCheckOrder[] = {
		{  32,   0 },
		{ -32,   0 },
		{   0,  32 },
		{   0, -32 },
		{ -32, +32 },
		{ +32, -32 },
		{ +32, +32 },
		{ -32, +32 },
		{   0,   0 }
	};

	for (i = 0; i < countof(NeighbourCheckOrder); i++) {
		rideIndex = banner_get_ride_index_at(x + NeighbourCheckOrder[i].x, y + NeighbourCheckOrder[i].y, z);
		if (rideIndex != -1) {
			return rideIndex;
		}
	}

	rideIndex = -1;
	int resultDistance = INT_MAX;
	FOR_ALL_RIDES(i, ride) {
		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
			continue;

		uint16 xy = ride->overall_view;
		if (xy == 0xFFFF)
			continue;

		int rideX = (xy & 0xFF) * 32;
		int rideY = (xy >> 8) * 32;
		int distance = abs(x - rideX) + abs(y - rideY);
		if (distance < resultDistance) {
			resultDistance = distance;
			rideIndex = i;
		}
	}

	return rideIndex;
}
