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
#include "zlib.h"

#include <cassert>
#include <stdexcept>
#include <string>

namespace OpenRCT2::Compression
{
    constexpr size_t kChunkSize = 128 * 1024;

    // Compress the source to gzip-compatible stream, write to dest.
    // Mainly used for compressing the crashdumps
    bool gzipCompress(FILE* source, FILE* dest)
    {
        if (source == nullptr || dest == nullptr)
        {
            return false;
        }
        int ret, flush;
        size_t have;
        z_stream strm{};
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        unsigned char in[kChunkSize];
        unsigned char out[kChunkSize];
        int windowBits = 15;
        int GZIP_ENCODING = 16;
        ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY);
        if (ret != Z_OK)
        {
            LOG_ERROR("Failed to initialise stream");
            return false;
        }
        do
        {
            strm.avail_in = uInt(fread(in, 1, kChunkSize, source));
            if (ferror(source))
            {
                deflateEnd(&strm);
                LOG_ERROR("Failed to read data from source");
                return false;
            }
            flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
            strm.next_in = in;
            do
            {
                strm.avail_out = kChunkSize;
                strm.next_out = out;
                ret = deflate(&strm, flush);
                if (ret == Z_STREAM_ERROR)
                {
                    LOG_ERROR("Failed to compress data");
                    return false;
                }
                have = kChunkSize - strm.avail_out;
                if (fwrite(out, 1, have, dest) != have || ferror(dest))
                {
                    deflateEnd(&strm);
                    LOG_ERROR("Failed to write data to destination");
                    return false;
                }
            } while (strm.avail_out == 0);
        } while (flush != Z_FINISH);
        deflateEnd(&strm);
        return true;
    }

    std::vector<uint8_t> gzip(const void* data, const size_t dataLen)
    {
        assert(data != nullptr);

        std::vector<uint8_t> output;
        z_stream strm{};
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;

        {
            const auto ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY);
            if (ret != Z_OK)
            {
                throw std::runtime_error("deflateInit2 failed with error " + std::to_string(ret));
            }
        }

        int flush = 0;
        const auto* src = static_cast<const Bytef*>(data);
        size_t srcRemaining = dataLen;
        do
        {
            const auto nextBlockSize = std::min(srcRemaining, kChunkSize);
            srcRemaining -= nextBlockSize;

            flush = srcRemaining == 0 ? Z_FINISH : Z_NO_FLUSH;
            strm.avail_in = static_cast<uInt>(nextBlockSize);
            strm.next_in = const_cast<Bytef*>(src);
            do
            {
                output.resize(output.size() + nextBlockSize);
                strm.avail_out = static_cast<uInt>(nextBlockSize);
                strm.next_out = &output[output.size() - nextBlockSize];
                const auto ret = deflate(&strm, flush);
                if (ret == Z_STREAM_ERROR)
                {
                    throw std::runtime_error("deflate failed with error " + std::to_string(ret));
                }
                output.resize(output.size() - strm.avail_out);
            } while (strm.avail_out == 0);

            src += nextBlockSize;
        } while (flush != Z_FINISH);
        deflateEnd(&strm);
        return output;
    }

    std::vector<uint8_t> ungzip(const void* data, const size_t dataLen)
    {
        assert(data != nullptr);

        std::vector<uint8_t> output;
        z_stream strm{};
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;

        {
            const auto ret = inflateInit2(&strm, 15 | 16);
            if (ret != Z_OK)
            {
                throw std::runtime_error("inflateInit2 failed with error " + std::to_string(ret));
            }
        }

        int flush = 0;
        const auto* src = static_cast<const Bytef*>(data);
        size_t srcRemaining = dataLen;
        do
        {
            const auto nextBlockSize = std::min(srcRemaining, kChunkSize);
            srcRemaining -= nextBlockSize;

            flush = srcRemaining == 0 ? Z_FINISH : Z_NO_FLUSH;
            strm.avail_in = static_cast<uInt>(nextBlockSize);
            strm.next_in = const_cast<Bytef*>(src);
            do
            {
                output.resize(output.size() + nextBlockSize);
                strm.avail_out = static_cast<uInt>(nextBlockSize);
                strm.next_out = &output[output.size() - nextBlockSize];
                const auto ret = inflate(&strm, flush);
                if (ret == Z_STREAM_ERROR)
                {
                    throw std::runtime_error("deflate failed with error " + std::to_string(ret));
                }
                output.resize(output.size() - strm.avail_out);
            } while (strm.avail_out == 0);

            src += nextBlockSize;
        } while (flush != Z_FINISH);
        inflateEnd(&strm);
        return output;
    }
} // namespace OpenRCT2::Compression
