/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../localisation/StringIds.h"
#include "TrackElementDescriptor.h"

#include <cstdint>

namespace OpenRCT2::TrackMetadata
{
    using PS = PaintSegment;

    static int32_t EvaluatorLargeZeroGRollUp(const int16_t progress)
    {
        if (progress > 114)
            return (371 - 2 * progress);
        return 0;
    }

    static int32_t EvaluatorLargeZeroGRollDown(const int16_t progress)
    {
        if (progress < 38)
            return 67 + 2 * progress;
        return 0;
    }

    static int32_t EvaluatorLargeZeroGRollUpLeft(const int16_t progress)
    {
        return 387 - 2 * progress;
    }

    static int32_t EvaluatorLargeZeroGRollUpRight(const int16_t progress)
    {
        return 2 * progress - 387;
    }

    static int32_t EvaluatorLargeZeroGRollDownLeft(const int16_t progress)
    {
        return 83 + 2 * progress;
    }

    static int32_t EvaluatorLargeZeroGRollDownRight(const int16_t progress)
    {
        return -(83 + 2 * progress);
    }

    static int32_t EvaluatorZeroGRollUpLeft(const int16_t progress)
    {
        return 174 - progress;
    }

    static int32_t EvaluatorZeroGRollUpRight(const int16_t progress)
    {
        return progress - 174;
    }

    static int32_t EvaluatorZeroGRollDownLeft(const int16_t progress)
    {
        return 73 + progress;
    }

    static int32_t EvaluatorZeroGRollDownRight(const int16_t progress)
    {
        return -(73 + progress);
    }

    static constexpr SequenceDescriptor kLeftZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            { PS::centre, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                       // inverted
            kSegmentsAll,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 8, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            { PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsAll,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 24, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            { PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsAll,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            { PS::centre, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                       // inverted
            kSegmentsAll,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 56, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            { PS::centre, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                       // inverted
            kSegmentsAll,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 96, 40, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            { PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsAll,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq3 = {
        .clearance = { -96, 0, 120, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            { PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsAll,                                                                 // wide
        } },
    };

    constexpr auto kTEDLeftZeroGRollUp = TrackElementDescriptor{
        .description = STR_ZERO_G_ROLL_LEFT,
        .coordinates = { 0, 0, 0, 56, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::leftZeroGRollDown, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::rightZeroGRollUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRoll, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<250>,
        .lateralFactor = EvaluatorZeroGRollUpLeft,
        .sequenceData = { 3, { kLeftZeroGRollUpSeq0, kLeftZeroGRollUpSeq1, kLeftZeroGRollUpSeq2 } },
    };

    constexpr auto kTEDRightZeroGRollUp = TrackElementDescriptor{
        .description = STR_ZERO_G_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, 56, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::rightZeroGRollDown, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::leftZeroGRollUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRoll, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<250>,
        .lateralFactor = EvaluatorZeroGRollUpRight,
        .sequenceData = kTEDLeftZeroGRollUp.sequenceData.flipXAxis(),
    };

    constexpr auto kTEDLeftZeroGRollDown = TrackElementDescriptor{
        .description = STR_ZERO_G_ROLL_LEFT,
        .coordinates = { 0, 0, 0, -56, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::leftZeroGRollUp },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::rightZeroGRollDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRoll, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        0 },
        .verticalFactor = EvaluatorConst<250>,
        .lateralFactor = EvaluatorZeroGRollDownLeft,
        .sequenceData = kTEDLeftZeroGRollUp.sequenceData.rotate(2).offset({ -64, 0, -56 }).reverse({ 2, 1, 0 }),
    };

    constexpr auto kTEDRightZeroGRollDown = TrackElementDescriptor{
        .description = STR_ZERO_G_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, -56, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::rightZeroGRollUp },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::leftZeroGRollDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRoll, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        0 },
        .verticalFactor = EvaluatorConst<250>,
        .lateralFactor = EvaluatorZeroGRollDownRight,
        .sequenceData = kTEDLeftZeroGRollDown.sequenceData.flipXAxis(),
    };

    constexpr auto kTEDLeftLargeZeroGRollUp = TrackElementDescriptor{
        .description = STR_LARGE_ZERO_G_ROLL_LEFT,
        .coordinates = { 0, 0, 0, 152, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackElemType::leftLargeZeroGRollDown, TrackCurve::none },
        .priceModifier = 425984,
        .mirrorElement = TrackElemType::rightLargeZeroGRollUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRollLarge, TrackPitch::none, TrackPitch::up60, TrackRoll::upsideDown, TrackRoll::none,
                        64 },
        .verticalFactor = EvaluatorLargeZeroGRollUp,
        .lateralFactor = EvaluatorLargeZeroGRollUpLeft,
        .sequenceData = { 4,
                          { kLeftLargeZeroGRollUpSeq0, kLeftLargeZeroGRollUpSeq1, kLeftLargeZeroGRollUpSeq2,
                            kLeftLargeZeroGRollUpSeq3 } },
    };

    constexpr auto kTEDRightLargeZeroGRollUp = TrackElementDescriptor{
        .description = STR_LARGE_ZERO_G_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, 152, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackElemType::rightLargeZeroGRollDown, TrackCurve::none },
        .priceModifier = 425984,
        .mirrorElement = TrackElemType::leftLargeZeroGRollUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRollLarge, TrackPitch::none, TrackPitch::up60, TrackRoll::upsideDown, TrackRoll::none,
                        64 },
        .verticalFactor = EvaluatorLargeZeroGRollUp,
        .lateralFactor = EvaluatorLargeZeroGRollUpRight,
        .sequenceData = kTEDLeftLargeZeroGRollUp.sequenceData.flipXAxis(),
    };

    constexpr auto kTEDLeftLargeZeroGRollDown = TrackElementDescriptor{
        .description = STR_LARGE_ZERO_G_ROLL_LEFT,
        .coordinates = { 0, 0, 0, -152, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackElemType::leftLargeZeroGRollUp },
        .priceModifier = 425984,
        .mirrorElement = TrackElemType::rightLargeZeroGRollDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRollLarge, TrackPitch::down60, TrackPitch::none, TrackRoll::none,
                        TrackRoll::upsideDown, -64 },
        .verticalFactor = EvaluatorLargeZeroGRollDown,
        .lateralFactor = EvaluatorLargeZeroGRollDownLeft,
        .sequenceData = kTEDLeftLargeZeroGRollUp.sequenceData.rotate(2).offset({ -96, 0, -152 }).reverse({ 3, 2, 1, 0 }),
    };

    constexpr auto kTEDRightLargeZeroGRollDown = TrackElementDescriptor{
        .description = STR_LARGE_ZERO_G_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, -152, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackElemType::rightLargeZeroGRollUp },
        .priceModifier = 425984,
        .mirrorElement = TrackElemType::leftLargeZeroGRollDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRollLarge, TrackPitch::down60, TrackPitch::none, TrackRoll::none,
                        TrackRoll::upsideDown, -64 },
        .verticalFactor = EvaluatorLargeZeroGRollDown,
        .lateralFactor = EvaluatorLargeZeroGRollDownRight,
        .sequenceData = kTEDLeftLargeZeroGRollDown.sequenceData.flipXAxis(),
    };
} // namespace OpenRCT2::TrackMetadata
