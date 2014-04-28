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
#include "peep.h"
#include "rct2.h"
#include "sprite.h"

int peep_get_staff_count()
{
	uint16 sprite_index;
	rct_peep *peep;
	int count = 0;

	sprite_index = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16);
	while (sprite_index != SPRITE_INDEX_NULL) {
		peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_index].peep);
		sprite_index = peep->next;

		if (peep->type == PEEP_TYPE_STAFF)
			count++;
	}

	return count;
}

/**
 *
 *  rct2: 0x0068F0A9
 */
void peep_update_all()
{
	int i;
	uint16 sprite_index;
	rct_peep* peep;	

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)
		return;

	sprite_index = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16);
	i = 0;
	while (sprite_index != 0xFFFF) {
		peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_index].peep);
		sprite_index = peep->next;

		if ((i & 0x7F) != (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 0x7F)) {
			RCT2_CALLPROC_X(0x0068FC1E, 0, 0, 0, 0, peep, 0, 0);
		} else {
			RCT2_CALLPROC_X(0x0068F41A, 0, 0, 0, i, peep, 0, 0);
			if (peep->var_08 == 4)
				RCT2_CALLPROC_X(0x0068FC1E, 0, 0, 0, 0, peep, 0, 0);
		}

		i++;
	}
}
