/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "AudioFormat.h"

#include <SDL.h>
#include <memory>
#include <openrct2/audio/AudioSource.h>
#include <vector>

namespace OpenRCT2::Audio
{
    struct IAudioMixer;

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#    pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif
    class SDLAudioSource : public IAudioSource
    {
    private:
        bool _released{};

    public:
        void Release() override;
        int32_t GetBytesPerSecond() const override;
        bool IsReleased() const override;
        std::unique_ptr<SDLAudioSource> ToMemory(const AudioFormat& target);

        virtual AudioFormat GetFormat() const = 0;

    protected:
        virtual void Unload() = 0;

    private:
        IAudioMixer* GetMixer();
    };
#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif

    std::unique_ptr<SDLAudioSource> CreateAudioSource(SDL_RWops* rw);
    std::unique_ptr<SDLAudioSource> CreateAudioSource(SDL_RWops* rw, uint32_t cssIndex);
    std::unique_ptr<SDLAudioSource> CreateMemoryAudioSource(
        const AudioFormat& target, const AudioFormat& src, std::vector<uint8_t>&& pcmData);
    std::unique_ptr<SDLAudioSource> CreateFlacAudioSource(SDL_RWops* rw);
    std::unique_ptr<SDLAudioSource> CreateOggAudioSource(SDL_RWops* rw);
    std::unique_ptr<SDLAudioSource> CreateWavAudioSource(SDL_RWops* rw);
} // namespace OpenRCT2::Audio
