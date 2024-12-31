/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SDLAudioSource.h"

#include <cassert>
#include <openrct2/Diagnostic.h>
#include <stdexcept>

#ifndef DISABLE_VORBIS
    #include <SDL.h>
    #include <optional>
    #include <vector>
    #include <vorbis/vorbisfile.h>
#endif

namespace OpenRCT2::Audio
{
#ifndef DISABLE_VORBIS
    /**
     * An audio source which decodes a OGG/Vorbis stream.
     */
    class OggAudioSource final : public SDLAudioSource
    {
    private:
        AudioFormat _format = {};
        SDL_RWops* _rw = nullptr;

        std::optional<OggVorbis_File> _file;
        uint64_t _dataLength{};
        uint32_t _totalSamples{};
        int32_t _section{};
        size_t _currentOffset{};

    public:
        ~OggAudioSource() override
        {
            Release();
        }

        [[nodiscard]] uint64_t GetLength() const override
        {
            return _dataLength;
        }

        [[nodiscard]] AudioFormat GetFormat() const override
        {
            return _format;
        }

        bool LoadOgg(SDL_RWops* rw)
        {
            _rw = rw;

            ov_callbacks callbacks{};
            callbacks.read_func = VorbisCallbackRead;
            callbacks.tell_func = VorbisCallbackTell;
            callbacks.seek_func = VorbisCallbackSeek;
            _file.emplace();
            if (ov_open_callbacks(_rw, &*_file, NULL, 0, callbacks) < 0)
            {
                LOG_VERBOSE("Could not open OGG/Vorbis stream");
                return false;
            }

            auto vi = ov_info(&*_file, -1);
            if (vi == nullptr)
            {
                LOG_VERBOSE("Failed to get OGG/Vorbis info");
                return false;
            }

            _format.format = AUDIO_S16LSB;
            _format.channels = vi->channels;
            _format.freq = vi->rate;
            _totalSamples = ov_pcm_total(&*_file, -1);
            _dataLength = _totalSamples * _format.channels * sizeof(int16_t);
            _currentOffset = 0;
            _section = -1;
            return true;
        }

        size_t Read(void* dst, uint64_t offset, size_t len) override
        {
            if (!_file)
                return 0;

            if (_currentOffset != offset)
            {
                // We have been asked for a new position in the stream
                auto byteRate = _format.GetByteRate();
                auto sampleIndex = offset / byteRate;
                ov_pcm_seek(&*_file, sampleIndex);
                _currentOffset = offset;
            }

            auto readLen = static_cast<int64_t>(len);
            auto dst8 = reinterpret_cast<char*>(dst);
            int64_t totalBytesRead{};
            int64_t bytesRead;
            do
            {
                int section = _section;
                bytesRead = ov_read(&*_file, dst8, readLen, SDL_BYTEORDER == SDL_BIG_ENDIAN, 2, 1, &section);
                if (_section != section)
                {
                    _section = section;
                }
                assert(bytesRead <= readLen);
                dst8 += bytesRead;
                readLen -= bytesRead;
                totalBytesRead += bytesRead;
            } while (bytesRead > 0 && readLen > 0);

            _currentOffset += totalBytesRead;
            return totalBytesRead;
        }

    protected:
        void Unload() override
        {
            if (_file)
            {
                ov_clear(&*_file);
            }
            if (_rw != nullptr)
            {
                SDL_RWclose(_rw);
                _rw = nullptr;
            }
        }

    private:
        static size_t VorbisCallbackRead(void* ptr, size_t size, size_t nmemb, void* datasource)
        {
            return SDL_RWread(reinterpret_cast<SDL_RWops*>(datasource), ptr, size, nmemb);
        }

        static int VorbisCallbackSeek(void* datasource, ogg_int64_t offset, int whence)
        {
            return (SDL_RWseek(reinterpret_cast<SDL_RWops*>(datasource), offset, whence) < 0) ? -1 : 0;
        }

        static long VorbisCallbackTell(void* datasource)
        {
            return static_cast<long>(SDL_RWtell(reinterpret_cast<SDL_RWops*>(datasource)));
        }
    };
#endif

    std::unique_ptr<SDLAudioSource> CreateOggAudioSource(SDL_RWops* rw)
    {
#ifndef DISABLE_VORBIS
        auto source = std::make_unique<OggAudioSource>();
        if (!source->LoadOgg(rw))
        {
            throw std::runtime_error("Unable to load OGG/vorbis stream");
        }
        return source;
#else
        throw std::runtime_error("OpenRCT2 has not been compiled with OGG/vorbis support");
#endif
    }
} // namespace OpenRCT2::Audio
