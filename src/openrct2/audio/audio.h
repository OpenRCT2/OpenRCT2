/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../ride/RideTypes.h"

#define AUDIO_DEVICE_NAME_SIZE 256
#define AUDIO_MAX_RIDE_MUSIC 32
#define AUDIO_MAX_VEHICLE_SOUNDS 14
#define NUM_DEFAULT_MUSIC_TRACKS 46
#define AUDIO_PLAY_AT_CENTRE 0x8000
#define AUDIO_PLAY_AT_LOCATION 0x8001
#define SOUND_ID_NULL 0xFFFF

enum class SoundId : uint8_t;
struct CoordsXYZ;

struct audio_device
{
    char name[AUDIO_DEVICE_NAME_SIZE];
};

struct rct_ride_music
{
    ride_id_t ride_id;
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
    ride_id_t ride_id;
    uint8_t tune_id;
    int32_t offset;
    int16_t volume;
    int16_t pan;
    uint16_t frequency;
};

struct Sound
{
    SoundId Id;
    int16_t Volume;
    int16_t Pan;
    uint16_t Frequency;
    void* Channel;
};

struct rct_vehicle_sound
{
    uint16_t id;
    int16_t volume;
    Sound TrackSound;
    Sound OtherSound;
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

enum class SoundId : uint8_t
{
    LiftClassic,
    TrackFrictionClassicWood,
    FrictionClassic,
    Scream1,
    Click1,
    Click2,
    PlaceItem,
    Scream2,
    Scream3,
    Scream4,
    Scream5,
    Scream6,
    LiftFrictionWheels,
    Purchase,
    Crash,
    LayingOutWater,
    Water1,
    Water2,
    TrainWhistle,
    TrainDeparting,
    WaterSplash,
    GoKartEngine,
    RideLaunch1,
    RideLaunch2,
    Cough1,
    Cough2,
    Cough3,
    Cough4,
    Rain,
    Thunder1,
    Thunder2,
    TrackFrictionTrain,
    TrackFrictionWater,
    BalloonPop,
    MechanicFix,
    Scream7,
    ToiletFlush,
    Click3,
    Quack,
    NewsItem,
    WindowOpen,
    Laugh1,
    Laugh2,
    Laugh3,
    Applause,
    HauntedHouseScare,
    HauntedHouseScream1,
    HauntedHouseScream2,
    BlockBrakeClose,
    BlockBrakeRelease,
    Error,
    BrakeRelease,
    LiftArrow,
    LiftWood,
    TrackFrictionWood,
    LiftWildMouse,
    LiftBM,
    TrackFrictionBM,
    Scream8,
    Tram,
    DoorOpen,
    DoorClose,
    Portcullis,
    NoScream = 254,
    Null = 255
};

constexpr uint8_t RCT2SoundCount = static_cast<uint32_t>(SoundId::Portcullis) + 1;

extern audio_device* gAudioDevices;
extern int32_t gAudioDeviceCount;
extern int32_t gAudioCurrentDevice;

extern bool gGameSoundsOff;
extern int32_t gVolumeAdjustZoom;

extern void* gTitleMusicChannel;
extern void* gRainSoundChannel;

extern rct_ride_music gRideMusicList[AUDIO_MAX_RIDE_MUSIC];
extern rct_ride_music_info gRideMusicInfoList[NUM_DEFAULT_MUSIC_TRACKS];
extern rct_ride_music_params gRideMusicParamsList[AUDIO_MAX_RIDE_MUSIC];
extern rct_ride_music_params* gRideMusicParamsListEnd;

extern rct_vehicle_sound gVehicleSoundList[AUDIO_MAX_VEHICLE_SOUNDS];

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
 */
void audio_play_sound(SoundId soundId, int32_t volume, int32_t pan);
/**
 * Plays the specified sound at a virtual location.
 * @param soundId The sound effect to play.
 * @param x The x coordinate of the location.
 * @param y The y coordinate of the location.
 * @param z The z coordinate of the location.
 */
void audio_play_sound_at_location(SoundId soundId, const CoordsXYZ& loc);
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
