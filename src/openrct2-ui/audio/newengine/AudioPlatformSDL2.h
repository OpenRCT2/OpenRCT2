/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "AudioEngine.h"

#include <SDL.h>
#include <cstdint>
#include <string>
#include <vector>

namespace OpenRCT2::Audio
{
    class AudioPlatformSDL2
    {
    public:
        AudioPlatformSDL2(AudioEngine& engine);
        ~AudioPlatformSDL2();

        bool open(const char* deviceName);
        void close();
        void pause();
        void unpause();

        [[nodiscard]] bool isOpen() const;
        [[nodiscard]] uint32_t getSampleRate() const;
        [[nodiscard]] AudioSampleFormat getFormat() const;
        [[nodiscard]] uint8_t getChannels() const;
        [[nodiscard]] uint16_t getBufferSamples() const;
        [[nodiscard]] const std::string& getCurrentDeviceName() const;
        [[nodiscard]] SDL_AudioDeviceID getDeviceId() const;

        static std::vector<std::string> enumerateDevices();
        static AudioSampleFormat mapSDLFormat(uint16_t sdlFormat);

    private:
        static void sdlCallback(void* userdata, uint8_t* stream, int length);

        AudioEngine& _engine;
        SDL_AudioDeviceID _deviceId = 0;
        uint32_t _sampleRate = 0;
        AudioSampleFormat _format = AudioSampleFormat::unknown;
        uint16_t _sdlFormat = 0;
        uint8_t _channels = 0;
        uint16_t _bufferSamples = 0;
        size_t _renderBufferCapacity = 0;

        std::string _currentDeviceName;

        std::vector<float> _renderBuffer;
    };

} // namespace OpenRCT2::Audio
