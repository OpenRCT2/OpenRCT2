/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SDLAudioSource.h"

#include <SDL3/SDL.h>
#include <stdexcept>

namespace OpenRCT2::Audio
{
    static uint32_t ReadU32LE(SDL_IOStream* rw)
    {
        Uint32 value{};
        SDL_ReadU32LE(rw, &value);
        return value;
    }

    static uint16_t ReadU16LE(SDL_IOStream* rw)
    {
        Uint16 value{};
        SDL_ReadU16LE(rw, &value);
        return value;
    }

    /**
     * An audio source where raw PCM data is stored in RAM.
     */
    class WavAudioSource final : public SDLAudioSource
    {
    private:
        static constexpr uint32_t kChunkIdDATA = 0x61746164;
        static constexpr uint32_t kChunkIdFMT = 0x20746D66;
        static constexpr uint32_t kChunkIdRIFF = 0x46464952;
        static constexpr uint32_t kChunkIdWAVE = 0x45564157;
        static constexpr uint16_t kPCMFormat = 0x0001;

        SDL_IOStream* _rw{};
        AudioFormat _format = {};
        uint64_t _dataBegin{};
        uint64_t _dataLength{};

    public:
        WavAudioSource(SDL_IOStream* rw)
            : _rw(rw)
        {
            auto chunkId = ReadU32LE(rw);
            if (chunkId != kChunkIdRIFF)
            {
                SDL_CloseIO(rw);
                throw std::runtime_error("Not a WAV file");
            }

            // Read and discard chunk size
            ReadU32LE(rw);
            auto chunkFormat = ReadU32LE(rw);
            if (chunkFormat != kChunkIdWAVE)
            {
                SDL_CloseIO(rw);
                throw std::runtime_error("Not in WAVE format");
            }

            auto fmtChunkSize = FindChunk(rw, kChunkIdFMT);
            if (!fmtChunkSize)
            {
                SDL_CloseIO(rw);
                throw std::runtime_error("Could not find FMT chunk");
            }

            auto chunkStart = SDL_TellIO(rw);

            auto encoding = ReadU16LE(rw);
            if (encoding != kPCMFormat)
            {
                SDL_CloseIO(rw);
                throw std::runtime_error("Not in PCM format");
            }

            _format.channels = ReadU16LE(rw);
            _format.freq = ReadU32LE(rw);
            [[maybe_unused]] auto byterate = ReadU32LE(rw);
            [[maybe_unused]] auto blockalign = ReadU16LE(rw);
            [[maybe_unused]] auto bitspersample = ReadU16LE(rw);
            switch (bitspersample)
            {
                case 8:
                    _format.format = SDL_AUDIO_U8;
                    break;
                case 16:
                    _format.format = SDL_AUDIO_S16LE;
                    break;
                default:
                    SDL_CloseIO(rw);
                    throw std::runtime_error("Unsupported bits per sample");
            }

            SDL_SeekIO(rw, chunkStart + fmtChunkSize, SDL_IO_SEEK_SET);

            auto dataChunkSize = FindChunk(rw, kChunkIdDATA);
            if (dataChunkSize == 0)
            {
                SDL_CloseIO(rw);
                throw std::runtime_error("Could not find DATA chunk");
            }

            _dataLength = dataChunkSize;
            _dataBegin = static_cast<uint64_t>(SDL_TellIO(rw));
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
            int64_t currentPosition = SDL_TellIO(_rw);
            if (currentPosition != -1)
            {
                size_t bytesToRead = static_cast<size_t>(std::min<uint64_t>(len, _dataLength - offset));
                int64_t dataOffset = _dataBegin + offset;
                if (currentPosition != dataOffset)
                {
                    int64_t newPosition = SDL_SeekIO(_rw, dataOffset, SDL_IO_SEEK_SET);
                    if (newPosition == -1)
                    {
                        return 0;
                    }
                }
                bytesRead = SDL_ReadIO(_rw, dst, bytesToRead);
            }
            return bytesRead;
        }

    protected:
        void Unload() override
        {
            if (_rw != nullptr)
            {
                SDL_CloseIO(_rw);
                _rw = nullptr;
            }
            _dataBegin = 0;
            _dataLength = 0;
        }

    private:
        static uint32_t FindChunk(SDL_IOStream* rw, uint32_t wantedId)
        {
            uint32_t subchunkId = ReadU32LE(rw);
            uint32_t subchunkSize = ReadU32LE(rw);
            if (subchunkId == wantedId)
            {
                return subchunkSize;
            }
            constexpr uint32_t kChunkIdFACT = 0x74636166;
            constexpr uint32_t kChunkIdLIST = 0x5453494c;
            constexpr uint32_t kChunkIdBEXT = 0x74786562;
            constexpr uint32_t kChunkIdJUNK = 0x4B4E554A;
            while (subchunkId == kChunkIdFACT || subchunkId == kChunkIdLIST || subchunkId == kChunkIdBEXT
                   || subchunkId == kChunkIdJUNK)
            {
                SDL_SeekIO(rw, subchunkSize, SDL_IO_SEEK_CUR);
                subchunkId = ReadU32LE(rw);
                subchunkSize = ReadU32LE(rw);
                if (subchunkId == wantedId)
                {
                    return subchunkSize;
                }
            }
            return 0;
        }
    };

    std::unique_ptr<SDLAudioSource> CreateWavAudioSource(SDL_IOStream* rw)
    {
        return std::make_unique<WavAudioSource>(rw);
    }
} // namespace OpenRCT2::Audio
