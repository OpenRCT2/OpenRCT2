/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioContext.h"

namespace OpenRCT2::Audio
{
    class DummyAudioContext final : public IAudioContext
    {
        IAudioMixer* GetMixer() override
        {
            return nullptr;
        }

        std::vector<std::string> GetOutputDevices() override
        {
            return std::vector<std::string>();
        }
        void SetOutputDevice(const std::string& /*deviceName*/) override
        {
        }

        IAudioSource* CreateStreamFromCSS(std::unique_ptr<IStream> /* stream */, uint32_t /* index */) override
        {
            return nullptr;
        }

        IAudioSource* CreateStreamFromWAV(std::unique_ptr<IStream>) override
        {
            return nullptr;
        }

        void StartTitleMusic() override
        {
        }

        void ToggleAllSounds() override
        {
        }
        void PauseSounds() override
        {
        }
        void UnpauseSounds() override
        {
        }

        void StopAll() override
        {
        }
        void StopCrowdSound() override
        {
        }
        void StopRideMusic() override
        {
        }
        void StopTitleMusic() override
        {
        }
        void StopVehicleSounds() override
        {
        }
    };

    std::unique_ptr<IAudioContext> CreateDummyAudioContext()
    {
        return std::make_unique<DummyAudioContext>();
    }
} // namespace OpenRCT2::Audio
