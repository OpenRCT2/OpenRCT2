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

#ifndef _BANNER_H_
#define _BANNER_H_

#include "../common.h"
#include "../world/map.h"

#define BANNER_NULL 255
#define MAX_BANNERS 250

typedef struct {
	uint8 type;
	uint8 flags; //0x01 bit 0 is no entry
	rct_string_id string_idx; //0x02
	uint8 colour; //0x04
	uint8 text_colour; //0x05
	uint8 x; //0x06
	uint8 y; //0x07
} rct_banner;

enum{
	BANNER_FLAG_NO_ENTRY = (1 << 0),
	BANNER_FLAG_1 = (1 << 1),
	BANNER_FLAG_2 = (1 << 2)
} BANNER_FLAGS;

extern rct_banner *gBanners;

void banner_init();
int create_new_banner(uint8 flags);
rct_map_element *banner_get_map_element(int bannerIndex);

#endif
