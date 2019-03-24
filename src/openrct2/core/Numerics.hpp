/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
        return (((_UIntType)(x) << shift) | ((_UIntType)(x) >> (limits::digits - shift)));
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
        return (((_UIntType)(x) >> shift) | ((_UIntType)(x) << (limits::digits - shift)));
    }

} // namespace Numerics
