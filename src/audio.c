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

#include <SDL.h>
#include "audio.h"
#include "addresses.h"
#include "rct2.h"

int gAudioDeviceCount;
audio_device *gAudioDevices = NULL;

void audio_init(int i)
{
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		RCT2_ERROR("SDL_Init %s", SDL_GetError());
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

		strcpy(gAudioDevices[0].name, "Default sound device");
		for (i = 1; i < gAudioDeviceCount; i++) {
			const char *utf8_name = SDL_GetAudioDeviceName(i - 1, SDL_FALSE);
			if (utf8_name == NULL)
				utf8_name = "(UNKNOWN)";

			strcpy(gAudioDevices[i].name, utf8_name);
		}
	}
}

/**
*
*  rct2: 0x0040502E
*/
void get_dsound_devices()
{
	RCT2_CALLPROC(0x0040502E);
}

int sound_prepare(int sound_id, rct_sound *sound, int var_8, int var_c)
{
	return RCT2_CALLFUNC_4(0x00404C6D, int, int, rct_sound*, int, int, sound_id, sound, var_8, var_c);
}

void sound_play_panned(int sound_id, int x)
{
	RCT2_CALLPROC_X(0x006BB76E, sound_id, x, 0, 0, 0, 0, 0);
}

/**
*
*  rct2: 0x00401999
*/
int sound_channel_play(int channel, int a2, int volume, int pan, int frequency)
{
	RCT2_GLOBAL(0x1426444 + (91 * channel * 4), uint32) = a2;
	sound_channel_set_frequency(channel, frequency);
	sound_channel_set_pan(channel, pan);
	sound_channel_set_volume(channel, volume);
	LPDIRECTSOUNDBUFFER dsbuffer = RCT2_ADDRESS(RCT2_ADDRESS_DSOUND_BUFFERS, LPDIRECTSOUNDBUFFER)[channel];
	dsbuffer->lpVtbl->SetCurrentPosition(dsbuffer, 0);
	dsbuffer->lpVtbl->Play(dsbuffer, 0, 0, DSBPLAY_LOOPING);
	RCT2_GLOBAL(0x14262E0 + (91 * channel * 4), uint32) = 1;
	return 1;
}

/**
*
*  rct2: 0x00401A93
*/
int sound_channel_set_frequency(int channel, int frequency)
{
	LPDIRECTSOUNDBUFFER dsbuffer = RCT2_ADDRESS(RCT2_ADDRESS_DSOUND_BUFFERS, LPDIRECTSOUNDBUFFER)[channel];
	if(dsbuffer){
		if(SUCCEEDED(dsbuffer->lpVtbl->SetFrequency(dsbuffer, frequency))){
			return 1;
		}
	}
	return 0;
}

/**
*
*  rct2: 0x00401AB3
*/
int sound_channel_set_pan(int channel, int pan)
{
	LPDIRECTSOUNDBUFFER dsbuffer = RCT2_ADDRESS(RCT2_ADDRESS_DSOUND_BUFFERS, LPDIRECTSOUNDBUFFER)[channel];
	if(dsbuffer){
		if(SUCCEEDED(dsbuffer->lpVtbl->SetPan(dsbuffer, pan))){
			return 1;
		}
	}
	return 0;
}

/**
*
*  rct2: 0x00401AD3
*/
int sound_channel_set_volume(int channel, int volume)
{
	LPDIRECTSOUNDBUFFER dsbuffer = RCT2_ADDRESS(RCT2_ADDRESS_DSOUND_BUFFERS, LPDIRECTSOUNDBUFFER)[channel];
	if(dsbuffer){
		if(SUCCEEDED(dsbuffer->lpVtbl->SetVolume(dsbuffer, volume))){
			return 1;
		}
	}
	return 0;
}

/**
*
*  rct2: 0x00404E7F
*/
int sound_play(rct_sound* sound, int looping, int volume, int pan, int frequency)
{
	if(sound){
		sound_set_frequency(sound, frequency);
		sound_set_pan(sound, pan);
		sound_set_volume(sound, volume);
		DWORD playflags;
		if(looping){
			if(looping != 1)
				return 1;
			playflags = DSBPLAY_LOOPING;
		}else{
			playflags = 0;
		}
		if(SUCCEEDED(sound->dsbuffer->lpVtbl->Play(sound->dsbuffer, 0, 0, playflags))){
			return 1;
		}
	}
	return 0;
}

/**
*
*  rct2: 0x00404ED7
*/
int sound_set_frequency(rct_sound* sound, int frequency)
{
	if(sound){
		if(SUCCEEDED(sound->dsbuffer->lpVtbl->SetFrequency(sound->dsbuffer, frequency))){
			return 1;
		}
	}
	return 0;
}

/**
*
*  rct2: 0x00404EF2
*/
int sound_set_pan(rct_sound* sound, int pan)
{
	if(sound){
		if(SUCCEEDED(sound->dsbuffer->lpVtbl->SetPan(sound->dsbuffer, pan))){
			return 1;
		}
	}
	return 0;
}

/**
*
*  rct2: 0x00404F0D
*/
int sound_set_volume(rct_sound* sound, int volume)
{
	if(sound){
		if(SUCCEEDED(sound->dsbuffer->lpVtbl->SetVolume(sound->dsbuffer, volume))){
			return 1;
		}
	}
	return 0;
}

/**
*
*  rct2: 0x00404DD8
*/
void sound_stop(rct_sound* sound)
{
	if(sound->dsbuffer){
		sound->dsbuffer->lpVtbl->Release(sound->dsbuffer);
		sound->dsbuffer = 0;
		sound_remove(sound);
	}
}

/**
*
*  rct2: 0x00405143
*/
rct_sound* sound_remove(rct_sound* sound)
{
	rct_sound* result = RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_BEGIN, rct_sound*);
	if(sound == result){
		if(sound == RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_END, rct_sound*)){
			RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_END, rct_sound*) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_BEGIN, rct_sound*) = 0;
		}
		result = sound->next;
		RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_BEGIN, rct_sound*) = result;
	}
	else{
		while(result->next != sound){
			result = result->next;
		}
		if(sound == RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_END, rct_sound*)){
			RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_END, rct_sound*) = result;
			result->next = 0;
		}
		else{
			result->next = sound->next;
		}
	}
	sound->next = 0;
	return result;
}

/**
*
*  rct2: 0x006BABB4
*/
void pause_sounds() {
	if (++RCT2_GLOBAL(0x009AF59C, uint8) == 1) {
		RCT2_CALLPROC_EBPSAFE(0x006BCAE5);
		RCT2_CALLPROC_EBPSAFE(0x006BABDF);
		RCT2_CALLPROC_EBPSAFE(0x006BCA9F);
		RCT2_CALLPROC_EBPSAFE(0x006BD07F);
	}
	g_sounds_disabled = 1;
}

/**
*
*  rct2: 0x006BABD8
*/
void unpause_sounds() {
	RCT2_GLOBAL(0x009AF59C, uint8)--;
	g_sounds_disabled = 0;
}