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

namespace OpenRCT2::Audio
{
    /**
     * An audio source where raw PCM data is streamed directly from
     * a file.
     */
    class FileAudioSource final : public ISDLAudioSource
    {
    private:
        AudioFormat _format = {};
        SDL_RWops* _rw = nullptr;
        uint64_t _dataBegin = 0;
        uint64_t _dataLength = 0;
        bool _released{};

    public:
        ~FileAudioSource() override
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
            return _dataLength;
        }

        [[nodiscard]] AudioFormat GetFormat() const override
        {
            return _format;
        }

        size_t Read(void* dst, uint64_t offset, size_t len) override
        {
            size_t bytesRead = 0;
            int64_t currentPosition = SDL_RWtell(_rw);
            if (currentPosition != -1)
            {
                size_t bytesToRead = static_cast<size_t>(std::min<uint64_t>(len, _dataLength - offset));
                int64_t dataOffset = _dataBegin + offset;
                if (currentPosition != dataOffset)
                {
                    int64_t newPosition = SDL_RWseek(_rw, dataOffset, SEEK_SET);
                    if (newPosition == -1)
                    {
                        return 0;
                    }
                }
                bytesRead = SDL_RWread(_rw, dst, 1, bytesToRead);
            }
            return bytesRead;
        }

        bool LoadWAV(SDL_RWops* rw)
        {
            constexpr uint32_t DATA = 0x61746164;
            constexpr uint32_t FMT = 0x20746D66;
            constexpr uint32_t RIFF = 0x46464952;
            constexpr uint32_t WAVE = 0x45564157;
            constexpr uint16_t pcmformat = 0x0001;

            Unload();

            if (rw == nullptr)
            {
                return false;
            }
            _rw = rw;

            uint32_t chunkId = SDL_ReadLE32(rw);
            if (chunkId != RIFF)
            {
                log_verbose("Not a WAV file");
                return false;
            }

            // Read and discard chunk size
            SDL_ReadLE32(rw);
            uint32_t chunkFormat = SDL_ReadLE32(rw);
            if (chunkFormat != WAVE)
            {
                log_verbose("Not in WAVE format");
                return false;
            }

            uint32_t fmtChunkSize = FindChunk(rw, FMT);
            if (!fmtChunkSize)
            {
                log_verbose("Could not find FMT chunk");
                return false;
            }

            uint64_t chunkStart = SDL_RWtell(rw);

            WaveFormat waveFormat{};
            SDL_RWread(rw, &waveFormat, sizeof(waveFormat), 1);
            SDL_RWseek(rw, chunkStart + fmtChunkSize, RW_SEEK_SET);
            if (waveFormat.encoding != pcmformat)
            {
                log_verbose("Not in proper format");
                return false;
            }

            _format.freq = waveFormat.frequency;
            switch (waveFormat.bitspersample)
            {
                case 8:
                    _format.format = AUDIO_U8;
                    break;
                case 16:
                    _format.format = AUDIO_S16LSB;
                    break;
                default:
                    log_verbose("Invalid bits per sample");
                    return false;
            }
            _format.channels = waveFormat.channels;

            uint32_t dataChunkSize = FindChunk(rw, DATA);
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
        static uint32_t FindChunk(SDL_RWops* rw, uint32_t wantedId)
        {
            uint32_t subchunkId = SDL_ReadLE32(rw);
            uint32_t subchunkSize = SDL_ReadLE32(rw);
            if (subchunkId == wantedId)
            {
                return subchunkSize;
            }
            constexpr uint32_t FACT = 0x74636166;
            constexpr uint32_t LIST = 0x5453494c;
            constexpr uint32_t BEXT = 0x74786562;
            constexpr uint32_t JUNK = 0x4B4E554A;
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

    std::unique_ptr<ISDLAudioSource> AudioSource::CreateStreamFromWAV(const std::string& path)
    {
        auto* rw = SDL_RWFromFile(path.c_str(), "rb");
        if (rw != nullptr)
        {
            return AudioSource::CreateStreamFromWAV(rw);
        }
        return nullptr;
    }

    std::unique_ptr<ISDLAudioSource> AudioSource::CreateStreamFromWAV(SDL_RWops* rw)
    {
        auto magic = SDL_ReadLE32(rw);
        SDL_RWseek(rw, -4, RW_SEEK_CUR);
        if (magic == 0x43614C66)
        {
            return AudioSource::CreateStreamFromFlac(rw);
        }

        auto source = std::make_unique<FileAudioSource>();
        if (!source->LoadWAV(rw))
        {
            source = nullptr;
        }
        return source;
    }
} // namespace OpenRCT2::Audio
