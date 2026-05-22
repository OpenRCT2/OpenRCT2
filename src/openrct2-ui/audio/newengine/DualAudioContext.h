/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "NewAudioContext.h"

#include <memory>
#include <openrct2/audio/AudioContext.h>

namespace OpenRCT2::Audio
{
    [[nodiscard]] std::unique_ptr<IAudioContext> CreateAudioContext();

    class DualAudioContext final : public IAudioContext
    {
    public:
        DualAudioContext();
        ~DualAudioContext() override;

        IAudioMixer* GetMixer() override;
        std::vector<std::string> GetOutputDevices() override;
        void SetOutputDevice(const std::string& deviceName) override;

        IAudioSource* CreateStreamFromCSS(std::unique_ptr<IStream> stream, uint32_t index) override;
        IAudioSource* CreateStreamFromWAV(std::unique_ptr<IStream> stream) override;

        void StartTitleMusic() override;
        void ToggleAllSounds() override;
        void PauseSounds() override;
        void UnpauseSounds() override;
        void StopAll() override;
        void StopCrowdSound() override;
        void StopRideMusic() override;
        void StopTitleMusic() override;
        void StopVehicleSounds() override;

        bool IsNewEngine() const override;

        void PlayOneShot(IAudioSource* source, float volume, float pan) override;
        void SyncVolumeSettings() override;
        void SwitchAudioEngine() override;
        bool HandleAudioDeviceEvent(uint32_t eventType, uint32_t deviceIndex, bool isCapture) override;
        AudioStreamInfo ProbeStream(std::unique_ptr<IStream> stream) override;

        std::shared_ptr<IAudioChannel> CreateChannel(
            IAudioSource* source, MixerGroup group, bool loop, int32_t volume, float pan, double rate) override;

    private:
        IAudioContext* active();
        const IAudioContext* active() const;

        std::unique_ptr<IAudioContext> _legacy;
        std::unique_ptr<NewAudioContext> _newEngine;
        bool _useNewEngine = false;
    };

    [[nodiscard]] std::unique_ptr<IAudioContext> createDualAudioContext();

} // namespace OpenRCT2::Audio
