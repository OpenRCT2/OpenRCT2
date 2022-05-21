/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace Numerics
{
    /**
     * Bitwise left rotate
     * @tparam _UIntType unsigned integral type
     * @param x value
     * @param shift positions to shift
     * @return rotated value
     */
    template<typename _UIntType> static constexpr _UIntType rol(_UIntType x, size_t shift)
    {
        static_assert(std::is_unsigned<_UIntType>::value, "result_type must be an unsigned integral type");
        using limits = typename std::numeric_limits<_UIntType>;
        return ((static_cast<_UIntType>(x) << shift) | (static_cast<_UIntType>(x) >> (limits::digits - shift)));
    }

    /**
     * Bitwise left rotate of lowest 4 bits
     * @param x unsigned 8-bit integer value
     * @param shift positions to shift
     * @return rotated value
     */
    [[maybe_unused]] static constexpr uint8_t rol4(uint8_t x, size_t shift)
    {
        x &= 0x0F;
        return (x << shift | x >> (4 - shift)) & 0x0F;
    }

    /**
     * Bitwise right rotate
     * @tparam _UIntType unsigned integral type
     * @param x value
     * @param shift positions to shift
     * @return rotated value
     */
    template<typename _UIntType> static constexpr _UIntType ror(_UIntType x, size_t shift)
    {
        static_assert(std::is_unsigned<_UIntType>::value, "result_type must be an unsigned integral type");
        using limits = std::numeric_limits<_UIntType>;
        return ((static_cast<_UIntType>(x) >> shift) | (static_cast<_UIntType>(x) << (limits::digits - shift)));
    }

    /**
     * Bitwise right rotate of lowest 4 bits
     * @param x unsigned 8-bit integer value
     * @param shift positions to shift
     * @return rotated value
     */
    [[maybe_unused]] static constexpr uint8_t ror4(uint8_t x, size_t shift)
    {
        x &= 0x0F;
        return (x >> shift | x << (4 - shift)) & 0x0F;
    }

    const constexpr auto rol8 = rol<uint8_t>;
    const constexpr auto ror8 = ror<uint8_t>;
    const constexpr auto rol16 = rol<uint16_t>;
    const constexpr auto ror16 = ror<uint16_t>;
    const constexpr auto rol32 = rol<uint32_t>;
    const constexpr auto ror32 = ror<uint32_t>;
    const constexpr auto rol64 = rol<uint64_t>;
    const constexpr auto ror64 = ror<uint64_t>;

} // namespace Numerics
