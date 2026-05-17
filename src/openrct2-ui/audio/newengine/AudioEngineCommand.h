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
    // Upper 16 bits = generation, lower 16 bits = slot index
    struct AudioHandle
    {
        uint32_t value;

        static constexpr uint32_t kInvalid = 0;
        static constexpr uint32_t kGenerationShift = 16;
        static constexpr uint32_t kIndexMask = 0xFFFF;

        [[nodiscard]] uint16_t slotIndex() const
        {
            return static_cast<uint16_t>(value & kIndexMask);
        }

        [[nodiscard]] uint16_t generation() const
        {
            return static_cast<uint16_t>(value >> kGenerationShift);
        }

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

        static AudioHandle make(uint16_t slotIndex, uint16_t generation)
        {
            return { (static_cast<uint32_t>(generation) << kGenerationShift) | slotIndex };
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
        playLoop,
        stop,
        stopAll,
        setVolume,
        setMasterVolume,
        stopGroup,
        setPan,
        setRate,
        setGroupVolume,
        fadeOut,
    };

    // pcmData pointers are NOT owned, caller keeps them alive during playback
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
                const float* pcmData;
                uint64_t pcmLengthInFrames;
                uint32_t sampleRate;
                uint8_t channels;
                float volume;
                float pan;
                float rate;
                AudioEngineGroup group;
                AudioHandle handle;
                bool looping;
            } playLoop;

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

            struct
            {
                AudioEngineGroup group;
            } stopGroup;

            struct
            {
                AudioHandle handle;
                float pan;
            } setPan;

            struct
            {
                AudioHandle handle;
                float rate;
            } setRate;

            struct
            {
                AudioEngineGroup group;
                float volume;
            } setGroupVolume;

            struct
            {
                AudioHandle handle;
                float durationMs;
            } fadeOut;
        } data;
    };

    static_assert(std::is_trivially_copyable_v<AudioCommand>, "AudioCommand must be trivially copyable for ring buffer");

} // namespace OpenRCT2::Audio
