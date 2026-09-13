/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "IStream.hpp"

#include <cstdint>
#include <cstdio>
#include <vector>

namespace OpenRCT2::Compression
{
    // both zlib and zstd don't use 0 as a real compression level, so use it to mean no compression
    constexpr int16_t kNoCompressionLevel = 0;

    // Zlib methods, using the DEFLATE compression algorithm
    constexpr int16_t kZlibDefaultCompressionLevel = -1; // zlib value for "default level"
    constexpr int16_t kZlibMaxCompressionLevel = 9;

    enum class ZlibHeaderType
    {
        none = 0,
        zlib = 1,
        gzip = 2,
    };

    bool zlibCompress(
        IStream& source, uint64_t sourceLength, IStream& dest, ZlibHeaderType header,
        int16_t level = kZlibDefaultCompressionLevel);
    bool zlibDecompress(
        IStream& source, uint64_t sourceLength, IStream& dest, uint64_t decompressLength, ZlibHeaderType header);

    // Zstd methods, using the ZStandard compression algorithm
    constexpr int16_t kZstdDefaultCompressionLevel = 3;

    // Options for optional metadata to attach to a ZStandard frame. Zstd default is length-only,
    // but callers to zstdCompress should use whatever is not already duplicated by other headers.
    enum class ZstdMetadata
    {
        none = 0,
        length = 1,
        checksum = 2,
        both = 3,
    };

    bool zstdCompress(
        IStream& source, uint64_t sourceLength, IStream& dest, ZstdMetadata metadata,
        int16_t level = kZstdDefaultCompressionLevel);
    bool zstdDecompress(IStream& source, uint64_t sourceLength, IStream& dest, uint64_t decompressLength);
} // namespace OpenRCT2::Compression
