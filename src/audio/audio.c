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
#include "../util/util.h"

typedef struct rct_audio_params {
	bool in_range;
	int volume;
	int pan;
} rct_audio_params;

audio_device *gAudioDevices = NULL;
int gAudioDeviceCount;
void *gCrowdSoundChannel = 0;
bool gGameSoundsOff = false;
void *gRainSoundChannel = 0;
rct_ride_music gRideMusicList[AUDIO_MAX_RIDE_MUSIC];
rct_ride_music_info *gRideMusicInfoList[NUM_DEFAULT_MUSIC_TRACKS];
rct_ride_music_params gRideMusicParamsList[AUDIO_MAX_RIDE_MUSIC];
rct_ride_music_params *gRideMusicParamsListEnd;
void *gTitleMusicChannel = 0;
rct_vehicle_sound gVehicleSoundList[AUDIO_MAX_VEHICLE_SOUNDS];
rct_vehicle_sound_params gVehicleSoundParamsList[AUDIO_MAX_VEHICLE_SOUNDS];
rct_vehicle_sound_params *gVehicleSoundParamsListEnd;

rct_audio_params audio_get_params_from_location(int soundId, const rct_xyz16 *location);
void audio_stop_channel(void **channel);

void audio_init()
{
	int result = SDL_Init(SDL_INIT_AUDIO);
	if (result >= 0)
		return;

	log_fatal("SDL_Init %s", SDL_GetError());
	exit(-1);
}

void audio_quit()
{
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void audio_populate_devices()
{
	if (gAudioDevices != NULL)
		free(gAudioDevices);

	gAudioDeviceCount = SDL_GetNumAudioDevices(SDL_FALSE);
	if (gAudioDeviceCount <= 0)
		return;

	gAudioDeviceCount++;
	gAudioDevices = malloc(gAudioDeviceCount * sizeof(audio_device));
	safe_strncpy(gAudioDevices[0].name, language_get_string(5510), AUDIO_DEVICE_NAME_SIZE);

	for (int i = 1; i < gAudioDeviceCount; i++) {
		const char *utf8Name = SDL_GetAudioDeviceName(i - 1, SDL_FALSE);
		if (utf8Name == NULL)
			utf8Name = language_get_string(5511);

		safe_strncpy(gAudioDevices[i].name, utf8Name, AUDIO_DEVICE_NAME_SIZE);
	}
}

int audio_play_sound_panned(int soundId, int pan, sint16 x, sint16 y, sint16 z)
{
	if (pan == AUDIO_PLAY_AT_LOCATION)
		return audio_play_sound_at_location(soundId, x, y, z);

	return audio_play_sound(soundId, 0, pan);
}

int audio_play_sound_at_location(int soundId, sint16 x, sint16 y, sint16 z)
{
	if (gGameSoundsOff)
		return 0;

	rct_xyz16 location;
	location.x = x;
	location.y = y;
	location.z = z;

	rct_audio_params params = audio_get_params_from_location(soundId, &location);
	if (!params.in_range)
		return soundId;

	return audio_play_sound(soundId, params.volume, params.pan);
}

/**
* Returns the audio parameters to use when playing the specified sound at a virtual location.
* @param soundId The sound effect to be played.
* @param location The location at which the sound effect is to be played.
* @return The audio parameters to be used when playing this sound effect.
*/
rct_audio_params audio_get_params_from_location(int soundId, const rct_xyz16 *location)
{
	int volumeDown = 0;
	rct_audio_params params;
	params.in_range = true;

	rct_map_element *element = map_get_surface_element_at(location->x / 32, location->y / 32);
	if (element && (element->base_height * 8) - 5 > location->z)
		volumeDown = 10;

	uint8 rotation = get_current_rotation();
	rct_xy16 pos2 = coordinate_3d_to_2d(location, rotation);
	rct_window *window = RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*);
	while (true) {
		window--;
		if (window < RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_LIST, rct_window))
			break;

		rct_viewport *viewport = window->viewport;
		if (!viewport || !(viewport->flags & VIEWPORT_FLAG_SOUND_ON))
			continue;

		sint16 vy = pos2.y - viewport->view_y;
		sint16 vx = pos2.x - viewport->view_x;
		params.pan = viewport->x + (vx >> viewport->zoom);
		params.volume = RCT2_ADDRESS(0x0099282C, int)[soundId] + ((-1024 * viewport->zoom - 1) << volumeDown) + 1;

		if (vy < 0 || vy >= viewport->view_height || vx < 0 || vx >= viewport->view_width || params.volume < -10000) {
			params.in_range = false;
			return params;
		}
	}

	return params;
}

int audio_play_sound(int soundId, int volume, int pan)
{
	if (gGameSoundsOff)
		return 0;

	int mixerPan = 0;
	if (pan != AUDIO_PLAY_AT_CENTRE) {
		int x2 = pan << 16;
		uint16 screenWidth = max(64, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16));
		mixerPan = ((x2 / screenWidth) - 0x8000) >> 4;
	}

	Mixer_Play_Effect(soundId, MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(mixerPan), 1, 1);
	return 0;
}

void audio_start_title_music()
{
	if (gGameSoundsOff || !(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TITLE_DEMO)) {
		audio_stop_title_music();
		return;
	}

	if (gTitleMusicChannel)
		return;

	int pathId;
	switch (gConfigSound.title_music) {
	case 1:
		pathId = PATH_ID_CSS50;
		break;
	case 2:
		pathId = PATH_ID_CSS17;
		break;
	case 3:
		if (rand() & 1)
			pathId = PATH_ID_CSS50;
		else
			pathId = PATH_ID_CSS17;
		break;
	default:
		return;
	}

	gTitleMusicChannel = Mixer_Play_Music(pathId, MIXER_LOOP_INFINITE, true);
}

void audio_stop_ride_music()
{
	for (int i = 0; i < AUDIO_MAX_RIDE_MUSIC; i++) {
		rct_ride_music *rideMusic = &gRideMusicList[i];
		if (rideMusic->ride_id == (uint8)-1)
			continue;

		if (rideMusic->sound_channel)
			Mixer_Stop_Channel(rideMusic->sound_channel);

		rideMusic->ride_id = -1;
	}
}

void audio_stop_crowd_sound()
{
	audio_stop_channel(&gCrowdSoundChannel);
}

void audio_stop_title_music()
{
	audio_stop_channel(&gTitleMusicChannel);
}

void audio_stop_rain_sound()
{
	audio_stop_channel(&gRainSoundChannel);
}

/**
* Stops the specified audio channel from playing.
* @param channel The channel to stop.
*/
void audio_stop_channel(void **channel)
{
	if (!*channel)
		return;

	Mixer_Stop_Channel(*channel);
	*channel = 0;
}

void audio_init_ride_sounds_and_info()
{
	int deviceNum = 0;
	audio_init_ride_sounds(deviceNum);

	for (int m = 0; m < countof(gRideMusicInfoList); m++) {
		rct_ride_music_info *rideMusicInfo = gRideMusicInfoList[m];
		const utf8 *path = get_file_path(rideMusicInfo->path_id);
		SDL_RWops *file = SDL_RWFromFile(path, "rb");
		if (file == NULL)
			continue;

		uint32 head;
		SDL_RWread(file, &head, sizeof(head), 1);
		SDL_RWclose(file);
		RCT2_GLOBAL(0x014241BC, uint32) = 0;
		if (head == 0x78787878)
			rideMusicInfo->length = 0;
	}
}

void audio_init_ride_sounds(int device)
{
	audio_close();
	for (int i = 0; i < AUDIO_MAX_VEHICLE_SOUNDS; i++) {
		rct_vehicle_sound *vehicleSound = &gVehicleSoundList[i];
		vehicleSound->id = -1;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) = device;
	config_save_default();
	for (int i = 0; i < AUDIO_MAX_RIDE_MUSIC; i++) {
		rct_ride_music *rideMusic = &gRideMusicList[i];
		rideMusic->ride_id = -1;
	}
}

void audio_close()
{
	audio_stop_crowd_sound();
	audio_stop_title_music();
	audio_stop_ride_music();
	audio_stop_rain_sound();
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) = -1;
}

void audio_toggle_all_sounds(){
	gConfigSound.sound = !gConfigSound.sound;
	if (gConfigSound.sound)
		audio_unpause_sounds();
	else {
		audio_stop_title_music();
		audio_pause_sounds();
	}
}

void audio_pause_sounds()
{
	gGameSoundsOff = true;
	audio_stop_vehicle_sounds();
	audio_stop_ride_music();
	audio_stop_crowd_sound();
	audio_stop_rain_sound();
}

void audio_unpause_sounds()
{
	gGameSoundsOff = false;
}

void audio_stop_vehicle_sounds()
{
	if (gOpenRCT2Headless || RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, sint32) == -1)
		return;

	for (int i = 0; i < countof(gVehicleSoundList); i++) {
		rct_vehicle_sound *vehicleSound = &gVehicleSoundList[i];
		if (vehicleSound->id == 0xFFFF)
			continue;

		if (vehicleSound->sound1_id != 0xFFFF)
			Mixer_Stop_Channel(vehicleSound->sound1_channel);

		if (vehicleSound->sound2_id != 0xFFFF)
			Mixer_Stop_Channel(vehicleSound->sound2_channel);

		vehicleSound->id = 0xFFFF;
	}
}
