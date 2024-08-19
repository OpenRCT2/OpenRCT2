/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "IStream.hpp"

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
        std::byte* _data = nullptr;
        size_t _position = 0;

    public:
        MemoryStream() = default;
        MemoryStream(const MemoryStream& copy);
        MemoryStream(MemoryStream&& mv) noexcept;
        explicit MemoryStream(size_t capacity);
        MemoryStream(void* data, size_t dataSize);
        MemoryStream(const void* data, size_t dataSize);
        // Constructor that takes ownership, the memory has to be allocated by Memory::Allocate, this function only exists for
        // Android interop reasons.
        MemoryStream(const void* data, size_t dataSize, bool);
        virtual ~MemoryStream();

        MemoryStream& operator=(MemoryStream&& mv) noexcept;

        const void* GetData() const override;

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

        void Write(const void* buffer, uint64_t length) override;
        void Write1(const void* buffer) override;
        void Write2(const void* buffer) override;
        void Write4(const void* buffer) override;
        void Write8(const void* buffer) override;
        void Write16(const void* buffer) override;

        uint64_t TryRead(void* buffer, uint64_t length) override;

        void Clear();

    private:
        void EnsureCapacity(size_t capacity);

        template<size_t N> void Read(void* buffer)
        {
            if (_position + N > _dataSize)
            {
                throw IOException("Attempted to read past end of stream.");
            }

            std::memcpy(buffer, _data + _position, N);
            _position += N;
        }

        template<size_t N> void Write(const void* buffer)
        {
            const auto nextPosition = _position + N;
            EnsureCapacity(nextPosition);

            std::memcpy(_data + _position, buffer, N);
            _position += N;
            _dataSize = std::max(_dataSize, nextPosition);
        }
    };

} // namespace OpenRCT2
