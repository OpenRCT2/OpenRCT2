/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/IStream.hpp"
#include "SawyerChunkReader.h"

 // malloc is very slow for large allocations in MSVC debug builds as it allocates
 // memory on a special debug heap and then initialises all the memory to 0xCC.
#if defined(_WIN32) && defined(DEBUG)
#define __USE_HEAP_ALLOC__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

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
    uint64_t originalPosition = _stream->GetPosition();
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
    uint64_t originalPosition = _stream->GetPosition();
    try
    {
        auto header = _stream->ReadValue<sawyercoding_chunk_header>();
        switch (header.encoding) {
        case CHUNK_ENCODING_NONE:
        case CHUNK_ENCODING_RLE:
        case CHUNK_ENCODING_RLECOMPRESSED:
        case CHUNK_ENCODING_ROTATE:
        {
            std::unique_ptr<uint8_t[]> compressedData(new uint8_t[header.length]);
            if (_stream->TryRead(compressedData.get(), header.length) != header.length)
            {
                throw SawyerChunkException(EXCEPTION_MSG_CORRUPT_CHUNK_SIZE);
            }

            auto buffer = (uint8_t *)AllocateLargeTempBuffer();
            size_t uncompressedLength = DecodeChunk(buffer, MAX_UNCOMPRESSED_CHUNK_SIZE, compressedData.get(), header);
            Guard::Assert(uncompressedLength != 0, "Encountered zero-sized chunk!");
            buffer = (uint8_t *)FinaliseLargeTempBuffer(buffer, uncompressedLength);
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
    auto chunkData = (const uint8_t *)chunk->GetData();
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
            auto offset = (uint8_t *)dst + chunkLength;
            std::memset(offset, 0, remainingLength);
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
        throw SawyerChunkException(EXCEPTION_MSG_INVALID_CHUNK_ENCODING);
    }
    return resultLength;
}

size_t SawyerChunkReader::DecodeChunkRLERepeat(void * dst, size_t dstCapacity, const void * src, size_t srcLength)
{
    auto immBuffer = AllocateLargeTempBuffer();
    auto immLength = DecodeChunkRLE(immBuffer, MAX_UNCOMPRESSED_CHUNK_SIZE, src, srcLength);
    auto size = DecodeChunkRepeat(dst, dstCapacity, immBuffer, immLength);
    FreeLargeTempBuffer(immBuffer);
    return size;
}

size_t SawyerChunkReader::DecodeChunkRLE(void * dst, size_t dstCapacity, const void * src, size_t srcLength)
{
    auto src8 = static_cast<const uint8_t *>(src);
    auto dst8 = static_cast<uint8_t *>(dst);
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
                throw SawyerChunkException(EXCEPTION_MSG_CORRUPT_RLE);
            }
            if (dst8 + count > dstEnd)
            {
                throw SawyerChunkException(EXCEPTION_MSG_DESTINATION_TOO_SMALL);
            }

            std::memset(dst8, src8[i], count);
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

            std::memcpy(dst8, src8 + i + 1, rleCodeByte + 1);
            dst8 += rleCodeByte + 1;
            i += rleCodeByte + 1;
        }
    }
    return (uintptr_t)dst8 - (uintptr_t)dst;
}

size_t SawyerChunkReader::DecodeChunkRepeat(void * dst, size_t dstCapacity, const void * src, size_t srcLength)
{
    auto src8 = static_cast<const uint8_t *>(src);
    auto dst8 = static_cast<uint8_t *>(dst);
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
            const uint8_t * copySrc = dst8 + (int32_t)(src8[i] >> 3) - 32;

            if (dst8 + count >= dstEnd || copySrc + count >= dstEnd)
            {
                throw SawyerChunkException(EXCEPTION_MSG_DESTINATION_TOO_SMALL);
            }

            std::memcpy(dst8, copySrc, count);
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

    auto src8 = static_cast<const uint8_t *>(src);
    auto dst8 = static_cast<uint8_t *>(dst);
    uint8_t code = 1;
    for (size_t i = 0; i < srcLength; i++)
    {
        dst8[i] = ror8(src8[i], code);
        code = (code + 2) % 8;
    }
    return srcLength;
}

void * SawyerChunkReader::AllocateLargeTempBuffer()
{
#ifdef __USE_HEAP_ALLOC__
    auto buffer = HeapAlloc(GetProcessHeap(), 0, MAX_UNCOMPRESSED_CHUNK_SIZE);
#else
    auto buffer = std::malloc(MAX_UNCOMPRESSED_CHUNK_SIZE);
#endif
    if (buffer == nullptr)
    {
        throw std::runtime_error("Unable to allocate large temporary buffer.");
    }
    return buffer;
}

void * SawyerChunkReader::FinaliseLargeTempBuffer(void * buffer, size_t len)
{
#ifdef __USE_HEAP_ALLOC__
    auto finalBuffer = std::malloc(len);
    std::memcpy(finalBuffer, buffer, len);
    HeapFree(GetProcessHeap(), 0, buffer);
#else
    auto finalBuffer = (uint8_t *)std::realloc(buffer, len);
#endif
    if (finalBuffer == nullptr)
    {
        throw std::runtime_error("Unable to allocate final buffer.");
    }
    return finalBuffer;
}

void SawyerChunkReader::FreeLargeTempBuffer(void * buffer)
{
#ifdef __USE_HEAP_ALLOC__
    HeapFree(GetProcessHeap(), 0, buffer);
#else
    std::free(buffer);
#endif
}
