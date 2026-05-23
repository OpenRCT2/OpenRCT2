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
#include "AudioRingBuffer.h"
#include "AudioVoicePool.h"

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace OpenRCT2::Audio
{
    static constexpr size_t kCommandBufferSize = 1024;

    struct AudioEngineStats
    {
        size_t activeVoices = 0;
        size_t culledVoices = 0;
        size_t voiceLimit = 128;
        float lastCallbackDurationUs = 0;
        float budgetPercent = 0;
        uint64_t droppedCommands = 0;
    };

    // This is the core audio mixer. The game thread sends commands (play, stop,
    // volume changes, etc.) into a lock free ring buffer. On every audio
    // callback the engine drains this buffer, mixes all active voices in
    // float32, and writes the result to the platform output buffer
    //
    // There is NO SDL dependency here... The platform layer (AudioPlatformSDL2)
    // owns the device and calls render() from the SDL callback. This split
    // keeps the mixer testable and portable
    class AudioEngine
    {
    public:
        AudioEngine();
        ~AudioEngine();

        // Push a command onto the ring buffer from the game thread.
        // Returns false if the buffer is full, the command is dropped
        // rather than blocking the game thread
        bool submitCommand(const AudioCommand& cmd);

        // Fire and forget: plays once, voice is recycled automatically.
        // Returns kInvalid. Use playTracked() if you need to control
        // the sound after it starts (volume, pan, stopping, etc.)
        AudioHandle playOneShot(
            const float* pcmData, uint64_t lengthInFrames, uint32_t sampleRate, uint8_t channels, float volume, float pan,
            AudioEngineGroup group);

        // Returns a handle so the caller (you/whoever) can adjust volume/pan/rate or
        // stop the sound later. Works for both looping and non looping
        AudioHandle playTracked(
            const float* pcmData, uint64_t lengthInFrames, uint32_t sampleRate, uint8_t channels, float volume, float pan,
            float rate, AudioEngineGroup group, bool looping);

        void stop(AudioHandle handle);
        void stopAll();
        void setVolume(AudioHandle handle, float volume);
        void setPan(AudioHandle handle, float pan);
        void setRate(AudioHandle handle, float rate);
        void setOffset(AudioHandle handle, uint64_t offsetInFrames);
        void setMasterVolume(float volume);
        void setGroupVolume(AudioEngineGroup group, float volume);
        void fadeOut(AudioHandle handle, float durationMs);
        void stopGroup(AudioEngineGroup group);

        // Called from the audio thread (the SDL callback). Drains the
        // command buffer, mixes active voices, and writes interleaved
        // float32 stereo into outputBuffer
        void render(float* outputBuffer, size_t framesRequested, uint32_t outputSampleRate);

        // Converts the float32 mix result to whatever format the
        // platform device actually uses (S16, F32, etc.)
        static void convertToDevice(const float* mixBuffer, uint8_t* dst, size_t frames, AudioSampleFormat format);

        // Safe to call from the game thread, reads atomic shadow state
        // only, never touches the voice pool. Returns false once the
        // voice finishes, is stopped, or gets stolen by another sound
        [[nodiscard]] bool isHandleActive(AudioHandle gameHandle) const;

        // Returns the current playback position in frames for the given handle.
        // Returns 0 if the handle's invalid
        [[nodiscard]] uint64_t getOffset(AudioHandle gameHandle) const;

        [[nodiscard]] AudioEngineStats getStats() const;

    private:
        void processCommands();
        void processPlayOneShot(const AudioCommand& cmd);
        void processPlayLoop(const AudioCommand& cmd);
        void processStop(const AudioCommand& cmd);
        void processSetVolume(const AudioCommand& cmd);
        void processSetPan(const AudioCommand& cmd);
        void processSetRate(const AudioCommand& cmd);
        void processSetMasterVolume(const AudioCommand& cmd);
        void processSetGroupVolume(const AudioCommand& cmd);
        void processFadeOut(const AudioCommand& cmd);
        void processSetOffset(const AudioCommand& cmd);

        Voice* resolveVoice(AudioHandle handle);

        void mixAllVoices(float* outputBuffer, size_t frames, uint32_t outputSampleRate, size_t& culled);
        void mixVoice(Voice& voice, float* outputBuffer, size_t frames, uint32_t outputSampleRate);
        void updateGovernor(float callbackDurationUs, size_t framesRequested);

        AudioRingBuffer<AudioCommand, kCommandBufferSize> _commandBuffer;
        AudioVoicePool _voicePool;

        float _masterVolume = 1.0f;
        float _soundVolume = 1.0f;
        float _musicVolume = 1.0f;

        float getGroupVolume(AudioEngineGroup group) const;

        AudioEngineStats _stats{};
        uint32_t _outputSampleRate = 48000;

        // Adaptive quality governor. Measures how much of the audio
        // deadline each callback consumes and adjusts the voice limit
        // and cull threshold so the engine stays within budget
        static constexpr float kCullVolumeMin = 0.005f;
        static constexpr float kCullVolumeMax = 0.05f;
        static constexpr size_t kVoiceLimitMin = 64;
        static constexpr size_t kVoiceLimitMax = 256;
        float _cullThreshold = kCullVolumeMin;
        float _budgetAvg = 0.0f;
        uint32_t _comfortStreak = 0;
        uint32_t _stressStreak = 0;
        float _limiterGain = 1.0f;

        // Incremented from the game thread when the ring buffer is full.
        // Atomic so getStats() can read it safely from any thread
        std::atomic<uint64_t> _droppedCommands{ 0 };

        void markSlotActive(size_t slotIndex, AudioHandle gameHandle);
        void markSlotInactive(size_t slotIndex);

        // Shadow state for cross thread voice completion queries.
        // The audio thread writes these (relaxed stores), the game
        // thread reads them (relaxed loads). This is separate from the
        // voice pool because I never want to touch hot mixing data from the game code
        struct SlotStatus
        {
            std::atomic<uint32_t> gameHandle{ AudioHandle::kInvalid };
            std::atomic<bool> active{ false };
            std::atomic<uint64_t> positionInFrames{ 0 };
        };
        std::array<SlotStatus, kMaxVoices> _slotStatus{};

        void updateSlotPosition(size_t slotIndex, uint64_t positionInFrames);

        // gameHandle -> slotIndex. Entries are packed as:
        //   bits [63:32] = gameHandle value (0 means empty)
        //   bits [15:0]  = voice slot index
        // With 512 entries and at most 256 active handles sequential
        static constexpr size_t kHandleLookupSize = 512;
        static constexpr size_t kHandleLookupMask = kHandleLookupSize - 1;
        std::array<std::atomic<uint64_t>, kHandleLookupSize> _handleLookup{};

        // Frozy was here
    };

} // namespace OpenRCT2::Audio
