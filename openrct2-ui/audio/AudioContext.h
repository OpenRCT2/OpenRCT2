/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <openrct2/audio/AudioChannel.h>
#include <openrct2/audio/AudioSource.h>
#include <openrct2/common.h>
#include <string>

struct SDL_RWops;
using SpeexResamplerState = struct SpeexResamplerState_;

namespace OpenRCT2::Audio
{
    struct AudioFormat;
    struct IAudioContext;

#pragma pack(push, 1)
    struct WaveFormat
    {
        uint16_t encoding;
        uint16_t channels;
        uint32_t frequency;
        uint32_t byterate;
        uint16_t blockalign;
        uint16_t bitspersample;
    };
    assert_struct_size(WaveFormat, 16);

    struct WaveFormatEx
    {
        uint16_t encoding;
        uint16_t channels;
        uint32_t frequency;
        uint32_t byterate;
        uint16_t blockalign;
        uint16_t bitspersample;
        uint16_t extrasize;
    };
    assert_struct_size(WaveFormatEx, 18);
#pragma pack(pop)

    struct ISDLAudioSource : public IAudioSource
    {
        [[nodiscard]] virtual AudioFormat GetFormat() const abstract;
    };

    struct ISDLAudioChannel : public IAudioChannel
    {
        [[nodiscard]] virtual AudioFormat GetFormat() const abstract;
        [[nodiscard]] virtual SpeexResamplerState* GetResampler() const abstract;
        virtual void SetResampler(SpeexResamplerState* value) abstract;
    };

    namespace AudioSource
    {
        IAudioSource* CreateMemoryFromCSS1(const std::string& path, size_t index, const AudioFormat* targetFormat = nullptr);
        IAudioSource* CreateMemoryFromWAV(const std::string& path, const AudioFormat* targetFormat = nullptr);
        IAudioSource* CreateStreamFromWAV(const std::string& path);
        IAudioSource* CreateStreamFromWAV(SDL_RWops* rw);
        IAudioSource* CreateStreamFromWAV(std::unique_ptr<IStream> stream);
    } // namespace AudioSource

    namespace AudioChannel
    {
        ISDLAudioChannel* Create();
    }

    namespace AudioMixer
    {
        IAudioMixer* Create();
    }

    [[nodiscard]] std::unique_ptr<IAudioContext> CreateAudioContext();

} // namespace OpenRCT2::Audio
