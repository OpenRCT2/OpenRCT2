/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MemoryStream.h"

#include "Memory.hpp"

#include <bit>
#include <cstring>

namespace OpenRCT2
{
    MemoryStream::MemoryStream(const MemoryStream& copy)
    {
        _access = copy._access;
        _dataCapacity = copy._dataCapacity;
        _dataSize = copy._dataSize;

        if (_access & MEMORY_ACCESS::OWNER)
        {
            _position = copy.GetPosition();
            _data = Memory::Allocate<std::byte>(_dataCapacity);
            std::memcpy(_data, copy._data, _dataCapacity);
        }
    }

    MemoryStream::MemoryStream(size_t capacity)
    {
        _dataCapacity = capacity;
        _data = Memory::Allocate<std::byte>(capacity);
        _position = 0;
    }

    MemoryStream::MemoryStream(void* data, size_t dataSize)
        : _access{ MEMORY_ACCESS::READ | MEMORY_ACCESS::WRITE }
        , _dataCapacity{ dataSize }
        , _dataSize{ dataSize }
        , _data{ static_cast<std::byte*>(data) }
        , _position{ 0 }
    {
    }

    MemoryStream::MemoryStream(const void* data, size_t dataSize)
        : _access{ MEMORY_ACCESS::READ }
        , _dataCapacity{ dataSize }
        , _dataSize{ dataSize }
        , _data{ static_cast<std::byte*>(const_cast<void*>(data)) }
        , _position{ 0 }
    {
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
        if (_access & MEMORY_ACCESS::OWNER)
        {
            Memory::Free(_data);
        }
        _dataCapacity = 0;
        _dataSize = 0;
        _data = nullptr;
    }

    MemoryStream& MemoryStream::operator=(MemoryStream&& mv) noexcept
    {
        if (this != &mv)
        {
            if (_access & MEMORY_ACCESS::OWNER)
            {
                Memory::Free(_data);
            }

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

    const void* MemoryStream::GetData() const
    {
        return static_cast<const void*>(_data);
    }

    bool MemoryStream::CanRead() const
    {
        return (_access & MEMORY_ACCESS::READ) != 0;
    }

    bool MemoryStream::CanWrite() const
    {
        return (_access & MEMORY_ACCESS::WRITE) != 0;
    }

    uint64_t MemoryStream::GetLength() const
    {
        return _dataSize;
    }

    uint64_t MemoryStream::GetPosition() const
    {
        return _position;
    }

    void MemoryStream::SetPosition(uint64_t position)
    {
        Seek(position, STREAM_SEEK_BEGIN);
    }

    void MemoryStream::Seek(int64_t offset, int32_t origin)
    {
        uint64_t newPosition;
        switch (origin)
        {
            default:
            case STREAM_SEEK_BEGIN:
                newPosition = offset;
                break;
            case STREAM_SEEK_CURRENT:
                newPosition = GetPosition() + offset;
                break;
            case STREAM_SEEK_END:
                newPosition = _dataSize + offset;
                break;
        }

        if (newPosition > _dataSize)
        {
            throw IOException("New position out of bounds.");
        }
        _position = newPosition;
    }

    void MemoryStream::Read(void* buffer, uint64_t length)
    {
        if (_position + length > _dataSize)
        {
            throw IOException("Attempted to read past end of stream.");
        }

        std::memcpy(buffer, _data + _position, length);
        _position += length;
    }

    void MemoryStream::Read1(void* buffer)
    {
        Read<1>(buffer);
    }

    void MemoryStream::Read2(void* buffer)
    {
        Read<2>(buffer);
    }

    void MemoryStream::Read4(void* buffer)
    {
        Read<4>(buffer);
    }

    void MemoryStream::Read8(void* buffer)
    {
        Read<8>(buffer);
    }

    void MemoryStream::Read16(void* buffer)
    {
        Read<16>(buffer);
    }

    uint64_t MemoryStream::TryRead(void* buffer, uint64_t length)
    {
        uint64_t remainingBytes = GetLength() - GetPosition();
        uint64_t bytesToRead = std::min(length, remainingBytes);
        Read(buffer, bytesToRead);
        return bytesToRead;
    }

    void MemoryStream::Write(const void* buffer, uint64_t length)
    {
        const auto nextPosition = _position + length;
        EnsureCapacity(nextPosition);

        std::memcpy(_data + _position, buffer, length);
        _position += length;
        _dataSize = std::max(_dataSize, nextPosition);
    }

    void MemoryStream::Write1(const void* buffer)
    {
        Write<1>(buffer);
    }

    void MemoryStream::Write2(const void* buffer)
    {
        Write<2>(buffer);
    }

    void MemoryStream::Write4(const void* buffer)
    {
        Write<4>(buffer);
    }

    void MemoryStream::Write8(const void* buffer)
    {
        Write<8>(buffer);
    }

    void MemoryStream::Write16(const void* buffer)
    {
        Write<16>(buffer);
    }

    void MemoryStream::Clear()
    {
        _dataSize = 0;
        _position = 0;
    }

    void MemoryStream::EnsureCapacity(size_t capacity)
    {
        if (capacity <= _dataCapacity)
        {
            return;
        }

        if ((_access & MEMORY_ACCESS::OWNER) == 0)
        {
            throw IOException("Can not grow external memory");
        }

        size_t newCapacity = std::max<size_t>(32, std::bit_ceil(capacity));

        _dataCapacity = newCapacity;
        _data = Memory::Reallocate(_data, _dataCapacity);
    }

} // namespace OpenRCT2
