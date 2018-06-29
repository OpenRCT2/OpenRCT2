/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <cstring>
#include "Math.hpp"
#include "Memory.hpp"
#include "MemoryStream.h"

MemoryStream::MemoryStream(const MemoryStream &copy)
{
    _access = copy._access;
    _dataCapacity = copy._dataCapacity;
    _dataSize = copy._dataSize;

    if (_access & MEMORY_ACCESS::OWNER)
    {
        _data = Memory::Allocate<void>(_dataCapacity);
        std::memcpy(_data, copy._data, _dataCapacity);
        _position = (void*)((uintptr_t)_data + copy.GetPosition());
    }
}

MemoryStream::MemoryStream(size_t capacity)
{
    _dataCapacity = capacity;
    _data = Memory::Allocate<void>(capacity);
    _position = _data;
}

MemoryStream::MemoryStream(void * data, size_t dataSize, uint8_t access)
{
    _access = access;
    _dataCapacity = dataSize;
    _dataSize = dataSize;
    _data = data;
    _position = _data;
}

MemoryStream::MemoryStream(const void * data, size_t dataSize)
    : MemoryStream((void *)data, dataSize, MEMORY_ACCESS::READ)
{
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

const void * MemoryStream::GetData() const
{
    return _data;
}

void * MemoryStream::GetDataCopy() const
{
    auto result = Memory::Allocate<void>(_dataSize);
    std::memcpy(result, _data, _dataSize);
    return result;
}

void * MemoryStream::TakeData()
{
    _access &= ~MEMORY_ACCESS::OWNER;
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
    return (uint64_t)((uintptr_t)_position - (uintptr_t)_data);
}

void MemoryStream::SetPosition(uint64_t position)
{
    Seek(position, STREAM_SEEK_BEGIN);
}

void MemoryStream::Seek(int64_t offset, int32_t origin)
{
    uint64_t newPosition;
    switch (origin) {
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
    _position = (void*)((uintptr_t)_data + (uintptr_t)newPosition);
}

void MemoryStream::Read(void * buffer, uint64_t length)
{
    uint64_t position = GetPosition();
    if (position + length > _dataSize)
    {
        throw IOException("Attempted to read past end of stream.");
    }

    std::copy_n((const uint8_t *)_position, length, (uint8_t *)buffer);
    _position = (void*)((uintptr_t)_position + length);
}

uint64_t MemoryStream::TryRead(void * buffer, uint64_t length)
{
    uint64_t remainingBytes = GetLength() - GetPosition();
    uint64_t bytesToRead = std::min(length, remainingBytes);
    Read(buffer, bytesToRead);
    return bytesToRead;
}

void MemoryStream::Write(const void * buffer, uint64_t length)
{
    uint64_t position = GetPosition();
    uint64_t nextPosition = position + length;
    if (nextPosition > _dataCapacity)
    {
        if (_access & MEMORY_ACCESS::OWNER)
        {
            EnsureCapacity((size_t)nextPosition);
        }
        else
        {
            throw IOException("Attempted to write past end of stream.");
        }
    }

    std::copy_n((const uint8_t *)buffer, length, (uint8_t *)_position);
    _position = (void*)((uintptr_t)_position + length);
    _dataSize = std::max<size_t>(_dataSize, (size_t)nextPosition);
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
        _position = (void *)((uintptr_t)_data + (uintptr_t)position);
    }
}
