/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

#ifdef _MSC_VER
    #include <intrin.h>
#endif

namespace OpenRCT2::Numerics
{
    inline int32_t bitScanForward(uint32_t source)
    {
#if defined(_MSC_VER) && (_MSC_VER >= 1400) // Visual Studio 2005
        unsigned long i;
        uint8_t success = _BitScanForward(&i, source);
        return success != 0 ? i : -1;
#elif defined(__GNUC__)
        int32_t success = __builtin_ffs(source);
        return success - 1;
#else
    #pragma message("Falling back to iterative bitscan32 forward, consider using intrinsics")
        // This is a low-hanging optimisation boost, check if your compiler offers
        // any intrinsic.
        // cf. https://github.com/OpenRCT2/OpenRCT2/pull/2093
        for (int32_t i = 0; i < 32; i++)
            if (source & (1u << i))
                return i;

        return -1;
#endif
    }

    inline int64_t bitScanForward(uint64_t source)
    {
#if defined(_MSC_VER) && (_MSC_VER >= 1400) && (defined(_M_X64) || defined(_M_ARM64)) // Visual Studio 2005
        unsigned long i;
        uint8_t success = _BitScanForward64(&i, source);
        return success != 0 ? i : -1;
#elif defined(__GNUC__)
        int32_t success = __builtin_ffsll(source);
        return success - 1;
#else
    #pragma message("Falling back to iterative bitscan64 forward, consider using intrinsics")
        // This is a low-hanging optimisation boost, check if your compiler offers
        // any intrinsic.
        // cf. https://github.com/OpenRCT2/OpenRCT2/pull/2093
        for (int32_t i = 0; i < 64; i++)
            if (source & (1uLL << i))
                return i;

        return -1;
#endif
    }

    // Rounds an integer down to the given power of 2. Alignment must be a power of 2.
    template<typename T>
    constexpr T floor2(T value, size_t alignment)
    {
        // Ensure alignment is power of two or 0.
        assert(alignment > 0 && std::has_single_bit(alignment));

        return value & ~(alignment - 1);
    }

    // Rounds an integer up to the given power of 2. Alignment must be a power of 2.
    template<typename T>
    constexpr T ceil2(T value, size_t alignment)
    {
        return floor2(static_cast<T>(value + alignment - 1), alignment);
    }

    /**
     * Bitwise left rotate
     * @tparam _UIntType unsigned integral type
     * @param x value
     * @param shift positions to shift
     * @return rotated value
     */
    template<typename _UIntType>
    static constexpr _UIntType rol(_UIntType x, size_t shift)
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
    template<typename _UIntType>
    static constexpr _UIntType ror(_UIntType x, size_t shift)
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

} // namespace OpenRCT2::Numerics
