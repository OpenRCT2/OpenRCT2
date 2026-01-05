/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MemoryStream.h"

#include "Guard.hpp"
#include "Memory.hpp"

#include <cstring>

namespace OpenRCT2
{
    MemoryStream::MemoryStream(const MemoryStream& copy)
    {
        _access = copy._access;
        _dataCapacity = copy._dataCapacity;
        _dataSize = copy._dataSize;

        if (_access.has(MemoryAccess::owner))
        {
            _data = Memory::Allocate<uint8_t>(_dataCapacity);
            std::memcpy(_data, copy._data, _dataCapacity);
        }
    }

    MemoryStream::MemoryStream(size_t capacity)
    {
        _dataCapacity = capacity;
        _data = Memory::Allocate<uint8_t>(capacity);
    }

    MemoryStream::MemoryStream(const std::vector<uint8_t>& v)
    {
        _dataCapacity = v.size();
        _dataSize = v.size();
        _data = Memory::Allocate<uint8_t>(v.size());
        std::memcpy(_data, v.data(), v.size());
    }

    MemoryStream::MemoryStream(void* data, size_t dataSize, AccessFlags access)
    {
        _access = access;
        _dataCapacity = dataSize;
        _dataSize = dataSize;
        _data = static_cast<uint8_t*>(data);
    }

    MemoryStream::MemoryStream(MemoryStream&& mv) noexcept
        : _access(mv._access)
        , _dataCapacity(mv._dataCapacity)
        , _dataSize(mv._dataSize)
        , _data(mv._data)
        , _position(mv._position)
    {
        mv._data = nullptr;
        mv._position = 0;
        mv._dataCapacity = 0;
        mv._dataSize = 0;
    }

    MemoryStream::~MemoryStream()
    {
        if (_access.has(MemoryAccess::owner))
            Memory::Free(_data);

        _data = nullptr;
        _position = 0;
        _dataCapacity = 0;
        _dataSize = 0;
    }

    MemoryStream& MemoryStream::operator=(MemoryStream&& mv) noexcept
    {
        if (this != &mv)
        {
            if (_access.has(MemoryAccess::owner))
                Memory::Free(_data);

            _access = mv._access;
            _dataCapacity = mv._dataCapacity;
            _data = mv._data;
            _dataSize = mv._dataSize;
            _position = mv._position;

            mv._data = nullptr;
            mv._position = 0;
            mv._dataCapacity = 0;
            mv._dataSize = 0;
        }

        return *this;
    }

    void MemoryStream::SetPosition(uint64_t position)
    {
        if (position > _dataSize)
            throw IOException("New position out of bounds.");
        _position = static_cast<size_t>(position);
    }

    void MemoryStream::Seek(int64_t offset, int32_t origin)
    {
        switch (origin)
        {
            default:
            case STREAM_SEEK_BEGIN:
                SetPosition(offset);
                break;
            case STREAM_SEEK_CURRENT:
                SetPosition(_position + offset);
                break;
            case STREAM_SEEK_END:
                SetPosition(_dataSize + offset);
                break;
        }
    }

    void MemoryStream::Read(void* buffer, uint64_t length)
    {
        if (_position + length > _dataSize)
            throw IOException("Attempted to read past end of stream.");

        std::memcpy(buffer, _data + _position, length);
        _position += static_cast<size_t>(length);
    }

    uint64_t MemoryStream::TryRead(void* buffer, uint64_t length)
    {
        uint64_t bytesToRead = std::min(length, static_cast<uint64_t>(_dataSize - _position));
        std::memcpy(buffer, _data + _position, static_cast<size_t>(bytesToRead));
        _position += static_cast<size_t>(bytesToRead);
        return bytesToRead;
    }

    void MemoryStream::Write(const void* buffer, uint64_t length)
    {
        EnsureWriteCapacity(length);
        std::memcpy(_data + _position, buffer, length);
        _position += static_cast<size_t>(length);
        _dataSize = std::max(_dataSize, _position);
    }

    void MemoryStream::Clear()
    {
        _dataSize = 0;
        _position = 0;
    }

    const void* MemoryStream::ReadDirect(size_t length)
    {
        if (_position + length > _dataSize)
            throw IOException("Attempted to read past end of stream.");

        const void* readPosition = _data + _position;
        _position += length;
        return readPosition;
    }

    void* MemoryStream::WriteDirectStart(size_t maxLength)
    {
        EnsureWriteCapacity(maxLength);
        return _data + _position;
    }

    void MemoryStream::WriteDirectCommit(size_t length)
    {
        Guard::Assert(_dataCapacity >= _position + length);
        _position += length;
        _dataSize = std::max(_dataSize, _position);
    }

    void MemoryStream::CopyFromStream(IStream& stream, uint64_t length)
    {
        EnsureWriteCapacity(length);
        // Copy directly into storage to avoid intermediate buffer
        stream.Read(_data + _position, length);
        _position += static_cast<size_t>(length);
        _dataSize = std::max(_dataSize, _position);
    }

    void MemoryStream::EnsureWriteCapacity(uint64_t length)
    {
        if (_position + static_cast<size_t>(length) > _dataCapacity)
        {
            if (_access.has(MemoryAccess::owner))
            {
                // resize to the larger of the requested capacity and double the current capacity, with a minimum of 8.
                size_t newCapacity = std::max(_position + static_cast<size_t>(length), _dataCapacity * 2);
                newCapacity = std::max<size_t>(newCapacity, 8);

                _dataCapacity = newCapacity;
                _data = Memory::Reallocate(_data, _dataCapacity);
            }
            else
            {
                throw IOException("Attempted to write past end of stream.");
            }
        }
    }

} // namespace OpenRCT2
