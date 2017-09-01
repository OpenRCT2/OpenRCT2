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

#include "../interface/viewport.h"
#include "../paint/paint.h"
#include "supports.h"
#include "map_element/map_element.h"

/** rct2: 0x0097AF20, 0x0097AF21 */
const rct_xy8 loc_97AF20[] = {
    {4,  4},
    {28, 4},
    {4,  28},
    {28, 28},
    {16, 16},
    {16, 4},
    {4,  16},
    {28, 16},
    {16, 28}
};

/** rct2: 0x0097AF32 */
static const uint8 _97AF32[] = {
    5, 2, 5, 2, 5, 2, 5, 2,
    7, 1, 7, 1, 7, 1, 7, 1,
    6, 3, 6, 3, 6, 3, 6, 3,
    8, 0, 8, 0, 8, 0, 8, 0,
    5, 3, 6, 0, 8, 1, 7, 2,
    1, 2, 1, 2, 1, 2, 1, 2,
    0, 3, 0, 3, 0, 3, 0, 3,
    3, 1, 3, 1, 3, 1, 3, 1,
    2, 0, 2, 0, 2, 0, 2, 0,

    6, 1, 6, 1, 6, 1, 6, 1,
    5, 0, 5, 0, 5, 0, 5, 0,
    8, 2, 8, 2, 8, 2, 8, 2,
    7, 3, 7, 3, 7, 3, 7, 3,
    6, 0, 8, 1, 7, 2, 5, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 2, 1, 2, 1, 2, 1,
    1, 3, 1, 3, 1, 3, 1, 3,
    3, 2, 3, 2, 3, 2, 3, 2,

    1, 6, 1, 6, 1, 6, 1, 6,
    3, 5, 3, 5, 3, 5, 3, 5,
    0, 7, 0, 7, 0, 7, 0, 7,
    2, 4, 2, 4, 2, 4, 2, 4,
    8, 1, 7, 2, 5, 3, 6, 0,
    4, 1, 4, 1, 4, 1, 4, 1,
    4, 2, 4, 2, 4, 2, 4, 2,
    4, 0, 4, 0, 4, 0, 4, 0,
    4, 3, 4, 3, 4, 3, 4, 3,

    2, 5, 2, 5, 2, 5, 2, 5,
    0, 4, 0, 4, 0, 4, 0, 4,
    3, 6, 3, 6, 3, 6, 3, 6,
    1, 7, 1, 7, 1, 7, 1, 7,
    7, 2, 5, 3, 6, 0, 8, 1,
    8, 5, 8, 5, 8, 5, 8, 5,
    7, 6, 7, 6, 7, 6, 7, 6,
    6, 4, 6, 4, 6, 4, 6, 4,
    5, 7, 5, 7, 5, 7, 5, 7,
};

/** rct2: 0x0097B052, 0x0097B053 */
const rct_xy16 loc_97B052[] = {
    {-15, -1},
    {0,   -2},
    {-2,  -1},
    {-1,  -15},
    {-26, -1},
    {0,   -2},
    {-2,  -1},
    {-1,  -26}
};

/** rct2: 0x0097B062, 0x0097B063 */
const rct_xy8 _97B062[] = {
    { 18,  3 },
    {  3, 18 },
    { 18,  3 },
    {  3, 18 },
    { 32,  3 },
    {  3, 32 },
    { 32,  3 },
    {  3, 32 }
};

/** rct2: 0x0097B072 */
const uint32 _metalSupportTypeToCrossbeamImages[][8] = {
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // METAL_SUPPORTS_TUBES
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // METAL_SUPPORTS_FORK
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // METAL_SUPPORTS_FORK_ALT
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // METAL_SUPPORTS_BOXED
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // METAL_SUPPORTS_STICK
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // METAL_SUPPORTS_STICK_ALT
    { 3378, 3383, 3378, 3383, 3380, 3385, 3380, 3385 }, // METAL_SUPPORTS_THICK_CENTRED
    { 3378, 3383, 3378, 3383, 3380, 3385, 3380, 3385 }, // METAL_SUPPORTS_THICK
    { 3382, 3379, 3382, 3379, 3384, 3381, 3384, 3381 }, // METAL_SUPPORTS_THICK_ALT
    { 3382, 3379, 3382, 3379, 3384, 3381, 3384, 3381 }, // METAL_SUPPORTS_THICK_ALT_CENTRED
    { 3378, 3379, 3378, 3379, 3380, 3381, 3380, 3381 }, // METAL_SUPPORTS_TRUSS
    { 3386, 3387, 3386, 3387, 3388, 3389, 3388, 3389 }, // METAL_SUPPORTS_TUBES_INVERTED
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // METAL_SUPPORTS_BOXED_COATED
};

/** rct2: 0x0097B142 */
static const uint8 supportTypeToHeight[] = {
    6,
    3,
    3,
    6,
    3,
    3,
    6,
    6,
    6,
    6,
    4,
    3,
    6,
};

typedef struct metal_supports_images {
    uint16 base_id;
    uint16 beam_id;
} metal_supports_images;

/** rct2: 0x0097B15C */
static const metal_supports_images _97B15C[] = {
    { 3243, 3209 },
    { 3279, 3262 },
    { 3298, 3262 },
    { 3334, 3317 },
    {    0, 3658 },
    {    0, 3658 },
    {    0, 3141 },
    {    0, 3158 },
    {    0, 3175 },
    {    0, 3192 },
    {    0, 3124 },
    { 3243, 3209 },
    { 3334, 3353 },
};

/** rct2: 0x0097B190 */
static const metal_supports_images _97B190[] = {
    { 3243, 3226 }, // 3209
    { 3279, 3262 },
    { 3298, 3262 },
    { 3334, 3317 },
    {    0, 3658 },
    {    0, 3658 },
    {    0, 3141 },
    {    0, 3158 },
    {    0, 3175 },
    {    0, 3192 },
    {    0, 3124 },
    { 3243, 3226 }, // 3209
    { 3334, 3353 },
};

/** rct2: 0x0097B404 */
static const uint8 metal_supports_slope_image_map[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,  0,
     0,  0,  0,  0,  0,  0,  0, 15,  0,  0,  0, 16,  0, 17, 18,  0,
};

typedef struct supports_id_desc {
    uint16 full;
    uint16 half;
    uint16 flat;
    uint16 slope;
} supports_id_desc;

/* 0x0097B1C4 */
const supports_id_desc WoodenSupportImageIds[] = {
    { 3392, 3393, 3394, 3536 },
    { 3390, 3391, 3394, 3514 },
    { 3558, 3559, 3560, 3570 },
    { 3561, 3562, 3563, 3592 },
    { 3564, 3565, 3566, 3614 },
    { 3567, 3568, 3569, 3636 },
    { 3677, 3678, 3680, 3739 },
    { 3675, 3676, 3679, 3717 },
    { 3761, 3762, 3763, 3773 },
    { 3764, 3765, 3766, 3795 },
    { 3767, 3768, 3769, 3817 },
    { 3770, 3771, 3772, 3839 },
};

/* 0x0097B224 */
const uint16 WoodenCurveSupportImageIds[] = {
    3465,
    3465,
    0,
    0,
    0,
    0,
    3681,
    3681,
    0,
    0,
    0,
    0,
};

typedef struct unk_supports_desc_bound_box {
    struct {
        uint8 x, y, z;
    } offset;
    struct {
        uint8 x, y, z;
    } length;
} unk_supports_desc_bound_box;

typedef struct unk_supports_desc {
    unk_supports_desc_bound_box bounding_box;
    uint8 var_6;
    uint8 var_7;
} unk_supports_desc;

/* 0x0097B23C */
const unk_supports_desc byte_97B23C[] = {
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{10, 0,  0}, {10, 32, 44}}, 1, 1},
    {{{0,  10, 0}, {32, 10, 44}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{10, 0,  0}, {10, 32, 44}}, 1, 1},
    {{{0,  10, 0}, {32, 10, 44}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{10, 0,  2}, {10, 32, 76}}, 1, 1},
    {{{0,  10, 2}, {32, 10, 76}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  4}},  0, 1},
    {{{0,  0,  0}, {1,  1,  4}},  0, 1},
    {{{0,  0,  0}, {1,  1,  4}},  0, 1},
    {{{0,  0,  0}, {1,  1,  4}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{2,  2,  1}, {28, 28, 2}},  0, 1},
};

/* 0x0098D8D4 */
static const unk_supports_desc byte_98D8D4[] = {
    {{{0, 0, 0}, {1, 1, 4}}, 0, 1},
    {{{0, 0, 0}, {1, 1, 4}}, 0, 1},
    {{{0, 0, 0}, {1, 1, 4}}, 0, 1},
    {{{0, 0, 0}, {1, 1, 4}}, 0, 1},
};

/* 0x0097B3C4 */
const uint16 word_97B3C4[] = {
    0,
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    14,
    0,
    0,
    0,
    17,
    0,
    16,
    15,
    0,
};

extern bool gUseOriginalRidePaint;

/**
 * Adds paint structs for wooden supports.
 *  rct2: 0x006629BC
 * @param supportType (edi) Type and direction of supports.
 * @param special (ax) Used for curved supports.
 * @param height (dx) The height of the supports.
 * @param imageColourFlags (ebp) The colour and palette flags for the support sprites.
 * @param[out] underground (Carry flag) true if underground.
 * @returns (al) true if any supports have been drawn, otherwise false.
 */
bool wooden_a_supports_paint_setup(sint32 supportType, sint32 special, sint32 height, uint32 imageColourFlags, bool* underground)
{
    if (underground != NULL){
        *underground = false;
    }

    if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS) {
        return false;
    }

    if (!(g141E9DB & G141E9DB_FLAG_1)) {
        return false;
    }

    sint32 z = floor2(gPaintSession.Support.height + 15, 16);
    height -= z;
    if (height < 0) {
        if (underground != NULL) {
            *underground = true;
        }
        return false;
    }
    height /= 16;

    bool hasSupports = false;
    bool drawFlatPiece = false;
    sint32 rotation = get_current_rotation();

    // Draw base support (usually shaped to the slope)
    sint32 slope = gPaintSession.Support.slope;
    if (slope & (1 << 5)) {
        // Above scenery (just put a base piece above it)
        drawFlatPiece = true;
    } else if (slope & (1 << 4)) {
        // Steep diagonal (place the correct shaped support for the slope)
        height -= 2;
        if (height < 0) {
            if (underground != NULL) {
                *underground = true;
            }
            return false;
        }

        sint32 imageId = WoodenSupportImageIds[supportType].slope;
        if (imageId == 0) {
            drawFlatPiece = true;
        } else {
            imageId += word_97B3C4[slope & 0x1F];
            imageId |= imageColourFlags;
            sub_98197C(imageId, 0, 0, 32, 32, 11, z, 0, 0, z + 2, rotation);

            sub_98197C(imageId + 4, 0, 0, 32, 32, 11, z + 16, 0, 0, z + 16 + 2, rotation);

            hasSupports = true;
        }
        z += 32;
    } else if ((slope & 0x0F) != 0) {
        // 1 to 3 quarters up
        height--;
        if (height < 0) {
            if (underground != NULL) {
                *underground = true;
            }
            return false;
        }

        sint32 imageId = WoodenSupportImageIds[supportType].slope;
        if (imageId == 0) {
            drawFlatPiece = true;
        } else {
            imageId += word_97B3C4[slope & 0x1F];
            imageId |= imageColourFlags;

            sub_98197C(imageId, 0, 0, 32, 32, 11, z, 0, 0, z + 2, rotation);
            hasSupports = true;
        }
        z += 16;
    }

    // Draw flat base support
    if (drawFlatPiece) {
        sint32 imageId = WoodenSupportImageIds[supportType].flat | imageColourFlags;
        sub_98196C(imageId, 0, 0, 32, 32, 0, z - 2, rotation);
        hasSupports = true;
    }

    // Draw repeated supports for left over space
    while (height != 0) {
        if ((z & 16) == 0 && height >= 2 && z + 16 != gUnk141E9DC) {
            // Full support
            sint32 imageId = WoodenSupportImageIds[supportType].full | imageColourFlags;
            uint8 ah = height == 2 ? 23 : 28;
            sub_98196C(imageId, 0, 0, 32, 32, ah, z, rotation);
            hasSupports = true;
            z += 32;
            height -= 2;
        } else {
            // Half support
            sint32 imageId = WoodenSupportImageIds[supportType].half | imageColourFlags;
            uint8 ah = height == 1 ? 7 : 12;
            sub_98196C(imageId, 0, 0, 32, 32, ah, z, rotation);
            hasSupports = true;
            z += 16;
            height -= 1;
        }
    }

    // Draw special pieces, e.g. curved supports
    if (special != 0) {
        special = (special - 1) & 0xFFFF;

        sint32 imageId = WoodenCurveSupportImageIds[supportType];
        if (imageId != 0 && byte_97B23C[special].var_7 != 0) {
            imageId += special;
            imageId |= imageColourFlags;

            unk_supports_desc_bound_box bBox = byte_97B23C[special].bounding_box;

            if (byte_97B23C[special].var_6 == 0 || gPaintSession.WoodenSupportsPrependTo == NULL) {
                sub_98197C(imageId, 0, 0, bBox.length.x, bBox.length.y, bBox.length.z, z, bBox.offset.x, bBox.offset.y, bBox.offset.z + z, rotation);
                hasSupports = true;
            } else {
                hasSupports = true;
                paint_struct* ps = sub_98198C(imageId, 0, 0, bBox.length.x, bBox.length.y, bBox.length.z, z, bBox.offset.x, bBox.offset.y, bBox.offset.z + z, rotation);
                if (ps != NULL) {
                    paint_struct* edi = gPaintSession.WoodenSupportsPrependTo;
                    edi->var_20 = ps;
                }
            }
        }
    }

    return hasSupports;
}

/**
 * Wooden supports
 *  rct2: 0x00662D5C
 *
 * @param supportType (edi)
 * @param special (ax)
 * @param height (dx)
 * @param imageColourFlags (ebp)
 * @param[out] underground (Carry Flag)
 *
 * @return (al) whether supports have been drawn
 */
bool wooden_b_supports_paint_setup(sint32 supportType, sint32 special, sint32 height, uint32 imageColourFlags, bool * underground)
{
#ifndef NO_RCT2
    if (gUseOriginalRidePaint) {
        sint32 eax = special, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
        RCT2_CALLFUNC_X(0x00662D5C, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
        return eax & 0xFF;
    }
#endif

    bool _9E32B1 = false;

    if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS) {
        if (underground != NULL) *underground = false; // AND
        return false;
    }

    if (!(g141E9DB & G141E9DB_FLAG_1)) {
        if (underground != NULL) *underground = false; // AND
        return false;
    }

    uint16 baseHeight = ceil2(gPaintSession.Support.height, 16);
    sint16 supportLength = height - baseHeight;

    if (supportLength < 0) {
        if (underground != NULL) *underground = true; // STC
        return false;
    }

    sint16 heightSteps = supportLength / 16;

    bool goTo662E8B = false;

    if (gPaintSession.Support.slope & 0x20) {
        goTo662E8B = true;
    } else if (gPaintSession.Support.slope & 0x10) {
        heightSteps -= 2;
        if (heightSteps < 0) {
            if (underground != NULL) *underground = true; // STC
            return false;
        }

        uint32 imageId = WoodenSupportImageIds[supportType].slope;
        if (imageId == 0) {
            baseHeight += 32;
            goTo662E8B = true;
        } else {
            imageId += word_97B3C4[gPaintSession.Support.slope & 0x1F];

            sub_98197C(
                imageId | imageColourFlags,
                0, 0,
                32, 32, 11,
                baseHeight,
                0, 0, baseHeight + 2,
                get_current_rotation()
            );
            baseHeight += 16;

            sub_98197C(
                (imageId + 4) | imageColourFlags,
                0, 0,
                32, 32, 3,
                baseHeight,
                0, 0, baseHeight + 2,
                get_current_rotation()
            );
            baseHeight += 16;

            _9E32B1 = true;
        }
    } else if ((gPaintSession.Support.slope & 0x0F) != 0) {
        heightSteps -= 1;
        if (heightSteps < 0) {
            if (underground != NULL) *underground = true; // STC
            return false;
        }

        uint32 imageId = WoodenSupportImageIds[supportType].slope;
        if (imageId == 0) {
            baseHeight += 16;
            goTo662E8B = true;
        } else {
            imageId += word_97B3C4[gPaintSession.Support.slope & 0x1F];

            sub_98197C(
                imageId | imageColourFlags,
                0, 0,
                32, 32, 3,
                baseHeight,
                0, 0, baseHeight + 2,
                get_current_rotation()
            );
            baseHeight += 16;

            _9E32B1 = true;
        }
    }

    bool skipTo663004 = false;
    if (goTo662E8B) {
        if (heightSteps == 0) {
            skipTo663004 = true;
        } else {
            sub_98196C(
                WoodenSupportImageIds[supportType].flat | imageColourFlags,
                0, 0,
                32, 32, 0,
                baseHeight - 2,
                get_current_rotation()
            );
            _9E32B1 = true;
        }
    }

    if (!skipTo663004) {
        while (heightSteps > 0) {
            if (baseHeight & 0x10 || heightSteps == 1 || baseHeight + 16 == gUnk141E9DC) {
                sub_98196C(
                    WoodenSupportImageIds[supportType].half | imageColourFlags,
                    0, 0,
                    32, 32, ((heightSteps == 1) ? 7 : 12),
                    baseHeight,
                    get_current_rotation()
                );
                heightSteps -= 1;
                baseHeight += 16;
                _9E32B1 = true;
            } else {
                sub_98196C(
                    WoodenSupportImageIds[supportType].full | imageColourFlags,
                    0, 0,
                    32, 32, ((heightSteps == 2) ? 23 : 28),
                    baseHeight,
                    get_current_rotation()
                );
                heightSteps -= 2;
                baseHeight += 32;
                _9E32B1 = true;
            }
        }
    }

    if (special != 0) {
        uint16 specialIndex = (special - 1) & 0xFFFF;

        uint32 imageId = WoodenCurveSupportImageIds[supportType];
        unk_supports_desc supportsDesc = byte_97B23C[specialIndex];

        if (imageId != 0 && supportsDesc.var_7 != 0) { // byte_97B23C[special].var_7 is never 0
            imageId = (imageId + specialIndex) | imageColourFlags;

            unk_supports_desc_bound_box boundBox = supportsDesc.bounding_box;

            if (supportsDesc.var_6 == 0 || gPaintSession.WoodenSupportsPrependTo == NULL) {
                sub_98197C(
                    imageId | imageColourFlags,
                    0, 0,
                    boundBox.length.x, boundBox.length.y, boundBox.length.z,
                    baseHeight,
                    boundBox.offset.x, boundBox.offset.y, boundBox.offset.z + baseHeight,
                    get_current_rotation()
                );
                _9E32B1 = true;
            } else {
                paint_struct * paintStruct = sub_98198C(
                    imageId | imageColourFlags,
                    0, 0,
                    boundBox.length.x, boundBox.length.y, boundBox.length.z,
                    baseHeight,
                    boundBox.offset.x, boundBox.offset.y, boundBox.offset.z + baseHeight,
                    get_current_rotation()
                );
                _9E32B1 = true;
                if (paintStruct != NULL) {
                    gPaintSession.WoodenSupportsPrependTo->var_20 = paintStruct;
                }
            }
        }
    }

    if (underground != NULL) *underground = false; // AND
    return _9E32B1;
}

/**
 * Metal pole supports
 * @param supportType (edi)
 * @param segment (ebx)
 * @param special (ax)
 * @param height (edx)
 * @param imageColourFlags (ebp)
 *  rct2: 0x00663105
 */
bool metal_a_supports_paint_setup(uint8 supportType, uint8 segment, sint32 special, sint32 height, uint32 imageColourFlags)
{
    support_height * supportSegments = gPaintSession.SupportSegments;

    if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS) {
        return false;
    }

    if (!(g141E9DB & G141E9DB_FLAG_1)) {
        return false;
    }

    sint16 originalHeight = height;
    sint32 originalSegment = segment;

    const uint8 rotation = get_current_rotation();
    sint16 unk9E3294 = -1;
    if (height < supportSegments[segment].height){
        unk9E3294 = height;

        height -= supportTypeToHeight[supportType];
        if (height < 0)
            return false;

        const uint8* esi = &(_97AF32[rotation * 2]);

        uint8 newSegment = esi[segment * 8];
        if (height <= supportSegments[newSegment].height) {
            esi += 72;
            newSegment = esi[segment * 8];
            if (height <= supportSegments[newSegment].height) {
                esi += 72;
                newSegment = esi[segment * 8];
                if (height <= supportSegments[newSegment].height) {
                    esi += 72;
                    newSegment = esi[segment * 8];
                    if (height <= supportSegments[newSegment].height) {
                        return false;
                    }
                }
            }
        }

        uint8 ebp = esi[segment * 8 + 1];

        sint8 xOffset = loc_97AF20[segment].x;
        sint8 yOffset = loc_97AF20[segment].y;
        xOffset += loc_97B052[ebp].x;
        yOffset += loc_97B052[ebp].y;

        sint16 boundBoxLengthX = _97B062[ebp].x;
        sint16 boundBoxLengthY = _97B062[ebp].y;

        uint32 image_id = _metalSupportTypeToCrossbeamImages[supportType][ebp];
        image_id |= imageColourFlags;
        sub_98196C(image_id, xOffset, yOffset, boundBoxLengthX, boundBoxLengthY, 1, height, rotation);

        segment = newSegment;
    }
    sint16 si = height;
    if (supportSegments[segment].slope & (1 << 5) ||
        height - supportSegments[segment].height < 6 ||
        _97B15C[supportType].base_id == 0
        ) {

        height = supportSegments[segment].height;
    }else{
        sint8 xOffset = loc_97AF20[segment].x;
        sint8 yOffset = loc_97AF20[segment].y;

        uint32 image_id = _97B15C[supportType].base_id;
        image_id += metal_supports_slope_image_map[supportSegments[segment].slope & 0x1F];
        image_id |= imageColourFlags;

        sub_98196C(image_id, xOffset, yOffset, 0, 0, 5, supportSegments[segment].height, rotation);

        height = supportSegments[segment].height + 6;
    }


    // Work out if a small support segment required to bring support to normal
    // size (aka floor2(x, 16))
    sint16 heightDiff = floor2(height + 16, 16);
    if (heightDiff > si) {
        heightDiff = si;
    }

    heightDiff -= height;

    if (heightDiff > 0) {
        sint8 xOffset = loc_97AF20[segment].x;
        sint8 yOffset = loc_97AF20[segment].y;

        uint32 image_id = _97B15C[supportType].beam_id;
        image_id += heightDiff - 1;
        image_id |= imageColourFlags;


        sub_98196C(image_id, xOffset, yOffset, 0, 0, heightDiff - 1, height, rotation);
    }

    height += heightDiff;
    //6632e6

    for (uint8 count = 0; ;count++) {
        if (count >= 4)
            count = 0;

        sint16 z = height + 16;
        if (z > si) {
            z = si;
        }

        z -= height;
        if (z <= 0)
            break;

        sint8 xOffset = loc_97AF20[segment].x;
        sint8 yOffset = loc_97AF20[segment].y;

        uint32 image_id = _97B15C[supportType].beam_id;
        image_id += z - 1;
        image_id |= imageColourFlags;

        if (count == 3 && z == 0x10)
            image_id++;

        sub_98196C(image_id, xOffset, yOffset, 0, 0, z - 1, height, rotation);

        height += z;
    }

    supportSegments[segment].height = unk9E3294;
    supportSegments[segment].slope = 0x20;

    height = originalHeight;
    segment = originalSegment;
    if (special == 0)
        return true;

    if (special < 0) {
        special = -special;
        height--;
    }

    sint16 boundBoxOffsetX = loc_97AF20[segment].x;
    sint16 boundBoxOffsetY = loc_97AF20[segment].y;
    sint16 boundBoxOffsetZ = height;
    si = height + special;

    while (1) {
        sint16 z = height + 16;
        if (z > si) {
            z = si;
        }

        z -= height;
        if (z <= 0)
            break;

        sint8 xOffset = loc_97AF20[segment].x;
        sint8 yOffset = loc_97AF20[segment].y;

        uint32 image_id = _97B190[supportType].beam_id;
        image_id += z - 1;
        image_id |= imageColourFlags;

        sub_98197C(image_id, xOffset, yOffset, 0, 0, 0, height, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ, rotation);

        height += z;
    }

    return true;

    //sint32 eax = special, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
    //RCT2_CALLFUNC_X(0x00663105, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
    //return eax & 0xFF;
}

/**
 * Metal pole supports
 *  rct2: 0x00663584
 *
 * @param supportType (edi)
 * @param segment (ebx)
 * @param special (ax)
 * @param height (edx)
 * @param imageColourFlags (ebp)
 *
 * @return (Carry Flag)
 */
bool metal_b_supports_paint_setup(uint8 supportType, uint8 segment, sint32 special, sint32 height, uint32 imageColourFlags)
{
#ifndef NO_RCT2
    if (gUseOriginalRidePaint) {
        sint32 eax = special, ebx = segment, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
        RCT2_CALLFUNC_X(0x00663584, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
        return eax & 0xFF;
    }
#endif

    support_height * supportSegments = gPaintSession.SupportSegments;
    uint8 originalSegment = segment;

    if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS) {
        return false; // AND
    }

    if (!(g141E9DB & G141E9DB_FLAG_1)) {
        return false; // AND
    }

    uint16 _9E3294 = 0xFFFF;
    sint32 baseHeight = height;

    if (height < supportSegments[segment].height) {
        _9E3294 = height;

        baseHeight -= supportTypeToHeight[supportType];
        if (baseHeight < 0) {
            return false; // AND
        }

        uint16 baseIndex = get_current_rotation() * 2;

        uint8 ebp = _97AF32[baseIndex + segment * 8];
        if (baseHeight <= supportSegments[ebp].height) {
            baseIndex += 9 * 4 * 2; // 9 segments, 4 directions, 2 values
            uint8 ebp2 = _97AF32[baseIndex + segment * 8];
            if (baseHeight <= supportSegments[ebp2].height) {
                baseIndex += 9 * 4 * 2;
                uint8 ebp3 = _97AF32[baseIndex + segment * 8];
                if (baseHeight <= supportSegments[ebp3].height) {
                    baseIndex += 9 * 4 * 2;
                    uint8 ebp4 = _97AF32[baseIndex + segment * 8];
                    if (baseHeight <= supportSegments[ebp4].height) {
                        return true; // STC
                    }
                }

            }
        }

        ebp = _97AF32[baseIndex + segment * 8 + 1];
        if (ebp >= 4) {
            return true; // STC
        }

        sub_98196C(
            _metalSupportTypeToCrossbeamImages[supportType][ebp] | imageColourFlags,
            loc_97AF20[originalSegment].x + loc_97B052[ebp].x, loc_97AF20[originalSegment].y + loc_97B052[ebp].y,
            _97B062[ebp].x, _97B062[ebp].y, 1,
            baseHeight,
            get_current_rotation()
        );
    }

    sint32 si = baseHeight;

    if ((supportSegments[segment].slope & 0x20)
        || (baseHeight - supportSegments[segment].height < 6)
        || (_97B15C[supportType].base_id == 0)) {
        baseHeight = supportSegments[segment].height;
    } else {
        uint32 imageOffset = metal_supports_slope_image_map[supportSegments[segment].slope & 0x1F];
        uint32 imageId = _97B15C[supportType].base_id + imageOffset;

        sub_98196C(
            imageId | imageColourFlags,
            loc_97AF20[segment].x, loc_97AF20[segment].y,
            0, 0, 5,
            supportSegments[segment].height,
            get_current_rotation()
        );

        baseHeight = supportSegments[segment].height + 6;
    }

    sint16 heightDiff = floor2(baseHeight + 16, 16);
    if (heightDiff > si) {
        heightDiff = si;
    }

    heightDiff -= baseHeight;
    if (heightDiff > 0) {
        sub_98196C(
            (_97B15C[supportType].beam_id + (heightDiff - 1)) | imageColourFlags,
            loc_97AF20[segment].x, loc_97AF20[segment].y,
            0, 0, heightDiff - 1,
            baseHeight,
            get_current_rotation()
        );
    }

    baseHeight += heightDiff;


    sint16 endHeight;


    sint32 i = 1;
    while (true) {
        endHeight = baseHeight + 16;
        if (endHeight > si) {
            endHeight = si;
        }

        sint16 beamLength = endHeight - baseHeight;

        if (beamLength <= 0) {
            break;
        }

        uint32 imageId = _97B15C[supportType].beam_id + (beamLength - 1);

        if (i % 4 == 0) {
            // Each fourth run, draw a special image
            if (beamLength == 16) {
                imageId += 1;
            }
        }

        sub_98196C(
            imageId | imageColourFlags,
            loc_97AF20[segment].x, loc_97AF20[segment].y,
            0, 0, beamLength - 1,
            baseHeight,
            get_current_rotation()
        );

        baseHeight += beamLength;
        i++;
    }

    supportSegments[segment].height = _9E3294;
    supportSegments[segment].slope = 0x20;

    if (special != 0) {
        baseHeight = height;
        si = height + special;
        while (true) {
            endHeight = baseHeight + 16;
            if (endHeight > si) {
                endHeight = si;
            }

            sint16 beamLength = endHeight - baseHeight;
            if (beamLength <= 0) {
                break;
            }

            uint32 imageId = _97B15C[supportType].beam_id + (beamLength - 1);
            sub_98197C(
                imageId | imageColourFlags,
                loc_97AF20[originalSegment].x, loc_97AF20[originalSegment].y,
                0, 0, 0,
                baseHeight,
                loc_97AF20[originalSegment].x, loc_97AF20[originalSegment].y, height,
                get_current_rotation()
            );
            baseHeight += beamLength;
        }
    }

    return false; // AND
}

/**
 *  rct2: 0x006A2ECC
 *
 * @param supportType (edi)
 * @param special (ax)
 * @param height (dx)
 * @param imageColourFlags (ebp)
 * @param pathEntry (0x00F3EF6C)
 * @param[out] underground (Carry Flag)
 *
 * @return Whether supports were drawn
 */
bool path_a_supports_paint_setup(sint32 supportType, sint32 special, sint32 height, uint32 imageColourFlags,
                                 rct_footpath_entry * pathEntry, bool * underground)
{
#ifndef NO_RCT2
    if (gUseOriginalRidePaint) {
        RCT2_GLOBAL(0xF3EF6C, rct_footpath_entry *) = pathEntry;
        sint32 eax = special, ebx = 0, ecx = 0, edx = height, esi = 0, _edi = supportType, ebp = imageColourFlags;
        RCT2_CALLFUNC_X(0x006A2ECC, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
        return eax & 0xFF;
    }
#endif

    if (underground != NULL) {
        *underground = false; // AND
    }

    if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS) {
        return false;
    }

    if (!(g141E9DB & G141E9DB_FLAG_1)) {
        return false;
    }

    uint16 baseHeight = ceil2(gPaintSession.Support.height, 16);
    sint32 supportLength = height - baseHeight;
    if (supportLength < 0) {
        if (underground != NULL) *underground = true; // STC
        return false;
    }

    bool hasSupports = false;

    sint16 heightSteps = supportLength / 16;

    if (gPaintSession.Support.slope & 0x20) {
        //save dx2
        sub_98196C(
            (pathEntry->bridge_image + 48) | imageColourFlags,
            0, 0,
            32, 32, 0,
            baseHeight - 2,
            get_current_rotation()
        );
        hasSupports = true;
    } else if (gPaintSession.Support.slope & 0x10) {
        heightSteps -= 2;
        if (heightSteps < 0) {
            if (underground != NULL) *underground = true; // STC
            return false;
        }

        uint32 imageId = (supportType * 24) + word_97B3C4[gPaintSession.Support.slope & 0x1F] + pathEntry->bridge_image;

        sub_98197C(
            imageId | imageColourFlags,
            0, 0,
            32, 32, 11,
            baseHeight,
            0, 0, baseHeight + 2,
            get_current_rotation()
        );
        baseHeight += 16;

        sub_98197C(
            (imageId + 4) | imageColourFlags,
            0, 0,
            32, 32, 11,
            baseHeight,
            0, 0, baseHeight + 2,
            get_current_rotation()
        );
        baseHeight += 16;

        hasSupports = true;

    } else if (gPaintSession.Support.slope & 0x0F) {
        heightSteps -= 1;
        if (heightSteps < 0) {
            if (underground != NULL) *underground = true; // STC
            return false;
        }

        uint32 ebx = (supportType * 24) + word_97B3C4[gPaintSession.Support.slope & 0x1F] + pathEntry->bridge_image;

        sub_98197C(
            ebx | imageColourFlags,
            0, 0,
            32, 32, 11,
            baseHeight,
            0, 0, baseHeight + 2,
            get_current_rotation()
        );

        hasSupports = true;
        baseHeight += 16;
    }

    while (heightSteps > 0) {
        if (baseHeight & 0x10 || heightSteps == 1 || baseHeight + 16 == gUnk141E9DC) {

            uint32 imageId = (supportType * 24) + pathEntry->bridge_image + 23;

            sub_98196C(
                imageId | imageColourFlags,
                0, 0,
                32, 32, ((heightSteps == 1) ? 7 : 12),
                baseHeight,
                get_current_rotation()
            );
            heightSteps -= 1;
            baseHeight += 16;
            hasSupports = true;
        } else {
            uint32 imageId = (supportType * 24) + pathEntry->bridge_image + 22;

            sub_98196C(
                imageId | imageColourFlags,
                0, 0,
                32, 32, ((heightSteps == 2) ? 23 : 28),
                baseHeight,
                get_current_rotation()
            );
            heightSteps -= 2;
            baseHeight += 32;
            hasSupports = true;
        }
    }

    if (special != 0) {
        uint16 specialIndex = (special - 1) & 0xFFFF;

        uint32 imageId = pathEntry->bridge_image + 55 + specialIndex;

        unk_supports_desc supportsDesc = byte_98D8D4[specialIndex];
        unk_supports_desc_bound_box boundBox = supportsDesc.bounding_box;

        if (supportsDesc.var_6 == 0 || gPaintSession.WoodenSupportsPrependTo == NULL) {
            sub_98197C(
                imageId | imageColourFlags,
                0, 0,
                boundBox.length.y, boundBox.length.x, boundBox.length.z,
                baseHeight,
                boundBox.offset.x, boundBox.offset.y, baseHeight + boundBox.offset.z,
                get_current_rotation()
            );
            hasSupports = true;
        } else {
            paint_struct * paintStruct = sub_98198C(
                imageId | imageColourFlags,
                0, 0,
                boundBox.length.y, boundBox.length.x, boundBox.length.z,
                baseHeight,
                boundBox.offset.x, boundBox.offset.y, baseHeight + boundBox.offset.z,
                get_current_rotation()
            );
            hasSupports = true;
            if (paintStruct != NULL) {
                gPaintSession.WoodenSupportsPrependTo->var_20 = paintStruct;
            }
        }
    }

    if (underground != NULL) *underground = false; // AND

    return hasSupports;
}

/**
 *
 *  rct2: 0x006A326B
 *
 * @param segment (ebx)
 * @param special (ax)
 * @param height (dx)
 * @param imageColourFlags (ebp)
 * @param pathEntry (0x00F3EF6C)
 *
 * @return Whether supports were drawn
 */
bool path_b_supports_paint_setup(sint32 segment, sint32 special, sint32 height, uint32 imageColourFlags,
                                 rct_footpath_entry * pathEntry)
{
#ifndef NO_RCT2
    if (gUseOriginalRidePaint) {
        RCT2_GLOBAL(0xF3EF6C, rct_footpath_entry *) = pathEntry;
        sint32 eax = special, ebx = segment, ecx = 0, edx = height, esi = 0, _edi = 0, ebp = imageColourFlags;
        RCT2_CALLFUNC_X(0x006A326B, &eax, &ebx, &ecx, &edx, &esi, &_edi, &ebp);
        return eax & 0xFF;
    }
#endif

    support_height * supportSegments = gPaintSession.SupportSegments;

    if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS) {
        return false; // AND
    }

    if (!(g141E9DB & G141E9DB_FLAG_1)) {
        return false; // AND
    }

    if (height < supportSegments[segment].height) {
        return true; // STC
    }

    uint16 baseHeight;

    if ((supportSegments[segment].slope & 0x20)
        || (height - supportSegments[segment].height < 6)
        || !(pathEntry->flags & FOOTPATH_ENTRY_FLAG_HAS_SUPPORT_BASE_SPRITE)) {
        baseHeight = supportSegments[segment].height;
    } else {
        uint8 imageOffset = metal_supports_slope_image_map[supportSegments[segment].slope & 0x1F];
        baseHeight = supportSegments[segment].height;

        sub_98196C(
            (pathEntry->bridge_image + 37 + imageOffset) | imageColourFlags,
            loc_97AF20[segment].x, loc_97AF20[segment].y,
            0, 0, 5,
            baseHeight,
            get_current_rotation()
        );
        baseHeight += 6;
    }

    // si = height
    // dx = baseHeight

    sint16 heightDiff = floor2(baseHeight + 16, 16);
    if (heightDiff > height) {
        heightDiff = height;
    }

    heightDiff -= baseHeight;

    if (heightDiff > 0) {
        sub_98196C(
            (pathEntry->bridge_image + 20 + (heightDiff - 1)) | imageColourFlags,
            loc_97AF20[segment].x, loc_97AF20[segment].y,
            0, 0, heightDiff - 1,
            baseHeight,
            get_current_rotation()
        );
    }

    baseHeight += heightDiff;

    bool keepGoing = true;
    while (keepGoing) {
        sint16 z;

        for (sint32 i = 0; i < 4; ++i) {
            z = baseHeight + 16;
            if (z > height) {
                z = height;
            }
            z -= baseHeight;

            if (z <= 0) {
                keepGoing = false;
                break;
            }

            if (i == 3) {
                // Only do the z check in the fourth run.
                break;
            }

            sub_98196C(
                (pathEntry->bridge_image + 20 + (z - 1)) | imageColourFlags,
                loc_97AF20[segment].x, loc_97AF20[segment].y,
                0, 0, (z - 1),
                baseHeight,
                get_current_rotation()
            );

            baseHeight += z;
        }

        if (keepGoing == false) {
            break;
        }

        uint32 imageId = pathEntry->bridge_image + 20 + (z - 1);
        if (z == 16) {
            imageId += 1;
        }

        sub_98196C(
            imageId | imageColourFlags,
            loc_97AF20[segment].x, loc_97AF20[segment].y,
            0, 0, (z - 1),
            baseHeight,
            get_current_rotation()
        );

        baseHeight += z;
    }

    // loc_6A34D8
    supportSegments[segment].height = 0xFFFF;
    supportSegments[segment].slope = 0x20;

    if (special != 0) {
        sint16 si = special + baseHeight;

        while (true) {
            sint16 z = baseHeight + 16;
            if (z > si) {
                z = si;
            }

            z -= baseHeight;
            if (z <= 0) {
                break;
            }

            uint32 imageId = pathEntry->bridge_image + 20 + (z - 1);
            sub_98197C(
                imageId | imageColourFlags,
                loc_97AF20[segment].x, loc_97AF20[segment].y,
                0, 0, 0,
                baseHeight,
                loc_97AF20[segment].x, loc_97AF20[segment].y, baseHeight,
                get_current_rotation()
            );

            baseHeight += z;
        }
    }

    return false; // AND
}
