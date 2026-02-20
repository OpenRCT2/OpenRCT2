/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StreamBuffer.hpp"

#include "Guard.hpp"
#include "Memory.hpp"

namespace OpenRCT2
{
    StreamReadBuffer::StreamReadBuffer(IStream& stream, uint64_t length, size_t bufferLength)
    {
        Guard::Assert(bufferLength > 0);

        _remaining = length;
        if (length <= std::numeric_limits<size_t>::max())
            _buffer = static_cast<const uint8_t*>(stream.ReadDirect(static_cast<size_t>(length)));

        if (_buffer == nullptr)
        {
            if (length > stream.GetLength() - stream.GetPosition())
                throw IOException("Buffer Will Read Past the End of the Stream");

            _bufferLength = bufferLength;
            _buffer = OpenRCT2::Memory::Allocate<const uint8_t>(_bufferLength);
        }
    }

    StreamReadBuffer::~StreamReadBuffer()
    {
        if (_bufferLength > 0)
            Memory::Free(_buffer);

        _buffer = nullptr;
        _bufferLength = 0;
        _remaining = 0;
    }

    StreamReadBuffer::StreamReadBuffer(StreamReadBuffer&& other) noexcept
    {
        _buffer = other._buffer;
        _bufferLength = other._bufferLength;
        _remaining = other._remaining;

        other._buffer = nullptr;
        other._bufferLength = 0;
        other._remaining = 0;
    }

    StreamReadBuffer& StreamReadBuffer::operator=(StreamReadBuffer&& other) noexcept
    {
        if (&other != this)
        {
            _buffer = other._buffer;
            _bufferLength = other._bufferLength;
            _remaining = other._remaining;

            other._buffer = nullptr;
            other._bufferLength = 0;
            other._remaining = 0;
        }
        return *this;
    }

    std::pair<const void*, size_t> StreamReadBuffer::ReadBlock(IStream& stream, size_t maxLength)
    {
        const uint8_t* ptr = _buffer;
        size_t len = static_cast<size_t>(std::min<uint64_t>(_remaining, maxLength));
        if (_bufferLength == 0)
        {
            _buffer += len;
        }
        else
        {
            len = static_cast<size_t>(std::min(len, _bufferLength));
            stream.Read(const_cast<uint8_t*>(ptr), len);
        }
        _remaining -= len;
        return { ptr, len };
    }

    StreamWriteBuffer::StreamWriteBuffer(IStream& stream, uint64_t length, size_t bufferLength)
    {
        Guard::Assert(bufferLength > 0);

        _remaining = length;
        if (length <= std::numeric_limits<size_t>::max())
            _buffer = static_cast<uint8_t*>(stream.WriteDirectStart(static_cast<size_t>(length)));

        if (_buffer == nullptr)
        {
            _bufferLength = bufferLength;
            _buffer = OpenRCT2::Memory::Allocate<uint8_t>(_bufferLength);
        }
    }

    StreamWriteBuffer::~StreamWriteBuffer()
    {
        if (_bufferLength > 0)
            Memory::Free(_buffer);

        _buffer = nullptr;
        _bufferLength = 0;
        _remaining = 0;
    }

    StreamWriteBuffer::StreamWriteBuffer(StreamWriteBuffer&& other) noexcept
    {
        _buffer = other._buffer;
        _bufferLength = other._bufferLength;
        _remaining = other._remaining;

        other._buffer = nullptr;
        other._bufferLength = 0;
        other._remaining = 0;
    }

    StreamWriteBuffer& StreamWriteBuffer::operator=(StreamWriteBuffer&& other) noexcept
    {
        if (&other != this)
        {
            _buffer = other._buffer;
            _bufferLength = other._bufferLength;
            _remaining = other._remaining;

            other._buffer = nullptr;
            other._bufferLength = 0;
            other._remaining = 0;
        }
        return *this;
    }

    std::pair<void*, size_t> StreamWriteBuffer::WriteBlockStart(size_t maxLength)
    {
        size_t len = static_cast<size_t>(std::min<uint64_t>(_remaining, maxLength));
        if (_bufferLength > 0)
            len = std::min(len, _bufferLength);

        return { _buffer, len };
    }

    void StreamWriteBuffer::WriteBlockCommit(IStream& stream, size_t length)
    {
        if (_bufferLength == 0)
        {
            stream.WriteDirectCommit(length);
            _buffer += length;
        }
        else
        {
            stream.Write(_buffer, length);
        }
        _remaining -= length;
    }
} // namespace OpenRCT2
