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

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <dsound.h>

/**
 * Represents a single directsound device.
 */
typedef struct {
	GUID guid;
	CHAR desc[256];
	CHAR drvname[256];
} rct_dsdevice;

/**
 * Represents a prepared sound.
 */
typedef struct {
	LPDIRECTSOUNDBUFFER dsbuffer;
	int id;
	int has_caps;
	int var_0C;
	int var_10;
} rct_sound;

void get_dsound_devices();
int sound_prepare(int sound_id, rct_sound *sound, int var_8, int var_c);
void sound_play_panned(int sound_id, int x);
int sound_play(rct_sound* sound, int looping, int volume, int pan, int frequency);
void sound_stop(rct_sound *sound);

typedef enum {
	RCT2_SOUND_SCREAM = 11,
	RCT2_SOUND_CHAINLIFT = 56,
	RCT2_SOUND_TRACKFRICTION = 57,
} RCT2_SOUND;

#endif