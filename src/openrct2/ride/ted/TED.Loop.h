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

#include <cmath>
#include <cstdint>

namespace OpenRCT2::TrackMetadata
{
    using PS = PaintSegment;

    static int32_t EvaluatorVerticalLoop(const int16_t progress)
    {
        return (abs(progress - 155) / 2) + 28;
    }

    static int32_t EvaluatorHalfLoopUp(const int16_t progress)
    {
        return ((static_cast<uint16_t>(-(progress - 155))) / 2) + 28;
    }

    static int32_t EvaluatorHalfLoopDown(const int16_t progress)
    {
        return (progress / 2) + 28;
    }

    static int32_t EvaluatorMediumHalfLoopUp(const int16_t progress)
    {
        return (244 - progress) / 4 + 51;
    }

    static int32_t EvaluatorMediumHalfLoopDown(const int16_t progress)
    {
        return (progress / 4) + 51;
    }

    static int32_t EvaluatorLargeHalfLoopUp(const int16_t progress)
    {
        return ((static_cast<uint16_t>(-(progress - 311))) / 4) + 46;
    }

    static int32_t EvaluatorLargeHalfLoopDown(const int16_t progress)
    {
        return (progress / 4) + 46;
    }

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(
                PS::top, PS::left, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight),   // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsAll,                                                                           // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b0110, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq4 = {
        .clearance = { -32, -32, 120, 0, { 0b0000, 0 }, {} },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsNone,          // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq5 = {
        .clearance = { 0, 0, 120, 0, { 0b0000, 0 }, {} },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsNone,          // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq6 = {
        .clearance = { 0, -32, 120, 16, { 0b1001, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq7 = {
        .clearance = { 32, -32, 32, 96, { 0b1000, 0 }, {} },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq8 = {
        .clearance = { 0, -32, 16, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsAll,                                                                                   // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq9 = {
        .clearance = { -32, -32, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight),   // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b1001, 0 }, {} },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq4 = {
        .clearance = { -32, 32, 120, 0, { 0b0000, 0 }, {} },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq5 = {
        .clearance = { 0, 0, 120, 0, { 0b0000, 0 }, {} },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq6 = {
        .clearance = { 0, 32, 120, 16, { 0b0110, 0 }, {} },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq7 = {
        .clearance = { 32, 32, 32, 96, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq7.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq8 = {
        .clearance = { 0, 32, 16, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq8.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq9 = {
        .clearance = { -32, 32, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq9.blockedSegments),
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0011, 0 }, {} },
        .allowedWallEdges = 0b1011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -120, 96, { 0b0011, 0 }, {} },
        .allowedWallEdges = 0b1011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq2 = {
        .clearance = { 0, 0, -136, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq3 = {
        .clearance = { 32, 0, -152, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq4 = {
        .clearance = { -128, -32, 120, 96, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq5 = {
        .clearance = { -96, -32, 64, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq6 = {
        .clearance = { -64, -32, 248, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq4 = {
        .clearance = { -128, 32, 120, 96, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq5 = {
        .clearance = { -96, 32, 64, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq6 = {
        .clearance = { -64, 32, 248, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kRightLargeHalfLoopUpSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = kRightLargeHalfLoopUpSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kRightLargeHalfLoopUpSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq3 = {
        .clearance = { -32, -32, -216, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightLargeHalfLoopUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq4 = {
        .clearance = { 0, -32, -248, 56, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = kRightLargeHalfLoopUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq5 = {
        .clearance = { 32, -32, -264, 40, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kRightLargeHalfLoopUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq6 = {
        .clearance = { 64, -32, -280, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightLargeHalfLoopUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kLeftLargeHalfLoopUpSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = kLeftLargeHalfLoopUpSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kLeftLargeHalfLoopUpSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq3 = {
        .clearance = { -32, 32, -216, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftLargeHalfLoopUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq4 = {
        .clearance = { 0, 32, -248, 56, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = kLeftLargeHalfLoopUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq5 = {
        .clearance = { 32, 32, -264, 40, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kLeftLargeHalfLoopUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 56, 120, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq3 = {
        .clearance = { -64, -32, 56, 120, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq4 = {
        .clearance = { -32, -32, 168, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 56, 120, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq3 = {
        .clearance = { -64, 32, 56, 120, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq4 = {
        .clearance = { -32, 32, 168, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -48, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightMediumHalfLoopUpSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -160, 120, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kRightMediumHalfLoopUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq2 = {
        .clearance = { -32, -32, -160, 120, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightMediumHalfLoopUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq3 = {
        .clearance = { 0, -32, -200, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = kRightMediumHalfLoopUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq4 = {
        .clearance = { 32, -32, -216, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightMediumHalfLoopUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -48, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -160, 120, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq2 = {
        .clearance = { -32, 32, -160, 120, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq3 = {
        .clearance = { 0, 32, -200, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq4 = {
        .clearance = { 32, 32, -216, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq4 = {
        .clearance = { -128, -32, 120, 96, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq5 = {
        .clearance = { -96, -32, 64, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq6 = {
        .clearance = { -64, -32, 248, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq4 = {
        .clearance = { -128, 32, 120, 96, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq5 = {
        .clearance = { -96, 32, 64, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq6 = {
        .clearance = { -64, 32, 248, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                 // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq1 = {
        .clearance = { -32, 0, -184, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                 // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq2 = {
        .clearance = { -64, 0, -128, 96, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                          // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq3 = {
        .clearance = { -32, -32, -184, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq4 = {
        .clearance = { 0, -32, -216, 56, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq5 = {
        .clearance = { 32, -32, -232, 40, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq6 = {
        .clearance = { 64, -32, -248, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq1 = {
        .clearance = { -32, 0, -184, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq2 = {
        .clearance = { -64, 0, -128, 96, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq3 = {
        .clearance = { -32, 32, -184, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq4 = {
        .clearance = { 0, 32, -216, 56, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq5 = {
        .clearance = { 32, 32, -232, 40, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq6 = {
        .clearance = { 64, 32, -248, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                 // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                 // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq4 = {
        .clearance = { -128, -32, 120, 96, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq5 = {
        .clearance = { -96, -32, 64, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq6 = {
        .clearance = { -64, -32, 248, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq4 = {
        .clearance = { -128, 32, 120, 96, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq5 = {
        .clearance = { -96, 32, 64, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq6 = {
        .clearance = { -64, 32, 248, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq0 = {
        .clearance = { 0, 0, -32, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq3 = {
        .clearance = { -32, -32, -216, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq4 = {
        .clearance = { 0, -32, -248, 56, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq5 = {
        .clearance = { 32, -32, -264, 40, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq6 = {
        .clearance = { 64, -32, -280, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq0 = {
        .clearance = { 0, 0, -32, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq3 = {
        .clearance = { -32, 32, -216, 192, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq4 = {
        .clearance = { 0, 32, -248, 56, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq5 = {
        .clearance = { 32, 32, -264, 40, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq6 = {
        .clearance = { 64, 32, -280, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0011, 0 }, {} },
        .allowedWallEdges = 0b1011,
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            EnumsToFlags(
                PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft,
                PS::bottomRight),   // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq3 = {
        .clearance = { -32, 0, 120, 32, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq0 = {
        .clearance = { 0, 0, -32, 32, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq1 = {
        .clearance = { -32, 0, -120, 96, { 0b0011, 0 }, {} },
        .allowedWallEdges = 0b1011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq2 = {
        .clearance = { 0, 0, -136, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq3 = {
        .clearance = { 32, 0, -152, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq6 = {
        .clearance = { 64, 32, -280, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftLargeHalfLoopUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0011, 0 }, {} },
        .allowedWallEdges = 0b1011,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::bottomLeft, PS::topLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::bottomLeft, PS::topLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlyerHalfLoopUninvertedUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq1 = {
        .clearance = { -32, 0, -88, 96, { 0b0011, 0 }, {} },
        .allowedWallEdges = 0b1011,
        .blockedSegments = kFlyerHalfLoopUninvertedUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq2 = {
        .clearance = { 0, 0, -104, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = kFlyerHalfLoopUninvertedUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq3 = {
        .clearance = { 32, 0, -120, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kFlyerHalfLoopUninvertedUpSeq0.blockedSegments,
    };

    constexpr auto kTEDLeftVerticalLoop = TrackElementDescriptor{
        .description = STR_VERTICAL_LOOP_LEFT,
        .coordinates = { 0, 0, 0, 0, -32, -32 },
        .pieceLength = 120,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 491520,
        .mirrorElement = TrackElemType::rightVerticalLoop,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::verticalLoop, TrackPitch::down25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, -48 },
        .verticalFactor = EvaluatorVerticalLoop,
        .sequenceData = { 10,
                          { kLeftVerticalLoopSeq0, kLeftVerticalLoopSeq1, kLeftVerticalLoopSeq2, kLeftVerticalLoopSeq3,
                            kLeftVerticalLoopSeq4, kLeftVerticalLoopSeq5, kLeftVerticalLoopSeq6, kLeftVerticalLoopSeq7,
                            kLeftVerticalLoopSeq8, kLeftVerticalLoopSeq9 } },
    };

    constexpr auto kTEDRightVerticalLoop = TrackElementDescriptor{
        .description = STR_VERTICAL_LOOP_RIGHT,
        .coordinates = { 0, 0, 0, 0, -32, 32 },
        .pieceLength = 120,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 491520,
        .mirrorElement = TrackElemType::leftVerticalLoop,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::verticalLoop, TrackPitch::down25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, -48 },
        .verticalFactor = EvaluatorVerticalLoop,
        .sequenceData = { 10,
                          { kRightVerticalLoopSeq0, kRightVerticalLoopSeq1, kRightVerticalLoopSeq2, kRightVerticalLoopSeq3,
                            kRightVerticalLoopSeq4, kRightVerticalLoopSeq5, kRightVerticalLoopSeq6, kRightVerticalLoopSeq7,
                            kRightVerticalLoopSeq8, kRightVerticalLoopSeq9 } },
    };

    constexpr auto kTEDHalfLoopUp = TrackElementDescriptor{
        .description = STR_HALF_LOOP,
        .coordinates = { 0, 2, 0, 152, -32, 0 },
        .pieceLength = 60,
        .curveChain = { TrackElemType::halfLoopDown, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::halfLoopUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoop, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none, 64 },
        .verticalFactor = EvaluatorHalfLoopUp,
        .sequenceData = { 4, { kHalfLoopUpSeq0, kHalfLoopUpSeq1, kHalfLoopUpSeq2, kHalfLoopUpSeq3 } },
    };

    constexpr auto kTEDHalfLoopDown = TrackElementDescriptor{
        .description = STR_HALF_LOOP,
        .coordinates = { 0, 2, 0, -152, 32, 0 },
        .pieceLength = 60,
        .curveChain = { TrackCurve::none, TrackElemType::halfLoopUp },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::halfLoopDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoop, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -64 },
        .verticalFactor = EvaluatorHalfLoopDown,
        .sequenceData = { 4, { kHalfLoopDownSeq0, kHalfLoopDownSeq1, kHalfLoopDownSeq2, kHalfLoopDownSeq3 } },
    };

    constexpr auto kTEDLeftLargeHalfLoopUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, 280, -64, -32 },
        .pieceLength = 100,
        .curveChain = { TrackElemType::rightLargeHalfLoopDown, TrackCurve::none },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::rightLargeHalfLoopUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopLarge, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none,
                        64 },
        .verticalFactor = EvaluatorLargeHalfLoopUp,
        .sequenceData = { 7,
                          { kLeftLargeHalfLoopUpSeq0, kLeftLargeHalfLoopUpSeq1, kLeftLargeHalfLoopUpSeq2,
                            kLeftLargeHalfLoopUpSeq3, kLeftLargeHalfLoopUpSeq4, kLeftLargeHalfLoopUpSeq5,
                            kLeftLargeHalfLoopUpSeq6 } },
    };

    constexpr auto kTEDRightLargeHalfLoopUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, 280, -64, 32 },
        .pieceLength = 100,
        .curveChain = { TrackElemType::leftLargeHalfLoopDown, TrackCurve::none },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::leftLargeHalfLoopUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopLarge, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none,
                        64 },
        .verticalFactor = EvaluatorLargeHalfLoopUp,
        .sequenceData = { 7,
                          { kRightLargeHalfLoopUpSeq0, kRightLargeHalfLoopUpSeq1, kRightLargeHalfLoopUpSeq2,
                            kRightLargeHalfLoopUpSeq3, kRightLargeHalfLoopUpSeq4, kRightLargeHalfLoopUpSeq5,
                            kRightLargeHalfLoopUpSeq6 } },
    };

    constexpr auto kTEDLeftLargeHalfLoopDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, -280, 64, -32 },
        .pieceLength = 100,
        .curveChain = { TrackCurve::none, TrackElemType::rightLargeHalfLoopUp },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::rightLargeHalfLoopDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopLarge, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -64 },
        .verticalFactor = EvaluatorLargeHalfLoopDown,
        .sequenceData = { 7,
                          { kLeftLargeHalfLoopDownSeq0, kLeftLargeHalfLoopDownSeq1, kLeftLargeHalfLoopDownSeq2,
                            kLeftLargeHalfLoopDownSeq3, kLeftLargeHalfLoopDownSeq4, kLeftLargeHalfLoopDownSeq5,
                            kLeftLargeHalfLoopDownSeq6 } },
    };

    constexpr auto kTEDRightLargeHalfLoopDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, -280, 64, 32 },
        .pieceLength = 100,
        .curveChain = { TrackCurve::none, TrackElemType::leftLargeHalfLoopUp },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::leftLargeHalfLoopDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopLarge, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -64 },
        .verticalFactor = EvaluatorLargeHalfLoopDown,
        .sequenceData = { 7,
                          { kRightLargeHalfLoopDownSeq0, kRightLargeHalfLoopDownSeq1, kRightLargeHalfLoopDownSeq2,
                            kRightLargeHalfLoopDownSeq3, kRightLargeHalfLoopDownSeq4, kRightLargeHalfLoopDownSeq5,
                            kRightLargeHalfLoopDownSeq6 } },
    };

    constexpr auto kTEDFlyerHalfLoopUninvertedUp = TrackElementDescriptor{
        .description = STR_HALF_LOOP,
        .coordinates = { 0, 2, 0, 120, -32, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::flyerHalfLoopUninvertedUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::flyingHalfLoopUninvertedUp, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown,
                        TrackRoll::none, 64 },
        .verticalFactor = EvaluatorHalfLoopUp,
        .sequenceData = { 4,
                          { kFlyerHalfLoopUninvertedUpSeq0, kFlyerHalfLoopUninvertedUpSeq1, kFlyerHalfLoopUninvertedUpSeq2,
                            kFlyerHalfLoopUninvertedUpSeq3 } },
    };

    constexpr auto kTEDFlyerHalfLoopInvertedDown = TrackElementDescriptor{
        .description = STR_HALF_LOOP,
        .coordinates = { 0, 2, 0, -120, 32, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::flyerHalfLoopInvertedDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::flyingHalfLoopInvertedDown, TrackPitch::down25, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, -64 },
        .verticalFactor = EvaluatorHalfLoopDown,
        .sequenceData = { 4,
                          { kFlyerHalfLoopInvertedDownSeq0, kFlyerHalfLoopInvertedDownSeq1, kFlyerHalfLoopInvertedDownSeq2,
                            kFlyerHalfLoopInvertedDownSeq3 } },
    };

    constexpr auto kTEDLeftMediumHalfLoopUp = TrackElementDescriptor{
        .description = STR_MEDIUM_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, 216, -32, -32 },
        .pieceLength = 80,
        .curveChain = { TrackElemType::rightMediumHalfLoopDown, TrackCurve::none },
        .priceModifier = 589824,
        .mirrorElement = TrackElemType::rightMediumHalfLoopUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopMedium, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none,
                        64 },
        .verticalFactor = EvaluatorMediumHalfLoopUp,
        .sequenceData = { 5,
                          { kLeftMediumHalfLoopUpSeq0, kLeftMediumHalfLoopUpSeq1, kLeftMediumHalfLoopUpSeq2,
                            kLeftMediumHalfLoopUpSeq3, kLeftMediumHalfLoopUpSeq4 } },
    };

    constexpr auto kTEDRightMediumHalfLoopUp = TrackElementDescriptor{
        .description = STR_MEDIUM_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, 216, -32, 32 },
        .pieceLength = 80,
        .curveChain = { TrackElemType::leftMediumHalfLoopDown, TrackCurve::none },
        .priceModifier = 589824,
        .mirrorElement = TrackElemType::leftMediumHalfLoopUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopMedium, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none,
                        64 },
        .verticalFactor = EvaluatorMediumHalfLoopUp,
        .sequenceData = { 5,
                          { kRightMediumHalfLoopUpSeq0, kRightMediumHalfLoopUpSeq1, kRightMediumHalfLoopUpSeq2,
                            kRightMediumHalfLoopUpSeq3, kRightMediumHalfLoopUpSeq4 } },
    };

    constexpr auto kTEDLeftMediumHalfLoopDown = TrackElementDescriptor{
        .description = STR_MEDIUM_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, -216, 32, -32 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackElemType::rightMediumHalfLoopUp },
        .priceModifier = 589824,
        .mirrorElement = TrackElemType::rightMediumHalfLoopDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopMedium, TrackPitch::down25, TrackPitch::none, TrackRoll::none,
                        TrackRoll::upsideDown, -64 },
        .verticalFactor = EvaluatorMediumHalfLoopDown,
        .sequenceData = { 5,
                          { kLeftMediumHalfLoopDownSeq0, kLeftMediumHalfLoopDownSeq1, kLeftMediumHalfLoopDownSeq2,
                            kLeftMediumHalfLoopDownSeq3, kLeftMediumHalfLoopDownSeq4 } },
    };

    constexpr auto kTEDRightMediumHalfLoopDown = TrackElementDescriptor{
        .description = STR_MEDIUM_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, -216, 32, 32 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackElemType::leftMediumHalfLoopUp },
        .priceModifier = 589824,
        .mirrorElement = TrackElemType::leftMediumHalfLoopDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopMedium, TrackPitch::down25, TrackPitch::none, TrackRoll::none,
                        TrackRoll::upsideDown, -64 },
        .verticalFactor = EvaluatorMediumHalfLoopDown,
        .sequenceData = { 5,
                          { kRightMediumHalfLoopDownSeq0, kRightMediumHalfLoopDownSeq1, kRightMediumHalfLoopDownSeq2,
                            kRightMediumHalfLoopDownSeq3, kRightMediumHalfLoopDownSeq4 } },
    };

    constexpr auto kTEDLeftFlyerLargeHalfLoopUninvertedUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, 248, -64, -32 },
        .pieceLength = 100,
        .curveChain = { TrackElemType::rightFlyerLargeHalfLoopInvertedDown, TrackCurve::none },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::rightFlyerLargeHalfLoopUninvertedUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flyingLargeHalfLoopUninvertedUp, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown,
                        TrackRoll::none, 64 },
        .verticalFactor = EvaluatorLargeHalfLoopUp,
        .sequenceData = { 7,
                          { kLeftFlyerLargeHalfLoopUninvertedUpSeq0, kLeftFlyerLargeHalfLoopUninvertedUpSeq1,
                            kLeftFlyerLargeHalfLoopUninvertedUpSeq2, kLeftFlyerLargeHalfLoopUninvertedUpSeq3,
                            kLeftFlyerLargeHalfLoopUninvertedUpSeq4, kLeftFlyerLargeHalfLoopUninvertedUpSeq5,
                            kLeftFlyerLargeHalfLoopUninvertedUpSeq6 } },
    };

    constexpr auto kTEDRightFlyerLargeHalfLoopUninvertedUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, 248, -64, 32 },
        .pieceLength = 100,
        .curveChain = { TrackElemType::leftFlyerLargeHalfLoopInvertedDown, TrackCurve::none },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::leftFlyerLargeHalfLoopUninvertedUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flyingLargeHalfLoopUninvertedUp, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown,
                        TrackRoll::none, 64 },
        .verticalFactor = EvaluatorLargeHalfLoopUp,
        .sequenceData = { 7,
                          { kRightFlyerLargeHalfLoopUninvertedUpSeq0, kRightFlyerLargeHalfLoopUninvertedUpSeq1,
                            kRightFlyerLargeHalfLoopUninvertedUpSeq2, kRightFlyerLargeHalfLoopUninvertedUpSeq3,
                            kRightFlyerLargeHalfLoopUninvertedUpSeq4, kRightFlyerLargeHalfLoopUninvertedUpSeq5,
                            kRightFlyerLargeHalfLoopUninvertedUpSeq6 } },
    };

    constexpr auto kTEDLeftFlyerLargeHalfLoopInvertedDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, -248, 64, -32 },
        .pieceLength = 100,
        .curveChain = { TrackCurve::none, TrackElemType::rightFlyerLargeHalfLoopUninvertedUp },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::rightFlyerLargeHalfLoopInvertedDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::flyingLargeHalfLoopInvertedDown, TrackPitch::down25, TrackPitch::none,
                        TrackRoll::upsideDown, TrackRoll::none, -64 },
        .verticalFactor = EvaluatorLargeHalfLoopDown,
        .sequenceData = { 7,
                          { kLeftFlyerLargeHalfLoopInvertedDownSeq0, kLeftFlyerLargeHalfLoopInvertedDownSeq1,
                            kLeftFlyerLargeHalfLoopInvertedDownSeq2, kLeftFlyerLargeHalfLoopInvertedDownSeq3,
                            kLeftFlyerLargeHalfLoopInvertedDownSeq4, kLeftFlyerLargeHalfLoopInvertedDownSeq5,
                            kLeftFlyerLargeHalfLoopInvertedDownSeq6 } },
    };

    constexpr auto kTEDRightFlyerLargeHalfLoopInvertedDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, -248, 64, 32 },
        .pieceLength = 100,
        .curveChain = { TrackCurve::none, TrackElemType::leftFlyerLargeHalfLoopUninvertedUp },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::leftFlyerLargeHalfLoopInvertedDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::flyingLargeHalfLoopInvertedDown, TrackPitch::down25, TrackPitch::none,
                        TrackRoll::upsideDown, TrackRoll::none, -64 },
        .verticalFactor = EvaluatorLargeHalfLoopDown,
        .sequenceData = { 7,
                          { kRightFlyerLargeHalfLoopInvertedDownSeq0, kRightFlyerLargeHalfLoopInvertedDownSeq1,
                            kRightFlyerLargeHalfLoopInvertedDownSeq2, kRightFlyerLargeHalfLoopInvertedDownSeq3,
                            kRightFlyerLargeHalfLoopInvertedDownSeq4, kRightFlyerLargeHalfLoopInvertedDownSeq5,
                            kRightFlyerLargeHalfLoopInvertedDownSeq6 } },
    };

    constexpr auto kTEDLeftFlyerLargeHalfLoopInvertedUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, 280, -64, -32 },
        .pieceLength = 100,
        .curveChain = { TrackElemType::rightFlyerLargeHalfLoopUninvertedDown, TrackCurve::none },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::rightFlyerLargeHalfLoopInvertedUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::inversionToNormal, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flyingLargeHalfLoopInvertedUp, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown,
                        TrackRoll::none, 64 },
        .verticalFactor = EvaluatorLargeHalfLoopUp,
        .sequenceData = { 7,
                          { kLeftFlyerLargeHalfLoopInvertedUpSeq0, kLeftFlyerLargeHalfLoopInvertedUpSeq1,
                            kLeftFlyerLargeHalfLoopInvertedUpSeq2, kLeftFlyerLargeHalfLoopInvertedUpSeq3,
                            kLeftFlyerLargeHalfLoopInvertedUpSeq4, kLeftFlyerLargeHalfLoopInvertedUpSeq5,
                            kLeftFlyerLargeHalfLoopInvertedUpSeq6 } },
    };

    constexpr auto kTEDRightFlyerLargeHalfLoopInvertedUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, 280, -64, 32 },
        .pieceLength = 100,
        .curveChain = { TrackElemType::leftFlyerLargeHalfLoopUninvertedDown, TrackCurve::none },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::leftFlyerLargeHalfLoopInvertedUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::inversionToNormal, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flyingLargeHalfLoopInvertedUp, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown,
                        TrackRoll::none, 64 },
        .verticalFactor = EvaluatorLargeHalfLoopUp,
        .sequenceData = { 7,
                          { kRightFlyerLargeHalfLoopInvertedUpSeq0, kRightFlyerLargeHalfLoopInvertedUpSeq1,
                            kRightFlyerLargeHalfLoopInvertedUpSeq2, kRightFlyerLargeHalfLoopInvertedUpSeq3,
                            kRightFlyerLargeHalfLoopInvertedUpSeq4, kRightFlyerLargeHalfLoopInvertedUpSeq5,
                            kRightFlyerLargeHalfLoopInvertedUpSeq6 } },
    };

    constexpr auto kTEDLeftFlyerLargeHalfLoopUninvertedDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, -280, 64, -32 },
        .pieceLength = 100,
        .curveChain = { TrackCurve::none, TrackElemType::rightFlyerLargeHalfLoopInvertedUp },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::rightFlyerLargeHalfLoopUninvertedDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::normalToInversion },
        .definition = { TrackGroup::flyingLargeHalfLoopUninvertedDown, TrackPitch::down25, TrackPitch::none,
                        TrackRoll::upsideDown, TrackRoll::none, -64 },
        .verticalFactor = EvaluatorLargeHalfLoopDown,
        .sequenceData = { 7,
                          { kLeftFlyerLargeHalfLoopUninvertedDownSeq0, kLeftFlyerLargeHalfLoopUninvertedDownSeq1,
                            kLeftFlyerLargeHalfLoopUninvertedDownSeq2, kLeftFlyerLargeHalfLoopUninvertedDownSeq3,
                            kLeftFlyerLargeHalfLoopUninvertedDownSeq4, kLeftFlyerLargeHalfLoopUninvertedDownSeq5,
                            kLeftFlyerLargeHalfLoopUninvertedDownSeq6 } },
    };

    constexpr auto kTEDRightFlyerLargeHalfLoopUninvertedDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, -280, 64, 32 },
        .pieceLength = 100,
        .curveChain = { TrackCurve::none, TrackElemType::leftFlyerLargeHalfLoopInvertedUp },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::leftFlyerLargeHalfLoopUninvertedDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::normalToInversion },
        .definition = { TrackGroup::flyingLargeHalfLoopUninvertedDown, TrackPitch::down25, TrackPitch::none,
                        TrackRoll::upsideDown, TrackRoll::none, -64 },
        .verticalFactor = EvaluatorLargeHalfLoopDown,
        .sequenceData = { 7,
                          { kRightFlyerLargeHalfLoopUninvertedDownSeq0, kRightFlyerLargeHalfLoopUninvertedDownSeq1,
                            kRightFlyerLargeHalfLoopUninvertedDownSeq2, kRightFlyerLargeHalfLoopUninvertedDownSeq3,
                            kRightFlyerLargeHalfLoopUninvertedDownSeq4, kRightFlyerLargeHalfLoopUninvertedDownSeq5,
                            kRightFlyerLargeHalfLoopUninvertedDownSeq6 } },
    };

    constexpr auto kTEDFlyerHalfLoopInvertedUp = TrackElementDescriptor{
        .description = STR_HALF_LOOP,
        .coordinates = { 0, 2, 0, 152, -32, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::flyerHalfLoopInvertedUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::inversionToNormal, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flyingHalfLoopInvertedUp, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown,
                        TrackRoll::none, 64 },
        .verticalFactor = EvaluatorHalfLoopUp,
        .sequenceData = { 4,
                          { kFlyerHalfLoopInvertedUpSeq0, kFlyerHalfLoopInvertedUpSeq1, kFlyerHalfLoopInvertedUpSeq2,
                            kFlyerHalfLoopInvertedUpSeq3 } },
    };

    constexpr auto kTEDFlyerHalfLoopUninvertedDown = TrackElementDescriptor{
        .description = STR_HALF_LOOP,
        .coordinates = { 0, 2, 0, -152, 32, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::flyerHalfLoopUninvertedDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::normalToInversion },
        .definition = { TrackGroup::flyingHalfLoopUninvertedDown, TrackPitch::down25, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, -64 },
        .verticalFactor = EvaluatorHalfLoopDown,
        .sequenceData = { 4,
                          { kFlyerHalfLoopUninvertedDownSeq0, kFlyerHalfLoopUninvertedDownSeq1,
                            kFlyerHalfLoopUninvertedDownSeq2, kFlyerHalfLoopUninvertedDownSeq3 } },
    };

} // namespace OpenRCT2::TrackMetadata
