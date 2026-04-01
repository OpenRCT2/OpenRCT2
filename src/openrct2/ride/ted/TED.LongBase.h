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

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq2 = {
        .clearance = { -64, 0, 16, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq3 = {
        .clearance = { -96, 0, 40, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 0, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq1 = {
        .clearance = { -32, 0, 40, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq2 = {
        .clearance = { -64, 0, 64, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq3 = {
        .clearance = { -96, 0, 80, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 40, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq1 = {
        .clearance = { -32, 0, 16, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq3 = {
        .clearance = { -96, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq0 = {
        .clearance = { 0, 0, 80, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq1 = {
        .clearance = { -32, 0, 64, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq2 = {
        .clearance = { -64, 0, 40, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq3 = {
        .clearance = { -96, 0, 0, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq3 = {
        .clearance = { -32, 32, 0, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq4 = {
        .clearance = { -32, 64, 8, 40, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq5 = {
        .clearance = { -64, 32, 8, 40, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq6 = {
        .clearance = { -64, 64, 8, 64, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq7 = {
        .clearance = { -64, 96, 40, 88, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq8 = {
        .clearance = { -96, 64, 40, 88, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq9 = {
        .clearance = { -96, 96, 40, 104, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq1 = {
        .clearance = { 0, 32, 0, 72, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq2 = {
        .clearance = { -32, 0, 0, 72, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq3 = {
        .clearance = { -32, 32, 0, 80, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq4 = {
        .clearance = { -32, 64, 56, 32, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq5 = {
        .clearance = { -64, 32, 56, 32, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq6 = {
        .clearance = { -64, 64, 56, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq7 = {
        .clearance = { -64, 96, 80, 8, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq8 = {
        .clearance = { -96, 64, 80, 8, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq9 = {
        .clearance = { -96, 96, 80, 8, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq0 = {
        .clearance = { 0, 0, 80, 8, { 0b1101, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq9.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq1 = {
        .clearance = { 0, 32, 80, 8, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq8.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq2 = {
        .clearance = { -32, 0, 80, 8, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq7.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq3 = {
        .clearance = { -32, 32, 56, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq6.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq4 = {
        .clearance = { -32, 64, 56, 32, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq5.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq5 = {
        .clearance = { -64, 32, 56, 32, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq4.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq6 = {
        .clearance = { -64, 64, 0, 80, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq3.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq7 = {
        .clearance = { -64, 96, 0, 72, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq2.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq8 = {
        .clearance = { -96, 64, 0, 72, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq1.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq9 = {
        .clearance = { -96, 96, 0, 64, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq0.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 40, 104, { 0b1101, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq9.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq1 = {
        .clearance = { 0, 32, 40, 88, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq8.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq2 = {
        .clearance = { -32, 0, 40, 88, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq7.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq3 = {
        .clearance = { -32, 32, 8, 64, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq6.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq4 = {
        .clearance = { -32, 64, 8, 40, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq5.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq5 = {
        .clearance = { -64, 32, 8, 40, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq4.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq6 = {
        .clearance = { -64, 64, 0, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq3.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq7 = {
        .clearance = { -64, 96, 0, 16, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq2.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq8 = {
        .clearance = { -96, 64, 0, 16, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq1.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq9 = {
        .clearance = { -96, 96, 0, 0, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq0.blockedSegments, 2),
    };

    constexpr auto kTEDFlatToUp60LongBase = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 88, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 376832,
        .mirrorElement = TrackElemType::flatToUp60LongBase,
        .flags = { TrackElementFlag::up },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::up60, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<160>,
        .sequenceData = { 4,
                          { kFlatToUp60LongBaseSeq0, kFlatToUp60LongBaseSeq1, kFlatToUp60LongBaseSeq2,
                            kFlatToUp60LongBaseSeq3 } },
    };

    constexpr auto kTEDUp60ToFlatLongBase = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 88, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 376832,
        .mirrorElement = TrackElemType::up60ToFlatLongBase,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::none, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-160>,
        .sequenceData = { 4,
                          { kUp60ToFlatLongBaseSeq0, kUp60ToFlatLongBaseSeq1, kUp60ToFlatLongBaseSeq2,
                            kUp60ToFlatLongBaseSeq3 } },
    };

    constexpr auto kTEDDown60ToFlatLongBase = TrackElementDescriptor{
        .coordinates = { 0, 0, 88, 0, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 376832,
        .mirrorElement = TrackElemType::down60ToFlatLongBase,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::none, TrackPitch::down60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<160>,
        .sequenceData = { 4,
                          { kDown60ToFlatLongBaseSeq0, kDown60ToFlatLongBaseSeq1, kDown60ToFlatLongBaseSeq2,
                            kDown60ToFlatLongBaseSeq3 } },
    };

    constexpr auto kTEDFlatToDown60LongBase = TrackElementDescriptor{
        .coordinates = { 0, 0, 88, 0, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 376832,
        .mirrorElement = TrackElemType::flatToDown60LongBase,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-160>,
        .sequenceData = { 4,
                          { kFlatToDown60LongBaseSeq0, kFlatToDown60LongBaseSeq1, kFlatToDown60LongBaseSeq2,
                            kFlatToDown60LongBaseSeq3 } },
    };
} // namespace OpenRCT2::TrackMetadata
