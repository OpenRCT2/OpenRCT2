/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "AudioMixer.h"

namespace OpenRCT2::Audio
{
    /**
     * Represents a readable source of audio PCM data.
     */
    struct IAudioSource
    {
        virtual ~IAudioSource() = default;

        virtual void Release() abstract;
        virtual bool IsReleased() const abstract;
        virtual int32_t GetBytesPerSecond() const abstract;
        virtual uint64_t GetLength() const abstract;
        virtual size_t Read(void* dst, uint64_t offset, size_t len) abstract;
    };
} // namespace OpenRCT2::Audio
