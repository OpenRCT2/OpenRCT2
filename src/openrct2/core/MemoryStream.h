/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "IStream.hpp"

namespace MEMORY_ACCESS
{
    constexpr uint8_t READ    = 1 << 0;
    constexpr uint8_t WRITE   = 1 << 1;
    constexpr uint8_t OWNER   = 1 << 2;
};

/**
 * A stream for reading and writing to a buffer in memory. By default this buffer can grow.
 */
class MemoryStream final : public IStream
{
private:
    uint8_t  _access       = MEMORY_ACCESS::READ | MEMORY_ACCESS::WRITE | MEMORY_ACCESS::OWNER;
    size_t _dataCapacity = 0;
    size_t _dataSize     = 0;
    void * _data         = nullptr;
    void * _position     = nullptr;

public:
    MemoryStream() = default;
    MemoryStream(const MemoryStream & copy);
    explicit MemoryStream(size_t capacity);
    MemoryStream(void * data, size_t dataSize, uint8_t access = MEMORY_ACCESS::READ);
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

    uint64_t  GetLength()                               const override;
    uint64_t  GetPosition()                             const override;
    void    SetPosition(uint64_t position)                    override;
    void    Seek(int64_t offset, int32_t origin)                 override;

    void    Read(void * buffer, uint64_t length)              override;
    void    Write(const void * buffer, uint64_t length)       override;

    uint64_t  TryRead(void * buffer, uint64_t length)           override;

private:
    void EnsureCapacity(size_t capacity);
};
