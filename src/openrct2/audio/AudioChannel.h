/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
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

        virtual IAudioSource* GetSource() const abstract;

        virtual MixerGroup GetGroup() const abstract;
        virtual void SetGroup(MixerGroup group) abstract;

        virtual double GetRate() const abstract;
        virtual void SetRate(double rate) abstract;

        virtual uint64_t GetOffset() const abstract;
        virtual bool SetOffset(uint64_t offset) abstract;

        virtual int32_t GetLoop() const abstract;
        virtual void SetLoop(int32_t value) abstract;

        virtual int32_t GetVolume() const abstract;
        virtual float GetVolumeL() const abstract;
        virtual float GetVolumeR() const abstract;
        virtual float GetOldVolumeL() const abstract;
        virtual float GetOldVolumeR() const abstract;
        virtual int32_t GetOldVolume() const abstract;
        virtual void SetVolume(int32_t volume) abstract;

        virtual float GetPan() const abstract;
        virtual void SetPan(float pan) abstract;

        virtual bool IsStopping() const abstract;
        virtual void SetStopping(bool value) abstract;

        virtual bool IsDone() const abstract;
        virtual void SetDone(bool value) abstract;

        virtual bool DeleteOnDone() const abstract;
        virtual void SetDeleteOnDone(bool value) abstract;

        virtual bool IsPlaying() const abstract;

        virtual void Play(IAudioSource* source, int32_t loop = 0) abstract;
        virtual void Stop() abstract;
        virtual void UpdateOldVolume() abstract;

        virtual size_t Read(void* dst, size_t len) abstract;
    };
} // namespace OpenRCT2::Audio
