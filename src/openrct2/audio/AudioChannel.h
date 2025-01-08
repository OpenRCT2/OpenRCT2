/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "AudioMixer.h"

namespace OpenRCT2::Audio
{
    struct IAudioSource;

    /**
     * Represents an audio channel that represents an audio source
     * and a number of properties such as volume, pan and loop information.
     */
    struct IAudioChannel
    {
        virtual ~IAudioChannel() = default;

        virtual IAudioSource* GetSource() const = 0;

        virtual MixerGroup GetGroup() const = 0;
        virtual void SetGroup(MixerGroup group) = 0;

        virtual double GetRate() const = 0;
        virtual void SetRate(double rate) = 0;

        virtual uint64_t GetOffset() const = 0;
        virtual bool SetOffset(uint64_t offset) = 0;

        virtual int32_t GetLoop() const = 0;
        virtual void SetLoop(int32_t value) = 0;

        virtual int32_t GetVolume() const = 0;
        virtual float GetVolumeL() const = 0;
        virtual float GetVolumeR() const = 0;
        virtual float GetOldVolumeL() const = 0;
        virtual float GetOldVolumeR() const = 0;
        virtual int32_t GetOldVolume() const = 0;
        virtual void SetVolume(int32_t volume) = 0;

        virtual float GetPan() const = 0;
        virtual void SetPan(float pan) = 0;

        virtual bool IsStopping() const = 0;
        virtual void SetStopping(bool value) = 0;

        virtual bool IsDone() const = 0;
        virtual void SetDone(bool value) = 0;

        virtual bool DeleteOnDone() const = 0;
        virtual void SetDeleteOnDone(bool value) = 0;

        virtual bool IsPlaying() const = 0;

        virtual void Play(IAudioSource* source, int32_t loop = 0) = 0;
        virtual void Stop() = 0;
        virtual void UpdateOldVolume() = 0;

        virtual size_t Read(void* dst, size_t len) = 0;
    };
} // namespace OpenRCT2::Audio
