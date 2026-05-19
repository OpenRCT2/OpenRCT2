/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "AudioEngineCommand.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace OpenRCT2::Audio
{
    static constexpr size_t kMaxVoices = 128;

    enum class VoiceState : uint8_t
    {
        idle,
        playing,
        stopping,
    };

    struct Voice
    {
        VoiceState state = VoiceState::idle;
        AudioEngineGroup group = AudioEngineGroup::sound;
        uint16_t generation = 0;

        const float* pcmData = nullptr;
        uint64_t pcmLengthInFrames = 0;
        uint32_t sampleRate = 0;
        uint8_t channels = 0;
        bool looping = false;

        double playbackPosition = 0.0;
        float volume = 1.0f;
        float targetVolume = 1.0f;
        float pan = 0.5f;
        float rate = 1.0f;
        float prevPanL = 1.0f;
        float prevPanR = 1.0f;

        float fadePerSample = 0.0f;

        AudioHandle gameHandle{AudioHandle::kInvalid};

        void reset()
        {
            state = VoiceState::idle;
            pcmData = nullptr;
            pcmLengthInFrames = 0;
            sampleRate = 0;
            channels = 0;
            looping = false;
            playbackPosition = 0.0;
            volume = 1.0f;
            targetVolume = 1.0f;
            pan = 0.5f;
            rate = 1.0f;
            prevPanL = 1.0f;
            prevPanR = 1.0f;
            fadePerSample = 0.0f;
            gameHandle = {AudioHandle::kInvalid};
        }
    };

    class AudioVoicePool
    {
    public:
        AudioVoicePool();

        AudioHandle claim();
        AudioHandle stealQuietest();
        Voice* get(AudioHandle handle);
        Voice* getByGameHandle(AudioHandle gameHandle);
        Voice& getByIndex(size_t index);
        void releaseAll();
        void releaseGroup(AudioEngineGroup group);
        [[nodiscard]] size_t indexOf(const Voice* voice) const;

        [[nodiscard]] size_t activeCount() const;

        void setVoiceLimit(size_t limit);
        [[nodiscard]] size_t getVoiceLimit() const { return _voiceLimit; }

    private:
        size_t _voiceLimit = 128;
        std::array<Voice, kMaxVoices> _voices{};
    };

} // namespace OpenRCT2::Audio
