#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../config/Config.h"
#include "../Context.h"
#include "../core/Collections.hpp"
#include "../core/File.h"
#include "../core/FileStream.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "../localisation/string_ids.h"
#include "../OpenRCT2.h"
#include "../ui/UiContext.h"
#include "audio.h"
#include "AudioContext.h"
#include "AudioMixer.h"

extern "C"
{
    #include "../interface/viewport.h"
    #include "../intro.h"
    #include "../localisation/language.h"
    #include "../ride/ride.h"
    #include "../util/util.h"
}

using namespace OpenRCT2::Audio;

struct AudioParams
{
    bool in_range;
    sint32 volume;
    sint32 pan;
};

audio_device *  gAudioDevices = nullptr;
sint32          gAudioDeviceCount;
sint32          gAudioCurrentDevice = -1;

bool    gGameSoundsOff = false;
sint32  gVolumeAdjustZoom = 0;

void *  gTitleMusicChannel = nullptr;
void *  gRainSoundChannel = nullptr;

rct_ride_music gRideMusicList[AUDIO_MAX_RIDE_MUSIC];
rct_ride_music_params   gRideMusicParamsList[6];
rct_ride_music_params * gRideMusicParamsListEnd;

rct_vehicle_sound           gVehicleSoundList[AUDIO_MAX_VEHICLE_SOUNDS];
rct_vehicle_sound_params    gVehicleSoundParamsList[AUDIO_MAX_VEHICLE_SOUNDS];
rct_vehicle_sound_params *  gVehicleSoundParamsListEnd;

static sint32 SoundVolumeAdjust[SOUND_MAXID] =
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

AudioParams audio_get_params_from_location(sint32 soundId, const rct_xyz16 *location);

void audio_init()
{
    if (str_is_null_or_empty(gConfigSound.device))
    {
        Mixer_Init(NULL);
        gAudioCurrentDevice = 0;
    }
    else
    {
        Mixer_Init(gConfigSound.device);

        audio_populate_devices();
        for (sint32 i = 0; i < gAudioDeviceCount; i++)
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

    IAudioContext * audioContext = OpenRCT2::GetContext()->GetAudioContext();
    std::vector<std::string> devices = audioContext->GetOutputDevices();

    // Replace blanks with localised unknown string
    for (size_t i = 0; i < devices.size(); i++)
    {
        if (devices[i].empty())
        {
            devices[i] = language_get_string(STR_OPTIONS_SOUND_VALUE_DEFAULT);
        }
    }

#ifndef __linux__
    // The first device is always system default on Windows and macOS
    std::string defaultDevice = language_get_string(STR_OPTIONS_SOUND_VALUE_DEFAULT);
    devices.insert(devices.begin(), defaultDevice);
#endif

    gAudioDeviceCount = (sint32)devices.size();
    gAudioDevices = Memory::AllocateArray<audio_device>(gAudioDeviceCount);
    for (sint32 i = 0; i < gAudioDeviceCount; i++)
    {
        auto device = &gAudioDevices[i];
        String::Set(device->name, sizeof(device->name), devices[i].c_str());
    }
}

sint32 audio_play_sound_at_location(sint32 soundId, sint16 x, sint16 y, sint16 z)
{
    if (gGameSoundsOff)
        return 0;

    rct_xyz16 location;
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
AudioParams audio_get_params_from_location(sint32 soundId, const rct_xyz16 *location)
{
    sint32 volumeDown = 0;
    AudioParams params;
    params.in_range = true;
    params.volume = 0;
    params.pan = 0;

    rct_map_element * element = map_get_surface_element_at(location->x >> 5, location->y >> 5);
    if (element && (element->base_height * 8) - 5 > location->z)
    {
        volumeDown = 10;
    }

    uint8 rotation = get_current_rotation();
    rct_xy16 pos2 = coordinate_3d_to_2d(location, rotation);
    rct_window * window = gWindowNextSlot;
    while (true)
    {
        window--;
        if (window < g_window_list)
        {
            break;
        }

        rct_viewport * viewport = window->viewport;
        if (viewport != nullptr && (viewport->flags & VIEWPORT_FLAG_SOUND_ON))
        {
            sint16 vy = pos2.y - viewport->view_y;
            sint16 vx = pos2.x - viewport->view_x;
            params.pan = viewport->x + (vx >> viewport->zoom);
            params.volume = SoundVolumeAdjust[soundId] + ((-1024 * viewport->zoom - 1) << volumeDown) + 1;

            if (vy < 0 || vy >= viewport->view_height || vx < 0 || vx >= viewport->view_width || params.volume < -10000)
            {
                params.in_range = false;
                return params;
            }
        }
    }

    return params;
}

sint32 audio_play_sound(sint32 soundId, sint32 volume, sint32 pan)
{
    if (gGameSoundsOff)
        return 0;

    sint32 mixerPan = 0;
    if (pan != AUDIO_PLAY_AT_CENTRE)
    {
        sint32 x2 = pan << 16;
        uint16 screenWidth = Math::Max<sint32>(64, OpenRCT2::GetContext()->GetUiContext()->GetWidth());
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

    sint32 pathId;
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
    for (sint32 i = 0; i < AUDIO_MAX_RIDE_MUSIC; i++)
    {
        rct_ride_music * rideMusic = &gRideMusicList[i];
        if (rideMusic->ride_id != RIDE_ID_NULL)
        {
            rideMusic->ride_id = RIDE_ID_NULL;
            if (rideMusic->sound_channel != nullptr)
            {
                Mixer_Stop_Channel(rideMusic->sound_channel);
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
    sint32 deviceNum = 0;
    audio_init_ride_sounds(deviceNum);

    for (size_t m = 0; m < Util::CountOf(gRideMusicInfoList); m++)
    {
        rct_ride_music_info *rideMusicInfo = gRideMusicInfoList[m];
        const utf8 *path = get_file_path(rideMusicInfo->path_id);
        if (File::Exists(path))
        {
            try
            {
                auto fs = FileStream(path, FILE_MODE_OPEN);
                uint32 head = fs.ReadValue<uint32>();
                if (head == 0x78787878)
                {
                    rideMusicInfo->length = 0;
                }
            }
            catch (const Exception &)
            {
            }
        }
    }
}

void audio_init_ride_sounds(sint32 device)
{
    audio_close();
    for (sint32 i = 0; i < AUDIO_MAX_VEHICLE_SOUNDS; i++)
    {
        rct_vehicle_sound * vehicleSound = &gVehicleSoundList[i];
        vehicleSound->id = SOUND_ID_NULL;
    }

    gAudioCurrentDevice = device;
    config_save_default();
    for (sint32 i = 0; i < AUDIO_MAX_RIDE_MUSIC; i++)
    {
        rct_ride_music * rideMusic = &gRideMusicList[i];
        rideMusic->ride_id = RIDE_ID_NULL;
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

    for (size_t i = 0; i < Util::CountOf(gVehicleSoundList); i++)
    {
        rct_vehicle_sound * vehicleSound = &gVehicleSoundList[i];
        if (vehicleSound->id != SOUND_ID_NULL)
        {
            vehicleSound->id = SOUND_ID_NULL;
            if (vehicleSound->sound1_id != SOUND_ID_NULL)
            {
                Mixer_Stop_Channel(vehicleSound->sound1_channel);
            }
            if (vehicleSound->sound2_id != SOUND_ID_NULL)
            {
                Mixer_Stop_Channel(vehicleSound->sound2_channel);
            }
        }
    }
}
