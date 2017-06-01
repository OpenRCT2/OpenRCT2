#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include <memory>
#include "../common.h"
#include "SawyerChunk.h"

interface IStream;

/**
 * Reads sawyer encoding chunks from a data stream. This can be used to read
 * SC6, SV6 and RCT2 objects.
 */
class SawyerChunkReader final
{
private:
    IStream * const _stream = nullptr;

public:
    SawyerChunkReader(IStream * stream);

    /**
     * Skips the next chunk in the stream without decoding or reading its data
     * into RAM.
     */
    void SkipChunk();

    /**
     * Reads the next chunk from the stream.
     */
    std::shared_ptr<SawyerChunk> ReadChunk();

    /**
     * Reads the next chunk from the stream and copies it directly to the
     * destination buffer. If the chunk is larger than length, only length
     * is copied. If the chunk is smaller than length, the remaining space
     * is padded with zero.
     * @param dst The destination buffer.
     * @param length The size of the destination buffer.
     */
    void ReadChunk(void * dst, size_t length);

    /**
     * Reads the next chunk from the stream into a buffer returned as the
     * specified type. If the chunk is smaller than the size of the type
     * then the remaining space is padded with zero.
     */
    template<typename T>
    T ReadChunkAs()
    {
        T result;
        ReadChunk(&result, sizeof(result));
        return result;
    }
};
