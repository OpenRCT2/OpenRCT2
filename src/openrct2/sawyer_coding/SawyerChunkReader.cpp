/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SawyerChunkReader.h"

#include "../core/IStream.hpp"
#include "../core/MemoryStream.h"
#include "../core/Numerics.hpp"

namespace OpenRCT2::SawyerCoding
{
    // Allow chunks to be uncompressed to a maximum of 16 MiB
    constexpr size_t kMaxUncompressedChunkSize = 16 * 1024 * 1024;

    constexpr const char* kExceptionMessageCorruptChunkSize = "Corrupt chunk size.";
    constexpr const char* kExceptionMessageCorruptRLE = "Corrupt RLE compression data.";
    constexpr const char* kExceptionMessageDestinationTooSmall = "Chunk data larger than allocated destination capacity.";
    constexpr const char* kExceptionMessageInvalidChunkEncoding = "Invalid chunk encoding.";
    constexpr const char* kExceptionMessageZeroSizedChunk = "Encountered zero-sized chunk.";

    static MemoryStream DecodeChunk(const void* src, const SawyerCoding::ChunkHeader& header);

    SawyerChunkReader::SawyerChunkReader(OpenRCT2::IStream* stream)
        : _stream(stream)
    {
    }

    void SawyerChunkReader::SkipChunk()
    {
        uint64_t originalPosition = _stream->GetPosition();
        try
        {
            auto header = _stream->ReadValue<SawyerCoding::ChunkHeader>();
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
            auto header = _stream->ReadValue<SawyerCoding::ChunkHeader>();
            if (header.length >= kMaxUncompressedChunkSize)
                throw SawyerChunkException(kExceptionMessageCorruptChunkSize);

            switch (header.encoding)
            {
                case ChunkEncoding::none:
                case ChunkEncoding::rle:
                case ChunkEncoding::rleCompressed:
                case ChunkEncoding::rotate:
                {
                    auto compressedData = std::make_unique<uint8_t[]>(header.length);
                    if (_stream->TryRead(compressedData.get(), header.length) != header.length)
                    {
                        throw SawyerChunkException(kExceptionMessageCorruptChunkSize);
                    }

                    auto buffer = DecodeChunk(compressedData.get(), header);
                    if (buffer.GetLength() == 0)
                    {
                        throw SawyerChunkException(kExceptionMessageZeroSizedChunk);
                    }

                    return std::make_shared<SawyerChunk>(static_cast<ChunkEncoding>(header.encoding), std::move(buffer));
                }
                default:
                    throw SawyerChunkException(kExceptionMessageInvalidChunkEncoding);
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
                throw SawyerChunkException(kExceptionMessageZeroSizedChunk);
            }
            uint32_t compressedDataLength = compressedDataLength64;
            auto compressedData = std::make_unique<uint8_t[]>(compressedDataLength);

            if (_stream->TryRead(compressedData.get(), compressedDataLength) != compressedDataLength)
            {
                throw SawyerChunkException(kExceptionMessageCorruptChunkSize);
            }

            SawyerCoding::ChunkHeader header{ ChunkEncoding::rle, compressedDataLength };
            auto buffer = DecodeChunk(compressedData.get(), header);
            if (buffer.GetLength() == 0)
            {
                throw SawyerChunkException(kExceptionMessageZeroSizedChunk);
            }
            return std::make_shared<SawyerChunk>(ChunkEncoding::rle, std::move(buffer));
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

    static MemoryStream DecodeChunkRLE(const void* src, size_t srcLength)
    {
        MemoryStream buf;

        auto src8 = static_cast<const uint8_t*>(src);
        for (size_t i = 0; i < srcLength; i++)
        {
            uint8_t rleCodeByte = src8[i];
            if (rleCodeByte & 128)
            {
                i++;
                size_t count = 257 - rleCodeByte;

                if (i >= srcLength)
                {
                    throw SawyerChunkException(kExceptionMessageCorruptRLE);
                }
                if (buf.GetLength() + count > kMaxUncompressedChunkSize)
                {
                    throw SawyerChunkException(kExceptionMessageDestinationTooSmall);
                }

                for (size_t n = 0; n < count; n++)
                {
                    buf.WriteValue(src8[i]);
                }
            }
            else
            {
                const auto len = rleCodeByte + 1;

                if (i + 1 >= srcLength)
                {
                    throw SawyerChunkException(kExceptionMessageCorruptRLE);
                }
                if (buf.GetLength() + len > kMaxUncompressedChunkSize)
                {
                    throw SawyerChunkException(kExceptionMessageDestinationTooSmall);
                }
                if (i + 1 + len > srcLength)
                {
                    throw SawyerChunkException(kExceptionMessageCorruptRLE);
                }

                const auto* pos = src8 + i + 1;

                buf.Write(pos, len);
                i += len;
            }
        }

        return buf;
    }

    static MemoryStream DecodeChunkRepeat(const void* src, size_t srcLength)
    {
        MemoryStream buf;

        auto src8 = static_cast<const uint8_t*>(src);
        for (size_t i = 0; i < srcLength; i++)
        {
            if (src8[i] == 0xFF)
            {
                if (i + 1 >= srcLength)
                {
                    throw SawyerChunkException(kExceptionMessageCorruptRLE);
                }
                i++;
                buf.WriteValue(src8[i]);
            }
            else
            {
                size_t count = (src8[i] & 7) + 1;
                int32_t offset = static_cast<int32_t>(src8[i] >> 3) - 32;
                const uint8_t* copySrc = static_cast<const uint8_t*>(buf.GetData()) + (buf.GetLength() + offset);

                if (copySrc < static_cast<const uint8_t*>(buf.GetData())
                    || copySrc + count > static_cast<const uint8_t*>(buf.GetData()) + buf.GetLength())
                {
                    throw SawyerChunkException(kExceptionMessageCorruptRLE);
                }

                // We need a temporary buffer as the vector might invalidate the pointer.
                uint8_t temp[16];
                std::memcpy(temp, copySrc, count);

                buf.Write(temp, count);
            }
        }

        return buf;
    }

    static MemoryStream DecodeChunkRLERepeat(const void* src, size_t srcLength)
    {
        auto tempBuf = DecodeChunkRLE(src, srcLength);
        return DecodeChunkRepeat(tempBuf.GetData(), tempBuf.GetLength());
    }

    static MemoryStream DecodeChunkRotate(const void* src, size_t srcLength)
    {
        MemoryStream buf;

        auto src8 = static_cast<const uint8_t*>(src);

        uint8_t code = 1;
        for (size_t i = 0; i < srcLength; i++)
        {
            buf.WriteValue(Numerics::ror8(src8[i], code));
            code = (code + 2) % 8;
        }

        return buf;
    }

    static MemoryStream DecodeChunk(const void* src, const SawyerCoding::ChunkHeader& header)
    {
        MemoryStream buf;

        switch (header.encoding)
        {
            case ChunkEncoding::none:
                buf.Write(src, header.length);
                break;
            case ChunkEncoding::rle:
                buf = DecodeChunkRLE(src, header.length);
                break;
            case ChunkEncoding::rleCompressed:
                buf = DecodeChunkRLERepeat(src, header.length);
                break;
            case ChunkEncoding::rotate:
                buf = DecodeChunkRotate(src, header.length);
                break;
            default:
                throw SawyerChunkException(kExceptionMessageInvalidChunkEncoding);
        }

        // Return the stream with the position at the beginning.
        buf.SetPosition(0);

        return buf;
    }
} // namespace OpenRCT2::SawyerCoding
