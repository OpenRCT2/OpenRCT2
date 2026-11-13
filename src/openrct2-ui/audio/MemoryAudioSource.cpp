/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioContext.h"
#include "AudioFormat.h"
#include "SDLAudioSource.h"

#include <SDL3/SDL.h>
#include <algorithm>
#include <openrct2/audio/AudioSource.h>
#include <stdexcept>
#include <vector>

namespace OpenRCT2::Audio
{
    /**
     * An audio source where raw PCM data is stored in RAM.
     */
    class MemoryAudioSource final : public SDLAudioSource
    {
    private:
        AudioFormat _format = {};
        std::vector<uint8_t> _data;

    public:
        MemoryAudioSource(const AudioFormat& format, std::vector<uint8_t>&& pcmData)
            : _format(format)
            , _data(pcmData)
        {
        }

        [[nodiscard]] AudioFormat GetFormat() const override
        {
            return _format;
        }

        [[nodiscard]] uint64_t GetLength() const override
        {
            return _data.size();
        }

        size_t Read(void* dst, uint64_t offset, size_t len) override
        {
            size_t bytesToRead = 0;
            if (offset < _data.size())
            {
                bytesToRead = static_cast<size_t>(std::min<uint64_t>(len, _data.size() - offset));

                auto src = _data.data();
                if (src != nullptr)
                {
                    std::copy_n(src + offset, bytesToRead, static_cast<uint8_t*>(dst));
                }
            }
            return bytesToRead;
        }

    protected:
        void Unload() override
        {
            _data.clear();
            _data.shrink_to_fit();
        }
    };

    static bool ConvertPcmData(const AudioFormat& target, const AudioFormat& src, std::vector<uint8_t>& pcmData)
    {
        if (target != src)
        {
            SDL_AudioSpec srcSpec{ src.format, src.channels, src.freq };
            SDL_AudioSpec dstSpec{ target.format, target.channels, target.freq };

            Uint8* dstData = nullptr;
            int dstLen = 0;
            if (SDL_ConvertAudioSamples(
                    &srcSpec, pcmData.data(), static_cast<int>(pcmData.size()), &dstSpec, &dstData, &dstLen))
            {
                pcmData.assign(dstData, dstData + dstLen);
                SDL_free(dstData);
                return true;
            }
        }
        return true;
    }

    std::unique_ptr<SDLAudioSource> CreateMemoryAudioSource(
        const AudioFormat& target, const AudioFormat& src, std::vector<uint8_t>&& pcmData)
    {
        if (!ConvertPcmData(target, src, pcmData))
            throw std::runtime_error("Unable to convert PCM data to target format");

        return std::make_unique<MemoryAudioSource>(target, std::move(pcmData));
    }
} // namespace OpenRCT2::Audio
