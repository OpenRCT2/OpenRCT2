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

#include "../core/Exception.hpp"
#include "../core/IStream.hpp"
#include "../core/Math.hpp"
#include "SawyerChunkWriter.h"

#include "../util/sawyercoding.h"

// Maximum buffer size to store compressed data, maximum of 16 MiB
constexpr size_t MAX_COMPRESSED_CHUNK_SIZE = 16 * 1024 * 1024;

SawyerChunkWriter::SawyerChunkWriter(IStream * stream)
    : _stream(stream)
{
}

void SawyerChunkWriter::WriteChunk(const SawyerChunk * chunk)
{
    WriteChunk(chunk->GetData(), chunk->GetLength(), chunk->GetEncoding());
}

void SawyerChunkWriter::WriteChunk(const void * src, size_t length, SAWYER_ENCODING encoding)
{
    sawyercoding_chunk_header header;
    header.encoding = (uint8)encoding;
    header.length = (uint32)length;

    auto data = std::make_unique<uint8[]>(MAX_COMPRESSED_CHUNK_SIZE);
    size_t dataLength = sawyercoding_write_chunk_buffer(data.get(), (const uint8 *)src, header);

    _stream->Write(data.get(), dataLength);
}
