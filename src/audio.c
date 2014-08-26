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
#include "config.h"
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
*  rct2: 0x006BAB21
*/
void sub_6BAB21(void)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) != -1) {
		stop_other_sounds();
		stop_peep_sounds();
		RCT2_CALLPROC_EBPSAFE(0x006BD0BD);
		if (RCT2_GLOBAL(0x009AF284, uint32) & (1 << 0)) {
			stop_ride_music();
			RCT2_GLOBAL(0x014241BC, uint32) = 1;
			RCT2_CALLPROC(0x004018F0); // remove multimedia timer
			RCT2_GLOBAL(0x014241BC, uint32) = 0;
		}
		RCT2_GLOBAL(0x014241BC, uint32) = 1;
		RCT2_CALLPROC(0x00404C45);
		RCT2_CALLPROC(0x00404BD2);
		RCT2_GLOBAL(0x014241BC, uint32) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) = -1;
	}
}

/**
*
*  rct2: 0x006BA9B5
*/
void audio_init2(int device)
{
	sub_6BAB21();
	for (int i = 0; i < 7; i++) {
		rct_vehicle_sound* vehicle_sound = &RCT2_ADDRESS(RCT2_ADDRESS_VEHICLE_SOUND_LIST, rct_vehicle_sound)[i];
		vehicle_sound->id = 0xFFFF;
	}
	for (int i = 0; i < 7; i++) {
		rct_other_sound* other_sound = &RCT2_ADDRESS(0x009AF484, rct_other_sound)[i];
		other_sound->id = 0xFFFF;
	}
	RCT2_GLOBAL(0x014241BC, uint32) = 1;
	int v5 = RCT2_CALLFUNC_5(0x00404932, int, int, int, int, int, int, 0, device, 2, 22050, 16);
	RCT2_GLOBAL(0x014241BC, uint32) = 0;
	if (!v5) {
		return;
	}
	const char * filepath = get_file_path(2);
	RCT2_GLOBAL(0x014241BC, uint32) = 1;
	int v8 = RCT2_CALLFUNC_1(0x00404C1A, int, const char *, filepath);
	RCT2_GLOBAL(0x014241BC, uint32) = 0;
	if (!v8) {
		RCT2_GLOBAL(0x014241BC, uint32) = 1;
		RCT2_CALLPROC(0x00404BD2);
		RCT2_GLOBAL(0x014241BC, uint32) = 0;
		return;
	}
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) = device;
	int* data = RCT2_ADDRESS(RCT2_ADDRESS_DSOUND_DEVICES + (0x210 * device), int);
	RCT2_GLOBAL(0x009AAC5D, uint32) = data[0];
	RCT2_GLOBAL(0x009AAC61, uint32) = data[1];
	RCT2_GLOBAL(0x009AAC65, uint32) = data[2];
	RCT2_GLOBAL(0x009AAC69, uint32) = data[3];
	RCT2_GLOBAL(0x009AAC5C, uint8) = 1;
	config_save();
	RCT2_GLOBAL(0x014241BC, uint32) = 1;
	int result = RCT2_CALLFUNC(0x004018A6, int); // create multimedia timer
	RCT2_GLOBAL(0x014241BC, uint32) = 0;
	if (result) {
		if ((RCT2_GLOBAL(0x009AF284, uint32) & (1 << 0))) {
			for (int i = 0; i < 2; i++) {
				RCT2_GLOBAL(0x009AF46C + (i * 8), uint8) = -1;
			}
		}
	}
	if (!RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & 1 << 4) {
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_SOUND_SW_BUFFER, uint32) = RCT2_GLOBAL(0x001425B74, uint32) != RCT2_GLOBAL(0x001425B78, uint32) || RCT2_GLOBAL(0x001425B74, uint32) != RCT2_GLOBAL(0x001425B7C, uint32);
		RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) |= 1 << 4;
		config_save();
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

/**
*
*  rct2: 0x00404D99
*/
int sound_duplicate(rct_sound* newsound, rct_sound* sound)
{
	if (FAILED(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->DuplicateSoundBuffer(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND), sound->dsbuffer, &newsound->dsbuffer))) {
		return 0;
	} else {
		newsound->id = sound->id;
		newsound->has_caps = sound->has_caps;
		newsound->var_0C = sound->var_0C;
		sound_add(newsound);
		return 1;
	}
}

/**
*
*  rct2: 0x00405103
*/
rct_sound* sound_begin()
{
	return RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_BEGIN, rct_sound*);
}

/**
*
*  rct2: 0x00405109
*/
rct_sound* sound_next(rct_sound* sound)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_END, rct_sound*) == sound) {
		return 0;
	} else {
		return sound->next;
	}
}

/**
*
*  rct2: 0x00405206
*/
rct_sound_info* sound_get_info(uint16 sound_id)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_OFFSET, int) && sound_id < RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_OFFSET, int)) {
		return (rct_sound_info*)(RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_OFFSET, int) + RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_OFFSET, int*)[sound_id + 1]);
	}
	return 0;
}

/**
*
*  rct2: 0x00405054
*/
int sound_info_loadvars(rct_sound_info* sound_info, LPWAVEFORMATEX* waveformat, char** data, DWORD* buffersize)
{
	*buffersize = sound_info->size;
	*waveformat = &sound_info->format;
	*data = (char*)&sound_info->data;
	return 1;
}

/**
*
*  rct2: 0x00405076
*/
int sound_fill_buffer(LPDIRECTSOUNDBUFFER dsbuffer, char* src, int size)
{
	char* buf;
	char* buf2;
	int buf2size;
	if (SUCCEEDED(dsbuffer->lpVtbl->Lock(dsbuffer, 0, size, &buf, &size, &buf2, &buf2size, 0))) {
		memset(buf, 0, size);
		memcpy(buf, src, size);
		if (buf2size) {
			memcpy(buf2, &src[size], buf2size);
		}
		dsbuffer->lpVtbl->Unlock(dsbuffer, buf, size, buf2, buf2size);
		return 1;
	}
	return 0;
}

/**
*
*  rct2: 0x00404C6D
*/
int sound_prepare(int sound_id, rct_sound *sound, int channels, int software)
{
	//return RCT2_CALLFUNC_4(0x00404C6D, int, int, rct_sound*, int, int, sound_id, sound, channels, software);
	DSBUFFERDESC bufferdesc;
	char* buffer = 0;
	memset(&bufferdesc, 0, sizeof(bufferdesc));
	bufferdesc.dwSize = sizeof(bufferdesc);
	rct_sound* tempsound = sound_begin();
	if (tempsound) {
		int wasduplicated = 0;
		while (!tempsound->dsbuffer || tempsound->id != sound_id || !sound_duplicate(sound, tempsound)) {
			tempsound = sound_next(tempsound);
			if (!tempsound) {
				wasduplicated = 1;
				break;
			}
		}
		if (!wasduplicated) {
			return 1;
		}
	}
	rct_sound_info* sound_info = sound_get_info(sound_id);
	if (sound_info) {
		if (sound_info_loadvars(sound_info, &bufferdesc.lpwfxFormat, &buffer, &bufferdesc.dwBufferBytes)) {
			bufferdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STATIC;
			if (channels) {
				if (channels == 2) {
					bufferdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC;
				} else {
					bufferdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRL3D | DSBCAPS_STATIC;
				}
				if (RCT2_GLOBAL(0x009E2B90, uint32)) {
					bufferdesc.dwFlags |= DSBCAPS_CTRLPAN;
				}
				if (software) {
					bufferdesc.dwFlags |= DSBCAPS_LOCSOFTWARE;
				}
				if (SUCCEEDED(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->CreateSoundBuffer(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND), &bufferdesc, &sound->dsbuffer, 0))) {
					if (sound_fill_buffer(sound->dsbuffer, buffer, bufferdesc.dwBufferBytes)) {
						sound->id = sound_id;
						DSBCAPS caps;
						caps.dwSize = sizeof(caps);
						sound->dsbuffer->lpVtbl->GetCaps(sound->dsbuffer, &caps);
						sound->has_caps = caps.dwFlags;
						sound_add(sound);
						return 1;
					}
					sound->dsbuffer->lpVtbl->Release(sound->dsbuffer);
					sound->dsbuffer = 0;
				}
			}
			sound->dsbuffer = 0;
		}
	}
	return 0;
}

/**
*
*  rct2: 0x006BB76E
*/
int sound_play_panned(int sound_id, int x)
{
	//RCT2_CALLPROC_X(0x006BB76E, sound_id, x, 0, 0, 0, 0, 0);
	// this function is not complete, need to add in volume adjust
	int result = 0;
	if (RCT2_GLOBAL(0x009AF59D, uint8) & 1) {
		RCT2_GLOBAL(0x00F438AD, uint8) = 0;
		int volume = 0;
		if (x == 0x8001) {
			// stuff to adjust volume
		}
		int i = 0;
		rct_other_sound* other_sound = &RCT2_ADDRESS(0x009AF484, rct_other_sound)[i];
		while (other_sound->id != 0xFFFF) {
			i++;
			other_sound = &RCT2_ADDRESS(0x009AF484, rct_other_sound)[i];
			if (i > RCT2_GLOBAL(0x009AAC76, uint8)) { // too many sounds playing
				return sound_id;
			}
		}
		other_sound->id = sound_id;
		int pan;
		if (x == 0x8000) {
			pan = 0;
		} else {
			int x2 = x << 16;
			uint16 screenwidth = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
			if (screenwidth < 64) {
				screenwidth = 64;
			}
			pan = ((x2 / screenwidth) - 0x8000) >> 4;
		}
		if (!RCT2_GLOBAL(0x009AAC6D, uint8)) {
			pan = 0;
		}

		RCT2_GLOBAL(0x014241BC, uint32) = 1;
		sound_prepare(sound_id, &other_sound->sound, 1, RCT2_GLOBAL(0x009AAC6E, uint32));
		RCT2_GLOBAL(0x014241BC, uint32) = 0;
		RCT2_GLOBAL(0x014241BC, uint32) = 1;
		result = sound_play(&other_sound->sound, 0, volume, pan, 0);
		RCT2_GLOBAL(0x014241BC, uint32) = 0;
	}
	return result;
}

/**
*
*  rct2: 0x00401999
*/
int sound_channel_play(int channel, int a2, int volume, int pan, int frequency)
{
	rct_sound_channel* sound_channel = &RCT2_ADDRESS(RCT2_ADDRESS_SOUND_CHANNEL_LIST, rct_sound_channel)[channel];
	sound_channel->var_164 = a2;
	sound_channel_set_frequency(channel, frequency);
	sound_channel_set_pan(channel, pan);
	sound_channel_set_volume(channel, volume);
	LPDIRECTSOUNDBUFFER dsbuffer = RCT2_ADDRESS(RCT2_ADDRESS_DSOUND_BUFFERS, LPDIRECTSOUNDBUFFER)[channel];
	dsbuffer->lpVtbl->SetCurrentPosition(dsbuffer, 0);
	dsbuffer->lpVtbl->Play(dsbuffer, 0, 0, DSBPLAY_LOOPING);
	sound_channel->var_0 = 1;
	return 1;
}

/**
*
*  rct2: 0x00401A93
*/
int sound_channel_set_frequency(int channel, int frequency)
{
	LPDIRECTSOUNDBUFFER dsbuffer = RCT2_ADDRESS(RCT2_ADDRESS_DSOUND_BUFFERS, LPDIRECTSOUNDBUFFER)[channel];
	if (dsbuffer) {
		if (SUCCEEDED(dsbuffer->lpVtbl->SetFrequency(dsbuffer, frequency)))
			return 1;

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
	if (dsbuffer) {
		if (SUCCEEDED(dsbuffer->lpVtbl->SetPan(dsbuffer, pan)))
			return 1;

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
	if (dsbuffer) {
		if (SUCCEEDED(dsbuffer->lpVtbl->SetVolume(dsbuffer, volume)))
			return 1;

	}
	return 0;
}

/**
*
*  rct2: 0x00404E7F
*/
int sound_play(rct_sound* sound, int looping, int volume, int pan, int frequency)
{
	if (sound) {
		sound_set_frequency(sound, frequency);
		sound_set_pan(sound, pan);
		sound_set_volume(sound, volume);
		DWORD playflags;
		if (looping) {
			if (looping != 1)
				return 1;

			playflags = DSBPLAY_LOOPING;
		} else {
			playflags = 0;
		}
		if (SUCCEEDED(sound->dsbuffer->lpVtbl->Play(sound->dsbuffer, 0, 0, playflags))) 
			return 1;

	}
	return 0;
}

/**
*
*  rct2: 0x00404E53
*/
int sound_is_playing(rct_sound* sound){
	if (sound) {
		DWORD status;
		if (SUCCEEDED(sound->dsbuffer->lpVtbl->GetStatus(sound->dsbuffer, &status))) {
			if (status & DSBSTATUS_PLAYING || status & DSBSTATUS_LOOPING)
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
	if (sound) {
		if (SUCCEEDED(sound->dsbuffer->lpVtbl->SetFrequency(sound->dsbuffer, frequency)))
			return 1;

	}
	return 0;
}

/**
*
*  rct2: 0x00404EF2
*/
int sound_set_pan(rct_sound* sound, int pan)
{
	if (sound) {
		if (SUCCEEDED(sound->dsbuffer->lpVtbl->SetPan(sound->dsbuffer, pan)))
			return 1;

	}
	return 0;
}

/**
*
*  rct2: 0x00404F0D
*/
int sound_set_volume(rct_sound* sound, int volume)
{
	if (sound) {
		if (SUCCEEDED(sound->dsbuffer->lpVtbl->SetVolume(sound->dsbuffer, volume)))
			return 1;

	}
	return 0;
}

/**
*
*  rct2: 0x00404DD8
*/
rct_sound* sound_stop(rct_sound* sound)
{
	if (sound->dsbuffer) {
		sound->dsbuffer->lpVtbl->Release(sound->dsbuffer);
		sound->dsbuffer = 0;
		return sound_remove(sound);
	}
	return sound;
}

/**
*
*  rct2: 0x0040511C
*/
rct_sound* sound_add(rct_sound* sound)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_BEGIN, rct_sound*)) {
		RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_END, rct_sound*)->next = sound;
	} else {
		RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_BEGIN, rct_sound*) = sound;
	}
	RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_END, rct_sound*) = sound;
	sound->next = 0;
	return sound;
}

/**
*
*  rct2: 0x00405143
*/
rct_sound* sound_remove(rct_sound* sound)
{
	rct_sound* result = RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_BEGIN, rct_sound*);
	if (sound == result) {
		if (sound == RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_END, rct_sound*)) {
			RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_END, rct_sound*) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_BEGIN, rct_sound*) = 0;
		}
		result = sound->next;
		RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_BEGIN, rct_sound*) = result;
	} else {
		while (result->next != sound)
			result = result->next;

		if (sound == RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_END, rct_sound*)) {
			RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_END, rct_sound*) = result;
			result->next = 0;
		} else
			result->next = sound->next;

	}
	sound->next = 0;
	return result;
}

/**
*
*  rct2: 0x00405436
*/
void sound_channel_free(HMMIO* hmmio, HGLOBAL* hmem)
{
	if(*hmem)
	{
		GlobalFree(*hmem);
		*hmem = 0;
	}
	if(*hmmio)
	{
		mmioClose(*hmmio, 0);
		*hmmio = 0;
	}
}

/**
*
*  rct2: 0x006BABB4
*/
void pause_sounds()
{
	if (++RCT2_GLOBAL(0x009AF59C, uint8) == 1) {
		stop_other_sounds();
		stop_vehicle_sounds();
		stop_ride_music();
		stop_peep_sounds();
	}
	g_sounds_disabled = 1;
}

/**
*
*  rct2: 0x006BCAE5
*/
void stop_other_sounds()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) != -1) {
		if (RCT2_GLOBAL(0x009AF5A8, uint32) != 1) {
			RCT2_GLOBAL(0x014241BC, uint32) = 1;
			sound_stop(RCT2_GLOBAL(0x009AF5AC, rct_sound*));
			RCT2_GLOBAL(0x014241BC, uint32) = 0;
			RCT2_GLOBAL(0x009AF5A8, uint32) = 1;
		}
		if (RCT2_GLOBAL(0x009AF5C0, uint32) != 8) {
			RCT2_GLOBAL(0x014241BC, uint32) = 1;
			sound_stop(RCT2_GLOBAL(0x009AF5C4, rct_sound*));
			RCT2_GLOBAL(0x014241BC, uint32) = 0;
			RCT2_GLOBAL(0x009AF5C0, uint32) = 8;
		}
		if (RCT2_GLOBAL(0x009AF5D8, uint32) != 8) {
			RCT2_GLOBAL(0x014241BC, uint32) = 1;
			sound_stop(RCT2_GLOBAL(0x009AF5DC, rct_sound*));
			RCT2_GLOBAL(0x014241BC, uint32) = 0;
			RCT2_GLOBAL(0x009AF5D8, uint32) = 8;
		}
	}
}

/**
*
*  rct2: 0x006BABDF
*/
void stop_vehicle_sounds()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, sint32) != -1) {
		for (int i = 0; i < 7; i++) {
			rct_vehicle_sound* vehicle_sound = &RCT2_ADDRESS(RCT2_ADDRESS_VEHICLE_SOUND_LIST, rct_vehicle_sound)[i];
			if (vehicle_sound->id != 0xFFFF) {
				if (vehicle_sound->var_18 != 0xFFFF) {
					RCT2_GLOBAL(0x014241BC, uint32) = 1;
					sound_stop(&vehicle_sound->sound1);
					RCT2_GLOBAL(0x014241BC, uint32) = 0;
				}
				if (vehicle_sound->var_34 != 0xFFFF) {
					RCT2_GLOBAL(0x014241BC, uint32) = 1;
					sound_stop(&vehicle_sound->sound2);
					RCT2_GLOBAL(0x014241BC, uint32) = 0;
				}
			}
			vehicle_sound->id = 0xFFFF;
		}
	}
}

/**
*
*  rct2: 0x006BCA9F
*/
void stop_ride_music()
{
	if ((RCT2_GLOBAL(0x009AF284, uint32) & (1 << 0))) {
		for (int i = 0; i < 2; i++) {
			uint8 * data = RCT2_ADDRESS(0x009AF46C + (i * 8), uint8);
			if (data[0] != 0xFF) {
				RCT2_GLOBAL(0x014241BC, uint32) = 1;
				sound_channel_stop(i);
				RCT2_GLOBAL(0x014241BC, uint32) = 0;
				data[0] = 0xFF;
			}
		}
	}
}

/**
*
*  rct2: 0x006BD07F
*/
void stop_peep_sounds()
{
	if ((RCT2_GLOBAL(0x009AF284, uint32) & (1 << 0))) {
		if (RCT2_GLOBAL(0x009AF5FC, uint32) != 1) {
			RCT2_GLOBAL(0x014241BC, uint32) = 1;
			sound_channel_stop(2);
			RCT2_GLOBAL(0x014241BC, uint32) = 0;
			RCT2_GLOBAL(0x009AF5FC, uint32) = 1;
		}
	}
}

/**
*
*  rct2: 0x00401A05
*/
int sound_channel_stop(int channel)
{
	rct_sound_channel* sound_channel = &RCT2_ADDRESS(RCT2_ADDRESS_SOUND_CHANNEL_LIST, rct_sound_channel)[channel];
	sound_channel->var_0 = 0;
	sound_channel->var_160 = 1;
	while (_InterlockedExchange(&RCT2_GLOBAL(0x009E1AAC, LONG), 1) != 1) {
		Sleep(10);
	}
	if (sound_channel->hmem) {
		sound_channel_free(&sound_channel->hmmio, &sound_channel->hmem);
	}

	LPDIRECTSOUNDBUFFER dsbuffer = RCT2_ADDRESS(RCT2_ADDRESS_DSOUND_BUFFERS, LPDIRECTSOUNDBUFFER)[channel];
	if (dsbuffer) {
		dsbuffer->lpVtbl->Stop(dsbuffer);
		dsbuffer->lpVtbl->Release(dsbuffer);
		RCT2_ADDRESS(RCT2_ADDRESS_DSOUND_BUFFERS, LPDIRECTSOUNDBUFFER)[channel] = 0;
	}
	_InterlockedExchange(&RCT2_GLOBAL(0x009E1AAC, LONG), 0);
	return 1;
}

/**
*
*  rct2: 0x006BABD8
*/
void unpause_sounds()
{
	RCT2_GLOBAL(0x009AF59C, uint8)--;
	g_sounds_disabled = 0;
}