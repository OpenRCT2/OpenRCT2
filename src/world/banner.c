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
