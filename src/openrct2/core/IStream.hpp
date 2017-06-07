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

#include <string>
#include "../common.h"
#include "Exception.hpp"
#include "Memory.hpp"

enum {
    STREAM_SEEK_BEGIN,
    STREAM_SEEK_CURRENT,
    STREAM_SEEK_END
};

/**
 * Represents a stream that can be read or written to. Implemented by types such as FileStream, NetworkStream or MemoryStream.
 */
interface IStream
{
    ///////////////////////////////////////////////////////////////////////////
    // Interface methods
    ///////////////////////////////////////////////////////////////////////////
    virtual ~IStream() { }

    virtual bool    CanRead()                                 const abstract;
    virtual bool    CanWrite()                                const abstract;

    virtual uint64  GetLength()                               const abstract;
    virtual uint64  GetPosition()                             const abstract;
    virtual void    SetPosition(uint64 position)                    abstract;
    virtual void    Seek(sint64 offset, sint32 origin)                 abstract;

    virtual void    Read(void * buffer, uint64 length)              abstract;
    virtual void    Write(const void * buffer, uint64 length)       abstract;

    virtual uint64  TryRead(void * buffer, uint64 length)           abstract;

    ///////////////////////////////////////////////////////////////////////////
    // Helper methods
    ///////////////////////////////////////////////////////////////////////////

    /**
     * Reads the size of the given type from the stream directly into the given address.
     */
    template<typename T>
    void Read(T * value)
    {
        Read(value, sizeof(T));
    }

    /**
     * Writes the size of the given type to the stream directly from the given address.
     */
    template<typename T>
    void Write(const T * value)
    {
        Write(value, sizeof(T));
    }

    /**
     * Reads the given type from the stream. Use this only for small types (e.g. sint8, sint64, double)
     */
    template<typename T>
    T ReadValue()
    {
        T buffer;
        Read(&buffer);
        return buffer;
    }

    /**
     * Writes the given type to the stream. Use this only for small types (e.g. sint8, sint64, double)
     */
    template<typename T>
    void WriteValue(const T value)
    {
        Write(&value);
    }

    template<typename T>
    T * ReadArray(size_t count)
    {
        T * buffer = Memory::AllocateArray<T>(count);
        Read(buffer, sizeof(T) * count);
        return buffer;
    }

    template<typename T>
    void WriteArray(T * buffer, size_t count)
    {
        Write(buffer, sizeof(T) * count);
    }

    utf8 * ReadString();
    std::string ReadStdString();
    void WriteString(const utf8 * str);
    void WriteString(const std::string &string);
};

class IOException : public Exception
{
public:
    explicit IOException(const char * message) : Exception(message) { }
    explicit IOException(const std::string &message) : Exception(message) { }
};
