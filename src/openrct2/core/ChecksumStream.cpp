/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ChecksumStream.h"

#include "Endianness.h"

#include <cstddef>

namespace OpenRCT2
{
#ifndef DISABLE_NETWORK
    ChecksumStream::ChecksumStream(std::array<std::byte, 20>& buf)
        : _checksum(buf)
    {
        uint64_t* hash = reinterpret_cast<uint64_t*>(_checksum.data());
        *hash = Seed;
    }

    void ChecksumStream::Write(const void* buffer, uint64_t length)
    {
        uint64_t* hash = reinterpret_cast<uint64_t*>(_checksum.data());
        for (size_t i = 0; i < length; i += sizeof(uint64_t))
        {
            const auto maxLen = std::min<size_t>(sizeof(uint64_t), length - i);

            uint64_t temp{};
            std::memcpy(&temp, reinterpret_cast<const std::byte*>(buffer) + i, maxLen);

            // Always use value as little endian, most common systems are little.
#    if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            temp = ByteSwapBE(temp);
#    endif

            *hash ^= temp;
            *hash *= Prime;
        }
    }

#endif
} // namespace OpenRCT2
