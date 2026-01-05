/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"
#include "IStream.hpp"

#include <algorithm>
#include <cstring>
#include <vector>

namespace OpenRCT2
{
    enum class MemoryAccess : uint8_t
    {
        read,
        write,
        owner,
    };
    using AccessFlags = FlagHolder<uint8_t, MemoryAccess>;

    /**
     * A stream for reading and writing to a buffer in memory. By default this buffer can grow.
     */
    class MemoryStream final : public IStream
    {
    private:
        AccessFlags _access = { MemoryAccess::read, MemoryAccess::write, MemoryAccess::owner };
        size_t _dataCapacity = 0;
        size_t _dataSize = 0;
        uint8_t* _data = nullptr;
        size_t _position = 0;

    public:
        MemoryStream() = default;
        MemoryStream(const MemoryStream& copy);
        MemoryStream(MemoryStream&& mv) noexcept;
        explicit MemoryStream(size_t capacity);
        MemoryStream(const std::vector<uint8_t>& v);
        MemoryStream(void* data, size_t dataSize, AccessFlags access = { MemoryAccess::read });
        MemoryStream(const void* data, size_t dataSize)
            : MemoryStream(const_cast<void*>(data), dataSize, MemoryAccess::read)
        {
        }
        virtual ~MemoryStream();

        MemoryStream& operator=(MemoryStream&& mv) noexcept;

        ///////////////////////////////////////////////////////////////////////////
        // ISteam methods
        ///////////////////////////////////////////////////////////////////////////

        const void* GetData() const override
        {
            return _data;
        }

        bool CanRead() const override
        {
            return _access.has(MemoryAccess::read);
        }
        bool CanWrite() const override
        {
            return _access.has(MemoryAccess::write);
        }

        uint64_t GetLength() const override
        {
            return _dataSize;
        }
        uint64_t GetPosition() const override
        {
            return _position;
        }

        void SetPosition(uint64_t position) override;
        void Seek(int64_t offset, int32_t origin) override;

        void Read(void* buffer, uint64_t length) override;
        uint64_t TryRead(void* buffer, uint64_t length) override;

        template<size_t N>
        void Read(void* buffer)
        {
            if (_position + N > _dataSize)
                throw IOException("Attempted to read past end of stream.");

            std::memcpy(buffer, _data + _position, N);
            _position += static_cast<size_t>(N);
        }

        void Write(const void* buffer, uint64_t length) override;

        template<size_t N>
        void Write(const void* buffer)
        {
            EnsureWriteCapacity(N);
            std::memcpy(_data + _position, buffer, N);
            _position += static_cast<size_t>(N);
            _dataSize = std::max(_dataSize, _position);
        }

        const void* ReadDirect(size_t length) override;
        void* WriteDirectStart(size_t maxLength) override;
        void WriteDirectCommit(size_t length) override;

        void CopyFromStream(IStream& stream, uint64_t length) override;

        void Clear();

    private:
        void Read1(void* buffer) override
        {
            Read<1>(buffer);
        }
        void Read2(void* buffer) override
        {
            Read<2>(buffer);
        }
        void Read4(void* buffer) override
        {
            Read<4>(buffer);
        }
        void Read8(void* buffer) override
        {
            Read<8>(buffer);
        }
        void Read16(void* buffer) override
        {
            Read<16>(buffer);
        }

        void Write1(const void* buffer) override
        {
            Write<1>(buffer);
        }
        void Write2(const void* buffer) override
        {
            Write<2>(buffer);
        }
        void Write4(const void* buffer) override
        {
            Write<4>(buffer);
        }
        void Write8(const void* buffer) override
        {
            Write<8>(buffer);
        }
        void Write16(const void* buffer) override
        {
            Write<16>(buffer);
        }

        void EnsureWriteCapacity(uint64_t length);
    };

} // namespace OpenRCT2
