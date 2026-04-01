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

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsAll,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsNone,          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq2 = {
        .clearance = { -32, 0, 0, 4, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq3 = {
        .clearance = { -32, -32, 0, 4, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq4 = {
        .clearance = { -32, -64, 8, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                           // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq5 = {
        .clearance = { 0, -64, 8, 0, { 0b0100, 0b0000 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsNone,          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq6 = {
        .clearance = { -32, -96, 8, 4, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq7 = {
        .clearance = { 0, -96, 8, 4, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -2,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0b0000 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq2 = {
        .clearance = { -32, 0, 0, 4, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq3 = {
        .clearance = { -32, 32, 0, 4, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq4 = {
        .clearance = { -32, 64, 8, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq5 = {
        .clearance = { 0, 64, 8, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq6 = {
        .clearance = { -32, 96, 8, 4, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq7 = {
        .clearance = { 0, 96, 8, 4, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq7.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq1 = {
        .clearance = { 0, -32, 8, 4, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq2 = {
        .clearance = { -32, 0, 8, 0, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq3 = {
        .clearance = { -32, -32, 8, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq4 = {
        .clearance = { -32, -64, 0, 4, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq5 = {
        .clearance = { 0, -64, 0, 4, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq6 = {
        .clearance = { -32, -96, 0, 0, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq7 = {
        .clearance = { 0, -96, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -2,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq7.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq1 = {
        .clearance = { 0, 32, 8, 4, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq2 = {
        .clearance = { -32, 0, 8, 0, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq3 = {
        .clearance = { -32, 32, 8, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq4 = {
        .clearance = { -32, 64, 0, 4, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq5 = {
        .clearance = { 0, 64, 0, 4, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq6 = {
        .clearance = { -32, 96, 0, 0, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq7 = {
        .clearance = { 0, 96, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq7.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsAll,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsUnimplemented,                                 // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq4 = {
        .clearance = { -32, -64, 0, 4, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsUnimplemented,                                 // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq5 = {
        .clearance = { -64, -32, 0, 4, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq6 = {
        .clearance = { -64, -64, 0, 4, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq7 = {
        .clearance = { -64, -96, 8, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                           // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq8 = {
        .clearance = { -32, -96, 8, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            kSegmentsNone,                                    // narrow
            kSegmentsUnimplemented,                           // inverted
            EnumsToFlags(PS::top, PS::topLeft, PS::topRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq9 = {
        .clearance = { -64, -128, 8, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq10 = {
        .clearance = { -32, -128, 8, 0, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            EnumsToFlags(
                PS::top, PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq11 = {
        .clearance = { 0, -128, 8, 4, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            kSegmentsNone,                                    // narrow
            kSegmentsUnimplemented,                           // inverted
            EnumsToFlags(PS::top, PS::topLeft, PS::topRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq12 = {
        .clearance = { -32, -160, 8, 4, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq13 = {
        .clearance = { 0, -160, 8, 4, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -2,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq4 = {
        .clearance = { -32, 64, 0, 4, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq5 = {
        .clearance = { -64, 32, 0, 4, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq6 = {
        .clearance = { -64, 64, 0, 4, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq7 = {
        .clearance = { -64, 96, 8, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq7.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq8 = {
        .clearance = { -32, 96, 8, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq8.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq9 = {
        .clearance = { -64, 128, 8, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq9.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq10 = {
        .clearance = { -32, 128, 8, 0, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq10.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq11 = {
        .clearance = { 0, 128, 8, 4, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq11.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq12 = {
        .clearance = { -32, 160, 8, 4, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq12.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq13 = {
        .clearance = { 0, 160, 8, 4, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq13.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq1 = {
        .clearance = { 0, -32, 8, 4, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq2 = {
        .clearance = { -32, 0, 8, 4, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq3 = {
        .clearance = { -32, -32, 8, 0, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq4 = {
        .clearance = { -32, -64, 8, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq5 = {
        .clearance = { -64, -32, 8, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq6 = {
        .clearance = { -64, -64, 8, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq7 = {
        .clearance = { -64, -96, 0, 4, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq7.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq8 = {
        .clearance = { -32, -96, 0, 4, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq8.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq9 = {
        .clearance = { -64, -128, 0, 4, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq9.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq10 = {
        .clearance = { -32, -128, 0, 0, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq10.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq11 = {
        .clearance = { 0, -128, 0, 0, { 0b0100, 0b0000 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq11.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq12 = {
        .clearance = { -32, -160, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq12.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq13 = {
        .clearance = { 0, -160, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -2,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq13.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq1 = {
        .clearance = { 0, 32, 8, 4, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq2 = {
        .clearance = { -32, 0, 8, 4, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq3 = {
        .clearance = { -32, 32, 8, 0, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq4 = {
        .clearance = { -32, 64, 8, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq5 = {
        .clearance = { -64, 32, 8, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq6 = {
        .clearance = { -64, 64, 8, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq7 = {
        .clearance = { -64, 96, 0, 4, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq7.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq8 = {
        .clearance = { -32, 96, 0, 4, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq8.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq9 = {
        .clearance = { -64, 128, 0, 4, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq9.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq10 = {
        .clearance = { -32, 128, 0, 0, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq10.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq11 = {
        .clearance = { 0, 128, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq11.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq12 = {
        .clearance = { -32, 160, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq12.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq13 = {
        .clearance = { 0, 160, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq13.blockedSegments,
    };

    constexpr auto kTEDLeftHalfBankedHelixUpSmall = TrackElementDescriptor{
        .description = STR_HELIX_UP_SMALL,
        .coordinates = { 0, 2, 0, 16, 0, -96 },
        .pieceLength = 150,
        .curveChain = { TrackElemType::leftHalfBankedHelixUpSmall, TrackElemType::leftHalfBankedHelixUpSmall },
        .priceModifier = 328192,
        .mirrorElement = TrackElemType::rightHalfBankedHelixUpSmall,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixUpBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorConst<100>,
        .lateralFactor = EvaluatorConst<100>,
        .sequenceData = { 8,
                          { kLeftHalfBankedHelixUpSmallSeq0, kLeftHalfBankedHelixUpSmallSeq1, kLeftHalfBankedHelixUpSmallSeq2,
                            kLeftHalfBankedHelixUpSmallSeq3, kLeftHalfBankedHelixUpSmallSeq4, kLeftHalfBankedHelixUpSmallSeq5,
                            kLeftHalfBankedHelixUpSmallSeq6, kLeftHalfBankedHelixUpSmallSeq7 } },
    };

    constexpr auto kTEDRightHalfBankedHelixUpSmall = TrackElementDescriptor{
        .description = STR_HELIX_UP_SMALL,
        .coordinates = { 0, 2, 0, 16, 0, 96 },
        .pieceLength = 150,
        .curveChain = { TrackElemType::rightHalfBankedHelixUpSmall, TrackElemType::rightHalfBankedHelixUpSmall },
        .priceModifier = 328192,
        .mirrorElement = TrackElemType::leftHalfBankedHelixUpSmall,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixUpBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorConst<100>,
        .lateralFactor = EvaluatorConst<-100>,
        .sequenceData = { 8,
                          { kRightHalfBankedHelixUpSmallSeq0, kRightHalfBankedHelixUpSmallSeq1,
                            kRightHalfBankedHelixUpSmallSeq2, kRightHalfBankedHelixUpSmallSeq3,
                            kRightHalfBankedHelixUpSmallSeq4, kRightHalfBankedHelixUpSmallSeq5,
                            kRightHalfBankedHelixUpSmallSeq6, kRightHalfBankedHelixUpSmallSeq7 } },
    };

    constexpr auto kTEDLeftHalfBankedHelixDownSmall = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_SMALL,
        .coordinates = { 0, 2, 16, 0, 0, -96 },
        .pieceLength = 150,
        .curveChain = { TrackElemType::leftHalfBankedHelixDownSmall, TrackElemType::leftHalfBankedHelixDownSmall },
        .priceModifier = 328192,
        .mirrorElement = TrackElemType::rightHalfBankedHelixDownSmall,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixDownBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorConst<100>,
        .lateralFactor = EvaluatorConst<100>,
        .sequenceData = { 8,
                          { kLeftHalfBankedHelixDownSmallSeq0, kLeftHalfBankedHelixDownSmallSeq1,
                            kLeftHalfBankedHelixDownSmallSeq2, kLeftHalfBankedHelixDownSmallSeq3,
                            kLeftHalfBankedHelixDownSmallSeq4, kLeftHalfBankedHelixDownSmallSeq5,
                            kLeftHalfBankedHelixDownSmallSeq6, kLeftHalfBankedHelixDownSmallSeq7 } },
    };

    constexpr auto kTEDRightHalfBankedHelixDownSmall = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_SMALL,
        .coordinates = { 0, 2, 16, 0, 0, 96 },
        .pieceLength = 150,
        .curveChain = { TrackElemType::rightHalfBankedHelixDownSmall, TrackElemType::rightHalfBankedHelixDownSmall },
        .priceModifier = 328192,
        .mirrorElement = TrackElemType::leftHalfBankedHelixDownSmall,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixDownBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorConst<100>,
        .lateralFactor = EvaluatorConst<-100>,
        .sequenceData = { 8,
                          { kRightHalfBankedHelixDownSmallSeq0, kRightHalfBankedHelixDownSmallSeq1,
                            kRightHalfBankedHelixDownSmallSeq2, kRightHalfBankedHelixDownSmallSeq3,
                            kRightHalfBankedHelixDownSmallSeq4, kRightHalfBankedHelixDownSmallSeq5,
                            kRightHalfBankedHelixDownSmallSeq6, kRightHalfBankedHelixDownSmallSeq7 } },
    };

    constexpr auto kTEDLeftHalfBankedHelixUpLarge = TrackElementDescriptor{
        .description = STR_HELIX_UP_LARGE,
        .coordinates = { 0, 2, 0, 16, 0, -160 },
        .pieceLength = 248,
        .curveChain = { TrackElemType::leftHalfBankedHelixUpLarge, TrackElemType::leftHalfBankedHelixUpLarge },
        .priceModifier = 547072,
        .mirrorElement = TrackElemType::rightHalfBankedHelixUpLarge,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixUpBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 14,
                          { kLeftHalfBankedHelixUpLargeSeq0, kLeftHalfBankedHelixUpLargeSeq1, kLeftHalfBankedHelixUpLargeSeq2,
                            kLeftHalfBankedHelixUpLargeSeq3, kLeftHalfBankedHelixUpLargeSeq4, kLeftHalfBankedHelixUpLargeSeq5,
                            kLeftHalfBankedHelixUpLargeSeq6, kLeftHalfBankedHelixUpLargeSeq7, kLeftHalfBankedHelixUpLargeSeq8,
                            kLeftHalfBankedHelixUpLargeSeq9, kLeftHalfBankedHelixUpLargeSeq10, kLeftHalfBankedHelixUpLargeSeq11,
                            kLeftHalfBankedHelixUpLargeSeq12, kLeftHalfBankedHelixUpLargeSeq13 } },
    };

    constexpr auto kTEDRightHalfBankedHelixUpLarge = TrackElementDescriptor{
        .description = STR_HELIX_UP_LARGE,
        .coordinates = { 0, 2, 0, 16, 0, 160 },
        .pieceLength = 248,
        .curveChain = { TrackElemType::rightHalfBankedHelixUpLarge, TrackElemType::rightHalfBankedHelixUpLarge },
        .priceModifier = 547072,
        .mirrorElement = TrackElemType::leftHalfBankedHelixUpLarge,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixUpBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 14,
                          { kRightHalfBankedHelixUpLargeSeq0, kRightHalfBankedHelixUpLargeSeq1,
                            kRightHalfBankedHelixUpLargeSeq2, kRightHalfBankedHelixUpLargeSeq3,
                            kRightHalfBankedHelixUpLargeSeq4, kRightHalfBankedHelixUpLargeSeq5,
                            kRightHalfBankedHelixUpLargeSeq6, kRightHalfBankedHelixUpLargeSeq7,
                            kRightHalfBankedHelixUpLargeSeq8, kRightHalfBankedHelixUpLargeSeq9,
                            kRightHalfBankedHelixUpLargeSeq10, kRightHalfBankedHelixUpLargeSeq11,
                            kRightHalfBankedHelixUpLargeSeq12, kRightHalfBankedHelixUpLargeSeq13 } },
    };

    constexpr auto kTEDLeftHalfBankedHelixDownLarge = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_LARGE,
        .coordinates = { 0, 2, 16, 0, 0, -160 },
        .pieceLength = 248,
        .curveChain = { TrackElemType::leftHalfBankedHelixDownLarge, TrackElemType::leftHalfBankedHelixDownLarge },
        .priceModifier = 547072,
        .mirrorElement = TrackElemType::rightHalfBankedHelixDownLarge,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixDownBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 14,
                          { kLeftHalfBankedHelixDownLargeSeq0, kLeftHalfBankedHelixDownLargeSeq1,
                            kLeftHalfBankedHelixDownLargeSeq2, kLeftHalfBankedHelixDownLargeSeq3,
                            kLeftHalfBankedHelixDownLargeSeq4, kLeftHalfBankedHelixDownLargeSeq5,
                            kLeftHalfBankedHelixDownLargeSeq6, kLeftHalfBankedHelixDownLargeSeq7,
                            kLeftHalfBankedHelixDownLargeSeq8, kLeftHalfBankedHelixDownLargeSeq9,
                            kLeftHalfBankedHelixDownLargeSeq10, kLeftHalfBankedHelixDownLargeSeq11,
                            kLeftHalfBankedHelixDownLargeSeq12, kLeftHalfBankedHelixDownLargeSeq13 } },
    };

    constexpr auto kTEDRightHalfBankedHelixDownLarge = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_LARGE,
        .coordinates = { 0, 2, 16, 0, 0, 160 },
        .pieceLength = 248,
        .curveChain = { TrackElemType::rightHalfBankedHelixDownLarge, TrackElemType::rightHalfBankedHelixDownLarge },
        .priceModifier = 547072,
        .mirrorElement = TrackElemType::leftHalfBankedHelixDownLarge,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixDownBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 14,
                          { kRightHalfBankedHelixDownLargeSeq0, kRightHalfBankedHelixDownLargeSeq1,
                            kRightHalfBankedHelixDownLargeSeq2, kRightHalfBankedHelixDownLargeSeq3,
                            kRightHalfBankedHelixDownLargeSeq4, kRightHalfBankedHelixDownLargeSeq5,
                            kRightHalfBankedHelixDownLargeSeq6, kRightHalfBankedHelixDownLargeSeq7,
                            kRightHalfBankedHelixDownLargeSeq8, kRightHalfBankedHelixDownLargeSeq9,
                            kRightHalfBankedHelixDownLargeSeq10, kRightHalfBankedHelixDownLargeSeq11,
                            kRightHalfBankedHelixDownLargeSeq12, kRightHalfBankedHelixDownLargeSeq13 } },
    };
} // namespace OpenRCT2::TrackMetadata
