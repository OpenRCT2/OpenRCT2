/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"
#include "../core/Guard.hpp"
#include "Drawing.h"

#ifdef __SSE4_1__

#    include <immintrin.h>

void mask_sse4_1(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap)
{
    if (width == 32)
    {
        const __m128i zero128 = {};
        for (int32_t yy = 0; yy < height; yy++)
        {
            int32_t colourStep = yy * (colourWrap + 32);
            int32_t maskStep = yy * (maskWrap + 32);
            int32_t dstStep = yy * (dstWrap + 32);

            // first half
            const __m128i colour1 = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(colourSrc + colourStep));
            const __m128i mask1 = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(maskSrc + maskStep));
            const __m128i dest1 = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(dst + dstStep));
            const __m128i mc1 = _mm_and_si128(colour1, mask1);
            const __m128i saturate1 = _mm_cmpeq_epi8(mc1, zero128);
            // _mm_blendv_epi8 is SSE4.1
            const __m128i blended1 = _mm_blendv_epi8(mc1, dest1, saturate1);

            // second half
            const __m128i colour2 = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(colourSrc + 16 + colourStep));
            const __m128i mask2 = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(maskSrc + 16 + maskStep));
            const __m128i dest2 = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(dst + 16 + dstStep));
            const __m128i mc2 = _mm_and_si128(colour2, mask2);
            const __m128i saturate2 = _mm_cmpeq_epi8(mc2, zero128);
            // _mm_blendv_epi8 is SSE4.1
            const __m128i blended2 = _mm_blendv_epi8(mc2, dest2, saturate2);

            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + dstStep), blended1);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + 16 + dstStep), blended2);
        }
    }
    else
    {
        mask_scalar(width, height, maskSrc, colourSrc, dst, maskWrap, colourWrap, dstWrap);
    }
}

#else

#    ifdef OPENRCT2_X86
#        error You have to compile this file with SSE4.1 enabled, when targeting x86!
#    endif

void mask_sse4_1(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap)
{
    openrct2_assert(false, "SSE 4.1 function called on a CPU that doesn't support SSE 4.1");
}

#endif // __SSE4_1__
