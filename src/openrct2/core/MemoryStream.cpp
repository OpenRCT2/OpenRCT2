/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MemoryStream.h"

#include "Memory.hpp"

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
            _data = Memory::Allocate<void>(_dataCapacity);
            std::memcpy(_data, copy._data, _dataCapacity);
            _position = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(_data) + copy.GetPosition());
        }
    }

    MemoryStream::MemoryStream(size_t capacity)
    {
        _dataCapacity = capacity;
        _data = Memory::Allocate<void>(capacity);
        _position = _data;
    }

    MemoryStream::MemoryStream(void* data, size_t dataSize, uint8_t access)
    {
        _access = access;
        _dataCapacity = dataSize;
        _dataSize = dataSize;
        _data = data;
        _position = _data;
    }

    MemoryStream::MemoryStream(const void* data, size_t dataSize)
        : MemoryStream(const_cast<void*>(data), dataSize, MEMORY_ACCESS::READ)
    {
    }

    MemoryStream::MemoryStream(std::vector<uint8_t>&& v)
    {
        _access = MEMORY_ACCESS::OWNER;
        _dataCapacity = v.size();
        _dataSize = v.size();
        _data = Memory::Allocate<void>(v.size());
        _position = _data;
        std::memcpy(_data, v.data(), v.size());
    }

    MemoryStream::MemoryStream(MemoryStream&& mv) noexcept
        : _access(mv._access)
        , _dataCapacity(mv._dataCapacity)
        , _dataSize(mv._dataSize)
        , _data(mv._data)
        , _position(mv._position)
    {
        mv._data = nullptr;
        mv._position = nullptr;
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
            _access = mv._access;
            _dataCapacity = mv._dataCapacity;
            _data = mv._data;
            _dataSize = mv._dataSize;
            _position = mv._position;

            mv._data = nullptr;
            mv._position = nullptr;
            mv._dataCapacity = 0;
            mv._dataSize = 0;
        }

        return *this;
    }

    const void* MemoryStream::GetData() const
    {
        return _data;
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
        return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(_position) - reinterpret_cast<uintptr_t>(_data));
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
        _position = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(_data) + static_cast<uintptr_t>(newPosition));
    }

    void MemoryStream::Read(void* buffer, uint64_t length)
    {
        uint64_t position = GetPosition();
        if (position + length > _dataSize)
        {
            throw IOException("Attempted to read past end of stream.");
        }

        std::memcpy(buffer, _position, length);
        _position = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(_position) + length);
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
        uint64_t position = GetPosition();
        uint64_t nextPosition = position + length;
        if (nextPosition > _dataCapacity)
        {
            if (_access & MEMORY_ACCESS::OWNER)
            {
                EnsureCapacity(static_cast<size_t>(nextPosition));
            }
            else
            {
                throw IOException("Attempted to write past end of stream.");
            }
        }

        std::memcpy(_position, buffer, length);
        _position = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(_position) + length);
        _dataSize = std::max<size_t>(_dataSize, static_cast<size_t>(nextPosition));
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
        SetPosition(0);
    }

    void MemoryStream::EnsureCapacity(size_t capacity)
    {
        if (_dataCapacity < capacity)
        {
            size_t newCapacity = std::max<size_t>(8, _dataCapacity);
            while (newCapacity < capacity)
            {
                newCapacity *= 2;
            }

            uint64_t position = GetPosition();
            _dataCapacity = newCapacity;
            _data = Memory::Reallocate(_data, _dataCapacity);
            _position = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(_data) + static_cast<uintptr_t>(position));
        }
    }

} // namespace OpenRCT2
