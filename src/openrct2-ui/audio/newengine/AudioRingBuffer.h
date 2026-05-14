/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace OpenRCT2::Audio
{
#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4324)
#endif

    // Lock-free SPSC ring buffer. Capacity must be a power of two.
    template<typename T, size_t Capacity>
    class AudioRingBuffer
    {
        static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");
        static_assert(Capacity > 0, "Capacity must be greater than zero");
        static_assert(std::is_trivially_copyable_v<T>, "Ring buffer elements must be trivially copyable");

    public:
        bool tryPush(const T& item)
        {
            const uint64_t write = _writeIndex.load(std::memory_order_relaxed);
            const uint64_t read = _readIndex.load(std::memory_order_acquire);

            if (write - read >= Capacity)
                return false;

            _buffer[static_cast<size_t>(write & kMask)] = item;
            _writeIndex.store(write + 1, std::memory_order_release);
            return true;
        }

        bool tryPop(T& out)
        {
            const uint64_t read = _readIndex.load(std::memory_order_relaxed);
            const uint64_t write = _writeIndex.load(std::memory_order_acquire);

            if (read == write)
                return false;

            out = _buffer[static_cast<size_t>(read & kMask)];
            _readIndex.store(read + 1, std::memory_order_release);
            return true;
        }

        [[nodiscard]] size_t size() const
        {
            const uint64_t write = _writeIndex.load(std::memory_order_acquire);
            const uint64_t read = _readIndex.load(std::memory_order_acquire);
            return static_cast<size_t>(write - read);
        }

        [[nodiscard]] bool empty() const
        {
            return size() == 0;
        }

        [[nodiscard]] constexpr size_t capacity() const
        {
            return Capacity;
        }

    private:
        static constexpr uint64_t kMask = static_cast<uint64_t>(Capacity - 1);

        alignas(64) std::atomic<uint64_t> _writeIndex{ 0 };
        alignas(64) std::atomic<uint64_t> _readIndex{ 0 };
        std::array<T, Capacity> _buffer{};
    };

#ifdef _MSC_VER
    #pragma warning(pop)
#endif

} // namespace OpenRCT2::Audio
