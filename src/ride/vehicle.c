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
#include "../game.h"
#include "../audio/audio.h"
#include "../audio/mixer.h"
#include "../config.h"
#include "../hook.h"
#include "../interface/viewport.h"
#include "../openrct2.h"
#include "../scenario.h"
#include "../world/sprite.h"
#include "../world/scenery.h"
#include "track.h"
#include "ride.h"
#include "ride_data.h"
#include "track.h"
#include "vehicle.h"

static void vehicle_update(rct_vehicle *vehicle);

static void vehicle_ride_null_update_moving_to_end_of_station(rct_vehicle *vehicle);
static void vehicle_ride_null_update_waiting_to_depart(rct_vehicle *vehicle);
static void vehicle_ride_null_update_departing(rct_vehicle *vehicle);
static void vehicle_ride_null_update_travelling(rct_vehicle *vehicle);
static void vehicle_ride_null_update_arriving(rct_vehicle *vehicle);

static void vehicle_update_showing_film(rct_vehicle *vehicle);
static void vehicle_update_doing_circus_show(rct_vehicle *vehicle);
static void vehicle_update_moving_to_end_of_station(rct_vehicle *vehicle);
static void vehicle_update_waiting_for_passengers(rct_vehicle* vehicle);
static void vehicle_update_waiting_to_depart(rct_vehicle* vehicle);
static void vehicle_update_bumpcar_mode(rct_vehicle* vehicle);
static void vehicle_update_swinging(rct_vehicle* vehicle);
static void vehicle_update_waiting_for_cable_lift(rct_vehicle *vehicle);
static void vehicle_update_crash(rct_vehicle *vehicle);

static void vehicle_update_sound(rct_vehicle *vehicle);
static int vehicle_update_scream_sound(rct_vehicle *vehicle);

#define NO_SCREAM 254

const uint8 byte_9A3A14[] = { SOUND_SCREAM_8, SOUND_SCREAM_1 };
const uint8 byte_9A3A16[] = { SOUND_SCREAM_1, SOUND_SCREAM_6 };
const uint8 byte_9A3A18[] = {
	SOUND_SCREAM_3, SOUND_SCREAM_1, SOUND_SCREAM_5, SOUND_SCREAM_6,
	SOUND_SCREAM_7, SOUND_SCREAM_2, SOUND_SCREAM_4, SOUND_LIFT_1
};

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

/* rct2: 0x006D6956 
 * returns 0 when all closed
 */
static int vehicle_close_restraints(rct_vehicle* vehicle){
	rct_ride* ride = GET_RIDE(vehicle->ride);
	int ebp = 0;
	uint16 vehicle_id = vehicle->sprite_index;

	do {
		vehicle = GET_VEHICLE(vehicle_id);
		if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_BROKEN_CAR &&
			vehicle->restraints_position != 0 &&
			(
			ride->breakdown_reason_pending == BREAKDOWN_RESTRAINTS_STUCK_OPEN ||
			ride->breakdown_reason_pending == BREAKDOWN_DOORS_STUCK_OPEN)
			){

			if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)){

				ride->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;

				ride_breakdown_add_news_item(vehicle->ride);

				ride->window_invalidate_flags |=
					RIDE_INVALIDATE_RIDE_MAIN |
					RIDE_INVALIDATE_RIDE_LIST |
					RIDE_INVALIDATE_RIDE_MAINTENANCE;

				ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;

				rct_vehicle* broken_vehicle = GET_VEHICLE(ride->vehicles[ride->broken_vehicle]);
				ride->inspection_station = broken_vehicle->current_station;

				ride->breakdown_reason = ride->breakdown_reason_pending;
			}
		}
		else{
			if (vehicle->restraints_position - 20 < 0){
				vehicle->restraints_position = 0;
				continue;
			}
			vehicle->restraints_position -= 20;
		}		
		invalidate_sprite_2((rct_sprite*)vehicle);
		ebp++;
	} while ((vehicle_id = vehicle->next_vehicle_on_train) != 0xFFFF);

	return ebp;
}

/* rct2: 0x006D6A2C 
 * returns 0 when all open
 */
static int vehicle_open_restraints(rct_vehicle* vehicle){
	//return (RCT2_CALLPROC_X(0x006d6a2c, 0, 0, 0, 0, (int)vehicle, 0, 0) & 0x100);

	int ebp = 0;
	uint16 vehicle_id = vehicle->sprite_index;

	do {
		vehicle = GET_VEHICLE(vehicle_id);

		vehicle->var_4C = 0;
		vehicle->var_4E = 0;
		vehicle->var_4A = 0;

		rct_ride* ride = GET_RIDE(vehicle->ride);
		rct_ride_type* rideEntry = GET_RIDE_ENTRY(vehicle->ride_subtype);
		rct_ride_type_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

		if (vehicleEntry->var_14 & (1 << 2)) {
			if (abs(vehicle->var_B6) <= 700 &&
				!(vehicle->var_BA & 0x30) &&
				(
					!(vehicleEntry->var_12 & (1 << 14)) ||
					!(vehicle->var_BA & 0xF8))
				){
				vehicle->var_B6 = 0;
			}
			else {
				ebp++;

				if (abs(vehicle->var_B6) < 600) {
					vehicle->var_B6 = 600;
				}
				sint16 value = vehicle->var_B6 / 256;
				vehicle->var_BA += value;
				vehicle->var_B6 -= value;

				invalidate_sprite_2((rct_sprite*)vehicle);
				continue;
			}
		}
		if (vehicleEntry->var_11 == 6 &&
			vehicle->var_C5 != 0){

			if (vehicle->var_C8 + 0x3333 < 0xFFFF){
				vehicle->var_C8 = vehicle->var_C8 + 0x3333 - 0xFFFF;
				vehicle->var_C5++;
				vehicle->var_C5 &= 7;
				invalidate_sprite_2((rct_sprite*)vehicle);
			}
			else{
				vehicle->var_C8 += 0x3333;
			}
			ebp++;
			continue;
		}

		if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_BROKEN_CAR &&
			vehicle->restraints_position != 0xFF &&
			(
			ride->breakdown_reason_pending == BREAKDOWN_RESTRAINTS_STUCK_CLOSED ||
			ride->breakdown_reason_pending == BREAKDOWN_DOORS_STUCK_CLOSED)
			){

			if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)){

				ride->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;

				ride_breakdown_add_news_item(vehicle->ride);

				ride->window_invalidate_flags |=
					RIDE_INVALIDATE_RIDE_MAIN |
					RIDE_INVALIDATE_RIDE_LIST |
					RIDE_INVALIDATE_RIDE_MAINTENANCE;

				ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;

				rct_vehicle* broken_vehicle = GET_VEHICLE(ride->vehicles[ride->broken_vehicle]);
				ride->inspection_station = broken_vehicle->current_station;

				ride->breakdown_reason = ride->breakdown_reason_pending;
			}

		}
		else{
			if (vehicle->restraints_position + 20 > 0xFF){
				vehicle->restraints_position = 255;
				continue;
			}
			vehicle->restraints_position += 20;
			invalidate_sprite_2((rct_sprite*)vehicle);
			ebp++;
		}

	} while ((vehicle_id = vehicle->next_vehicle_on_train) != 0xFFFF);

	return ebp;
}

/* rct2: 0x006D6D1F */
static void vehicle_update_measurements(rct_vehicle *vehicle)
{
	rct_ride *ride;

	ride = GET_RIDE(vehicle->ride);
	//RCT2_CALLPROC_X(0x006D6D1F, 0, 0, 0, 0, (int)vehicle, (int)vehicle->ride * sizeof(rct_ride), 0);
	//return;

	if (vehicle->status == VEHICLE_STATUS_TRAVELLING_07){
		ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
		ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
		ride->lifecycle_flags &= ~RIDE_LIFECYCLE_TEST_IN_PROGRESS;
		vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_TESTING;
		window_invalidate_by_number(WC_RIDE, vehicle->ride);
		return;
	}

	uint8 entrance = ride->var_1F6;
	if (ride->entrances[entrance] != 0xFFFF){

		//ecx
		uint8 var_E0 = ride->var_0E0;

		ride->var_0E1++;
		if (ride->var_0E1 >= 32)ride->var_0E1 = 0;

		sint32 velocity = abs(vehicle->velocity);
		if (velocity > ride->max_speed){
			ride->max_speed = velocity;
		}

		if (ride->var_0E1 == 0 && velocity > 0x8000){
			ride->average_speed += velocity;
			ride->time[var_E0]++;
		}

		sint32 distance = abs(((vehicle->velocity + vehicle->var_2C) >> 10) * 42);
		if (vehicle->var_CE == 0){
			ride->length[var_E0] += distance;
		}

		if (RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride->type * 2] & RIDE_TYPE_FLAG_HAS_G_FORCES){
			int vertical_g, lateral_g;
			vehicle_get_g_forces(vehicle, &vertical_g, &lateral_g);

			vertical_g += ride->previous_vertical_g;
			lateral_g += ride->previous_lateral_g;
			vertical_g >>= 1;
			lateral_g >>= 1;

			ride->previous_vertical_g = vertical_g;
			ride->previous_lateral_g = lateral_g;

			if (vertical_g <= 0){
				ride->total_air_time++;
			}

			if (vertical_g > ride->max_positive_vertical_g)
				ride->max_positive_vertical_g = vertical_g;

			if (vertical_g < ride->max_negative_vertical_g)
				ride->max_negative_vertical_g = vertical_g;

			lateral_g = abs(lateral_g);

			if (lateral_g > ride->max_lateral_g)
				ride->max_lateral_g = lateral_g;
		}
	}

	uint16 map_location = (vehicle->track_x / 32) | ((vehicle->track_y / 32) << 8);
	if (vehicle->track_z / 8 != ride->var_11F || map_location != ride->var_10C){
		ride->var_11F = vehicle->track_z / 8;
		ride->var_10C = map_location;

		if (ride->entrances[ride->var_1F6] == 0xFFFF)
			return;

		uint16 track_elem_type = vehicle->track_type / 4;
		if (track_elem_type == TRACK_ELEM_POWERED_LIFT || (vehicle->update_flags & VEHICLE_UPDATE_FLAG_0)){
			if (!(ride->testing_flags & RIDE_TESTING_POWERED_LIFT)){
				ride->testing_flags |= RIDE_TESTING_POWERED_LIFT;
				if (ride->drops + 64 < 0xFF){
					ride->drops += 64;
				}
			}
		}
		else{
			ride->testing_flags &= ~RIDE_TESTING_POWERED_LIFT;
		}

		if (ride->type == RIDE_TYPE_WATER_COASTER){
			if (track_elem_type >= TRACK_ELEM_FLAT_COVERED && track_elem_type <= TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED){
				ride->special_track_elements |= RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
			}
		}

		switch (track_elem_type)
		{
		case TRACK_ELEM_RAPIDS:
		case TRACK_ELEM_SPINNING_TUNNEL:
			ride->special_track_elements |= RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
			break;
		case TRACK_ELEM_WATERFALL:
		case TRACK_ELEM_LOG_FLUME_REVERSER:
			ride->special_track_elements |= RIDE_ELEMENT_REVERSER_OR_WATERFALL;
			break;
		case TRACK_ELEM_WHIRLPOOL:
			ride->special_track_elements |= RIDE_ELEMENT_WHIRLPOOL;
			break;
		case TRACK_ELEM_WATER_SPLASH:
			if (vehicle->velocity >= 0xB0000){
				ride->special_track_elements |= RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
			}
		}

		// ax
		uint16 track_flags = RCT2_ADDRESS(0x0099423C, uint16)[track_elem_type];

		uint32 testing_flags = ride->testing_flags;
		if (testing_flags & RIDE_TESTING_TURN_LEFT && 
			track_flags & TRACK_ELEM_FLAG_TURN_LEFT){
			// 0x800 as this is masked to CURRENT_TURN_COUNT_MASK
			ride->turn_count_default += 0x800;
		}
		else if (testing_flags & RIDE_TESTING_TURN_RIGHT && 
			track_flags & TRACK_ELEM_FLAG_TURN_RIGHT){
			// 0x800 as this is masked to CURRENT_TURN_COUNT_MASK
			ride->turn_count_default += 0x800;
		}
		else if (testing_flags & RIDE_TESTING_TURN_RIGHT || 
			testing_flags & RIDE_TESTING_TURN_LEFT){

			ride->testing_flags &= ~(
				RIDE_TESTING_TURN_LEFT | 
				RIDE_TESTING_TURN_RIGHT | 
				RIDE_TESTING_TURN_BANKED | 
				RIDE_TESTING_TURN_SLOPED);

			uint8 turn_type = 1;
			if (!(testing_flags & RIDE_TESTING_TURN_BANKED)){
				turn_type = 2;
				if (!(testing_flags & RIDE_TESTING_TURN_SLOPED)){
					turn_type = 0;
				}
			}
			switch (ride->turn_count_default >> 11){
			case 0:
				increment_turn_count_1_element(ride, turn_type);
				break;
			case 1:
				increment_turn_count_2_elements(ride, turn_type);
				break;
			case 2:
				increment_turn_count_3_elements(ride, turn_type);
				break;			
			default:
				increment_turn_count_4_plus_elements(ride, turn_type);
				break;
			}
		}
		else {
			if (track_flags & TRACK_ELEM_FLAG_TURN_LEFT){
				ride->testing_flags |= RIDE_TESTING_TURN_LEFT;
				ride->turn_count_default &= ~CURRENT_TURN_COUNT_MASK;

				if (track_flags & TRACK_ELEM_FLAG_TURN_BANKED){
					ride->testing_flags |= RIDE_TESTING_TURN_BANKED;
				}
				if (track_flags & TRACK_ELEM_FLAG_TURN_SLOPED){
					ride->testing_flags |= RIDE_TESTING_TURN_SLOPED;
				}
			}

			if (track_flags & TRACK_ELEM_FLAG_TURN_RIGHT){
				ride->testing_flags |= RIDE_TESTING_TURN_RIGHT;
				ride->turn_count_default &= ~CURRENT_TURN_COUNT_MASK;

				if (track_flags & TRACK_ELEM_FLAG_TURN_BANKED){
					ride->testing_flags |= RIDE_TESTING_TURN_BANKED;
				}
				if (track_flags & TRACK_ELEM_FLAG_TURN_SLOPED){
					ride->testing_flags |= RIDE_TESTING_TURN_SLOPED;
				}
			}
		}
			
		if (testing_flags & RIDE_TESTING_DROP_DOWN){
			if (vehicle->velocity < 0 || !(track_flags & TRACK_ELEM_FLAG_DOWN)){
				ride->testing_flags &= ~RIDE_TESTING_DROP_DOWN;

				sint16 z = vehicle->z / 8 - ride->start_drop_height;
				if (z < 0){
					z = abs(z);
					if (z > ride->highest_drop_height){
						ride->highest_drop_height = (uint8)z;
					}
				}
			}
		}
		else if (track_flags & TRACK_ELEM_FLAG_DOWN && vehicle->velocity >= 0){
			ride->testing_flags &= ~RIDE_TESTING_DROP_UP;
			ride->testing_flags |= RIDE_TESTING_DROP_DOWN;

			uint8 drops = ride->drops & 0x3F;
			if (drops != 0x3F)
				drops++;
			ride->drops &= ~0x3F;
			ride->drops |= drops;

			ride->start_drop_height = vehicle->z / 8;
			testing_flags &= ~RIDE_TESTING_DROP_UP;
		}

		if (testing_flags & RIDE_TESTING_DROP_UP){
			if (vehicle->velocity > 0 || !(track_flags & TRACK_ELEM_FLAG_UP)){
				ride->testing_flags &= ~RIDE_TESTING_DROP_UP;

				sint16 z = vehicle->z / 8 - ride->start_drop_height;
				if (z < 0){
					z = abs(z);
					if (z > ride->highest_drop_height){
						ride->highest_drop_height = (uint8)z;
					}
				}
			}
		}
		else if (track_flags & TRACK_ELEM_FLAG_UP && vehicle->velocity <= 0){
			ride->testing_flags &= ~RIDE_TESTING_DROP_DOWN;
			ride->testing_flags |= RIDE_TESTING_DROP_UP;

			uint8 drops = ride->drops & 0x3F;
			if (drops != 0x3F)
				drops++;
			ride->drops &= ~0x3F;
			ride->drops |= drops;

			ride->start_drop_height = vehicle->z / 8;
		}

		if (track_flags & TRACK_ELEM_FLAG_INVERSION){
			uint8 inversions = ride->inversions & 0x1F;
			if (inversions != 0x1F)
				inversions++;

			ride->inversions &= ~0x1F;
			ride->inversions |= inversions;
		}

		if (track_flags & TRACK_ELEM_FLAG_HELIX){
			uint8 helixes = ride_get_helix_sections(ride);
			if (helixes != 0x1F)
				helixes++;

			ride->special_track_elements &= ~0x1F;
			ride->special_track_elements |= helixes;
		}
		
	}

	if (ride->entrances[ride->var_1F6] == 0xFFFF)
		return;

	sint16 x, y;
	x = vehicle->x;
	y = vehicle->y;

	if (x == SPRITE_LOCATION_NULL){
		ride->testing_flags &= ~RIDE_TESTING_SHELTERED;
		return;
	}

	rct_map_element* map_element = map_get_surface_element_at(x / 32, y / 32);
	if (map_element->base_height * 8 <= vehicle->z){

		bool cover_found = false;
		do{
			if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE) {
				cover_found = true;
				break;
			}

			if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_PATH) {
				cover_found = true;
				break;
			}

			if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_SCENERY)
				continue;

			rct_scenery_entry* scenery = g_smallSceneryEntries[map_element->properties.scenery.type];
			if (scenery->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE) {
				cover_found = true;
				break;
			}
		} while (!map_element_is_last_for_tile(map_element++));

		if (cover_found == false) {
			ride->testing_flags &= ~RIDE_TESTING_SHELTERED;
			return;
		}
	}

	if (!(ride->testing_flags & RIDE_TESTING_SHELTERED)){
		ride->testing_flags |= RIDE_TESTING_SHELTERED;

		uint8 num_sheltered_sections = ride->num_sheltered_sections & 0x1F;
		if (num_sheltered_sections != 0x1F)
			num_sheltered_sections++;
		ride->num_sheltered_sections &= ~0x1F;
		ride->num_sheltered_sections |= num_sheltered_sections;

		if (vehicle->var_1F != 0){
			ride->num_sheltered_sections |= (1 << 5);
		}

		if (vehicle->var_20 != 0){
			ride->num_sheltered_sections |= (1 << 6);
		}
	}

	sint32 distance = ((vehicle->velocity + vehicle->var_2C) >> 10) * 42;
	if (distance < 0)return;

	ride->sheltered_length += distance;
}

static uint16 sub_6D7AC0(int currentSoundId, int currentVolume, int targetSoundId, int targetVolume)
{
	if (currentSoundId != 255) {
		if (currentSoundId == targetSoundId) {
			currentVolume = min(currentVolume + 15, targetVolume);
			return (currentVolume << 8) | currentSoundId;
		} else {
			currentVolume -= 9;
			if (currentVolume >= 80)
				return (currentVolume << 8) | currentSoundId;
		}
	}

	// Begin sound at quarter volume
	currentSoundId = targetSoundId;
	currentVolume = targetVolume == 255 ? 255 : targetVolume / 4;

	return (currentVolume << 8) | currentSoundId;
}

/**
 *
 *  rct2: 0x006D77F2
 */
static void vehicle_update(rct_vehicle *vehicle)
{
	// RCT2_CALLPROC_X(0x006D77F2, 0, 0, 0, 0, (int)vehicle, 0, 0);

	rct_ride *ride;
	rct_ride_type *rideEntry;

	if (vehicle->ride_subtype == RIDE_TYPE_NULL) {
		switch (vehicle->status) {
		case VEHICLE_STATUS_MOVING_TO_END_OF_STATION:
			vehicle_ride_null_update_moving_to_end_of_station(vehicle);
			break;
		case VEHICLE_STATUS_WAITING_TO_DEPART:
			vehicle_ride_null_update_waiting_to_depart(vehicle);
			break;
		case VEHICLE_STATUS_DEPARTING:
			vehicle_ride_null_update_departing(vehicle);
			break;
		case VEHICLE_STATUS_TRAVELLING:
			vehicle_ride_null_update_travelling(vehicle);
			break;
		case VEHICLE_STATUS_ARRIVING:
			vehicle_ride_null_update_arriving(vehicle);
			break;
		}
		return;
	}

	rideEntry = GET_RIDE_ENTRY(vehicle->ride_subtype);

	rct_ride_type_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

	ride = GET_RIDE(vehicle->ride);
	if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_TESTING)
		vehicle_update_measurements(vehicle);

	RCT2_GLOBAL(0x00F64E34, uint8) = 255;
	if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)) {
		RCT2_GLOBAL(0x00F64E34, uint8) = ride->breakdown_reason_pending;
		if ((vehicleEntry->var_14 & 8) && ride->breakdown_reason_pending == BREAKDOWN_SAFETY_CUT_OUT) {
			if (!(vehicleEntry->var_14 & 0x2000) ||
				(
					vehicle->var_1F == 2 &&
					vehicle->velocity <= 0x20000
				)
			) {
				vehicle->update_flags |= VEHICLE_UPDATE_FLAG_7;
			}
		}
	}

	switch (vehicle->status) {
	case VEHICLE_STATUS_MOVING_TO_END_OF_STATION:
		vehicle_update_moving_to_end_of_station(vehicle);
		break;
	case VEHICLE_STATUS_WAITING_FOR_PASSENGERS:
		vehicle_update_waiting_for_passengers(vehicle);
		break;	
	case VEHICLE_STATUS_WAITING_TO_DEPART:	
		vehicle_update_waiting_to_depart(vehicle);
		break;
	case VEHICLE_STATUS_CRASHING:
	case VEHICLE_STATUS_CRASHED:
		vehicle_update_crash(vehicle);
		break;
	case VEHICLE_STATUS_TRAVELING_BUMPER_CARS:
		vehicle_update_bumpcar_mode(vehicle);
		break;	
	case VEHICLE_STATUS_SWINGING:
		vehicle_update_swinging(vehicle);
		break;
	case VEHICLE_STATUS_DEPARTING:
	case VEHICLE_STATUS_TRAVELLING:
	case VEHICLE_STATUS_ARRIVING:
	case VEHICLE_STATUS_UNLOADING_PASSENGERS:
	case VEHICLE_STATUS_TRAVELLING_07:

	case VEHICLE_STATUS_ROTATING:
	case VEHICLE_STATUS_FERRIS_WHEEL_ROTATING:
	case VEHICLE_STATUS_SIMULATOR_OPERATING:
	case VEHICLE_STATUS_SPACE_RINGS_OPERATING:
	case VEHICLE_STATUS_TOP_SPIN_OPERATING:
	case VEHICLE_STATUS_HAUNTED_HOUSE_OPERATING:
	case VEHICLE_STATUS_CROOKED_HOUSE_OPERATING:
	case VEHICLE_STATUS_TRAVELLING_15:
		{
			int *addressSwitchPtr = (int*)(0x006D7B70 + (vehicle->status * 4));
			RCT2_CALLPROC_X(*addressSwitchPtr, 0, 0, 0, (vehicle->sub_state << 8) | ride->mode, (int)vehicle, 0, 0);
		}
		break;
	case VEHICLE_STATUS_WAITING_FOR_CABLE_LIFT:
		vehicle_update_waiting_for_cable_lift(vehicle);
		break;
	case VEHICLE_STATUS_SHOWING_FILM:
		vehicle_update_showing_film(vehicle);
		break;
	case VEHICLE_STATUS_DOING_CIRCUS_SHOW:
		vehicle_update_doing_circus_show(vehicle);
	}

	vehicle_update_sound(vehicle);
}

static void vehicle_ride_null_update_moving_to_end_of_station(rct_vehicle *vehicle)
{
	RCT2_CALLPROC_X(0x006DF8A4, 0, 0, 0, 0, (int)vehicle, 0, 0);
}

static void vehicle_ride_null_update_waiting_to_depart(rct_vehicle *vehicle)
{
	RCT2_CALLPROC_X(0x006DF8F1, 0, 0, 0, 0, (int)vehicle, 0, 0);
}

static void vehicle_ride_null_update_departing(rct_vehicle *vehicle)
{
	RCT2_CALLPROC_X(0x006DF97A, 0, 0, 0, 0, (int)vehicle, 0, 0);
}

static void vehicle_ride_null_update_travelling(rct_vehicle *vehicle)
{
	RCT2_CALLPROC_X(0x006DF99C, 0, 0, 0, 0, (int)vehicle, 0, 0);
}

static void vehicle_ride_null_update_arriving(rct_vehicle *vehicle)
{
	vehicle->sub_state++;
	if (vehicle->sub_state >= 64)
		vehicle->status = VEHICLE_STATUS_MOVING_TO_END_OF_STATION;
}

/**
*
*  rct2: 0x006D7BCC
*/
static void vehicle_update_moving_to_end_of_station(rct_vehicle *vehicle){
	rct_ride* ride = GET_RIDE(vehicle->ride);
	int eax, ebx;

	switch (ride->mode){
	case RIDE_MODE_UPWARD_LAUNCH:
	case RIDE_MODE_ROTATING_LIFT:
	case RIDE_MODE_DOWNWARD_LAUNCH:
	case RIDE_MODE_FREEFALL_DROP:
		if (vehicle->velocity >= -131940){
			vehicle->var_2C = -3298;
		}
		if (vehicle->velocity < -131940){
			vehicle->velocity = vehicle->velocity / 16;
			vehicle->var_2C = 0;
		}

		eax = sub_6DAB4C(vehicle, &ebx);
		if (!(eax&(1 << 5)))
			break;
		//Fall through to next case
	case RIDE_MODE_BUMPERCAR:
	case RIDE_MODE_SWING:
	case RIDE_MODE_ROTATION:
	case RIDE_MODE_FORWARD_ROTATION:
	case RIDE_MODE_BACKWARD_ROTATION:
	case RIDE_MODE_FILM_AVENGING_AVIATORS:
	case RIDE_MODE_FILM_THRILL_RIDERS:
	case RIDE_MODE_BEGINNERS:
	case RIDE_MODE_INTENSE:
	case RIDE_MODE_BERSERK:
	case RIDE_MODE_3D_FILM_MOUSE_TAILS:
	case RIDE_MODE_3D_FILM_STORM_CHASERS:
	case RIDE_MODE_3D_FILM_SPACE_RAIDERS:
	case RIDE_MODE_SPACE_RINGS:
	case RIDE_MODE_HAUNTED_HOUSE:
	case RIDE_MODE_CROOKED_HOUSE:
	case RIDE_MODE_CIRCUS_SHOW:
		vehicle->current_station = 0;
		vehicle->velocity = 0;
		vehicle->var_2C = 0;
		vehicle->status = VEHICLE_STATUS_WAITING_FOR_PASSENGERS;
		vehicle->sub_state = 0;
		vehicle_invalidate_window(vehicle);
		break;
	default:
	{
		rct_ride_type* rideEntry = GET_RIDE_ENTRY(vehicle->ride_subtype);
		rct_ride_type_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

		if (!(vehicleEntry->var_14 & (1 << 3))){
			if (vehicle->velocity <= 131940){
				vehicle->var_2C = 3298;
			}
		}
		if (vehicle->velocity > 131940){
			vehicle->velocity = vehicle->velocity / 16;
			vehicle->var_2C = 0;
		}

		int station;
		eax = sub_6DAB4C(vehicle, &station);

		if (eax & (1 << 1)){
			vehicle->velocity = 0;
			vehicle->var_2C = 0;
			vehicle->sub_state++;

			if (ride->mode == RIDE_MODE_RACE &&
				vehicle->sub_state >= 40){
				vehicle->status = VEHICLE_STATUS_WAITING_FOR_PASSENGERS;
				vehicle->sub_state = 0;
				vehicle_invalidate_window(vehicle);
				break;
			}
		}
		else{
			if (vehicle->velocity > 98955){
				vehicle->sub_state = 0;
			}
		}

		if (!(eax & (1 << 0)))
			break;

		vehicle->current_station = station;
		vehicle->velocity = 0;
		vehicle->var_2C = 0;
		vehicle->status = VEHICLE_STATUS_WAITING_FOR_PASSENGERS;
		vehicle->sub_state = 0;
		vehicle_invalidate_window(vehicle);
		break; 
	}
	}
}

/* 0x006D7FB4 */
static void train_ready_to_depart(rct_vehicle* vehicle, uint8 num_peeps_on_train, uint8 num_used_seats){

	if (num_peeps_on_train != num_used_seats)
		return;

	rct_ride* ride = GET_RIDE(vehicle->ride);

	if (ride->status == RIDE_STATUS_OPEN &&
		!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) &&
		!(vehicle->update_flags & VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART)){
		return;
	}

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)){
		if (ride->status != RIDE_STATUS_CLOSED ||
			ride->num_riders != 0){
			ride->train_at_station[vehicle->current_station] = 0xFF;
			vehicle->sub_state = 2;
			return;
		}
	}

	if (ride->mode == RIDE_MODE_FORWARD_ROTATION ||
		ride->mode == RIDE_MODE_BACKWARD_ROTATION){
		
		uint8 peep = ((-vehicle->var_1F) / 8) & 0xF;
		if (vehicle->peep[peep] != 0xFFFF){
			ride->train_at_station[vehicle->current_station] = 0xFF;
			vehicle->status = VEHICLE_STATUS_UNLOADING_PASSENGERS;
			vehicle->sub_state = 2;
			vehicle_invalidate_window(vehicle);
			return;
		}

		if (vehicle->num_peeps == 0)
			return;

		ride->train_at_station[vehicle->current_station] = 0xFF;
		vehicle->sub_state = 2;
		return;
	}

	if (num_peeps_on_train == 0)
		return;

	ride->train_at_station[vehicle->current_station] = 0xFF;
	vehicle->status = VEHICLE_STATUS_UNLOADING_PASSENGERS;
	vehicle->sub_state = 2;
	vehicle_invalidate_window(vehicle);
}

/* 0x006D7DA1 */
static void vehicle_update_waiting_for_passengers(rct_vehicle* vehicle){
	vehicle->velocity = 0;

	rct_ride* ride = GET_RIDE(vehicle->ride);

	if (vehicle->sub_state == 0){
		if (vehicle_open_restraints(vehicle))
			return;

		if (ride->entrances[vehicle->current_station] == 0xFFFF){
			ride->train_at_station[vehicle->current_station] = 0xFF;
			vehicle->sub_state = 2;
			return;
		}

		uint8 train_index = 0;
		while (ride->vehicles[train_index] != vehicle->sprite_index)train_index++;

		if (ride->train_at_station[vehicle->current_station] != 0xFF)
			return;

		ride->train_at_station[vehicle->current_station] = train_index;
		vehicle->sub_state = 1;
		vehicle->var_C0 = 0;

		invalidate_sprite_2((rct_sprite*)vehicle);
		return;
	}
	else if (vehicle->sub_state == 1){
		if (vehicle->var_C0 != 0xFFFF)
			vehicle->var_C0++;

		vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;

		// 0xF64E31, 0xF64E32, 0xF64E33
		uint8 num_peeps_on_train = 0, num_used_seats_on_train = 0, num_seats_on_train = 0;

		for (uint16 sprite_id = vehicle->sprite_index; sprite_id != 0xFFFF;){
			rct_vehicle* train_vehicle = GET_VEHICLE(sprite_id);

			num_peeps_on_train += train_vehicle->num_peeps;
			num_used_seats_on_train += train_vehicle->next_free_seat;
			num_seats_on_train += train_vehicle->num_seats;

			sprite_id = train_vehicle->next_vehicle_on_train;
		}

		num_seats_on_train &= 0x7F;

		if (!(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride->type * 2] & RIDE_TYPE_FLAG_NO_TEST_MODE)){
			if (vehicle->var_C0 < 20){
				train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
				return;
			}
		}
		else{
			if (num_peeps_on_train == 0){
				train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
				return;
			}
		}

		if (RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride->type * 2] & RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS){
			if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH){
				if (ride->min_waiting_time * 32 > vehicle->var_C0){
					train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
					return;
				}
			}
			if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH){
				if (ride->max_waiting_time * 32 < vehicle->var_C0){
					vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
					train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
					return;
				}
			}
		}

		if (ride->depart_flags & RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES){

			for (int i = 0; i < 32; ++i){
				uint16 train_id = ride->vehicles[i];
				if (train_id == 0xFFFF)
					continue;

				if (train_id == vehicle->sprite_index)
					continue;

				rct_vehicle* train = GET_VEHICLE(train_id);

				if (train->status == VEHICLE_STATUS_UNLOADING_PASSENGERS ||
					train->status == VEHICLE_STATUS_MOVING_TO_END_OF_STATION){
					if (train->current_station == vehicle->current_station){
						vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
						train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
						return;
					}
				}
			}
		}

		if (RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride->type * 2] & RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS &&
			ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD){

			if (num_peeps_on_train == num_seats_on_train){
				vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
				train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
				return;
			}

			uint8 load = ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK;
			if (load == 3){
				train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
				return;
			}

			uint8 three_quater_seats = (3 * num_seats_on_train) / 4;
			if (three_quater_seats != 0 && num_peeps_on_train >= three_quater_seats){
				vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
				train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
				return;
			}

			if (load == 2){
				train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
				return;
			}

			if (num_seats_on_train / 2 != 0 && num_peeps_on_train >= num_seats_on_train / 2){
				vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
				train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
				return;
			}

			if (load == 1){
				train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
				return;
			}

			if (num_seats_on_train / 4 != 0 && num_peeps_on_train >= num_seats_on_train / 4){
				vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
				train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
				return;
			}

			if (load == 0){
				vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
				train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
				return;
			}

			if (num_peeps_on_train != 0){
				vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
			}				
			train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
			return;
		}
		vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
		train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
		return;
	}

	if (vehicle_close_restraints(vehicle))
		return;

	vehicle->velocity = 0;
	vehicle->status = VEHICLE_STATUS_WAITING_TO_DEPART;
	vehicle->sub_state = 0;
	vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT;

	if (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS){
		vehicle->update_flags |= VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT;
	}

	vehicle_invalidate_window(vehicle);
}

/* rct2: 0x006D91BF */
static void vehicle_update_bumpcar_mode(rct_vehicle* vehicle) {
	rct_ride* ride = GET_RIDE(vehicle->ride);
	rct_ride_type* rideEntry = GET_RIDE_ENTRY(vehicle->ride_subtype);
	rct_ride_type_vehicle* vehicleEntry = &rideEntry[vehicle->vehicle_type];

	if (vehicleEntry->var_12 & (1 << 7) && vehicle->var_C5 != 1) {
		vehicle->var_C5 = 1;
		invalidate_sprite_2((rct_sprite*)vehicle);
	}

	RCT2_CALLPROC_X(0x006DA44E, 0, 0, 0, 0, (int)vehicle, 0, 0);

	// Update the length of time vehicle has been in bumper mode
	if (vehicle->sub_state++ == 0xFF) {
		vehicle->var_CE++;
	}

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
		return;

	vehicle->var_C5 = 0;
	invalidate_sprite_2((rct_sprite*)vehicle);
	vehicle->velocity = 0;
	vehicle->var_2C = 0;
	vehicle->status = VEHICLE_STATUS_UNLOADING_PASSENGERS;
	vehicle->sub_state = 0;
	vehicle_invalidate_window(vehicle);
}

/* rct2: 0x006D808BE */
static void vehicle_update_waiting_to_depart(rct_vehicle* vehicle) {
	rct_ride* ride = GET_RIDE(vehicle->ride);
	bool shouldBreak = false;
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
		switch (ride->breakdown_reason_pending) {
		case BREAKDOWN_RESTRAINTS_STUCK_CLOSED:
		case BREAKDOWN_RESTRAINTS_STUCK_OPEN:
		case BREAKDOWN_DOORS_STUCK_CLOSED:
		case BREAKDOWN_DOORS_STUCK_OPEN:
			break;
		default:
			shouldBreak = true;
			break;
		}
	}

	bool skipCheck = false;
	if (shouldBreak == true || ride->status != RIDE_STATUS_OPEN) {
		if (ride->mode == RIDE_MODE_FORWARD_ROTATION ||
			ride->mode == RIDE_MODE_BACKWARD_ROTATION) {
			uint8 bl = ((-vehicle->var_1F) >> 3) & 0xF;
			if (vehicle->peep[bl] == 0xFFFF) {
				if (vehicle->num_peeps == 0) {
					skipCheck = true;
				}
			}
			else {
				if (ride->exits[vehicle->current_station] != 0xFFFF) {
					vehicle->status = VEHICLE_STATUS_UNLOADING_PASSENGERS;
					vehicle->sub_state = 0;
					vehicle_invalidate_window(vehicle);
					return;
				}
			}
		}
		else {
			uint16 spriteId = vehicle->sprite_index;
			for (rct_vehicle* curVehicle; spriteId != 0xFFFF; spriteId = curVehicle->next_vehicle_on_train) {
				curVehicle = GET_VEHICLE(spriteId);

				if (curVehicle->num_peeps != 0) {
					if (ride->exits[vehicle->current_station] != 0xFFFF) {
						vehicle->status = VEHICLE_STATUS_UNLOADING_PASSENGERS;
						vehicle->sub_state = 0;
						vehicle_invalidate_window(vehicle);
						return;
					}
					break;
				}
			}
		}
	}

	if (skipCheck == false) {
		if (!(ride->station_depart[vehicle->current_station] & STATION_DEPART_FLAG))
			return;
	}

	if (RCT2_ADDRESS(RCT2_ADDRESS_RIDE_FLAGS, uint32)[ride->type * 2]
		& RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS) {
		if (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS) {
			if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT) {
				if (RCT2_CALLPROC_X(0x006DE287, 0, 0, 0, 0, (int)vehicle, 0, 0) & 0x100)
					return;
			}
		}
	}

	vehicle->status = VEHICLE_STATUS_DEPARTING;
	vehicle->sub_state = 0;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT) {
		RCT2_GLOBAL(0x00F441D2, uint8) = vehicle->ride;
		rct_xy_element track;
		int z;
		int direction;

		if (track_block_get_next_from_zero(
			vehicle->track_x,
			vehicle->track_y,
			vehicle->track_z,
			vehicle->ride,
			(uint8)(vehicle->track_direction & 0x3),
			&track,
			&z,
			&direction)) {

			if (track_element_is_cable_lift(track.element)) {
				vehicle->status = VEHICLE_STATUS_WAITING_FOR_CABLE_LIFT;
			}
		}
	}

	switch (ride->mode) {
	case RIDE_MODE_BUMPERCAR:
		vehicle->status = VEHICLE_STATUS_TRAVELING_BUMPER_CARS;
		vehicle_invalidate_window(vehicle);
		// Bumper mode uses sub_state / var_CE to tell how long
		// the vehicle has been ridden.
		vehicle->sub_state = 0;
		vehicle->var_CE = 0;
		vehicle_update_bumpcar_mode(vehicle);
		break;
	case RIDE_MODE_SWING:
		vehicle->status = VEHICLE_STATUS_SWINGING;
		vehicle->sub_state = 0;
		vehicle_invalidate_window(vehicle);
		vehicle->var_CE = 0;
		vehicle->var_4C = 0xFFFF;
		vehicle_update_swinging(vehicle);
		break;
	case RIDE_MODE_ROTATION:
		vehicle->status = VEHICLE_STATUS_ROTATING;
		vehicle->sub_state = 0;
		vehicle_invalidate_window(vehicle);
		vehicle->var_CE = 0;
		vehicle->var_4C = 0xFFFF;
		//6d92ff
		break;
	case RIDE_MODE_FILM_AVENGING_AVIATORS:
	case RIDE_MODE_FILM_THRILL_RIDERS:
		vehicle->status = VEHICLE_STATUS_SIMULATOR_OPERATING;
		vehicle->sub_state = 0;
		if (ride->mode == RIDE_MODE_FILM_THRILL_RIDERS)
			vehicle->sub_state = 1;
		vehicle_invalidate_window(vehicle);
		vehicle->var_4C = 0xFFFF;
		//6d94f2
		break;
	case RIDE_MODE_BEGINNERS:
	case RIDE_MODE_INTENSE:
	case RIDE_MODE_BERSERK:
		vehicle->status = VEHICLE_STATUS_TOP_SPIN_OPERATING;
		vehicle_invalidate_window(vehicle);

		switch (ride->mode) {
		case RIDE_MODE_BEGINNERS:
			vehicle->sub_state = 0;
			break;
		case RIDE_MODE_INTENSE:
			vehicle->sub_state = 1;
			break;
		case RIDE_MODE_BERSERK:
			vehicle->sub_state = 2;
			break;
		}
		vehicle->var_4C = 0xFFFF;
		vehicle->var_1F = 0;
		vehicle->var_20 = 0;
		//6d9547
		break;
	case RIDE_MODE_FORWARD_ROTATION:
	case RIDE_MODE_BACKWARD_ROTATION:
		vehicle->status = VEHICLE_STATUS_FERRIS_WHEEL_ROTATING;
		vehicle->sub_state = vehicle->var_1F;
		vehicle_invalidate_window(vehicle);
		vehicle->var_CE = 0;
		vehicle->var_4C = 0x808;
		//6d9413
		break;
	case RIDE_MODE_3D_FILM_MOUSE_TAILS:
	case RIDE_MODE_3D_FILM_STORM_CHASERS:
	case RIDE_MODE_3D_FILM_SPACE_RAIDERS:
		vehicle->status = VEHICLE_STATUS_SHOWING_FILM;
		vehicle_invalidate_window(vehicle);
		switch (ride->mode) {
		case RIDE_MODE_3D_FILM_MOUSE_TAILS:
			vehicle->sub_state = 0;
			break;
		case RIDE_MODE_3D_FILM_STORM_CHASERS:
			vehicle->sub_state = 1;
			break;
		case RIDE_MODE_3D_FILM_SPACE_RAIDERS:
			vehicle->sub_state = 2;
			break;
		}
		vehicle->var_4C = 0xFFFF;
		vehicle_update_showing_film(vehicle);
		break;
	case RIDE_MODE_CIRCUS_SHOW:
		vehicle->status = VEHICLE_STATUS_DOING_CIRCUS_SHOW;
		vehicle->sub_state = 0;
		vehicle_invalidate_window(vehicle);
		vehicle->var_4C = 0xFFFF;
		vehicle_update_doing_circus_show(vehicle);
		break;
	case RIDE_MODE_SPACE_RINGS:
		vehicle->status = VEHICLE_STATUS_SPACE_RINGS_OPERATING;
		vehicle->sub_state = 0;
		vehicle_invalidate_window(vehicle);
		vehicle->var_1F = 0;
		vehicle->var_4C = 0xFFFF;
		//6d97cb
		break;
	case RIDE_MODE_HAUNTED_HOUSE:
		vehicle->status = VEHICLE_STATUS_HAUNTED_HOUSE_OPERATING;
		vehicle->sub_state = 0;
		vehicle_invalidate_window(vehicle);
		vehicle->var_1F = 0;
		vehicle->var_4C = 0xFFFF;
		//6d9641
		break;
	case RIDE_MODE_CROOKED_HOUSE:
		vehicle->status = VEHICLE_STATUS_CROOKED_HOUSE_OPERATING;
		vehicle->sub_state = 0;
		vehicle_invalidate_window(vehicle);
		vehicle->var_1F = 0;
		vehicle->var_4C = 0xFFFF;
		//6d9781
		break;
	default:
		vehicle->sub_state = 0;
		vehicle_invalidate_window(vehicle);
		vehicle->var_CE = 0;
		break;
	}
}

/* rct2: 0x006D9249 */
static void vehicle_update_swinging(rct_vehicle* vehicle) {
	rct_ride* ride = GET_RIDE(vehicle->ride);
	rct_ride_type* rideEntry = GET_RIDE_ENTRY(vehicle->ride_subtype);

	// SubState for this ride means swinging state
	// 0 == first swing
	// 3 == full swing
	uint8 swingState = vehicle->sub_state;
	if (rideEntry->flags & RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_1) {
		swingState += 4;
		if (rideEntry->flags & RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_2)
			swingState += 4;
	}
	uint8* edi = RCT2_ADDRESS(0x0099F9D0, uint8*)[swingState];
	uint8 al = edi[(uint16)(vehicle->var_4C + 1)];

	// 0x80 indicates that a complete swing has been
	// completed and the next swing can start
	if (al != 0x80) {
		vehicle->var_4C++;
		if (al == vehicle->var_1F)
			return;
		// Used to know which sprite to draw
		vehicle->var_1F = al;
		invalidate_sprite_2((rct_sprite*)vehicle);
		return;
	}

	vehicle->var_4C = 0xFFFF;
	vehicle->var_CE++;
	if (ride->status != RIDE_STATUS_CLOSED) {
		// It takes 3 swings to get into full swing
		// ride->rotations already takes this into account
		if (vehicle->var_CE + 3 < ride->rotations) {
			// Go to the next swing state until we
			// are at full swing.
			if (vehicle->sub_state != 3) {
				vehicle->sub_state++;
			}
			vehicle_update_swinging(vehicle);
			return;
		}
	}

	// To get to this part of the code the
	// swing has to be in slowing down phase
	if (vehicle->sub_state == 0) {
		vehicle->status = VEHICLE_STATUS_ARRIVING;
		vehicle_invalidate_window(vehicle);
		vehicle->sub_state = 0;
		vehicle->var_C0 = 0;
		return;
	}
	// Go towards first swing state
	vehicle->sub_state--;
	vehicle_update_swinging(vehicle);
}

/**
*
*  rct2: 0x006D9CE9
*/
static void vehicle_update_waiting_for_cable_lift(rct_vehicle *vehicle)
{
	rct_ride* ride = GET_RIDE(vehicle->ride);

	rct_vehicle* cableLift = GET_VEHICLE(ride->cable_lift);

	if (cableLift->status != VEHICLE_STATUS_WAITING_FOR_PASSENGERS)
		return;

	cableLift->status = VEHICLE_STATUS_WAITING_TO_DEPART;
	cableLift->var_C0 = vehicle->sprite_index;
}

/**
*
*  rct2: 0x006D95AD
*/
static void vehicle_update_showing_film(rct_vehicle *vehicle)
{
	int currentTime, totalTime;

	if (RCT2_GLOBAL(0x00F64E34, uint8) == 0)
		return;

	totalTime = RideFilmLength[vehicle->sub_state];
	currentTime = vehicle->var_4C + 1;
	if (currentTime <= totalTime) {
		vehicle->var_4C = currentTime;
	}
	else {
		vehicle->status = VEHICLE_STATUS_ARRIVING;
		vehicle_invalidate_window(vehicle);
		vehicle->sub_state = 0;
		vehicle->var_C0 = 0;
	}
}

/**
 * 
 *  rct2: 0x006D95F7
 */
static void vehicle_update_doing_circus_show(rct_vehicle *vehicle)
{
	int currentTime, totalTime;

	if (RCT2_GLOBAL(0x00F64E34, uint8) == 0)
		return;

	totalTime = *(RCT2_ADDRESS(0x009A0AB4, uint16*)[vehicle->sub_state]);
	currentTime = vehicle->var_4C + 1;
	if (currentTime <= totalTime) {
		vehicle->var_4C = currentTime;
	} else {
		vehicle->status = VEHICLE_STATUS_ARRIVING;
		vehicle_invalidate_window(vehicle);
		vehicle->sub_state = 0;
		vehicle->var_C0 = 0;
	}
}

/* rct2: 0x0068B8BD 
 * returns the map element that the vehicle will collide with
 * returns NULL if no collisions.
 */
static rct_map_element* vehicle_check_collision(sint16 x, sint16 y, sint16 z) {
	rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);

	uint8 bl;
	if ((x & 0x1F) >= 16) {
		bl = 1;
		if ((y & 0x1F) < 16)
			bl = 2;
	}
	else {
		bl = 4;
		if ((y & 0x1F) >= 16)
			bl = 8;
	}

	do {
		if (z / 8 < mapElement->base_height)
			continue;

		if (z / 8 >= mapElement->clearance_height)
			continue;

		if (mapElement->flags & bl)
			return mapElement;
	} while (!map_element_is_last_for_tile(mapElement++));

	return NULL;
}

/* rct2: 0x006DE6C6 */
static void vehicle_kill_all_passengers(rct_vehicle* vehicle) {
	uint16 numFatalities = 0;

	uint16 spriteId = vehicle->sprite_index;
	for (rct_vehicle* curVehicle; spriteId != 0xFFFF; spriteId = curVehicle->next_vehicle_on_train) {
		curVehicle = GET_VEHICLE(spriteId);
		numFatalities += curVehicle->num_peeps;
	}

	rct_ride* ride = GET_RIDE(vehicle->ride);
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = numFatalities;

	uint8 crashType = numFatalities == 0 ?
		RIDE_CRASH_TYPE_NO_FATALITIES :
		RIDE_CRASH_TYPE_FATALITIES;
	
	if (crashType >= ride->last_crash_type)
		ride->last_crash_type = crashType;

	if (numFatalities != 0) {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_SIX_FLAGS_DEPRECATED)) {
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint16) = ride->name;
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 4, uint32) = ride->name_arguments;

			news_item_add_to_queue(NEWS_ITEM_RIDE, 2219, vehicle->ride);
		}

		if (RCT2_GLOBAL(0x135882E, uint16) < 500) {
			RCT2_GLOBAL(0x135882E, uint16) += 200;
		}
	}

	spriteId = vehicle->sprite_index;
	for (rct_vehicle* curVehicle; spriteId != 0xFFFF; spriteId = curVehicle->next_vehicle_on_train) {
		curVehicle = GET_VEHICLE(spriteId);

		if (curVehicle->num_peeps != curVehicle->next_free_seat)
			continue;

		if (curVehicle->num_peeps == 0)
			continue;

		for (uint8 i = 0; i < curVehicle->num_peeps; i++) {
			rct_peep* peep = GET_PEEP(curVehicle->peep[i]);
			if (peep->outside_of_park == 0) {
				RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16)--;
				RCT2_GLOBAL(RCT2_ADDRESS_BTM_TOOLBAR_DIRTY_FLAGS, uint16) |=
					BTM_TB_DIRTY_FLAG_PEEP_COUNT;
			}
			ride->num_riders--;
			peep_sprite_remove(peep);
		}

		curVehicle->num_peeps = 0;
		curVehicle->next_free_seat = 0;
	}
}

static void vehicle_crash_on_land(rct_vehicle* vehicle) {
	vehicle->status = VEHICLE_STATUS_CRASHED;
	vehicle_invalidate_window(vehicle);

	rct_ride* ride = GET_RIDE(vehicle->ride);
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED)) {

		rct_vehicle* frontVehicle = vehicle;
		while (frontVehicle->is_child != 0)frontVehicle = GET_VEHICLE(frontVehicle->prev_vehicle_on_ride);

		uint8 trainIndex = 0;
		while (ride->vehicles[trainIndex] != frontVehicle->sprite_index)
			trainIndex++;

		ride_crash(vehicle->ride, trainIndex);

		if (ride->status != RIDE_STATUS_CLOSED) {
			ride_set_status(vehicle->ride, RIDE_STATUS_CLOSED);
		}
	}
	ride->lifecycle_flags |= RIDE_LIFECYCLE_CRASHED;
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;

	if (vehicle->is_child == 0) {
		vehicle_kill_all_passengers(vehicle);
	}

	vehicle->sub_state = 2;
	audio_play_sound_at_location(SOUND_CRASH, vehicle->x, vehicle->y, vehicle->z);

	sprite_misc_3_create(vehicle->x, vehicle->y, vehicle->z);
	sprite_misc_5_create(vehicle->x, vehicle->y, vehicle->z);

	uint8 numParticles = min(vehicle->sprite_width, 7);

	while (numParticles-- != 0)
		crashed_vehicle_particle_create(vehicle->colours, vehicle->x, vehicle->y, vehicle->z);

	vehicle->var_0C |= (1 << 7);
	vehicle->var_C5 = 0;
	vehicle->var_C8 = 0;
	vehicle->sprite_width = 13;
	vehicle->sprite_height_negative = 45;
	vehicle->sprite_height_positive = 5;

	sprite_move(vehicle->x, vehicle->y, vehicle->z, (rct_sprite*)vehicle);
	invalidate_sprite_2((rct_sprite*)vehicle);

	vehicle->var_4E = 0;
}

static void vehicle_crash_on_water(rct_vehicle* vehicle) {
	vehicle->status = VEHICLE_STATUS_CRASHED;
	vehicle_invalidate_window(vehicle);

	rct_ride* ride = GET_RIDE(vehicle->ride);
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED)) {

		rct_vehicle* frontVehicle = vehicle;
		while (frontVehicle->is_child != 0)frontVehicle = GET_VEHICLE(frontVehicle->prev_vehicle_on_ride);

		uint8 trainIndex = 0;
		while (ride->vehicles[trainIndex] != frontVehicle->sprite_index)
			trainIndex++;

		ride_crash(vehicle->ride, trainIndex);

		if (ride->status != RIDE_STATUS_CLOSED) {
			ride_set_status(vehicle->ride, RIDE_STATUS_CLOSED);
		}
	}
	ride->lifecycle_flags |= RIDE_LIFECYCLE_CRASHED;
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;

	if (vehicle->is_child == 0) {
		vehicle_kill_all_passengers(vehicle);
	}

	vehicle->sub_state = 2;
	audio_play_sound_at_location(SOUND_WATER_1, vehicle->x, vehicle->y, vehicle->z);

	crash_splash_create(vehicle->x, vehicle->y, vehicle->z);
	crash_splash_create(vehicle->x - 8, vehicle->y - 9, vehicle->z);
	crash_splash_create(vehicle->x + 11, vehicle->y - 9, vehicle->z);
	crash_splash_create(vehicle->x + 11, vehicle->y + 8, vehicle->z);
	crash_splash_create(vehicle->x - 4, vehicle->y + 8, vehicle->z);

	for (int i = 0; i < 10; ++i)
		crashed_vehicle_particle_create(vehicle->colours, vehicle->x - 4, vehicle->y + 8, vehicle->z);
	
	vehicle->var_0C |= (1 << 7);
	vehicle->var_C5 = 0;
	vehicle->var_C8 = 0;
	vehicle->sprite_width = 13;
	vehicle->sprite_height_negative = 45;
	vehicle->sprite_height_positive = 5;

	sprite_move(vehicle->x, vehicle->y, vehicle->z, (rct_sprite*)vehicle);
	invalidate_sprite_2((rct_sprite*)vehicle);

	vehicle->var_4E = 0xFFFF;
}

/**
*
*  rct2: 0x006D98CA
*/
static void vehicle_update_crash(rct_vehicle *vehicle){
	uint16 spriteId = vehicle->sprite_index;
	rct_vehicle* curVehicle;
	do {
		curVehicle = GET_VEHICLE(spriteId);
		if (curVehicle->sub_state > 1) {
			if (curVehicle->var_4E <= 96) {
				curVehicle->var_4E++;
				if ((scenario_rand() & 0xFFFF) <= 0x1555) {
					sprite_misc_3_create(
						curVehicle->x + (scenario_rand() & 2 - 1),
						curVehicle->y + (scenario_rand() & 2 - 1),
						curVehicle->z
						);
				}
			}
			if (curVehicle->var_C8 + 7281 > 0xFFFF) {
				curVehicle->var_C5++;
				if (curVehicle->var_C5 >= 8)
					curVehicle->var_C5 = 0;
				invalidate_sprite_2((rct_sprite*)curVehicle);
			}
			curVehicle->var_C8 += 7281;
			continue;
		}

		rct_map_element* collideElement = vehicle_check_collision(curVehicle->x, curVehicle->y, curVehicle->z);
		if (collideElement == NULL) {
			curVehicle->sub_state = 1;
		}
		else if (curVehicle->sub_state == 1){
			vehicle_crash_on_land(curVehicle);
			continue;
		}

		int z = map_element_height(curVehicle->x, curVehicle->y);
		sint16 waterHeight = (z >> 16) & 0xFFFF;
		z = (sint16)(z & 0xFFFF);
		sint16 zDiff;
		if (waterHeight != 0) {
			zDiff = curVehicle->z - waterHeight;
			if (zDiff <= 0 && zDiff >= -20) {
				vehicle_crash_on_water(curVehicle);
				continue;
			}
		}
		
		zDiff = curVehicle->z - z;
		if ((zDiff <= 0 && zDiff >= -20) || curVehicle->z < 16){
			vehicle_crash_on_land(curVehicle);
			continue;
		}

		invalidate_sprite_2((rct_sprite*)curVehicle);

		rct_xyz16 curPosition = {
			.x = curVehicle->x,
			.y = curVehicle->y,
			.z = curVehicle->z
		};

		curPosition.x += (sint8)(curVehicle->var_B6 >> 8);
		curPosition.y += (sint8)(curVehicle->var_C0 >> 8);
		curPosition.z += (sint8)(curVehicle->var_4E >> 8);
		curVehicle->track_x += (sint16)(curVehicle->var_B6 << 8);
		curVehicle->track_y += (sint16)(curVehicle->var_C0 << 8);
		curVehicle->track_z += (sint16)(curVehicle->var_4E << 8);

		if (curPosition.x > 0x1FFF ||
			curPosition.y > 0x1FFF) {
			vehicle_crash_on_land(curVehicle);
			continue;
		}

		sprite_move(curPosition.x, curPosition.y, curPosition.z, (rct_sprite*)curVehicle);
		invalidate_sprite_2((rct_sprite*)curVehicle);

		if (curVehicle->sub_state == 1) {
			curVehicle->var_4E += 0xFFEC;
		}
	} while ((spriteId = curVehicle->next_vehicle_on_train) != 0xFFFF);
}
/**
 * 
 *  rct2: 0x006D7888
 */
static void vehicle_update_sound(rct_vehicle *vehicle)
{
	RCT2_CALLPROC_X(0x006D7888, 0, 0, 0, 0, (int)vehicle, 0, 0); return;

	// PROBLEMS
	rct_ride *ride;
	rct_ride_type *rideEntry;
	// bl should be set before hand
	uint8 bl = 255, dl = 255;
	uint8 screamId, screamVolume;
	uint16 soundIdVolume;

	ride = GET_RIDE(vehicle->ride);
	rideEntry = GET_RIDE_ENTRY(vehicle->ride_subtype);

	rct_ride_type_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

	int ecx = abs(vehicle->velocity) - 0x10000;
	if (ecx >= 0) {
		dl = vehicleEntry->var_57;
		ecx >>= 15;
		if (208 + (ecx & 0xFF) > 255)
			bl = 255;
		else
			bl = 208 + (ecx & 0xFF);
	}

	switch (vehicleEntry->sound_range) {
	case 3:
		screamVolume = vehicle->scream_sound_id;
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 0x7F)) {
			if (vehicle->velocity < 0x40000 || vehicle->scream_sound_id != 255)
				goto loc_6D7A97;

			if ((scenario_rand() & 0xFFFF) <= 0x5555) {
				vehicle->scream_sound_id = SOUND_TRAIN_WHISTLE;
				screamId = 255;
				break;
			}
		}
		if (screamVolume != 254) screamVolume = 255;
		screamId = 255;
		break;

	case 4:
		screamVolume = vehicle->scream_sound_id;
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 0x7F)) {
			if (vehicle->velocity < 0x40000 || vehicle->scream_sound_id != 255)
				goto loc_6D7A97;

			if ((scenario_rand() & 0xFFFF) <= 0x5555) {
				vehicle->scream_sound_id = SOUND_TRAM;
				screamId = 255;
				break;
			}
		}
		if (screamVolume != 254) screamVolume = 255;
		screamId = 255;
		break;

	default:
		if ((vehicleEntry->var_14 & 0x10)) {
			screamId = vehicle_update_scream_sound(vehicle);
			if (screamId == 255)
				goto loc_6D7A97;

			screamVolume = 255;
			break;
		}

	loc_6D7A97:
		vehicle->scream_sound_id = 255;
		screamVolume = RCT2_GLOBAL(0x0097D7C8 + (ride->type * 4), uint8);
		screamId = 243;
		if (!(vehicle->var_B8 & 2))
			screamVolume = 255;
	}


	// Friction sound
	soundIdVolume = sub_6D7AC0(vehicle->sound1_id, vehicle->sound1_volume, bl, dl);
	vehicle->sound1_id = soundIdVolume & 0xFF;
	vehicle->sound1_volume = (soundIdVolume >> 8) & 0xFF;

	// Scream sound
	soundIdVolume = sub_6D7AC0(vehicle->sound2_id, vehicle->sound2_volume, screamId, screamVolume);
	vehicle->sound2_id = soundIdVolume & 0xFF;
	vehicle->sound2_volume = (soundIdVolume >> 8) & 0xFF;

	{
		int ebx = RCT2_ADDRESS(0x009A3684, sint16)[vehicle->sprite_direction];
		int eax = ((vehicle->velocity >> 14) * ebx) >> 14;
		eax = max(eax, 0xFF81);
		eax = min(eax, 0x7F);
		vehicle->var_BF = eax & 0xFF;
	}
}

/**
 *
 * 
 *  rct2: 0x006D796B
 */
static int vehicle_update_scream_sound(rct_vehicle *vehicle)
{
	int r;
	uint16 spriteIndex;
	rct_ride_type *rideEntry;
	rct_vehicle *vehicle2;

	rideEntry = GET_RIDE_ENTRY(vehicle->ride_subtype);

	rct_ride_type_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

	int totalNumPeeps = vehicle_get_total_num_peeps(vehicle);
	if (totalNumPeeps == 0)
		return 255;

	if (vehicle->velocity < 0) {
		if (vehicle->velocity > -0x2C000)
			return 255;

		spriteIndex = vehicle->sprite_index;
		do {
			vehicle2 = &(g_sprite_list[spriteIndex].vehicle);
			if (vehicle2->var_1F < 1)
				continue;
			if (vehicle2->var_1F <= 4)
				goto produceScream;
			if (vehicle2->var_1F < 9)
				continue;
			if (vehicle2->var_1F <= 15)
				goto produceScream;
		} while ((spriteIndex = vehicle2->next_vehicle_on_train) != SPRITE_INDEX_NULL);
		return 255;
	}

	if (vehicle->velocity < 0x2C000)
		return 255;

	spriteIndex = vehicle->sprite_index;
	do {
		vehicle2 = &(g_sprite_list[spriteIndex].vehicle);
		if (vehicle2->var_1F < 5)
			continue;
		if (vehicle2->var_1F <= 8)
			goto produceScream;
		if (vehicle2->var_1F < 17)
			continue;
		if (vehicle2->var_1F <= 23)
			goto produceScream;
	} while ((spriteIndex = vehicle2->next_vehicle_on_train) != SPRITE_INDEX_NULL);
	return 255;

produceScream:
	if (vehicle->scream_sound_id == 255) {
		r = scenario_rand();
		if (totalNumPeeps >= r % 16) {
			switch (vehicleEntry->sound_range) {
			case 0:
				vehicle->scream_sound_id = byte_9A3A14[r % 2];
				break;
			case 1:
				vehicle->scream_sound_id = byte_9A3A18[r % 8];
				break;
			case 2:
				vehicle->scream_sound_id = byte_9A3A16[r % 2];
				break;
			default:
				vehicle->scream_sound_id = NO_SCREAM;
				break;
			}
		} else {
			vehicle->scream_sound_id = NO_SCREAM;
		}
	}
	return vehicle->scream_sound_id;
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
	current->restraints_position = 0;
	current->var_BA = 0;
	current->var_B6 = 0;
	current->var_B8 = 0;
	current->sound1_id = 0xFF;
	current->sound2_id = 0xFF;
	current->var_C4 = 0;
	current->var_C5 = 0;
	current->var_C8 = 0;
	current->pad_CA[0] = 0;
	current->pad_CA[1] = 0;
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
	current->sub_state = 0;
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

/**
 *
 *  rct2: 0x006DAB4C
 */
int sub_6DAB4C(rct_vehicle *vehicle, int *outStation)
{
	registers regs;
	regs.esi = (int)vehicle;

	RCT2_CALLFUNC_Y(0x006DAB4C, &regs);

	if (outStation != NULL) *outStation = regs.ebx;
	return regs.eax;
}

rct_ride_type_vehicle *vehicle_get_vehicle_entry(rct_vehicle *vehicle)
{
	rct_ride_type *rideEntry = GET_RIDE_ENTRY(vehicle->ride_subtype);
	return &rideEntry->vehicles[vehicle->vehicle_type];
}

int vehicle_get_total_num_peeps(rct_vehicle *vehicle)
{
	uint16 spriteIndex;
	int numPeeps = 0;
	for (;;) {
		numPeeps += vehicle->num_peeps;
		spriteIndex = vehicle->next_vehicle_on_train;
		if (spriteIndex == SPRITE_INDEX_NULL)
			break;

		vehicle = &(g_sprite_list[spriteIndex].vehicle);
	}

	return numPeeps;
}

/**
 * 
 *  rct2: 0x006DA1EC
 */
void vehicle_invalidate_window(rct_vehicle *vehicle)
{
	int viewVehicleIndex;
	rct_ride *ride;
	rct_window *w;

	w = window_find_by_number(WC_RIDE, vehicle->ride);
	if (w == NULL)
		return;

	ride = GET_RIDE(vehicle->ride);
	viewVehicleIndex = w->ride.view - 1;
	if (viewVehicleIndex < 0 || viewVehicleIndex >= ride->num_vehicles)
		return;

	if (vehicle->sprite_index != ride->vehicles[viewVehicleIndex])
		return;

	window_invalidate(w);
}
