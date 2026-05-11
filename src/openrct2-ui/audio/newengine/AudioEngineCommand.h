/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace OpenRCT2::Audio
{
    struct AudioHandle
    {
        uint32_t value;

        static constexpr uint32_t kInvalid = 0;

        [[nodiscard]] bool isValid() const
        {
            return value != kInvalid;
        }

        bool operator==(const AudioHandle& other) const
        {
            return value == other.value;
        }

        bool operator!=(const AudioHandle& other) const
        {
            return value != other.value;
        }
    };

    enum class AudioEngineGroup : uint8_t
    {
        sound,
        rideMusic,
        titleMusic,
    };

    enum class AudioSampleFormat : uint16_t
    {
        unknown = 0,
        u8 = 1,
        s16 = 2,
        s32 = 3,
        f32 = 4,
    };

    enum class AudioCommandType : uint8_t
    {
        playOneShot,
        stop,
        stopAll,
        setVolume,
        setMasterVolume,
    };

    struct AudioCommand
    {
        AudioCommandType type;

        union
        {
            struct
            {
                const float* pcmData;
                uint64_t pcmLengthInFrames;
                uint32_t sampleRate;
                uint8_t channels;
                float volume;
                float pan;
                AudioEngineGroup group;
            } playOneShot;

            struct
            {
                AudioHandle handle;
            } stop;

            struct
            {
                AudioHandle handle;
                float volume;
            } setVolume;

            struct
            {
                float volume;
            } setMasterVolume;
        } data;
    };

    static_assert(std::is_trivially_copyable_v<AudioCommand>, "AudioCommand must be trivially copyable for ring buffer");

} // namespace OpenRCT2::Audio
