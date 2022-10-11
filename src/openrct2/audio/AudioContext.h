/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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

        virtual IAudioSource* CreateStreamFromCSS(std::unique_ptr<IStream> stream, uint32_t index) abstract;
        virtual IAudioSource* CreateStreamFromWAV(std::unique_ptr<IStream> stream) abstract;

        virtual void StartTitleMusic() abstract;

        virtual void ToggleAllSounds() abstract;
        virtual void PauseSounds() abstract;
        virtual void UnpauseSounds() abstract;

        virtual void StopAll() abstract;
        virtual void StopCrowdSound() abstract;
        virtual void StopRideMusic() abstract;
        virtual void StopTitleMusic() abstract;
        virtual void StopVehicleSounds() abstract;
    };

    [[nodiscard]] std::unique_ptr<IAudioContext> CreateDummyAudioContext();

} // namespace OpenRCT2::Audio
