/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Compression.h"

#include "../Diagnostic.h"
#include "Guard.hpp"
#include "StreamBuffer.hpp"

#ifndef ZLIB_CONST
    #define ZLIB_CONST
#endif

#include <limits>
#include <zlib.h>

namespace OpenRCT2::Compression
{
    constexpr size_t kZlibChunkSize = 128 * 1024;
    constexpr size_t kZlibMaxChunkSize = static_cast<size_t>(std::numeric_limits<uInt>::max());
    constexpr int kZlibWindowBits[] = { -15, 15, 15 + 16 };

    /*
     * Modified copy of compressBound() from zlib 1.3.1, with the argument type changed from ULong
     * (which may be only 32 bits) to uint64_t, and adds space for the default gzip header,
     */
    static uint64_t zlibCompressBound(uint64_t length)
    {
        return length + (length >> 12) + (length >> 14) + (length >> 25) + 13 + (18 - 6);
    }

    bool zlibCompress(IStream& source, uint64_t sourceLength, IStream& dest, ZlibHeaderType header, int16_t level)
    {
        Guard::Assert(sourceLength <= source.GetLength() - source.GetPosition());

        int ret;
        StreamReadBuffer sourceBuf(source, sourceLength, kZlibChunkSize);
        StreamWriteBuffer destBuf(dest, zlibCompressBound(sourceLength), kZlibChunkSize);

        z_stream strm{};
        ret = deflateInit2(&strm, level, Z_DEFLATED, kZlibWindowBits[static_cast<int>(header)], 8, Z_DEFAULT_STRATEGY);
        if (ret != Z_OK)
        {
            LOG_ERROR("Failed to initialise stream");
            return false;
        }

        do
        {
            auto readBlock = sourceBuf.ReadBlock(source, kZlibMaxChunkSize);
            strm.next_in = static_cast<const Bytef*>(readBlock.first);
            strm.avail_in = static_cast<uInt>(readBlock.second);

            do
            {
                Guard::Assert(destBuf, "Compression Overruns Ouput Size");

                auto writeBlock = destBuf.WriteBlockStart(kZlibMaxChunkSize);
                strm.next_out = static_cast<Bytef*>(writeBlock.first);
                strm.avail_out = static_cast<uInt>(writeBlock.second);

                ret = deflate(&strm, sourceBuf ? Z_NO_FLUSH : Z_FINISH);
                if (ret == Z_STREAM_ERROR)
                {
                    LOG_ERROR("Failed to compress data");
                    deflateEnd(&strm);
                    return false;
                }

                destBuf.WriteBlockCommit(dest, writeBlock.second - strm.avail_out);
            } while (strm.avail_in > 0);
        } while (sourceBuf);

        deflateEnd(&strm);
        return true;
    }

    bool zlibDecompress(IStream& source, uint64_t sourceLength, IStream& dest, uint64_t decompressLength, ZlibHeaderType header)
    {
        Guard::Assert(sourceLength <= source.GetLength() - source.GetPosition());

        int ret;
        StreamReadBuffer sourceBuf(source, sourceLength, kZlibChunkSize);
        StreamWriteBuffer destBuf(dest, decompressLength, kZlibChunkSize);

        z_stream strm{};
        ret = inflateInit2(&strm, kZlibWindowBits[static_cast<int>(header)]);
        if (ret != Z_OK)
        {
            LOG_ERROR("Failed to initialise stream");
            return false;
        }

        do
        {
            auto readBlock = sourceBuf.ReadBlock(source, kZlibMaxChunkSize);
            strm.next_in = static_cast<const Bytef*>(readBlock.first);
            strm.avail_in = static_cast<uInt>(readBlock.second);

            do
            {
                if (!destBuf)
                {
                    LOG_ERROR("Decompressed data larger than expected");
                    inflateEnd(&strm);
                    return false;
                }

                auto writeBlock = destBuf.WriteBlockStart(kZlibMaxChunkSize);
                strm.next_out = static_cast<Bytef*>(writeBlock.first);
                strm.avail_out = static_cast<uInt>(writeBlock.second);

                ret = inflate(&strm, sourceBuf ? Z_NO_FLUSH : Z_FINISH);
                if (ret == Z_STREAM_ERROR)
                {
                    LOG_ERROR("Failed to decompress data");
                    inflateEnd(&strm);
                    return false;
                }

                destBuf.WriteBlockCommit(dest, writeBlock.second - strm.avail_out);
            } while (strm.avail_in > 0);
        } while (sourceBuf);

        if (destBuf)
        {
            LOG_ERROR("Decompressed data smaller than expected");
            inflateEnd(&strm);
            return false;
        }

        inflateEnd(&strm);
        return true;
    }
} // namespace OpenRCT2::Compression
