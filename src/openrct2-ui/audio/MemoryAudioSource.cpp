/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioContext.h"
#include "AudioFormat.h"
#include "SDLAudioSource.h"

#include <SDL.h>
#include <algorithm>
#include <openrct2/audio/AudioSource.h>
#include <openrct2/common.h>
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
                    std::copy_n(src + offset, bytesToRead, reinterpret_cast<uint8_t*>(dst));
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
            SDL_AudioCVT cvt;
            if (SDL_BuildAudioCVT(&cvt, src.format, src.channels, src.freq, target.format, target.channels, target.freq) >= 0)
            {
                auto srcData = pcmData.data();
                auto srcLen = pcmData.size();
                auto cvtBuffer = std::vector<uint8_t>(srcLen * cvt.len_mult);
                std::copy_n(srcData, srcLen, cvtBuffer.data());
                cvt.len = static_cast<int32_t>(srcLen);
                cvt.buf = cvtBuffer.data();
                if (SDL_ConvertAudio(&cvt) >= 0)
                {
                    cvtBuffer.resize(cvt.len_cvt);
                    cvtBuffer.shrink_to_fit();
                    pcmData = std::move(cvtBuffer);
                    return true;
                }
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
