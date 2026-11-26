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

    static int32_t EvaluatorHeartLineTransferUp(const int16_t progress)
    {
        if (progress < 32)
            return 103;
        if (progress < 64)
            return -103;
        if (progress < 96)
            return 0;
        if (progress < 128)
            return 103;
        return -103;
    }

    static int32_t EvaluatorHeartLineTransferDown(const int16_t progress)
    {
        if (progress < 32)
            return -103;
        if (progress < 64)
            return 103;
        if (progress < 96)
            return 0;
        if (progress < 128)
            return -103;
        return 103;
    }

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq1 = {
        .clearance = { -32, 0, 0, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq3 = {
        .clearance = { 0, 0, 32, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq2 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq3 = {
        .clearance = { -96, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq4 = {
        .clearance = { -128, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq5 = {
        .clearance = { -160, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    constexpr auto kTEDHeartLineTransferUp = TrackElementDescriptor{
        .description = STR_UPPER_TRANSFER,
        .coordinates = { 0, 2, 0, 32, 0, 0 },
        .pieceLength = 16,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::heartLineTransferUp,
        .flags = {},
        .definition = { TrackGroup::heartlineTransfer, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorHeartLineTransferUp,
        .sequenceData = { 4,
                          { kHeartLineTransferUpSeq0, kHeartLineTransferUpSeq1, kHeartLineTransferUpSeq2,
                            kHeartLineTransferUpSeq3 } },
    };

    constexpr auto kTEDHeartLineTransferDown = TrackElementDescriptor{
        .description = STR_LOWER_TRANSFER,
        .coordinates = { 0, 2, 0, -32, 0, 0 },
        .pieceLength = 16,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::heartLineTransferDown,
        .flags = {},
        .definition = { TrackGroup::heartlineTransfer, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorHeartLineTransferDown,
        .sequenceData = kTEDHeartLineTransferUp.sequenceData.offset({ 0, 0, -32 }).reverse({ 3, 1, 2, 0 }),
    };

    constexpr auto kTEDLeftHeartLineRoll = TrackElementDescriptor{
        .description = STR_HEARTLINE_ROLL_LEFT,
        .coordinates = { 0, 0, 0, 0, -160, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 417792,
        .mirrorElement = TrackElemType::rightHeartLineRoll,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal,
                   TrackElementFlag::canBePartlyUnderground },
        .definition = { TrackGroup::heartlineRoll, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 6,
                          { kLeftHeartLineRollSeq0, kLeftHeartLineRollSeq1, kLeftHeartLineRollSeq2, kLeftHeartLineRollSeq3,
                            kLeftHeartLineRollSeq4, kLeftHeartLineRollSeq5 } },
    };

    constexpr auto kTEDRightHeartLineRoll = TrackElementDescriptor{
        .description = STR_HEARTLINE_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, 0, -160, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 417792,
        .mirrorElement = TrackElemType::leftHeartLineRoll,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal,
                   TrackElementFlag::canBePartlyUnderground },
        .definition = { TrackGroup::heartlineRoll, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = kTEDLeftHeartLineRoll.sequenceData.flipXAxis(),
    };
} // namespace OpenRCT2::TrackMetadata
