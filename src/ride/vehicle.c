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
#include "../audio/mixer.h"
#include "../config.h"
#include "../hook.h"
#include "../interface/viewport.h"
#include "../openrct2.h"
#include "../world/sprite.h"
#include "ride.h"
#include "ride_data.h"
#include "track.h"
#include "vehicle.h"

static void vehicle_update(rct_vehicle *vehicle);

/**
*
*  rct2: 0x006BB9FF
*/
void vehicle_update_sound_params(rct_vehicle* vehicle)
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) && (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER) || RCT2_GLOBAL(0x0141F570, uint8) == 6)) {
		if (vehicle->sound1_id != (uint8)-1 || vehicle->sound2_id != (uint8)-1) {
			if (vehicle->sprite_left != (sint16)0x8000) {
				RCT2_GLOBAL(0x009AF5A0, sint16) = vehicle->sprite_left;
				RCT2_GLOBAL(0x009AF5A2, sint16) = vehicle->sprite_top;
				RCT2_GLOBAL(0x009AF5A4, sint16) = vehicle->sprite_right;
				RCT2_GLOBAL(0x009AF5A6, sint16) = vehicle->sprite_bottom;
				sint16 v4 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_x;
				sint16 v5 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_y;
				sint16 v6 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_width / 4;
				sint16 v7 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_height / 4;
				if (!RCT2_GLOBAL(0x00F438A8, rct_window*)->classification) {
					v4 -= v6;
					v5 -= v7;
				}
				if (v4 < RCT2_GLOBAL(0x009AF5A4, sint16) && v5 < RCT2_GLOBAL(0x009AF5A6, sint16)) {
					sint16 t8 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_width + v4;
					sint16 t9 = RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_height + v5;
					if (!RCT2_GLOBAL(0x00F438A8, rct_window*)->classification) {
						t8 += v6 + v6;
						t9 += v7 + v7;
					}
					if (t8 >= RCT2_GLOBAL(0x009AF5A0, sint16) && t9 >= RCT2_GLOBAL(0x009AF5A2, sint16)) {
						uint16 v9 = sub_6BC2F3(vehicle);
						rct_vehicle_sound_params* i;
						for (i = &gVehicleSoundParamsList[0]; i < gVehicleSoundParamsListEnd && v9 <= i->var_A; i++);
						if (i < &gVehicleSoundParamsList[countof(gVehicleSoundParamsList)]) {
							if (gVehicleSoundParamsListEnd < &gVehicleSoundParamsList[countof(gVehicleSoundParamsList)]) {
								gVehicleSoundParamsListEnd++;
							}
							rct_vehicle_sound_params* j = gVehicleSoundParamsListEnd - 1;
							while (j >= i) {
								j--;
								*(j + 1) = *j;
							}
							i->var_A = v9;
							int pan_x = (RCT2_GLOBAL(0x009AF5A0, sint16) / 2) + (RCT2_GLOBAL(0x009AF5A4, sint16) / 2) - RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_x;
							pan_x >>= RCT2_GLOBAL(0x00F438A4, rct_viewport*)->zoom;
							pan_x += RCT2_GLOBAL(0x00F438A4, rct_viewport*)->x;

							uint16 screenwidth = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
							if (screenwidth < 64) {
								screenwidth = 64;
							}
							i->pan_x = ((((pan_x << 16) / screenwidth) - 0x8000) >> 4);

							int pan_y = (RCT2_GLOBAL(0x009AF5A2, sint16) / 2) + (RCT2_GLOBAL(0x009AF5A6, sint16) / 2) - RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_y;
							pan_y >>= RCT2_GLOBAL(0x00F438A4, rct_viewport*)->zoom;
							pan_y += RCT2_GLOBAL(0x00F438A4, rct_viewport*)->y;

							uint16 screenheight = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);
							if (screenheight < 64) {
								screenheight = 64;
							}
							i->pan_y = ((((pan_y << 16) / screenheight) - 0x8000) >> 4);

							sint32 v19 = vehicle->velocity;

							rct_ride_type* ride_type = GET_RIDE_ENTRY(vehicle->ride_subtype);
							uint8 test = ride_type->vehicles[vehicle->vehicle_type].var_5A;

							if (test & 1) {
								v19 *= 2;
							}
							if (v19 < 0) {
								v19 = -v19;
							}
							v19 >>= 5;
							v19 *= 5512;
							v19 >>= 14;
							v19 += 11025;
							v19 += 16 * vehicle->var_BF;
							i->frequency = (uint16)v19;
							i->id = vehicle->sprite_index;
							i->volume = 0;
							if (vehicle->x != (sint16)0x8000) {
								int tile_idx = (((vehicle->y & 0xFFE0) * 256) + (vehicle->x & 0xFFE0)) / 32;
								rct_map_element* map_element;
								for (map_element = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[tile_idx]; map_element->type & MAP_ELEMENT_TYPE_MASK; map_element++);
								if (map_element->base_height * 8 > vehicle->z) { // vehicle underground
									i->volume = 0x30;
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
		result += vehicle_temp->friction;
	} while (vehicle_temp->next_vehicle_on_train != (uint16)-1 && (vehicle_temp = GET_VEHICLE(vehicle_temp->next_vehicle_on_train)));
	sint32 v4 = vehicle->velocity;
	if (v4 < 0) {
		v4 = -v4;
	}
	result += ((uint16)v4) >> 13;
	rct_vehicle_sound* vehicle_sound = &gVehicleSoundList[0];

	while (vehicle_sound->id != vehicle->sprite_index) {
		vehicle_sound++;

		if (vehicle_sound >= &gVehicleSoundList[countof(gVehicleSoundList)]) {
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
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) != -1 && !gGameSoundsOff && gConfigSound.sound && !gOpenRCT2Headless) {
		RCT2_GLOBAL(0x00F438A4, rct_viewport*) = (rct_viewport*)-1;
		rct_viewport* viewport = (rct_viewport*)-1;
		rct_window* window = RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*);
		while (1) {
			window--;
			if (window < RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_LIST, rct_window)) {
				break;
			}
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
			gVehicleSoundParamsListEnd = &gVehicleSoundParamsList[0];
			for (uint16 i = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_VEHICLE, uint16); i != SPRITE_INDEX_NULL; i = g_sprite_list[i].vehicle.next) {
				vehicle_update_sound_params(&g_sprite_list[i].vehicle);
			}
			for(int i = 0; i < countof(gVehicleSoundList); i++){
				rct_vehicle_sound* vehicle_sound = &gVehicleSoundList[i];
				if (vehicle_sound->id != (uint16)-1) {
					for (rct_vehicle_sound_params* vehicle_sound_params = &gVehicleSoundParamsList[0]; vehicle_sound_params != gVehicleSoundParamsListEnd; vehicle_sound_params++) {
						if (vehicle_sound->id == vehicle_sound_params->id) {
							goto label26;
						}
					}
					if (vehicle_sound->sound1_id != (uint16)-1) {
						Mixer_Stop_Channel(vehicle_sound->sound1_channel);
					}
					if (vehicle_sound->sound2_id != (uint16)-1) {
						Mixer_Stop_Channel(vehicle_sound->sound2_channel);
					}
					vehicle_sound->id = (uint16)-1;
				}
			label26:
				;
			}

			for (rct_vehicle_sound_params* vehicle_sound_params = &gVehicleSoundParamsList[0]; ; vehicle_sound_params++) {
			label28:
				if (vehicle_sound_params >= gVehicleSoundParamsListEnd) {
					return;
				}
				uint8 vol1 = 0xFF;
				uint8 vol2 = 0xFF;
				sint16 pan_y = vehicle_sound_params->pan_y;
				if (pan_y < 0) {
					pan_y = -pan_y;
				}
				if (pan_y > 0xFFF) {
					pan_y = 0xFFF;
				}
				pan_y -= 0x800;
				if (pan_y > 0) {
					pan_y -= 0x400;
					pan_y = -pan_y;
					pan_y = pan_y / 4;
					vol1 = LOBYTE(pan_y);
					if ((sint8)HIBYTE(pan_y) != 0) {
						vol1 = 0xFF;
						if ((sint8)HIBYTE(pan_y) < 0) {
							vol1 = 0;
						}
					}
				}

				sint16 pan_x = vehicle_sound_params->pan_x;
				if (pan_x < 0) {
					pan_x = -pan_x;
				}
				if (pan_x > 0xFFF) {
					pan_x = 0xFFF;
				}
				pan_x -= 0x800;
				if (pan_x > 0) {
					pan_x -= 0x400;
					pan_x = -pan_x;
					pan_x = pan_x / 4;
					vol2 = LOBYTE(pan_x);
					if ((sint8)HIBYTE(pan_x) != 0) {
						vol2 = 0xFF;
						if ((sint8)HIBYTE(pan_x) < 0) {
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

				rct_vehicle_sound* vehicle_sound = &gVehicleSoundList[0];
				while (vehicle_sound_params->id != vehicle_sound->id) {
					vehicle_sound++;
					if (vehicle_sound >= &gVehicleSoundList[countof(gVehicleSoundList)]) {
						vehicle_sound = &gVehicleSoundList[0];
						int i = 0;
						while (vehicle_sound->id != (uint16)-1) {
							vehicle_sound++;
							i++;
							if (i >= countof(gVehicleSoundList)) {
								vehicle_sound_params = (rct_vehicle_sound_params*)((int)vehicle_sound_params + 10);
								goto label28;
							}
						}
						vehicle_sound->id = vehicle_sound_params->id;
						vehicle_sound->sound1_id = (uint16)-1;
						vehicle_sound->sound2_id = (uint16)-1;
						vehicle_sound->volume = 0x30;
						break;
					}
				}

				int tempvolume = vehicle_sound->volume;
				if (tempvolume != vehicle_sound_params->volume) {
					if (tempvolume < vehicle_sound_params->volume) {
						tempvolume += 4;
					} else {
						tempvolume -= 4;
					}
				}
				vehicle_sound->volume = tempvolume;
				if (vol1 < tempvolume) {
					vol1 = 0;
				} else {
					vol1 = vol1 - tempvolume;
				}

				// do sound1 stuff, track noise
				rct_sprite* sprite = &g_sprite_list[vehicle_sound_params->id];
				int volume = sprite->vehicle.sound1_volume;
				volume *= vol1;
				volume = volume / 8;
				volume -= 0x1FFF;
				if (volume < -10000) {
					volume = -10000;
				}
				if (sprite->vehicle.sound1_id == (uint8)-1) {
					if (vehicle_sound->sound1_id != (uint16)-1) {
						vehicle_sound->sound1_id = -1;
						Mixer_Stop_Channel(vehicle_sound->sound1_channel);
					}
				} else {
					if (vehicle_sound->sound1_id == (uint16)-1) {
						goto label69;
					}
					if (sprite->vehicle.sound1_id != vehicle_sound->sound1_id) {
						Mixer_Stop_Channel(vehicle_sound->sound1_channel);
					label69:
						vehicle_sound->sound1_id = sprite->vehicle.sound1_id;
						vehicle_sound->sound1_pan = vehicle_sound_params->pan_x;
						vehicle_sound->sound1_volume = volume;
						vehicle_sound->sound1_freq = vehicle_sound_params->frequency;
						uint16 frequency = vehicle_sound_params->frequency;
						if (RCT2_ADDRESS(0x009AF51F, uint8)[2 * sprite->vehicle.sound1_id] & 2) {
							frequency = (frequency / 2) + 4000;
						}
						uint8 looping = RCT2_ADDRESS(0x009AF51E, uint8)[2 * sprite->vehicle.sound1_id];
						int pan = vehicle_sound_params->pan_x;
						vehicle_sound->sound1_channel = Mixer_Play_Effect(sprite->vehicle.sound1_id, looping ? MIXER_LOOP_INFINITE : MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(pan), DStoMixerRate(frequency), 0);
						goto label87;
					}
					if (volume != vehicle_sound->sound1_volume) {
						vehicle_sound->sound1_volume = volume;
						Mixer_Channel_Volume(vehicle_sound->sound1_channel, DStoMixerVolume(volume));
					}
					if (vehicle_sound_params->pan_x != vehicle_sound->sound1_pan) {
						vehicle_sound->sound1_pan = vehicle_sound_params->pan_x;
						Mixer_Channel_Pan(vehicle_sound->sound1_channel, DStoMixerPan(vehicle_sound_params->pan_x));
					}
					if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 3) && vehicle_sound_params->frequency != vehicle_sound->sound1_freq) {
						vehicle_sound->sound1_freq = vehicle_sound_params->frequency;
						uint16 frequency = vehicle_sound_params->frequency;
						if (RCT2_GLOBAL(0x009AF51F, uint8*)[2 * sprite->vehicle.sound1_id] & 2) {
							frequency = (frequency / 2) + 4000;
						}
						Mixer_Channel_Rate(vehicle_sound->sound1_channel, DStoMixerRate(frequency));
					}
				}
			label87: // do sound2 stuff, screams
				sprite = &g_sprite_list[vehicle_sound_params->id];
				volume = sprite->vehicle.sound2_volume;
				volume *= vol1;
				volume = (uint16)volume / 8;
				volume -= 0x1FFF;
				if (volume < -10000) {
					volume = -10000;
				}
				if (sprite->vehicle.sound2_id == (uint8)-1) {
					if (vehicle_sound->sound2_id != (uint16)-1) {
						vehicle_sound->sound2_id = -1;
						Mixer_Stop_Channel(vehicle_sound->sound2_channel);
					}
				} else {
					if (vehicle_sound->sound2_id == (uint16)-1) {
						goto label93;
					}
					if (sprite->vehicle.sound2_id != vehicle_sound->sound2_id) {
						Mixer_Stop_Channel(vehicle_sound->sound2_channel);
					label93:
						vehicle_sound->sound2_id = sprite->vehicle.sound2_id;
						vehicle_sound->sound2_pan = vehicle_sound_params->pan_x;
						vehicle_sound->sound2_volume = volume;
						vehicle_sound->sound2_freq = vehicle_sound_params->frequency;
						uint16 frequency = vehicle_sound_params->frequency;
						if (RCT2_ADDRESS(0x009AF51F, uint8)[2 * sprite->vehicle.sound2_id] & 1) {
							frequency = 12649;
						}
						frequency = (frequency * 2) - 3248;
						if (frequency > 25700) {
							frequency = 25700;
						}
						uint8 looping = RCT2_ADDRESS(0x009AF51E, uint8)[2 * sprite->vehicle.sound2_id];
						int pan = vehicle_sound_params->pan_x;
						vehicle_sound->sound2_channel = Mixer_Play_Effect(sprite->vehicle.sound2_id, looping ? MIXER_LOOP_INFINITE : MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(pan), DStoMixerRate(frequency), 0);
						goto label114;
					}
					if (volume != vehicle_sound->sound2_volume) {
						Mixer_Channel_Volume(vehicle_sound->sound2_channel, DStoMixerVolume(volume));
						vehicle_sound->sound2_volume = volume;
					}
					if (vehicle_sound_params->pan_x != vehicle_sound->sound2_pan) {
						vehicle_sound->sound2_pan = vehicle_sound_params->pan_x;
						Mixer_Channel_Pan(vehicle_sound->sound2_channel, DStoMixerPan(vehicle_sound_params->pan_x));
					}
					if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 3) && vehicle_sound_params->frequency != vehicle_sound->sound2_freq) {
						vehicle_sound->sound2_freq = vehicle_sound_params->frequency;
						if (!(RCT2_ADDRESS(0x009AF51F, uint8)[2 * sprite->vehicle.sound2_id] & 1)) {
							uint16 frequency = (vehicle_sound_params->frequency * 2) - 3248;
							if (frequency > 25700) {
								frequency = 25700;
							}
							Mixer_Channel_Rate(vehicle_sound->sound2_channel, DStoMixerRate(frequency));
						}
					}
				}
			label114:
				;
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

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR)
		return;

	if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER) && RCT2_GLOBAL(0x0141F570, uint8) != 6)
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

void vehicle_set_map_toolbar(rct_vehicle *vehicle)
{
	rct_ride *ride;
	int vehicleIndex;

	ride = GET_RIDE(vehicle->ride);

	while (vehicle->is_child) {
		vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
	}

	for (vehicleIndex = 0; vehicleIndex < 32; vehicleIndex++)
		if (ride->vehicles[vehicleIndex] == vehicle->sprite_index)
			break;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 0, uint16) = 2215;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 2, uint16) = 1165;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 4, uint16) = ride->name;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 6, uint32) = ride->name_arguments;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 10, uint16) = RideNameConvention[ride->type].vehicle_name + 2;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 12, uint16) = vehicleIndex + 1;

	int arg0, arg1;
	ride_get_status(vehicle->ride, &arg0, &arg1);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 14, uint16) = (uint16)arg0;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS + 16, uint32) = (uint16)arg1;
}

rct_vehicle *vehicle_get_head(rct_vehicle *vehicle)
{
	rct_vehicle *prevVehicle;

	for (;;) {
		prevVehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
		if (prevVehicle->next_vehicle_on_train == SPRITE_INDEX_NULL)
			break;

		vehicle = prevVehicle;
	}

	return vehicle;
}

rct_vehicle *vehicle_get_tail(rct_vehicle *vehicle)
{
	uint16 spriteIndex;

	while ((spriteIndex = vehicle->next_vehicle_on_train) != SPRITE_INDEX_NULL) {
		vehicle = GET_VEHICLE(spriteIndex);
	}
	return vehicle;
}

int vehicle_is_used_in_pairs(rct_vehicle *vehicle)
{
	return vehicle->num_seats & VEHICLE_SEAT_PAIR_FLAG;
}

/**
 *
 *  rct2: 0x006DEF56
 */
void sub_6DEF56(rct_vehicle *cableLift)
{
	RCT2_CALLPROC_X(0x006DEF56, 0, 0, 0, 0, (int)cableLift, 0, 0);
}

rct_vehicle *cable_lift_segment_create(int rideIndex, int x, int y, int z, int direction, uint16 var_44, uint32 var_24, bool head)
{
	rct_ride *ride = GET_RIDE(rideIndex);
	rct_vehicle *current = &(create_sprite(1)->vehicle);
	current->sprite_identifier = SPRITE_IDENTIFIER_VEHICLE;
	current->ride = rideIndex;
	current->ride_subtype = 0xFF;
	if (head) {
		move_sprite_to_list((rct_sprite*)current, SPRITE_LINKEDLIST_OFFSET_VEHICLE);
		ride->cable_lift = current->sprite_index;
	}
	current->is_child = head ? 0 : 1;
	current->var_44 = var_44;
	current->var_24 = var_24;
	current->sprite_width = 10;
	current->sprite_height_negative = 10;
	current->sprite_height_positive = 10;
	current->friction = 100;
	current->num_seats = 0;
	current->speed = 20;
	current->acceleration = 80;
	current->velocity = 0;
	current->var_2C = 0;
	current->var_4A = 0;
	current->var_4C = 0;
	current->var_4E = 0;
	current->var_B5 = 0;
	current->var_BA = 0;
	current->var_B6 = 0;
	current->var_B8 = 0;
	current->sound1_id = 0xFF;
	current->sound2_id = 0xFF;
	current->var_C4 = 0;
	current->var_C5 = 0;
	current->var_C8 = 0;
	current->scream_sound_id = 0xFF;
	current->var_1F = 0;
	current->var_20 = 0;
	for (int j = 0; j < 32; j++) {
		current->peep[j] = SPRITE_INDEX_NULL;
	}
	current->var_CD = 0;
	current->sprite_direction = direction << 3;
	current->track_x = x;
	current->track_y = y;

	z = z * 8;
	current->track_z = z;
	z += RCT2_GLOBAL(0x0097D21A + (ride->type * 8), uint8);

	sprite_move(16, 16, z, (rct_sprite*)current);
	current->track_type = (TRACK_ELEM_CABLE_LIFT_HILL << 2) | (current->sprite_direction >> 3);
	current->var_34 = 164;
	current->update_flags = VEHICLE_UPDATE_FLAG_1;
	current->status = VEHICLE_STATUS_MOVING_TO_END_OF_STATION;
	current->var_51 = 0;
	current->num_peeps = 0;
	current->next_free_seat = 0;
	return current;
}

/**
 *
 *  rct2: 0x006DD365
 */
bool sub_6DD365(rct_vehicle *vehicle)
{
	registers regs;
	regs.esi = (int)vehicle;

	return RCT2_CALLFUNC_Y(0x006DD365, &regs) & 0x100;
}

// 0x0x009A2970
const sint32 *dword_9A2970 = (sint32*)0x009A2970;

/**
 *
 *  rct2: 0x006DAB6F
 */
static void sub_6DAB4C_chunk_1(rct_vehicle *vehicle)
{
	rct_ride_type_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);
	int verticalG, lateralG;

	RCT2_GLOBAL(0x00F64E2C, uint8) = 0;
	RCT2_GLOBAL(0x00F64E04, rct_vehicle*) = vehicle;
	RCT2_GLOBAL(0x00F64E18, uint32) = 0;
	RCT2_GLOBAL(0x00F64E1C, uint32) = 0xFFFFFFFF;

	if (vehicleEntry->var_12 & (1 << 1)) {
		int trackType = vehicle->track_type >> 2;
		if (trackType < 68 || trackType >= 87) {
			vehicle_get_g_forces(vehicle, &verticalG, &lateralG);
			lateralG = abs(lateralG);
			if (lateralG <= 150) {
				if (dword_9A2970[vehicle->var_1F] < 0) {
					if (verticalG > -40) {
						return;
					}
				} else if (verticalG > -80) {
					return;
				}
			}

			if (vehicle->var_1F != 8) {
				RCT2_GLOBAL(0x00F64E18, uint32) |= (1 << 6);
			}
		}
	} else if (vehicleEntry->var_12 & (1 << 2)) {
		int trackType = vehicle->track_type >> 2;
		if (trackType < 68 || trackType >= 87) {
			vehicle_get_g_forces(vehicle, &verticalG, &lateralG);

			if (dword_9A2970[vehicle->var_1F] < 0) {
				if (verticalG > -45) {
					return;
				}
			} else {
				if (verticalG > -80) {
					return;
				}
			}

			if (vehicle->var_1F != 8 && vehicle->var_1F != 55) {
				RCT2_GLOBAL(0x00F64E18, uint32) |= (1 << 6);
			}
		}
	}
}

/**
 *
 *  rct2: 0x006DAC43
 */
static void sub_6DAB4C_chunk_2(rct_vehicle *vehicle)
{
	rct_ride *ride = GET_RIDE(vehicle->ride);
	rct_ride_type_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);

	if (vehicleEntry->var_14 & (1 << 12)) {
		sint32 velocity = ride->speed << 16;
		if (RCT2_GLOBAL(0x00F64E34, uint8) == 0) {
			velocity = 0;
		}
		vehicle->velocity = velocity;
		vehicle->var_2C = 0;
	}
	
	int trackType = vehicle->track_type >> 2;
	switch (trackType) {
	case 1:
	case 216:
		if (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT || ride_is_block_sectioned(ride)) {
			break;
		}
		return;
	case 9:
	case 63:
	case 123:
	case 147:
	case 155:
		if (ride_is_block_sectioned(ride)) {
			break;
		}
		return;
	default:
		return;
	}

	rct_map_element *trackElement =  map_get_track_element_at_of_type(
		vehicle->track_x,
		vehicle->track_y,
		vehicle->track_z >> 3,
		trackType
	);
	if (trackType == 1) {
		if (trackElement->flags & (1 << 5)) {
			RCT2_GLOBAL(0x00F64E18, uint32) |= 0x400;
		}
	} else if (trackType == 123 || trackType == 216 || track_element_is_lift_hill(trackElement)) {
		if (!(trackElement->flags & (1 << 5))) {
			if (trackType == 216 && vehicle->velocity >= 0) {
				if (vehicle->velocity <= 0x20364) {
					vehicle->velocity = 0x20364;
					vehicle->var_2C = 0;
				} else {
					vehicle->velocity -= vehicle->velocity >> 4;
					vehicle->var_2C = 0;
				}
			}
			return;
		}
		RCT2_GLOBAL(0x00F64E18, uint32) |= 0x400;
		vehicle->var_2C = 0;
		if (vehicle->velocity <= 0x20000) {
			vehicle->velocity = 0;
		}
		vehicle->velocity -= vehicle->velocity >> 3;
	}
}

/**
 *
 *  rct2: 0x006DADAE
 */
static void sub_6DAB4C_chunk_3(rct_vehicle *vehicle)
{
	sint32 nextVelocity = vehicle->var_2C + vehicle->velocity;
	if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_7) {
		nextVelocity = 0;
	}
	if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_10) {
		vehicle->var_D2--;
		if (vehicle->var_D2 == -70) {
			vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_10;
		}
		if (vehicle->var_D2 >= 0) {
			nextVelocity = 0;
			vehicle->var_2C = 0;
		}
	}
	vehicle->velocity = nextVelocity;

	RCT2_GLOBAL(0x00F64E08, sint32) = nextVelocity;
	RCT2_GLOBAL(0x00F64E0C, sint32) = (nextVelocity >> 10) * 42;
}

/**
 *
 *  rct2: 0x006D6776
 */
static void sub_6D6776(rct_vehicle *vehicle)
{
	rct_ride_type_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);
	RCT2_CALLPROC_X(0x006D6776, 0, 0, 0, 0, (int)vehicle, (int)vehicleEntry, 0);
}

/**
 *
 *  rct2: 0x006D661F
 */
static void sub_6D661F(rct_vehicle *vehicle)
{
	RCT2_CALLPROC_X(0x006D661F, 0, 0, 0, 0, (int)vehicle, 0, 0);
}

/**
 *
 *  rct2: 0x006D63D4
 */
static void sub_6D63D4(rct_vehicle *vehicle)
{
	RCT2_CALLPROC_X(0x006D63D4, 0, 0, 0, 0, (int)vehicle, 0, 0);
}

/**
 *
 *  rct2: 0x006DAB4C
 */
int sub_6DAB4C(rct_vehicle *vehicle, int *outStation)
{
	registers regs;

	rct_ride *ride = GET_RIDE(vehicle->ride);
	rct_ride_type *rideEntry = GET_RIDE_ENTRY(vehicle->ride_subtype);
	rct_ride_type_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);
	
	// esi = vehicle
	// eax = rideEntry
	// edi = vehicleEntry

	if (vehicleEntry->var_12 & (1 << 3)) {
		goto loc_6DC3A7;
	}

	sub_6DAB4C_chunk_1(vehicle);
	sub_6DAB4C_chunk_2(vehicle);
	sub_6DAB4C_chunk_3(vehicle);

	if (RCT2_GLOBAL(0x00F64E08, sint32) < 0) {
		vehicle = vehicle_get_tail(vehicle);
	}
	RCT2_GLOBAL(0x00F64E00, rct_vehicle*) = vehicle;

loc_6DAE27:
	if (vehicleEntry->var_14 & (1 << 1)) {
		sub_6D6776(vehicle);
	}
	if (vehicleEntry->var_14 & (1 << 2)) {
		sub_6D661F(vehicle);
	}
	if ((vehicleEntry->var_14 & (1 << 7)) || (vehicleEntry->var_14 & (1 << 8))) {
		sub_6D63D4(vehicle);
	}
	vehicle->var_2C = dword_9A2970[vehicle->var_1F];
	RCT2_GLOBAL(0x00F64E10, uint32) = 1;

	regs.eax = RCT2_GLOBAL(0x00F64E0C, sint32) + vehicle->var_24;
	vehicle->var_24 = regs.eax;
	if (regs.eax < 0) {
		goto loc_6DBA13;
	}
	if (regs.eax < 0x368A) {
		goto loc_6DBF3E;
	}
	vehicle->var_B8 &= ~(1 << 1);
	RCT2_GLOBAL(0x00F64E20, uint32) = vehicle->x | (vehicle->y << 16);
	RCT2_GLOBAL(0x00F64E24, uint16) = vehicle->z;
	invalidate_sprite_2((rct_sprite*)vehicle);

loc_6DAEB9:
	regs.edi = vehicle->track_type;
	regs.cx = vehicle->track_type >> 2;
	
	int trackType = vehicle->track_type >> 2;
	if (trackType == 197 || trackType == 198) {
		if (vehicle->var_34 == 80) {
			vehicle->vehicle_type ^= 1;
			vehicleEntry = vehicle_get_vehicle_entry(vehicle);
		}
		if (RCT2_GLOBAL(0x00F64E08, sint32) >= 0x40000) {
			vehicle->var_2C = -RCT2_GLOBAL(0x00F64E08, sint32) * 8;
		} else if (RCT2_GLOBAL(0x00F64E08, sint32) < 0x20000) {
			vehicle->var_2C = 0x50000;
		}
	} else if (trackType == TRACK_ELEM_BRAKES) {
		if (!(
			ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN &&
			ride->breakdown_reason_pending != BREAKDOWN_BRAKES_FAILURE &&
			ride->mechanic_status == RIDE_MECHANIC_STATUS_4
		)) {
			regs.eax = vehicle->var_CF << 16;
			if (regs.eax < RCT2_GLOBAL(0x00F64E08, sint32)) {
				vehicle->var_2C = -RCT2_GLOBAL(0x00F64E08, sint32) * 16;
			} else if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 0x0F)) {
				if (RCT2_GLOBAL(0x00F64E2C, uint8) == 0) {
					RCT2_GLOBAL(0x00F64E2C, uint8)++;
					sound_play_panned(SOUND_51, 0x8001, vehicle->x, vehicle->y, vehicle->z);
				}
			}
		}
	}

	if ((trackType == TRACK_ELEM_FLAT && ride->type == RIDE_TYPE_REVERSE_FREEFALL_COASTER) ||
		(trackType == TRACK_ELEM_POWERED_LIFT)
	) {
		vehicle->var_2C = RCT2_GLOBAL(0x0097CF40 + (ride->type * 8) + 7, uint8) << 10;
	}
	if (trackType == TRACK_ELEM_BRAKE_FOR_DROP) {
		if (!vehicle->is_child) {
			if (!(vehicle->update_flags & VEHICLE_UPDATE_FLAG_10)) {
				if (vehicle->var_34 >= 8) {
					vehicle->var_2C = -RCT2_GLOBAL(0x00F64E08, sint32) * 16;
					if (vehicle->var_34 >= 24) {
						vehicle->update_flags |= VEHICLE_UPDATE_FLAG_10;
						vehicle->var_D2 = 90;
					}
				}
			}
		}
	}
	if (trackType == TRACK_ELEM_LOG_FLUME_REVERSER) {
		if (vehicle->var_34 != 16 || vehicle->velocity < 0x40000) {
			if (vehicle->var_34 == 32) {
				vehicle->vehicle_type = vehicleEntry->var_58;
				vehicleEntry = vehicle_get_vehicle_entry(vehicle);
			}
		} else {
			vehicle->var_34 += 17;
		}
	}

loc_6DB06B:
	regs.esi = vehicle;
	regs.edi = vehicle->track_type;
	RCT2_CALLFUNC_Y(0x006DB06B, &regs);
	goto end;

loc_6DBA13:
	regs.esi = vehicle;
	RCT2_CALLFUNC_Y(0x006DBA13, &regs);
	goto end;

loc_6DBF3E:
	regs.esi = vehicle;
	RCT2_CALLFUNC_Y(0x006DBF3E, &regs);
	goto end;

loc_6DC3A7:
	regs.esi = vehicle;
	RCT2_CALLFUNC_Y(0x006DC3A7, &regs);

end:
	hook_setreturnregisters(&regs);
	return regs.eax;
}

rct_ride_type_vehicle *vehicle_get_vehicle_entry(rct_vehicle *vehicle)
{
	rct_ride_type *rideEntry = GET_RIDE_ENTRY(vehicle->ride_subtype);
	return &rideEntry->vehicles[vehicle->vehicle_type];
}
