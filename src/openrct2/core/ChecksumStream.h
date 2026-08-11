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

#include <algorithm>
#include <array>
#include <cstring>

namespace OpenRCT2
{
    /**
     * A stream for checksumming a stream of data
     */
    class ChecksumStream final : public IStream
    {
        // FIXME: Move the checksum implementation out.
        std::array<std::byte, 20>& _checksum;

        static constexpr uint64_t kSeed = 0xcbf29ce484222325ULL;
        static constexpr uint64_t kPrime = 0x00000100000001B3ULL;

    public:
        ChecksumStream(std::array<std::byte, 20>& buf);

        virtual ~ChecksumStream() = default;

        const void* GetData() const override
        {
            return _checksum.data();
        }

        ///////////////////////////////////////////////////////////////////////////
        // ISteam methods
        ///////////////////////////////////////////////////////////////////////////
        bool CanRead() const override
        {
            return false;
        }
        bool CanWrite() const override
        {
            return true;
        }

        uint64_t GetLength() const override
        {
            return _checksum.size();
        }

        uint64_t GetPosition() const override
        {
            return 0;
        }

        void SetPosition(uint64_t position) override
        {
        }

        void Seek(int64_t offset, int32_t origin) override
        {
        }

        void Read(void* buffer, uint64_t length) override
        {
        }

        void Write(const void* buffer, uint64_t length) override;

        template<size_t N>
        void Write(const void* buffer)
        {
            for (size_t i = 0; i < N; i += sizeof(uint64_t))
            {
                const auto maxLen = std::min<size_t>(sizeof(uint64_t), N - i);

                uint64_t value{};
                std::memcpy(&value, reinterpret_cast<const std::byte*>(buffer) + i, maxLen);

                Step(value);
            }
        }

        uint64_t TryRead(void* buffer, uint64_t length) override
        {
            return 0;
        }

    private:
        void Write1(const void* buffer) override
        {
            Step(*static_cast<const uint8_t*>(buffer));
        }

        void Write2(const void* buffer) override
        {
            WriteUnaligned<uint16_t>(buffer);
        }

        void Write4(const void* buffer) override
        {
            WriteUnaligned<uint32_t>(buffer);
        }

        void Write8(const void* buffer) override
        {
            WriteUnaligned<uint64_t>(buffer);
        }

        void Write16(const void* buffer) override
        {
            WriteUnaligned<uint64_t>(static_cast<const std::byte*>(buffer) + 0);
            WriteUnaligned<uint64_t>(static_cast<const std::byte*>(buffer) + 8);
        }

        template<typename T>
        void WriteUnaligned(const void* buffer)
        {
            T value;
            std::memcpy(&value, buffer, sizeof(T));
            Step(value);
        }

        void Step(uint64_t value);
    };

} // namespace OpenRCT2
