/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ChecksumStream.h"

namespace OpenRCT2
{
#if !defined(DISABLE_NETWORK) || defined(__amigaos__) || defined(OPENRCT2_KEEP_CHECKSUM)
    ChecksumStream::ChecksumStream(std::array<std::byte, 20>& buf)
        : _checksum(buf)
    {
        StoreHash(kSeed);
    }

    void ChecksumStream::Write(const void* buffer, uint64_t length)
    {
        for (size_t i = 0; i < length; i += sizeof(uint64_t))
        {
            const auto maxLen = std::min<size_t>(sizeof(uint64_t), length - i);

            uint64_t value{};
            std::memcpy(&value, reinterpret_cast<const std::byte*>(buffer) + i, maxLen);

            Step(IStream::LittleEndianToHost(value)); // words are little-endian by definition
        }
    }

    void ChecksumStream::Step(uint64_t value)
    {
        uint64_t hash = LoadHash();
        hash ^= value;
        hash *= kPrime;
        StoreHash(hash);
    }

    // The 64-bit hash lives little-endian in the first 8 checksum bytes so it prints the same on every host.
    uint64_t ChecksumStream::LoadHash() const
    {
        uint64_t raw;
        std::memcpy(&raw, _checksum.data(), sizeof(raw));
        return IStream::LittleEndianToHost(raw);
    }

    void ChecksumStream::StoreHash(uint64_t hash)
    {
        const uint64_t raw = IStream::LittleEndianToHost(hash);
        std::memcpy(_checksum.data(), &raw, sizeof(raw));
    }

#endif
} // namespace OpenRCT2
