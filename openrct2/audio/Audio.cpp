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
#include "../core/FileStream.h"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../entity/Peep.h"
#include "../interface/Viewport.h"
#include "../localisation/Language.h"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"
#include "../ride/RideAudio.h"
#include "../ui/UiContext.h"
#include "../util/Util.h"
#include "AudioContext.h"
#include "AudioMixer.h"

#include <algorithm>
#include <vector>

namespace OpenRCT2::Audio
{
    struct AudioParams
    {
        bool in_range;
        int32_t volume;
        int32_t pan;
    };

    static std::vector<std::string> _audioDevices;
    static int32_t _currentAudioDevice = -1;

    bool gGameSoundsOff = false;
    int32_t gVolumeAdjustZoom = 0;

    void* gTitleMusicChannel = nullptr;
    void* gWeatherSoundChannel = nullptr;

    VehicleSound gVehicleSoundList[MaxVehicleSounds];

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

    static AudioParams GetParametersFromLocation(SoundId soundId, const CoordsXYZ& location);

    bool IsAvailable()
    {
        if (_currentAudioDevice == -1)
            return false;
        if (gGameSoundsOff)
            return false;
        if (!gConfigSound.sound_enabled)
            return false;
        if (gOpenRCT2Headless)
            return false;
        return true;
    }

    void Init()
    {
        if (str_is_null_or_empty(gConfigSound.device))
        {
            Mixer_Init(nullptr);
            _currentAudioDevice = 0;
        }
        else
        {
            Mixer_Init(gConfigSound.device);

            PopulateDevices();
            for (int32_t i = 0; i < GetDeviceCount(); i++)
            {
                if (_audioDevices[i] == gConfigSound.device)
                {
                    _currentAudioDevice = i;
                }
            }
        }
    }

    void PopulateDevices()
    {
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

        _audioDevices = devices;
    }

    void Play3D(SoundId soundId, const CoordsXYZ& loc)
    {
        if (!IsAvailable())
            return;

        AudioParams params = GetParametersFromLocation(soundId, loc);
        if (params.in_range)
        {
            Play(soundId, params.volume, params.pan);
        }
    }

    /**
     * Returns the audio parameters to use when playing the specified sound at a virtual location.
     * @param soundId The sound effect to be played.
     * @param location The location at which the sound effect is to be played.
     * @return The audio parameters to be used when playing this sound effect.
     */
    static AudioParams GetParametersFromLocation(SoundId soundId, const CoordsXYZ& location)
    {
        int32_t volumeDown = 0;
        AudioParams params;
        params.in_range = true;
        params.volume = 0;
        params.pan = 0;

        auto element = map_get_surface_element_at(location);
        if (element != nullptr && (element->GetBaseZ()) - 5 > location.z)
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
                params.pan = viewport->pos.x + viewport->zoom.ApplyInversedTo(vx);
                params.volume = SoundVolumeAdjust[static_cast<uint8_t>(soundId)]
                    + ((viewport->zoom.ApplyTo(-1024) - 1) * (1 << volumeDown)) + 1;

                if (!viewport->Contains(pos2) || params.volume < -10000)
                {
                    params.in_range = false;
                    return params;
                }
            }
        }

        return params;
    }

    void Play(SoundId soundId, int32_t volume, int32_t pan)
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

    void PlayTitleMusic()
    {
        if (gGameSoundsOff || !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) || gIntroState != IntroState::None)
        {
            StopTitleMusic();
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

    void StopAll()
    {
        StopTitleMusic();
        StopVehicleSounds();
        RideAudio::StopAllChannels();
        peep_stop_crowd_noise();
        StopWeatherSound();
    }

    int32_t GetDeviceCount()
    {
        return static_cast<int32_t>(_audioDevices.size());
    }

    const std::string& GetDeviceName(int32_t index)
    {
        if (index < 0 || index >= GetDeviceCount())
        {
            static std::string InvalidDevice = "Invalid Device";
            return InvalidDevice;
        }
        return _audioDevices[index];
    }

    int32_t GetCurrentDeviceIndex()
    {
        return _currentAudioDevice;
    }

    void StopTitleMusic()
    {
        if (gTitleMusicChannel != nullptr)
        {
            Mixer_Stop_Channel(gTitleMusicChannel);
            gTitleMusicChannel = nullptr;
        }
    }

    void StopWeatherSound()
    {
        if (gWeatherSoundChannel != nullptr)
        {
            Mixer_Stop_Channel(gWeatherSoundChannel);
            gWeatherSoundChannel = nullptr;
        }
    }

    void InitRideSoundsAndInfo()
    {
        InitRideSounds(0);
    }

    void InitRideSounds(int32_t device)
    {
        Close();
        for (auto& vehicleSound : gVehicleSoundList)
        {
            vehicleSound.id = SoundIdNull;
        }

        _currentAudioDevice = device;
        config_save_default();
    }

    void Close()
    {
        peep_stop_crowd_noise();
        StopTitleMusic();
        RideAudio::StopAllChannels();
        StopWeatherSound();
        _currentAudioDevice = -1;
    }

    void ToggleAllSounds()
    {
        gConfigSound.master_sound_enabled = !gConfigSound.master_sound_enabled;
        if (gConfigSound.master_sound_enabled)
        {
            Resume();
            PlayTitleMusic();
        }
        else
        {
            StopTitleMusic();
            Pause();
        }

        window_invalidate_by_class(WC_OPTIONS);
    }

    void Pause()
    {
        gGameSoundsOff = true;
        StopVehicleSounds();
        RideAudio::StopAllChannels();
        peep_stop_crowd_noise();
        StopWeatherSound();
    }

    void Resume()
    {
        gGameSoundsOff = false;
    }

    void StopVehicleSounds()
    {
        if (!IsAvailable())
            return;

        for (auto& vehicleSound : gVehicleSoundList)
        {
            if (vehicleSound.id != SoundIdNull)
            {
                vehicleSound.id = SoundIdNull;
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

} // namespace OpenRCT2::Audio
