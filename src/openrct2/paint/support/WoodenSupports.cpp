/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WoodenSupports.h"

#include "../../interface/Viewport.h"
#include "../../ride/TrackData.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../../world/tile_element/Slope.h"
#include "../Boundbox.h"
#include "../Paint.SessionFlags.h"
#include "../Paint.h"

#include <cassert>

using namespace OpenRCT2;
using namespace OpenRCT2::Numerics;

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
    { SPR_G2_SUPPORT_WOODEN_TRUSS, SPR_G2_SUPPORT_WOODEN_TRUSS + 1, SPR_G2_SUPPORT_WOODEN_TRUSS + 2,
      SPR_G2_SUPPORT_WOODEN_TRUSS + 3 }, // Flat to steep large 1
    { SPR_G2_SUPPORT_WOODEN_TRUSS + 4, SPR_G2_SUPPORT_WOODEN_TRUSS + 5, SPR_G2_SUPPORT_WOODEN_TRUSS + 6,
      SPR_G2_SUPPORT_WOODEN_TRUSS + 7 }, // Flat to steep large 2
    { SPR_G2_SUPPORT_WOODEN_TRUSS + 8, SPR_G2_SUPPORT_WOODEN_TRUSS + 9, SPR_G2_SUPPORT_WOODEN_TRUSS + 10,
      SPR_G2_SUPPORT_WOODEN_TRUSS + 11 }, // Flat to steep large 3
    { SPR_G2_SUPPORT_WOODEN_TRUSS + 12, SPR_G2_SUPPORT_WOODEN_TRUSS + 13, SPR_G2_SUPPORT_WOODEN_TRUSS + 14,
      SPR_G2_SUPPORT_WOODEN_TRUSS + 15 }, // Flat to steep large 4
    { SPR_G2_SUPPORT_WOODEN_TRUSS + 16, SPR_G2_SUPPORT_WOODEN_TRUSS + 17, SPR_G2_SUPPORT_WOODEN_TRUSS + 18,
      SPR_G2_SUPPORT_WOODEN_TRUSS + 19 }, // Steep to flat large 1
    { SPR_G2_SUPPORT_WOODEN_TRUSS + 20, SPR_G2_SUPPORT_WOODEN_TRUSS + 21, SPR_G2_SUPPORT_WOODEN_TRUSS + 22,
      SPR_G2_SUPPORT_WOODEN_TRUSS + 23 }, // Steep to flat large 2
    { SPR_G2_SUPPORT_WOODEN_TRUSS + 24, SPR_G2_SUPPORT_WOODEN_TRUSS + 25, SPR_G2_SUPPORT_WOODEN_TRUSS + 26,
      SPR_G2_SUPPORT_WOODEN_TRUSS + 27 }, // Steep to flat large 3
    { SPR_G2_SUPPORT_WOODEN_TRUSS + 28, SPR_G2_SUPPORT_WOODEN_TRUSS + 29, SPR_G2_SUPPORT_WOODEN_TRUSS + 30,
      SPR_G2_SUPPORT_WOODEN_TRUSS + 31 }, // Steep to flat large 4
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
    { SPR_G2_SUPPORT_WOODEN_MINE, SPR_G2_SUPPORT_WOODEN_MINE + 1, SPR_G2_SUPPORT_WOODEN_MINE + 2,
      SPR_G2_SUPPORT_WOODEN_MINE + 3 }, // Flat to steep large 1
    { SPR_G2_SUPPORT_WOODEN_MINE + 4, SPR_G2_SUPPORT_WOODEN_MINE + 5, SPR_G2_SUPPORT_WOODEN_MINE + 6,
      SPR_G2_SUPPORT_WOODEN_MINE + 7 }, // Flat to steep large 2
    { SPR_G2_SUPPORT_WOODEN_MINE + 8, SPR_G2_SUPPORT_WOODEN_MINE + 9, SPR_G2_SUPPORT_WOODEN_MINE + 10,
      SPR_G2_SUPPORT_WOODEN_MINE + 11 }, // Flat to steep large 3
    { SPR_G2_SUPPORT_WOODEN_MINE + 12, SPR_G2_SUPPORT_WOODEN_MINE + 13, SPR_G2_SUPPORT_WOODEN_MINE + 14,
      SPR_G2_SUPPORT_WOODEN_MINE + 15 }, // Flat to steep large 4
    { SPR_G2_SUPPORT_WOODEN_MINE + 16, SPR_G2_SUPPORT_WOODEN_MINE + 17, SPR_G2_SUPPORT_WOODEN_MINE + 18,
      SPR_G2_SUPPORT_WOODEN_MINE + 19 }, // Steep to flat large 1
    { SPR_G2_SUPPORT_WOODEN_MINE + 20, SPR_G2_SUPPORT_WOODEN_MINE + 21, SPR_G2_SUPPORT_WOODEN_MINE + 22,
      SPR_G2_SUPPORT_WOODEN_MINE + 23 }, // Steep to flat large 2
    { SPR_G2_SUPPORT_WOODEN_MINE + 24, SPR_G2_SUPPORT_WOODEN_MINE + 25, SPR_G2_SUPPORT_WOODEN_MINE + 26,
      SPR_G2_SUPPORT_WOODEN_MINE + 27 }, // Steep to flat large 3
    { SPR_G2_SUPPORT_WOODEN_MINE + 28, SPR_G2_SUPPORT_WOODEN_MINE + 29, SPR_G2_SUPPORT_WOODEN_MINE + 30,
      SPR_G2_SUPPORT_WOODEN_MINE + 31 }, // Steep to flat large 4
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

struct SlopedSupportsDescriptor {
    BoundBoxXYZ BoundingBox;
    bool AsOrphan;
};

/* 0x0097B23C */
static constexpr SlopedSupportsDescriptor SupportsDescriptors[] = {
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
static constexpr SlopedSupportsDescriptor kSlopedPathSupportsDescriptor = {{{0, 0, 0}, {1, 1, 4}}, false};

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
        const bool isHalf = baseHeight & 0x10 || heightSteps == 1 || baseHeight + kWaterHeightStep == session.WaterHeight;
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
static void PaintSlopeTransitions(
    const SlopedSupportsDescriptor& supportsDesc, ImageIndex imageIndex, PaintSession& session, const ImageId& imageTemplate,
    uint16_t baseHeight)
{
    auto imageId = imageTemplate.WithIndex(imageIndex);

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
}

static bool WoodenABPaintSlopeTransitions(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType,
    WoodenSupportTransitionType transitionType, Direction direction, const ImageId& imageTemplate, uint16_t baseHeight)
{
    if (EnumValue(transitionType) >= 21)
    {
        transitionType = static_cast<WoodenSupportTransitionType>(EnumValue(transitionType) - 21);
        direction = DirectionReverse(direction);
    }

    const uint16_t supportsDescriptorIndex = (EnumValue(transitionType) * kNumOrthogonalDirections) + direction;
    const SlopedSupportsDescriptor& supportsDesc = SupportsDescriptors[supportsDescriptorIndex];
    const auto* imageIds = WoodenCurveSupportImageIds[EnumValue(supportType)][EnumValue(subType)];

    if (imageIds == nullptr || imageIds[EnumValue(transitionType)][direction] == 0)
        return false;

    PaintSlopeTransitions(supportsDesc, imageIds[EnumValue(transitionType)][direction], session, imageTemplate, baseHeight);

    return true;
}

template<uint8_t zOffset, bool doHeightStepsCheck>
static inline bool WoodenSupportsPaintSetupCommon(
    PaintSession& session, const SupportsIdDescriptor& supportImages, int32_t height, ImageId& imageTemplate, bool& hasSupports,
    uint16_t& baseHeight)
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

    baseHeight = ceil2(session.Support.height, 16);
    int16_t supportLength = height - baseHeight;

    if (supportLength < 0)
    {
        return false;
    }

    int16_t heightSteps = supportLength / 16;

    hasSupports = false;
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

        ImageIndex imageIndex = supportImages.Slope;
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
                { { 0, 0, baseHeight + 16 + 2 }, { 32, 32, zOffset } });

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

        ImageIndex imageIndex = supportImages.Slope;
        if (imageIndex == 0)
        {
            drawFlatPiece = true;
        }
        else
        {
            auto imageId = imageTemplate.WithIndex(imageIndex + word_97B3C4[slope & kTileSlopeMask]);
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight }, { { 0, 0, baseHeight + 2 }, { 32, 32, zOffset } });
            hasSupports = true;
        }

        baseHeight += 16;
    }

    // Draw flat base support
    if (drawFlatPiece)
    {
        bool shouldDraw = true;
        if constexpr (doHeightStepsCheck)
        {
            shouldDraw = heightSteps > 0;
        }

        if (shouldDraw)
        {
            auto imageId = imageTemplate.WithIndex(supportImages.Flat);
            PaintAddImageAsParent(session, imageId, { 0, 0, baseHeight - 2 }, { 32, 32, 0 });
            hasSupports = true;
        }
    }

    PaintRepeatedWoodenSupports(supportImages, imageTemplate, heightSteps, session, baseHeight, hasSupports);
    return true;
}

template<uint8_t zOffset, bool doHeightStepsCheck>
inline bool WoodenABSupportsPaintSetupCommon(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType, Direction direction)
{
    assert(subType != WoodenSupportSubType::Null);

    uint16_t baseHeight = 0;
    bool hasSupports = false;
    auto supportIds = GetWoodenSupportIds(supportType, subType);

    if (!WoodenSupportsPaintSetupCommon<zOffset, doHeightStepsCheck>(
            session, supportIds, height, imageTemplate, hasSupports, baseHeight))
    {
        return false;
    }

    if (transitionType != WoodenSupportTransitionType::None)
    {
        hasSupports = WoodenABPaintSlopeTransitions(
            session, supportType, subType, transitionType, direction, imageTemplate, baseHeight);
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
bool WoodenASupportsPaintSetup(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType, int32_t height, ImageId imageTemplate,
    WoodenSupportTransitionType transitionType, Direction direction)
{
    return WoodenABSupportsPaintSetupCommon<11, false>(
        session, supportType, subType, height, imageTemplate, transitionType, direction);
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
    return WoodenABSupportsPaintSetupCommon<3, true>(
        session, supportType, subType, height, imageTemplate, transitionType, direction);
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

    uint16_t baseHeight = 0;
    bool hasSupports = false;
    SupportsIdDescriptor supportIds = {
        .Full = pathPaintInfo.BridgeImageId + 22 + supportOrientationOffset,
        .Half = pathPaintInfo.BridgeImageId + 23 + supportOrientationOffset,
        .Flat = pathPaintInfo.BridgeImageId + 48,
        .Slope = pathPaintInfo.BridgeImageId + supportOrientationOffset,
    };

    if (!WoodenSupportsPaintSetupCommon<11, false>(session, supportIds, height, imageTemplate, hasSupports, baseHeight))
    {
        return false;
    }

    if (isSloped)
    {
        ImageIndex imageIndex = pathPaintInfo.BridgeImageId + 55 + slopeRotation;

        PaintSlopeTransitions(kSlopedPathSupportsDescriptor, imageIndex, session, imageTemplate, baseHeight);
        hasSupports = true;
    }

    return hasSupports;
}

bool DrawSupportForSequenceA(
    PaintSession& session, WoodenSupportType supportType, OpenRCT2::TrackElemType trackType, uint8_t sequence,
    Direction direction, int32_t height, ImageId imageTemplate)
{
    const auto& ted = OpenRCT2::TrackMetaData::GetTrackElementDescriptor(trackType);
    const auto& desc = ted.sequences[sequence].woodenSupports;

    if (desc.subType == WoodenSupportSubType::Null)
        return false;

    return WoodenASupportsPaintSetupRotated(
        session, supportType, desc.subType, direction, height, imageTemplate, desc.transitionType);
}

bool DrawSupportForSequenceB(
    PaintSession& session, WoodenSupportType supportType, OpenRCT2::TrackElemType trackType, uint8_t sequence,
    Direction direction, int32_t height, ImageId imageTemplate)
{
    const auto& ted = OpenRCT2::TrackMetaData::GetTrackElementDescriptor(trackType);
    const auto& desc = ted.sequences[sequence].woodenSupports;

    if (desc.subType == WoodenSupportSubType::Null)
        return false;

    return WoodenBSupportsPaintSetupRotated(
        session, supportType, desc.subType, direction, height, imageTemplate, desc.transitionType);
}
