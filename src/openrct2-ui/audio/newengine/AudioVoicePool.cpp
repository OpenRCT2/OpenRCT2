/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioVoicePool.h"

namespace OpenRCT2::Audio
{
    AudioVoicePool::AudioVoicePool()
    {
        for (size_t i = 0; i < kMaxVoices; i++)
        {
            _voices[i].generation = 0;
            _voices[i].state = VoiceState::idle;
        }
    }

    AudioHandle AudioVoicePool::claim()
    {
        for (size_t i = 0; i < kMaxVoices; i++)
        {
            if (_voices[i].state == VoiceState::idle)
            {
                auto& voice = _voices[i];
                voice.reset();
                voice.state = VoiceState::playing;
                voice.generation++;
                return AudioHandle::make(static_cast<uint16_t>(i), voice.generation);
            }
        }

        return { AudioHandle::kInvalid };
    }

    Voice* AudioVoicePool::get(AudioHandle handle)
    {
        if (!handle.isValid())
            return nullptr;

        uint16_t index = handle.slotIndex();
        if (index >= kMaxVoices)
            return nullptr;

        auto& voice = _voices[index];
        if (voice.generation != handle.generation())
            return nullptr;

        return &voice;
    }

    Voice* AudioVoicePool::getByGameHandle(AudioHandle gameHandle)
    {
        if (!gameHandle.isValid())
            return nullptr;
        for (auto& voice : _voices)
        {
            if (voice.state != VoiceState::idle && voice.gameHandle == gameHandle)
                return &voice;
        }
        return nullptr;
    }

    Voice& AudioVoicePool::getByIndex(size_t index)
    {
        return _voices[index];
    }

    void AudioVoicePool::releaseAll()
    {
        for (auto& voice : _voices)
        {
            if (voice.state != VoiceState::idle)
            {
                voice.reset();
            }
        }
    }

    size_t AudioVoicePool::indexOf(const Voice* voice) const
    {
        if (voice >= &_voices[0] && voice < &_voices[kMaxVoices])
            return static_cast<size_t>(voice - &_voices[0]);
        return kMaxVoices;
    }

    size_t AudioVoicePool::activeCount() const
    {
        size_t count = 0;
        for (const auto& voice : _voices)
        {
            if (voice.state != VoiceState::idle)
                count++;
        }
        return count;
    }

} // namespace OpenRCT2::Audio
