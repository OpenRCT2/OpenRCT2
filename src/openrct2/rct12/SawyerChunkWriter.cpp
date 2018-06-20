/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/IStream.hpp"
#include "../core/Math.hpp"
#include "SawyerChunkWriter.h"

#include "../util/SawyerCoding.h"

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
    header.encoding = (uint8_t)encoding;
    header.length = (uint32_t)length;

    auto data = std::make_unique<uint8_t[]>(MAX_COMPRESSED_CHUNK_SIZE);
    size_t dataLength = sawyercoding_write_chunk_buffer(data.get(), (const uint8_t *)src, header);

    _stream->Write(data.get(), dataLength);
}
