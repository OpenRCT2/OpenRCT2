/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#define AUDIO_DEVICE_NAME_SIZE      256
#define AUDIO_MAX_RIDE_MUSIC        2
#define AUDIO_MAX_VEHICLE_SOUNDS    14
#define NUM_DEFAULT_MUSIC_TRACKS    46
#define AUDIO_PLAY_AT_CENTRE        0x8000
#define AUDIO_PLAY_AT_LOCATION      0x8001
#define SOUND_ID_NULL               0xFFFF

struct audio_device
{
    char name[AUDIO_DEVICE_NAME_SIZE];
};

struct rct_ride_music
{
    uint8_t ride_id;
    uint8_t tune_id;
    int16_t volume;
    int16_t pan;
    uint16_t frequency;
    void* sound_channel;
};

struct rct_ride_music_info
{
    uint8_t path_id;
    uint32_t offset;
    uint32_t length;
};

struct rct_ride_music_params
{
    uint8_t ride_id;
    uint8_t tune_id;
    int32_t offset;
    int16_t volume;
    int16_t pan;
    uint16_t frequency;
};

struct rct_vehicle_sound
{
    uint16_t id;
    int16_t volume;
    uint16_t sound1_id;
    int16_t sound1_volume;
    int16_t sound1_pan;
    uint16_t sound1_freq;
    uint16_t sound2_id;
    int16_t sound2_volume;
    int16_t sound2_pan;
    uint16_t sound2_freq;
    void* sound1_channel;
    void* sound2_channel;
};

struct rct_vehicle_sound_params
{
    uint16_t id;
    int16_t pan_x;
    int16_t pan_y;
    uint16_t frequency;
    int16_t volume;
    uint16_t priority;
};

enum RCT2_SOUND
{
    SOUND_LIFT_1,
    SOUND_TRACK_FRICTION_1,
    SOUND_LIFT_2,
    SOUND_SCREAM_1,
    SOUND_CLICK_1,
    SOUND_CLICK_2,
    SOUND_PLACE_ITEM,
    SOUND_SCREAM_2,
    SOUND_SCREAM_3,
    SOUND_SCREAM_4,
    SOUND_SCREAM_5,
    SOUND_SCREAM_6,
    SOUND_LIFT_3,
    SOUND_PURCHASE,
    SOUND_CRASH,
    SOUND_LAYING_OUT_WATER,
    SOUND_WATER_1,
    SOUND_WATER_2,
    SOUND_TRAIN_WHISTLE,
    SOUND_TRAIN_CHUGGING,
    SOUND_WATER_SPLASH,
    SOUND_HAMMERING,
    SOUND_RIDE_LAUNCH_1,
    SOUND_RIDE_LAUNCH_2,
    SOUND_COUGH_1,
    SOUND_COUGH_2,
    SOUND_COUGH_3,
    SOUND_COUGH_4,
    SOUND_RAIN_1,
    SOUND_THUNDER_1,
    SOUND_THUNDER_2,
    SOUND_RAIN_2,
    SOUND_RAIN_3,
    SOUND_BALLOON_POP,
    SOUND_MECHANIC_FIX,
    SOUND_SCREAM_7,
    SOUND_TOILET_FLUSH,
    SOUND_CLICK_3,
    SOUND_QUACK,
    SOUND_NEWS_ITEM,
    SOUND_WINDOW_OPEN,
    SOUND_LAUGH_1,
    SOUND_LAUGH_2,
    SOUND_LAUGH_3,
    SOUND_APPLAUSE,
    SOUND_HAUNTED_HOUSE_SCARE,
    SOUND_HAUNTED_HOUSE_SCREAM_1,
    SOUND_HAUNTED_HOUSE_SCREAM_2,
    SOUND_48,
    SOUND_49,
    SOUND_ERROR,
    SOUND_51,
    SOUND_LIFT_4,
    SOUND_LIFT_5,
    SOUND_TRACK_FRICTION_2,
    SOUND_LIFT_6,
    SOUND_LIFT_7,
    SOUND_TRACK_FRICTION_3,
    SOUND_SCREAM_8,
    SOUND_TRAM,
    SOUND_DOOR_OPEN,
    SOUND_DOOR_CLOSE,
    SOUND_62,
    SOUND_MAXID
};

extern audio_device *   gAudioDevices;
extern int32_t           gAudioDeviceCount;
extern int32_t           gAudioCurrentDevice;

extern bool     gGameSoundsOff;
extern int32_t   gVolumeAdjustZoom;

extern void *   gTitleMusicChannel;
extern void *   gRainSoundChannel;

extern rct_ride_music           gRideMusicList[AUDIO_MAX_RIDE_MUSIC];
extern rct_ride_music_info      gRideMusicInfoList[NUM_DEFAULT_MUSIC_TRACKS];
extern rct_ride_music_params    gRideMusicParamsList[6];
extern rct_ride_music_params *  gRideMusicParamsListEnd;

extern rct_vehicle_sound            gVehicleSoundList[AUDIO_MAX_VEHICLE_SOUNDS];
extern rct_vehicle_sound_params     gVehicleSoundParamsList[AUDIO_MAX_VEHICLE_SOUNDS];
extern rct_vehicle_sound_params *   gVehicleSoundParamsListEnd;

/**
* Deregisters the audio device.
* rct2: 0x006BAB21
*/
void audio_close();
/**
* Initialises the audio subsystem.
*/
void audio_init();
/**
* Loads the ride sounds and info.
* rct2: 0x006BA8E0
*/
void audio_init_ride_sounds_and_info();
/**
* Loads the ride sounds.
* rct2: 0x006BA9B5
*/
void audio_init_ride_sounds(int32_t device);
/**
* Temporarily stops playing sounds until audio_unpause_sounds() is called.
* rct2: 0x006BABB4
*/
void audio_pause_sounds();
/**
* Plays the specified sound.
* @param soundId The sound effect to play.
* @param volume The volume at which the sound effect should be played.
* @param pan The pan at which the sound effect should be played. If set to anything other than AUDIO_PLAY_AT_CENTRE, plays the
* sound at a position relative to the centre of the viewport.
* @return 0 if the sound was not out of range; otherwise, soundId.
*/
int32_t audio_play_sound(int32_t soundId, int32_t volume, int32_t pan);
/**
* Plays the specified sound at a virtual location.
* @param soundId The sound effect to play.
* @param x The x coordinate of the location.
* @param y The y coordinate of the location.
* @param z The z coordinate of the location.
* @return 0 if the sound was not out of range; otherwise, soundId.
*/
int32_t audio_play_sound_at_location(int32_t soundId, int16_t x, int16_t y, int16_t z);
/**
* Populates the gAudioDevices array with the available audio devices.
*/
void audio_populate_devices();
/**
* Starts playing the title music.
* rct2: 0x006BD0F8
*/
void audio_start_title_music();
/**
* Stops the rain sound effect from playing.
*/
void audio_stop_rain_sound();
/**
* Stops ride music from playing.
* rct2: 0x006BCA9F
*/
void audio_stop_ride_music();
/**
* Stops the title music from playing.
* rct2: 0x006BD0BD
*/
void audio_stop_title_music();
/**
* Stops vehicle sounds from playing.
* rct2: 0x006BABDF
*/
void audio_stop_vehicle_sounds();
/**
* Toggles whether all sounds should be played.
* rct2: 0x006BAB8A
*/
void audio_toggle_all_sounds();
/**
* Resumes playing sounds that had been paused by a call to audio_pause_sounds().
* rct2: 0x006BABD8
*/
void audio_unpause_sounds();

void audio_stop_all_music_and_sounds();
