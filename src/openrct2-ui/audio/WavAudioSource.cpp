/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SDLAudioSource.h"

#include <SDL.h>
#include <stdexcept>

namespace OpenRCT2::Audio
{
    /**
     * An audio source where raw PCM data is stored in RAM.
     */
    class WavAudioSource final : public SDLAudioSource
    {
    private:
        static constexpr uint32_t DATA = 0x61746164;
        static constexpr uint32_t FMT = 0x20746D66;
        static constexpr uint32_t RIFF = 0x46464952;
        static constexpr uint32_t WAVE = 0x45564157;
        static constexpr uint16_t pcmformat = 0x0001;

        SDL_RWops* _rw{};
        AudioFormat _format = {};
        uint64_t _dataBegin{};
        uint64_t _dataLength{};

    public:
        WavAudioSource(SDL_RWops* rw)
            : _rw(rw)
        {
            auto chunkId = SDL_ReadLE32(rw);
            if (chunkId != RIFF)
            {
                SDL_RWclose(rw);
                throw std::runtime_error("Not a WAV file");
            }

            // Read and discard chunk size
            SDL_ReadLE32(rw);
            auto chunkFormat = SDL_ReadLE32(rw);
            if (chunkFormat != WAVE)
            {
                SDL_RWclose(rw);
                throw std::runtime_error("Not in WAVE format");
            }

            auto fmtChunkSize = FindChunk(rw, FMT);
            if (!fmtChunkSize)
            {
                SDL_RWclose(rw);
                throw std::runtime_error("Could not find FMT chunk");
            }

            auto chunkStart = SDL_RWtell(rw);

            auto encoding = SDL_ReadLE16(rw);
            if (encoding != pcmformat)
            {
                SDL_RWclose(rw);
                throw std::runtime_error("Not in PCM format");
            }

            _format.channels = SDL_ReadLE16(rw);
            _format.freq = SDL_ReadLE32(rw);
            [[maybe_unused]] auto byterate = SDL_ReadLE32(rw);
            [[maybe_unused]] auto blockalign = SDL_ReadLE16(rw);
            [[maybe_unused]] auto bitspersample = SDL_ReadLE16(rw);
            switch (bitspersample)
            {
                case 8:
                    _format.format = AUDIO_U8;
                    break;
                case 16:
                    _format.format = AUDIO_S16LSB;
                    break;
                default:
                    SDL_RWclose(rw);
                    throw std::runtime_error("Unsupported bits per sample");
            }

            SDL_RWseek(rw, chunkStart + fmtChunkSize, RW_SEEK_SET);

            auto dataChunkSize = FindChunk(rw, DATA);
            if (dataChunkSize == 0)
            {
                SDL_RWclose(rw);
                throw std::runtime_error("Could not find DATA chunk");
            }

            _dataLength = dataChunkSize;
            _dataBegin = static_cast<uint64_t>(SDL_RWtell(rw));
        }

        ~WavAudioSource() override
        {
            Release();
        }

        [[nodiscard]] AudioFormat GetFormat() const override
        {
            return _format;
        }

        [[nodiscard]] uint64_t GetLength() const override
        {
            return _dataLength;
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

    protected:
        void Unload() override
        {
            if (_rw != nullptr)
            {
                SDL_RWclose(_rw);
                _rw = nullptr;
            }
            _dataBegin = 0;
            _dataLength = 0;
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
    };

    std::unique_ptr<SDLAudioSource> CreateWavAudioSource(SDL_RWops* rw)
    {
        return std::make_unique<WavAudioSource>(rw);
    }
} // namespace OpenRCT2::Audio
