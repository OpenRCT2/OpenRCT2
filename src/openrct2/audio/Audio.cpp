/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../config/Config.h"
#include "../Context.h"
#include "../core/File.h"
#include "../core/FileStream.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../localisation/StringIds.h"
#include "../OpenRCT2.h"
#include "../ui/UiContext.h"
#include "audio.h"
#include "AudioContext.h"
#include "AudioMixer.h"

#include "../interface/Viewport.h"
#include "../Intro.h"
#include "../localisation/Language.h"
#include "../ride/Ride.h"
#include "../util/Util.h"

using namespace OpenRCT2::Audio;

struct AudioParams
{
    bool in_range;
    int32_t volume;
    int32_t pan;
};

audio_device *  gAudioDevices = nullptr;
int32_t          gAudioDeviceCount;
int32_t          gAudioCurrentDevice = -1;

bool    gGameSoundsOff = false;
int32_t  gVolumeAdjustZoom = 0;

void *  gTitleMusicChannel = nullptr;
void *  gRainSoundChannel = nullptr;

rct_ride_music gRideMusicList[AUDIO_MAX_RIDE_MUSIC];
rct_ride_music_params   gRideMusicParamsList[6];
rct_ride_music_params * gRideMusicParamsListEnd;

rct_vehicle_sound           gVehicleSoundList[AUDIO_MAX_VEHICLE_SOUNDS];
rct_vehicle_sound_params    gVehicleSoundParamsList[AUDIO_MAX_VEHICLE_SOUNDS];
rct_vehicle_sound_params *  gVehicleSoundParamsListEnd;

// clang-format off
static int32_t SoundVolumeAdjust[SOUND_MAXID] =
{
    0,      // SOUND_LIFT_1
    0,      // SOUND_TRACK_FRICTION_1
    0,      // SOUND_LIFT_2
    0,      // SOUND_SCREAM_1
    0,      // SOUND_CLICK_1
    0,      // SOUND_CLICK_2
    0,      // SOUND_PLACE_ITEM
    0,      // SOUND_SCREAM_2
    0,      // SOUND_SCREAM_3
    0,      // SOUND_SCREAM_4
    0,      // SOUND_SCREAM_5
    0,      // SOUND_SCREAM_6
    0,      // SOUND_LIFT_3
    -400,   // SOUND_PURCHASE
    0,      // SOUND_CRASH
    0,      // SOUND_LAYING_OUT_WATER
    0,      // SOUND_WATER_1
    0,      // SOUND_WATER_2
    0,      // SOUND_TRAIN_WHISTLE
    0,      // SOUND_TRAIN_CHUGGING
    -1000,  // SOUND_WATER_SPLASH
    0,      // SOUND_HAMMERING
    -800,   // SOUND_RIDE_LAUNCH_1
    -1700,  // SOUND_RIDE_LAUNCH_2
    -700,   // SOUND_COUGH_1
    -700,   // SOUND_COUGH_2
    -700,   // SOUND_COUGH_3
    -700,   // SOUND_COUGH_4
    0,      // SOUND_RAIN_1
    0,      // SOUND_THUNDER_1
    0,      // SOUND_THUNDER_2
    0,      // SOUND_RAIN_2
    0,      // SOUND_RAIN_3
    0,      // SOUND_BALLOON_POP
    -700,   // SOUND_MECHANIC_FIX
    0,      // SOUND_SCREAM_7
    -2500,  // SOUND_TOILET_FLUSH original value: -1000
    0,      // SOUND_CLICK_3
    0,      // SOUND_QUACK
    0,      // SOUND_NEWS_ITEM
    0,      // SOUND_WINDOW_OPEN
    -900,   // SOUND_LAUGH_1
    -900,   // SOUND_LAUGH_2
    -900,   // SOUND_LAUGH_3
    0,      // SOUND_APPLAUSE
    -600,   // SOUND_HAUNTED_HOUSE_SCARE
    -700,   // SOUND_HAUNTED_HOUSE_SCREAM_1
    -700,   // SOUND_HAUNTED_HOUSE_SCREAM_2
    -2550,  // SOUND_48
    -2900,  // SOUND_49
    0,      // SOUND_ERROR
    -3400,  // SOUND_51
    0,      // SOUND_LIFT_4
    0,      // SOUND_LIFT_5
    0,      // SOUND_TRACK_FRICTION_2
    0,      // SOUND_LIFT_6
    0,      // SOUND_LIFT_7
    0,      // SOUND_TRACK_FRICTION_3
    0,      // SOUND_SCREAM_8
    0,      // SOUND_TRAM
    -2000,  // SOUND_DOOR_OPEN
    -2700,  // SOUND_DOOR_CLOSE
    -700    // SOUND_62
};
// clang-format on

AudioParams audio_get_params_from_location(int32_t soundId, const LocationXYZ16 *location);

void audio_init()
{
    if (str_is_null_or_empty(gConfigSound.device))
    {
        Mixer_Init(nullptr);
        gAudioCurrentDevice = 0;
    }
    else
    {
        Mixer_Init(gConfigSound.device);

        audio_populate_devices();
        for (int32_t i = 0; i < gAudioDeviceCount; i++)
        {
            if (String::Equals(gAudioDevices[i].name, gConfigSound.device))
            {
                gAudioCurrentDevice = i;
            }
        }
    }
}

void audio_populate_devices()
{
    SafeFree(gAudioDevices);

    auto audioContext = OpenRCT2::GetContext()->GetAudioContext();
    std::vector<std::string> devices = audioContext->GetOutputDevices();

    // Replace blanks with localised unknown string
    for (auto &device : devices)
    {
        if (device.empty())
        {
            device = language_get_string(STR_OPTIONS_SOUND_VALUE_DEFAULT);
        }
    }

#ifndef __linux__
    // The first device is always system default on Windows and macOS
    std::string defaultDevice = language_get_string(STR_OPTIONS_SOUND_VALUE_DEFAULT);
    devices.insert(devices.begin(), defaultDevice);
#endif

    gAudioDeviceCount = (int32_t)devices.size();
    gAudioDevices = Memory::AllocateArray<audio_device>(gAudioDeviceCount);
    for (int32_t i = 0; i < gAudioDeviceCount; i++)
    {
        auto device = &gAudioDevices[i];
        String::Set(device->name, sizeof(device->name), devices[i].c_str());
    }
}

int32_t audio_play_sound_at_location(int32_t soundId, int16_t x, int16_t y, int16_t z)
{
    if (gGameSoundsOff)
        return 0;

    LocationXYZ16 location;
    location.x = x;
    location.y = y;
    location.z = z;

    AudioParams params = audio_get_params_from_location(soundId, &location);
    if (params.in_range)
    {
        soundId = audio_play_sound(soundId, params.volume, params.pan);
    }
    return soundId;
}

/**
* Returns the audio parameters to use when playing the specified sound at a virtual location.
* @param soundId The sound effect to be played.
* @param location The location at which the sound effect is to be played.
* @return The audio parameters to be used when playing this sound effect.
*/
AudioParams audio_get_params_from_location(int32_t soundId, const LocationXYZ16 *location)
{
    int32_t volumeDown = 0;
    AudioParams params;
    params.in_range = true;
    params.volume = 0;
    params.pan = 0;

    rct_tile_element * element = map_get_surface_element_at({location->x, location->y});
    if (element && (element->base_height * 8) - 5 > location->z)
    {
        volumeDown = 10;
    }

    uint8_t rotation = get_current_rotation();
    LocationXY16 pos2 = coordinate_3d_to_2d(location, rotation);

    rct_viewport * viewport = nullptr;
    while ((viewport = window_get_previous_viewport(viewport)) != nullptr)
    {
        if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
        {
            int16_t vy = pos2.y - viewport->view_y;
            int16_t vx = pos2.x - viewport->view_x;
            params.pan = viewport->x + (vx >> viewport->zoom);
            params.volume = SoundVolumeAdjust[soundId] + ((-1024 * viewport->zoom - 1) * (1 << volumeDown)) + 1;

            if (vy < 0 || vy >= viewport->view_height || vx < 0 || vx >= viewport->view_width || params.volume < -10000)
            {
                params.in_range = false;
                return params;
            }
        }
    }

    return params;
}

int32_t audio_play_sound(int32_t soundId, int32_t volume, int32_t pan)
{
    if (gGameSoundsOff)
        return 0;

    int32_t mixerPan = 0;
    if (pan != AUDIO_PLAY_AT_CENTRE)
    {
        int32_t x2 = pan << 16;
        uint16_t screenWidth = std::max<int32_t>(64, OpenRCT2::GetContext()->GetUiContext()->GetWidth());
        mixerPan = ((x2 / screenWidth) - 0x8000) >> 4;
    }

    Mixer_Play_Effect(soundId, MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(mixerPan), 1, 1);
    return 0;
}

void audio_start_title_music()
{
    if (gGameSoundsOff || !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) || gIntroState != INTRO_STATE_NONE)
    {
        audio_stop_title_music();
        return;
    }

    if (gTitleMusicChannel != nullptr)
    {
        return;
    }

    int32_t pathId;
    switch (gConfigSound.title_music) {
    case 1:
        pathId = PATH_ID_CSS50;
        break;
    case 2:
        pathId = PATH_ID_CSS17;
        break;
    case 3:
        pathId = (util_rand() & 1) ? PATH_ID_CSS50 :
                                     PATH_ID_CSS17;
        break;
    default:
        return;
    }

    gTitleMusicChannel = Mixer_Play_Music(pathId, MIXER_LOOP_INFINITE, true);
    if (gTitleMusicChannel != nullptr)
    {
        Mixer_Channel_SetGroup(gTitleMusicChannel, MIXER_GROUP_TITLE_MUSIC);
    }
}

void audio_stop_ride_music()
{
    for (auto &rideMusic : gRideMusicList)
    {
        if (rideMusic.ride_id != RIDE_ID_NULL)
        {
            rideMusic.ride_id = RIDE_ID_NULL;
            if (rideMusic.sound_channel != nullptr)
            {
                Mixer_Stop_Channel(rideMusic.sound_channel);
            }
        }
    }
}

void audio_stop_all_music_and_sounds()
{
    audio_stop_title_music();
    audio_stop_vehicle_sounds();
    audio_stop_ride_music();
    peep_stop_crowd_noise();
    audio_stop_rain_sound();
}

void audio_stop_title_music()
{
    if (gTitleMusicChannel != nullptr)
    {
        Mixer_Stop_Channel(gTitleMusicChannel);
        gTitleMusicChannel = nullptr;
    }
}

void audio_stop_rain_sound()
{
    if (gRainSoundChannel != nullptr)
    {
        Mixer_Stop_Channel(gRainSoundChannel);
        gRainSoundChannel = nullptr;
    }
}

void audio_init_ride_sounds_and_info()
{
    int32_t deviceNum = 0;
    audio_init_ride_sounds(deviceNum);

    for (auto &rideMusicInfo : gRideMusicInfoList)
    {
        const utf8 * path = context_get_path_legacy(rideMusicInfo.path_id);
        if (File::Exists(path))
        {
            try
            {
                auto fs = FileStream(path, FILE_MODE_OPEN);
                uint32_t head = fs.ReadValue<uint32_t>();
                if (head == 0x78787878)
                {
                    rideMusicInfo.length = 0;
                }
                // The length used to be hardcoded, but we stopped doing that to allow replacement.
                if (rideMusicInfo.length == 0)
                {
                    rideMusicInfo.length = fs.GetLength();
                }
            }
            catch (const std::exception &)
            {
            }
        }
    }
}

void audio_init_ride_sounds(int32_t device)
{
    audio_close();
    for (auto &vehicleSound : gVehicleSoundList)
    {
        vehicleSound.id = SOUND_ID_NULL;
    }

    gAudioCurrentDevice = device;
    config_save_default();
    for (auto &rideMusic : gRideMusicList)
    {
        rideMusic.ride_id = RIDE_ID_NULL;
    }
}

void audio_close()
{
    peep_stop_crowd_noise();
    audio_stop_title_music();
    audio_stop_ride_music();
    audio_stop_rain_sound();
    gAudioCurrentDevice = -1;
}

void audio_toggle_all_sounds()
{
    gConfigSound.sound_enabled = !gConfigSound.sound_enabled;
    if (gConfigSound.sound_enabled)
    {
        audio_unpause_sounds();
    }
    else
    {
        audio_stop_title_music();
        audio_pause_sounds();
    }
}

void audio_pause_sounds()
{
    gGameSoundsOff = true;
    audio_stop_vehicle_sounds();
    audio_stop_ride_music();
    peep_stop_crowd_noise();
    audio_stop_rain_sound();
}

void audio_unpause_sounds()
{
    gGameSoundsOff = false;
}

void audio_stop_vehicle_sounds()
{
    if (gAudioCurrentDevice == -1)
    {
        return;
    }

    for (auto &vehicleSound : gVehicleSoundList)
    {
        if (vehicleSound.id != SOUND_ID_NULL)
        {
            vehicleSound.id = SOUND_ID_NULL;
            if (vehicleSound.sound1_id != SOUND_ID_NULL)
            {
                Mixer_Stop_Channel(vehicleSound.sound1_channel);
            }
            if (vehicleSound.sound2_id != SOUND_ID_NULL)
            {
                Mixer_Stop_Channel(vehicleSound.sound2_channel);
            }
        }
    }
}
