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

namespace OpenRCT2::TrackMetadata
{
    using PS = PaintSegment;

    static constexpr SequenceDescriptor kLeftCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft),                            // narrow
            EnumsToFlags(PS::top, PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                                  // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewUpSeq2 = {
        .clearance = { -32, -32, 48, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight),                     // narrow
            EnumsToFlags(PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                               // wide
        } },
    };

    static constexpr SequenceDescriptor kRightCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCorkscrewUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCorkscrewUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCorkscrewUpSeq2 = {
        .clearance = { -32, 32, 48, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight),                   // narrow
            EnumsToFlags(PS::top, PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightCorkscrewUpSeq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -56, 32, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightCorkscrewUpSeq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftCorkscrewDownSeq2 = {
        .clearance = { -32, -32, -80, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsRotate(kRightCorkscrewUpSeq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kRightCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftCorkscrewUpSeq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -56, 32, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kLeftCorkscrewUpSeq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightCorkscrewDownSeq2 = {
        .clearance = { -32, 32, -80, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsRotate(kLeftCorkscrewUpSeq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftCorkscrewUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftCorkscrewUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewUpSeq2 = {
        .clearance = { -32, -32, 48, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kLeftCorkscrewUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightCorkscrewUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightCorkscrewUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewUpSeq2 = {
        .clearance = { -32, 32, 48, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kRightCorkscrewUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                       // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -24, 32, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewDownSeq2 = {
        .clearance = { -32, -32, -48, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                             // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                              // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                              // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -24, 32, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerCorkscrewDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewDownSeq2 = {
        .clearance = { -32, 32, -48, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerCorkscrewDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsUnimplemented,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 8, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq2 = {
        .clearance = { -64, 0, 32, 24, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq3 = {
        .clearance = { -32, -32, 32, 24, { 0b1000, 0 }, {} },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq4 = {
        .clearance = { -64, -32, 48, 40, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq5 = {
        .clearance = { -64, -64, 72, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 8, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq2 = {
        .clearance = { -64, 0, 32, 24, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq3 = {
        .clearance = { -32, 32, 32, 24, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq4 = {
        .clearance = { -64, 32, 48, 40, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq5 = {
        .clearance = { -64, 64, 72, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -40, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq5.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -64, 40, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq4.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq2 = {
        .clearance = { -32, -32, -80, 24, { 0b1000, 0 }, {} },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq3.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq3 = {
        .clearance = { -64, 0, -80, 24, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq4 = {
        .clearance = { -64, -32, -104, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq5 = {
        .clearance = { -64, -64, -112, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -40, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -64, 40, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq2 = {
        .clearance = { -32, 32, -80, 24, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq3 = {
        .clearance = { -64, 0, -80, 24, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq4 = {
        .clearance = { -64, 32, -104, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq5 = {
        .clearance = { -64, 64, -112, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq5.blockedSegments),
    };

    constexpr auto kTEDLeftCorkscrewUp = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_LEFT,
        .coordinates = { 0, 3, 0, 80, -32, -32 },
        .pieceLength = 55,
        .curveChain = { TrackElemType::rightCorkscrewDown, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::rightCorkscrewUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrew, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<70>,
        .sequenceData = { 3, { kLeftCorkscrewUpSeq0, kLeftCorkscrewUpSeq1, kLeftCorkscrewUpSeq2 } },
    };

    constexpr auto kTEDRightCorkscrewUp = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_RIGHT,
        .coordinates = { 0, 1, 0, 80, -32, 32 },
        .pieceLength = 55,
        .curveChain = { TrackElemType::leftCorkscrewDown, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::leftCorkscrewUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrew, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<-70>,
        .sequenceData = { 3, { kRightCorkscrewUpSeq0, kRightCorkscrewUpSeq1, kRightCorkscrewUpSeq2 } },
    };

    constexpr auto kTEDLeftCorkscrewDown = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_LEFT,
        .coordinates = { 0, 3, 0, -80, -32, -32 },
        .pieceLength = 55,
        .curveChain = { TrackCurve::none, TrackElemType::rightCorkscrewUp },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::rightCorkscrewDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrew, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -32 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<-70>,
        .sequenceData = { 3, { kLeftCorkscrewDownSeq0, kLeftCorkscrewDownSeq1, kLeftCorkscrewDownSeq2 } },
    };

    constexpr auto kTEDRightCorkscrewDown = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_RIGHT,
        .coordinates = { 0, 1, 0, -80, -32, 32 },
        .pieceLength = 55,
        .curveChain = { TrackCurve::none, TrackElemType::leftCorkscrewUp },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::leftCorkscrewDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrew, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -32 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<70>,
        .sequenceData = { 3, { kRightCorkscrewDownSeq0, kRightCorkscrewDownSeq1, kRightCorkscrewDownSeq2 } },
    };

    constexpr auto kTEDLeftFlyerCorkscrewUp = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_LEFT,
        .coordinates = { 0, 3, 0, 48, -32, -32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::rightFlyerCorkscrewUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewUninverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<70>,
        .sequenceData = { 3, { kLeftFlyerCorkscrewUpSeq0, kLeftFlyerCorkscrewUpSeq1, kLeftFlyerCorkscrewUpSeq2 } },
    };

    constexpr auto kTEDRightFlyerCorkscrewUp = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_RIGHT,
        .coordinates = { 0, 1, 0, 48, -32, 32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::leftFlyerCorkscrewUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewUninverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<-70>,
        .sequenceData = { 3, { kRightFlyerCorkscrewUpSeq0, kRightFlyerCorkscrewUpSeq1, kRightFlyerCorkscrewUpSeq2 } },
    };

    constexpr auto kTEDLeftFlyerCorkscrewDown = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_LEFT,
        .coordinates = { 0, 3, 0, -48, -32, -32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::rightFlyerCorkscrewDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewInverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, -32 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<-70>,
        .sequenceData = { 3, { kLeftFlyerCorkscrewDownSeq0, kLeftFlyerCorkscrewDownSeq1, kLeftFlyerCorkscrewDownSeq2 } },
    };

    constexpr auto kTEDRightFlyerCorkscrewDown = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_RIGHT,
        .coordinates = { 0, 1, 0, -48, -32, 32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::leftFlyerCorkscrewDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewInverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, -32 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<70>,
        .sequenceData = { 3, { kRightFlyerCorkscrewDownSeq0, kRightFlyerCorkscrewDownSeq1, kRightFlyerCorkscrewDownSeq2 } },
    };

    constexpr auto kTEDLeftLargeCorkscrewUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_CORKSCREW_LEFT,
        .coordinates = { 0, 3, 0, 112, -64, -64 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::rightLargeCorkscrewDown, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::rightLargeCorkscrewUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewLarge, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none,
                        32 },
        .verticalFactor = EvaluatorConst<89>,
        .lateralFactor = EvaluatorConst<117>,
        .sequenceData = { 6,
                          { kLeftLargeCorkscrewUpSeq0, kLeftLargeCorkscrewUpSeq1, kLeftLargeCorkscrewUpSeq2,
                            kLeftLargeCorkscrewUpSeq3, kLeftLargeCorkscrewUpSeq4, kLeftLargeCorkscrewUpSeq5 } },
    };

    constexpr auto kTEDRightLargeCorkscrewUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_CORKSCREW_RIGHT,
        .coordinates = { 0, 1, 0, 112, -64, 64 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::leftLargeCorkscrewDown, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::leftLargeCorkscrewUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewLarge, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none,
                        32 },
        .verticalFactor = EvaluatorConst<89>,
        .lateralFactor = EvaluatorConst<-117>,
        .sequenceData = { 6,
                          { kRightLargeCorkscrewUpSeq0, kRightLargeCorkscrewUpSeq1, kRightLargeCorkscrewUpSeq2,
                            kRightLargeCorkscrewUpSeq3, kRightLargeCorkscrewUpSeq4, kRightLargeCorkscrewUpSeq5 } },
    };

    constexpr auto kTEDLeftLargeCorkscrewDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_CORKSCREW_LEFT,
        .coordinates = { 0, 3, 0, -112, -64, -64 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::rightLargeCorkscrewUp },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::rightLargeCorkscrewDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewLarge, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -32 },
        .verticalFactor = EvaluatorConst<89>,
        .lateralFactor = EvaluatorConst<-117>,
        .sequenceData = { 6,
                          { kLeftLargeCorkscrewDownSeq0, kLeftLargeCorkscrewDownSeq1, kLeftLargeCorkscrewDownSeq2,
                            kLeftLargeCorkscrewDownSeq3, kLeftLargeCorkscrewDownSeq4, kLeftLargeCorkscrewDownSeq5 } },
    };

    constexpr auto kTEDRightLargeCorkscrewDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_CORKSCREW_RIGHT,
        .coordinates = { 0, 1, 0, -112, -64, 64 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::leftLargeCorkscrewUp },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::leftLargeCorkscrewDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewLarge, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -32 },
        .verticalFactor = EvaluatorConst<89>,
        .lateralFactor = EvaluatorConst<117>,
        .sequenceData = { 6,
                          { kRightLargeCorkscrewDownSeq0, kRightLargeCorkscrewDownSeq1, kRightLargeCorkscrewDownSeq2,
                            kRightLargeCorkscrewDownSeq3, kRightLargeCorkscrewDownSeq4, kRightLargeCorkscrewDownSeq5 } },
    };

} // namespace OpenRCT2::TrackMetadata
