/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <zstd.h>

namespace OpenRCT2::Compression
{
    static constexpr size_t kZlibChunkSize = 128 * 1024;
    static constexpr size_t kZlibMaxChunkSize = static_cast<size_t>(std::numeric_limits<uInt>::max());
    static constexpr int kZlibWindowBits[] = { -15, 15, 15 + 16 };

    /*
     * Modified copy of compressBound() from zlib 1.3.1, with the argument type changed from ULong
     * (which may be only 32 bits) to uint64_t, and adds space for the default gzip header,
     */
    static uint64_t zlibCompressBound(uint64_t length)
    {
        return length + (length >> 12) + (length >> 14) + (length >> 25) + 13uLL + (18uLL - 6uLL);
    }

    bool zlibCompress(IStream& source, uint64_t sourceLength, IStream& dest, ZlibHeaderType header, int16_t level)
    {
        if (sourceLength > source.GetLength() - source.GetPosition())
            throw IOException("Not Enough Data to Compress");

        StreamReadBuffer sourceBuf(source, sourceLength, kZlibChunkSize);
        StreamWriteBuffer destBuf(dest, zlibCompressBound(sourceLength), kZlibChunkSize);

        z_stream strm{};
        int ret = deflateInit2(&strm, level, Z_DEFLATED, kZlibWindowBits[static_cast<int>(header)], 8, Z_DEFAULT_STRATEGY);
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
        if (sourceLength > source.GetLength() - source.GetPosition())
            throw IOException("Not Enough Data to Decompress");

        StreamReadBuffer sourceBuf(source, sourceLength, kZlibChunkSize);
        StreamWriteBuffer destBuf(dest, decompressLength, kZlibChunkSize);

        z_stream strm{};
        int ret = inflateInit2(&strm, kZlibWindowBits[static_cast<int>(header)]);
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

    /*
     * Modified copy of ZSTD_COMPRESSBOUND / ZSTD_compressBound() from zstd 1.5.7, with the argument
     * type changed from size_t (which may be only 32 bits) to uint64_t, and removes the error handling.
     */
    static uint64_t zstdCompressBound(uint64_t length)
    {
        return length + (length >> 8)
            + ((length < (128uLL << 10)) ? (((128uLL << 10) - length) >> 11) /* margin, from 64 to 0 */ : 0uLL);
    }

    bool zstdCompress(IStream& source, uint64_t sourceLength, IStream& dest, ZstdMetadata metadata, int16_t level)
    {
        if (sourceLength > source.GetLength() - source.GetPosition())
            throw IOException("Not Enough Data to Compress");

        StreamReadBuffer sourceBuf(source, sourceLength, ZSTD_CStreamInSize());
        StreamWriteBuffer destBuf(dest, zstdCompressBound(sourceLength), ZSTD_CStreamOutSize());
        unsigned metaFlags = static_cast<unsigned>(metadata);

        const auto deleter = [](ZSTD_CCtx* ptr) { ZSTD_freeCCtx(ptr); };
        std::unique_ptr<ZSTD_CCtx, decltype(deleter)> ctx(ZSTD_createCCtx(), deleter);
        if (ctx == nullptr)
        {
            LOG_ERROR("Failed to create zstd context");
            return false;
        }

        size_t ret = ZSTD_CCtx_setParameter(ctx.get(), ZSTD_c_compressionLevel, level);
        if (ZSTD_isError(ret))
        {
            LOG_ERROR("Failed to set compression level with error: %s", ZSTD_getErrorName(ret));
            return false;
        }
        // set options for content size (default on) and checksum (default off)
        ret = ZSTD_CCtx_setParameter(ctx.get(), ZSTD_c_contentSizeFlag, (metaFlags & 1) != 0);
        if (ZSTD_isError(ret))
        {
            LOG_ERROR("Failed to set content size flag with error: %s", ZSTD_getErrorName(ret));
            return false;
        }
        ret = ZSTD_CCtx_setParameter(ctx.get(), ZSTD_c_checksumFlag, (metaFlags & 2) != 0);
        if (ZSTD_isError(ret))
        {
            LOG_ERROR("Failed to set checksum flag with error: %s", ZSTD_getErrorName(ret));
            return false;
        }
        // unlike gzip, zstd puts the decompressed content size at the start of the file,
        // so we need to tell zstd how big the input is before we start compressing.
        ret = ZSTD_CCtx_setPledgedSrcSize(ctx.get(), sourceLength);
        if (ZSTD_isError(ret))
        {
            LOG_ERROR("Failed to set file length with error: %s", ZSTD_getErrorName(ret));
            return false;
        }

        do
        {
            auto readBlock = sourceBuf.ReadBlock(source);
            ZSTD_inBuffer input = { readBlock.first, readBlock.second, 0 };

            do
            {
                Guard::Assert(destBuf, "Compression Overruns Ouput Size");

                auto writeBlock = destBuf.WriteBlockStart();
                ZSTD_outBuffer output = { writeBlock.first, writeBlock.second, 0 };

                ret = ZSTD_compressStream2(ctx.get(), &output, &input, sourceBuf ? ZSTD_e_continue : ZSTD_e_end);
                if (ZSTD_isError(ret))
                {
                    LOG_ERROR("Failed to compress data with error: %s", ZSTD_getErrorName(ret));
                    return false;
                }

                destBuf.WriteBlockCommit(dest, output.pos);
            } while (input.pos < input.size || (!sourceBuf && ret > 0));
        } while (sourceBuf);

        return true;
    }

    bool zstdDecompress(IStream& source, uint64_t sourceLength, IStream& dest, uint64_t decompressLength)
    {
        if (sourceLength > source.GetLength() - source.GetPosition())
            throw IOException("Not Enough Data to Decompress");

        size_t ret;
        StreamReadBuffer sourceBuf(source, sourceLength, ZSTD_DStreamInSize());
        StreamWriteBuffer destBuf(dest, decompressLength, ZSTD_DStreamOutSize());

        const auto deleter = [](ZSTD_DCtx* ptr) { ZSTD_freeDCtx(ptr); };
        std::unique_ptr<ZSTD_DCtx, decltype(deleter)> ctx(ZSTD_createDCtx(), deleter);
        if (ctx == nullptr)
        {
            LOG_ERROR("Failed to create zstd context");
            return false;
        }

        do
        {
            auto readBlock = sourceBuf.ReadBlock(source);
            ZSTD_inBuffer input = { readBlock.first, readBlock.second, 0 };

            do
            {
                if (!destBuf)
                {
                    LOG_ERROR("Decompressed data larger than expected");
                    return false;
                }

                auto writeBlock = destBuf.WriteBlockStart();
                ZSTD_outBuffer output = { writeBlock.first, writeBlock.second, 0 };

                ret = ZSTD_decompressStream(ctx.get(), &output, &input);
                if (ZSTD_isError(ret))
                {
                    LOG_ERROR("Failed to compress data with error: %s", ZSTD_getErrorName(ret));
                    return false;
                }

                destBuf.WriteBlockCommit(dest, output.pos);
            } while (input.pos < input.size || (!sourceBuf && ret > 0));
        } while (sourceBuf);

        if (destBuf)
        {
            LOG_ERROR("Decompressed data smaller than expected");
            return false;
        }

        return true;
    }
} // namespace OpenRCT2::Compression
