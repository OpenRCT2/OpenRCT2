/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SawyerChunkWriter.h"

#include "../core/IStream.hpp"
#include "../core/Numerics.hpp"
#include "../sawyer_coding/SawyerCoding.h"

namespace OpenRCT2::SawyerCoding
{
    // Maximum buffer size to store compressed data, maximum of 16 MiB
    constexpr size_t MAX_COMPRESSED_CHUNK_SIZE = 16 * 1024 * 1024;

    SawyerChunkWriter::SawyerChunkWriter(IStream* stream)
        : _stream(stream)
    {
    }

    void SawyerChunkWriter::WriteChunk(const SawyerChunk* chunk)
    {
        WriteChunk(chunk->GetData(), chunk->GetLength(), chunk->GetEncoding());
    }

    void SawyerChunkWriter::WriteChunk(const void* src, size_t length, ChunkEncoding encoding)
    {
        ChunkHeader header;
        header.encoding = encoding;
        header.length = static_cast<uint32_t>(length);

        auto data = std::make_unique<uint8_t[]>(MAX_COMPRESSED_CHUNK_SIZE);
        size_t dataLength = WriteChunkBuffer(data.get(), static_cast<const uint8_t*>(src), header);

        _stream->Write(data.get(), dataLength);
    }

    void SawyerChunkWriter::WriteChunkTrack(const void* src, size_t length)
    {
        auto data = std::make_unique<uint8_t[]>(MAX_COMPRESSED_CHUNK_SIZE);
        size_t dataLength = EncodeChunkRLE(static_cast<const uint8_t*>(src), data.get(), length);

        uint32_t checksum = 0;
        for (size_t i = 0; i < dataLength; i++)
        {
            uint8_t newByte = ((checksum & 0xFF) + data[i]) & 0xFF;
            checksum = (checksum & 0xFFFFFF00) + newByte;
            checksum = Numerics::rol32(checksum, 3);
        }
        checksum -= 0x1D4C1;

        _stream->Write(data.get(), dataLength);
        _stream->WriteValue<uint32_t>(checksum);
    }
} // namespace OpenRCT2::SawyerCoding
