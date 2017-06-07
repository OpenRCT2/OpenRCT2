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

#include <openrct2/common.h>
#include <SDL.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/core/String.hpp>
#include "../SDLException.h"
#include "AudioContext.h"

namespace OpenRCT2 { namespace Audio
{
    class AudioContext : public IAudioContext
    {
    private:
        IAudioMixer * _audioMixer;

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
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
        }

        IAudioMixer * GetMixer() override
        {
            return _audioMixer;
        }

        std::vector<std::string> GetOutputDevices() override
        {
            std::vector<std::string> devices;
            sint32 numDevices = SDL_GetNumAudioDevices(SDL_FALSE);
            for (sint32 i = 0; i < numDevices; i++)
            {
                std::string deviceName = String::ToStd(SDL_GetAudioDeviceName(i, SDL_FALSE));
                devices.push_back(deviceName);
            }
            return devices;
        }

        void SetOutputDevice(const std::string &deviceName) override
        {
            const char * szDeviceName = nullptr;
            if (!deviceName.empty())
            {
                szDeviceName = deviceName.c_str();
            }
            _audioMixer->Init(szDeviceName);
        }

        IAudioSource * CreateStreamFromWAV(const std::string &path) override
        {
            return AudioSource::CreateStreamFromWAV(path);
        }

        void StartTitleMusic() override { }

        IAudioChannel * PlaySound(sint32 soundId, sint32 volume, sint32 pan) override { return nullptr; }
        IAudioChannel * PlaySoundAtLocation(sint32 soundId, sint16 x, sint16 y, sint16 z) override { return nullptr; }
        IAudioChannel * PlaySoundPanned(sint32 soundId, sint32 pan, sint16 x, sint16 y, sint16 z) override { return nullptr; }

        void ToggleAllSounds() override { }
        void PauseSounds() override { }
        void UnpauseSounds() override { }

        void StopAll() override { }
        void StopCrowdSound() override { }
        void StopRainSound() override { }
        void StopRideMusic() override { }
        void StopTitleMusic() override { }
        void StopVehicleSounds() override { }
    };

    IAudioContext * CreateAudioContext()
    {
        return new AudioContext();
    }
} }
