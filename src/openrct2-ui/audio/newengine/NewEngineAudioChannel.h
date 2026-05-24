/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "AudioEngine.h"
#include "AudioEngineCommand.h"

#include <atomic>
#include <openrct2/audio/AudioChannel.h>
#include <openrct2/audio/AudioMixer.h>
#include <openrct2/audio/AudioSource.h>

namespace OpenRCT2::Audio
{
    class NewEngineAudioChannel final : public IAudioChannel
    {
    public:
        NewEngineAudioChannel(
            AudioEngine* engine, AudioHandle handle, MixerGroup group, uint8_t channels, uint64_t lengthInFrames,
            size_t sourceBytesPerFrame);
        ~NewEngineAudioChannel() override;

        IAudioSource* GetSource() const override;
        MixerGroup GetGroup() const override;
        void SetGroup(MixerGroup group) override;
        double GetRate() const override;
        void SetRate(double rate) override;
        uint64_t GetOffset() const override;
        bool SetOffset(uint64_t offset) override;
        int32_t GetLoop() const override;
        void SetLoop(int32_t value) override;
        int32_t GetVolume() const override;
        float GetVolumeL() const override;
        float GetVolumeR() const override;
        float GetOldVolumeL() const override;
        float GetOldVolumeR() const override;
        int32_t GetOldVolume() const override;
        void SetVolume(int32_t volume) override;
        float GetPan() const override;
        void SetPan(float pan) override;
        bool IsStopping() const override;
        void SetStopping(bool value) override;
        bool IsDone() const override;
        void SetDone(bool value) override;
        bool DeleteOnDone() const override;
        void SetDeleteOnDone(bool value) override;
        bool IsPlaying() const override;
        void Play(IAudioSource* source, int32_t loop) override;
        void Stop() override;
        void UpdateOldVolume() override;
        size_t Read(void* dst, size_t len) override;

    private:
        AudioEngine* _engine;
        AudioHandle _handle;
        MixerGroup _group;

        int32_t _volume = kMixerVolumeMax;
        int32_t _oldVolume = kMixerVolumeMax;
        float _pan = 0.5f;
        double _rate = 1.0;
        bool _stopping = false;
        bool _done = false;
        bool _deleteOnDone = false;
        float _volumeL = 0.5f;
        float _volumeR = 0.5f;
        float _oldVolumeL = 0.5f;
        float _oldVolumeR = 0.5f;
        mutable std::atomic<bool> _pendingActivation{ true };

        uint8_t _channels = 1;
        uint64_t _lengthInFrames = 0;
        size_t _sourceBytesPerFrame = 2;
    };

} // namespace OpenRCT2::Audio
