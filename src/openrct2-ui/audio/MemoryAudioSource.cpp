/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioContext.h"
#include "AudioFormat.h"

#include <SDL.h>
#include <algorithm>
#include <openrct2/audio/AudioSource.h>
#include <openrct2/common.h>
#include <vector>

namespace OpenRCT2::Audio
{
    /**
     * An audio source where raw PCM data is initially loaded into RAM from
     * a file and then streamed.
     */
    class MemoryAudioSource final : public ISDLAudioSource
    {
    private:
        AudioFormat _format = {};
        std::vector<uint8_t> _data;
        uint8_t* _dataSDL{};
        size_t _length{};
        bool _released{};

        const uint8_t* GetData()
        {
            return _dataSDL != nullptr ? _dataSDL : _data.data();
        }

    public:
        ~MemoryAudioSource() override
        {
            Release();
        }

        bool IsReleased() const override
        {
            return _released;
        }

        void Release() override
        {
            if (!_released)
            {
                Unload();
                _released = true;
            }
        }

        [[nodiscard]] uint64_t GetLength() const override
        {
            return _length;
        }

        [[nodiscard]] AudioFormat GetFormat() const override
        {
            return _format;
        }

        size_t Read(void* dst, uint64_t offset, size_t len) override
        {
            size_t bytesToRead = 0;
            if (offset < _length)
            {
                bytesToRead = static_cast<size_t>(std::min<uint64_t>(len, _length - offset));

                auto src = GetData();
                if (src != nullptr)
                {
                    std::copy_n(src + offset, bytesToRead, reinterpret_cast<uint8_t*>(dst));
                }
            }
            return bytesToRead;
        }

        bool LoadWAV(SDL_RWops* rw)
        {
            Unload();

            bool result = false;
            SDL_AudioSpec audiospec = {};
            uint32_t audioLen;
            SDL_AudioSpec* spec = SDL_LoadWAV_RW(rw, false, &audiospec, &_dataSDL, &audioLen);
            if (spec != nullptr)
            {
                _format.freq = spec->freq;
                _format.format = spec->format;
                _format.channels = spec->channels;
                _length = audioLen;
                result = true;
            }
            SDL_RWclose(rw);
            return result;
        }

        bool LoadCSS1(SDL_RWops* rw, size_t index)
        {
            Unload();

            bool result{};
            uint32_t numSounds{};
            SDL_RWread(rw, &numSounds, sizeof(numSounds), 1);
            if (index < numSounds)
            {
                SDL_RWseek(rw, index * 4, RW_SEEK_CUR);

                uint32_t pcmOffset{};
                SDL_RWread(rw, &pcmOffset, sizeof(pcmOffset), 1);
                SDL_RWseek(rw, pcmOffset, RW_SEEK_SET);

                uint32_t pcmSize{};
                SDL_RWread(rw, &pcmSize, sizeof(pcmSize), 1);
                _length = pcmSize;

                WaveFormatEx waveFormat{};
                SDL_RWread(rw, &waveFormat, sizeof(waveFormat), 1);
                _format.freq = waveFormat.frequency;
                _format.format = AUDIO_S16LSB;
                _format.channels = waveFormat.channels;

                try
                {
                    _data.resize(_length);
                    SDL_RWread(rw, _data.data(), _length, 1);
                    result = true;
                }
                catch (const std::bad_alloc&)
                {
                    log_verbose("Unable to allocate data");
                }
            }
            SDL_RWclose(rw);
            return result;
        }

        bool Convert(const AudioFormat* format)
        {
            if (*format != _format)
            {
                SDL_AudioCVT cvt;
                if (SDL_BuildAudioCVT(
                        &cvt, _format.format, _format.channels, _format.freq, format->format, format->channels, format->freq)
                    >= 0)
                {
                    auto src = GetData();
                    auto cvtBuffer = std::vector<uint8_t>(_length * cvt.len_mult);
                    std::copy_n(src, _length, cvtBuffer.data());
                    cvt.len = static_cast<int32_t>(_length);
                    cvt.buf = cvtBuffer.data();
                    if (SDL_ConvertAudio(&cvt) >= 0)
                    {
                        cvtBuffer.resize(cvt.len_cvt);

                        Unload();
                        _data = std::move(cvtBuffer);
                        _length = cvt.len_cvt;
                        _format = *format;
                        return true;
                    }
                }
            }
            return false;
        }

    private:
        void Unload()
        {
            // Free our data
            _data.clear();
            _data.shrink_to_fit();

            // Free SDL2's data
            SDL_FreeWAV(_dataSDL);
            _dataSDL = nullptr;

            _length = 0;
        }
    };

    std::unique_ptr<ISDLAudioSource> AudioSource::CreateMemoryFromCSS1(
        const std::string& path, size_t index, const AudioFormat* targetFormat)
    {
        log_verbose("AudioSource::CreateMemoryFromCSS1(%s, %d)", path.c_str(), index);
        SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
        if (rw != nullptr)
        {
            return CreateMemoryFromCSS1(rw, index, targetFormat);
        }
        else
        {
            log_verbose("Unable to load %s", path.c_str());
        }
        return nullptr;
    }

    std::unique_ptr<ISDLAudioSource> AudioSource::CreateMemoryFromCSS1(
        SDL_RWops* rw, size_t index, const AudioFormat* targetFormat)
    {
        auto source = std::make_unique<MemoryAudioSource>();
        if (source->LoadCSS1(rw, index))
        {
            if (targetFormat != nullptr && source->GetFormat() != *targetFormat)
            {
                if (!source->Convert(targetFormat))
                {
                    source = nullptr;
                }
            }
        }
        else
        {
            source = nullptr;
        }
        return source;
    }

    std::unique_ptr<ISDLAudioSource> AudioSource::CreateMemoryFromWAV(SDL_RWops* rw, const AudioFormat* targetFormat)
    {
        auto source = std::make_unique<MemoryAudioSource>();
        if (source->LoadWAV(rw))
        {
            if (targetFormat != nullptr && source->GetFormat() != *targetFormat)
            {
                if (!source->Convert(targetFormat))
                {
                    source = nullptr;
                }
            }
        }
        else
        {
            source = nullptr;
        }
        return source;
    }
} // namespace OpenRCT2::Audio
