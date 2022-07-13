/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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

enum class AudioCodecKind
{
    Unknown,
    Wav,
    Ogg,
    Flac,
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

    auto audioContext = ctx->GetAudioContext();
    if (audioContext == nullptr)
        return nullptr;

    return audioContext->GetMixer();
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

static AudioCodecKind GetAudioCodec(SDL_RWops* rw)
{
    constexpr uint32_t MAGIC_FLAC = 0x43614C66;
    constexpr uint32_t MAGIC_OGG = 0x5367674F;
    constexpr uint32_t MAGIC_RIFF = 0x46464952;

    auto originalPosition = SDL_RWtell(rw);
    auto magic = SDL_ReadLE32(rw);
    SDL_RWseek(rw, originalPosition, RW_SEEK_SET);
    switch (magic)
    {
        case MAGIC_FLAC:
            return AudioCodecKind::Flac;
        case MAGIC_OGG:
            return AudioCodecKind::Ogg;
        case MAGIC_RIFF:
            return AudioCodecKind::Wav;
        default:
            return AudioCodecKind::Unknown;
    }
}

std::unique_ptr<SDLAudioSource> OpenRCT2::Audio::CreateAudioSource(SDL_RWops* rw)
{
    auto codec = GetAudioCodec(rw);
    switch (codec)
    {
        case AudioCodecKind::Flac:
            return CreateFlacAudioSource(rw);
        case AudioCodecKind::Ogg:
            return CreateOggAudioSource(rw);
        case AudioCodecKind::Wav:
            return CreateWavAudioSource(rw);
        default:
            throw std::runtime_error("Unsupported audio codec");
    }
}

std::unique_ptr<SDLAudioSource> OpenRCT2::Audio::CreateAudioSource(SDL_RWops* rw, uint32_t cssIndex)
{
    auto numSounds = SDL_ReadLE32(rw);
    if (cssIndex < numSounds)
    {
        SDL_RWseek(rw, cssIndex * 4, RW_SEEK_CUR);

        auto pcmOffset = SDL_ReadLE32(rw);
        SDL_RWseek(rw, pcmOffset, RW_SEEK_SET);

        auto pcmLength = SDL_ReadLE32(rw);

        AudioFormat format;
        [[maybe_unused]] auto encoding = SDL_ReadLE16(rw);
        format.channels = SDL_ReadLE16(rw);
        format.freq = SDL_ReadLE32(rw);
        [[maybe_unused]] auto byterate = SDL_ReadLE32(rw);
        [[maybe_unused]] auto blockalign = SDL_ReadLE16(rw);
        [[maybe_unused]] auto bitspersample = SDL_ReadLE16(rw);
        switch (bitspersample)
        {
            case 8:
                format.format = AUDIO_U8;
                break;
            case 16:
                format.format = AUDIO_S16LSB;
                break;
            default:
                SDL_RWclose(rw);
                throw std::runtime_error("Unsupported bits per sample");
        }
        [[maybe_unused]] auto extrasize = SDL_ReadLE16(rw);

        std::vector<uint8_t> pcmData;
        pcmData.resize(pcmLength);
        SDL_RWread(rw, pcmData.data(), pcmLength, 1);

        SDL_RWclose(rw);
        return CreateMemoryAudioSource(format, format, std::move(pcmData));
    }
    else
    {
        SDL_RWclose(rw);
        throw std::runtime_error("CSS does not contain required entry");
    }
}
