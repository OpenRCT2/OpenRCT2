/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WoodenSupports.h"

#include "../../SpriteIds.h"
#include "../../interface/Viewport.h"
#include "../../ride/TrackData.h"
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

using ImagesByTransitionTypeArray = std::array<
    std::array<ImageIndex, kNumOrthogonalDirections>, kWoodenSupportTransitionTypeUninvertedCount>;

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
    { SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 1,
      SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 2, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 3 }, // Flat to steep large 1
    { SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 4, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 5,
      SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 6, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 7 }, // Flat to steep large 2
    { SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 8, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 9,
      SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 10, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 11 }, // Flat to steep large 3
    { SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 12, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 13,
      SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 14, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 15 }, // Flat to steep large 4
    { SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 16, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 17,
      SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 18, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 19 }, // Steep to flat large 1
    { SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 20, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 21,
      SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 22, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 23 }, // Steep to flat large 2
    { SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 24, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 25,
      SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 26, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 27 }, // Steep to flat large 3
    { SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 28, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 29,
      SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 30, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 31 }, // Steep to flat large 4
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined },         // None steep adjacent
} };
static_assert(std::size(WoodenCurveSupportImageIds0) == kWoodenSupportTransitionTypeUninvertedCount);

static constexpr ImagesByTransitionTypeArray WoodenCurveSupportFrontImageIds0 = { {
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Flat to gentle
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Gentle to flat
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Gentle slope
    { kImageIndexUndefined, SPR_G2_SUPPORT_WOODEN_TRUSS_3478_FRONT, SPR_G2_SUPPORT_WOODEN_TRUSS_3479_FRONT,
      kImageIndexUndefined }, // Gentle to steep
    { kImageIndexUndefined, SPR_G2_SUPPORT_WOODEN_TRUSS_3482_FRONT, SPR_G2_SUPPORT_WOODEN_TRUSS_3483_FRONT,
      kImageIndexUndefined },                                                                   // Steep to gentle
    { kImageIndexUndefined, 3486, 3487, kImageIndexUndefined },                                 // Steep slope
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Slope
    { kImageIndexUndefined, SPR_G2_SUPPORT_WOODEN_TRUSS_3494_FRONT, SPR_G2_SUPPORT_WOODEN_TRUSS_3495_FRONT,
      kImageIndexUndefined }, // Flat to steep small
    { kImageIndexUndefined, SPR_G2_SUPPORT_WOODEN_TRUSS_3498_FRONT, SPR_G2_SUPPORT_WOODEN_TRUSS_3499_FRONT,
      kImageIndexUndefined },                                                                   // Steep to flat small
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // ?
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // ?
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // ?
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Large scenery
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Flat to steep large 1
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Flat to steep large 2
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Flat to steep large 3
    { kImageIndexUndefined, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 13, SPR_G2_SUPPORT_WOODEN_TRUSS_FLAT_TO_STEEP + 14,
      kImageIndexUndefined }, // Flat to steep large 4
    { kImageIndexUndefined, SPR_G2_SUPPORT_WOODEN_TRUSS_STEEP_TO_FLAT_1_2_FRONT,
      SPR_G2_SUPPORT_WOODEN_TRUSS_STEEP_TO_FLAT_1_3_FRONT, kImageIndexUndefined }, // Steep to flat large 1
    { kImageIndexUndefined, SPR_G2_SUPPORT_WOODEN_TRUSS_STEEP_TO_FLAT_2_2_FRONT,
      SPR_G2_SUPPORT_WOODEN_TRUSS_STEEP_TO_FLAT_2_3_FRONT, kImageIndexUndefined },              // Steep to flat large 2
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Steep to flat large 3
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Steep to flat large 4
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // None steep adjacent
} };
static_assert(std::size(WoodenCurveSupportFrontImageIds0) == kWoodenSupportTransitionTypeUninvertedCount);

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
    { SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 1,
      SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 2, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 3 }, // Flat to steep large 1
    { SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 4, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 5,
      SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 6, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 7 }, // Flat to steep large 2
    { SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 8, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 9,
      SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 10, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 11 }, // Flat to steep large 3
    { SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 12, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 13,
      SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 14, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 15 }, // Flat to steep large 4
    { SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 16, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 17,
      SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 18, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 19 }, // Steep to flat large 1
    { SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 20, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 21,
      SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 22, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 23 }, // Steep to flat large 2
    { SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 24, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 25,
      SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 26, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 27 }, // Steep to flat large 3
    { SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 28, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 29,
      SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 30, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 31 }, // Steep to flat large 4
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined },       // None steep adjacent
} };
static_assert(std::size(WoodenCurveSupportImageIds1) == kWoodenSupportTransitionTypeUninvertedCount);

static constexpr ImagesByTransitionTypeArray WoodenCurveSupportFrontImageIds1 = { {
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Flat to gentle
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Gentle to flat
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Gentle slope
    { kImageIndexUndefined, SPR_G2_SUPPORT_WOODEN_MINE_3694_FRONT, SPR_G2_SUPPORT_WOODEN_MINE_3695_FRONT,
      kImageIndexUndefined }, // Gentle to steep
    { kImageIndexUndefined, SPR_G2_SUPPORT_WOODEN_MINE_3698_FRONT, SPR_G2_SUPPORT_WOODEN_MINE_3699_FRONT,
      kImageIndexUndefined },                                                                   // Steep to gentle
    { kImageIndexUndefined, 3702, 3703, kImageIndexUndefined },                                 // Steep slope
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Slope
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Flat to steep small
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Steep to flat small
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // ?
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // ?
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // ?
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Large scenery
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Flat to steep large 1
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Flat to steep large 2
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Flat to steep large 3
    { kImageIndexUndefined, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 13, SPR_G2_SUPPORT_WOODEN_MINE_FLAT_TO_STEEP + 14,
      kImageIndexUndefined }, // Flat to steep large 4
    { kImageIndexUndefined, SPR_G2_SUPPORT_WOODEN_MINE_STEEP_TO_FLAT_1_2_FRONT,
      SPR_G2_SUPPORT_WOODEN_MINE_STEEP_TO_FLAT_1_3_FRONT, kImageIndexUndefined }, // Steep to flat large 1
    { kImageIndexUndefined, SPR_G2_SUPPORT_WOODEN_MINE_STEEP_TO_FLAT_2_2_FRONT,
      SPR_G2_SUPPORT_WOODEN_MINE_STEEP_TO_FLAT_2_3_FRONT, kImageIndexUndefined },               // Steep to flat large 2
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Steep to flat large 3
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // Steep to flat large 4
    { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined }, // None steep adjacent
} };
static_assert(std::size(WoodenCurveSupportFrontImageIds1) == kWoodenSupportTransitionTypeUninvertedCount);

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

// clang-format off
static constexpr const ImagesByTransitionTypeArray::const_pointer WoodenCurveSupportFrontImageIds[kNumWoodenSupportTypes][kNumWoodenSupportSubTypes] = {
    // WoodenSupportType::Truss
    {
        WoodenCurveSupportFrontImageIds0.data(), // WoodenSupportSubType::NeSw
        WoodenCurveSupportFrontImageIds0.data(), // WoodenSupportSubType::NwSe
        nullptr,                                 // WoodenSupportSubType::Corner0
        nullptr,                                 // WoodenSupportSubType::Corner1
        nullptr,                                 // WoodenSupportSubType::Corner2
        nullptr,                                 // WoodenSupportSubType::Corner3
    },
    // WoodenSupportType::Mine
    {
        WoodenCurveSupportFrontImageIds1.data(), // WoodenSupportSubType::NeSw
        WoodenCurveSupportFrontImageIds1.data(), // WoodenSupportSubType::NwSe
        nullptr,                                 // WoodenSupportSubType::Corner0
        nullptr,                                 // WoodenSupportSubType::Corner1
        nullptr,                                 // WoodenSupportSubType::Corner2
        nullptr,                                 // WoodenSupportSubType::Corner3
    },
};

struct SlopedSupportsDescriptor {
    BoundBoxXYZ BoundingBox;
    bool hasFrontSprite;
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
    {{{29, 0,  0}, {1, 28, 64}},   true},
    {{{0, 29,  0}, {28, 1, 64}},   true},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep to gentle
    {{{29, 0,  0}, {1, 28, 64}},   true},
    {{{0, 29,  0}, {28, 1, 64}},   true},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep slope
    {{{29, 0,  0}, {1, 28, 96}},   true},
    {{{0, 29,  0}, {28, 1, 96}},   true},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  4}},  false}, // Slope
    {{{0,  0,  0}, {1,  1,  4}},  false},
    {{{0,  0,  0}, {1,  1,  4}},  false},
    {{{0,  0,  0}, {1,  1,  4}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Flat to steep small
    {{{29, 0,  0}, {1, 28, 56}},   true},
    {{{0, 29,  0}, {28, 1, 56}},   true},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep to flat small
    {{{29, 0,  0}, {1, 28, 56}},   true},
    {{{0, 29,  0}, {28, 1, 56}},   true},
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
    {{{29, 0,  0}, {1, 28, 80}},   true},
    {{{0, 29,  0}, {28, 1, 80}},   true},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep to flat large 1
    {{{29, 0,  0}, {1, 28, 80}},   true},
    {{{0, 29,  0}, {28, 1, 80}},   true},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep to flat large 2
    {{{29, 0,  0}, {1, 28, 72}},   true},
    {{{0, 29,  0}, {28, 1, 72}},   true},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep to flat large 3
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // Steep to flat large 4
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false},
    {{{0,  0,  0}, {1,  1,  8}},  false}, // None steep adjacent
    {{{0,  0,  0}, {1,  1,  8}},  true},
    {{{0,  0,  0}, {1,  1,  8}},  true},
    {{{0,  0,  0}, {1,  1,  8}},  false},    
};
static_assert(std::size(SupportsDescriptors) == kWoodenSupportTransitionTypeUninvertedCount * 4);

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
    uint16_t& baseHeight, bool& hasSupports, const int32_t boundBoxOffset, const int32_t boundBoxLength)
{
    while (heightSteps > 0)
    {
        const bool isHalf = baseHeight & 0x10 || heightSteps == 1 || baseHeight + kWaterHeightStep == session.WaterHeight;
        if (isHalf)
        {
            // Half support
            auto imageId = imageTemplate.WithIndex(supportImages.Half);
            uint8_t boundBoxHeight = (heightSteps == 1) ? 7 : 12;
            PaintAddImageAsParent(
                session, imageId, { 0, 0, baseHeight },
                { { boundBoxOffset, boundBoxOffset, baseHeight }, { boundBoxLength, boundBoxLength, boundBoxHeight } });
            baseHeight += 16;
            heightSteps -= 1;
        }
        else
        {
            // Full support
            auto imageId = imageTemplate.WithIndex(supportImages.Full);
            uint8_t boundBoxHeight = (heightSteps == 2) ? 23 : 28;
            PaintAddImageAsParent(
                session, imageId, { 0, 0, baseHeight },
                { { boundBoxOffset, boundBoxOffset, baseHeight }, { boundBoxLength, boundBoxLength, boundBoxHeight } });
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
    PaintSession& session, const ImageIndex imageIndex, const ImageId& imageTemplate, const uint16_t baseHeight)
{
    PaintAddImageAsParent(
        session, imageTemplate.WithIndex(imageIndex), { 0, 0, baseHeight }, { { 0, 0, baseHeight }, { 1, 1, 1 } });
}

static void PaintSlopeTransitionsFront(
    PaintSession& session, const SlopedSupportsDescriptor& supportsDesc, const ImageIndex imageIndex,
    const ImageIndex imageIndexFront, const ImageId& imageTemplate, const uint16_t baseHeight)
{
    PaintAddImageAsParent(
        session, imageTemplate.WithIndex(imageIndex), { 0, 0, baseHeight }, { { 1, 1, baseHeight }, { 1, 1, 1 } });

    auto boundingBox = supportsDesc.BoundingBox;
    boundingBox.offset.z += baseHeight;
    PaintAddImageAsParent(session, imageTemplate.WithIndex(imageIndexFront), { 0, 0, baseHeight }, boundingBox);
}

static bool WoodenABPaintSlopeTransitions(
    PaintSession& session, WoodenSupportType supportType, WoodenSupportSubType subType,
    WoodenSupportTransitionType transitionType, const SlopedSupportsDescriptor& supportsDesc, Direction direction,
    const ImageId& imageTemplate, uint16_t baseHeight)
{
    const auto* imageIds = WoodenCurveSupportImageIds[EnumValue(supportType)][EnumValue(subType)];

    if (imageIds == nullptr || imageIds[EnumValue(transitionType)][direction] == 0)
        return false;

    if (!supportsDesc.hasFrontSprite)
    {
        PaintSlopeTransitions(session, imageIds[EnumValue(transitionType)][direction], imageTemplate, baseHeight);
    }
    else
    {
        const auto* frontImageIds = WoodenCurveSupportFrontImageIds[EnumValue(supportType)][EnumValue(subType)];
        PaintSlopeTransitionsFront(
            session, supportsDesc, imageIds[EnumValue(transitionType)][direction],
            frontImageIds[EnumValue(transitionType)][direction], imageTemplate, baseHeight);
    }

    return true;
}

template<uint8_t zOffset, bool doHeightStepsCheck>
static inline bool WoodenSupportsPaintSetupCommon(
    PaintSession& session, const SupportsIdDescriptor& supportImages, int32_t height, ImageId& imageTemplate, bool& hasSupports,
    uint16_t& baseHeight, const bool rearFacingSteep)
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

    const int32_t boundBoxOffset = rearFacingSteep ? 1 : 0;
    const int32_t boundBoxLength = rearFacingSteep ? 30 : 32;

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

            PaintAddImageAsParent(
                session, imageId, { 0, 0, baseHeight },
                { { boundBoxOffset, boundBoxOffset, baseHeight + 2 }, { boundBoxLength, boundBoxLength, 11 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(4), { 0, 0, baseHeight + 16 },
                { { boundBoxOffset, boundBoxOffset, baseHeight + 16 + 2 }, { boundBoxLength, boundBoxLength, zOffset } });

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
            PaintAddImageAsParent(
                session, imageId, { 0, 0, baseHeight },
                { { boundBoxOffset, boundBoxOffset, baseHeight + 2 }, { boundBoxLength, boundBoxLength, zOffset } });
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
            PaintAddImageAsParent(
                session, imageId, { 0, 0, baseHeight - 2 },
                { { boundBoxOffset, boundBoxOffset, baseHeight - 2 }, { boundBoxLength, boundBoxLength, 0 } });
            hasSupports = true;
        }
    }

    PaintRepeatedWoodenSupports(
        supportImages, imageTemplate, heightSteps, session, baseHeight, hasSupports, boundBoxOffset, boundBoxLength);
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

    size_t supportsDescriptorIndex = 0;
    bool rearFacingSteep = false;
    if (transitionType != WoodenSupportTransitionType::None)
    {
        if (EnumValue(transitionType) >= kWoodenSupportTransitionTypeUninvertedCount)
        {
            transitionType = static_cast<WoodenSupportTransitionType>(
                EnumValue(transitionType) - kWoodenSupportTransitionTypeUninvertedCount);
            direction = DirectionReverse(direction);
        }
        supportsDescriptorIndex = (EnumValue(transitionType) * kNumOrthogonalDirections) + direction;
        const SlopedSupportsDescriptor& supportsDesc = SupportsDescriptors[supportsDescriptorIndex];
        rearFacingSteep = supportsDesc.hasFrontSprite;
    }

    if (!WoodenSupportsPaintSetupCommon<zOffset, doHeightStepsCheck>(
            session, supportIds, height, imageTemplate, hasSupports, baseHeight, rearFacingSteep))
    {
        return false;
    }

    if (transitionType != WoodenSupportTransitionType::None)
    {
        hasSupports = WoodenABPaintSlopeTransitions(
            session, supportType, subType, transitionType, SupportsDescriptors[supportsDescriptorIndex], direction,
            imageTemplate, baseHeight);
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

    if (!WoodenSupportsPaintSetupCommon<11, false>(session, supportIds, height, imageTemplate, hasSupports, baseHeight, false))
    {
        return false;
    }

    if (isSloped)
    {
        ImageIndex imageIndex = pathPaintInfo.BridgeImageId + 55 + slopeRotation;

        PaintSlopeTransitions(session, imageIndex, imageTemplate, baseHeight);
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
