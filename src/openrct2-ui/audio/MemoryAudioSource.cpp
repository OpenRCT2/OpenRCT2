#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
#include <openrct2/core/Math.hpp>
#include <openrct2/core/Memory.hpp>
#include <openrct2/audio/AudioMixer.h>
#include <openrct2/audio/AudioSource.h>
#include "AudioContext.h"
#include "AudioFormat.h"

namespace OpenRCT2 { namespace Audio
{
    /**
     * An audio source where raw PCM data is initially loaded into RAM from
     * a file and then streamed.
     */
    class MemoryAudioSource final : public ISDLAudioSource
    {
    private:
        AudioFormat _format = { 0 };
        uint8 *     _data = nullptr;
        size_t      _length = 0;
        bool        _isSDLWav = false;

    public:
        ~MemoryAudioSource()
        {
            Unload();
        }

        uint64 GetLength() const override
        {
            return _length;
        }

        AudioFormat GetFormat() const override
        {
            return _format;
        }

        size_t Read(void * dst, uint64 offset, size_t len) override
        {
            size_t bytesToRead = 0;
            if (offset < _length)
            {
                bytesToRead = (size_t)Math::Min<uint64>(len, _length - offset);
                Memory::Copy<void>(dst, _data + offset, bytesToRead);
            }
            return bytesToRead;
        }

        bool LoadWAV(const utf8 * path)
        {
            log_verbose("MemoryAudioSource::LoadWAV(%s)", path);

            Unload();

            bool result = false;
            SDL_RWops * rw = SDL_RWFromFile(path, "rb");
            if (rw != nullptr)
            {
                SDL_AudioSpec audiospec = { 0 };
                uint32 audioLen;
                SDL_AudioSpec * spec = SDL_LoadWAV_RW(rw, false, &audiospec, &_data, &audioLen);
                if (spec != nullptr)
                {
                    _format.freq = spec->freq;
                    _format.format = spec->format;
                    _format.channels = spec->channels;
                    _length = audioLen;
                    _isSDLWav = true;
                    result = true;
                }
                else
                {
                    log_verbose("Error loading %s, unsupported WAV format", path);
                }
                SDL_RWclose(rw);
            }
            else
            {
                log_verbose("Error loading %s", path);
            }
            return result;
        }

        bool LoadCSS1(const utf8 * path, size_t index)
        {
            log_verbose("MemoryAudioSource::LoadCSS1(%s, %d)", path, index);

            Unload();

            bool result = false;
            SDL_RWops * rw = SDL_RWFromFile(path, "rb");
            if (rw != nullptr)
            {
                uint32 numSounds;
                SDL_RWread(rw, &numSounds, sizeof(numSounds), 1);
                if (index < numSounds)
                {
                    SDL_RWseek(rw, index * 4, RW_SEEK_CUR);

                    uint32 pcmOffset;
                    SDL_RWread(rw, &pcmOffset, sizeof(pcmOffset), 1);
                    SDL_RWseek(rw, pcmOffset, RW_SEEK_SET);

                    uint32 pcmSize;
                    SDL_RWread(rw, &pcmSize, sizeof(pcmSize), 1);
                    _length = pcmSize;

                    WaveFormatEx waveFormat;
                    SDL_RWread(rw, &waveFormat, sizeof(waveFormat), 1);
                    _format.freq = waveFormat.frequency;
                    _format.format = AUDIO_S16LSB;
                    _format.channels = waveFormat.channels;

                    _data = new (std::nothrow) uint8[_length];
                    if (_data != nullptr)
                    {
                        SDL_RWread(rw, _data, _length, 1);
                        result = true;
                    }
                    else
                    {
                        log_verbose("Unable to allocate data");
                    }
                }
                SDL_RWclose(rw);
            }
            else
            {
                log_verbose("Unable to load %s", path);
            }
            return result;
        }

        bool Convert(const AudioFormat * format)
        {
            if (*format != _format)
            {
                SDL_AudioCVT cvt;
                if (SDL_BuildAudioCVT(&cvt, _format.format, _format.channels, _format.freq, format->format, format->channels, format->freq) >= 0)
                {
                    cvt.len = (sint32)_length;
                    cvt.buf = new uint8[cvt.len * cvt.len_mult];
                    Memory::Copy(cvt.buf, _data, _length);
                    if (SDL_ConvertAudio(&cvt) >= 0)
                    {
                        Unload();
                        _data = cvt.buf;
                        _length = cvt.len_cvt;
                        _format = *format;
                        return true;
                    }
                    else
                    {
                        delete[] cvt.buf;
                    }
                }
            }
            return false;
        }

    private:
        void Unload()
        {
            if (_data != nullptr)
            {
                if (_isSDLWav)
                {
                    SDL_FreeWAV(_data);
                }
                else
                {
                    delete[] _data;
                }
                _data = nullptr;
            }
            _isSDLWav = false;
            _length = 0;
        }
    };

    IAudioSource * AudioSource::CreateMemoryFromCSS1(const std::string &path, size_t index, const AudioFormat * targetFormat)
    {
        auto source = new MemoryAudioSource();
        if (source->LoadCSS1(path.c_str(), index))
        {
            if (targetFormat != nullptr)
            {
                if (!source->Convert(targetFormat))
                {
                    delete source;
                    source = nullptr;
                }
            }
        }
        else
        {
            delete source;
            source = nullptr;
        }
        return source;
    }

    IAudioSource * AudioSource::CreateMemoryFromWAV(const std::string &path, const AudioFormat * targetFormat)
    {
        auto source = new MemoryAudioSource();
        if (source->LoadWAV(path.c_str()))
        {
            if (targetFormat != nullptr)
            {
                if (!source->Convert(targetFormat))
                {
                    SafeDelete(source);
                }
            }
        }
        else
        {
            delete source;
            source = nullptr;
        }
        return source;
    }
} }
