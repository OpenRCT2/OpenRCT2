/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "IStream.hpp"

#include <vector>

namespace OpenRCT2
{
    namespace MEMORY_ACCESS
    {
        constexpr uint8_t READ = 1 << 0;
        constexpr uint8_t WRITE = 1 << 1;
        constexpr uint8_t OWNER = 1 << 2;
    }; // namespace MEMORY_ACCESS

    /**
     * A stream for reading and writing to a buffer in memory. By default this buffer can grow.
     */
    class MemoryStream final : public IStream
    {
    private:
        uint8_t _access = MEMORY_ACCESS::READ | MEMORY_ACCESS::WRITE | MEMORY_ACCESS::OWNER;
        size_t _dataCapacity = 0;
        size_t _dataSize = 0;
        void* _data = nullptr;
        void* _position = nullptr;

    public:
        MemoryStream() = default;
        MemoryStream(const MemoryStream& copy);
        MemoryStream(MemoryStream&& mv) noexcept;
        explicit MemoryStream(size_t capacity);
        MemoryStream(void* data, size_t dataSize, uint8_t access = MEMORY_ACCESS::READ);
        MemoryStream(const void* data, size_t dataSize);
        MemoryStream(std::vector<uint8_t>&& v);
        virtual ~MemoryStream();

        MemoryStream& operator=(MemoryStream&& mv) noexcept;

        const void* GetData() const override;
        void* GetDataCopy() const;
        void* TakeData();

        ///////////////////////////////////////////////////////////////////////////
        // ISteam methods
        ///////////////////////////////////////////////////////////////////////////
        bool CanRead() const override;
        bool CanWrite() const override;

        uint64_t GetLength() const override;
        uint64_t GetPosition() const override;
        void SetPosition(uint64_t position) override;
        void Seek(int64_t offset, int32_t origin) override;

        void Read(void* buffer, uint64_t length) override;
        void Read1(void* buffer) override;
        void Read2(void* buffer) override;
        void Read4(void* buffer) override;
        void Read8(void* buffer) override;
        void Read16(void* buffer) override;

        template<size_t N> void Read(void* buffer)
        {
            uint64_t position = GetPosition();
            if (position + N > _dataSize)
            {
                throw IOException("Attempted to read past end of stream.");
            }

            std::memcpy(buffer, _position, N);
            _position = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(_position) + N);
        }

        void Write(const void* buffer, uint64_t length) override;
        void Write1(const void* buffer) override;
        void Write2(const void* buffer) override;
        void Write4(const void* buffer) override;
        void Write8(const void* buffer) override;
        void Write16(const void* buffer) override;

        template<size_t N> void Write(const void* buffer)
        {
            uint64_t position = GetPosition();
            uint64_t nextPosition = position + N;
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

            std::memcpy(_position, buffer, N);
            _position = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(_position) + N);
            _dataSize = std::max<size_t>(_dataSize, static_cast<size_t>(nextPosition));
        }

        uint64_t TryRead(void* buffer, uint64_t length) override;

        void Clear();

    private:
        void EnsureCapacity(size_t capacity);
    };

} // namespace OpenRCT2
