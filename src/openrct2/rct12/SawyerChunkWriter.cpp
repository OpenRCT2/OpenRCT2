/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SawyerChunkWriter.h"

#include "../core/IStream.hpp"
#include "../core/Numerics.hpp"
#include "../util/SawyerCoding.h"

// Maximum buffer size to store compressed data, maximum of 16 MiB
constexpr size_t MAX_COMPRESSED_CHUNK_SIZE = 16 * 1024 * 1024;

SawyerChunkWriter::SawyerChunkWriter(OpenRCT2::IStream* stream)
    : _stream(stream)
{
}

void SawyerChunkWriter::WriteChunk(const SawyerChunk* chunk)
{
    WriteChunk(chunk->GetData(), chunk->GetLength(), chunk->GetEncoding());
}

void SawyerChunkWriter::WriteChunk(const void* src, size_t length, SAWYER_ENCODING encoding)
{
    sawyercoding_chunk_header header;
    header.encoding = static_cast<uint8_t>(encoding);
    header.length = static_cast<uint32_t>(length);

    auto data = std::make_unique<uint8_t[]>(MAX_COMPRESSED_CHUNK_SIZE);
    size_t dataLength = SawyerCodingWriteChunkBuffer(data.get(), static_cast<const uint8_t*>(src), header);

    _stream->Write(data.get(), dataLength);
}

/**
 * Ensure dst_buffer is bigger than src_buffer then resize afterwards
 * returns length of dst_buffer
 */
static size_t EncodeChunkRLE(const uint8_t* src_buffer, uint8_t* dst_buffer, size_t length)
{
    const uint8_t* src = src_buffer;
    uint8_t* dst = dst_buffer;
    const uint8_t* end_src = src + length;
    uint8_t count = 0;
    const uint8_t* src_norm_start = src;

    while (src < end_src - 1)
    {
        if ((count && *src == src[1]) || count > 125)
        {
            *dst++ = count - 1;
            std::memcpy(dst, src_norm_start, count);
            dst += count;
            src_norm_start += count;
            count = 0;
        }
        if (*src == src[1])
        {
            for (; (count < 125) && ((src + count) < end_src); count++)
            {
                if (*src != src[count])
                    break;
            }
            *dst++ = 257 - count;
            *dst++ = *src;
            src += count;
            src_norm_start = src;
            count = 0;
        }
        else
        {
            count++;
            src++;
        }
    }
    if (src == end_src - 1)
        count++;
    if (count)
    {
        *dst++ = count - 1;
        std::memcpy(dst, src_norm_start, count);
        dst += count;
    }
    return dst - dst_buffer;
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
