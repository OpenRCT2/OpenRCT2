/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#pragma once

#include "../common.h"

namespace OpenRCT2::Audio
{
    interface IAudioSource;

    /**
     * Represents an audio channel that represents an audio source
     * and a number of properties such as volume, pan and loop information.
     */
    interface IAudioChannel
    {
        virtual ~IAudioChannel() = default;

        virtual IAudioSource * GetSource() const abstract;

        virtual sint32  GetGroup() const abstract;
        virtual void    SetGroup(sint32 group) abstract;

        virtual double  GetRate() const abstract;
        virtual void    SetRate(double rate) abstract;

        virtual uint64  GetOffset() const abstract;
        virtual bool    SetOffset(uint64 offset) abstract;

        virtual sint32  GetLoop() const abstract;
        virtual void    SetLoop(sint32 value) abstract;

        virtual sint32  GetVolume() const abstract;
        virtual float   GetVolumeL() const abstract;
        virtual float   GetVolumeR() const abstract;
        virtual float   GetOldVolumeL() const abstract;
        virtual float   GetOldVolumeR() const abstract;
        virtual sint32  GetOldVolume() const abstract;
        virtual void    SetVolume(sint32 volume) abstract;

        virtual float   GetPan() const abstract;
        virtual void    SetPan(float pan) abstract;

        virtual bool IsStopping() const abstract;
        virtual void SetStopping(bool value) abstract;

        virtual bool IsDone() const abstract;
        virtual void SetDone(bool value) abstract;

        virtual bool DeleteOnDone() const abstract;
        virtual void SetDeleteOnDone(bool value) abstract;

        virtual void SetDeleteSourceOnDone(bool value) abstract;

        virtual bool IsPlaying() const abstract;

        virtual void Play(IAudioSource * source, sint32 loop = 0) abstract;
        virtual void UpdateOldVolume() abstract;

        virtual size_t Read(void * dst, size_t len) abstract;
    };
} // namespace OpenRCT2::Audio
