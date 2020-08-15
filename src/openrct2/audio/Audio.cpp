/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "audio.h"

#include "../Context.h"
#include "../Intro.h"
#include "../OpenRCT2.h"
#include "../config/Config.h"
#include "../core/File.h"
#include "../core/FileStream.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../interface/Viewport.h"
#include "../localisation/Language.h"
#include "../localisation/StringIds.h"
#include "../peep/Peep.h"
#include "../ride/Ride.h"
#include "../ui/UiContext.h"
#include "../util/Util.h"
#include "AudioContext.h"
#include "AudioMixer.h"

#include <algorithm>

using namespace OpenRCT2::Audio;

struct AudioParams
{
    bool in_range;
    int32_t volume;
    int32_t pan;
};

audio_device* gAudioDevices = nullptr;
int32_t gAudioDeviceCount;
int32_t gAudioCurrentDevice = -1;

bool gGameSoundsOff = false;
int32_t gVolumeAdjustZoom = 0;

void* gTitleMusicChannel = nullptr;
void* gRainSoundChannel = nullptr;

rct_ride_music gRideMusicList[AUDIO_MAX_RIDE_MUSIC];
rct_ride_music_params gRideMusicParamsList[AUDIO_MAX_RIDE_MUSIC];
rct_ride_music_params* gRideMusicParamsListEnd;

rct_vehicle_sound gVehicleSoundList[AUDIO_MAX_VEHICLE_SOUNDS];

// clang-format off
static int32_t SoundVolumeAdjust[RCT2SoundCount] =
{
    0,      // LiftClassic
    0,      // TrackFrictionClassicWood
    0,      // FrictionClassic
    0,      // Scream1
    0,      // Click1
    0,      // Click2
    0,      // PlaceItem
    0,      // Scream2
    0,      // Scream3
    0,      // Scream4
    0,      // Scream5
    0,      // Scream6
    0,      // LiftFrictionWheels
    -400,   // Purchase
    0,      // Crash
    0,      // LayingOutWater
    0,      // Water1
    0,      // Water2
    0,      // TrainWhistle
    0,      // TrainDeparting
    -1000,  // WaterSplash
    0,      // GoKartEngine
    -800,   // RideLaunch1
    -1700,  // RideLaunch2
    -700,   // Cough1
    -700,   // Cough2
    -700,   // Cough3
    -700,   // Cough4
    0,      // Rain
    0,      // Thunder1
    0,      // Thunder2
    0,      // TrackFrictionTrain
    0,      // TrackFrictionWater
    0,      // BalloonPop
    -700,   // MechanicFix
    0,      // Scream7
    -2500,  // ToiletFlush original value: -1000
    0,      // Click3
    0,      // Quack
    0,      // NewsItem
    0,      // WindowOpen
    -900,   // Laugh1
    -900,   // Laugh2
    -900,   // Laugh3
    0,      // Applause
    -600,   // HauntedHouseScare
    -700,   // HauntedHouseScream1
    -700,   // HauntedHouseScream2
    -2550,  // BlockBrakeClose
    -2900,  // BlockBrakeRelease
    0,      // Error
    -3400,  // BrakeRelease
    0,      // LiftArrow
    0,      // LiftWood
    0,      // TrackFrictionWood
    0,      // LiftWildMouse
    0,      // LiftBM
    0,      // TrackFrictionBM
    0,      // Scream8
    0,      // Tram
    -2000,  // DoorOpen
    -2700,  // DoorClose
    -700    // Portcullis
};
// clang-format on

static AudioParams audio_get_params_from_location(SoundId soundId, const CoordsXYZ& location);

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
    for (auto& device : devices)
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

    gAudioDeviceCount = static_cast<int32_t>(devices.size());
    gAudioDevices = Memory::AllocateArray<audio_device>(gAudioDeviceCount);
    for (int32_t i = 0; i < gAudioDeviceCount; i++)
    {
        auto device = &gAudioDevices[i];
        String::Set(device->name, sizeof(device->name), devices[i].c_str());
    }
}

void audio_play_sound_at_location(SoundId soundId, const CoordsXYZ& loc)
{
    if (gGameSoundsOff)
        return;

    AudioParams params = audio_get_params_from_location(soundId, loc);
    if (params.in_range)
    {
        audio_play_sound(soundId, params.volume, params.pan);
    }
}

/**
 * Returns the audio parameters to use when playing the specified sound at a virtual location.
 * @param soundId The sound effect to be played.
 * @param location The location at which the sound effect is to be played.
 * @return The audio parameters to be used when playing this sound effect.
 */
static AudioParams audio_get_params_from_location(SoundId soundId, const CoordsXYZ& location)
{
    int32_t volumeDown = 0;
    AudioParams params;
    params.in_range = true;
    params.volume = 0;
    params.pan = 0;

    auto element = map_get_surface_element_at(location);
    if (element && (element->GetBaseZ()) - 5 > location.z)
    {
        volumeDown = 10;
    }

    uint8_t rotation = get_current_rotation();
    auto pos2 = translate_3d_to_2d_with_z(rotation, location);

    rct_viewport* viewport = nullptr;
    while ((viewport = window_get_previous_viewport(viewport)) != nullptr)
    {
        if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
        {
            int16_t vx = pos2.x - viewport->viewPos.x;
            int16_t vy = pos2.y - viewport->viewPos.y;
            params.pan = viewport->pos.x + (vx / viewport->zoom);
            params.volume = SoundVolumeAdjust[static_cast<uint8_t>(soundId)]
                + ((-1024 * viewport->zoom - 1) * (1 << volumeDown)) + 1;

            if (vy < 0 || vy >= viewport->view_height || vx < 0 || vx >= viewport->view_width || params.volume < -10000)
            {
                params.in_range = false;
                return params;
            }
        }
    }

    return params;
}

void audio_play_sound(SoundId soundId, int32_t volume, int32_t pan)
{
    if (gGameSoundsOff)
        return;

    int32_t mixerPan = 0;
    if (pan != AUDIO_PLAY_AT_CENTRE)
    {
        int32_t x2 = pan << 16;
        uint16_t screenWidth = std::max<int32_t>(64, OpenRCT2::GetContext()->GetUiContext()->GetWidth());
        mixerPan = ((x2 / screenWidth) - 0x8000) >> 4;
    }

    Mixer_Play_Effect(soundId, MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(mixerPan), 1, 1);
}

void audio_start_title_music()
{
    if (gGameSoundsOff || !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) || gIntroState != IntroState::None)
    {
        audio_stop_title_music();
        return;
    }

    if (gTitleMusicChannel != nullptr)
    {
        return;
    }

    int32_t pathId;
    switch (gConfigSound.title_music)
    {
        case 1:
            pathId = PATH_ID_CSS50;
            break;
        case 2:
            pathId = PATH_ID_CSS17;
            break;
        case 3:
            pathId = (util_rand() & 1) ? PATH_ID_CSS50 : PATH_ID_CSS17;
            break;
        default:
            return;
    }

    gTitleMusicChannel = Mixer_Play_Music(pathId, MIXER_LOOP_INFINITE, true);
    if (gTitleMusicChannel != nullptr)
    {
        Mixer_Channel_SetGroup(gTitleMusicChannel, OpenRCT2::Audio::MixerGroup::TitleMusic);
    }
}

void audio_stop_ride_music()
{
    for (auto& rideMusic : gRideMusicList)
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

    for (auto& rideMusicInfo : gRideMusicInfoList)
    {
        const utf8* path = context_get_path_legacy(rideMusicInfo.path_id);
        if (File::Exists(path))
        {
            try
            {
                auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);
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
            catch (const std::exception&)
            {
            }
        }
    }
}

void audio_init_ride_sounds(int32_t device)
{
    audio_close();
    for (auto& vehicleSound : gVehicleSoundList)
    {
        vehicleSound.id = SOUND_ID_NULL;
    }

    gAudioCurrentDevice = device;
    config_save_default();
    for (auto& rideMusic : gRideMusicList)
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
    gConfigSound.master_sound_enabled = !gConfigSound.master_sound_enabled;
    if (gConfigSound.master_sound_enabled)
    {
        audio_unpause_sounds();
    }
    else
    {
        audio_stop_title_music();
        audio_pause_sounds();
    }

    window_invalidate_by_class(WC_OPTIONS);
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

    for (auto& vehicleSound : gVehicleSoundList)
    {
        if (vehicleSound.id != SOUND_ID_NULL)
        {
            vehicleSound.id = SOUND_ID_NULL;
            if (vehicleSound.TrackSound.Id != SoundId::Null)
            {
                Mixer_Stop_Channel(vehicleSound.TrackSound.Channel);
            }
            if (vehicleSound.OtherSound.Id != SoundId::Null)
            {
                Mixer_Stop_Channel(vehicleSound.OtherSound.Channel);
            }
        }
    }
}
