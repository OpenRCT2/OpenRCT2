/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "../PlatformEnvironment.h"
#include "../config/Config.h"
#include "../core/File.h"
#include "../core/FileStream.h"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../entity/Peep.h"
#include "../interface/Viewport.h"
#include "../localisation/Language.h"
#include "../localisation/StringIds.h"
#include "../object/AudioObject.h"
#include "../object/ObjectManager.h"
#include "../ride/Ride.h"
#include "../ride/RideAudio.h"
#include "../ui/UiContext.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "AudioChannel.h"
#include "AudioContext.h"
#include "AudioMixer.h"

#include <algorithm>
#include <cmath>
#include <memory>
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
    static ObjectEntryIndex _soundsAudioObjectEntryIndex = OBJECT_ENTRY_INDEX_NULL;
    static ObjectEntryIndex _soundsAdditionalAudioObjectEntryIndex = OBJECT_ENTRY_INDEX_NULL;
    static ObjectEntryIndex _titleAudioObjectEntryIndex = OBJECT_ENTRY_INDEX_NULL;

    bool gGameSoundsOff = false;
    int32_t gVolumeAdjustZoom = 0;

    static std::shared_ptr<IAudioChannel> _titleMusicChannel = nullptr;

    VehicleSound gVehicleSoundList[MaxVehicleSounds];

    bool IsAvailable()
    {
        if (_currentAudioDevice == -1)
            return false;
        if (gGameSoundsOff)
            return false;
        if (!gConfigSound.SoundEnabled)
            return false;
        if (gOpenRCT2Headless)
            return false;
        return true;
    }

    void Init()
    {
        auto audioContext = GetContext()->GetAudioContext();
        if (gConfigSound.Device.empty())
        {
            audioContext->SetOutputDevice("");
            _currentAudioDevice = 0;
        }
        else
        {
            audioContext->SetOutputDevice(gConfigSound.Device);

            PopulateDevices();
            for (int32_t i = 0; i < GetDeviceCount(); i++)
            {
                if (_audioDevices[i] == gConfigSound.Device)
                {
                    _currentAudioDevice = i;
                }
            }
        }
        LoadAudioObjects();
    }

    void LoadAudioObjects()
    {
        auto& objManager = GetContext()->GetObjectManager();

        Object* baseAudio{};

        // We have a different audio object for RCT Classic
        auto env = GetContext()->GetPlatformEnvironment();
        if (env->IsUsingClassic())
        {
            baseAudio = objManager.LoadObject(AudioObjectIdentifiers::Rct2cBase);
            if (baseAudio != nullptr)
            {
                _soundsAudioObjectEntryIndex = objManager.GetLoadedObjectEntryIndex(baseAudio);
            }
        }

        if (baseAudio == nullptr)
        {
            // Fallback to vanilla RCT2 audio object
            baseAudio = objManager.LoadObject(AudioObjectIdentifiers::Rct2Base);
            if (baseAudio != nullptr)
            {
                _soundsAudioObjectEntryIndex = objManager.GetLoadedObjectEntryIndex(baseAudio);
            }
        }

        objManager.LoadObject(AudioObjectIdentifiers::OpenRCT2Additional);
        _soundsAdditionalAudioObjectEntryIndex = objManager.GetLoadedObjectEntryIndex(
            AudioObjectIdentifiers::OpenRCT2Additional);
        objManager.LoadObject(AudioObjectIdentifiers::Rct2Circus);
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

    /**
     * Returns the audio parameters to use when playing the specified sound at a virtual location.
     * @param soundId The sound effect to be played.
     * @param location The location at which the sound effect is to be played.
     * @return The audio parameters to be used when playing this sound effect.
     */
    static AudioParams GetParametersFromLocation(AudioObject* obj, uint32_t sampleIndex, const CoordsXYZ& location)
    {
        int32_t volumeDown = 0;
        AudioParams params;
        params.in_range = true;
        params.volume = 0;
        params.pan = 0;

        auto element = MapGetSurfaceElementAt(location);
        if (element != nullptr && (element->GetBaseZ()) - 5 > location.z)
        {
            volumeDown = 10;
        }

        uint8_t rotation = get_current_rotation();
        auto pos2 = Translate3DTo2DWithZ(rotation, location);

        rct_viewport* viewport = nullptr;
        while ((viewport = window_get_previous_viewport(viewport)) != nullptr)
        {
            if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
            {
                int16_t vx = pos2.x - viewport->viewPos.x;
                params.pan = viewport->pos.x + viewport->zoom.ApplyInversedTo(vx);

                auto sampleModifier = obj->GetSampleModifier(sampleIndex);
                auto viewModifier = ((viewport->zoom.ApplyTo(-1024) - 1) * (1 << volumeDown)) + 1;
                params.volume = sampleModifier + viewModifier;

                if (!viewport->Contains(pos2) || params.volume < -10000)
                {
                    params.in_range = false;
                    return params;
                }
            }
        }

        return params;
    }

    static std::tuple<AudioObject*, uint32_t> GetAudioObjectAndSampleIndex(SoundId id)
    {
        auto& objManager = GetContext()->GetObjectManager();
        AudioObject* audioObject{};
        uint32_t sampleIndex = EnumValue(id);
        if (id >= SoundId::LiftRMC)
        {
            audioObject = static_cast<AudioObject*>(
                objManager.GetLoadedObject(ObjectType::Audio, _soundsAdditionalAudioObjectEntryIndex));
            sampleIndex -= EnumValue(SoundId::LiftRMC);
        }
        else
        {
            audioObject = static_cast<AudioObject*>(
                objManager.GetLoadedObject(ObjectType::Audio, _soundsAudioObjectEntryIndex));
        }
        return std::make_tuple(audioObject, sampleIndex);
    }

    static void Play(IAudioSource* audioSource, int32_t volume, int32_t pan)
    {
        int32_t mixerPan = 0;
        if (pan != AUDIO_PLAY_AT_CENTRE)
        {
            int32_t x2 = pan << 16;
            uint16_t screenWidth = std::max<int32_t>(64, OpenRCT2::GetContext()->GetUiContext()->GetWidth());
            mixerPan = ((x2 / screenWidth) - 0x8000) >> 4;
        }

        CreateAudioChannel(audioSource, MixerGroup::Sound, false, DStoMixerVolume(volume), DStoMixerPan(mixerPan), 1, true);
    }

    void Play3D(SoundId soundId, const CoordsXYZ& loc)
    {
        if (!IsAvailable())
            return;

        // Get sound from base object
        auto [baseAudioObject, sampleIndex] = GetAudioObjectAndSampleIndex(soundId);
        if (baseAudioObject != nullptr)
        {
            auto params = GetParametersFromLocation(baseAudioObject, sampleIndex, loc);
            if (params.in_range)
            {
                auto source = baseAudioObject->GetSample(sampleIndex);
                if (source != nullptr)
                {
                    Play(source, params.volume, params.pan);
                }
            }
        }
    }

    void Play(SoundId soundId, int32_t volume, int32_t pan)
    {
        if (!IsAvailable())
            return;

        // Get sound from base object
        auto [baseAudioObject, sampleIndex] = GetAudioObjectAndSampleIndex(soundId);
        if (baseAudioObject != nullptr)
        {
            auto source = baseAudioObject->GetSample(sampleIndex);
            if (source != nullptr)
            {
                Play(source, volume, pan);
            }
        }
    }

    static ObjectEntryDescriptor GetTitleMusicDescriptor()
    {
        switch (gConfigSound.TitleMusic)
        {
            default:
                return {};
            case TitleMusicKind::Rct1:
                return ObjectEntryDescriptor(ObjectType::Audio, AudioObjectIdentifiers::Rct1Title);
            case TitleMusicKind::Rct2:
                return ObjectEntryDescriptor(ObjectType::Audio, AudioObjectIdentifiers::Rct2Title);
            case TitleMusicKind::Random:
                return ObjectEntryDescriptor(
                    ObjectType::Audio,
                    (util_rand() & 1) ? AudioObjectIdentifiers::Rct1Title : AudioObjectIdentifiers::Rct2Title);
        }
    }

    void PlayTitleMusic()
    {
        if (gGameSoundsOff || !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) || gIntroState != IntroState::None)
        {
            StopTitleMusic();
            return;
        }

        if (_titleMusicChannel != nullptr && !_titleMusicChannel->IsDone())
        {
            return;
        }

        // Load title sequence audio object
        auto descriptor = GetTitleMusicDescriptor();
        auto& objManager = GetContext()->GetObjectManager();
        auto* audioObject = static_cast<AudioObject*>(objManager.LoadObject(descriptor));
        if (audioObject != nullptr)
        {
            _titleAudioObjectEntryIndex = objManager.GetLoadedObjectEntryIndex(audioObject);

            // Play first sample from object
            auto source = audioObject->GetSample(0);
            if (source != nullptr)
            {
                _titleMusicChannel = CreateAudioChannel(source, MixerGroup::TitleMusic, true);
            }
        }
    }

    void StopAll()
    {
        StopTitleMusic();
        StopVehicleSounds();
        RideAudio::StopAllChannels();
        peep_stop_crowd_noise();
        ClimateStopWeatherSound();
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
        if (_titleMusicChannel != nullptr)
        {
            _titleMusicChannel->Stop();
            _titleMusicChannel = nullptr;
        }

        // Unload the audio object
        if (_titleAudioObjectEntryIndex != OBJECT_ENTRY_INDEX_NULL)
        {
            auto& objManager = GetContext()->GetObjectManager();
            auto* obj = objManager.GetLoadedObject(ObjectType::Audio, _titleAudioObjectEntryIndex);
            if (obj != nullptr)
            {
                objManager.UnloadObjects({ obj->GetDescriptor() });
            }
            _titleAudioObjectEntryIndex = OBJECT_ENTRY_INDEX_NULL;
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
        ConfigSaveDefault();
    }

    void Close()
    {
        peep_stop_crowd_noise();
        StopTitleMusic();
        RideAudio::StopAllChannels();
        ClimateStopWeatherSound();
        _currentAudioDevice = -1;
    }

    void ToggleAllSounds()
    {
        gConfigSound.MasterSoundEnabled = !gConfigSound.MasterSoundEnabled;
        if (gConfigSound.MasterSoundEnabled)
        {
            Resume();
            PlayTitleMusic();
        }
        else
        {
            StopTitleMusic();
            Pause();
        }

        window_invalidate_by_class(WindowClass::Options);
    }

    void Pause()
    {
        gGameSoundsOff = true;
        StopVehicleSounds();
        RideAudio::StopAllChannels();
        peep_stop_crowd_noise();
        ClimateStopWeatherSound();
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
                    vehicleSound.TrackSound.Channel->Stop();
                }
                if (vehicleSound.OtherSound.Id != SoundId::Null)
                {
                    vehicleSound.OtherSound.Channel->Stop();
                }
            }
        }
    }

    static IAudioMixer* GetMixer()
    {
        auto audioContext = GetContext()->GetAudioContext();
        return audioContext->GetMixer();
    }

    std::shared_ptr<IAudioChannel> CreateAudioChannel(
        SoundId id, bool loop, int32_t volume, float pan, double rate, bool forget)
    {
        // Get sound from base object
        auto [baseAudioObject, sampleIndex] = GetAudioObjectAndSampleIndex(id);
        if (baseAudioObject != nullptr)
        {
            auto source = baseAudioObject->GetSample(sampleIndex);
            if (source != nullptr)
            {
                return CreateAudioChannel(source, MixerGroup::Sound, loop, volume, pan, rate, forget);
            }
        }
        return nullptr;
    }

    std::shared_ptr<IAudioChannel> CreateAudioChannel(
        IAudioSource* source, MixerGroup group, bool loop, int32_t volume, float pan, double rate, bool forget)
    {
        auto* mixer = GetMixer();
        if (mixer == nullptr)
        {
            return nullptr;
        }

        mixer->Lock();
        auto channel = mixer->Play(source, loop ? MIXER_LOOP_INFINITE : MIXER_LOOP_NONE, forget);
        if (channel != nullptr)
        {
            channel->SetGroup(group);
            channel->SetVolume(volume);
            channel->SetPan(pan);
            channel->SetRate(rate);
            channel->UpdateOldVolume();
        }
        mixer->Unlock();
        return channel;
    }

    int32_t DStoMixerVolume(int32_t volume)
    {
        return static_cast<int32_t>(MIXER_VOLUME_MAX * (std::pow(10.0f, static_cast<float>(volume) / 2000)));
    }

    float DStoMixerPan(int32_t pan)
    {
        constexpr int32_t DSBPAN_LEFT = -10000;
        constexpr int32_t DSBPAN_RIGHT = 10000;
        return ((static_cast<float>(pan) + -DSBPAN_LEFT) / DSBPAN_RIGHT) / 2;
    }

    double DStoMixerRate(int32_t frequency)
    {
        return static_cast<double>(frequency) / 22050;
    }

} // namespace OpenRCT2::Audio
