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
            { PS::right, PS::centre, PS::topRight, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                   // inverted
            kSegmentsUnimplemented,                                   // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 88, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            { PS::top, PS::centre, PS::topLeft, PS::topRight }, // narrow
            kSegmentsUnimplemented,                             // inverted
            kSegmentsUnimplemented,                             // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 88, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            { PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                      // inverted
            kSegmentsUnimplemented,                                      // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq3 = {
        .clearance = { -32, 32, 32, 80, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            { PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsUnimplemented,                                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq4 = {
        .clearance = { -64, 32, 80, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            { PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsUnimplemented,                                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq5 = {
        .clearance = { -96, 32, 112, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            { PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsUnimplemented,                                                       // wide
        } },
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
        .sequenceData = kTEDLeftEighthDiveLoopUpToOrthogonal.sequenceData.flipDiagonal(),
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
        .sequenceData =
            [] { // TED sequence override! (diagonal connection) (#27299)
                auto sequenceData = kTEDRightEighthDiveLoopUpToOrthogonal.sequenceData.rotate(1)
                                        .offset({ -96, -32, 0 })
                                        .reverse({ 5, 4, 3, 2, 1, 0 });
                sequenceData.sequences[5].clearance.quarterTile = QuarterTile(0b0001);
                return sequenceData;
            }(),
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
        .sequenceData = kTEDLeftEighthDiveLoopDownToDiag.sequenceData.flipDiagonal().rotate(3),
    };

} // namespace OpenRCT2::TrackMetadata
