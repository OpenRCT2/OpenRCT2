/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../common.h"

namespace OpenRCT2::Audio
{
    interface IAudioChannel;
    interface IAudioMixer;
    interface IAudioSource;

    /**
     * Audio services for playing music and sound effects.
     */
    interface IAudioContext
    {
        virtual ~IAudioContext() = default;

        virtual IAudioMixer * GetMixer() abstract;

        virtual std::vector<std::string> GetOutputDevices() abstract;
        virtual void SetOutputDevice(const std::string &deviceName) abstract;

        virtual IAudioSource * CreateStreamFromWAV(const std::string &path) abstract;


        virtual void StartTitleMusic() abstract;

        virtual IAudioChannel * PlaySound(sint32 soundId, sint32 volume, sint32 pan) abstract;
        virtual IAudioChannel * PlaySoundAtLocation(sint32 soundId, sint16 x, sint16 y, sint16 z) abstract;
        virtual IAudioChannel * PlaySoundPanned(sint32 soundId, sint32 pan, sint16 x, sint16 y, sint16 z) abstract;

        virtual void ToggleAllSounds() abstract;
        virtual void PauseSounds() abstract;
        virtual void UnpauseSounds() abstract;

        virtual void StopAll() abstract;
        virtual void StopCrowdSound() abstract;
        virtual void StopRainSound() abstract;
        virtual void StopRideMusic() abstract;
        virtual void StopTitleMusic() abstract;
        virtual void StopVehicleSounds() abstract;
    };

    std::unique_ptr<IAudioContext> CreateDummyAudioContext();

} // namespace OpenRCT2::Audio
