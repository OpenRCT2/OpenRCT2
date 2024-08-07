/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WoodenSupports.h"

#include "../../interface/Viewport.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../../world/Surface.h"
#include "../../world/tile_element/Slope.h"
#include "../Boundbox.h"
#include "../Paint.SessionFlags.h"
#include "../Paint.h"

#include <cassert>

using namespace OpenRCT2;

constexpr auto kNumWoodenSupportTypes = 2;
constexpr auto kNumWoodenSupportSubTypes = 6;

struct SupportsIdDescriptor
{
    ImageIndex Full;
    ImageIndex Half;
    ImageIndex Flat;
    ImageIndex Slope;
};

/* 0x0097B1C4 */
static constexpr SupportsIdDescriptor WoodenSupportImageIds[kNumWoodenSupportTypes][kNumWoodenSupportSubTypes] = {
    // WoodenSupportType::Truss
    {
        { 3392, 3393, 3394, 3536 }, // WoodenSupportSubType::NeSw
        { 3390, 3391, 3394, 3514 }, // WoodenSupportSubType::NwSe
        { 3558, 3559, 3560, 3570 }, // WoodenSupportSubType::Corner0
        { 3561, 3562, 3563, 3592 }, // WoodenSupportSubType::Corner1
        { 3564, 3565, 3566, 3614 }, // WoodenSupportSubType::Corner2
        { 3567, 3568, 3569, 3636 }, // WoodenSupportSubType::Corner3
    },
    // WoodenSupportType::Mine
    {
        { 3677, 3678, 3680, 3739 }, // WoodenSupportSubType::NeSw
        { 3675, 3676, 3679, 3717 }, // WoodenSupportSubType::NwSe
        { 3761, 3762, 3763, 3773 }, // WoodenSupportSubType::Corner0
        { 3764, 3765, 3766, 3795 }, // WoodenSupportSubType::Corner1
        { 3767, 3768, 3769, 3817 }, // WoodenSupportSubType::Corner2
        { 3770, 3771, 3772, 3839 }, // WoodenSupportSubType::Corner3
    },
};

constexpr SupportsIdDescriptor GetWoodenSupportIds(WoodenSupportType supportType, WoodenSupportSubType subType)
{
    return WoodenSupportImageIds[EnumValue(supportType)][EnumValue(subType)];
}

using ImagesByTransitionTypeArray = std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 21>;

static constexpr ImagesByTransitionTypeArray WoodenCurveSupportImageIds0 = { {
    { 3465, 3466, 3467, 3468 }, // Flat to gentle
    { 3469, 3470, 3471, 3472 }, // Gentle to flat
    { 3473, 3474, 3475, 3476 }, // Gentle slope
    { 3477, 3478, 3479, 3480 }, // Gentle to steep
    { 3481, 3482, 3483, 3484 }, // Steep to gentle
    { 3485, 3486, 3487, 3488 }, // Steep slope
    { 3489, 3490, 3491, 3492 }, // Slope
    { 3493, 3494, 3495, 3496 }, // Flat to steep small
    { 3497, 3498, 3499, 3500 }, // Steep to flat small
    { 3501, 3502, 3503, 3504 }, // ?
    { 3505, 3506, 3507, 3508 }, // ?
    { 3509, 3510, 3511, 3512 }, // ?
    { 3513, 3513, 3513, 3513 }, // Large scenery
    { SPR_G2_SUPPORT_BEGIN, SPR_G2_SUPPORT_BEGIN + 1, SPR_G2_SUPPORT_BEGIN + 2,
      SPR_G2_SUPPORT_BEGIN + 3 }, // Flat to steep large 1
    { SPR_G2_SUPPORT_BEGIN + 4, SPR_G2_SUPPORT_BEGIN + 5, SPR_G2_SUPPORT_BEGIN + 6,
      SPR_G2_SUPPORT_BEGIN + 7 }, // Flat to steep large 2
    { SPR_G2_SUPPORT_BEGIN + 8, SPR_G2_SUPPORT_BEGIN + 9, SPR_G2_SUPPORT_BEGIN + 10,
      SPR_G2_SUPPORT_BEGIN + 11 }, // Flat to steep large 3
    { SPR_G2_SUPPORT_BEGIN + 12, SPR_G2_SUPPORT_BEGIN + 13, SPR_G2_SUPPORT_BEGIN + 14,
      SPR_G2_SUPPORT_BEGIN + 15 }, // Flat to steep large 4
    { SPR_G2_SUPPORT_BEGIN + 16, SPR_G2_SUPPORT_BEGIN + 17, SPR_G2_SUPPORT_BEGIN + 18,
      SPR_G2_SUPPORT_BEGIN + 19 }, // Steep to flat large 1
    { SPR_G2_SUPPORT_BEGIN + 20, SPR_G2_SUPPORT_BEGIN + 21, SPR_G2_SUPPORT_BEGIN + 22,
      SPR_G2_SUPPORT_BEGIN + 23 }, // Steep to flat large 2
    { SPR_G2_SUPPORT_BEGIN + 24, SPR_G2_SUPPORT_BEGIN + 25, SPR_G2_SUPPORT_BEGIN + 26,
      SPR_G2_SUPPORT_BEGIN + 27 }, // Steep to flat large 3
    { SPR_G2_SUPPORT_BEGIN + 28, SPR_G2_SUPPORT_BEGIN + 29, SPR_G2_SUPPORT_BEGIN + 30,
      SPR_G2_SUPPORT_BEGIN + 31 }, // Steep to flat large 4
} };

static constexpr ImagesByTransitionTypeArray WoodenCurveSupportImageIds1 = { {
    { 3681, 3682, 3683, 3684 }, // Flat to gentle
    { 3685, 3686, 3687, 3688 }, // Gentle to flat
    { 3689, 3690, 3691, 3692 }, // Gentle slope
    { 3693, 3694, 3695, 3696 }, // Gentle to steep
    { 3697, 3698, 3699, 3700 }, // Steep to gentle
    { 3701, 3702, 3703, 3704 }, // Steep slope
    { 3705, 3706, 3707, 3708 }, // Slope
    { 3709, 3710, 3711, 3712 }, // Flat to steep small
    { 3713, 3714, 3715, 3716 }, // Steep to flat small
    { 3717, 3718, 3719, 3720 }, // ?
    { 3721, 3722, 3723, 3724 }, // ?
    { 3725, 3726, 3727, 3728 }, // ?
    { 3729, 3729, 3729, 3729 }, // Large scenery
    { 0, 0, 0, 0 },             // Flat to steep large 1
    { 0, 0, 0, 0 },             // Flat to steep large 2
    { 0, 0, 0, 0 },             // Flat to steep large 3
    { 0, 0, 0, 0 },             // Flat to steep large 4
    { 0, 0, 0, 0 },             // Steep to flat large 1
    { 0, 0, 0, 0 },             // Steep to flat large 2
    { 0, 0, 0, 0 },             // Steep to flat large 3
    { 0, 0, 0, 0 },             // Steep to flat large 4
} };

// clang-format off
/* 0x0097B224 */
static constexpr const ImagesByTransitionTypeArray::const_pointer WoodenCurveSupportImageIds[kNumWoodenSupportTypes][kNumWoodenSupportSubTypes] = {
    // WoodenSupportType::Truss
    { 
        WoodenCurveSupportImageIds0.data(), // WoodenSupportSubType::NeSw
        WoodenCurveSupportImageIds0.data(), // WoodenSupportSubType::NwSe
        nullptr,                            // WoodenSupportSubType::Corner0
        nullptr,                            // WoodenSupportSubType::Corner1
        nullptr,                            // WoodenSupportSubType::Corner2
        nullptr,                            // WoodenSupportSubType::Corner3
    }, 
    // WoodenSupportType::Mine
    { 
        WoodenCurveSupportImageIds1.data(), // WoodenSupportSubType::NeSw
        WoodenCurveSupportImageIds1.data(), // WoodenSupportSubType::NwSe
        nullptr,                            // WoodenSupportSubType::Corner0
        nullptr,                            // WoodenSupportSubType::Corner1
        nullptr,                            // WoodenSupportSubType::Corner2
        nullptr,                            // WoodenSupportSubType::Corner3
    }, 
};

struct UnkSupportsDescriptor {
    BoundBoxXYZ BoundingBox;
    bool AsOrphan;
};

/* 0x0097B23C */
static constexpr UnkSupportsDescriptor SupportsDescriptors[] = {
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Flat to gentle
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Gentle to flat
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Gentle slope
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Gentle to steep
    {{{10, 0,  0}, {10, 32, 44}}, true},
    {{{0,  10, 0}, {32, 10, 44}}, true},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep to gentle
    {{{10, 0,  0}, {10, 32, 44}}, true},
    {{{0,  10, 0}, {32, 10, 44}}, true},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep slope
    {{{10, 0,  2}, {10, 32, 76}}, true},
    {{{0,  10, 2}, {32, 10, 76}}, true},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  4}},  false}, // Slope
    {{{0,  0,  0}, {1,  1,  4}},  false},
    {{{0,  0,  0}, {1,  1,  4}},  false},
    {{{0,  0,  0}, {1,  1,  4}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Flat to steep small
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep to flat small
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // ?
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // ?
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // ?
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{2,  2,  1}, {28, 28, 2}},  false}, // Large scenery
    {{{2,  2,  1}, {28, 28, 2}},  false},
    {{{2,  2,  1}, {28, 28, 2}},  false},
    {{{2,  2,  1}, {28, 28, 2}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Flat to steep large 1
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Flat to steep large 2
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Flat to steep large 3
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Flat to steep large 4
    {{{10, 0,  2}, {10, 32, 52}}, true},
    {{{0,  10, 2}, {32, 10, 52}}, true},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep to flat large 1
    {{{10, 0,  2}, {10, 32, 76}}, true},
    {{{0,  10, 2}, {32, 10, 76}}, true},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep to flat large 2
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep to flat large 3
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep to flat large 4
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
};

/* 0x0098D8D4 */
static constexpr UnkSupportsDescriptor Byte98D8D4[] = {
    {{{0, 0, 0}, {1, 1, 4}}, false},
    {{{0, 0, 0}, {1, 1, 4}}, false},
    {{{0, 0, 0}, {1, 1, 4}}, false},
    {{{0, 0, 0}, {1, 1, 4}}, false},
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

static WoodenSupportSubType rotatedWoodenSupportSubTypes[kNumWoodenSupportSubTypes][kNumOrthogonalDirections] = {
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
 */
static void PaintRepeatedWoodenSupports(
    const SupportsIdDescriptor supportImages, const ImageId& imageTemplate, int16_t heightSteps, PaintSession& session,
    uint16_t& baseHeight, bool& hasSupports)
{
    while (heightSteps > 0)
    {
        const bool isHalf = baseHeight & 0x10 || heightSteps == 1 || baseHeight + WATER_HEIGHT_STEP == session.WaterHeight;
        if (isHalf)
        {
            // Half support
            auto imageId = imageTemplate.WithIndex(supportImages.Half);
            uint8_t boundBoxHeight = (heightSteps == 1) ? 7 : 12;
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { 32, 32, boundBoxHeight });
            baseHeight += 16;
            heightSteps -= 1;
        }
        else
        {
            // Full support
            auto imageId = imageTemplate.WithIndex(supportImages.Full);
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
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType,
    WoodenSupportTransitionType transitionType, Direction direction, const ImageId& imageTemplate, uint16_t baseHeight)
{
    const uint16_t supportsDescriptorIndex = (EnumValue(transitionType) * kNumOrthogonalDirections) + direction;
    const UnkSupportsDescriptor& supportsDesc = SupportsDescriptors[supportsDescriptorIndex];
    const auto* imageIds = WoodenCurveSupportImageIds[EnumValue(supportType)][EnumValue(subType)];

    if (imageIds == nullptr || imageIds[EnumValue(transitionType)][direction] == 0)
        return false;

    auto imageId = imageTemplate.WithIndex(imageIds[EnumValue(transitionType)][direction]);

    auto boundBox = supportsDesc.BoundingBox;
    boundBox.offset.z += baseHeight;

    if (supportsDesc.AsOrphan == false || session.WoodenSupportsPrependTo == nullptr)
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

    return true;
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
bool WoodenASupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType, Direction direction)
{
    assert(subType != WoodenSupportSubType::Null);

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

    auto supportIds = GetWoodenSupportIds(supportType, subType);

    // Draw base support (usually shaped to the slope)
    auto slope = session.Support.slope;
    if (slope & kTileSlopeAboveTrackOrScenery)
    {
        // Above scenery (just put a base piece above it)
        drawFlatPiece = true;
    }
    else if (slope & kTileSlopeDiagonalFlag)
    {
        // Steep diagonal (place the correct shaped support for the slope)
        heightSteps -= 2;
        if (heightSteps < 0)
        {
            return false;
        }

        auto imageIndex = supportIds.Slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & kTileSlopeMask]);

            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(4), { 0, 0, baseHeight + 16 },
                { { 0, 0, baseHeight + 16 + 2 }, { 32, 32, 11 } });

            hasSupports = true;
        }

        baseHeight += 32;
    }
    else if ((slope & kTileSlopeRaisedCornersMask) != 0)
    {
        // 1 to 3 quarters up
        heightSteps--;
        if (heightSteps < 0)
        {
            return false;
        }

        auto imageIndex = supportIds.Slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & kTileSlopeMask]);
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
            hasSupports = true;
        }

        baseHeight += 16;
    }

    // Draw flat base support
    if (drawFlatPiece)
    {
        auto imageId = imageTemplate.WithIndex(supportIds.Flat);
        PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight - 2 }, { 32, 32, 0 });
        hasSupports = true;
    }

    PaintRepeatedWoodenSupports(supportIds, imageTemplate, heightSteps, session, baseHeight, hasSupports);

    if (transitionType != WoodenSupportTransitionType::None)
    {
        hasSupports = WoodenABSupportPaintSetupPaintSpecial(
            session, supportType, subType, transitionType, direction, imageTemplate, baseHeight);
    }

    return hasSupports;
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
bool WoodenBSupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType, Direction direction)
{
    assert(subType != WoodenSupportSubType::Null);

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

    auto supportIds = GetWoodenSupportIds(supportType, subType);

    int16_t heightSteps = supportLength / 16;

    bool hasSupports = false;
    bool drawFlatPiece = false;

    // Draw base support (usually shaped to the slope)
    auto slope = session.Support.slope;
    if (slope & kTileSlopeAboveTrackOrScenery)
    {
        // Above scenery (just put a base piece above it)
        drawFlatPiece = true;
    }
    else if (slope & kTileSlopeDiagonalFlag)
    {
        // Steep diagonal (place the correct shaped support for the slope)
        heightSteps -= 2;
        if (heightSteps < 0)
        {
            return false;
        }

        auto imageIndex = supportIds.Slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & kTileSlopeMask]);

            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(4), { 0, 0, baseHeight + 16 },
                { { 0, 0, baseHeight + 16 + 2 }, { 32, 32, 3 } });

            hasSupports = true;
        }

        baseHeight += 32;
    }
    else if ((slope & kTileSlopeRaisedCornersMask) != 0)
    {
        // 1 to 3 quarters up
        heightSteps--;
        if (heightSteps < 0)
        {
            return false;
        }

        auto imageIndex = supportIds.Slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & kTileSlopeMask]);
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
            auto imageId = imageTemplate.WithIndex(supportIds.Flat);
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight - 2 }, { 32, 32, 0 });
            hasSupports = true;
        }
    }

    PaintRepeatedWoodenSupports(supportIds, imageTemplate, heightSteps, session, baseHeight, hasSupports);

    if (transitionType != WoodenSupportTransitionType::None)
    {
        hasSupports = WoodenABSupportPaintSetupPaintSpecial(
            session, supportType, subType, transitionType, direction, imageTemplate, baseHeight);
    }

    return hasSupports;
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

    if (session.Support.slope & kTileSlopeAboveTrackOrScenery)
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

        uint32_t imageId = supportOrientationOffset + word_97B3C4[session.Support.slope & kTileSlopeMask]
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

        uint32_t ebx = supportOrientationOffset + word_97B3C4[session.Support.slope & kTileSlopeMask]
            + pathPaintInfo.BridgeImageId;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(ebx), { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, 11 } });

        hasSupports = true;
        baseHeight += 16;
    }

    SupportsIdDescriptor supportImages = {
        .Full = pathPaintInfo.BridgeImageId + 22 + supportOrientationOffset,
        .Half = pathPaintInfo.BridgeImageId + 23 + supportOrientationOffset,
        .Flat = pathPaintInfo.BridgeImageId + 48,
        .Slope = pathPaintInfo.BridgeImageId + supportOrientationOffset,
    };
    PaintRepeatedWoodenSupports(supportImages, imageTemplate, heightSteps, session, baseHeight, hasSupports);

    if (isSloped)
    {
        ImageIndex imageIndex = pathPaintInfo.BridgeImageId + 55 + slopeRotation;

        const UnkSupportsDescriptor& supportsDesc = Byte98D8D4[slopeRotation];
        auto boundBox = supportsDesc.BoundingBox;
        boundBox.offset.z += baseHeight;

        if (supportsDesc.AsOrphan == false || session.WoodenSupportsPrependTo == nullptr)
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
