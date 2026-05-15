/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "DualAudioContext.h"

#include <openrct2/Diagnostic.h>
#include <openrct2/config/Config.h>

namespace OpenRCT2::Audio
{
    DualAudioContext::DualAudioContext()
    {
        _legacy = CreateAudioContext();
    }

    DualAudioContext::~DualAudioContext()
    {
        _newEngine.reset();
        _legacy.reset();
    }

    IAudioContext* DualAudioContext::active()
    {
        if (_useNewEngine && _newEngine)
            return _newEngine.get();
        return _legacy.get();
    }

    const IAudioContext* DualAudioContext::active() const
    {
        if (_useNewEngine && _newEngine)
            return _newEngine.get();
        return _legacy.get();
    }

    IAudioMixer* DualAudioContext::GetMixer()
    {
        return active()->GetMixer();
    }

    std::vector<std::string> DualAudioContext::GetOutputDevices()
    {
        return active()->GetOutputDevices();
    }

    void DualAudioContext::SetOutputDevice(const std::string& deviceName)
    {
        if (Config::Get().sound.audioEngineType == AudioEngineType::newEngine)
        {
            if (!_newEngine)
            {
                LOG_INFO("Config requests new audio engine, initializing...");
                _newEngine = std::make_unique<NewAudioContext>();
            }
            _useNewEngine = true;
            _newEngine->SetOutputDevice(deviceName);
            LOG_INFO("New audio engine is now active");
        }
        else
        {
            _useNewEngine = false;
            _legacy->SetOutputDevice(deviceName);
        }
    }

    IAudioSource* DualAudioContext::CreateStreamFromCSS(std::unique_ptr<IStream> stream, uint32_t index)
    {
        return active()->CreateStreamFromCSS(std::move(stream), index);
    }

    IAudioSource* DualAudioContext::CreateStreamFromWAV(std::unique_ptr<IStream> stream)
    {
        return active()->CreateStreamFromWAV(std::move(stream));
    }

    void DualAudioContext::StartTitleMusic()
    {
        active()->StartTitleMusic();
    }

    void DualAudioContext::ToggleAllSounds()
    {
        active()->ToggleAllSounds();
    }

    void DualAudioContext::PauseSounds()
    {
        active()->PauseSounds();
    }

    void DualAudioContext::UnpauseSounds()
    {
        active()->UnpauseSounds();
    }

    void DualAudioContext::StopAll()
    {
        active()->StopAll();
    }

    void DualAudioContext::StopCrowdSound()
    {
        active()->StopCrowdSound();
    }

    void DualAudioContext::StopRideMusic()
    {
        active()->StopRideMusic();
    }

    void DualAudioContext::StopTitleMusic()
    {
        active()->StopTitleMusic();
    }

    void DualAudioContext::StopVehicleSounds()
    {
        active()->StopVehicleSounds();
    }

    bool DualAudioContext::IsNewEngine() const
    {
        return _useNewEngine && _newEngine != nullptr;
    }

    void DualAudioContext::PlayOneShot(IAudioSource* source, float volume, float pan)
    {
        if (_useNewEngine && _newEngine)
            _newEngine->PlayOneShot(source, volume, pan);
    }

    std::shared_ptr<IAudioChannel> DualAudioContext::CreateChannel(
        IAudioSource* source, MixerGroup group, bool loop, int32_t volume, float pan, double rate)
    {
        if (_useNewEngine && _newEngine)
            return _newEngine->CreateChannel(source, group, loop, volume, pan, rate);
        return nullptr;
    }

    std::unique_ptr<IAudioContext> createDualAudioContext()
    {
        return std::make_unique<DualAudioContext>();
    }

} // namespace OpenRCT2::Audio
