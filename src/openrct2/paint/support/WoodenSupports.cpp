/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WoodenSupports.h"

#include "../../sprites.h"
#include "../../world/Map.h"
#include "../../world/Surface.h"
#include "../Boundbox.h"
#include "../Paint.SessionFlags.h"
#include "../Paint.h"
#include "Generic.h"

struct SupportsIdDescriptor
{
    uint16_t full;
    uint16_t half;
    uint16_t flat;
    uint16_t slope;
};

// clang-format off
/* 0x0097B1C4 */
static constexpr SupportsIdDescriptor WoodenSupportImageIds[] = {
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

static constexpr uint16_t WoodenCurveSupportImageIds0[81] = {
    3465,
    3466,
    3467,
    3468,
    3469,
    3470,
    3471,
    3472,
    3473,
    3474,
    3475,
    3476,
    3477,
    3478,
    3479,
    3480,
    3481,
    3482,
    3483,
    3484,
    3485,
    3486,
    3487,
    3488,
    3489,
    3490,
    3491,
    3492,
    3493,
    3494,
    3495,
    3496,
    3497,
    3498,
    3499,
    3500,
    3501,
    3502,
    3503,
    3504,
    3505,
    3506,
    3507,
    3508,
    3509,
    3510,
    3511,
    3512,
    3513,
    SPR_G2_SUPPORT_BEGIN,
    SPR_G2_SUPPORT_BEGIN + 1,
    SPR_G2_SUPPORT_BEGIN + 2,
    SPR_G2_SUPPORT_BEGIN + 3,
    SPR_G2_SUPPORT_BEGIN + 4,
    SPR_G2_SUPPORT_BEGIN + 5,
    SPR_G2_SUPPORT_BEGIN + 6,
    SPR_G2_SUPPORT_BEGIN + 7,
    SPR_G2_SUPPORT_BEGIN + 8,
    SPR_G2_SUPPORT_BEGIN + 9,
    SPR_G2_SUPPORT_BEGIN + 10,
    SPR_G2_SUPPORT_BEGIN + 11,
    SPR_G2_SUPPORT_BEGIN + 12,
    SPR_G2_SUPPORT_BEGIN + 13,
    SPR_G2_SUPPORT_BEGIN + 14,
    SPR_G2_SUPPORT_BEGIN + 15,
    SPR_G2_SUPPORT_BEGIN + 16,
    SPR_G2_SUPPORT_BEGIN + 17,
    SPR_G2_SUPPORT_BEGIN + 18,
    SPR_G2_SUPPORT_BEGIN + 19,
    SPR_G2_SUPPORT_BEGIN + 20,
    SPR_G2_SUPPORT_BEGIN + 21,
    SPR_G2_SUPPORT_BEGIN + 22,
    SPR_G2_SUPPORT_BEGIN + 23,
    SPR_G2_SUPPORT_BEGIN + 24,
    SPR_G2_SUPPORT_BEGIN + 25,
    SPR_G2_SUPPORT_BEGIN + 26,
    SPR_G2_SUPPORT_BEGIN + 27,
    SPR_G2_SUPPORT_BEGIN + 28,
    SPR_G2_SUPPORT_BEGIN + 29,
    SPR_G2_SUPPORT_BEGIN + 30,
    SPR_G2_SUPPORT_BEGIN + 31,
};

static constexpr uint16_t WoodenCurveSupportImageIds1[81] = {
    3681, 3682, 3683, 3684, 3685, 3686, 3687, 3688, 3689, 3690, 3691, 3692, 3693, 3694, 3695, 3696, 3697,
    3698, 3699, 3700, 3701, 3702, 3703, 3704, 3705, 3706, 3707, 3708, 3709, 3710, 3711, 3712, 3713, 3714,
    3715, 3716, 3717, 3718, 3719, 3720, 3721, 3722, 3723, 3724, 3725, 3726, 3727, 3728, 3729,
};

/* 0x0097B224 */
static constexpr const uint16_t* WoodenCurveSupportImageIds[12] = {
    WoodenCurveSupportImageIds0,
    WoodenCurveSupportImageIds0,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    WoodenCurveSupportImageIds1,
    WoodenCurveSupportImageIds1,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};


struct UnkSupportsDescriptor {
    BoundBoxXYZ bounding_box;
    uint8_t var_6;
    uint8_t var_7;
};

/* 0x0097B23C */
static constexpr UnkSupportsDescriptor Byte97B23C[] = {
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Flat to gentle
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Gentle to flat
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Gentle slope
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Gentle to steep
    {{{10, 0,  0}, {10, 32, 44}}, 1, 1},
    {{{0,  10, 0}, {32, 10, 44}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep to gentle
    {{{10, 0,  0}, {10, 32, 44}}, 1, 1},
    {{{0,  10, 0}, {32, 10, 44}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep slope
    {{{10, 0,  2}, {10, 32, 76}}, 1, 1},
    {{{0,  10, 2}, {32, 10, 76}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  4}},  0, 1}, // Slope
    {{{0,  0,  0}, {1,  1,  4}},  0, 1},
    {{{0,  0,  0}, {1,  1,  4}},  0, 1},
    {{{0,  0,  0}, {1,  1,  4}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Flat to steep small
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep to flat small
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // ?
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // ?
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // ?
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{2,  2,  1}, {28, 28, 2}},  0, 1}, // Large scenery (all directions)
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Flat to steep large 1
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Flat to steep large 2
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Flat to steep large 3
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Flat to steep large 4
    {{{10, 0,  2}, {10, 32, 52}}, 1, 1},
    {{{0,  10, 2}, {32, 10, 52}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep to flat large 1
    {{{10, 0,  2}, {10, 32, 76}}, 1, 1},
    {{{0,  10, 2}, {32, 10, 76}}, 1, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep to flat large 2
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep to flat large 3
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1}, // Steep to flat large 4
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
    {{{0,  0,  0}, {1,  1,  8}},  0, 1},
};

/* 0x0098D8D4 */
static constexpr UnkSupportsDescriptor Byte98D8D4[] = {
    {{{0, 0, 0}, {1, 1, 4}}, 0, 1},
    {{{0, 0, 0}, {1, 1, 4}}, 0, 1},
    {{{0, 0, 0}, {1, 1, 4}}, 0, 1},
    {{{0, 0, 0}, {1, 1, 4}}, 0, 1},
};

/* 0x0097B3C4 */
static constexpr uint16_t word_97B3C4[] = {
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
// clang-format on

static WoodenSupportSubType rotatedWoodenSupportSubTypes[6][NumOrthogonalDirections] = {
    {
        WoodenSupportSubType::NeSw,
        WoodenSupportSubType::NwSe,
        WoodenSupportSubType::NeSw,
        WoodenSupportSubType::NwSe,
    },
    {
        WoodenSupportSubType::NwSe,
        WoodenSupportSubType::NeSw,
        WoodenSupportSubType::NwSe,
        WoodenSupportSubType::NeSw,
    },
    {
        WoodenSupportSubType::Corner0,
        WoodenSupportSubType::Corner1,
        WoodenSupportSubType::Corner2,
        WoodenSupportSubType::Corner3,
    },
    {
        WoodenSupportSubType::Corner1,
        WoodenSupportSubType::Corner2,
        WoodenSupportSubType::Corner3,
        WoodenSupportSubType::Corner0,
    },
    {
        WoodenSupportSubType::Corner2,
        WoodenSupportSubType::Corner3,
        WoodenSupportSubType::Corner0,
        WoodenSupportSubType::Corner1,
    },
    {
        WoodenSupportSubType::Corner3,
        WoodenSupportSubType::Corner0,
        WoodenSupportSubType::Corner1,
        WoodenSupportSubType::Corner2,
    },
};

/**
 * Draw repeated supports for left over space
 *
 * @param supportType
 * @param imageTemplate
 * @param heightSteps
 * @param session
 * @param baseHeight
 * @param hasSupports
 */
static void WoodenABPaintRepeatedSupports(
    int32_t supportType, const ImageId& imageTemplate, int16_t heightSteps, PaintSession& session, uint16_t& baseHeight,
    bool& hasSupports)
{
    while (heightSteps > 0)
    {
        const bool isHalf = baseHeight & 0x10 || heightSteps == 1 || baseHeight + WATER_HEIGHT_STEP == session.WaterHeight;
        if (isHalf)
        {
            // Half support
            auto imageId = imageTemplate.WithIndex(WoodenSupportImageIds[supportType].half);
            uint8_t boundBoxHeight = (heightSteps == 1) ? 7 : 12;
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { 32, 32, boundBoxHeight });
            baseHeight += 16;
            heightSteps -= 1;
        }
        else
        {
            // Full support
            auto imageId = imageTemplate.WithIndex(WoodenSupportImageIds[supportType].full);
            uint8_t boundBoxHeight = (heightSteps == 2) ? 23 : 28;
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { 32, 32, boundBoxHeight });
            baseHeight += 32;
            heightSteps -= 2;
        }

        hasSupports = true;
    }
}

/**
 * Draw special pieces, e.g. curved supports.
 */
static bool WoodenABSupportPaintSetupPaintSpecial(
    PaintSession& session, int32_t supportType, int32_t special, const ImageId& imageTemplate, uint16_t baseHeight,
    bool hasSupports)
{
    uint16_t specialIndex = (special - 1) & 0xFFFF;

    const UnkSupportsDescriptor& supportsDesc = Byte97B23C[specialIndex];

    // TODO: Byte97B23C[specialIndex].var_7 is never 0
    if (WoodenCurveSupportImageIds[supportType] != nullptr && WoodenCurveSupportImageIds[supportType][specialIndex] != 0
        && supportsDesc.var_7 != 0)
    {
        hasSupports = true;
        auto imageId = imageTemplate.WithIndex(WoodenCurveSupportImageIds[supportType][specialIndex]);

        auto boundBox = supportsDesc.bounding_box;
        boundBox.offset.z += baseHeight;

        if (supportsDesc.var_6 == 0 || session.WoodenSupportsPrependTo == nullptr)
        {
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, boundBox);
        }
        else
        {
            auto* paintStruct = PaintAddImageAsOrphan(session, imageId, { 0, 0, baseHeight }, boundBox);
            if (paintStruct != nullptr)
            {
                session.WoodenSupportsPrependTo->Children = paintStruct;
            }
        }
    }

    return hasSupports;
}

/**
 * Adds paint structs for wooden supports.
 *  rct2: 0x006629BC
 * @param supportType (edi) Type and direction of supports.
 * @param special (ax) Used for curved supports.
 * @param height (dx) The height of the supports.
 * @param imageTemplate (ebp) The colour and palette flags for the support sprites.
 * @param[out] underground (Carry flag) true if underground.
 * @returns (al) true if any supports have been drawn, otherwise false.
 */
static bool WoodenASupportsPaintSetup(
    PaintSession& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate)
{
    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        return false;
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIDE_SUPPORTS)
    {
        if (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        {
            return false;
        }
        imageTemplate = ImageId().WithTransparency(FilterPaletteID::PaletteDarken1);
    }

    uint16_t baseHeight = Floor2(session.Support.height + 15, 16);
    int16_t supportLength = height - baseHeight;

    if (supportLength < 0)
    {
        return false;
    }

    int16_t heightSteps = supportLength / 16;

    bool hasSupports = false;
    bool drawFlatPiece = false;

    // Draw base support (usually shaped to the slope)
    auto slope = session.Support.slope;
    if (slope & SUPPORTS_SLOPE_5)
    {
        // Above scenery (just put a base piece above it)
        drawFlatPiece = true;
    }
    else if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
    {
        // Steep diagonal (place the correct shaped support for the slope)
        heightSteps -= 2;
        if (heightSteps < 0)
        {
            return false;
        }

        auto imageIndex = WoodenSupportImageIds[supportType].slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & TILE_ELEMENT_SURFACE_SLOPE_MASK]);

            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(4), { 0, 0, baseHeight + 16 },
                { { 0, 0, baseHeight + 16 + 2 }, { 32, 32, 11 } });

            hasSupports = true;
        }

        baseHeight += 32;
    }
    else if ((slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) != 0)
    {
        // 1 to 3 quarters up
        heightSteps--;
        if (heightSteps < 0)
        {
            return false;
        }

        auto imageIndex = WoodenSupportImageIds[supportType].slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & TILE_ELEMENT_SURFACE_SLOPE_MASK]);
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
            hasSupports = true;
        }

        baseHeight += 16;
    }

    // Draw flat base support
    if (drawFlatPiece)
    {
        auto imageId = imageTemplate.WithIndex(WoodenSupportImageIds[supportType].flat);
        PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight - 2 }, { 32, 32, 0 });
        hasSupports = true;
    }

    WoodenABPaintRepeatedSupports(supportType, imageTemplate, heightSteps, session, baseHeight, hasSupports);

    if (special != 0)
    {
        hasSupports = WoodenABSupportPaintSetupPaintSpecial(session, supportType, special, imageTemplate, baseHeight, false);
    }

    return hasSupports;
}

static int32_t GetSpecialOffsetForTransitionType(WoodenSupportTransitionType transitionType, Direction direction)
{
    if (transitionType == WoodenSupportTransitionType::None)
        return 0;

    // "Special" values are an offset into tables like Byte97B23C, plus 1.
    // Save for WoodenSupportTransitionType::LargeScenery, there are four entries (one per direction) for every
    // transition type. While these tables will have to be refactored in due course, we can only do so once all
    // drawing functions use WoodenSupportTransitionType instead of passing the "special" value directly.
    int32_t specialOffset = 0;
    if (transitionType < WoodenSupportTransitionType::Scenery)
        specialOffset = (EnumValue(transitionType) * NumOrthogonalDirections) + direction;
    else if (transitionType == WoodenSupportTransitionType::Scenery)
        specialOffset = (EnumValue(transitionType) * NumOrthogonalDirections);
    else
        specialOffset = (EnumValue(transitionType) * NumOrthogonalDirections) + direction - 3;

    return specialOffset + 1;
}

bool WoodenASupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType, Direction direction)
{
    assert(subType != WoodenSupportSubType::Null);
    int32_t oldSupportType = (EnumValue(supportType) * 6) + EnumValue(subType);
    int32_t special = GetSpecialOffsetForTransitionType(transitionType, direction);

    return WoodenASupportsPaintSetup(session, oldSupportType, special, height, imageTemplate);
}

bool WoodenASupportsPaintSetupRotated(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, Direction direction, int32_t height,
    ImageId imageTemplate, WoodenSupportTransitionType transitionType)
{
    assert(subType != WoodenSupportSubType::Null);
    subType = rotatedWoodenSupportSubTypes[EnumValue(subType)][direction];
    return WoodenASupportsPaintSetup(session, supportType, subType, height, imageTemplate, transitionType, direction);
}

/**
 * Wooden supports
 *  rct2: 0x00662D5C
 *
 * @param supportType (edi)
 * @param special (ax)
 * @param height (dx)
 * @param imageTemplate (ebp)
 * @param[out] underground (Carry Flag)
 *
 * @return (al) whether supports have been drawn
 */
static bool WoodenBSupportsPaintSetup(
    PaintSession& session, int32_t supportType, int32_t special, int32_t height, ImageId imageTemplate)
{
    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        return false;
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIDE_SUPPORTS)
    {
        if (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        {
            return false;
        }
        imageTemplate = ImageId().WithTransparency(FilterPaletteID::PaletteDarken1);
    }

    uint16_t baseHeight = Floor2(session.Support.height + 15, 16);
    int16_t supportLength = height - baseHeight;

    if (supportLength < 0)
    {
        return false;
    }

    int16_t heightSteps = supportLength / 16;

    bool hasSupports = false;
    bool drawFlatPiece = false;

    // Draw base support (usually shaped to the slope)
    auto slope = session.Support.slope;
    if (slope & SUPPORTS_SLOPE_5)
    {
        // Above scenery (just put a base piece above it)
        drawFlatPiece = true;
    }
    else if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
    {
        // Steep diagonal (place the correct shaped support for the slope)
        heightSteps -= 2;
        if (heightSteps < 0)
        {
            return false;
        }

        auto imageIndex = WoodenSupportImageIds[supportType].slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & TILE_ELEMENT_SURFACE_SLOPE_MASK]);

            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(4), { 0, 0, baseHeight + 16 },
                { { 0, 0, baseHeight + 16 + 2 }, { 32, 32, 3 } });

            hasSupports = true;
        }

        baseHeight += 32;
    }
    else if ((slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) != 0)
    {
        // 1 to 3 quarters up
        heightSteps--;
        if (heightSteps < 0)
        {
            return false;
        }

        auto imageIndex = WoodenSupportImageIds[supportType].slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & TILE_ELEMENT_SURFACE_SLOPE_MASK]);
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 3 } });
            hasSupports = true;
        }

        baseHeight += 16;
    }

    // Draw flat base support
    if (drawFlatPiece)
    {
        if (heightSteps > 0)
        {
            auto imageId = imageTemplate.WithIndex(WoodenSupportImageIds[supportType].flat);
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight - 2 }, { 32, 32, 0 });
            hasSupports = true;
        }
    }

    WoodenABPaintRepeatedSupports(supportType, imageTemplate, heightSteps, session, baseHeight, hasSupports);

    if (special != 0)
    {
        hasSupports = WoodenABSupportPaintSetupPaintSpecial(session, supportType, special, imageTemplate, baseHeight, false);
    }

    return hasSupports;
}

bool WoodenBSupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType, Direction direction)
{
    assert(subType != WoodenSupportSubType::Null);
    int32_t oldSupportType = (EnumValue(supportType) * 6) + EnumValue(subType);
    int32_t special = GetSpecialOffsetForTransitionType(transitionType, direction);

    return WoodenBSupportsPaintSetup(session, oldSupportType, special, height, imageTemplate);
}

bool WoodenBSupportsPaintSetupRotated(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, Direction direction, int32_t height,
    ImageId imageTemplate, WoodenSupportTransitionType transitionType)
{
    assert(subType != WoodenSupportSubType::Null);
    subType = rotatedWoodenSupportSubTypes[EnumValue(subType)][direction];
    return WoodenBSupportsPaintSetup(session, supportType, subType, height, imageTemplate, transitionType, direction);
}

/**
 *  rct2: 0x006A2ECC
 *
 * @param supportType (edi)
 * @param special (ax)
 * @param height (dx)
 * @param imageTemplate (ebp)
 * @param railingsDescriptor (0x00F3EF6C)
 * @param[out] underground (Carry Flag)
 *
 * @return Whether supports were drawn
 */
bool PathBoxSupportsPaintSetup(
    PaintSession& session, WoodenSupportSubType supportType, bool isSloped, Direction slopeRotation, int32_t height,
    ImageId imageTemplate, const FootpathPaintInfo& pathPaintInfo)
{
    auto supportOrientationOffset = (supportType == WoodenSupportSubType::NwSe) ? 24 : 0;

    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        return false;
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIDE_SUPPORTS)
    {
        if (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        {
            return false;
        }
        imageTemplate = ImageId().WithTransparency(FilterPaletteID::PaletteDarken1);
    }

    uint16_t baseHeight = Ceil2(session.Support.height, 16);
    int32_t supportLength = height - baseHeight;
    if (supportLength < 0)
    {
        return false;
    }

    bool hasSupports = false;

    int16_t heightSteps = supportLength / 16;

    if (session.Support.slope & 0x20)
    {
        // save dx2
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(pathPaintInfo.BridgeImageId + 48), { 0, 0, baseHeight - 2 }, { 32, 32, 0 });
        hasSupports = true;
    }
    else if (session.Support.slope & 0x10)
    {
        heightSteps -= 2;
        if (heightSteps < 0)
        {
            return false;
        }

        uint32_t imageId = supportOrientationOffset + word_97B3C4[session.Support.slope & TILE_ELEMENT_SURFACE_SLOPE_MASK]
            + pathPaintInfo.BridgeImageId;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageId), { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
        baseHeight += 16;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageId + 4), { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
        baseHeight += 16;

        hasSupports = true;
    }
    else if (session.Support.slope & 0x0F)
    {
        heightSteps -= 1;
        if (heightSteps < 0)
        {
            return false;
        }

        uint32_t ebx = supportOrientationOffset + word_97B3C4[session.Support.slope & TILE_ELEMENT_SURFACE_SLOPE_MASK]
            + pathPaintInfo.BridgeImageId;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(ebx), { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });

        hasSupports = true;
        baseHeight += 16;
    }

    while (heightSteps > 0)
    {
        if (baseHeight & 0x10 || heightSteps == 1 || baseHeight + WATER_HEIGHT_STEP == session.WaterHeight)
        {
            uint32_t imageId = supportOrientationOffset + pathPaintInfo.BridgeImageId + 23;

            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageId), { 0, 0, baseHeight }, { 32, 32, ((heightSteps == 1) ? 7 : 12) });
            heightSteps -= 1;
            baseHeight += 16;
            hasSupports = true;
        }
        else
        {
            uint32_t imageId = supportOrientationOffset + pathPaintInfo.BridgeImageId + 22;

            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageId), { 0, 0, baseHeight }, { 32, 32, ((heightSteps == 2) ? 23 : 28) });
            heightSteps -= 2;
            baseHeight += 32;
            hasSupports = true;
        }
    }

    if (isSloped)
    {
        ImageIndex imageIndex = pathPaintInfo.BridgeImageId + 55 + slopeRotation;

        const UnkSupportsDescriptor& supportsDesc = Byte98D8D4[slopeRotation];
        auto boundBox = supportsDesc.bounding_box;
        boundBox.offset.z += baseHeight;

        if (supportsDesc.var_6 == 0 || session.WoodenSupportsPrependTo == nullptr)
        {
            PaintAddImageAsParent(session, imageTemplate.WithIndex(imageIndex), { 0, 0, baseHeight }, boundBox);
            hasSupports = true;
        }
        else
        {
            PaintStruct* paintStruct = PaintAddImageAsOrphan(
                session, imageTemplate.WithIndex(imageIndex), { 0, 0, baseHeight }, boundBox);
            hasSupports = true;
            if (paintStruct != nullptr)
            {
                session.WoodenSupportsPrependTo->Children = paintStruct;
            }
        }
    }

    return hasSupports;
}
