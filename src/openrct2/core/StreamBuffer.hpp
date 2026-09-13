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
#include <limits>
#include <utility>

namespace OpenRCT2
{
    struct StreamReadBuffer
    {
        StreamReadBuffer(IStream& stream, uint64_t length, size_t bufferLength);

        ~StreamReadBuffer();
        // not copyable
        StreamReadBuffer(const StreamReadBuffer&) = delete;
        StreamReadBuffer& operator=(const StreamReadBuffer&) = delete;
        // moveable
        StreamReadBuffer(StreamReadBuffer&&) noexcept;
        StreamReadBuffer& operator=(StreamReadBuffer&&) noexcept;

        std::pair<const void*, size_t> ReadBlock(IStream& stream, size_t maxLength = std::numeric_limits<size_t>::max());

        uint64_t GetRemaining() const
        {
            return _remaining;
        }
        operator bool() const
        {
            return _remaining > 0;
        }

    private:
        const uint8_t* _buffer = nullptr;
        size_t _bufferLength = 0;
        uint64_t _remaining = 0;
    };

    struct StreamWriteBuffer
    {
        StreamWriteBuffer(IStream& stream, uint64_t length, size_t bufferLength);

        ~StreamWriteBuffer();
        // not copyable
        StreamWriteBuffer(const StreamWriteBuffer&) = delete;
        StreamWriteBuffer& operator=(const StreamWriteBuffer&) = delete;
        // moveable
        StreamWriteBuffer(StreamWriteBuffer&&) noexcept;
        StreamWriteBuffer& operator=(StreamWriteBuffer&&) noexcept;

        std::pair<void*, size_t> WriteBlockStart(size_t maxLength = std::numeric_limits<size_t>::max());
        void WriteBlockCommit(IStream& stream, size_t length);

        uint64_t GetRemaining() const
        {
            return _remaining;
        }
        operator bool() const
        {
            return _remaining > 0;
        }

    private:
        uint8_t* _buffer = nullptr;
        size_t _bufferLength = 0;
        uint64_t _remaining = 0;
    };
} // namespace OpenRCT2
