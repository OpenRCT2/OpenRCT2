/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioContext.h"

#include "../SDLException.h"

#include <SDL.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/common.h>
#include <openrct2/core/String.hpp>

namespace OpenRCT2::Audio
{
    class AudioContext final : public IAudioContext
    {
    private:
        IAudioMixer* _audioMixer = nullptr;

    public:
        AudioContext()
        {
            if (SDL_Init(SDL_INIT_AUDIO) < 0)
            {
                SDLException::Throw("SDL_Init(SDL_INIT_AUDIO)");
            }
            _audioMixer = AudioMixer::Create();
        }

        ~AudioContext() override
        {
            delete _audioMixer;
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
        }

        IAudioMixer* GetMixer() override
        {
            return _audioMixer;
        }

        std::vector<std::string> GetOutputDevices() override
        {
            std::vector<std::string> devices;
            int32_t numDevices = SDL_GetNumAudioDevices(SDL_FALSE);
            for (int32_t i = 0; i < numDevices; i++)
            {
                devices.emplace_back(String::ToStd(SDL_GetAudioDeviceName(i, SDL_FALSE)));
            }
            return devices;
        }

        void SetOutputDevice(const std::string& deviceName) override
        {
            const char* szDeviceName = nullptr;
            if (!deviceName.empty())
            {
                szDeviceName = deviceName.c_str();
            }
            _audioMixer->Init(szDeviceName);
        }

        IAudioSource* CreateStreamFromWAV(const std::string& path) override
        {
            return AudioSource::CreateStreamFromWAV(path);
        }

        IAudioSource* CreateStreamFromWAV(std::unique_ptr<IStream> stream) override
        {
            return AudioSource::CreateStreamFromWAV(std::move(stream));
        }

        void StartTitleMusic() override
        {
        }

        IAudioChannel* PlaySound(int32_t soundId, int32_t volume, int32_t pan) override
        {
            return nullptr;
        }
        IAudioChannel* PlaySoundAtLocation(int32_t soundId, int16_t x, int16_t y, int16_t z) override
        {
            return nullptr;
        }
        IAudioChannel* PlaySoundPanned(int32_t soundId, int32_t pan, int16_t x, int16_t y, int16_t z) override
        {
            return nullptr;
        }

        void ToggleAllSounds() override
        {
        }
        void PauseSounds() override
        {
        }
        void UnpauseSounds() override
        {
        }

        void StopAll() override
        {
        }
        void StopCrowdSound() override
        {
        }
        void StopWeatherSound() override
        {
        }
        void StopRideMusic() override
        {
        }
        void StopTitleMusic() override
        {
        }
        void StopVehicleSounds() override
        {
        }
    };

    std::unique_ptr<IAudioContext> CreateAudioContext()
    {
        return std::make_unique<AudioContext>();
    }
} // namespace OpenRCT2::Audio
