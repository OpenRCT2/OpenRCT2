#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "AudioSource.h"

namespace OpenRCT2 { namespace Audio
{
    /**
     * An audio source representing silence.
     */
    class NullAudioSource : public IAudioSource
    {
    public:
        uint64 GetLength() override
        {
            return 0;
        }

        size_t Read(void * dst, uint64 offset, size_t len) override
        {
            return 0;
        }
    };

    IAudioSource * AudioSource::CreateNull()
    {
        return new NullAudioSource();
    }
} }
