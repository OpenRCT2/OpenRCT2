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
#include "../audio/audio.h"
#include "../interface/viewport.h"
#include "../world/sprite.h"
#include "ride.h"
#include "vehicle.h"

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
						for (i = RCT2_ADDRESS(0x00F438B4, rct_sound_unknown); i < RCT2_GLOBAL(0x00F438B0, rct_sound_unknown*) && v9 <= i->var_A; i++);
						if (i < RCT2_ADDRESS(0x00F43908, rct_sound_unknown)) { // 0x00F43908 is end of rct_sound_unknown list, which has 7 elements, not to be confused with variable at 0x00F43908
							if (RCT2_GLOBAL(0x00F438B0, rct_sound_unknown*) < RCT2_ADDRESS(0x00F43908, rct_sound_unknown)) {
								RCT2_GLOBAL(0x00F438B0, rct_sound_unknown*)++;
							}
							rct_sound_unknown* j = RCT2_GLOBAL(0x00F438B0, rct_sound_unknown*) - 1;
							while (j >= i) {
								j--;
								*(j + 1) = *j;
							}
							i->var_A = v9;
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
							i->pan = (((((uint32)v12.width << 16) / v14) - 0x8000) >> 4);
							v15.width = 0;
							v15.height = (RCT2_GLOBAL(0x009AF5A0, rct_widthheight).height / 2) + (RCT2_GLOBAL(0x009AF5A4, rct_widthheight).height / 2) - RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_y;
							v15.height >>= RCT2_GLOBAL(0x00F438A4, rct_viewport*)->zoom;
							v15.height += RCT2_GLOBAL(0x00F438A4, rct_viewport*)->y;

							uint16 v18 = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);
							if (v18 < 64) {
								v18 = 64;
							}
							i->var_4 = (sint16)(((v15.both / v18) - 0x8000) >> 4);
							sint32 v19 = vehicle->velocity;

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
	sint32 v4 = vehicle->velocity;
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
*  rct2: 0x006BBC6B
*/
void vehicle_sounds_update()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) != -1 && !RCT2_GLOBAL(0x009AF59C, uint8) && RCT2_GLOBAL(0x009AF59D, uint8) & 1) {
		RCT2_GLOBAL(0x00F438A4, rct_viewport*) = (rct_viewport*)-1;
		rct_window* window = RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*);
		rct_viewport* viewport = (rct_viewport*)-1;
		for (window = RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*); window >= RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_LIST, rct_window); window--) {
			viewport = window->viewport;
			if (viewport && viewport->flags & VIEWPORT_FLAG_SOUND_ON) {
				break;
			}
		}
		RCT2_GLOBAL(0x00F438A4, rct_viewport*) = viewport;
		if (viewport != (rct_viewport*)-1) {
			if (window) {
				RCT2_GLOBAL(0x00F438A8, rct_window*) = window;
				RCT2_GLOBAL(RCT2_ADDRESS_VOLUME_ADJUST_ZOOM, uint8) = 0;
				if (viewport->zoom) {
					RCT2_GLOBAL(RCT2_ADDRESS_VOLUME_ADJUST_ZOOM, uint8) = 35;
					if (viewport->zoom != 1) {
						RCT2_GLOBAL(RCT2_ADDRESS_VOLUME_ADJUST_ZOOM, uint8) = 70;
					}
				}
			}
		//label12:
			RCT2_GLOBAL(0x00F438B0, rct_sound_unknown**) = &RCT2_GLOBAL(0x00F438B4, rct_sound_unknown*);
			for (uint16 i = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_VEHICLE, uint16); i != SPRITE_INDEX_NULL; i = g_sprite_list[i].vehicle.next) {
				sub_6BB9FF(&g_sprite_list[i].vehicle);
			}
			RCT2_ADDRESS_VEHICLE_SOUND_LIST;
			for (rct_vehicle_sound* vehicle_sound = &RCT2_GLOBAL(RCT2_ADDRESS_VEHICLE_SOUND_LIST, rct_vehicle_sound); vehicle_sound != &RCT2_GLOBAL(0x009AF42C, rct_vehicle_sound); vehicle_sound++) {
				if (vehicle_sound->id != (uint16)-1) {
					for (rct_sound_unknown* sound_unknown = &RCT2_GLOBAL(0x00F438B4, rct_sound_unknown); sound_unknown != RCT2_GLOBAL(0x00F438B0, rct_sound_unknown*); sound_unknown++) {
						if (vehicle_sound->id == sound_unknown->id) {
							goto label26;
						}
					}
					if (vehicle_sound->sound1_id != (uint16)-1) {
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_stop(&vehicle_sound->sound1);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
					}
					if (vehicle_sound->sound2_id != (uint16)-1) {
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_stop(&vehicle_sound->sound2);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
					}
					vehicle_sound->id = (uint16)-1;
				}
			label26:
				1;
			}

			for (rct_sound_unknown* sound_unknown = &RCT2_GLOBAL(0x00F438B4, rct_sound_unknown); ; sound_unknown++) {
			label28:
				if (sound_unknown >= RCT2_GLOBAL(0x00F438B0, rct_sound_unknown*)) {
					return;
				}
				uint8 vol1 = 0xFF;
				uint8 vol2 = 0xFF;
				sint16 v = sound_unknown->var_4;
				if (v < 0) {
					v = -v;
				}
				if (v > 0xFFF) {
					v = 0xFFF;
				}
				v -= 0x800;
				if (v > 0) {
					v -= 0x400;
					v = -v;
					v = (uint16)v / 4;
					vol1 = LOBYTE(v);
					if (HIBYTE(v) != 0) {
						vol1 = 0xFF;
						if (HIBYTE(v) < 0) {
							vol1 = 0;
						}
					}
				}

				sint16 w = sound_unknown->pan;
				if (w < 0) {
					w = -w;
				}
				if (w > 0xFFF) {
					w = 0xFFF;
				}
				w -= 0x800;
				if (w > 0) {
					w -= 0x400;
					w = -w;
					w = (uint16)w / 4;
					vol2 = LOBYTE(w);
					if (HIBYTE(w) != 0) {
						vol2 = 0xFF;
						if (HIBYTE(w) < 0) {
							vol2 = 0;
						}
					}
				}

				if (vol1 >= vol2) {
					vol1 = vol2;
				}
				if (vol1 < RCT2_GLOBAL(RCT2_ADDRESS_VOLUME_ADJUST_ZOOM, uint8)) {
					vol1 = 0;
				} else {
					vol1 = vol1 - RCT2_GLOBAL(RCT2_ADDRESS_VOLUME_ADJUST_ZOOM, uint8);
				}

				rct_vehicle_sound* vehicle_sound = &RCT2_GLOBAL(RCT2_ADDRESS_VEHICLE_SOUND_LIST, rct_vehicle_sound);
				while (sound_unknown->id != vehicle_sound->id) {
					vehicle_sound++;
					if (vehicle_sound >= &RCT2_GLOBAL(0x009AF42C, rct_vehicle_sound)) {
						vehicle_sound = &RCT2_GLOBAL(RCT2_ADDRESS_VEHICLE_SOUND_LIST, rct_vehicle_sound); 
						int i = 0;
						while (vehicle_sound->id != (uint16)-1) {
							vehicle_sound++;
							i++;
							if (i >= RCT2_GLOBAL(0x009AAC75, uint8)) {
								sound_unknown = (rct_sound_unknown*)((int)sound_unknown + 10);
								goto label28;
							}
						}
						vehicle_sound->id = sound_unknown->id;
						vehicle_sound->sound1_id = (uint16)-1;
						vehicle_sound->sound2_id = (uint16)-1;
						vehicle_sound->var_2 = 0x30;
						break;
					}
				}

				uint8 v21 = LOBYTE(sound_unknown->var_8);
				uint8 v22 = LOBYTE(vehicle_sound->var_2);
				if (v22 != v21) {
					if (v22 < v21) {
						v22 += 4;
					} else {
						v22 -= 4;
					}
				}
				vehicle_sound->var_2 = v22;
				if (vol1 < v22) {
					vol1 = 0;
				} else {
					vol1 = vol1 - v22;
				}
				// do sound1 stuff, track noise
				rct_sprite* sprite = &g_sprite_list[sound_unknown->id];
				sint16 volume = sprite->vehicle.var_BC;
				volume *= vol1;
				volume = (uint16)volume / 8;
				volume -= 0x1FFF;
				if (volume < -10000) {
					volume = -10000;
				}
				if (sprite->vehicle.var_BB == (uint8)-1) {
					if (vehicle_sound->sound1_id != (uint16)-1) {
						vehicle_sound->sound1_id = -1;
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_stop(&vehicle_sound->sound1);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
					}
				} else {
					if (vehicle_sound->sound1_id == (uint16)-1) {
						goto label69;
					}
					if (sprite->vehicle.var_BB != vehicle_sound->sound1_id) {
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_stop(&vehicle_sound->sound1);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
					label69:
						vehicle_sound->sound1_id = sprite->vehicle.var_BB;
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_prepare(sprite->vehicle.var_BB, &vehicle_sound->sound1, 1, RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_SW_BUFFER, uint32));
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
						vehicle_sound->sound1_pan = sound_unknown->pan;
						vehicle_sound->sound1_volume = volume;
						vehicle_sound->sound1_freq = sound_unknown->frequency;
						uint16 frequency = sound_unknown->frequency;
						if (RCT2_ADDRESS(0x009AF51F, uint8)[2 * sprite->vehicle.var_BB] & 2) {
							frequency = (frequency / 2) + 4000;
						}
						uint8 looping = RCT2_ADDRESS(0x009AF51E, uint8)[2 * sprite->vehicle.var_BB];
						int pan = sound_unknown->pan;
						if (!RCT2_GLOBAL(0x009AAC6D, uint8)) {
							pan = 0;
						}
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_play(&vehicle_sound->sound1, looping, volume, pan, frequency);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
						goto label87;
					}
					if (volume != vehicle_sound->sound1_volume) {
						vehicle_sound->sound1_volume = volume;
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_set_volume(&vehicle_sound->sound1, volume);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
					}
					if (sound_unknown->pan != vehicle_sound->sound1_pan) {
						vehicle_sound->sound1_pan = sound_unknown->pan;
						if (RCT2_GLOBAL(0x009AAC6D, uint8)) {
							RCT2_GLOBAL(0x014241BC, uint32) = 1;
							sound_set_pan(&vehicle_sound->sound1, sound_unknown->pan);
							RCT2_GLOBAL(0x014241BC, uint32) = 0;
						}
					}
					if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 3) && sound_unknown->frequency != vehicle_sound->sound1_freq) {
						vehicle_sound->sound1_freq = sound_unknown->frequency;
						uint16 frequency = sound_unknown->frequency;
						if (RCT2_GLOBAL(0x009AF51F, uint8*)[2 * sprite->vehicle.var_BB] & 2) {
							frequency = (frequency / 2) + 4000;
						}
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_set_frequency(&vehicle_sound->sound1, frequency);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
					}
				}
			label87: // do sound2 stuff, screams
				sprite = &g_sprite_list[sound_unknown->id];
				volume = sprite->vehicle.var_BE;
				volume *= vol1;
				volume = (uint16)volume / 8;
				volume -= 0x1FFF;
				if (volume < -10000) {
					volume = -10000;
				}
				if (sprite->vehicle.var_BD == (uint8)-1) {
					if (vehicle_sound->sound2_id != (uint16)-1) {
						vehicle_sound->sound2_id = -1;
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_stop(&vehicle_sound->sound2);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
					}
				} else {
					if (vehicle_sound->sound2_id == (uint16)-1) {
						goto label93;
					}
					if (sprite->vehicle.var_BD != vehicle_sound->sound2_id) {
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_stop(&vehicle_sound->sound2);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
					label93:
						vehicle_sound->sound2_id = sprite->vehicle.var_BD;
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_prepare(sprite->vehicle.var_BD, &vehicle_sound->sound2, 1, RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_SW_BUFFER, uint32));
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
						vehicle_sound->sound2_pan = sound_unknown->pan;
						vehicle_sound->sound2_volume = volume;
						vehicle_sound->sound2_freq = sound_unknown->frequency;
						uint16 frequency = sound_unknown->frequency;
						if (RCT2_ADDRESS(0x009AF51F, uint8)[2 * sprite->vehicle.var_BD] & 1) {
							frequency = 12649;
						}
						frequency = (frequency * 2) - 3248;
						if (frequency > 25700) {
							frequency = 25700;
						}
						uint8 looping = RCT2_ADDRESS(0x009AF51E, uint8)[2 * sprite->vehicle.var_BD];
						int pan = sound_unknown->pan;
						if (!RCT2_GLOBAL(0x009AAC6D, uint8)) {
							pan = 0;
						}
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_play(&vehicle_sound->sound2, looping, volume, pan, frequency);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
						goto label114;
					}
					if (volume != vehicle_sound->sound2_volume) {
						vehicle_sound->sound2_volume = volume;
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_set_volume(&vehicle_sound->sound2, volume);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
					}
					if (sound_unknown->pan != vehicle_sound->sound2_pan) {
						vehicle_sound->sound2_pan = sound_unknown->pan;
						if (RCT2_GLOBAL(0x009AAC6D, uint8)) {
							RCT2_GLOBAL(0x014241BC, uint32) = 1;
							sound_set_pan(&vehicle_sound->sound2, sound_unknown->pan);
							RCT2_GLOBAL(0x014241BC, uint32) = 0;
						}
					}
					if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 3) && sound_unknown->frequency != vehicle_sound->sound2_freq) {
						vehicle_sound->sound2_freq = sound_unknown->frequency;
						if (!(RCT2_ADDRESS(0x009AF51F, uint8)[2 * sprite->vehicle.var_BD] & 1)) {
							uint16 frequency = (sound_unknown->frequency * 2) - 3248;
							if (frequency > 25700) {
								frequency = 25700;
							}
							RCT2_GLOBAL(0x014241BC, uint32) = 1;
							sound_set_frequency(&vehicle_sound->sound2, frequency);
							RCT2_GLOBAL(0x014241BC, uint32) = 0;
						}
					}
				}
			label114:
				1;
			}
		}
	}
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

/**
 * 
 *  rct2: 0x006D73D0
 * ax: verticalG
 * dx: lateralG
 * esi: vehicle
 */
void vehicle_get_g_forces(rct_vehicle *vehicle, int *verticalG, int *lateralG)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;

	esi = (int)vehicle;
	RCT2_CALLFUNC_X(0x006D73D0, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	
	if (verticalG != NULL) *verticalG = (sint16)(eax & 0xFFFF);
	if (lateralG != NULL) *lateralG = (sint16)(edx & 0xFFFF);
}