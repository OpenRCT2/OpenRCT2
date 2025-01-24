/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "audio.h"

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../config/Config.h"
#include "../core/EnumUtils.hpp"
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
#include "../scenes/intro/IntroScene.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/tile_element/SurfaceElement.h"
#include "AudioChannel.h"
#include "AudioContext.h"
#include "AudioMixer.h"

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
    static ObjectEntryIndex _soundsAudioObjectEntryIndex = kObjectEntryIndexNull;
    static ObjectEntryIndex _soundsAdditionalAudioObjectEntryIndex = kObjectEntryIndexNull;
    static ObjectEntryIndex _titleAudioObjectEntryIndex = kObjectEntryIndexNull;

    bool gGameSoundsOff = false;
    int32_t gVolumeAdjustZoom = 0;

    static std::shared_ptr<IAudioChannel> _titleMusicChannel = nullptr;

    VehicleSound gVehicleSoundList[kMaxVehicleSounds];

    bool IsAvailable()
    {
        if (_currentAudioDevice == -1)
            return false;
        if (gGameSoundsOff)
            return false;
        if (!Config::Get().sound.SoundEnabled)
            return false;
        if (gOpenRCT2Headless)
            return false;
        return true;
    }

    void Init()
    {
        auto audioContext = GetContext()->GetAudioContext();
        if (Config::Get().sound.Device.empty())
        {
            audioContext->SetOutputDevice("");
            _currentAudioDevice = 0;
        }
        else
        {
            audioContext->SetOutputDevice(Config::Get().sound.Device);

            PopulateDevices();
            for (int32_t i = 0; i < GetDeviceCount(); i++)
            {
                if (_audioDevices[i] == Config::Get().sound.Device)
                {
                    _currentAudioDevice = i;
                }
            }
        }
    }

    void LoadAudioObjects()
    {
        auto& objManager = GetContext()->GetObjectManager();

        Object* baseAudio = objManager.LoadObject(AudioObjectIdentifiers::kRCT2);
        if (baseAudio != nullptr)
        {
            _soundsAudioObjectEntryIndex = objManager.GetLoadedObjectEntryIndex(baseAudio);
        }

        objManager.LoadObject(AudioObjectIdentifiers::kOpenRCT2Additional);
        _soundsAdditionalAudioObjectEntryIndex = objManager.GetLoadedObjectEntryIndex(
            AudioObjectIdentifiers::kOpenRCT2Additional);
        objManager.LoadObject(AudioObjectIdentifiers::kRCT2Circus);
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
                device = LanguageGetString(STR_OPTIONS_SOUND_VALUE_DEFAULT);
            }
        }

#ifndef __linux__
        // The first device is always system default on Windows and macOS
        std::string defaultDevice = LanguageGetString(STR_OPTIONS_SOUND_VALUE_DEFAULT);
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

        uint8_t rotation = GetCurrentRotation();
        auto pos2 = Translate3DTo2DWithZ(rotation, location);

        Viewport* viewport = nullptr;
        while ((viewport = WindowGetPreviousViewport(viewport)) != nullptr)
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
        if (pan != kAudioPlayAtCentre)
        {
            int32_t x2 = pan << 16;
            uint16_t screenWidth = std::max<int32_t>(64, ContextGetWidth());
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

    static bool IsRCT1TitleMusicAvailable()
    {
        auto env = GetContext()->GetPlatformEnvironment();
        auto rct1path = env->GetDirectoryPath(DIRBASE::RCT1);
        return !rct1path.empty();
    }

    static std::map<TitleMusicKind, std::string_view> GetAvailableMusicMap()
    {
        auto musicMap = std::map<TitleMusicKind, std::string_view>{
            { TitleMusicKind::OpenRCT2, AudioObjectIdentifiers::kOpenRCT2Title },
            { TitleMusicKind::RCT2, AudioObjectIdentifiers::kRCT2Title },
        };

        if (IsRCT1TitleMusicAvailable())
        {
            musicMap.emplace(TitleMusicKind::RCT1, AudioObjectIdentifiers::kRCT1Title);
        }

        return musicMap;
    }

    static ObjectEntryDescriptor GetTitleMusicDescriptor(TitleMusicKind musicKind)
    {
        auto musicMap = GetAvailableMusicMap();
        auto it = musicMap.find(musicKind);
        if (musicKind == TitleMusicKind::Random)
        {
            it = std::next(musicMap.begin(), UtilRand() % musicMap.size());
        }

        if (it != musicMap.end())
        {
            return ObjectEntryDescriptor(ObjectType::Audio, it->second);
        }

        // No music descriptor for the current setting, intentional for TitleMusicKind::None
        return {};
    }

    void PlayTitleMusic()
    {
        if (gGameSoundsOff || !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) || IntroIsPlaying())
        {
            StopTitleMusic();
            return;
        }

        if (_titleMusicChannel != nullptr && !_titleMusicChannel->IsDone())
        {
            return;
        }

        // Load title sequence audio object
        auto descriptor = GetTitleMusicDescriptor(Config::Get().sound.TitleMusic);
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
        PeepStopCrowdNoise();
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
        if (_titleAudioObjectEntryIndex != kObjectEntryIndexNull)
        {
            auto& objManager = GetContext()->GetObjectManager();
            auto* obj = objManager.GetLoadedObject(ObjectType::Audio, _titleAudioObjectEntryIndex);
            if (obj != nullptr)
            {
                objManager.UnloadObjects({ obj->GetDescriptor() });
            }
            _titleAudioObjectEntryIndex = kObjectEntryIndexNull;
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
            vehicleSound.id = kSoundIdNull;
        }

        _currentAudioDevice = device;
        Config::Save();
    }

    void Close()
    {
        PeepStopCrowdNoise();
        StopTitleMusic();
        RideAudio::StopAllChannels();
        ClimateStopWeatherSound();
        _currentAudioDevice = -1;
    }

    void ToggleAllSounds()
    {
        Config::Get().sound.MasterSoundEnabled = !Config::Get().sound.MasterSoundEnabled;
        if (Config::Get().sound.MasterSoundEnabled)
        {
            Resume();
        }
        else
        {
            Pause();
        }

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::Options);
    }

    void Pause()
    {
        gGameSoundsOff = true;
        StopVehicleSounds();
        RideAudio::StopAllChannels();
        PeepStopCrowdNoise();
        ClimateStopWeatherSound();
        StopTitleMusic();
    }

    void Resume()
    {
        gGameSoundsOff = false;
        PlayTitleMusic();
    }

    void StopVehicleSounds()
    {
        if (!IsAvailable())
            return;

        for (auto& vehicleSound : gVehicleSoundList)
        {
            if (vehicleSound.id != kSoundIdNull)
            {
                vehicleSound.id = kSoundIdNull;
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
        auto channel = mixer->Play(source, loop ? kMixerLoopInfinite : kMixerLoopNone, forget);
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
        return static_cast<int32_t>(kMixerVolumeMax * (std::pow(10.0f, static_cast<float>(volume) / 2000)));
    }

    float DStoMixerPan(int32_t pan)
    {
        constexpr int32_t kDSBPanLeft = -10000;
        constexpr int32_t kDSBPanRight = 10000;
        return ((static_cast<float>(pan) + -kDSBPanLeft) / kDSBPanRight) / 2;
    }

    double DStoMixerRate(int32_t frequency)
    {
        return static_cast<double>(frequency) / 22050;
    }

} // namespace OpenRCT2::Audio
