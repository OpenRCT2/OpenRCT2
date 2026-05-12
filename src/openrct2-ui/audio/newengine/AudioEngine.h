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

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace OpenRCT2::Audio
{
    static constexpr size_t kCommandBufferSize = 1024;

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

        void stop(AudioHandle handle);
        void stopAll();
        void setVolume(AudioHandle handle, float volume);
        void setMasterVolume(float volume);

        // Called from the audio thread (the SDL callback). Drains the
        // command buffer, mixes active voices, and writes interleaved
        // float32 stereo into outputBuffer
        void render(float* outputBuffer, size_t framesRequested, uint32_t outputSampleRate);

        // Converts the float32 mix result to whatever format the
        // platform device actually uses (S16, F32, etc.)
        static void convertToDevice(const float* mixBuffer, uint8_t* dst, size_t frames, AudioSampleFormat format);

        [[nodiscard]] size_t getActiveVoiceCount() const;

    private:
        void processCommands();
        void processPlayOneShot(const AudioCommand& cmd);
        void processStop(const AudioCommand& cmd);
        void processSetVolume(const AudioCommand& cmd);
        void processSetMasterVolume(const AudioCommand& cmd);

        Voice* findVoice(AudioHandle handle);

        void mixAllVoices(float* outputBuffer, size_t frames, uint32_t outputSampleRate);
        void mixVoice(Voice& voice, float* outputBuffer, size_t frames, uint32_t outputSampleRate);

        AudioRingBuffer<AudioCommand, kCommandBufferSize> _commandBuffer;
        AudioVoicePool _voicePool;

        float _masterVolume = 1.0f;

        // Incremented from the game thread when the ring buffer is full.
        // Atomic so getStats() can read it safely from any thread
        std::atomic<uint64_t> _droppedCommands{ 0 };
    };

} // namespace OpenRCT2::Audio
