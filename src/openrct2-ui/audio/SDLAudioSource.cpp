/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SDLAudioSource.h"

#include <openrct2/Context.h>
#include <openrct2/audio/AudioContext.h>
#include <stdexcept>

using namespace OpenRCT2::Audio;

static uint32_t ReadU32LE(SDL_IOStream* rw)
{
    Uint32 value{};
    SDL_ReadU32LE(rw, &value);
    return value;
}

static uint16_t ReadU16LE(SDL_IOStream* rw)
{
    Uint16 value{};
    SDL_ReadU16LE(rw, &value);
    return value;
}

enum class AudioCodecKind
{
    unknown,
    wav,
    ogg,
    flac,
};

bool SDLAudioSource::IsReleased() const
{
    return _released;
}

void SDLAudioSource::Release()
{
    if (!_released)
    {
        // Lock the mixer to make sure we aren't mixing
        // the source as we dispose it
        auto mixer = GetMixer();
        if (mixer != nullptr)
            mixer->Lock();

        Unload();

        if (mixer != nullptr)
            mixer->Unlock();

        _released = true;
    }
}

IAudioMixer* SDLAudioSource::GetMixer()
{
    auto ctx = GetContext();
    if (ctx == nullptr)
        return nullptr;

    auto& audioContext = ctx->GetAudioContext();
    return audioContext.GetMixer();
}

int32_t SDLAudioSource::GetBytesPerSecond() const
{
    auto format = GetFormat();
    return format.GetBytesPerSecond();
}

std::unique_ptr<SDLAudioSource> SDLAudioSource::ToMemory(const AudioFormat& target)
{
    auto pcmLength = GetLength();

    std::vector<uint8_t> pcmData;
    pcmData.resize(pcmLength);
    Read(pcmData.data(), 0, pcmLength);

    auto srcFormat = GetFormat();
    return CreateMemoryAudioSource(target, srcFormat, std::move(pcmData));
}

static AudioCodecKind GetAudioCodec(SDL_IOStream* rw)
{
    constexpr uint32_t kMagicFLAC = 0x43614C66;
    constexpr uint32_t kMagicOGG = 0x5367674F;
    constexpr uint32_t kMagicRIFF = 0x46464952;

    auto originalPosition = SDL_TellIO(rw);
    auto magic = ReadU32LE(rw);
    SDL_SeekIO(rw, originalPosition, SDL_IO_SEEK_SET);
    switch (magic)
    {
        case kMagicFLAC:
            return AudioCodecKind::flac;
        case kMagicOGG:
            return AudioCodecKind::ogg;
        case kMagicRIFF:
            return AudioCodecKind::wav;
        default:
            return AudioCodecKind::unknown;
    }
}

std::unique_ptr<SDLAudioSource> OpenRCT2::Audio::CreateAudioSource(SDL_IOStream* rw)
{
    auto codec = GetAudioCodec(rw);
    switch (codec)
    {
        case AudioCodecKind::flac:
            return CreateFlacAudioSource(rw);
        case AudioCodecKind::ogg:
            return CreateOggAudioSource(rw);
        case AudioCodecKind::wav:
            return CreateWavAudioSource(rw);
        default:
            throw std::runtime_error("Unsupported audio codec");
    }
}

std::unique_ptr<SDLAudioSource> OpenRCT2::Audio::CreateAudioSource(SDL_IOStream* rw, uint32_t cssIndex)
{
    auto numSounds = ReadU32LE(rw);
    if (cssIndex >= numSounds)
    {
        // Not enough sounds, caller is responsible for freeing rw
        return nullptr;
    }

    SDL_SeekIO(rw, cssIndex * 4, SDL_IO_SEEK_CUR);

    auto pcmOffset = ReadU32LE(rw);
    SDL_SeekIO(rw, pcmOffset, SDL_IO_SEEK_SET);

    auto pcmLength = ReadU32LE(rw);

    AudioFormat format;
    // encoding, 16 bits
    SDL_SeekIO(rw, 2, SDL_IO_SEEK_CUR);
    format.channels = ReadU16LE(rw);
    format.freq = ReadU32LE(rw);
    // byterate, 32 bits
    // blockalign, 16 bits
    SDL_SeekIO(rw, 6, SDL_IO_SEEK_CUR);
    auto bitspersample = ReadU16LE(rw);
    switch (bitspersample)
    {
        case 8:
            format.format = SDL_AUDIO_U8;
            break;
        case 16:
            format.format = SDL_AUDIO_S16LE;
            break;
        default:
            throw std::runtime_error("Unsupported bits per sample");
    }
    // extrasize, 16 bits
    SDL_SeekIO(rw, 2, SDL_IO_SEEK_CUR);

    std::vector<uint8_t> pcmData;
    pcmData.resize(pcmLength);
    SDL_ReadIO(rw, pcmData.data(), pcmLength);

    return CreateMemoryAudioSource(format, format, std::move(pcmData));
}
