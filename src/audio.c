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
*  rct2: 0x00404BD2
*/
int audio_release()
{
	sound_stop_all();
	if (RCT2_GLOBAL(0x009E2BA4, LPDIRECTSOUND3DBUFFER)) {
		RCT2_GLOBAL(0x009E2BA4, LPDIRECTSOUND3DBUFFER)->lpVtbl->Release(RCT2_GLOBAL(0x009E2BA4, LPDIRECTSOUND3DBUFFER));
		RCT2_GLOBAL(0x009E2BA4, LPDIRECTSOUND3DBUFFER) = 0;
	}
	if (RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER)) {
		RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER)->lpVtbl->Release(RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER));
		RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER) = 0;
	}
	int result = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)) {
		result = RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->Release(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND));
		RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND) = 0;
	}
	return result;
}

/**
*
*  rct2: 0x00404C45
*/
int unmap_sound_info()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_MAPPING, LPVOID)) {
		sound_stop_all();
		unmap_file(RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_MAPPING, LPVOID));
		RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_MAPPING, LPVOID) = 0;
		return 1;
	}
	return 0;
}

/**
*
*  rct2: 0x00404DF5
*/
int sound_stop_all()
{
	int result = 0;
	while (RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_BEGIN, rct_sound*)) {
		result = sound_stop(RCT2_GLOBAL(RCT2_ADDRESS_SOUNDLIST_BEGIN, rct_sound*));
	}
	return result;
}

/**
*
*  rct2: 0x004068A0
*/
int unmap_file(LPCVOID base)
{
	if (base) {
		return UnmapViewOfFile(base);
	}
	return 0;
}

/**
*
*  rct2: 0x004018F0
*/
int audio_remove_timer()
{
	for (int i = 0; i < 4; i++) {
		sound_channel_stop(i);
	}
	if (RCT2_GLOBAL(0x009E1AA4, int)) {
		timeKillEvent(RCT2_GLOBAL(0x009E1AA0, MMRESULT));
		timeEndPeriod(50);
		while (_InterlockedExchange(&RCT2_GLOBAL(0x009E1AAC, LONG), 1) != 1) {
			Sleep(100);
		}
		int result = _InterlockedExchange(&RCT2_GLOBAL(0x009E1AAC, LONG), 0);
		RCT2_GLOBAL(0x009E1AA4, int) = 0;
		return result;
	}
	return 0;
}

/**
*
*  rct2: 0x006BAB21
*/
void audio_close()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) != -1) {
		stop_other_sounds();
		stop_peep_sounds();
		stop_title_music();
		if (RCT2_GLOBAL(0x009AF284, uint32) & (1 << 0)) {
			stop_ride_music();
			RCT2_GLOBAL(0x014241BC, uint32) = 1;
			audio_remove_timer();
			RCT2_GLOBAL(0x014241BC, uint32) = 0;
		}
		RCT2_GLOBAL(0x014241BC, uint32) = 1;
		unmap_sound_info();
		audio_release();
		RCT2_GLOBAL(0x014241BC, uint32) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) = -1;
	}
}

/**
*
*  rct2: 0x000404F28
*/
int sound_load3dparameters()
{
	if (SUCCEEDED(RCT2_GLOBAL(0x009E2BA4, LPDIRECTSOUND3DBUFFER)->lpVtbl->GetAllParameters(RCT2_GLOBAL(0x009E2BA4, LPDIRECTSOUND3DBUFFER), &RCT2_GLOBAL(0x009A6084, DS3DBUFFER)))) {
		return 1;
	}
	return 0;
}

/**
*
*  rct2: 0x00404932
*/
int dsound_create_primary_buffer(int a, int device, int channels, int samples, int bits)
{
	rct_dsdevice* dsdevice = 0;
	if (device) {
		if (device > RCT2_GLOBAL(RCT2_ADDRESS_NUM_DSOUND_DEVICES, int)) {
			return 0;
		}
		dsdevice = &RCT2_GLOBAL(RCT2_ADDRESS_DSOUND_DEVICES, rct_dsdevice*)[device];
	}
	memset(&RCT2_GLOBAL(RCT2_ADDRESS_AUDIO_INFO, rct_audio_info), 0, sizeof(rct_audio_info));
	RCT2_GLOBAL(RCT2_ADDRESS_AUDIO_INFO, rct_audio_info).var_0 = 1;
	RCT2_GLOBAL(RCT2_ADDRESS_AUDIO_INFO, rct_audio_info).channels = channels;
	RCT2_GLOBAL(RCT2_ADDRESS_AUDIO_INFO, rct_audio_info).samples = samples;
	RCT2_GLOBAL(RCT2_ADDRESS_AUDIO_INFO, rct_audio_info).var_8 = samples * RCT2_GLOBAL(0x01425B4C, uint16);
	RCT2_GLOBAL(RCT2_ADDRESS_AUDIO_INFO, rct_audio_info).bytes = bits * channels / 8;
	RCT2_GLOBAL(RCT2_ADDRESS_AUDIO_INFO, rct_audio_info).bits = bits;
	RCT2_GLOBAL(RCT2_ADDRESS_AUDIO_INFO, rct_audio_info).var_E = 0;
	DSBUFFERDESC bufferdesc;
	memset(&bufferdesc, 0, sizeof(bufferdesc));
	bufferdesc.dwSize = sizeof(bufferdesc);
	if (a) {
		if (a != 1) {
			return 0;
		}
		bufferdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
		if (RCT2_GLOBAL(0x009E2B90, uint32)) {
			bufferdesc.dwFlags = DSBCAPS_STICKYFOCUS | DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
		}
		if (FAILED(DirectSoundCreate(&dsdevice->guid, &RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND), 0))) {
			return 0;
		}
		if (FAILED(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->SetCooperativeLevel(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND), RCT2_GLOBAL(0x009E2D70, HWND), DSSCL_NORMAL)) || 
			FAILED(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->CreateSoundBuffer(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND), &bufferdesc, &RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER), 0))) {
			RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->Release(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND));
			RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND) = 0;
			return 0;
		}
		if (SUCCEEDED(RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER)->lpVtbl->QueryInterface(RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER), &RCT2_GLOBAL(0x009A4444, IID) /* IID_IDirectSound3DBuffer */, (void**)&RCT2_GLOBAL(0x009E2BA4, LPDIRECTSOUND3DBUFFER)))) {
			// doesn't seem to ever make it here, below doesn't make sense and is probably remnants of unused 3d sound tests
			memset(&RCT2_GLOBAL(0x009A6084, DS3DBUFFER), 0, sizeof(RCT2_GLOBAL(0x009A6084, DS3DBUFFER)));
			RCT2_GLOBAL(0x009A6084, DS3DBUFFER).dwSize = sizeof(RCT2_GLOBAL(0x009A6084, DS3DBUFFER));

			if (sound_load3dparameters()) {
				RCT2_GLOBAL(0x009A6084, DS3DBUFFER).vPosition.x = 0.0f;
			}
			RCT2_GLOBAL(0x009A6084, DS3DBUFFER).vPosition.y = 0.0f;
			RCT2_GLOBAL(0x009A6084, DS3DBUFFER).vPosition.x = -1.0f;
			RCT2_GLOBAL(0x009A6084, DS3DBUFFER).flMaxDistance = 9.8999996f;
			RCT2_GLOBAL(0x009A6084, DS3DBUFFER).flMinDistance = 0.25f;
			if (RCT2_CALLFUNC(0x00404F3F, int)) {
				if (SUCCEEDED(RCT2_GLOBAL(0x009E2BA4, LPDIRECTSOUND3DBUFFER)->lpVtbl->SetMinDistance(RCT2_GLOBAL(0x009E2BA4, LPDIRECTSOUND3DBUFFER), RCT2_GLOBAL(0x009A6084, DS3DBUFFER).flMinDistance, 1))) {
					if (sound_load3dparameters()) {
						return 1;
					}
				}
			}
			RCT2_GLOBAL(0x009E2BA4, LPDIRECTSOUND3DBUFFER)->lpVtbl->Release(RCT2_GLOBAL(0x009E2BA4, LPDIRECTSOUND3DBUFFER));
			RCT2_GLOBAL(0x009E2BA4, LPDIRECTSOUND3DBUFFER) = 0;
			RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER)->lpVtbl->Release(RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER));
			RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->Release(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND));
			RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND) = 0;
		} else {
			RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER)->lpVtbl->Release(RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER));
			RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->Release(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND));
			RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND) = 0;
		}
	}
	bufferdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	if (RCT2_GLOBAL(0x009E2B90, uint32)) {
		bufferdesc.dwFlags = DSBCAPS_STICKYFOCUS | DSBCAPS_PRIMARYBUFFER;
	}
	if (FAILED(DirectSoundCreate(&dsdevice->guid, &RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND), 0))) {
		return 0;
	}
	if (FAILED(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->SetCooperativeLevel(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND), RCT2_GLOBAL(0x009E2D70, HWND), DSSCL_PRIORITY))) {
		RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->Release(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND));
		RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND) = 0;
		return 0;
	}
	RCT2_GLOBAL(0x01425B60, DSCAPS).dwSize = sizeof(DSCAPS);
	RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->GetCaps(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND), &RCT2_GLOBAL(0x01425B60, DSCAPS));
	if (FAILED(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->CreateSoundBuffer(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND), &bufferdesc, &RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER), 0))) {
		RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER)->lpVtbl->Release(RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER));
		RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND)->lpVtbl->Release(RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND));
		RCT2_GLOBAL(RCT2_ADDRESS_DIRECTSOUND, LPDIRECTSOUND) = 0;
	}
	WAVEFORMATEX waveformat1, waveformat2;
	RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER)->lpVtbl->GetFormat(RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER), &waveformat1, sizeof(WAVEFORMATEX), 0);
	RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER)->lpVtbl->SetFormat(RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER), &RCT2_GLOBAL(0x01425B40, WAVEFORMATEX));
	RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER)->lpVtbl->GetFormat(RCT2_GLOBAL(0x009E2BA8, LPDIRECTSOUNDBUFFER), &waveformat2, sizeof(WAVEFORMATEX), 0);
	return 1;
}

/**
*
*  rct2: 0x004067F9
*/
LPVOID map_file(LPCSTR lpFileName, DWORD dwCreationDisposition, DWORD dwNumberOfBytesToMap)
{
	DWORD dwDesiredAccess;
	DWORD dwDesiredAccessmap;
	DWORD flProtect;
	HANDLE filehandle;
	HANDLE filemaphandle;
	LPVOID address = 0;
	if (dwCreationDisposition) {
		if (dwCreationDisposition == CREATE_NEW) {
			dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
			dwDesiredAccessmap = FILE_MAP_WRITE;
			flProtect = PAGE_READWRITE;
			dwCreationDisposition = OPEN_ALWAYS;
		}
	} else {
		dwDesiredAccess = GENERIC_READ;
		flProtect = PAGE_READONLY;
		dwDesiredAccessmap = FILE_MAP_READ;
		dwCreationDisposition = OPEN_EXISTING;
	}
	filehandle = CreateFileA(lpFileName, dwDesiredAccess, 0, 0, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
	if (filehandle != INVALID_HANDLE_VALUE) {
		filemaphandle = CreateFileMappingA(filehandle, 0, flProtect, 0, dwNumberOfBytesToMap, 0);
		CloseHandle(filehandle);
		if (filemaphandle) {
			address = MapViewOfFile(filemaphandle, dwDesiredAccessmap, 0, 0, dwNumberOfBytesToMap);
			CloseHandle(filemaphandle);
		}
	}
	return address;
}

/**
*
*  rct2: 0x00404C1A
*/
int map_sound_info(const char* filename)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_MAPPING, LPVOID)) {
		return 0;
	} else {
		RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_MAPPING, LPVOID) = map_file(filename, 0, 0);
		return RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_MAPPING, LPVOID) != 0;
	}
}

/**
*
*  rct2: 0x00405383
*/
MMRESULT sub_405383(HMMIO hmmio, uint32 size, char* buffer, uint32* a4, int* read)
{
	MMIOINFO mmioinfo;
	MMRESULT result;
	result = mmioGetInfo(hmmio, &mmioinfo, 0);
	if (result != 0) {
		*read = 0;
		return 1;
	}
	int size2 = size;
	if (size > a4[1]) {
		size2 = a4[1];
	}
	int v8 = 0;
	a4[1] -= size2;
	if (size2) {
		while (1) {
			HPSTR p = mmioinfo.pchEndRead;
			if (mmioinfo.pchNext == mmioinfo.pchEndRead) {
				result = mmioAdvance(hmmio, &mmioinfo, 0);
				if (result != 0) {
					*read = 0;
					return result;
				}
				p = mmioinfo.pchEndRead;
				if (mmioinfo.pchNext == mmioinfo.pchEndRead) {
					break;
				}
			}
			int q = p - mmioinfo.pchNext;
			if (size2 - v8 < p - mmioinfo.pchNext) {
				q = size2 - v8;
			}
			memcpy(&buffer[v8], mmioinfo.pchNext, q);
			mmioinfo.pchNext += q;
			v8 += q;
			if (v8 >= size2) {
				result = mmioSetInfo(hmmio, &mmioinfo, 0);
				if (result != 0) {
					*read = 0;
					return result;
				} else {
					*read = size2;
					return result;
				}
			}
		}
		*read = 0;
		return 57603;
	}
	result = mmioSetInfo(hmmio, &mmioinfo, 0);
	if (result != 0) {
		*read = 0;
		return result;
	} else {
		*read = size2;
		return result;
	}
}

/**
*
*  rct2: 0x00401000
*/
void sub_401000(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2, int channel)
{
	rct_sound_channel* soundchannel = &RCT2_ADDRESS(0x014262E0, rct_sound_channel)[channel];
	DWORD status;
	DWORD dwCurrentPlayCursor;
	DWORD dwCurrentWriteCursor;
	int var1;
	int var2;
	int bufferlost = 0;
	char* buf1;
	int buf1size;
	char* buf2;
	int buf2size;
	soundchannel->dsbuffer->lpVtbl->GetStatus(soundchannel->dsbuffer, &status);
	if (status & DSBSTATUS_BUFFERLOST) {
		if (FAILED(soundchannel->dsbuffer->lpVtbl->Restore(soundchannel->dsbuffer))) {
			return;
		}
		soundchannel->playpos = 0;
		bufferlost = 1;
	}
	soundchannel->dsbuffer->lpVtbl->GetCurrentPosition(soundchannel->dsbuffer, &dwCurrentPlayCursor, &dwCurrentWriteCursor);
	if (dwCurrentPlayCursor != soundchannel->playpos || bufferlost) {
		if (soundchannel->var_168 && !soundchannel->var_15C) {
			if (!soundchannel->var_160) {
				soundchannel->var_160 = 1;
				if (!soundchannel->var_4) {
					LPDIRECTSOUNDBUFFER dsbuffer = RCT2_ADDRESS(RCT2_ADDRESS_DSOUND_BUFFERS, LPDIRECTSOUNDBUFFER)[channel];
					soundchannel->var_0 = 0;
					if (dsbuffer) {
						dsbuffer->lpVtbl->Stop(dsbuffer);
						dsbuffer->lpVtbl->Release(dsbuffer);
						RCT2_ADDRESS(RCT2_ADDRESS_DSOUND_BUFFERS, LPDIRECTSOUNDBUFFER)[channel] = 0;
					}
					if (soundchannel->hmmio) {
						sound_channel_free(&soundchannel->hmmio, &soundchannel->hmem);
					}
				}
			}
			return;
		}
		if (dwCurrentPlayCursor >= soundchannel->playpos) {
			var1 = dwCurrentPlayCursor - soundchannel->playpos;
		} else {
			var1 = dwCurrentPlayCursor + soundchannel->var_150 - soundchannel->playpos;
		}
		if (bufferlost) {
			var2 = 2 * soundchannel->var_150 / 6;
		} else {
			var2 = var1;
		}
		soundchannel->var_158 += var1;
		if (soundchannel->var_168) {
			int var3 = soundchannel->var_15C;
			int* var4 = &soundchannel->var_15C;
			if (var3) {
				if (var1 <= var3) {
					*var4 = var3 - var1;
				} else {
					*var4 = 0;
				}
				if (SUCCEEDED(soundchannel->dsbuffer->lpVtbl->Lock(soundchannel->dsbuffer, soundchannel->playpos, var2, &buf1, &buf1size, &buf2, &buf2size, 0))) {
					uint16 var5 = -(soundchannel->var_128 != 8);
					var5 &= 0x80;
					memset(buf1, var5 + 128, buf1size);
					if (buf2 && buf2size) {
						uint16 var5 = -(soundchannel->var_128 != 8);
						var5 &= 0x80;
						memset(buf2, var5 + 128, buf2size);
					}
					soundchannel->dsbuffer->lpVtbl->Unlock(soundchannel->dsbuffer, buf1, buf1size, buf2, buf2size);
					soundchannel->playpos += var2;
					if( soundchannel->playpos >= soundchannel->var_150) {
						soundchannel->playpos = soundchannel->playpos - soundchannel->var_150;
					}
					return;
				}
				// TimeFunc() could not lock DirectSoundBuffer
				return;
			}
		}
		if (FAILED(soundchannel->dsbuffer->lpVtbl->Lock(soundchannel->dsbuffer, soundchannel->playpos, var2, &buf1, &buf1size, &buf2, &buf2size, 0))) {
			// TimeFunc() could not lock DirectSoundBuffer
			return;
		}
		if (buf1size) {
			if (soundchannel->var_160) {
				uint16 var5 = -(soundchannel->var_128 != 8);
				var5 &= 0x80;
				memset(buf1, var5 + 128, buf1size);
				goto label49;
			}
		}
		int var7;
		sub_405383(soundchannel->hmmio, buf1size, buf1, &soundchannel->var_124, &var7);
		if (var7 < buf1size) {
			if (!soundchannel->var_164) {
				int s = soundchannel->var_128;
				int t = buf1size - var7;
				int v;
				if (s == 8) {
					v = 128;
				} else {
					if (s != 16) {
						goto label42;
					}
					v = 0;
				}
				memset(&buf1[var7], v, t);
			label42:
				soundchannel->var_168 = 1;
				if (dwCurrentPlayCursor <= soundchannel->playpos) {
					soundchannel->var_15C = soundchannel->playpos - dwCurrentPlayCursor;
				} else {
					soundchannel->var_15C = soundchannel->playpos + soundchannel->var_150 - dwCurrentPlayCursor;
				}
				goto label49;
			}
			char* v21 = buf1;
			int v38 = buf1size;
			do {
				v38 -= var7;
				v21 += var7;
				RCT2_CALLFUNC_1(0x0040153B, int, int, channel);
				sub_405383(soundchannel->hmmio, v38, v21, &soundchannel->var_124, &var7);
			} while(var7 < v38);
		}
	label49:
		if (buf2size == 0 || soundchannel->var_160 != 0) {
			if(buf2 != 0 && buf2size != 0 && soundchannel->var_160 != 0) {
				int var5 = -(soundchannel->var_128 != 8);
				var5 &= 0x80;
				memset(buf2, var5 + 128, buf2size);
			}
			goto label68;
		}
		sub_405383(soundchannel->hmmio, buf2size, buf2, &soundchannel->var_124, &var7);
		if (var7 >= buf2size) {
		label68:
			soundchannel->dsbuffer->lpVtbl->Unlock(soundchannel->dsbuffer, buf1, buf1size, buf2, buf2size);
			soundchannel->playpos += var2;
			if (soundchannel->playpos >= soundchannel->var_150) {
				soundchannel->playpos -= soundchannel->var_150;
			}
			if (bufferlost != 0) {
				soundchannel->dsbuffer->lpVtbl->Play(soundchannel->dsbuffer, 0, 0, DSBPLAY_LOOPING);
			}
			return;
		}
		if (soundchannel->var_164 != 0) {
			char* v26 = buf2;
			int v27 = buf2size;
			do {
				v26 += var7;
				v27 -= var7;
				RCT2_CALLFUNC_1(0x0040153B, int, int, channel);
				sub_405383(soundchannel->hmmio, v27, v26, &soundchannel->var_124, &var7);
			} while(var7 < v27);
			goto label68;
		}
		int s = buf2size - var7;
		int v;
		if (soundchannel->hmem == (HGLOBAL)8) {
			v = 128;
		} else {
			if (soundchannel->hmem != (HGLOBAL)16) {
				goto label58;
			}
			v = 0;
		}
		memset(&buf2[var7], v, s);
	label58:
		soundchannel->var_168 = 1;
		if (dwCurrentPlayCursor <= soundchannel->playpos) {
			soundchannel->var_15C = soundchannel->playpos - dwCurrentPlayCursor;
		} else {
			soundchannel->var_15C = soundchannel->playpos + soundchannel->var_150 - dwCurrentPlayCursor;
		}
		goto label68;
	}
}

/**
*
*  rct2: 0x004014DF
*/
int CALLBACK audio_timer_callback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	if (_InterlockedExchange(&RCT2_GLOBAL(0x009E1AAC, LONG), 1) == 0) {
		for (int i = 0; i < 4; i++) {
			rct_sound_channel* sound_channel = &RCT2_ADDRESS(RCT2_ADDRESS_SOUND_CHANNEL_LIST, rct_sound_channel)[i];
			if(sound_channel->var_0){
				sub_401000(uTimerID, uMsg, dwUser, dw1, dw2, i);
			}
		}
		return _InterlockedExchange(&RCT2_GLOBAL(0x009E1AAC, LONG), 0);
	}
	return 0;
}

/**
*
*  rct2: 0x004018A6
*/
int audio_create_timer()
{
	if (RCT2_GLOBAL(0x009E1AA4, int)) {
		return 0;
	}
	for (int i = 0; i < 4; i++) {
		rct_sound_channel* sound_channel = &RCT2_ADDRESS(RCT2_ADDRESS_SOUND_CHANNEL_LIST, rct_sound_channel)[i];
		sound_channel->var_0 = 0;
	}
	RCT2_GLOBAL(0x009E1AA0, MMRESULT) = timeSetEvent(50, 10, (LPTIMECALLBACK)audio_timer_callback, 0, TIME_PERIODIC);
	if (!RCT2_GLOBAL(0x009E1AA0, MMRESULT)) {
		return 0;
	}
	RCT2_GLOBAL(0x009E1AA4, int) = 1;
	return 1;
}

/**
*
*  rct2: 0x006BA9B5
*/
void audio_init2(int device)
{
	audio_close();
	for (int i = 0; i < 7; i++) {
		rct_vehicle_sound* vehicle_sound = &RCT2_ADDRESS(RCT2_ADDRESS_VEHICLE_SOUND_LIST, rct_vehicle_sound)[i];
		vehicle_sound->id = 0xFFFF;
	}
	for (int i = 0; i < 7; i++) {
		rct_other_sound* other_sound = &RCT2_ADDRESS(0x009AF484, rct_other_sound)[i];
		other_sound->id = 0xFFFF;
	}
	RCT2_GLOBAL(0x014241BC, uint32) = 1;
	int successdsound = dsound_create_primary_buffer(0, device, 2, 22050, 16);
	RCT2_GLOBAL(0x014241BC, uint32) = 0;
	if (!successdsound) {
		return;
	}
	const char * filepath = get_file_path(2);
	RCT2_GLOBAL(0x014241BC, uint32) = 1;
	int successmap = map_sound_info(filepath);
	RCT2_GLOBAL(0x014241BC, uint32) = 0;
	if (!successmap) {
		RCT2_GLOBAL(0x014241BC, uint32) = 1;
		audio_release();
		RCT2_GLOBAL(0x014241BC, uint32) = 0;
		return;
	}
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_SOUND_DEVICE, uint32) = device;
	rct_dsdevice dsdevice = RCT2_GLOBAL(RCT2_ADDRESS_DSOUND_DEVICES, rct_dsdevice*)[device];
	RCT2_GLOBAL(0x009AAC5D, uint32) = dsdevice.guid.Data1;
	RCT2_GLOBAL(0x009AAC61, uint32) = dsdevice.guid.Data2;
	RCT2_GLOBAL(0x009AAC65, uint32) = dsdevice.guid.Data3;
	RCT2_GLOBAL(0x009AAC69, uint32) = (uint32)dsdevice.guid.Data4;
	RCT2_GLOBAL(0x009AAC5C, uint8) = 1;
	config_save();
	RCT2_GLOBAL(0x014241BC, uint32) = 1;
	int successtimer = audio_create_timer();
	RCT2_GLOBAL(0x014241BC, uint32) = 0;
	if (successtimer) {
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
*  rct2: 0x00404F85
*/
BOOL CALLBACK dsound_enum_callback_count(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
	RCT2_GLOBAL(RCT2_ADDRESS_DSOUND_DEVICES_COUNTER, int)++;
	return 1;
}

/**
*
*  rct2: 0x00404F91
*/
int dsound_count_devices()
{
	RCT2_GLOBAL(RCT2_ADDRESS_DSOUND_DEVICES_COUNTER, int) = 0;
	if (SUCCEEDED(DirectSoundEnumerate(dsound_enum_callback_count, 0))) {
		return RCT2_GLOBAL(RCT2_ADDRESS_DSOUND_DEVICES_COUNTER, int);
	}
	return 0;
}

/**
*
*  rct2: 0x00404FB1
*/
BOOL CALLBACK dsound_enum_callback(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
	if (lpGuid) {
		memcpy(&RCT2_GLOBAL(RCT2_ADDRESS_DSOUND_DEVICES, rct_dsdevice*)[RCT2_GLOBAL(0x01425B54, int)].guid, lpGuid, sizeof(GUID));
	} else {
		memset(&RCT2_GLOBAL(RCT2_ADDRESS_DSOUND_DEVICES, rct_dsdevice*)[RCT2_GLOBAL(0x01425B54, int)].guid, 0, sizeof(GUID));
	}
	strcpy(RCT2_GLOBAL(RCT2_ADDRESS_DSOUND_DEVICES, rct_dsdevice*)[RCT2_GLOBAL(0x01425B54, int)].desc, lpcstrDescription);
	strcpy(RCT2_GLOBAL(RCT2_ADDRESS_DSOUND_DEVICES, rct_dsdevice*)[RCT2_GLOBAL(0x01425B54, int)].drvname, lpcstrModule);
	RCT2_GLOBAL(0x01425B54, int)++;
	return 1;
}

/**
*
*  rct2: 0x0040502E
*/
int get_dsound_devices()
{
	RCT2_GLOBAL(RCT2_ADDRESS_NUM_DSOUND_DEVICES, uint32) = dsound_count_devices();
	RCT2_GLOBAL(RCT2_ADDRESS_DSOUND_DEVICES, rct_dsdevice*) = malloc(RCT2_GLOBAL(RCT2_ADDRESS_NUM_DSOUND_DEVICES, uint32) * sizeof(rct_dsdevice));
	if (RCT2_GLOBAL(RCT2_ADDRESS_DSOUND_DEVICES, rct_dsdevice*)) {
		RCT2_GLOBAL(0x01425B54, int) = 0;
		DirectSoundEnumerate(dsound_enum_callback, 0);
		return RCT2_GLOBAL(0x01425B54, int);
	}
	return 0;
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
	if (RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_MAPPING, LPVOID) && sound_id < RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_MAPPING, uint32*)[0]) {
		return (rct_sound_info*)(RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_MAPPING, int) + RCT2_GLOBAL(RCT2_ADDRESS_SOUND_INFO_LIST_MAPPING, uint32*)[sound_id + 1]);
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
int sound_fill_buffer(LPDIRECTSOUNDBUFFER dsbuffer, char* src, DWORD size)
{
	LPVOID buf;
	LPVOID buf2;
	DWORD buf2size;
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
	if (sound->dsbuffer) {
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
	if (sound->dsbuffer) {
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
	if (sound->dsbuffer) {
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
	if (sound->dsbuffer) {
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
	if (sound->dsbuffer) {
		if (SUCCEEDED(sound->dsbuffer->lpVtbl->SetVolume(sound->dsbuffer, volume)))
			return 1;

	}
	return 0;
}

/**
*
*  rct2: 0x00404DD8
*/
int sound_stop(rct_sound* sound)
{
	if (sound->dsbuffer) {
		sound->dsbuffer->lpVtbl->Release(sound->dsbuffer);
		sound->dsbuffer = 0;
		return sound_remove(sound) ? 1 : 0;
	}
	return 0;
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
*  rct2: 0x006BD0BD
*/
void stop_title_music()
{
	if (RCT2_GLOBAL(0x009AF284, uint32) & (1 << 0)) {
		if (RCT2_GLOBAL(0x009AF600, uint8) != 0) {
			RCT2_GLOBAL(0x014241BC, uint32) = 1;
			sound_channel_stop(3);
			RCT2_GLOBAL(0x014241BC, uint32) = 0;
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