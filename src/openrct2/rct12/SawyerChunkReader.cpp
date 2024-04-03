/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SawyerChunkReader.h"

#include "../core/IStream.hpp"
#include "../core/Numerics.hpp"

// malloc is very slow for large allocations in MSVC debug builds as it allocates
// memory on a special debug heap and then initialises all the memory to 0xCC.
#if defined(_WIN32) && defined(DEBUG)
#    define __USE_HEAP_ALLOC__
#    include <windows.h>
#endif

// Allow chunks to be uncompressed to a maximum of 16 MiB
constexpr size_t kMaxUncompressedChunkSize = 16 * 1024 * 1024;

constexpr const char* kExceptionMsgCorruptChunkSize = "Corrupt chunk size.";
constexpr const char* kExceptionMsgCorruptRle = "Corrupt RLE compression data.";
constexpr const char* kExceptionMsgDestinationTooSmall = "Chunk data larger than allocated destination capacity.";
constexpr const char* kExceptionMsgInvalidChunkEncoding = "Invalid chunk encoding.";
constexpr const char* kExceptionMsgZeroSizedChunk = "Encountered zero-sized chunk.";

SawyerChunkReader::SawyerChunkReader(OpenRCT2::IStream* stream)
    : _stream(stream)
{
}

void SawyerChunkReader::SkipChunk()
{
    uint64_t originalPosition = _stream->GetPosition();
    try
    {
        auto header = _stream->ReadValue<SawyerCodingChunkHeader>();
        _stream->Seek(header.length, OpenRCT2::STREAM_SEEK_CURRENT);
    }
    catch (const std::exception&)
    {
        // Rewind stream back to original position
        _stream->SetPosition(originalPosition);
        throw;
    }
}

std::shared_ptr<SawyerChunk> SawyerChunkReader::ReadChunk()
{
    uint64_t originalPosition = _stream->GetPosition();
    try
    {
        auto header = _stream->ReadValue<SawyerCodingChunkHeader>();
        if (header.length >= kMaxUncompressedChunkSize)
            throw SawyerChunkException(kExceptionMsgCorruptChunkSize);

        switch (header.encoding)
        {
            case CHUNK_ENCODING_NONE:
            case CHUNK_ENCODING_RLE:
            case CHUNK_ENCODING_RLECOMPRESSED:
            case CHUNK_ENCODING_ROTATE:
            {
                auto compressedData = std::make_unique<uint8_t[]>(header.length);
                if (_stream->TryRead(compressedData.get(), header.length) != header.length)
                {
                    throw SawyerChunkException(kExceptionMsgCorruptChunkSize);
                }

                auto buffer = std::make_unique<uint8_t[]>(kMaxUncompressedChunkSize);
                size_t uncompressedLength = DecodeChunk(
                    buffer.get(), kMaxUncompressedChunkSize, compressedData.get(), header);
                if (uncompressedLength == 0)
                {
                    throw SawyerChunkException(kExceptionMsgZeroSizedChunk);
                }
                return std::make_shared<SawyerChunk>(
                    static_cast<SAWYER_ENCODING>(header.encoding), std::move(buffer), uncompressedLength);
            }
            default:
                throw SawyerChunkException(kExceptionMsgInvalidChunkEncoding);
        }
    }
    catch (const std::exception&)
    {
        // Rewind stream back to original position
        _stream->SetPosition(originalPosition);
        throw;
    }
}

std::shared_ptr<SawyerChunk> SawyerChunkReader::ReadChunkTrack()
{
    uint64_t originalPosition = _stream->GetPosition();
    try
    {
        // Remove 4 as we don't want to touch the checksum at the end of the file
        int64_t compressedDataLength64 = _stream->GetLength() - _stream->GetPosition() - 4;
        if (compressedDataLength64 < 0 || compressedDataLength64 > std::numeric_limits<uint32_t>::max())
        {
            throw SawyerChunkException(kExceptionMsgZeroSizedChunk);
        }
        uint32_t compressedDataLength = compressedDataLength64;
        auto compressedData = std::make_unique<uint8_t[]>(compressedDataLength);

        if (_stream->TryRead(compressedData.get(), compressedDataLength) != compressedDataLength)
        {
            throw SawyerChunkException(kExceptionMsgCorruptChunkSize);
        }

        auto buffer = std::make_unique<uint8_t[]>(kMaxUncompressedChunkSize);
        SawyerCodingChunkHeader header{ CHUNK_ENCODING_RLE, compressedDataLength };
        size_t uncompressedLength = DecodeChunk(buffer.get(), kMaxUncompressedChunkSize, compressedData.get(), header);
        if (uncompressedLength == 0)
        {
            throw SawyerChunkException(kExceptionMsgZeroSizedChunk);
        }
        return std::make_shared<SawyerChunk>(SAWYER_ENCODING::RLE, std::move(buffer), uncompressedLength);
    }
    catch (const std::exception&)
    {
        // Rewind stream back to original position
        _stream->SetPosition(originalPosition);
        throw;
    }
}

void SawyerChunkReader::ReadChunk(void* dst, size_t length)
{
    auto chunk = ReadChunk();
    auto chunkData = static_cast<const uint8_t*>(chunk->GetData());
    auto chunkLength = chunk->GetLength();
    if (chunkLength > length)
    {
        std::memcpy(dst, chunkData, length);
    }
    else
    {
        std::memcpy(dst, chunkData, chunkLength);
        auto remainingLength = length - chunkLength;
        if (remainingLength > 0)
        {
            auto offset = static_cast<uint8_t*>(dst) + chunkLength;
            std::fill_n(offset, remainingLength, 0x00);
        }
    }
}

size_t SawyerChunkReader::DecodeChunk(void* dst, size_t dstCapacity, const void* src, const SawyerCodingChunkHeader& header)
{
    size_t resultLength;
    switch (header.encoding)
    {
        case CHUNK_ENCODING_NONE:
            if (header.length > dstCapacity)
            {
                throw SawyerChunkException(kExceptionMsgDestinationTooSmall);
            }
            std::memcpy(dst, src, header.length);
            resultLength = header.length;
            break;
        case CHUNK_ENCODING_RLE:
            resultLength = DecodeChunkRLE(dst, dstCapacity, src, header.length);
            break;
        case CHUNK_ENCODING_RLECOMPRESSED:
            resultLength = DecodeChunkRLERepeat(dst, dstCapacity, src, header.length);
            break;
        case CHUNK_ENCODING_ROTATE:
            resultLength = DecodeChunkRotate(dst, dstCapacity, src, header.length);
            break;
        default:
            throw SawyerChunkException(kExceptionMsgInvalidChunkEncoding);
    }
    return resultLength;
}

size_t SawyerChunkReader::DecodeChunkRLERepeat(void* dst, size_t dstCapacity, const void* src, size_t srcLength)
{
    auto immBuffer = std::make_unique<uint8_t[]>(kMaxUncompressedChunkSize);
    auto immLength = DecodeChunkRLE(immBuffer.get(), kMaxUncompressedChunkSize, src, srcLength);
    auto size = DecodeChunkRepeat(dst, dstCapacity, immBuffer.get(), immLength);
    return size;
}

size_t SawyerChunkReader::DecodeChunkRLE(void* dst, size_t dstCapacity, const void* src, size_t srcLength)
{
    auto src8 = static_cast<const uint8_t*>(src);
    auto dst8 = static_cast<uint8_t*>(dst);
    auto dstEnd = dst8 + dstCapacity;
    for (size_t i = 0; i < srcLength; i++)
    {
        uint8_t rleCodeByte = src8[i];
        if (rleCodeByte & 128)
        {
            i++;
            size_t count = 257 - rleCodeByte;

            if (i >= srcLength)
            {
                throw SawyerChunkException(kExceptionMsgCorruptRle);
            }
            if (dst8 + count > dstEnd)
            {
                throw SawyerChunkException(kExceptionMsgDestinationTooSmall);
            }

            std::fill_n(dst8, count, src8[i]);
            dst8 += count;
        }
        else
        {
            if (i + 1 >= srcLength)
            {
                throw SawyerChunkException(kExceptionMsgCorruptRle);
            }
            if (dst8 + rleCodeByte + 1 > dstEnd)
            {
                throw SawyerChunkException(kExceptionMsgDestinationTooSmall);
            }
            if (i + 1 + rleCodeByte + 1 > srcLength)
            {
                throw SawyerChunkException(kExceptionMsgCorruptRle);
            }

            std::memcpy(dst8, src8 + i + 1, rleCodeByte + 1);
            dst8 += rleCodeByte + 1;
            i += rleCodeByte + 1;
        }
    }
    return reinterpret_cast<uintptr_t>(dst8) - reinterpret_cast<uintptr_t>(dst);
}

size_t SawyerChunkReader::DecodeChunkRepeat(void* dst, size_t dstCapacity, const void* src, size_t srcLength)
{
    auto src8 = static_cast<const uint8_t*>(src);
    auto dst8 = static_cast<uint8_t*>(dst);
    auto dstEnd = dst8 + dstCapacity;
    for (size_t i = 0; i < srcLength; i++)
    {
        if (src8[i] == 0xFF)
        {
            *dst8++ = src8[++i];
        }
        else
        {
            size_t count = (src8[i] & 7) + 1;
            const uint8_t* copySrc = dst8 + static_cast<int32_t>(src8[i] >> 3) - 32;

            if (dst8 + count >= dstEnd || copySrc + count >= dstEnd)
            {
                throw SawyerChunkException(kExceptionMsgDestinationTooSmall);
            }
            if (copySrc < dst)
            {
                throw SawyerChunkException(kExceptionMsgCorruptRle);
            }
            if ((copySrc < (dst8 + count) && copySrc >= dst8)
                || ((copySrc + count) <= (dst8 + count) && (copySrc + count) > dst8))
            {
                throw SawyerChunkException(kExceptionMsgCorruptRle);
            }

            std::memcpy(dst8, copySrc, count);
            dst8 += count;
        }
    }
    return reinterpret_cast<uintptr_t>(dst8) - reinterpret_cast<uintptr_t>(dst);
}

size_t SawyerChunkReader::DecodeChunkRotate(void* dst, size_t dstCapacity, const void* src, size_t srcLength)
{
    if (srcLength > dstCapacity)
    {
        throw SawyerChunkException(kExceptionMsgDestinationTooSmall);
    }

    auto src8 = static_cast<const uint8_t*>(src);
    auto dst8 = static_cast<uint8_t*>(dst);
    uint8_t code = 1;
    for (size_t i = 0; i < srcLength; i++)
    {
        dst8[i] = Numerics::ror8(src8[i], code);
        code = (code + 2) % 8;
    }
    return srcLength;
}
