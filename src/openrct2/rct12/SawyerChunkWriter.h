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
 * Writes sawyer encoding chunks to a data stream. This can be used to write
 * SC6 and SV6 files.
 */
class SawyerChunkWriter final
{
private:
    IStream * const _stream = nullptr;

public:
    SawyerChunkWriter(IStream * stream);

    /**
     * Writes a chunk to the stream.
     */
    void WriteChunk(const SawyerChunk * chunk);

    /**
     * Writes a chunk to the stream containing the given buffer.
     * @param dst The source buffer.
     * @param length The size of the source buffer.
     */
    void WriteChunk(const void * src, size_t length, SAWYER_ENCODING encoding);

    /**
     * Writes a chunk to the stream containing the given type.
     */
    template<typename T>
    void WriteChunk(const T * src, SAWYER_ENCODING encoding)
    {
        WriteChunk(src, sizeof(T), encoding);
    }
};
