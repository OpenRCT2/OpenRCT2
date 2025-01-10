/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "AudioMixer.h"

namespace OpenRCT2::Audio
{
    /**
     * Represents a readable source of audio PCM data.
     */
    struct IAudioSource
    {
        virtual ~IAudioSource() = default;

        virtual void Release() = 0;
        virtual bool IsReleased() const = 0;
        virtual int32_t GetBytesPerSecond() const = 0;
        virtual uint64_t GetLength() const = 0;
        virtual size_t Read(void* dst, uint64_t offset, size_t len) = 0;
    };
} // namespace OpenRCT2::Audio
