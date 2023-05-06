/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>

namespace OpenRCT2::Audio
{
    constexpr int32_t MIXER_VOLUME_MAX = 128;
    constexpr int32_t MIXER_LOOP_NONE = 0;
    constexpr int32_t MIXER_LOOP_INFINITE = -1;

    enum class SoundId : uint8_t;

    enum class MixerGroup : int32_t
    {
        Sound,
        RideMusic,
        TitleMusic,
    };

    struct IAudioSource;
    struct IAudioChannel;

    /**
     * Provides an audio stream by mixing multiple audio channels together.
     */
    struct IAudioMixer
    {
        virtual ~IAudioMixer() = default;

        virtual void Init(const char* device) = 0;
        virtual void Close() = 0;
        virtual void Lock() = 0;
        virtual void Unlock() = 0;
        virtual std::shared_ptr<IAudioChannel> Play(IAudioSource* source, int32_t loop, bool deleteondone) = 0;
        virtual void SetVolume(float volume) = 0;
    };
} // namespace OpenRCT2::Audio
