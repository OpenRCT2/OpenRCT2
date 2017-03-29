
#pragma once

#include <string>
#include <openrct2/common.h>
#include <openrct2/audio/AudioChannel.h>
#include <openrct2/audio/AudioSource.h>

struct SDL_RWops;
struct SpeexResamplerState_;
typedef struct SpeexResamplerState_ SpeexResamplerState;

namespace OpenRCT2 { namespace Audio
{
    struct AudioFormat;

#pragma pack(push, 1)
    struct WaveFormat
    {
        uint16 encoding;
        uint16 channels;
        uint32 frequency;
        uint32 byterate;
        uint16 blockalign;
        uint16 bitspersample;
    };
    assert_struct_size(WaveFormat, 16);

    struct WaveFormatEx
    {
        uint16 encoding;
        uint16 channels;
        uint32 frequency;
        uint32 byterate;
        uint16 blockalign;
        uint16 bitspersample;
        uint16 extrasize;
    };
    assert_struct_size(WaveFormatEx, 18);
#pragma pack(pop)

    interface ISDLAudioSource : public IAudioSource
    {
        virtual AudioFormat GetFormat() const abstract;
    };

    interface ISDLAudioChannel : public IAudioChannel
    {
        virtual AudioFormat GetFormat() const abstract;
        virtual SpeexResamplerState * GetResampler() const abstract;
        virtual void SetResampler(SpeexResamplerState * value) abstract;
    };

    namespace AudioSource
    {
        IAudioSource * CreateMemoryFromCSS1(const std::string &path, size_t index, const AudioFormat * targetFormat = nullptr);
        IAudioSource * CreateMemoryFromWAV(const std::string &path, const AudioFormat * targetFormat = nullptr);
        IAudioSource * CreateStreamFromWAV(const std::string &path);
        IAudioSource * CreateStreamFromWAV(SDL_RWops * rw);
    }

    namespace AudioChannel
    {
        ISDLAudioChannel * Create();
    }

    namespace AudioMixer
    {
        IAudioMixer * Create();
    }
} }
