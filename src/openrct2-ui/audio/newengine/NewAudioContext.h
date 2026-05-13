/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "AudioEngine.h"
#include "AudioEngineCommand.h"
#include "AudioPlatformSDL2.h"

#include <cstdint>
#include <memory>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/audio/AudioSource.h>
#include <string>
#include <vector>

namespace OpenRCT2::Audio
{
    struct Float32AudioData
    {
        std::vector<float> samples;
        uint32_t sampleRate = 0;
        uint8_t channels = 0;

        [[nodiscard]] uint64_t lengthInFrames() const
        {
            if (channels == 0)
                return 0;
            return samples.size() / channels;
        }
    };

    class Float32AudioSource final : public IAudioSource
    {
    public:
        explicit Float32AudioSource(Float32AudioData* data);

        void Release() override;
        bool IsReleased() const override;
        int32_t GetBytesPerSecond() const override;
        uint64_t GetLength() const override;
        size_t Read(void* dst, uint64_t offset, size_t len) override;

        Float32AudioData* getData() const;

    private:
        Float32AudioData* _data;
        bool _released = false;
    };

    class NewAudioContext final : public IAudioContext
    {
    public:
        NewAudioContext();
        ~NewAudioContext() override;

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

        AudioEngine* getEngine();

        bool IsNewEngine() const override;
        bool isDeviceOpen() const;

        void PlayOneShot(IAudioSource* source, float volume, float pan) override;

    private:
        Float32AudioData* convertToFloat32(const void* pcmData, size_t pcmLen, AudioSampleFormat format, uint8_t srcChannels, int srcFreq);

        std::unique_ptr<AudioEngine> _engine;
        std::unique_ptr<AudioPlatformSDL2> _platform;

        std::vector<std::unique_ptr<Float32AudioData>> _audioData;
        std::vector<std::unique_ptr<Float32AudioSource>> _sources;
    };

} // namespace OpenRCT2::Audio
