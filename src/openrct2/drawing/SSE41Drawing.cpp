#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../common.h"
#include "../core/Guard.hpp"
#include "Drawing.h"

#ifdef __SSE4_1__

#include <immintrin.h>

void mask_sse4_1(sint32 width, sint32 height, const uint8 * RESTRICT maskSrc, const uint8 * RESTRICT colourSrc,
                 uint8 * RESTRICT dst, sint32 maskWrap, sint32 colourWrap, sint32 dstWrap)
{
    if (width == 32)
    {
        const __m128i zero128 = {};
        for (sint32 yy = 0; yy < height; yy++)
        {
            sint32 colourStep = yy * (colourWrap + 32);
            sint32 maskStep   = yy * (maskWrap + 32);
            sint32 dstStep    = yy * (dstWrap + 32);

            // first half
            const __m128i colour1   = _mm_lddqu_si128((const __m128i *)(colourSrc + colourStep));
            const __m128i mask1     = _mm_lddqu_si128((const __m128i *)(maskSrc + maskStep));
            const __m128i dest1     = _mm_lddqu_si128((const __m128i *)(dst + dstStep));
            const __m128i mc1       = _mm_and_si128(colour1, mask1);
            const __m128i saturate1 = _mm_cmpeq_epi8(mc1, zero128);
            // _mm_blendv_epi8 is SSE4.1
            const __m128i blended1 = _mm_blendv_epi8(mc1, dest1, saturate1);

            // second half
            const __m128i colour2   = _mm_lddqu_si128((const __m128i *)(colourSrc + 16 + colourStep));
            const __m128i mask2     = _mm_lddqu_si128((const __m128i *)(maskSrc + 16 + maskStep));
            const __m128i dest2     = _mm_lddqu_si128((const __m128i *)(dst + 16 + dstStep));
            const __m128i mc2       = _mm_and_si128(colour2, mask2);
            const __m128i saturate2 = _mm_cmpeq_epi8(mc2, zero128);
            // _mm_blendv_epi8 is SSE4.1
            const __m128i blended2 = _mm_blendv_epi8(mc2, dest2, saturate2);

            _mm_storeu_si128((__m128i *)(dst + dstStep), blended1);
            _mm_storeu_si128((__m128i *)(dst + 16 + dstStep), blended2);
        }
    }
    else
    {
        mask_scalar(width, height, maskSrc, colourSrc, dst, maskWrap, colourWrap, dstWrap);
    }
}

#else

#ifdef OPENRCT2_X86
#error You have to compile this file with SSE4.1 enabled, when targetting x86!
#endif

void mask_sse4_1(sint32 width, sint32 height, const uint8 * RESTRICT maskSrc, const uint8 * RESTRICT colourSrc,
                 uint8 * RESTRICT dst, sint32 maskWrap, sint32 colourWrap, sint32 dstWrap)
{
    openrct2_assert(false, "SSE 4.1 function called on a CPU that doesn't support SSE 4.1");
}

#endif // __SSE4_1__
