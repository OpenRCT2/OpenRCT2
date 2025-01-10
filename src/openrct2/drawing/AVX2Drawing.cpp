/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/Guard.hpp"
#include "Drawing.h"

#ifdef __AVX2__

    #include <immintrin.h>

void MaskAvx2(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap)
{
    if (width == 32)
    {
        const int32_t maskWrapSIMD = maskWrap + 32;
        const int32_t colourWrapSIMD = colourWrap + 32;
        const int32_t dstWrapSIMD = dstWrap + 32;
        const __m256i zero = {};
        for (int32_t yy = 0; yy < height; yy++)
        {
            const __m256i colour = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(colourSrc + yy * colourWrapSIMD));
            const __m256i mask = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(maskSrc + yy * maskWrapSIMD));
            const __m256i dest = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(dst + yy * dstWrapSIMD));
            const __m256i mc = _mm256_and_si256(colour, mask);
            const __m256i saturate = _mm256_cmpeq_epi8(mc, zero);
            const __m256i blended = _mm256_blendv_epi8(mc, dest, saturate);
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst + yy * dstWrapSIMD), blended);
        }
    }
    else
    {
        MaskScalar(width, height, maskSrc, colourSrc, dst, maskWrap, colourWrap, dstWrap);
    }
}

#else

    #ifdef OPENRCT2_X86
        #error You have to compile this file with AVX2 enabled, when targeting x86!
    #endif

void MaskAvx2(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap)
{
    OpenRCT2::Guard::Fail("AVX2 function called on a CPU that doesn't support AVX2");
}

#endif // __AVX2__
