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
#include <openrct2/core/Math.hpp>
#include <openrct2/audio/AudioSource.h>
#include "AudioContext.h"
#include "AudioFormat.h"

namespace OpenRCT2 { namespace Audio
{
    /**
     * An audio source where raw PCM data is streamed directly from
     * a file.
     */
    class FileAudioSource final : public ISDLAudioSource
    {
    private:
        AudioFormat _format = { 0 };
        SDL_RWops * _rw = nullptr;
        uint64      _dataBegin = 0;
        uint64      _dataLength = 0;

    public:
        ~FileAudioSource()
        {
            Unload();
        }

        uint64 GetLength() const override
        {
            return _dataLength;
        }

        AudioFormat GetFormat() const override
        {
            return _format;
        }

        size_t Read(void * dst, uint64 offset, size_t len) override
        {
            size_t bytesRead = 0;
            sint64 currentPosition = SDL_RWtell(_rw);
            if (currentPosition != -1)
            {
                size_t bytesToRead = (size_t)Math::Min<uint64>(len, _dataLength - offset);
                sint64 dataOffset = _dataBegin + offset;
                if (currentPosition != dataOffset)
                {
                    sint64 newPosition = SDL_RWseek(_rw, dataOffset, SEEK_SET);
                    if (newPosition == -1)
                    {
                        return 0;
                    }
                }
                bytesRead = SDL_RWread(_rw, dst, 1, bytesToRead);
            }
            return bytesRead;
        }

        bool LoadWAV(SDL_RWops * rw)
        {
            const uint32 DATA = 0x61746164;
            const uint32 FMT  = 0x20746D66;
            const uint32 RIFF = 0x46464952;
            const uint32 WAVE = 0x45564157;
            const uint16 pcmformat = 0x0001;

            Unload();

            if (rw == nullptr)
            {
                return false;
            }
            _rw = rw;

            uint32 chunkId = SDL_ReadLE32(rw);
            if (chunkId != RIFF)
            {
                log_verbose("Not a WAV file");
                return false;
            }

            // Read and discard chunk size
            SDL_ReadLE32(rw);
            uint32 chunkFormat = SDL_ReadLE32(rw);
            if (chunkFormat != WAVE)
            {
                log_verbose("Not in WAVE format");
                return false;
            }

            uint32 fmtChunkSize = FindChunk(rw, FMT);
            if (!fmtChunkSize)
            {
                log_verbose("Could not find FMT chunk");
                return false;
            }

            uint64 chunkStart = SDL_RWtell(rw);

            WaveFormat waveFormat;
            SDL_RWread(rw, &waveFormat, sizeof(waveFormat), 1);
            SDL_RWseek(rw, chunkStart + fmtChunkSize, RW_SEEK_SET);
            if (waveFormat.encoding != pcmformat) {
                log_verbose("Not in proper format");
                return false;
            }

            _format.freq = waveFormat.frequency;
            switch (waveFormat.bitspersample) {
            case 8:
                _format.format = AUDIO_U8;
                break;
            case 16:
                _format.format = AUDIO_S16LSB;
                break;
            default:
                log_verbose("Invalid bits per sample");
                return false;
                break;
            }
            _format.channels = waveFormat.channels;

            uint32 dataChunkSize = FindChunk(rw, DATA);
            if (dataChunkSize == 0)
            {
                log_verbose("Could not find DATA chunk");
                return false;
            }

            _dataLength = dataChunkSize;
            _dataBegin = SDL_RWtell(rw);
            return true;
        }

    private:
        uint32 FindChunk(SDL_RWops * rw, uint32 wantedId)
        {
            uint32 subchunkId = SDL_ReadLE32(rw);
            uint32 subchunkSize = SDL_ReadLE32(rw);
            if (subchunkId == wantedId)
            {
                return subchunkSize;
            }
            const uint32 FACT = 0x74636166;
            const uint32 LIST = 0x5453494c;
            const uint32 BEXT = 0x74786562;
            const uint32 JUNK = 0x4B4E554A;
            while (subchunkId == FACT || subchunkId == LIST || subchunkId == BEXT || subchunkId == JUNK)
            {
                SDL_RWseek(rw, subchunkSize, RW_SEEK_CUR);
                subchunkId = SDL_ReadLE32(rw);
                subchunkSize = SDL_ReadLE32(rw);
                if (subchunkId == wantedId)
                {
                    return subchunkSize;
                }
            }
            return 0;
        }

        void Unload()
        {
            if (_rw != nullptr)
            {
                SDL_RWclose(_rw);
                _rw = nullptr;
            }
            _dataBegin = 0;
            _dataLength = 0;
        }
    };

    IAudioSource * AudioSource::CreateStreamFromWAV(const std::string &path)
    {
        IAudioSource * source = nullptr;
        SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
        if (rw != nullptr)
        {
            return AudioSource::CreateStreamFromWAV(rw);
        }
        return source;
    }

    IAudioSource * AudioSource::CreateStreamFromWAV(SDL_RWops * rw)
    {
        auto source = new FileAudioSource();
        if (!source->LoadWAV(rw))
        {
            delete source;
            source = nullptr;
        }
        return source;
    }
} }
