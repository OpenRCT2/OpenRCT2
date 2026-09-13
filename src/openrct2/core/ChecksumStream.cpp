/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ChecksumStream.h"

#include "Endianness.h"

namespace OpenRCT2
{
#ifndef DISABLE_NETWORK
    ChecksumStream::ChecksumStream(std::array<std::byte, 20>& buf)
        : _checksum(buf)
    {
        uint64_t* hash = reinterpret_cast<uint64_t*>(_checksum.data());
        *hash = kSeed;
    }

    void ChecksumStream::Write(const void* buffer, uint64_t length)
    {
        for (size_t i = 0; i < length; i += sizeof(uint64_t))
        {
            const auto maxLen = std::min<size_t>(sizeof(uint64_t), length - i);

            uint64_t value{};
            std::memcpy(&value, reinterpret_cast<const std::byte*>(buffer) + i, maxLen);

            Step(value);
        }
    }

    void ChecksumStream::Step(uint64_t value)
    {
        uint64_t* hash = reinterpret_cast<uint64_t*>(_checksum.data());

        *hash ^= value;
        *hash *= kPrime;
    }

#endif
} // namespace OpenRCT2
