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

#include "rct2.h"

typedef struct {
	char name[256];
} audio_device;

extern int gAudioDeviceCount;
extern audio_device *gAudioDevices;

void audio_init();
void audio_quit();
void audio_get_devices();
void audio_init2(int device);

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
typedef struct rct_sound {
	LPDIRECTSOUNDBUFFER dsbuffer;
	int id;
	int has_caps;
	int var_0C;
	struct rct_sound* next;
} rct_sound;

typedef struct {
	uint16 var_0;
	uint16 channels;
	uint32 samples;
	uint32 var_8;
	uint16 bytes;
	uint16 bits;
	uint16 var_E;
} rct_audio_info;

typedef struct {
	uint32 var_0;
	uint8 pad_4[0x118];
	HMMIO hmmio;
	HGLOBAL hmem;
	uint8 pad_124[0x3C];
	uint32 var_160;
	uint32 var_164;
	uint32 var_168;
} rct_sound_channel;

typedef struct {
	uint32 size;
	WAVEFORMATEX format;
	char* data;
} rct_sound_info;

typedef struct {
	uint16 id;
	uint16 var_2;
	rct_sound sound1;		// 0x04
	uint16 var_18;
	uint8 pad_1A[0x06];
	rct_sound sound2;		// 0x20
	uint16 var_34;
	uint8 pad_36[0x06];
} rct_vehicle_sound;

typedef struct {
	uint16 id;
	rct_sound sound;
} rct_other_sound;

typedef struct {
	uint32 var_0;
	uint32 var_4;
	uint32 var_8;
	uint16 var_C;
	uint16 var_E;
	uint32 var_10;
	uint32 var_14;
	uint32 var_18;
	uint32 var_1C;
	uint32 var_20;
	uint32 var_24;
	uint32 var_28;
	uint32 var_2C;
	uint32 var_30;
	uint32 var_34;
	uint32 var_38;
	uint32 var_3C;
	uint32 var_40;
	uint32 var_44;
	uint32 var_48;
	uint32 var_4C;
	uint32 var_50;
	uint32 var_54;
	uint32 var_58;
	uint32 var_5C;
	uint32 var_60;
	uint32 var_64;
	uint32 var_68;
	uint32 var_6C;
	uint32 var_70;
	uint32 var_74;
	uint32 var_78;
	uint32 var_7C;
	uint32 var_80;
	uint32 var_84;
	uint32 var_88;
	uint32 var_8C;
	uint32 var_90;
	uint32 var_94;
	uint32 var_98;
	uint32 var_9C;
	uint32 var_A0;
	uint32 var_A4;
	uint32 var_A8;
	uint32 var_AC;
	uint32 var_B0;
	uint32 var_B4;
	uint32 var_B8;
	uint32 var_BC;
	uint32 var_C0;
	uint32 var_C4;
	uint32 var_C8;
	uint32 var_CC;
	uint32 var_D0;
	uint32 var_D4;
	uint32 var_D8;
	uint32 var_DC;
	uint32 var_E0;
	uint32 var_E4;
	uint32 var_E8;
	uint32 var_EC;
	uint32 var_F0;
	uint32 var_F4;
	uint32 var_F8;
	uint32 var_FC;
	uint32 var_100;
	uint32 var_104;
	uint32 var_108;
	uint32 var_10C;
	uint32 var_110;
	uint32 var_114;
	uint32 var_118;
	HGLOBAL var_11C;
	HMMIO var_120;
	uint32 var_124;
	uint32 var_128;
	uint32 var_12C;
	uint32 var_130;
	uint32 var_134;
	uint32 var_138;
	uint32 var_13C;
	uint32 var_140;
	uint32 var_144;
	uint32 var_148;
	LPDIRECTSOUNDBUFFER dsbuffer;	// 0x14C
	uint32 var_150;
	uint32 playpos;					// 0x154
	uint32 var_158;
	uint32 var_15C;
	uint32 var_160;
	uint32 var_164;
	uint32 var_168;
} rct_sound_unknown;

int get_dsound_devices();
int sound_prepare(int sound_id, rct_sound *sound, int channels, int software);
int sound_play_panned(int sound_id, int x);
int sound_play(rct_sound* sound, int looping, int volume, int pan, int frequency);
int sound_is_playing(rct_sound* sound);
int sound_set_frequency(rct_sound* sound, int frequency);
int sound_set_pan(rct_sound* sound, int pan);
int sound_set_volume(rct_sound* sound, int volume);
int sound_channel_play(int channel, int a2, int volume, int pan, int frequency);
int sound_channel_set_frequency(int channel, int frequency);
int sound_channel_set_pan(int channel, int pan);
int sound_channel_set_volume(int channel, int volume);
void sound_channel_free(HMMIO* hmmio, HGLOBAL* hmem);
int sound_stop(rct_sound *sound);
int sound_stop_all();
int unmap_file(LPCVOID base);
int sound_channel_stop(int channel);
rct_sound* sound_add(rct_sound* sound);
rct_sound* sound_remove(rct_sound* sound);
rct_sound* sound_begin();
rct_sound* sound_next(rct_sound* sound);
void pause_sounds();
void stop_other_sounds();
void stop_vehicle_sounds();
void stop_ride_music();
void stop_peep_sounds();
void stop_title_music();
void unpause_sounds();

// 0x009AF59C probably does the same job
// once it's confirmed and calls in pause_sounds() are reversed, it can be used instead of this
int g_sounds_disabled;

typedef enum {
	SOUND_LIFT_1 = 0,
	SOUND_TRACK_FRICTION_1 = 1,
	SOUND_LIFT_2 = 2,
	SOUND_SCREAM_1 = 3,
	SOUND_CLICK_1 = 4,
	SOUND_CLICK_2 = 5,
	SOUND_PLACE_ITEM = 6,
	SOUND_SCREAM_2 = 7,
	SOUND_SCREAM_3 = 8,
	SOUND_SCREAM_4 = 9,
	SOUND_SCREAM_5 = 10,
	SOUND_SCREAM_6 = 11,
	SOUND_LIFT_3 = 12,
	SOUND_PURCHASE = 13,
	SOUND_CRASH = 14,
	SOUND_LAYING_OUT_WATER = 15,
	SOUND_WATER_1 = 16,
	SOUND_WATER_2 = 17,
	SOUND_TRAIN_WHISTLE = 18,
	SOUND_TRAIN_CHUGGING = 19,
	SOUND_WATER_SPLASH = 20,
	SOUND_HAMMERING = 21,
	SOUND_RIDE_LAUNCH_1 = 22,
	SOUND_RIDE_LAUNCH_2 = 23,
	SOUND_COUGH_1 = 24,
	SOUND_COUGH_2 = 25,
	SOUND_COUGH_3 = 26,
	SOUND_COUGH_4 = 27,
	SOUND_RAIN_1 = 28,
	SOUND_THUNDER_1 = 29,
	SOUND_THUNDER_2 = 30,
	SOUND_RAIN_2 = 31,
	SOUND_RAIN_3 = 32,
	SOUND_BALLOON_POP = 33,
	SOUND_MECHANIC_FIX = 34,
	SOUND_SCREAM_7 = 35,
	SOUND_TOILET_FLUSH = 36,
	SOUND_CLICK_3 = 37,
	SOUND_QUACK = 38,
	SOUND_NEWS_ITEM = 39,
	SOUND_WINDOW_OPEN = 40,
	SOUND_LAUGH_1 = 41,
	SOUND_LAUGH_2 = 42,
	SOUND_LAUGH_3 = 43,
	SOUND_APPLAUSE = 44,
	SOUND_HAUNTED_HOUSE_SCARE = 45,
	SOUND_HAUNTED_HOUSE_SCREAM_1 = 46,
	SOUND_HAUNTED_HOUSE_SCREAM_2 = 47,
	SOUND_48 = 48,
	SOUND_49 = 49,
	SOUND_ERROR = 50,
	SOUND_51 = 51,
	SOUND_LIFT_4 = 52,
	SOUND_LIFT_5 = 53,
	SOUND_TRACK_FRICTION_2 = 54,
	SOUND_LIFT_6 = 55,
	SOUND_LIFT_7 = 56,
	SOUND_TRACK_FRICTION_3 = 57,
	SOUND_SCREAM_8 = 58,
	SOUND_TRAM = 59,
	SOUND_DOOR_OPEN = 60,
	SOUND_DOOR_CLOSE = 61,
	SOUND_62 = 62
} RCT2_SOUND;

#endif
