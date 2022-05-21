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
#include "../core/IStream.hpp"

#include <memory>
#include <string>
#include <vector>

namespace OpenRCT2::Audio
{
    struct IAudioChannel;
    struct IAudioMixer;
    struct IAudioSource;

    /**
     * Audio services for playing music and sound effects.
     */
    struct IAudioContext
    {
        virtual ~IAudioContext() = default;

        virtual IAudioMixer* GetMixer() abstract;

        virtual std::vector<std::string> GetOutputDevices() abstract;
        virtual void SetOutputDevice(const std::string& deviceName) abstract;

        virtual IAudioSource* CreateStreamFromWAV(const std::string& path) abstract;
        virtual IAudioSource* CreateStreamFromWAV(std::unique_ptr<IStream> stream) abstract;

        virtual void StartTitleMusic() abstract;

        virtual IAudioChannel* PlaySound(int32_t soundId, int32_t volume, int32_t pan) abstract;
        virtual IAudioChannel* PlaySoundAtLocation(int32_t soundId, int16_t x, int16_t y, int16_t z) abstract;
        virtual IAudioChannel* PlaySoundPanned(int32_t soundId, int32_t pan, int16_t x, int16_t y, int16_t z) abstract;

        virtual void ToggleAllSounds() abstract;
        virtual void PauseSounds() abstract;
        virtual void UnpauseSounds() abstract;

        virtual void StopAll() abstract;
        virtual void StopCrowdSound() abstract;
        virtual void StopWeatherSound() abstract;
        virtual void StopRideMusic() abstract;
        virtual void StopTitleMusic() abstract;
        virtual void StopVehicleSounds() abstract;
    };

    [[nodiscard]] std::unique_ptr<IAudioContext> CreateDummyAudioContext();

} // namespace OpenRCT2::Audio
