/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <cstdio>
#include <vector>

namespace OpenRCT2::Compression
{
    bool gzipCompress(FILE* source, FILE* dest);
    std::vector<uint8_t> gzip(const void* data, const size_t dataLen);
    std::vector<uint8_t> ungzip(const void* data, const size_t dataLen);
} // namespace OpenRCT2::Compression
