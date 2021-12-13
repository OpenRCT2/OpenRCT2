/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <string>

namespace OpenRCT2
{
    namespace Detail
    {
        namespace BitSet
        {
            static constexpr size_t byte_bits = std::numeric_limits<std::underlying_type_t<std::byte>>::digits;

            template<size_t TNumBits> static constexpr size_t round_bits()
            {
                const auto reminder = TNumBits % byte_bits;
                if constexpr (reminder == 0u)
                {
                    return TNumBits;
                }
                else
                {
                    return TNumBits + (byte_bits - (TNumBits % byte_bits));
                }
            }

            static_assert(round_bits<1>() == 8);
            static_assert(round_bits<4>() == 8);
            static_assert(round_bits<8>() == 8);
            static_assert(round_bits<9>() == 16);
            static_assert(round_bits<9>() == 16);
            static_assert(round_bits<17>() == 24);
            static_assert(round_bits<24>() == 24);
            static_assert(round_bits<31>() == 32);

            // Returns the amount of bytes required for a single block.
            template<size_t TNumBits> static constexpr size_t storage_block_size()
            {
                constexpr size_t numBits = round_bits<TNumBits>();
                if constexpr (numBits >= std::numeric_limits<uintptr_t>::digits)
                {
                    return sizeof(uintptr_t);
                }
                else
                {
                    const auto numBytes = numBits / byte_bits;
                    auto mask = 1u;
                    while (mask < numBytes)
                    {
                        mask <<= 1u;
                    }
                    return mask;
                }
            }

            template<size_t TNumBits, size_t TBlockSizeBytes> static constexpr size_t storage_block_count()
            {
                size_t numBits = TNumBits;
                size_t numBlocks = 0;
                while (numBits > 0)
                {
                    numBlocks++;
                    numBits -= std::min(TBlockSizeBytes * byte_bits, numBits);
                }
                return numBlocks;
            }

            // TODO: Replace with std::popcount when C++20 is enabled.
            template<typename T> static constexpr size_t popcount(const T val)
            {
                size_t res = 0;
                auto x = static_cast<std::make_unsigned_t<T>>(val);
                while (x > 0u)
                {
                    if (x & 1u)
                        res++;
                    x >>= 1u;
                }
                return res;
            }

            static_assert(storage_block_size<1>() == sizeof(uint8_t));
            static_assert(storage_block_size<4>() == sizeof(uint8_t));
            static_assert(storage_block_size<8>() == sizeof(uint8_t));
            static_assert(storage_block_size<9>() == sizeof(uint16_t));
            static_assert(storage_block_size<14>() == sizeof(uint16_t));
            static_assert(storage_block_size<16>() == sizeof(uint16_t));
            static_assert(storage_block_size<18>() == sizeof(uint32_t));
            static_assert(storage_block_size<31>() == sizeof(uint32_t));
            static_assert(storage_block_size<33>() == sizeof(uintptr_t));

            template<size_t TByteSize> struct storage_block_type;

            template<> struct storage_block_type<1>
            {
                using value_type = uint8_t;
            };

            template<> struct storage_block_type<2>
            {
                using value_type = uint16_t;
            };

            template<> struct storage_block_type<4>
            {
                using value_type = uint32_t;
            };

            template<> struct storage_block_type<8>
            {
                using value_type = uint64_t;
            };

            template<size_t TBitSize> struct storage_block_type_aligned
            {
                using value_type = typename storage_block_type<storage_block_size<TBitSize>()>::value_type;
            };
        } // namespace BitSet
    }     // namespace Detail

    template<size_t TBitSize> class BitSet
    {
        static constexpr size_t byte_aligned_bitsize = Detail::BitSet::round_bits<TBitSize>();

        using storage_block_type = typename Detail::BitSet::storage_block_type_aligned<byte_aligned_bitsize>::value_type;

        static constexpr size_t block_byte_size = sizeof(storage_block_type);
        static constexpr size_t block_type_bit_size = block_byte_size * Detail::BitSet::byte_bits;
        static constexpr size_t block_count = Detail::BitSet::storage_block_count<byte_aligned_bitsize, block_byte_size>();
        static constexpr size_t capacity_bits = block_count * block_type_bit_size;

        static constexpr storage_block_type value_zero = storage_block_type{ 0u };
        static constexpr storage_block_type value_one = storage_block_type{ 1u };

        static constexpr storage_block_type block_init_value{};
        static constexpr storage_block_type block_mask_value = static_cast<storage_block_type>(~block_init_value);

        static constexpr bool requires_trim = TBitSize != capacity_bits;

    public:
        using block_type = storage_block_type;
        using storage_data = std::array<block_type, block_count>;

        // Proxy object to access the bits as single value.
        template<typename T> class reference_base
        {
            T& storage_;
            const size_t blockIndex_;
            const size_t blockOffset_;

        public:
            constexpr reference_base(T& data, size_t blockIndex, size_t blockOffset) noexcept
                : storage_(data)
                , blockIndex_(blockIndex)
                , blockOffset_(blockOffset)
            {
            }

            constexpr reference_base& operator=(const bool value) noexcept
            {
                if (!value)
                    storage_[blockIndex_] &= ~(value_one << blockOffset_);
                else
                    storage_[blockIndex_] |= (value_one << blockOffset_);
                return *this;
            }

            constexpr reference_base& operator=(const reference_base& value) noexcept
            {
                return reference_base::operator=(value.value());
            }

            constexpr bool value() const noexcept
            {
                return (storage_[blockIndex_] & (value_one << blockOffset_)) != value_zero;
            }

            constexpr operator bool() const noexcept
            {
                return value();
            }
        };

        using reference = reference_base<storage_data>;
        using const_reference = reference_base<const storage_data>;

        template<typename T, typename TValue> class iterator_base
        {
            T* _bitset{};
            size_t _pos{};

        public:
            constexpr iterator_base() = default;

            constexpr iterator_base(T* bset, size_t pos)
                : _bitset(bset)
                , _pos(pos)
            {
            }

            constexpr auto operator*() const
            {
                const auto blockIndex = compute_block_index(_pos);
                const auto blockOffset = compute_block_offset(_pos);
                return TValue(_bitset->data(), blockIndex, blockOffset);
            }

            constexpr bool operator==(iterator_base other) const
            {
                return _bitset == other._bitset && _pos == other._pos;
            }

            constexpr bool operator!=(iterator_base other) const
            {
                return !(*this == other);
            }

            constexpr iterator_base& operator++()
            {
                _pos++;
                return *this;
            }

            constexpr iterator_base operator++(int)
            {
                iterator_base res = *this;
                ++(*this);
                return res;
            }

            constexpr iterator_base& operator--()
            {
                _pos--;
                return *this;
            }

            constexpr iterator_base operator--(int)
            {
                iterator_base res = *this;
                --(*this);
                return res;
            }

        public:
            using difference_type = std::size_t;
            using value_type = TValue;
            using pointer = const TValue*;
            using reference = const TValue&;
            using iterator_category = std::forward_iterator_tag;
        };

        using iterator = iterator_base<BitSet, reference>;
        using const_iterator = iterator_base<const BitSet, const_reference>;

    private:
        storage_data data_{};

    public:
        constexpr BitSet() = default;

        constexpr BitSet(const storage_block_type val)
            : data_{ val }
        {
        }

        constexpr BitSet(const std::initializer_list<size_t>& indices)
        {
            for (auto idx : indices)
            {
                set(idx, true);
            }
        }

        constexpr size_t size() const noexcept
        {
            return TBitSize;
        }

        constexpr size_t count() const noexcept
        {
            size_t numBits = 0;
            for (auto& data : data_)
            {
                numBits += Detail::BitSet::popcount(data);
            }
            return numBits;
        }

        constexpr size_t capacity() const noexcept
        {
            return capacity_bits;
        }

        constexpr storage_data& data() noexcept
        {
            return data_;
        }

        constexpr const storage_data& data() const noexcept
        {
            return data_;
        }

        constexpr BitSet& set(size_t index, bool value) noexcept
        {
            const auto blockIndex = compute_block_index(index);
            const auto blockOffset = compute_block_offset(index);
            if (!value)
                data_[blockIndex] &= ~(value_one << blockOffset);
            else
                data_[blockIndex] |= (value_one << blockOffset);
            return *this;
        }

        constexpr bool get(size_t index) const noexcept
        {
            const auto blockIndex = compute_block_index(index);
            const auto blockOffset = compute_block_offset(index);
            return (data_[blockIndex] & (value_one << blockOffset)) != value_zero;
        }

        constexpr bool operator[](const size_t index) const noexcept
        {
            const auto blockIndex = compute_block_index(index);
            const auto blockOffset = compute_block_offset(index);
            const_reference ref(data_, blockIndex, blockOffset);
            return ref.value();
        }

        constexpr reference operator[](const size_t index) noexcept
        {
            const auto blockIndex = compute_block_index(index);
            const auto blockOffset = compute_block_offset(index);
            return reference(data_, blockIndex, blockOffset);
        }

        constexpr BitSet& flip() noexcept
        {
            for (auto& data : data_)
            {
                data ^= block_mask_value;
            }
            if constexpr (requires_trim)
            {
                trim();
            }
            return *this;
        }

        constexpr BitSet& reset() noexcept
        {
            std::fill(data_.begin(), data_.end(), block_init_value);
            if constexpr (requires_trim)
            {
                trim();
            }
            return *this;
        }

        constexpr const_iterator begin() const noexcept
        {
            return const_iterator(this, 0);
        }

        constexpr const_iterator end() const noexcept
        {
            return const_iterator(this, size());
        }

        constexpr iterator begin() noexcept
        {
            return iterator(this, 0);
        }

        constexpr iterator end() noexcept
        {
            return iterator(this, size());
        }

        template<class TChar = char, class TTraits = std::char_traits<TChar>, class TAlloc = std::allocator<TChar>>
        std::basic_string<TChar, TTraits, TAlloc> to_string(TChar zero = TChar{ '0' }, TChar one = TChar{ '1' }) const
        {
            std::basic_string<TChar, TTraits, TAlloc> res;
            res.resize(TBitSize);
            for (size_t i = 0; i < TBitSize; ++i)
            {
                // Printed as little-endian.
                res[TBitSize - i - 1] = get(i) ? one : zero;
            }
            return res;
        }

        constexpr BitSet operator^(const BitSet& other) const noexcept
        {
            BitSet res = *this;
            for (size_t i = 0; i < data_.size(); i++)
            {
                res.data_[i] ^= other.data_[i];
            }
            if constexpr (requires_trim)
            {
                res.trim();
            }
            return res;
        }

        constexpr BitSet& operator^=(const BitSet& other) noexcept
        {
            *this = *this ^ other;
            return *this;
        }

        constexpr BitSet operator|(const BitSet& other) const noexcept
        {
            BitSet res = *this;
            for (size_t i = 0; i < data_.size(); i++)
            {
                res.data_[i] |= other.data_[i];
            }
            if constexpr (requires_trim)
            {
                res.trim();
            }
            return res;
        }

        constexpr BitSet& operator|=(const BitSet& other) noexcept
        {
            *this = *this | other;
            return *this;
        }

        constexpr BitSet operator&(const BitSet& other) const noexcept
        {
            BitSet res = *this;
            for (size_t i = 0; i < data_.size(); i++)
            {
                res.data_[i] &= other.data_[i];
            }
            if constexpr (requires_trim)
            {
                res.trim();
            }
            return res;
        }

        constexpr BitSet& operator&=(const BitSet& other) noexcept
        {
            *this = *this & other;
            return *this;
        }

        constexpr BitSet operator~() const noexcept
        {
            BitSet res = *this;
            for (size_t i = 0; i < data_.size(); i++)
            {
                res.data_[i] = ~res.data_[i];
            }
            if constexpr (requires_trim)
            {
                res.trim();
            }
            return res;
        }

        constexpr bool operator<(const BitSet& other) const noexcept
        {
            return std::lexicographical_compare(
                data_.begin(), data_.end(), other.data_.begin(), other.data_.end(), std::less<storage_block_type>{});
        }

        constexpr bool operator<=(const BitSet& other) const noexcept
        {
            return std::lexicographical_compare(
                data_.begin(), data_.end(), other.data_.begin(), other.data_.end(), std::less_equal<storage_block_type>{});
        }

        constexpr bool operator>(const BitSet& other) const noexcept
        {
            return std::lexicographical_compare(
                data_.begin(), data_.end(), other.data_.begin(), other.data_.end(), std::greater<storage_block_type>{});
        }

        constexpr bool operator>=(const BitSet& other) const noexcept
        {
            return std::lexicographical_compare(
                data_.begin(), data_.end(), other.data_.begin(), other.data_.end(), std::greater_equal<storage_block_type>{});
        }

    private:
        static constexpr size_t compute_block_index(size_t idx) noexcept
        {
            if constexpr (block_count == 1)
            {
                return 0;
            }
            else
            {
                return idx / block_type_bit_size;
            }
        }

        static constexpr size_t compute_block_offset(size_t idx) noexcept
        {
            if constexpr (block_count == 1)
            {
                return idx;
            }
            else
            {
                return idx % block_type_bit_size;
            }
        }

        // Some operations require to trim of the excess.
        constexpr void trim() noexcept
        {
            const auto byteIdx = TBitSize / block_type_bit_size;
            const auto bitIdx = TBitSize % block_type_bit_size;
            if constexpr (bitIdx == 0)
                return;

            auto trimMask = block_mask_value;
            trimMask <<= (block_type_bit_size - bitIdx);
            trimMask >>= (block_type_bit_size - bitIdx);

            data_[byteIdx] &= trimMask;
        }
    };

} // namespace OpenRCT2
