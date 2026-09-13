/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "StringTypes.h"

#include <istream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsuggest-final-methods"
    #pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif

namespace OpenRCT2
{
    enum
    {
        STREAM_SEEK_BEGIN,
        STREAM_SEEK_CURRENT,
        STREAM_SEEK_END
    };

    /**
     * Represents a stream that can be read or written to. Implemented by types such as FileStream, NetworkStream or
     * MemoryStream.
     */
    struct IStream
    {
        ///////////////////////////////////////////////////////////////////////////
        // Interface methods
        ///////////////////////////////////////////////////////////////////////////
        virtual ~IStream()
        {
        }

        virtual bool CanRead() const = 0;
        virtual bool CanWrite() const = 0;

        virtual uint64_t GetLength() const = 0;
        virtual uint64_t GetPosition() const = 0;
        virtual void SetPosition(uint64_t position) = 0;
        virtual void Seek(int64_t offset, int32_t origin) = 0;

        virtual void Read(void* buffer, uint64_t length) = 0;
        virtual void Write(const void* buffer, uint64_t length) = 0;

        virtual uint64_t TryRead(void* buffer, uint64_t length) = 0;

        virtual const void* GetData() const
        {
            return nullptr;
        }

        virtual void CopyFromStream(IStream& stream, uint64_t length);

        ///////////////////////////////////////////////////////////////////////////
        // Direct Read/Write methods, class can override them if they're memory-backed.
        ///////////////////////////////////////////////////////////////////////////
        virtual const void* ReadDirect(size_t length)
        {
            return nullptr;
        }

        virtual void* WriteDirectStart(size_t maxLength)
        {
            return nullptr;
        }

        virtual void WriteDirectCommit(size_t length)
        {
        }

        ///////////////////////////////////////////////////////////////////////////
        // Fast path methods, class can override them to use specialised copies.
        ///////////////////////////////////////////////////////////////////////////
    private:
        virtual void Read1(void* buffer)
        {
            Read(buffer, 1);
        }
        virtual void Read2(void* buffer)
        {
            Read(buffer, 2);
        }
        virtual void Read4(void* buffer)
        {
            Read(buffer, 4);
        }
        virtual void Read8(void* buffer)
        {
            Read(buffer, 8);
        }
        virtual void Read16(void* buffer)
        {
            Read(buffer, 16);
        }

        virtual void Write1(const void* buffer)
        {
            Write(buffer, 1);
        }
        virtual void Write2(const void* buffer)
        {
            Write(buffer, 2);
        }
        virtual void Write4(const void* buffer)
        {
            Write(buffer, 4);
        }
        virtual void Write8(const void* buffer)
        {
            Write(buffer, 8);
        }
        virtual void Write16(const void* buffer)
        {
            Write(buffer, 16);
        }

        ///////////////////////////////////////////////////////////////////////////
        // Helper methods
        ///////////////////////////////////////////////////////////////////////////
    public:
        /**
         * Reads the given type from the stream
         */
        template<typename T>
        void ReadValue(T& value)
        {
            // Selects the best path at compile time
            if constexpr (sizeof(T) == 1)
            {
                Read1(&value);
            }
            else if constexpr (sizeof(T) == 2)
            {
                Read2(&value);
            }
            else if constexpr (sizeof(T) == 4)
            {
                Read4(&value);
            }
            else if constexpr (sizeof(T) == 8)
            {
                Read8(&value);
            }
            else if constexpr (sizeof(T) == 16)
            {
                Read16(&value);
            }
            else
            {
                Read(&value, sizeof(T));
            }
        }

        /**
         * Reads the given type from the stream and returns the value directly
         */
        template<typename T>
        T ReadValue()
        {
            T value;
            ReadValue(value);
            return value;
        }

        /**
         * Writes the given type to the stream
         */
        template<typename T>
        void WriteValue(const T& value)
        {
            // Selects the best path at compile time
            if constexpr (sizeof(T) == 1)
            {
                Write1(&value);
            }
            else if constexpr (sizeof(T) == 2)
            {
                Write2(&value);
            }
            else if constexpr (sizeof(T) == 4)
            {
                Write4(&value);
            }
            else if constexpr (sizeof(T) == 8)
            {
                Write8(&value);
            }
            else if constexpr (sizeof(T) == 16)
            {
                Write16(&value);
            }
            else
            {
                Write(&value, sizeof(T));
            }
        }

        template<typename T>
        void ReadArray(T* buffer, size_t count)
        {
            Read(buffer, sizeof(T) * count);
        }

        template<typename T>
        [[nodiscard]] std::unique_ptr<T[]> ReadArray(size_t count)
        {
            auto buffer = std::make_unique<T[]>(count);
            Read(buffer.get(), sizeof(T) * count);
            return buffer;
        }

        template<typename T>
        void WriteArray(T* buffer, size_t count)
        {
            Write(buffer, sizeof(T) * count);
        }

        std::string ReadString();
        void WriteString(std::string_view string);
    };

} // namespace OpenRCT2

#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic pop
#endif

class IOException : public std::runtime_error
{
public:
    explicit IOException(const std::string& message)
        : std::runtime_error(message)
    {
    }
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
            this->setg(
                reinterpret_cast<char*>(const_cast<unsigned char*>(vec.data())),
                reinterpret_cast<char*>(const_cast<unsigned char*>(vec.data())),
                reinterpret_cast<char*>(const_cast<unsigned char*>(vec.data() + vec.size())));
        }
    };

    vector_streambuf _streambuf;

public:
    ivstream(const std::vector<T>& vec)
        : std::istream(&_streambuf)
        , _streambuf(vec)
    {
    }
};
