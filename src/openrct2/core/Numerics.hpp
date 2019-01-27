#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

/**
 * Bitwise left rotate
 * @tparam _UIntType unsigned integral type
 * @param x value
 * @param shift positions to shift
 * @return rotated value
 */
template<typename _UIntType> constexpr _UIntType rol(_UIntType x, size_t shift)
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
template<typename _UIntType> constexpr _UIntType ror(_UIntType x, size_t shift)
{
    static_assert(std::is_unsigned<_UIntType>::value, "result_type must be an unsigned integral type");
    using limits = std::numeric_limits<_UIntType>;
    return (((_UIntType)(x) >> shift) | ((_UIntType)(x) << (limits::digits - shift)));
}
