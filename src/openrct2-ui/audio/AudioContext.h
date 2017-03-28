
#pragma once

#include <string>
#include <openrct2/common.h>

namespace OpenRCT2 { namespace Audio
{
    struct      AudioFormat;
    interface   IAudioSource;

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

    namespace AudioSource
    {
        IAudioSource * CreateMemoryFromCSS1(const std::string &path, size_t index, const AudioFormat * targetFormat = nullptr);
        IAudioSource * CreateMemoryFromWAV(const std::string &path, const AudioFormat * targetFormat = nullptr);
        IAudioSource * CreateStreamFromWAV(const std::string &path);
        IAudioSource * CreateStreamFromWAV(SDL_RWops * rw);
    }
} }
