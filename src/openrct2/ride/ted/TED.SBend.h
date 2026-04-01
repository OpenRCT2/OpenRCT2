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

    static int32_t EvaluatorSBendLeft(const int16_t progress)
    {
        return (progress < 48) ? 98 : -98;
    }

    static int32_t EvaluatorSBendRight(const int16_t progress)
    {
        return (progress < 48) ? -98 : 98;
    }

    static constexpr SequenceDescriptor kSBendLeftSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kSBendLeftSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kSBendLeftSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, {} },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight)),
    };

    static constexpr SequenceDescriptor kSBendLeftSeq3 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kSBendRightSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kSBendLeftSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kSBendRightSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kSBendLeftSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kSBendRightSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kSBendLeftSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kSBendRightSeq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kSBendLeftSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kSBendLeftSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kSBendLeftSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, {} },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kSBendLeftSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq3 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kSBendLeftSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kSBendRightSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kSBendRightSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kSBendRightSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kSBendRightSeq3.blockedSegments,
    };

    constexpr auto kTEDSBendLeft = TrackElementDescriptor{
        .description = STR_S_BEND_LEFT,
        .coordinates = { 0, 0, 0, 0, -64, -32 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::sBendLeftCovered,
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::sBendRight,
        .flags = {},
        .definition = { TrackGroup::sBend, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::lr,
        .lateralFactor = EvaluatorSBendLeft,
        .sequenceData = { 4, { kSBendLeftSeq0, kSBendLeftSeq1, kSBendLeftSeq2, kSBendLeftSeq3 } },
    };

    constexpr auto kTEDSBendRight = TrackElementDescriptor{
        .description = STR_S_BEND_RIGHT,
        .coordinates = { 0, 0, 0, 0, -64, 32 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::sBendRightCovered,
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::sBendLeft,
        .flags = {},
        .definition = { TrackGroup::sBend, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::rl,
        .lateralFactor = EvaluatorSBendRight,
        .sequenceData = { 4, { kSBendRightSeq0, kSBendRightSeq1, kSBendRightSeq2, kSBendRightSeq3 } },
    };

    constexpr auto kTEDSBendLeftCovered = TrackElementDescriptor{
        .description = STR_S_BEND_LEFT,
        .coordinates = { 0, 0, 0, 0, -64, -32 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 233472,
        .mirrorElement = TrackElemType::sBendRightCovered,
        .flags = {},
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::lr,
        .lateralFactor = EvaluatorSBendLeft,
        .sequenceData = { 4, { kSBendLeftCoveredSeq0, kSBendLeftCoveredSeq1, kSBendLeftCoveredSeq2, kSBendLeftCoveredSeq3 } },
    };

    constexpr auto kTEDSBendRightCovered = TrackElementDescriptor{
        .description = STR_S_BEND_RIGHT,
        .coordinates = { 0, 0, 0, 0, -64, 32 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 233472,
        .mirrorElement = TrackElemType::sBendLeftCovered,
        .flags = {},
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::rl,
        .lateralFactor = EvaluatorSBendRight,
        .sequenceData = { 4,
                          { kSBendRightCoveredSeq0, kSBendRightCoveredSeq1, kSBendRightCoveredSeq2, kSBendRightCoveredSeq3 } },
    };
} // namespace OpenRCT2::TrackMetadata
