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
#include "../interface/viewport.h"
#include "../world/sprite.h"
#include "ride.h"
#include "ride_data.h"
#include "vehicle.h"

static void vehicle_update(rct_vehicle *vehicle);

/**
*
*  rct2: 0x006BB9FF
*/
void vehicle_update_sound_params(rct_vehicle* vehicle)
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2) && (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 4) || RCT2_GLOBAL(0x0141F570, uint8) == 6)) {
		if (vehicle->sound1_id != (uint8)-1 || vehicle->sound2_id != (uint8)-1) {
			if (vehicle->sprite_left != 0x8000) {
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
						//for (i = RCT2_ADDRESS(0x00F438B4, rct_vehicle_sound_params); i < RCT2_GLOBAL(0x00F438B0, rct_vehicle_sound_params*) && v9 <= i->var_A; i++);
						for (i = &gVehicleSoundParamsList[0]; i < gVehicleSoundParamsListEnd && v9 <= i->var_A; i++);
						//if (i < RCT2_ADDRESS(0x00F43908, rct_vehicle_sound_params)) { // 0x00F43908 is end of rct_vehicle_sound_params list, which has 7 elements, not to be confused with variable at 0x00F43908
						if (i < &gVehicleSoundParamsList[countof(gVehicleSoundParamsList)]) {
							//if (RCT2_GLOBAL(0x00F438B0, rct_vehicle_sound_params*) < RCT2_ADDRESS(0x00F43908, rct_vehicle_sound_params)) {
							//	RCT2_GLOBAL(0x00F438B0, rct_vehicle_sound_params*)++;
							//}
							if (gVehicleSoundParamsListEnd < &gVehicleSoundParamsList[countof(gVehicleSoundParamsList)]) {
								gVehicleSoundParamsListEnd++;
							}
							//rct_vehicle_sound_params* j = RCT2_GLOBAL(0x00F438B0, rct_vehicle_sound_params*) - 1;
							rct_vehicle_sound_params* j = gVehicleSoundParamsListEnd - 1;
							while (j >= i) {
								j--;
								*(j + 1) = *j;
							}
							i->var_A = v9;
							int panx = (RCT2_GLOBAL(0x009AF5A0, sint16) / 2) + (RCT2_GLOBAL(0x009AF5A4, sint16) / 2) - RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_x;
							panx >>= RCT2_GLOBAL(0x00F438A4, rct_viewport*)->zoom;
							panx += RCT2_GLOBAL(0x00F438A4, rct_viewport*)->x;

							uint16 screenwidth = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
							if (screenwidth < 64) {
								screenwidth = 64;
							}
							i->panx = ((((panx << 16) / screenwidth) - 0x8000) >> 4);

							int pany = (RCT2_GLOBAL(0x009AF5A2, sint16) / 2) + (RCT2_GLOBAL(0x009AF5A6, sint16) / 2) - RCT2_GLOBAL(0x00F438A4, rct_viewport*)->view_y;
							pany >>= RCT2_GLOBAL(0x00F438A4, rct_viewport*)->zoom;
							pany += RCT2_GLOBAL(0x00F438A4, rct_viewport*)->y;

							uint16 screenheight = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);
							if (screenheight < 64) {
								screenheight = 64;
							}
							i->pany = ((((pany << 16) / screenheight) - 0x8000) >> 4);

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
							v19 *= 5512;
							v19 >>= 14;
							v19 += 11025;
							v19 += 16 * vehicle->var_BF;
							i->frequency = (uint16)v19;
							i->id = vehicle->sprite_index;
							i->volume = 0;
							if (vehicle->x != 0x8000) {
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
		result += vehicle_temp->var_46;
	} while (vehicle_temp->next_vehicle_on_train != (uint16)-1 && (vehicle_temp = GET_VEHICLE(vehicle_temp->next_vehicle_on_train)));
	sint32 v4 = vehicle->velocity;
	if (v4 < 0) {
		v4 = -v4;
	}
	result += ((uint16)v4) >> 13;
	rct_vehicle_sound* vehicle_sound = &gVehicleSoundList[0];
	//rct_vehicle_sound* vehicle_sound = RCT2_ADDRESS(RCT2_ADDRESS_VEHICLE_SOUND_LIST, rct_vehicle_sound);
	while (vehicle_sound->id != vehicle->sprite_index) {
		vehicle_sound++;
		//if (vehicle_sound >= RCT2_GLOBAL(0x009AF42C, rct_vehicle_sound*)) {
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
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) != -1 && !RCT2_GLOBAL(0x009AF59C, uint8) && RCT2_GLOBAL(0x009AF59D, uint8) & 1) {
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
		//label12:
			//RCT2_GLOBAL(0x00F438B0, rct_vehicle_sound_params**) = &RCT2_GLOBAL(0x00F438B4, rct_vehicle_sound_params*);
			gVehicleSoundParamsListEnd = &gVehicleSoundParamsList[0];
			for (uint16 i = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_VEHICLE, uint16); i != SPRITE_INDEX_NULL; i = g_sprite_list[i].vehicle.next) {
				vehicle_update_sound_params(&g_sprite_list[i].vehicle);
			}
			//for (rct_vehicle_sound* vehicle_sound = &RCT2_GLOBAL(RCT2_ADDRESS_VEHICLE_SOUND_LIST, rct_vehicle_sound); vehicle_sound != &RCT2_GLOBAL(0x009AF42C, rct_vehicle_sound); vehicle_sound++) {
			for(int i = 0; i < countof(gVehicleSoundList); i++){
				rct_vehicle_sound* vehicle_sound = &gVehicleSoundList[i];
				if (vehicle_sound->id != (uint16)-1) {
					//for (rct_vehicle_sound_params* vehicle_sound_params = &RCT2_GLOBAL(0x00F438B4, rct_vehicle_sound_params); vehicle_sound_params != RCT2_GLOBAL(0x00F438B0, rct_vehicle_sound_params*); vehicle_sound_params++) {
					for (rct_vehicle_sound_params* vehicle_sound_params = &gVehicleSoundParamsList[0]; vehicle_sound_params != gVehicleSoundParamsListEnd; vehicle_sound_params++) {
						if (vehicle_sound->id == vehicle_sound_params->id) {
							goto label26;
						}
					}
					if (vehicle_sound->sound1_id != (uint16)-1) {
#ifdef USE_MIXER
						Mixer_Stop_Channel(vehicle_sound->sound1_channel);
#else
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_stop(&vehicle_sound->sound1);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
					}
					if (vehicle_sound->sound2_id != (uint16)-1) {
#ifdef USE_MIXER
						Mixer_Stop_Channel(vehicle_sound->sound2_channel);
#else
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_stop(&vehicle_sound->sound2);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
					}
					vehicle_sound->id = (uint16)-1;
				}
			label26:
				1;
			}

			//for (rct_vehicle_sound_params* vehicle_sound_params = &RCT2_GLOBAL(0x00F438B4, rct_vehicle_sound_params); ; vehicle_sound_params++) {
			for (rct_vehicle_sound_params* vehicle_sound_params = &gVehicleSoundParamsList[0]; ; vehicle_sound_params++) {
			label28:
				//if (vehicle_sound_params >= RCT2_GLOBAL(0x00F438B0, rct_vehicle_sound_params*)) {
				if (vehicle_sound_params >= gVehicleSoundParamsListEnd) {
					return;
				}
				uint8 vol1 = 0xFF;
				uint8 vol2 = 0xFF;
				sint16 pany = vehicle_sound_params->pany;
				if (pany < 0) {
					pany = -pany;
				}
				if (pany > 0xFFF) {
					pany = 0xFFF;
				}
				pany -= 0x800;
				if (pany > 0) {
					pany -= 0x400;
					pany = -pany;
					pany = pany / 4;
					vol1 = LOBYTE(pany);
					if ((sint8)HIBYTE(pany) != 0) {
						vol1 = 0xFF;
						if ((sint8)HIBYTE(pany) < 0) {
							vol1 = 0;
						}
					}
				}

				sint16 panx = vehicle_sound_params->panx;
				if (panx < 0) {
					panx = -panx;
				}
				if (panx > 0xFFF) {
					panx = 0xFFF;
				}
				panx -= 0x800;
				if (panx > 0) {
					panx -= 0x400;
					panx = -panx;
					panx = panx / 4;
					vol2 = LOBYTE(panx);
					if ((sint8)HIBYTE(panx) != 0) {
						vol2 = 0xFF;
						if ((sint8)HIBYTE(panx) < 0) {
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
				//rct_vehicle_sound* vehicle_sound = &RCT2_GLOBAL(RCT2_ADDRESS_VEHICLE_SOUND_LIST, rct_vehicle_sound);
				while (vehicle_sound_params->id != vehicle_sound->id) {
					vehicle_sound++; // went here 2x
					//if (vehicle_sound >= &RCT2_GLOBAL(0x009AF42C, rct_vehicle_sound)) {
					if (vehicle_sound >= &gVehicleSoundList[countof(gVehicleSoundList)]) {
						//vehicle_sound = &RCT2_GLOBAL(RCT2_ADDRESS_VEHICLE_SOUND_LIST, rct_vehicle_sound);
						vehicle_sound = &gVehicleSoundList[0];
						int i = 0;
						while (vehicle_sound->id != (uint16)-1) {
							vehicle_sound++;
							i++;
							if (i >= countof(gVehicleSoundList)/*i >= RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_MAX_VEHICLE_SOUNDS, uint8)*/) {
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
#ifdef USE_MIXER
						Mixer_Stop_Channel(vehicle_sound->sound1_channel);
#else
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_stop(&vehicle_sound->sound1);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif			
					}
				} else {
					if (vehicle_sound->sound1_id == (uint16)-1) {
						goto label69;
					}
					if (sprite->vehicle.sound1_id != vehicle_sound->sound1_id) {
#ifdef USE_MIXER
						Mixer_Stop_Channel(vehicle_sound->sound1_channel);
#else
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_stop(&vehicle_sound->sound1);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
					label69:
						vehicle_sound->sound1_id = sprite->vehicle.sound1_id;
#ifndef USE_MIXER
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_prepare(sprite->vehicle.sound1_id, &vehicle_sound->sound1, 1, RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_SW_BUFFER, uint32));
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
						vehicle_sound->sound1_pan = vehicle_sound_params->panx;
						vehicle_sound->sound1_volume = volume;
						vehicle_sound->sound1_freq = vehicle_sound_params->frequency;
						uint16 frequency = vehicle_sound_params->frequency;
						if (RCT2_ADDRESS(0x009AF51F, uint8)[2 * sprite->vehicle.sound1_id] & 2) {
							frequency = (frequency / 2) + 4000;
						}
						uint8 looping = RCT2_ADDRESS(0x009AF51E, uint8)[2 * sprite->vehicle.sound1_id];
						int pan = vehicle_sound_params->panx;
						if (!RCT2_GLOBAL(0x009AAC6D, uint8)) {
							pan = 0;
						}
#ifdef USE_MIXER
						vehicle_sound->sound1_channel = Mixer_Play_Effect(sprite->vehicle.sound1_id, looping ? MIXER_LOOP_INFINITE : MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(pan), DStoMixerRate(frequency), 0);
#else
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_play(&vehicle_sound->sound1, looping, volume, pan, frequency);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
						goto label87;
					}
					if (volume != vehicle_sound->sound1_volume) {
						vehicle_sound->sound1_volume = volume;
#ifdef USE_MIXER
						Mixer_Channel_Volume(vehicle_sound->sound1_channel, DStoMixerVolume(volume));
#else
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_set_volume(&vehicle_sound->sound1, volume);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
					}
					if (vehicle_sound_params->panx != vehicle_sound->sound1_pan) {
						vehicle_sound->sound1_pan = vehicle_sound_params->panx;
						if (RCT2_GLOBAL(0x009AAC6D, uint8)) {
#ifdef USE_MIXER
							Mixer_Channel_Pan(vehicle_sound->sound1_channel, DStoMixerPan(vehicle_sound_params->panx));
#else
							RCT2_GLOBAL(0x014241BC, uint32) = 1;
							sound_set_pan(&vehicle_sound->sound1, vehicle_sound_params->panx);
							RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
						}
					}
					if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 3) && vehicle_sound_params->frequency != vehicle_sound->sound1_freq) {
						vehicle_sound->sound1_freq = vehicle_sound_params->frequency;
						uint16 frequency = vehicle_sound_params->frequency;
						if (RCT2_GLOBAL(0x009AF51F, uint8*)[2 * sprite->vehicle.sound1_id] & 2) {
							frequency = (frequency / 2) + 4000;
						}
#ifdef USE_MIXER
						Mixer_Channel_Rate(vehicle_sound->sound1_channel, DStoMixerRate(frequency));
#else
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_set_frequency(&vehicle_sound->sound1, frequency);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
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
#ifdef USE_MIXER
						Mixer_Stop_Channel(vehicle_sound->sound2_channel);
#else
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_stop(&vehicle_sound->sound2);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif	
					}
				} else {
					if (vehicle_sound->sound2_id == (uint16)-1) {
						goto label93;
					}
					if (sprite->vehicle.sound2_id != vehicle_sound->sound2_id) {
#ifdef USE_MIXER
						Mixer_Stop_Channel(vehicle_sound->sound2_channel);
#else
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_stop(&vehicle_sound->sound2);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
					label93:
						vehicle_sound->sound2_id = sprite->vehicle.sound2_id;
#ifndef USE_MIXER
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_prepare(sprite->vehicle.sound2_id, &vehicle_sound->sound2, 1, RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_SW_BUFFER, uint32));
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
						vehicle_sound->sound2_pan = vehicle_sound_params->panx;
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
						int pan = vehicle_sound_params->panx;
						if (!RCT2_GLOBAL(0x009AAC6D, uint8)) {
							pan = 0;
						}
#ifdef USE_MIXER
						vehicle_sound->sound2_channel = Mixer_Play_Effect(sprite->vehicle.sound2_id, looping ? MIXER_LOOP_INFINITE : MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(pan), DStoMixerRate(frequency), 0);
#else
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_play(&vehicle_sound->sound2, looping, volume, pan, frequency);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
						goto label114;
					}
					if (volume != vehicle_sound->sound2_volume) {
#ifdef USE_MIXER
						Mixer_Channel_Volume(vehicle_sound->sound2_channel, DStoMixerVolume(volume));
#else
						RCT2_GLOBAL(0x014241BC, uint32) = 1;
						sound_set_volume(&vehicle_sound->sound2, volume);
						RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
						vehicle_sound->sound2_volume = volume;
					}
					if (vehicle_sound_params->panx != vehicle_sound->sound2_pan) {
						vehicle_sound->sound2_pan = vehicle_sound_params->panx;
						if (RCT2_GLOBAL(0x009AAC6D, uint8)) {
#ifdef USE_MIXER
							Mixer_Channel_Pan(vehicle_sound->sound2_channel, DStoMixerPan(vehicle_sound_params->panx));
#else
							RCT2_GLOBAL(0x014241BC, uint32) = 1;
							sound_set_pan(&vehicle_sound->sound2, vehicle_sound_params->panx);
							RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
						}
					}
					if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 3) && vehicle_sound_params->frequency != vehicle_sound->sound2_freq) {
						vehicle_sound->sound2_freq = vehicle_sound_params->frequency;
						if (!(RCT2_ADDRESS(0x009AF51F, uint8)[2 * sprite->vehicle.sound2_id] & 1)) {
							uint16 frequency = (vehicle_sound_params->frequency * 2) - 3248;
							if (frequency > 25700) {
								frequency = 25700;
							}
#ifdef USE_MIXER
							Mixer_Channel_Rate(vehicle_sound->sound2_channel, DStoMixerRate(frequency));
#else
							RCT2_GLOBAL(0x014241BC, uint32) = 1;
							sound_set_frequency(&vehicle_sound->sound2, frequency);
							RCT2_GLOBAL(0x014241BC, uint32) = 0;
#endif
							
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

void vehicle_set_map_toolbar(rct_vehicle *vehicle)
{
	rct_ride *ride;
	int vehicleIndex;

	ride = GET_RIDE(vehicle->ride);

	while (vehicle->var_01 != 0)
		vehicle = &(g_sprite_list[vehicle->prev_vehicle_on_train].vehicle);

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
		prevVehicle = &(g_sprite_list[vehicle->prev_vehicle_on_train].vehicle);
		if (prevVehicle->next_vehicle_on_train == SPRITE_INDEX_NULL)
			break;

		vehicle = prevVehicle;
	}

	return vehicle;
}