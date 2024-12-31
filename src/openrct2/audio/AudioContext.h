/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

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

        virtual IAudioMixer* GetMixer() = 0;

        virtual std::vector<std::string> GetOutputDevices() = 0;
        virtual void SetOutputDevice(const std::string& deviceName) = 0;

        virtual IAudioSource* CreateStreamFromCSS(std::unique_ptr<IStream> stream, uint32_t index) = 0;
        virtual IAudioSource* CreateStreamFromWAV(std::unique_ptr<IStream> stream) = 0;

        virtual void StartTitleMusic() = 0;

        virtual void ToggleAllSounds() = 0;
        virtual void PauseSounds() = 0;
        virtual void UnpauseSounds() = 0;

        virtual void StopAll() = 0;
        virtual void StopCrowdSound() = 0;
        virtual void StopRideMusic() = 0;
        virtual void StopTitleMusic() = 0;
        virtual void StopVehicleSounds() = 0;
    };

    [[nodiscard]] std::unique_ptr<IAudioContext> CreateDummyAudioContext();

} // namespace OpenRCT2::Audio
