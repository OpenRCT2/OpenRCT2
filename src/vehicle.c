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
#include "audio.h"
#include "ride.h"
#include "sprite.h"
#include "vehicle.h"
#include "viewport.h"

static void vehicle_update(rct_vehicle *vehicle);

/**
*
*  rct2: 0x006BB9FF
*/
void sub_6BB9FF(rct_vehicle* vehicle)
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) && (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 4) || RCT2_GLOBAL(0x0141F570, uint8) == 6)) {
		if (vehicle->var_BB != (uint8)-1 || vehicle->var_BD != (uint8)-1) {
			if (vehicle->var_16.width != 0x8000) {
				RCT2_GLOBAL(0x009AF5A0, rct_widthheight) = vehicle->var_16;
				RCT2_GLOBAL(0x009AF5A4, rct_widthheight) = vehicle->view;
				sint16 v4 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_x;
				sint16 v5 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_y;
				sint16 v6 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_width / 4;
				sint16 v7 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_height / 4;
				if (!RCT2_GLOBAL(0x00F438A8, rct_window*)->classification) {
					v4 -= v6;
					v5 -= v7;
				}
				if (v4 < RCT2_GLOBAL(0x009AF5A4, rct_widthheight).width && v5 < RCT2_GLOBAL(0x009AF5A4, rct_widthheight).height) {
					sint16 t8 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_width + v4;
					sint16 t9 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_height + v5;
					if (!RCT2_GLOBAL(0x00F438A8, rct_window*)->classification) {
						t8 += v6 + v6;
						t9 += v7 + v7;
					}
					if (t8 >= RCT2_GLOBAL(0x009AF5A0, rct_widthheight).width && t9 >= RCT2_GLOBAL(0x009AF5A0, rct_widthheight).height) {
						uint16 v9 = sub_6BC2F3(vehicle);
						rct_sound_unknown* i;
						for (i = RCT2_ADDRESS(0x00F438B4, rct_sound_unknown); i < RCT2_GLOBAL(0x00F438B0, rct_sound_unknown*) && v9 <= i->next; i++);
						if (i < RCT2_ADDRESS(0x00F43908, rct_sound_unknown)) { // 0x00F43908 is end of rct_sound_unknown list, which has 7 elements, not to be confused with variable at 0x00F43908
							if (RCT2_GLOBAL(0x00F438B0, rct_sound_unknown*) < RCT2_ADDRESS(0x00F43908, rct_sound_unknown)) {
								RCT2_GLOBAL(0x00F438B0, rct_sound_unknown*)++;
							}
							rct_sound_unknown* j = RCT2_GLOBAL(0x00F438B0, rct_sound_unknown*) - 1;
							while (j >= i) {
								*(j + 1) = *j;
								j--;
							}
							i->next = v9;
							rct_widthheight v12;
							v12.height = vehicle->var_16.height;
							v12.width = ((uint16)RCT2_GLOBAL(0x009AF5A0, rct_widthheight).width / 2) + ((uint16)RCT2_GLOBAL(0x009AF5A4, rct_widthheight).width / 2) - RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_x;
							v12.width >>= RCT2_GLOBAL(0x00F438A4, rct_viewport*)->zoom;
							v12.width += RCT2_GLOBAL(0x00F438A4, rct_viewport*)->x;

							uint16 v14 = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
							if (v14 < 64) {
								v14 = 64;
							}
							rct_widthheight v15;
							i->var_2 = (((v12.width << 16) / v14) - 0x8000) >> 4;
							v15.width = 0;
							v15.height = (RCT2_GLOBAL(0x009AF5A0, rct_widthheight).height / 2) + (RCT2_GLOBAL(0x009AF5A4, rct_widthheight).height / 2) - RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_y;
							v15.height >>= RCT2_GLOBAL(0x00F438A4, rct_viewport*)->zoom;
							v15.height += RCT2_GLOBAL(0x00F438A4, rct_viewport*)->y;

							uint16 v18 = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);
							if (v18 < 64) {
								v18 = 64;
							}
							i->var_4 = (sint16)((v15.both / v18) - 0x8000) >> 4;
							sint32 v19 = vehicle->var_28;

							int testaddr = (vehicle->var_31 * 0x65);
							testaddr += (int)RCT2_ADDRESS(0x009ACFA4, rct_ride_type*)[vehicle->var_D6];
							uint8 test = ((uint8*)testaddr)[0x74];

							if (test & 1) {
								v19 *= 2;
							}
							if (v19 < 0) {
								v19 = -v19;
							}
							v19 >>= 5;
							v19 *= 0x1588;
							v19 >>= 14;
							v19 += 0x2B11;
							v19 += 16 * vehicle->var_BF;
							i->frequency = (uint16)v19;
							i->id = vehicle->sprite_index;
							i->var_8 = 0;
							if (vehicle->x != 0x8000) {
								uint16 v22 = (vehicle->y & 0xFFE0) << 8;
								v22 |= (vehicle->x & 0xFFE0 | v22) & 0xFFFF;
								rct_map_element* map_element;
								for (map_element = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[v22 >> 3]; map_element->type & MAP_ELEMENT_TYPE_MASK; map_element++);
								if (map_element->base_height * 8 > vehicle->z) {
									i->var_8 = 0x30;
								}
							}
						}
					}
				}
			}
		}
	}
}


/**
*
*  rct2: 0x006BC2F3
*/
int sub_6BC2F3(rct_vehicle* vehicle)
{
	int result = 0;
	rct_vehicle* vehicle_temp = vehicle;
	do {
		result += vehicle_temp->var_46;
	} while (vehicle_temp->next_vehicle_on_train != (uint16)-1 && (vehicle_temp = GET_VEHICLE(vehicle_temp->next_vehicle_on_train)));
	sint32 v4 = vehicle->var_28;
	if (v4 < 0) {
		v4 = -v4;
	}
	result += ((uint16)v4) >> 13;
	rct_vehicle_sound* vehicle_sound = RCT2_ADDRESS(RCT2_ADDRESS_VEHICLE_SOUND_LIST, rct_vehicle_sound);
	while (vehicle_sound->id != vehicle->sprite_index) {
		vehicle_sound++;
		if (vehicle_sound >= RCT2_GLOBAL(0x009AF42C, rct_vehicle_sound*)) {
			return result;
		}
	}
	return result + 300;
}

/**
 * 
 *  rct2: 0x006D4204
 */
void vehicle_update_all()
{
	uint16 sprite_index;
	rct_vehicle *vehicle;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
		return;

	if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 4) && RCT2_GLOBAL(0x0141F570, uint8) != 6)
		return;


	sprite_index = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_VEHICLE, uint16);
	while (sprite_index != SPRITE_INDEX_NULL) {
		vehicle = &(g_sprite_list[sprite_index].vehicle);
		sprite_index = vehicle->next;

		vehicle_update(vehicle);
	}
}

/**
 * 
 *  rct2: 0x006D77F2
 */
static void vehicle_update(rct_vehicle *vehicle)
{
	RCT2_CALLPROC_X(0x006D77F2, 0, 0, 0, 0, (int)vehicle, 0, 0);
}