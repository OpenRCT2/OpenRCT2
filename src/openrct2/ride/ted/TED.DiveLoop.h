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

    static int32_t EvaluatorDiveLoopUp(const int16_t progress)
    {
        return 385 - 2 * progress;
    }

    static int32_t EvaluatorDiveLoopDown(const int16_t progress)
    {
        return 67 + 2 * progress;
    }

    static int32_t EvaluatorDiveLoopUpLeft(const int16_t progress)
    {
        return 380 - 2 * progress;
    }

    static int32_t EvaluatorDiveLoopUpRight(const int16_t progress)
    {
        return 2 * progress - 380;
    }

    static int32_t EvaluatorDiveLoopDownLeft(const int16_t progress)
    {
        return -(62 + 2 * progress);
    }

    static int32_t EvaluatorDiveLoopDownRight(const int16_t progress)
    {
        return 62 + 2 * progress;
    }

    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 88, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 88, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq3 = {
        .clearance = { -32, 32, 32, 80, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq4 = {
        .clearance = { -64, 32, 80, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq5 = {
        .clearance = { -96, 32, 112, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq0.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq1 = {
        .clearance = { -32, 0, 0, 88, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq1.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq2 = {
        .clearance = { 0, 32, 0, 88, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq2.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq3 = {
        .clearance = { -32, 32, 32, 80, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq3.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq4 = {
        .clearance = { -32, 64, 80, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq4.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq5 = {
        .clearance = { -32, 96, 112, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq5.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq0 = {
        .clearance = { 0, 0, 112, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq5.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq1 = {
        .clearance = { -32, 0, 80, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq4.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq2 = {
        .clearance = { -64, 0, 32, 80, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq3.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq3 = {
        .clearance = { -64, -32, 0, 88, { 0b1000, 0 }, {} },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq2.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq4 = {
        .clearance = { -96, 0, 0, 88, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq1.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq5 = {
        .clearance = { -96, -32, 0, 64, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq0.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq0 = {
        .clearance = { 0, 0, 112, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq5.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq1 = {
        .clearance = { -32, 0, 80, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq4.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq2 = {
        .clearance = { -64, 0, 32, 80, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq3.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq3 = {
        .clearance = { -64, 32, 0, 88, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq2.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq4 = {
        .clearance = { -96, 0, 0, 88, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq1.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq5 = {
        .clearance = { -96, 32, 0, 64, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq0.blockedSegments, 2),
    };

    constexpr auto kTEDLeftEighthDiveLoopUpToOrthogonal = TrackElementDescriptor{
        .description = STR_DIVE_LOOP_LEFT,
        .coordinates = { 4, 0, 0, 152, -96, 32 },
        .pieceLength = 165,
        .curveChain = { TrackElemType::rightEighthDiveLoopDownToDiag, TrackCurve::none },
        .priceModifier = 458752,
        .mirrorElement = TrackElemType::rightEighthDiveLoopUpToOrthogonal,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::normalToInversion,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::diveLoop, TrackPitch::none, TrackPitch::up60, TrackRoll::upsideDown, TrackRoll::none, 56 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorDiveLoopUp,
        .lateralFactor = EvaluatorDiveLoopUpLeft,
        .sequenceData = { 6,
                          { kLeftEighthDiveLoopUpToOrthogonalSeq0, kLeftEighthDiveLoopUpToOrthogonalSeq1,
                            kLeftEighthDiveLoopUpToOrthogonalSeq2, kLeftEighthDiveLoopUpToOrthogonalSeq3,
                            kLeftEighthDiveLoopUpToOrthogonalSeq4, kLeftEighthDiveLoopUpToOrthogonalSeq5 } },
    };

    constexpr auto kTEDRightEighthDiveLoopUpToOrthogonal = TrackElementDescriptor{
        .description = STR_DIVE_LOOP_RIGHT,
        .coordinates = { 4, 1, 0, 152, -32, 96 },
        .pieceLength = 165,
        .curveChain = { TrackElemType::leftEighthDiveLoopDownToDiag, TrackCurve::none },
        .priceModifier = 458752,
        .mirrorElement = TrackElemType::leftEighthDiveLoopUpToOrthogonal,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::normalToInversion,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::diveLoop, TrackPitch::none, TrackPitch::up60, TrackRoll::upsideDown, TrackRoll::none, 56 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorDiveLoopUp,
        .lateralFactor = EvaluatorDiveLoopUpRight,
        .sequenceData = { 6,
                          { kRightEighthDiveLoopUpToOrthogonalSeq0, kRightEighthDiveLoopUpToOrthogonalSeq1,
                            kRightEighthDiveLoopUpToOrthogonalSeq2, kRightEighthDiveLoopUpToOrthogonalSeq3,
                            kRightEighthDiveLoopUpToOrthogonalSeq4, kRightEighthDiveLoopUpToOrthogonalSeq5 } },
    };

    constexpr auto kTEDLeftEighthDiveLoopDownToDiag = TrackElementDescriptor{
        .description = STR_DIVE_LOOP_LEFT,
        .coordinates = { 0, 7, 152, 0, -96, -32 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackElemType::rightEighthDiveLoopUpToOrthogonal },
        .priceModifier = 458752,
        .mirrorElement = TrackElemType::rightEighthDiveLoopDownToDiag,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::diveLoop, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -56 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorDiveLoopDown,
        .lateralFactor = EvaluatorDiveLoopDownLeft,
        .sequenceData = { 6,
                          { kLeftEighthDiveLoopDownToDiagSeq0, kLeftEighthDiveLoopDownToDiagSeq1,
                            kLeftEighthDiveLoopDownToDiagSeq2, kLeftEighthDiveLoopDownToDiagSeq3,
                            kLeftEighthDiveLoopDownToDiagSeq4, kLeftEighthDiveLoopDownToDiagSeq5 } },
    };

    constexpr auto kTEDRightEighthDiveLoopDownToDiag = TrackElementDescriptor{
        .description = STR_DIVE_LOOP_RIGHT,
        .coordinates = { 0, 4, 152, 0, -96, 32 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackElemType::leftEighthDiveLoopUpToOrthogonal },
        .priceModifier = 458752,
        .mirrorElement = TrackElemType::leftEighthDiveLoopDownToDiag,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::diveLoop, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -56 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorDiveLoopDown,
        .lateralFactor = EvaluatorDiveLoopDownRight,
        .sequenceData = { 6,
                          { kRightEighthDiveLoopDownToDiagSeq0, kRightEighthDiveLoopDownToDiagSeq1,
                            kRightEighthDiveLoopDownToDiagSeq2, kRightEighthDiveLoopDownToDiagSeq3,
                            kRightEighthDiveLoopDownToDiagSeq4, kRightEighthDiveLoopDownToDiagSeq5 } },
    };

} // namespace OpenRCT2::TrackMetadata
