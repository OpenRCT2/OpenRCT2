/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "IStream.hpp"

#include <array>

namespace OpenRCT2
{
    /**
     * A stream for checksumming a stream of data
     */
    class ChecksumStream final : public IStream
    {
        // FIXME: Move the checksum implementation out.
        std::array<std::byte, 20>& _checksum;

        static constexpr uint64_t Seed = 0xcbf29ce484222325ULL;
        static constexpr uint64_t Prime = 0x00000100000001B3ULL;

    public:
        ChecksumStream(std::array<std::byte, 20>& buf);

        virtual ~ChecksumStream() = default;

        const void* GetData() const override
        {
            return _checksum.data();
        };

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

        void Write1(const void* buffer) override
        {
            Write<1>(buffer);
        }

        void Write2(const void* buffer) override
        {
            Write<2>(buffer);
        }

        void Write4(const void* buffer) override
        {
            Write<4>(buffer);
        }

        void Write8(const void* buffer) override
        {
            Write<8>(buffer);
        }

        void Write16(const void* buffer) override
        {
            Write<16>(buffer);
        }

        template<size_t N> void Write(const void* buffer)
        {
            Write(buffer, N);
        }

        uint64_t TryRead(void* buffer, uint64_t length) override
        {
            return 0;
        }
    };

} // namespace OpenRCT2
