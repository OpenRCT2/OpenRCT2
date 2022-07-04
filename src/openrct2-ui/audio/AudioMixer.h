/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "AudioContext.h"
#include "AudioFormat.h"
#include "SDLAudioSource.h"

#include <SDL.h>
#include <cstdint>
#include <list>
#include <mutex>
#include <openrct2/Context.h>
#include <openrct2/audio/AudioChannel.h>
#include <openrct2/audio/AudioMixer.h>
#include <openrct2/audio/AudioSource.h>
#include <openrct2/audio/audio.h>
#include <vector>

namespace OpenRCT2::Audio
{
    class AudioMixer final : public IAudioMixer
    {
    private:
        std::vector<std::unique_ptr<SDLAudioSource>> _sources;

        SDL_AudioDeviceID _deviceId = 0;
        AudioFormat _format = {};
        std::list<std::shared_ptr<ISDLAudioChannel>> _channels;
        float _volume = 1.0f;
        float _adjustSoundVolume = 0.0f;
        float _adjustMusicVolume = 0.0f;
        uint8_t _settingSoundVolume = 0xFF;
        uint8_t _settingMusicVolume = 0xFF;

        std::vector<uint8_t> _channelBuffer;
        std::vector<uint8_t> _convertBuffer;
        std::vector<uint8_t> _effectBuffer;

        std::mutex _mutex;

    public:
        ~AudioMixer() override;
        void Init(const char* device) override;
        void Close() override;
        void Lock() override;
        void Unlock() override;
        std::shared_ptr<IAudioChannel> Play(IAudioSource* source, int32_t loop, bool deleteondone) override;
        void SetVolume(float volume) override;
        SDLAudioSource* AddSource(std::unique_ptr<SDLAudioSource> source);

        const AudioFormat& GetFormat() const;

    private:
        void GetNextAudioChunk(uint8_t* dst, size_t length);
        void UpdateAdjustedSound();
        void MixChannel(ISDLAudioChannel* channel, uint8_t* data, size_t length);
        void RemoveReleasedSources();

        /**
         * Resample the given buffer into _effectBuffer.
         * Assumes that srcBuffer is the same format as _format.
         */
        size_t ApplyResample(
            ISDLAudioChannel* channel, const void* srcBuffer, int32_t srcSamples, int32_t dstSamples, int32_t inRate,
            int32_t outRate);
        void ApplyPan(const IAudioChannel* channel, void* buffer, size_t len, size_t sampleSize);
        int32_t ApplyVolume(const IAudioChannel* channel, void* buffer, size_t len);
        static void EffectPanS16(const IAudioChannel* channel, int16_t* data, int32_t length);
        static void EffectPanU8(const IAudioChannel* channel, uint8_t* data, int32_t length);
        static void EffectFadeS16(int16_t* data, int32_t length, int32_t startvolume, int32_t endvolume);
        static void EffectFadeU8(uint8_t* data, int32_t length, int32_t startvolume, int32_t endvolume);
        bool Convert(SDL_AudioCVT* cvt, const void* src, size_t len);
    };
} // namespace OpenRCT2::Audio
