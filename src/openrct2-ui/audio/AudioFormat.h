/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>
#include <SDL2/SDL.h>

namespace OpenRCT2::Audio
{
    /**
     * Represents the size, frequency and number of channels for
     * an audio stream or buffer.
     */
    struct AudioFormat
    {
        int32_t          freq;
        SDL_AudioFormat format;
        int32_t          channels;

        int32_t BytesPerSample() const
        {
            return (SDL_AUDIO_BITSIZE(format)) / 8;
        }

        int32_t GetByteRate() const
        {
            return BytesPerSample() * channels;
        }
    };

    inline bool operator ==(const AudioFormat& lhs, const AudioFormat& rhs)
    {
        return lhs.freq == rhs.freq &&
               lhs.format == rhs.format &&
               lhs.channels == rhs.channels;
    }

    inline bool operator !=(const AudioFormat& lhs, const AudioFormat& rhs)
    {
        return !(lhs == rhs);
    }
} // namespace OpenRCT2::Audio
