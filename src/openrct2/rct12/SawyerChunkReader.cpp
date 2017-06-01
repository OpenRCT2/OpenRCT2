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
#include "SawyerChunkReader.h"

extern "C"
{
    #include "../util/sawyercoding.h"
}

// Allow chunks to be uncompressed to a maximum of 16 MiB
constexpr size_t MAX_UNCOMPRESSED_CHUNK_SIZE = 16 * 1024 * 1024;

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
    catch (Exception)
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
                throw SawyerChunkException("Corrupt chunk size.");
            }

            // Allow 16MiB for chunk data
            size_t bufferSize = MAX_UNCOMPRESSED_CHUNK_SIZE;
            uint8 * buffer = Memory::Allocate<uint8>(bufferSize);
            if (buffer == nullptr)
            {
                throw Exception("Unable to allocate buffer.");
            }

            size_t uncompressedLength = sawyercoding_read_chunk_buffer(buffer, compressedData.get(), header, bufferSize);
            buffer = Memory::Reallocate(buffer, uncompressedLength);
            if (buffer == nullptr)
            {
                throw Exception("Unable to reallocate buffer.");
            }

            return std::make_shared<SawyerChunk>((SAWYER_ENCODING)header.encoding, buffer, uncompressedLength);
        }
        default:
            throw SawyerChunkException("Invalid chunk encoding.");
        }
    }
    catch (Exception)
    {
        // Rewind stream back to original position
        _stream->SetPosition(originalPosition);
        throw;
    }
}

void SawyerChunkReader::ReadChunk(void * dst, size_t length)
{
    auto chunk = ReadChunk();
    const void * chunkData = chunk->GetData();
    size_t chunkLength = chunk->GetLength();
    if (chunkLength > length)
    {
        Memory::Copy(dst, chunkData, length);
    }
    else
    {
        Memory::Copy(dst, chunkData, chunkLength);
        size_t remainingLength = length - chunkLength;
        if (remainingLength > 0)
        {
            void * offset = (void *)((uintptr_t)dst + chunkLength);
            Memory::Set(offset, 0, remainingLength);
        }
    }
}
