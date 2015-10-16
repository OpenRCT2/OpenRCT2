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
#include "../config.h"
#include "../interface/viewport.h"
#include "../interface/window.h"
#include "../localisation/language.h"
#include "../platform/platform.h"
#include "../ride/ride.h"
#include "../world/map.h"
#include "../world/sprite.h"
#include "audio.h"
#include "mixer.h"
#include "../openrct2.h"

int gAudioDeviceCount;
audio_device *gAudioDevices = NULL;
rct_vehicle_sound gVehicleSoundList[AUDIO_MAX_VEHICLE_SOUNDS];
rct_vehicle_sound_params gVehicleSoundParamsList[AUDIO_MAX_VEHICLE_SOUNDS];
rct_vehicle_sound_params *gVehicleSoundParamsListEnd;
rct_ride_music gRideMusicList[AUDIO_MAX_RIDE_MUSIC];
rct_ride_music_params gRideMusicParamsList[AUDIO_MAX_RIDE_MUSIC];
rct_ride_music_params *gRideMusicParamsListEnd;
void *gCrowdSoundChannel = 0;
void *gTitleMusicChannel = 0;
bool gGameSoundsOff = false;

void audio_init(int i)
{
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		log_fatal("SDL_Init %s", SDL_GetError());
		exit(-1);
	}
}

void audio_quit()
{
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

/**
 * Populates audio devices.
 */
void audio_get_devices()
{
	int i;

	if (gAudioDevices != NULL)
		free(gAudioDevices);

	gAudioDeviceCount = SDL_GetNumAudioDevices(SDL_FALSE);
	if (gAudioDeviceCount > 0) {
		gAudioDeviceCount++;
		gAudioDevices = malloc(gAudioDeviceCount * sizeof(audio_device));

		strcpy(gAudioDevices[0].name, language_get_string(5510));
		for (i = 1; i < gAudioDeviceCount; i++) {
			const char *utf8_name = SDL_GetAudioDeviceName(i - 1, SDL_FALSE);
			if (utf8_name == NULL)
				utf8_name = language_get_string(5511);

			strcpy(gAudioDevices[i].name, utf8_name);
		}
	}
}

/**
*
*  rct2: 0x006BB76E
*
* @param sound_id (eax)
* @param ebx (ebx)
* @param x (cx)
* @param y (dx)
* @param z (bp)
*/
int sound_play_panned(int sound_id, int ebx, sint16 x, sint16 y, sint16 z)
{
	if (!gGameSoundsOff) {
		int volumedown = 0;
		int volume = 0;
		if (ebx == 0x8001) {
			rct_map_element* mapelement = map_get_surface_element_at(x / 32, y / 32);
			if (mapelement) {
				if ((mapelement->base_height * 8) - 5 > z) {
					volumedown = 10;
				}
			}
			sint16 rx;
			sint16 ry;
			switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)) {
				case 0:
					rx = y - x;
					ry = ((y + x) / 2) - z;
					break;
				case 1:
					rx = -x - y;
					ry = ((y - x) / 2) - z;
					break;
				case 2:
					rx = x - y;
					ry = ((-y - x) / 2) - z;
					break;
				case 3:
					rx = y + x;
					ry = ((x - y) / 2) - z;
					break;
			}
			rct_window* window = RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*);
			while (1) {
				window--;
				if (window < RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_LIST, rct_window)) {
					break;
				}
				rct_viewport* viewport = window->viewport;
				if (viewport && viewport->flags & VIEWPORT_FLAG_SOUND_ON) {
					sint16 vy = ry - viewport->view_y;
					sint16 vx = rx - viewport->view_x;
					ebx = viewport->x + (vx >> viewport->zoom);
					volume = RCT2_ADDRESS(0x0099282C, int)[sound_id] + ((-1024 * viewport->zoom - 1) << volumedown) + 1;
					if (vy < 0 || vy >= viewport->view_height || vx < 0 || vx >= viewport->view_width || volume < -10000) {
						return sound_id;
					}
				}
			}
		}
		int pan;
		if (ebx == (sint16)0x8000) {
			pan = 0;
		} else {
			int x2 = ebx << 16;
			uint16 screenwidth = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
			if (screenwidth < 64) {
				screenwidth = 64;
			}
			pan = ((x2 / screenwidth) - 0x8000) >> 4;
		}
		Mixer_Play_Effect(sound_id, MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(pan), 1, 1);
	}
	return 0;
}

/**
*
*  rct2: 0x006BD0F8
*/
void start_title_music()
{
	int musicPathId;
	switch (gConfigSound.title_music) {
	default:
		return;
	case 1:
		musicPathId = PATH_ID_CSS50;
		break;
	case 2:
		musicPathId = PATH_ID_CSS17;
		break;
	case 3:
		if (rand() & 1)
			musicPathId = PATH_ID_CSS50;
		else
			musicPathId = PATH_ID_CSS17;
		break;
	}

	if (!gGameSoundsOff && RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TITLE_DEMO) {
		if (!gTitleMusicChannel) {
			gTitleMusicChannel = Mixer_Play_Music(musicPathId, MIXER_LOOP_INFINITE, true);
		}
	} else {
		stop_title_music();
	}
}

/**
*
*  rct2: 0x006BCA9F
*/
void stop_ride_music()
{
	for (int i = 0; i < AUDIO_MAX_RIDE_MUSIC; i++) {
		rct_ride_music* ride_music = &gRideMusicList[i];
		if (ride_music->rideid != (uint8)-1) {
			if (ride_music->sound_channel) {
				Mixer_Stop_Channel(ride_music->sound_channel);
			}
			ride_music->rideid = -1;
		}
	}
}

/**
*
*  rct2: 0x006BD07F
*/
void stop_crowd_sound()
{
	if (gCrowdSoundChannel) {
		Mixer_Stop_Channel(gCrowdSoundChannel);
		gCrowdSoundChannel = 0;
	}
}

/**
*
*  rct2: 0x006BD0BD
*/
void stop_title_music()
{
	if (gTitleMusicChannel) {
		Mixer_Stop_Channel(gTitleMusicChannel);
		gTitleMusicChannel = 0;
	}
}

/**
*
*  rct2: 0x006BA8E0
*/
void audio_init1()
{
	int devicenum = 0;
	audio_init2(devicenum);

	for(int m = 0; m < countof(ride_music_info_list); m++) {
		rct_ride_music_info* ride_music_info = ride_music_info_list[m];
		const utf8* path = get_file_path(ride_music_info->pathid);
		SDL_RWops *file = SDL_RWFromFile(path, "rb");
		if (file != NULL) {
			uint32 head;
			SDL_RWread(file, &head, sizeof(head), 1);
			SDL_RWclose(file);
			RCT2_GLOBAL(0x014241BC, uint32) = 0;
			if (head == 0x78787878) {
				ride_music_info->length = 0;
			}
		}
	}
}

/**
*
*  rct2: 0x006BA9B5
*/
void audio_init2(int device)
{
	audio_close();
	for (int i = 0; i < AUDIO_MAX_VEHICLE_SOUNDS; i++) {
		rct_vehicle_sound* vehicle_sound = &gVehicleSoundList[i];
		vehicle_sound->id = -1;
	}
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) = device;
	config_save_default();
	for (int i = 0; i < AUDIO_MAX_RIDE_MUSIC; i++) {
		rct_ride_music* ride_music = &gRideMusicList[i];
		ride_music->rideid = -1;
	}
}

/**
*
*  rct2: 0x006BAB21
*/
void audio_close()
{
	stop_crowd_sound();
	stop_title_music();
	stop_ride_music();
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) = -1;
}

/* rct2: 0x006BAB8A */
void toggle_all_sounds(){
	gConfigSound.sound = !gConfigSound.sound;
	if (!gConfigSound.sound) {
		stop_title_music();
		pause_sounds();
	} else {
		unpause_sounds();
	}
}

/**
*
*  rct2: 0x006BABB4
*/
void pause_sounds()
{
	gGameSoundsOff = true;
	stop_vehicle_sounds();
	stop_ride_music();
	stop_crowd_sound();
}

/**
*
*  rct2: 0x006BABD8
*/
void unpause_sounds()
{
	gGameSoundsOff = false;
}

/**
*
*  rct2: 0x006BABDF
*/
void stop_vehicle_sounds()
{
	if (!gOpenRCT2Headless && RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, sint32) != -1) {
		for (int i = 0; i < countof(gVehicleSoundList); i++) {
			rct_vehicle_sound* vehicle_sound = &gVehicleSoundList[i];
			if (vehicle_sound->id != 0xFFFF) {
				if (vehicle_sound->sound1_id != 0xFFFF) {
					Mixer_Stop_Channel(vehicle_sound->sound1_channel);
				}
				if (vehicle_sound->sound2_id != 0xFFFF) {
					Mixer_Stop_Channel(vehicle_sound->sound2_channel);
				}
			}
			vehicle_sound->id = 0xFFFF;
		}
	}
}
