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

#include <algorithm>
#include "../core/IStream.hpp"
#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "SawyerChunkReader.h"

// Allow chunks to be uncompressed to a maximum of 16 MiB
constexpr size_t MAX_UNCOMPRESSED_CHUNK_SIZE = 16 * 1024 * 1024;

constexpr const char * EXCEPTION_MSG_CORRUPT_CHUNK_SIZE = "Corrupt chunk size.";
constexpr const char * EXCEPTION_MSG_DESTINATION_TOO_SMALL = "Chunk data larger than allocated destination capacity.";
constexpr const char * EXCEPTION_MSG_INVALID_CHUNK_ENCODING = "Invalid chunk encoding.";
constexpr const char * EXCEPTION_MSG_CORRUPT_RLE = "Corrupt RLE compression data.";

class SawyerChunkException : public IOException
{
public:
    explicit SawyerChunkException(const char * message) : IOException(message) { }
    explicit SawyerChunkException(const std::string &message) : IOException(message) { }
};

SawyerChunkReader::SawyerChunkReader(IStream * stream)
    : _stream(stream)
{
}

void SawyerChunkReader::SkipChunk()
{
    uint64 originalPosition = _stream->GetPosition();
    try
    {
        auto header = _stream->ReadValue<sawyercoding_chunk_header>();
        _stream->Seek(header.length, STREAM_SEEK_CURRENT);
    }
    catch (const std::exception &)
    {
        // Rewind stream back to original position
        _stream->SetPosition(originalPosition);
        throw;
    }
}

std::shared_ptr<SawyerChunk> SawyerChunkReader::ReadChunk()
{
    uint64 originalPosition = _stream->GetPosition();
    try
    {
        auto header = _stream->ReadValue<sawyercoding_chunk_header>();
        switch (header.encoding) {
        case CHUNK_ENCODING_NONE:
        case CHUNK_ENCODING_RLE:
        case CHUNK_ENCODING_RLECOMPRESSED:
        case CHUNK_ENCODING_ROTATE:
        {
            std::unique_ptr<uint8[]> compressedData(new uint8[header.length]);
            if (_stream->TryRead(compressedData.get(), header.length) != header.length)
            {
                throw SawyerChunkException(EXCEPTION_MSG_CORRUPT_CHUNK_SIZE);
            }

            // Allow 16MiB for chunk data
            size_t bufferSize = MAX_UNCOMPRESSED_CHUNK_SIZE;
            uint8 * buffer = Memory::Allocate<uint8>(bufferSize);
            if (buffer == nullptr)
            {
                throw std::runtime_error("Unable to allocate buffer.");
            }

            size_t uncompressedLength = DecodeChunk(buffer, bufferSize, compressedData.get(), header);
            Guard::Assert(uncompressedLength != 0, "Encountered zero-sized chunk!");
            buffer = Memory::Reallocate(buffer, uncompressedLength);
            if (buffer == nullptr)
            {
                throw std::runtime_error("Unable to reallocate buffer.");
            }

            return std::make_shared<SawyerChunk>((SAWYER_ENCODING)header.encoding, buffer, uncompressedLength);
        }
        default:
            throw SawyerChunkException(EXCEPTION_MSG_INVALID_CHUNK_ENCODING);
        }
    }
    catch (const std::exception &)
    {
        // Rewind stream back to original position
        _stream->SetPosition(originalPosition);
        throw;
    }
}

void SawyerChunkReader::ReadChunk(void * dst, size_t length)
{
    auto chunk = ReadChunk();
    auto chunkData = (const uint8 *)chunk->GetData();
    auto chunkLength = chunk->GetLength();
    if (chunkLength > length)
    {
        std::copy_n(chunkData, length, (uint8 *)dst);
    }
    else
    {
        std::copy_n(chunkData, chunkLength, (uint8 *)dst);
        auto remainingLength = length - chunkLength;
        if (remainingLength > 0)
        {
            auto offset = (uint8 *)dst + chunkLength;
            std::fill_n(offset, remainingLength, 0);
        }
    }
}

size_t SawyerChunkReader::DecodeChunk(void * dst, size_t dstCapacity, const void * src, const sawyercoding_chunk_header &header)
{
    size_t resultLength;
    switch (header.encoding)
    {
    case CHUNK_ENCODING_NONE:
        if (header.length > dstCapacity)
        {
            throw SawyerChunkException(EXCEPTION_MSG_DESTINATION_TOO_SMALL);
        }
        std::copy_n((const uint8 *)src, header.length, (uint8 *)dst);
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
        throw SawyerChunkException(EXCEPTION_MSG_INVALID_CHUNK_ENCODING);
    }
    return resultLength;
}

size_t SawyerChunkReader::DecodeChunkRLERepeat(void * dst, size_t dstCapacity, const void * src, size_t srcLength)
{
    auto immBufferLength = MAX_UNCOMPRESSED_CHUNK_SIZE;
    auto immBuffer = std::make_unique<uint8[]>(immBufferLength);
    auto immLength = DecodeChunkRLE(immBuffer.get(), immBufferLength, src, srcLength);
    return DecodeChunkRepeat(dst, dstCapacity, immBuffer.get(), immLength);
}

size_t SawyerChunkReader::DecodeChunkRLE(void * dst, size_t dstCapacity, const void * src, size_t srcLength)
{
    auto src8 = static_cast<const uint8 *>(src);
    auto dst8 = static_cast<uint8 *>(dst);
    auto dstEnd = dst8 + dstCapacity;
    for (size_t i = 0; i < srcLength; i++)
    {
        uint8 rleCodeByte = src8[i];
        if (rleCodeByte & 128)
        {
            i++;
            size_t count = 257 - rleCodeByte;

            if (i >= srcLength)
            {
                throw SawyerChunkException(EXCEPTION_MSG_CORRUPT_RLE);
            }
            if (dst8 + count > dstEnd)
            {
                throw SawyerChunkException(EXCEPTION_MSG_DESTINATION_TOO_SMALL);
            }

            std::fill_n(dst8, count, src8[i]);
            dst8 += count;
        }
        else
        {
            if (i + 1 >= srcLength)
            {
                throw SawyerChunkException(EXCEPTION_MSG_CORRUPT_RLE);
            }
            if (dst8 + rleCodeByte + 1 > dstEnd)
            {
                throw SawyerChunkException(EXCEPTION_MSG_DESTINATION_TOO_SMALL);
            }

            std::copy_n(src8 + i + 1, rleCodeByte + 1, dst8);
            dst8 += rleCodeByte + 1;
            i += rleCodeByte + 1;
        }
    }
    return (uintptr_t)dst8 - (uintptr_t)dst;
}

size_t SawyerChunkReader::DecodeChunkRepeat(void * dst, size_t dstCapacity, const void * src, size_t srcLength)
{
    auto src8 = static_cast<const uint8 *>(src);
    auto dst8 = static_cast<uint8 *>(dst);
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
            const uint8 * copySrc = dst8 + (sint32)(src8[i] >> 3) - 32;

            if (dst8 + count >= dstEnd || copySrc + count >= dstEnd)
            {
                throw SawyerChunkException(EXCEPTION_MSG_DESTINATION_TOO_SMALL);
            }

            std::copy_n(copySrc, count, dst8);
            dst8 += count;
        }
    }
    return (uintptr_t)dst8 - (uintptr_t)dst;
}

size_t SawyerChunkReader::DecodeChunkRotate(void * dst, size_t dstCapacity, const void * src, size_t srcLength)
{
    if (srcLength > dstCapacity)
    {
        throw SawyerChunkException(EXCEPTION_MSG_DESTINATION_TOO_SMALL);
    }

    auto src8 = static_cast<const uint8 *>(src);
    auto dst8 = static_cast<uint8 *>(dst);
    uint8 code = 1;
    for (size_t i = 0; i < srcLength; i++)
    {
        dst8[i] = ror8(src8[i], code);
        code = (code + 2) % 8;
    }
    return srcLength;
}
