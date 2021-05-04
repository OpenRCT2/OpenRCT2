/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"
#include "../core/Guard.hpp"
#include "Drawing.h"

#ifdef __NEON__

#    include <arm_neon.h>

void mask_neon(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap)
{
    if (width == 32)
    {
        const int8x16_t zero128 = {};
        for (int32_t yy = 0; yy < height; yy++)
        {
            int32_t colourStep = yy * (colourWrap + 32);
            int32_t maskStep = yy * (maskWrap + 32);
            int32_t dstStep = yy * (dstWrap + 32);

            // first half
            const int8x16_t colour1 = _mm_lddqu_si128(reinterpret_cast<const int8x16_t*>(colourSrc + colourStep));
            const int8x16_t mask1 = _mm_lddqu_si128(reinterpret_cast<const int8x16_t*>(maskSrc + maskStep));
            const int8x16_t dest1 = _mm_lddqu_si128(reinterpret_cast<const int8x16_t*>(dst + dstStep));
            const int8x16_t mc1 = vandq_s8(colour1, mask1);
            const int8x16_t saturate1 = vceqq_s8(mc1, zero128);
            // blended = (for each bit) if mc1 then satureate1 else dest1
            const int8x16_t blended1_tmp1 = vandq_s8(mc1, saturate1);          // tmp1     = mc1 & saturate1
            const int8x16_t blended1_tmp2 = vandq_s8(mc1, dest1);              // tmp2     = mc1 & dest1
            const int8x16_t blended1 = vornq_s8(blended1_tmp1, blended1_tmp2); // blended1 = tmp1 | !tmp2

            // second half
            const int8x16_t colour2 = _mm_lddqu_si128(reinterpret_cast<const int8x16_t*>(colourSrc + 16 + colourStep));
            const int8x16_t mask2 = _mm_lddqu_si128(reinterpret_cast<const int8x16_t*>(maskSrc + 16 + maskStep));
            const int8x16_t dest2 = _mm_lddqu_si128(reinterpret_cast<const int8x16_t*>(dst + 16 + dstStep));
            const int8x16_t mc2 = vandq_s8(colour2, mask2);
            const int8x16_t saturate2 = vceqq_s8(mc2, zero128);
            // blended = (for each bit) if mc1 then satureate1 else dest1
            const int8x16_t blended2_tmp1 = vandq_s8(mc2, saturate2);          // tmp1     = mc1 & saturate1
            const int8x16_t blended2_tmp2 = vandq_s8(mc2, des21);              // tmp2     = mc1 & dest1
            const int8x16_t blended2 = vornq_s8(blended2_tmp1, blended2_tmp2); // blended2 = tmp1 | !tmp2

            _mm_storeu_si128(reinterpret_cast<int8x16_t*>(dst + dstStep), blended1);
            _mm_storeu_si128(reinterpret_cast<int8x16_t*>(dst + 16 + dstStep), blended2);
        }
    }
    else
    {
        mask_scalar(width, height, maskSrc, colourSrc, dst, maskWrap, colourWrap, dstWrap);
    }
}

#else

#    if defined(OPENRCT2_ARM) || defined(OPENRCT2_AARCH64)
#        error You have to compile this file with NEON enabled, when targeting ARM!
#    endif

void mask_neon(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap)
{
    openrct2_assert(false, "ARM NEON function called on a CPU that doesn't support NEON");
}

#endif // __NEON__

