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

#pragma once

#include "../common.h"

interface IStream;

namespace SawyerEncoding
{
    void SkipChunk(IStream * stream);
    void * ReadChunk(IStream * stream, size_t * outSize);
    void ReadChunk(void * dst, size_t expectedSize, IStream * stream);
    void ReadChunkTolerant(void * dst, size_t expectedSize, IStream * stream);
    bool TryReadChunk(void * dst, size_t expectedSize, IStream * stream);

    template<typename T>
    bool TryReadChunk(T * dst, IStream * stream)
    {
        return TryReadChunk(dst, sizeof(T), stream);
    }

    bool ValidateChecksum(IStream * stream);
}
