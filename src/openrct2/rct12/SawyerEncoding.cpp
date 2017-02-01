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

#include "../core/IStream.hpp"
#include "SawyerEncoding.h"

extern "C"
{
    #include "../util/sawyercoding.h"
}

namespace SawyerEncoding
{
    bool TryReadChunk(void * dst, size_t expectedSize, IStream * stream)
    {
        uint64 originalPosition = stream->GetPosition();

        bool success = false;
        auto header = stream->ReadValue<sawyercoding_chunk_header>();
        switch (header.encoding) {
        case CHUNK_ENCODING_NONE:
        case CHUNK_ENCODING_RLE:
        case CHUNK_ENCODING_RLECOMPRESSED:
        case CHUNK_ENCODING_ROTATE:
            uint8 * compressedData = Memory::Allocate<uint8>(header.length);
            if (stream->TryRead(compressedData, header.length) == header.length)
            {
                size_t uncompressedLength = sawyercoding_read_chunk_buffer((uint8 *)dst, compressedData, header, expectedSize);
                if (uncompressedLength == expectedSize)
                {
                    success = true;
                }
            }
            Memory::Free(compressedData);
            break;
        }

        if (!success)
        {
            stream->SetPosition(originalPosition);
        }
        return success;
    }
}
