#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

enum MEMORY_ACCESS
{
    MEMORY_ACCESS_READ      = 1 << 0,
    MEMORY_ACCESS_WRITE     = 1 << 1,
    MEMORY_ACCESS_OWNER     = 1 << 2,
};

/**
 * A stream for reading and writing to files. Wraps an SDL_RWops, SDL2's cross platform file stream.
 */
class MemoryStream final : public IStream
{
private:
    uint16  _access;
    size_t  _dataCapacity;
    size_t  _dataSize;
    void *  _data;
    void *  _position;

public:
    MemoryStream();
    MemoryStream(const MemoryStream &copy);
    explicit MemoryStream(size_t capacity);
    MemoryStream(void * data, size_t dataSize, uint32 access = MEMORY_ACCESS_READ);
    MemoryStream(const void * data, size_t dataSize);
    virtual ~MemoryStream();

    void * GetData() const;
    void * TakeData();

    ///////////////////////////////////////////////////////////////////////////
    // ISteam methods
    ///////////////////////////////////////////////////////////////////////////
    bool    CanRead()                                 const override;
    bool    CanWrite()                                const override;

    uint64  GetLength()                               const override;
    uint64  GetPosition()                             const override;
    void    SetPosition(uint64 position)                    override;
    void    Seek(sint64 offset, int origin)                 override;

    void    Read(void * buffer, uint64 length)              override;
    void    Write(const void * buffer, uint64 length)       override;

    uint64  TryRead(void * buffer, uint64 length)           override;

private:
    void EnsureCapacity(size_t capacity);
};
