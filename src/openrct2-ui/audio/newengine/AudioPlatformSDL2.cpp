/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioPlatformSDL2.h"

#include <cstring>
#include <openrct2/Diagnostic.h>
#include <openrct2/core/String.hpp>

namespace OpenRCT2::Audio
{
    AudioPlatformSDL2::AudioPlatformSDL2(AudioEngine& engine)
        : _engine(engine)
    {
    }

    AudioPlatformSDL2::~AudioPlatformSDL2()
    {
        close();
    }

    bool AudioPlatformSDL2::open(const char* deviceName)
    {
        close();

        SDL_AudioSpec want{};
        want.freq = 48000;
        want.format = AUDIO_F32SYS;
        want.channels = 2;
        want.samples = 1024;
        want.callback = sdlCallback;
        want.userdata = this;

        SDL_AudioSpec have{};
        _deviceId = SDL_OpenAudioDevice(
            deviceName, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_SAMPLES_CHANGE);

        if (_deviceId == 0)
        {
            LOG_ERROR("Failed to open audio device: %s", SDL_GetError());
            _currentDeviceName.clear();
            return false;
        }

        _sampleRate = have.freq;
        _sdlFormat = have.format;
        _format = mapSDLFormat(have.format);
        _channels = have.channels;
        _bufferSamples = have.samples;

        if (deviceName != nullptr && deviceName[0] != '\0')
        {
            _currentDeviceName = deviceName;
        }
        else
        {
            _currentDeviceName.clear();
        }

        _renderBufferCapacity = static_cast<size_t>(_bufferSamples) * 2 * sizeof(float);
        _renderBuffer.resize(_renderBufferCapacity);

        LOG_VERBOSE(
            "New audio engine opened: %d Hz, format 0x%04X, %d ch, %d samples, device='%s'", _sampleRate, _sdlFormat, _channels,
            _bufferSamples, _currentDeviceName.empty() ? "(default)" : _currentDeviceName.c_str());

        SDL_PauseAudioDevice(_deviceId, 0);
        return true;
    }

    void AudioPlatformSDL2::close()
    {
        if (_deviceId != 0)
        {
            SDL_CloseAudioDevice(_deviceId);
            _deviceId = 0;
        }
        _currentDeviceName.clear();
        _renderBuffer.clear();
        _renderBuffer.shrink_to_fit();
    }

    void AudioPlatformSDL2::pause()
    {
        if (_deviceId != 0)
            SDL_PauseAudioDevice(_deviceId, 1);
    }

    void AudioPlatformSDL2::unpause()
    {
        if (_deviceId != 0)
            SDL_PauseAudioDevice(_deviceId, 0);
    }

    bool AudioPlatformSDL2::isOpen() const
    {
        return _deviceId != 0;
    }

    uint32_t AudioPlatformSDL2::getSampleRate() const
    {
        return _sampleRate;
    }

    AudioSampleFormat AudioPlatformSDL2::getFormat() const
    {
        return _format;
    }

    uint8_t AudioPlatformSDL2::getChannels() const
    {
        return _channels;
    }

    uint16_t AudioPlatformSDL2::getBufferSamples() const
    {
        return _bufferSamples;
    }

    const std::string& AudioPlatformSDL2::getCurrentDeviceName() const
    {
        return _currentDeviceName;
    }

    SDL_AudioDeviceID AudioPlatformSDL2::getDeviceId() const
    {
        return _deviceId;
    }

    bool AudioPlatformSDL2::handleDeviceEvent(uint32_t eventType, uint32_t deviceIndex, bool isCapture)
    {
        if (isCapture)
            return false;

        if (eventType == SDL_AUDIODEVICEREMOVED)
        {
            SDL_AudioDeviceID removedId = static_cast<SDL_AudioDeviceID>(deviceIndex);

            if (removedId == _deviceId)
            {
                LOG_WARNING("Audio device disconnected (ID %u)", removedId);

                close();

                std::string newDeviceName;
                bool reopened = false;

                if (_autoReconnect)
                {
                    LOG_INFO("Attempting to reconnect to default audio device...");
                    reopened = open(nullptr);
                    if (reopened)
                    {
                        LOG_INFO("Successfully reconnected to default audio device");
                        newDeviceName = _currentDeviceName;
                    }
                    else
                    {
                        LOG_ERROR("Failed to reconnect to any audio device");
                    }
                }

                if (_deviceChangeCallback)
                {
                    _deviceChangeCallback(true, newDeviceName);
                }

                return true;
            }
        }
        else if (eventType == SDL_AUDIODEVICEADDED)
        {
            const char* name = SDL_GetAudioDeviceName(static_cast<int>(deviceIndex), SDL_FALSE);
            std::string deviceName = name ? String::toStd(name) : "";

            LOG_VERBOSE("Audio device added: '%s'", deviceName.c_str());

            if (!isOpen() && _autoReconnect)
            {
                LOG_INFO("No audio device open, attempting to open newly added device...");
                if (open(name))
                {
                    LOG_INFO("Successfully opened newly added audio device: '%s'", deviceName.c_str());
                    if (_deviceChangeCallback)
                    {
                        _deviceChangeCallback(false, deviceName);
                    }
                    return true;
                }
            }

            if (_deviceChangeCallback)
            {
                _deviceChangeCallback(false, deviceName);
            }
        }

        return false;
    }

    void AudioPlatformSDL2::setDeviceChangeCallback(DeviceChangeCallback callback)
    {
        _deviceChangeCallback = std::move(callback);
    }

    void AudioPlatformSDL2::setAutoReconnect(bool enable)
    {
        _autoReconnect = enable;
    }

    bool AudioPlatformSDL2::reopen(const char* deviceName)
    {
        return open(deviceName);
    }

    std::vector<std::string> AudioPlatformSDL2::enumerateDevices()
    {
        std::vector<std::string> devices;
        int count = SDL_GetNumAudioDevices(SDL_FALSE);
        for (int i = 0; i < count; i++)
        {
            devices.emplace_back(String::toStd(SDL_GetAudioDeviceName(i, SDL_FALSE)));
        }
        return devices;
    }

    AudioSampleFormat AudioPlatformSDL2::mapSDLFormat(uint16_t sdlFormat)
    {
        switch (sdlFormat)
        {
            case AUDIO_U8:
                return AudioSampleFormat::u8;
            case AUDIO_S16SYS:
                return AudioSampleFormat::s16;
            case AUDIO_S32SYS:
                return AudioSampleFormat::s32;
            case AUDIO_F32SYS:
                return AudioSampleFormat::f32;
            default:
                return AudioSampleFormat::s16;
        }
    }

    void AudioPlatformSDL2::sdlCallback(void* userdata, uint8_t* stream, int length)
    {
        auto* platform = static_cast<AudioPlatformSDL2*>(userdata);

        size_t bytesPerFrame;
        switch (platform->_format)
        {
            case AudioSampleFormat::f32:
                bytesPerFrame = platform->_channels * sizeof(float);
                break;
            case AudioSampleFormat::s32:
                bytesPerFrame = platform->_channels * sizeof(int32_t);
                break;
            case AudioSampleFormat::s16:
            default:
                bytesPerFrame = platform->_channels * sizeof(int16_t);
                break;
        }

        size_t frames = static_cast<size_t>(length) / bytesPerFrame;

        size_t maxFrames = platform->_renderBufferCapacity / 2;
        if (frames > maxFrames)
            frames = maxFrames;

        platform->_engine.render(platform->_renderBuffer.data(), frames, platform->_sampleRate);

        if (platform->_format == AudioSampleFormat::f32 && platform->_channels == 2)
        {
            std::memcpy(stream, platform->_renderBuffer.data(), frames * 2 * sizeof(float));
        }
        else
        {
            AudioEngine::convertToDevice(platform->_renderBuffer.data(), stream, frames, platform->_format);
        }
    }

} // namespace OpenRCT2::Audio
