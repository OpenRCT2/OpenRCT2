#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"
#include "IStream.hpp"

namespace MEMORY_ACCESS
{
    constexpr uint8 READ    = 1 << 0;
    constexpr uint8 WRITE   = 1 << 1;
    constexpr uint8 OWNER   = 1 << 2;
};

/**
 * A stream for reading and writing to a buffer in memory. By default this buffer can grow.
 */
class MemoryStream final : public IStream
{
private:
    uint8  _access       = MEMORY_ACCESS::READ | MEMORY_ACCESS::WRITE | MEMORY_ACCESS::OWNER;
    size_t _dataCapacity = 0;
    size_t _dataSize     = 0;
    void * _data         = nullptr;
    void * _position     = nullptr;

public:
    MemoryStream() = default;
    MemoryStream(const MemoryStream & copy);
    explicit MemoryStream(size_t capacity);
    MemoryStream(void * data, size_t dataSize, uint8 access = MEMORY_ACCESS::READ);
    MemoryStream(const void * data, size_t dataSize);
    virtual ~MemoryStream();

    const void * GetData() const;
    void * GetDataCopy() const;
    void * TakeData();

    ///////////////////////////////////////////////////////////////////////////
    // ISteam methods
    ///////////////////////////////////////////////////////////////////////////
    bool    CanRead()                                 const override;
    bool    CanWrite()                                const override;

    uint64  GetLength()                               const override;
    uint64  GetPosition()                             const override;
    void    SetPosition(uint64 position)                    override;
    void    Seek(sint64 offset, sint32 origin)                 override;

    void    Read(void * buffer, uint64 length)              override;
    void    Write(const void * buffer, uint64 length)       override;

    uint64  TryRead(void * buffer, uint64 length)           override;

private:
    void EnsureCapacity(size_t capacity);
};
