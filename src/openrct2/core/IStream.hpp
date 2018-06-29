/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <istream>
#include <stdexcept>
#include <string>
#include <vector>
#include "../common.h"
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

    virtual uint64_t  GetLength()                               const abstract;
    virtual uint64_t  GetPosition()                             const abstract;
    virtual void    SetPosition(uint64_t position)                    abstract;
    virtual void    Seek(int64_t offset, int32_t origin)                 abstract;

    virtual void    Read(void * buffer, uint64_t length)              abstract;
    virtual void    Write(const void * buffer, uint64_t length)       abstract;

    virtual uint64_t  TryRead(void * buffer, uint64_t length)           abstract;

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
     * Reads the given type from the stream. Use this only for small types (e.g. int8_t, int64_t, double)
     */
    template<typename T>
    T ReadValue()
    {
        T buffer;
        Read(&buffer);
        return buffer;
    }

    /**
     * Writes the given type to the stream. Use this only for small types (e.g. int8_t, int64_t, double)
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

class IOException : public std::runtime_error
{
public:
    explicit IOException(const std::string &message) : std::runtime_error(message) { }
};

template<typename T>
class ivstream : public std::istream
{
private:
    class vector_streambuf : public std::basic_streambuf<char, std::char_traits<char>>
    {
    public:
        explicit vector_streambuf(const std::vector<T>& vec)
        {
            this->setg((char *)vec.data(), (char *)vec.data(), (char *)(vec.data() + vec.size()));
        }
    };

    vector_streambuf _streambuf;

public:
    ivstream(const std::vector<T>& vec)
        : std::istream(&_streambuf),
        _streambuf(vec)
    {
    }
};
