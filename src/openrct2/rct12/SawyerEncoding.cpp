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

#include <memory>
#include "../core/IStream.hpp"
#include "../core/Math.hpp"
#include "SawyerEncoding.h"

extern "C"
{
    #include "../util/sawyercoding.h"
}

namespace SawyerEncoding
{
    void SkipChunk(IStream * stream)
    {
        auto header = stream->ReadValue<sawyercoding_chunk_header>();
        stream->Seek(header.length, STREAM_SEEK_CURRENT);
    }

    void * ReadChunk(IStream * stream, size_t * outSize)
    {
        uint64 originalPosition = stream->GetPosition();

        auto header = stream->ReadValue<sawyercoding_chunk_header>();
        switch (header.encoding) {
        case CHUNK_ENCODING_NONE:
        case CHUNK_ENCODING_RLE:
        case CHUNK_ENCODING_RLECOMPRESSED:
        case CHUNK_ENCODING_ROTATE:
        {
            std::unique_ptr<uint8> compressedData = std::unique_ptr<uint8>(Memory::Allocate<uint8>(header.length));
            if (stream->TryRead(compressedData.get(), header.length) != header.length)
            {
                throw IOException("Corrupt chunk size.");
            }

            // Allow 16MiB for chunk data
            size_t bufferSize = 16 * 1024 * 1024;
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

            if (outSize != nullptr) *outSize = uncompressedLength;
            return buffer;
        }
        default:
            stream->SetPosition(originalPosition);
            throw IOException("Invalid chunk encoding.");
        }
    }

    void ReadChunk(void * dst, size_t expectedSize, IStream * stream)
    {
        if (!TryReadChunk(dst, expectedSize, stream))
        {
            throw IOException("Invalid or incorrect chunk size.");
        }
    }

    void ReadChunkTolerant(void * dst, size_t expectedSize, IStream * stream)
    {
        uint64 originalPosition = stream->GetPosition();

        auto header = stream->ReadValue<sawyercoding_chunk_header>();
        switch (header.encoding) {
        case CHUNK_ENCODING_NONE:
        case CHUNK_ENCODING_RLE:
        case CHUNK_ENCODING_RLECOMPRESSED:
        case CHUNK_ENCODING_ROTATE:
        {
            std::unique_ptr<uint8> compressedData = std::unique_ptr<uint8>(Memory::Allocate<uint8>(header.length));
            if (stream->TryRead(compressedData.get(), header.length) != header.length)
            {
                throw IOException("Corrupt chunk size.");
            }

            // Allow 16MiB for chunk data
            size_t bufferSize = 16 * 1024 * 1024;
            std::unique_ptr<uint8> buffer = std::unique_ptr<uint8>(Memory::Allocate<uint8>(bufferSize));
            size_t uncompressedLength = sawyercoding_read_chunk_buffer(buffer.get(), compressedData.get(), header, bufferSize);
            size_t copyLength = Math::Min(uncompressedLength, expectedSize);

            Memory::Set(dst, 0, expectedSize);
            Memory::Copy<void>(dst, buffer.get(), copyLength);
            break;
        }
        default:
            stream->SetPosition(originalPosition);
            throw IOException("Invalid chunk encoding.");
        }
    }

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

    bool ValidateChecksum(IStream * stream)
    {
        // Get data size
        uint64 initialPosition = stream->GetPosition();
        uint64 dataSize = stream->GetLength() - initialPosition;
        if (dataSize < 8)
        {
            return false;
        }
        dataSize -= 4;

        // Calculate checksum
        uint32 checksum = 0;
        do
        {
            uint8 buffer[4096];
            uint64 bufferSize = Math::Min<uint64>(dataSize, sizeof(buffer));
            stream->Read(buffer, bufferSize);

            for (uint64 i = 0; i < bufferSize; i++)
            {
                checksum += buffer[i];
            }

            dataSize -= bufferSize;
        }
        while (dataSize != 0);

        // Read file checksum
        uint32 fileChecksum = stream->ReadValue<uint32>();

        // Rewind
        stream->SetPosition(initialPosition);

        return checksum == fileChecksum;
    }
}
