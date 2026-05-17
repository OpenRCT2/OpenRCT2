/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackData.h"

#include "../core/EnumUtils.hpp"
#include "../localisation/StringIds.h"
#include "TrackPaint.h"
#include "ted/TED.Corkscrew.h"
#include "ted/TED.DiveLoop.h"
#include "ted/TED.FlatRide.h"
#include "ted/TED.HalfHelix.h"
#include "ted/TED.Heartline.h"
#include "ted/TED.LongBase.h"
#include "ted/TED.Loop.h"
#include "ted/TED.Maze.h"
#include "ted/TED.MiniGolf.h"
#include "ted/TED.SBend.h"
#include "ted/TED.Station.h"
#include "ted/TED.Water.h"
#include "ted/TED.ZeroG.h"
#include "ted/TrackElementDescriptor.h"

#include <cstdint>

using namespace OpenRCT2;

namespace OpenRCT2::TrackMetadata
{
#pragma region GForces

    static int32_t EvaluatorQuarterTurn3Tiles(const int16_t progress)
    {
        return -(progress / 2) + 134;
    }

    static int32_t EvaluatorDown90QuarterLoop(const int16_t progress)
    {
        return (progress / 4) + 55;
    }

    static int32_t EvaluatorUp90QuarterLoop(const int16_t progress)
    {
        return ((static_cast<uint16_t>(-(progress - 137))) / 4) + 55;
    }

#pragma endregion

#pragma region trackBlocks

    using PS = PaintSegment;

    static constexpr SequenceDescriptor kFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp25ToUp60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToUp60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown25ToDown60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToUp60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::null },
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsNone,                                          // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::null },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsNone,                                          // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight)),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::null },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::null },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlatToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kFlatToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kFlatToLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::null },
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::null },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::null },
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::null },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kUp25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kUp25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kUp25ToLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kDown25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kDown25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kDown25ToLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b0111, 0b0100 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq4 = {
        .clearance = { -32, -64, 48, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq5 = {
        .clearance = { -64, -32, 32, 16, { 0b0111, 0b0100 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq6 = {
        .clearance = { -64, -64, 48, 16, { 0b1111, 0b0110 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b1011, 0b1000 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq4 = {
        .clearance = { -32, 64, 48, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq5 = {
        .clearance = { -64, 32, 32, 16, { 0b1011, 0b1000 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq6 = {
        .clearance = { -64, 64, 48, 16, { 0b1111, 0b1001 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq1 = {
        .clearance = { 0, -32, 48, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b0111, 0b0001 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq4 = {
        .clearance = { -32, -64, 16, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq5 = {
        .clearance = { -64, -32, 16, 16, { 0b0111, 0b0001 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq6 = {
        .clearance = { -64, -64, 0, 16, { 0b1111, 0b1001 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq1 = {
        .clearance = { 0, 32, 48, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b1011, 0b0010 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq4 = {
        .clearance = { -32, 64, 16, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq5 = {
        .clearance = { -64, 32, 16, 16, { 0b1011, 0b0010 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq6 = {
        .clearance = { -64, 64, 0, 16, { 0b1111, 0b0110 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsAllTypes(EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsAllTypes(EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TilesSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TilesSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TilesSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TilesSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b0111, 0b0100 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsNone,                                                   // narrow
            kSegmentsNone,                                                   // inverted
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, -32, 16, 16, { 0b0111, 0b0110 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1011, 0b1000 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1011, 0b1001 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b0111, 0b0011 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn3TilesUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn3TilesUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn3TilesUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq3 = {
        .clearance = { -32, -32, 0, 16, { 0b0111, 0b0001 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn3TilesUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b1011, 0b0011 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn3TilesUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn3TilesUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn3TilesUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b1011, 0b0010 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn3TilesUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn1TileSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftTwistDownToUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft),         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftTwistDownToUpSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft),         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftTwistDownToUpSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft),         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightTwistDownToUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistDownToUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightTwistDownToUpSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistDownToUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightTwistDownToUpSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistDownToUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftTwistUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftTwistDownToUpSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftTwistUpToDownSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsRotate(kLeftTwistDownToUpSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftTwistUpToDownSeq2 = {
        .clearance = { -64, 0, -16, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsRotate(kLeftTwistDownToUpSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightTwistUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistUpToDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightTwistUpToDownSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistUpToDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightTwistUpToDownSeq2 = {
        .clearance = { -64, 0, -16, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistUpToDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlatToUp60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = -2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kTowerBaseSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq1 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq4 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq5 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq6 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq7 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq8 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerSectionSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
    };

    static constexpr SequenceDescriptor kTowerSectionSeq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, { ClearanceFlag::flag0, ClearanceFlag::flag1 } },
    };

    static constexpr SequenceDescriptor kFlatCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp60Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatToUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25ToUp60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp25ToUp60Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToUp25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp60ToUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25ToFlatCoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp25ToFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown60Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatToDown25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25ToDown60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown25ToDown60Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToDown25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown60ToDown25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25ToFlatCoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown25ToFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, {} },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, {} },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn3TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn3TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn3TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn3TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn3TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn3TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn3TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn3TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kBoosterSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::topLeft, PS::left), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight),       // inverted
            kSegmentsAll,                                                                           // wide
        } },
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeUpSeq0.clearance, 0, false),
        .tunnels = {
            SequenceTunnel{
                .direction = 0,
                .tunnelInfo = {
                    SequenceTunnelInfo{ 0, TunnelSubType::Flat }, // upright
                    SequenceTunnelInfo{ 0, TunnelSubType::Flat }, // inverted
                    SequenceTunnelInfo{ 0, TunnelSubType::Tall }, // invertedFlying
                },
            },
        },
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq1 = {
        .clearance = { 0, -32, 0, 8, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq1.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeUpSeq1.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            EnumsToFlags(
                PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottom, PS::bottomRight), // wide
        } },
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeUpSeq2.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq3 = {
        .clearance = { -32, -32, 0, 16, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight, PS::bottom, PS::top), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight),                      // inverted
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeUpSeq3.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq4 = {
        .clearance = { -32, -64, 0, 16, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .extraSupportRotation = -1,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq4.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeUpSeq4.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq5 = {
        .clearance = { -64, -32, 0, 16, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // inverted
            EnumsToFlags(
                PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight, PS::top, PS::topRight), // wide
        } },
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeUpSeq5.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq6 = {
        .clearance = { -64, -64, 0, 16, { 0b1111, 0b0110 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25even },
        .metalSupports = { MetalSupportPlace::centre, false, kCoordsZStep },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight, PS::bottomLeft, PS::left), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight),             // inverted
            kSegmentsAll,                                                                                 // wide
        } },
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeUpSeq6.clearance, 0, false),
        .tunnels = {
            SequenceTunnel{
                .direction = 3,
                .tunnelInfo = {
                    SequenceTunnelInfo{ kLandHeightStep, TunnelSubType::FlatTo25Deg }, // upright
                    SequenceTunnelInfo{ kLandHeightStep, TunnelSubType::Flat },        // inverted
                    SequenceTunnelInfo{ kLandHeightStep, TunnelSubType::Tall },        // invertedFlying
                },
            },
        },
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterBankedHelixLargeUpSeq0.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeUpSeq0.clearance, 0, false),
        .tunnels = sequenceTunnelsModify<DirectionFlipXAxis>(kLeftQuarterBankedHelixLargeUpSeq0.tunnels),
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterBankedHelixLargeUpSeq1.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeUpSeq1.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterBankedHelixLargeUpSeq2.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeUpSeq2.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterBankedHelixLargeUpSeq3.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeUpSeq3.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq4 = {
        .clearance = { -32, 64, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterBankedHelixLargeUpSeq4.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeUpSeq4.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq5 = {
        .clearance = { -64, 32, 0, 16, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterBankedHelixLargeUpSeq5.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeUpSeq5.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq6 = {
        .clearance = { -64, 64, 0, 16, { 0b1111, 0b1001 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25even },
        .metalSupports = { MetalSupportPlace::centre, false, kCoordsZStep },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterBankedHelixLargeUpSeq6.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeUpSeq6.clearance, 0, false),
        .tunnels = sequenceTunnelsModify<DirectionFlipXAxis>(kLeftQuarterBankedHelixLargeUpSeq6.tunnels),
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25even },
        .metalSupports = { MetalSupportPlace::centre, false, kCoordsZStep },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq0.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeDownSeq0.clearance, 0, false),
        .tunnels = sequenceTunnelsModify<DirectionPrev>(kRightQuarterBankedHelixLargeUpSeq6.tunnels),
        .reversedTrackSequence = 6,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq1 = {
        .clearance = { 0, -32, 0, 16, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq1.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeDownSeq1.clearance, 0, false),
        .reversedTrackSequence = 4,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq2.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeDownSeq2.clearance, 0, false),
        .reversedTrackSequence = 5,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq3 = {
        .clearance = { -32, -32, 0, 16, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq3.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeDownSeq3.clearance, 0, false),
        .reversedTrackSequence = 3,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq4 = {
        .clearance = { -32, -64, 0, 8, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq4.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeDownSeq4.clearance, 0, false),
        .reversedTrackSequence = 1,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq5 = {
        .clearance = { -64, -32, 0, 8, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq5.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeDownSeq5.clearance, 0, false),
        .reversedTrackSequence = 2,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq6 = {
        .clearance = { -64, -64, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq6.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterBankedHelixLargeDownSeq6.clearance, 0, false),
        .tunnels = sequenceTunnelsModify<DirectionPrev>(kRightQuarterBankedHelixLargeUpSeq0.tunnels),
        .reversedTrackSequence = 0,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25even },
        .metalSupports = { MetalSupportPlace::centre, false, kCoordsZStep },
        .extraSupportRotation = 2,
        .blockedSegments = kRightQuarterBankedHelixLargeUpSeq0.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeDownSeq0.clearance, 0, false),
        .tunnels = sequenceTunnelsModify<DirectionFlipXAxis>(kLeftQuarterBankedHelixLargeDownSeq0.tunnels),
        .reversedTrackSequence = 6,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterBankedHelixLargeUpSeq1.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeDownSeq1.clearance, 0, false),
        .reversedTrackSequence = 4,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightQuarterBankedHelixLargeUpSeq2.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeDownSeq2.clearance, 0, false),
        .reversedTrackSequence = 5,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightQuarterBankedHelixLargeUpSeq3.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeDownSeq3.clearance, 0, false),
        .reversedTrackSequence = 3,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq4 = {
        .clearance = { -32, 64, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterBankedHelixLargeUpSeq4.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeDownSeq4.clearance, 0, false),
        .reversedTrackSequence = 1,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq5 = {
        .clearance = { -64, 32, 0, 8, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterBankedHelixLargeUpSeq5.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeDownSeq5.clearance, 0, false),
        .reversedTrackSequence = 2,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq6 = {
        .clearance = { -64, 64, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterBankedHelixLargeUpSeq6.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterBankedHelixLargeDownSeq6.clearance, 0, false),
        .tunnels = sequenceTunnelsModify<DirectionFlipXAxis>(kLeftQuarterBankedHelixLargeDownSeq6.tunnels),
        .reversedTrackSequence = 0,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::topLeft),     // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeUpSeq0.clearance, 0, false),
        .tunnels = kLeftQuarterBankedHelixLargeUpSeq0.tunnels,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq1 = {
        .clearance = { 0, -32, 0, 8, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq1.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeUpSeq1.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq2.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeUpSeq2.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq3 = {
        .clearance = { -32, -32, 0, 16, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq3.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeUpSeq3.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq4 = {
        .clearance = { -32, -64, 0, 16, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq4.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeUpSeq4.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq5 = {
        .clearance = { -64, -32, 0, 16, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq5.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeUpSeq5.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq6 = {
        .clearance = { -64, -64, 0, 16, { 0b1111, 0b0110 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25even },
        .metalSupports = { MetalSupportPlace::centre, false, kCoordsZStep },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight),   // inverted
            kSegmentsAll,                                                                       // wide
        } },
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeUpSeq6.clearance, 0, false),
        .tunnels = kLeftQuarterBankedHelixLargeUpSeq6.tunnels,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterHelixLargeUpSeq0.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeUpSeq0.clearance, 0, false),
        .tunnels = kRightQuarterBankedHelixLargeUpSeq0.tunnels,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterHelixLargeUpSeq1.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeUpSeq1.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterHelixLargeUpSeq2.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeUpSeq2.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterHelixLargeUpSeq3.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeUpSeq3.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq4 = {
        .clearance = { -32, 64, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterHelixLargeUpSeq4.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeUpSeq4.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq5 = {
        .clearance = { -64, 32, 0, 16, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterHelixLargeUpSeq5.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeUpSeq5.clearance, 0, false),
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq6 = {
        .clearance = { -64, 64, 0, 16, { 0b1111, 0b1001 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25even },
        .metalSupports = { MetalSupportPlace::centre, false, kCoordsZStep },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterHelixLargeUpSeq6.blockedSegments),
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeUpSeq6.clearance, 0, false),
        .tunnels = kRightQuarterBankedHelixLargeUpSeq6.tunnels,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25even },
        .metalSupports = { MetalSupportPlace::centre, false, kCoordsZStep },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftQuarterHelixLargeUpSeq0.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeDownSeq0.clearance, 0, false),
        .tunnels = kLeftQuarterBankedHelixLargeDownSeq0.tunnels,
        .reversedTrackSequence = 6,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq1 = {
        .clearance = { 0, -32, 0, 16, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterHelixLargeUpSeq1.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeDownSeq1.clearance, 0, false),
        .reversedTrackSequence = 4,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftQuarterHelixLargeUpSeq2.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeDownSeq2.clearance, 0, false),
        .reversedTrackSequence = 5,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq3 = {
        .clearance = { -32, -32, 0, 16, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftQuarterBankedHelixLargeUpSeq3.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeDownSeq3.clearance, 0, false),
        .reversedTrackSequence = 3,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq4 = {
        .clearance = { -32, -64, 0, 8, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterHelixLargeUpSeq4.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeDownSeq4.clearance, 0, false),
        .reversedTrackSequence = 1,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq5 = {
        .clearance = { -64, -32, 0, 8, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterHelixLargeUpSeq5.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeDownSeq5.clearance, 0, false),
        .reversedTrackSequence = 2,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq6 = {
        .clearance = { -64, -64, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterHelixLargeUpSeq6.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kLeftQuarterHelixLargeDownSeq6.clearance, 0, false),
        .tunnels = kLeftQuarterBankedHelixLargeDownSeq6.tunnels,
        .reversedTrackSequence = 0,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25even },
        .metalSupports = { MetalSupportPlace::centre, false, kCoordsZStep },
        .extraSupportRotation = 2,
        .blockedSegments = kRightQuarterHelixLargeUpSeq0.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeDownSeq0.clearance, 0, false),
        .tunnels = kRightQuarterBankedHelixLargeDownSeq0.tunnels,
        .reversedTrackSequence = 6,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterHelixLargeUpSeq1.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeDownSeq1.clearance, 0, false),
        .reversedTrackSequence = 4,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightQuarterHelixLargeUpSeq2.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeDownSeq2.clearance, 0, false),
        .reversedTrackSequence = 5,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightQuarterBankedHelixLargeUpSeq3.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeDownSeq3.clearance, 0, false),
        .reversedTrackSequence = 3,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq4 = {
        .clearance = { -32, 64, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterHelixLargeUpSeq4.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeDownSeq4.clearance, 0, false),
        .reversedTrackSequence = 1,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq5 = {
        .clearance = { -64, 32, 0, 8, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterHelixLargeUpSeq5.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeDownSeq5.clearance, 0, false),
        .reversedTrackSequence = 2,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq6 = {
        .clearance = { -64, 64, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterHelixLargeUpSeq6.blockedSegments,
        .generalSupportHeight = calculateGeneralSupportHeight(kRightQuarterHelixLargeDownSeq6.clearance, 0, false),
        .tunnels = kRightQuarterBankedHelixLargeDownSeq6.tunnels,
        .reversedTrackSequence = 0,
    };

    static constexpr SequenceDescriptor kUp25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kOnRidePhotoSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kDown25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq1 = {
        .clearance = { -32, 0, 0, 8, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq2 = {
        .clearance = { -64, 0, -32, 32, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq3 = {
        .clearance = { -96, 0, -96, 64, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq1 = {
        .clearance = { -32, 0, 0, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq2 = {
        .clearance = { -64, 0, 0, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq3 = {
        .clearance = { -96, 0, 0, 80, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq4 = {
        .clearance = { -128, 0, 0, 160, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq5 = {
        .clearance = { -192, 0, 0, 208, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq6 = {
        .clearance = { -160, 0, 0, 208, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallVerticalSeq0 = {
        .clearance = { 0, 0, 0, 48, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallVerticalSeq1 = {
        .clearance = { 32, 0, 0, 48, { 0b1111, 0 }, {} },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp90Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, { ClearanceFlag::flag0, ClearanceFlag::isVertical } },
    };

    static constexpr SequenceDescriptor kDown90Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, { ClearanceFlag::flag0, ClearanceFlag::isVertical } },
    };

    static constexpr SequenceDescriptor kUp60ToUp90Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToUp90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, { ClearanceFlag::flag0, ClearanceFlag::isVertical } },
    };

    static constexpr SequenceDescriptor kDown90ToDown60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1110,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kUp90ToUp60Seq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToDown90Seq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1110,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToDown90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, { ClearanceFlag::flag0, ClearanceFlag::isVertical } },
    };

    static constexpr SequenceDescriptor kBrakeForDropSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1000, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0010, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq4 = {
        .clearance = { -64, -32, 0, 0, { 0b0001, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::bottomCorner },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::leftCorner },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq4.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq2 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq4.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq3.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                            // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1000, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0010, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq4 = {
        .clearance = { -64, -32, 0, 0, { 0b0001, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(
                PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // inverted
            kSegmentsAll,         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq4.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq2 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq4.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq3.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kDiagFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq1 = {
        .clearance = { 0, 32, 0, 64, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq2 = {
        .clearance = { -32, 0, 0, 64, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq3 = {
        .clearance = { -32, 32, 0, 64, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, {} },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60Seq1 = {
        .clearance = { 0, 32, 0, 64, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60Seq2 = {
        .clearance = { -32, 0, 0, 64, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60Seq3 = {
        .clearance = { -32, 32, 0, 64, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToFlatSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToFlatSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToFlatSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToFlatSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, {} },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, {} },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToUp60Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToUp60Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToUp60Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToUp60Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, {} },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, {} },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),             // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),          // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),             // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),          // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),             // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),          // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kLogFlumeReverserSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kSpinningTunnelSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::disallowDoors, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollUpToDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollUpToDownSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                              // inverted
            kSegmentsAll,                                                                        // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBarrelRollUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollUpToDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBarrelRollUpToDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollUpToDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBarrelRollUpToDownSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollUpToDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBarrelRollDownToUpSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftBarrelRollUpToDownSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftBarrelRollDownToUpSeq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftBarrelRollUpToDownSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftBarrelRollDownToUpSeq2 = {
        .clearance = { -64, 0, -32, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftBarrelRollUpToDownSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightBarrelRollDownToUpSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollDownToUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBarrelRollDownToUpSeq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollDownToUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBarrelRollDownToUpSeq2 = {
        .clearance = { -64, 0, -32, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollDownToUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0b0100 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, -32, 0, 16, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsNone,          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsNone,                                                   // narrow
            kSegmentsUnimplemented,                                          // inverted
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, -32, 16, 16, { 0b0111, 0b0110 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1011, 0b1000 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToLeftQuarterTurn3TilesUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToLeftQuarterTurn3TilesUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToLeftQuarterTurn3TilesUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1011, 0b1001 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToLeftQuarterTurn3TilesUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 16, 16, { 0b0111, 0b0011 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsRotate(kRightBankToRightQuarterTurn3TilesUp25Seq3.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq1 = {
        .clearance = { 0, -32, 0, 16, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightBankToRightQuarterTurn3TilesUp25Seq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightBankToRightQuarterTurn3TilesUp25Seq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0b0001 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsRotate(kRightBankToRightQuarterTurn3TilesUp25Seq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq0 = {
        .clearance = { 0, 0, 16, 16, { 0b1011, 0b0011 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesDown25ToLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesDown25ToLeftBankSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesDown25ToLeftBankSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0b0010 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesDown25ToLeftBankSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kPoweredLiftSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistUpSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistUpSeq2 = {
        .clearance = { -64, 0, -16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerTwistUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistUpSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistUpSeq2 = {
        .clearance = { -64, 0, -16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftFlyerTwistUpSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsRotate(kLeftFlyerTwistUpSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistDownSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftFlyerTwistUpSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistDownSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq1 = {
        .clearance = { -32, 0, -40, 32, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq2 = {
        .clearance = { -64, 0, -96, 56, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq3 = {
        .clearance = { -96, 0, -96, 0, { 0b0000, 0 }, { ClearanceFlag::flag0, ClearanceFlag::isVertical } },
    };

    static constexpr SequenceDescriptor kUp90ToInvertedFlatQuarterLoopSeq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kUp90ToInvertedFlatQuarterLoopSeq1 = {
        .clearance = { 32, 0, 56, 32, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kUp90ToInvertedFlatQuarterLoopSeq2 = {
        .clearance = { 64, 0, 96, 16, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq1 = {
        .clearance = { -32, 0, -72, 32, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq2 = {
        .clearance = { -64, 0, -128, 56, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq3 = {
        .clearance = { -96, 0, -128, 0, { 0b0000, 0 }, { ClearanceFlag::flag0, ClearanceFlag::isVertical } },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsNone,          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq3 = {
        .clearance = { -32, -32, 0, 8, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCurvedLiftHillSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCurvedLiftHillSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCurvedLiftHillSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCurvedLiftHillSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftReverserSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq3 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq4 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq5 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightReverserSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq4 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq5 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq1 = {
        .clearance = { 32, 0, 0, 32, { 0b1111, 0 }, {} },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq2 = {
        .clearance = { -64, 0, 0, 32, { 0b1111, 0 }, {} },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq3 = {
        .clearance = { -32, 0, 0, 32, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownSeq0 = {
        .clearance = { 0, 0, 0, 48, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownSeq1 = {
        .clearance = { 32, 0, 0, 48, { 0b1111, 0 }, {} },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq0 = {
        .clearance = { 0, 0, 0, 208, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq1 = {
        .clearance = { 32, 0, 0, 208, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq2 = {
        .clearance = { -32, 0, 0, 160, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq3 = {
        .clearance = { -64, 0, 0, 80, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq4 = {
        .clearance = { -96, 0, 0, 48, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq5 = {
        .clearance = { -128, 0, 0, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq6 = {
        .clearance = { -160, 0, 0, 16, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kBlockBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b0111, 0b0100 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsNone,                                                   // narrow
            kSegmentsNone,                                                   // inverted
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq3 = {
        .clearance = { -32, -32, 16, 16, { 0b0111, 0b0110 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1011, 0b1000 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1011, 0b1001 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b0111, 0b0011 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn3TileUp25Seq3.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn3TileUp25Seq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn3TileUp25Seq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq3 = {
        .clearance = { -32, -32, 0, 16, { 0b0111, 0b0001 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn3TileUp25Seq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b1011, 0b0011 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileDown25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileDown25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b1011, 0b0010 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileDown25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsNone,                                          // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b0111, 0b0100 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq4 = {
        .clearance = { -32, -64, 48, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsNone,                                          // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq5 = {
        .clearance = { -64, -32, 32, 16, { 0b0111, 0b0100 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight)),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq6 = {
        .clearance = { -64, -64, 48, 16, { 0b1111, 0b0110 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b1011, 0b1000 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq4 = {
        .clearance = { -32, 64, 48, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq5 = {
        .clearance = { -64, 32, 32, 16, { 0b1011, 0b1000 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq6 = {
        .clearance = { -64, 64, 48, 16, { 0b1111, 0b1001 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq6.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq1 = {
        .clearance = { 0, -32, 48, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq4.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b0111, 0b0001 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq5.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq3.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq4 = {
        .clearance = { -32, -64, 16, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq5 = {
        .clearance = { -64, -32, 16, 16, { 0b0111, 0b0001 }, {} },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq6 = {
        .clearance = { -64, -64, 0, 16, { 0b1111, 0b1001 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq1 = {
        .clearance = { 0, 32, 48, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b1011, 0b0010 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq4 = {
        .clearance = { -32, 64, 16, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq5 = {
        .clearance = { -64, 32, 16, 16, { 0b1011, 0b0010 }, {} },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq6 = {
        .clearance = { -64, 64, 0, 16, { 0b1111, 0b0110 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kUp25ToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kUp25ToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kUp25ToLeftBankedUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedUp25ToUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kDown25ToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kDown25ToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kDown25ToLeftBankedDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedDown25ToDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedFlatToLeftBankedUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedUp25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedUp25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedUp25ToLeftBankedFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedFlatToLeftBankedDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedDown25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedDown25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedDown25ToLeftBankedFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kFlatToLeftBankedUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedUp25ToFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kFlatToLeftBankedDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedDown25ToFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileUp90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileUp90Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0000, 0 }, { ClearanceFlag::flag0, ClearanceFlag::isVertical } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileUp90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn1TileUp90Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileUp90Seq1 = {
        .clearance = { 0, -32, 0, 0, { 0b0000, 0 }, { ClearanceFlag::flag0, ClearanceFlag::isVertical } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileDown90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::centre, PS::topLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileDown90Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0000, 0 }, { ClearanceFlag::flag0, ClearanceFlag::isVertical } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileDown90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn1TileDown90Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileDown90Seq1 = {
        .clearance = { 0, -32, 0, 0, { 0b0000, 0 }, { ClearanceFlag::flag0, ClearanceFlag::isVertical } },
    };

    static constexpr SequenceDescriptor kMultiDimUp90ToInvertedFlatQuarterLoopSeq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimUp90ToInvertedFlatQuarterLoopSeq1 = {
        .clearance = { 32, 0, 56, 32, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 = {
        .clearance = { 64, 0, 96, 16, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq1 = {
        .clearance = { -32, 0, -72, 32, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq2 = {
        .clearance = { -64, 0, -128, 56, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq3 = {
        .clearance = { -96, 0, -128, 0, { 0b0000, 0 }, { ClearanceFlag::flag0, ClearanceFlag::isVertical } },
    };

    static constexpr SequenceDescriptor kMultiDimInvertedUp90ToFlatQuarterLoopSeq0 = {
        .clearance = { 0, 0, 32, 56, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedUp90ToFlatQuarterLoopSeq1 = {
        .clearance = { 32, 0, 88, 32, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 = {
        .clearance = { 64, 0, 128, 16, { 0b1111, 0 }, { ClearanceFlag::isVertical } },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kRotationControlToggleSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0b1100 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq2 = {
        .clearance = { -32, -32, 32, 16, { 0b1000, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                         // inverted
            kSegmentsAll,                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0010, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq4 = {
        .clearance = { -64, -32, 32, 24, { 0b0001, 0 }, {} },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                           // inverted
            kSegmentsAll,                                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq2 = {
        .clearance = { -32, 32, 32, 16, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq4 = {
        .clearance = { -64, 32, 32, 24, { 0b0010, 0 }, {} },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftEighthToDiagUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthToDiagUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq2 = {
        .clearance = { -32, -32, -48, 16, { 0b1000, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftEighthToDiagUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0010, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftEighthToDiagUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq4 = {
        .clearance = { -64, -32, -48, 16, { 0b0001, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftEighthToDiagUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq2 = {
        .clearance = { -32, 32, -48, 16, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq4.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq1 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq2 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq4 = {
        .clearance = { -64, 32, 32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq4.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq3.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq4 = {
        .clearance = { -32, 64, 32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq1 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq2 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kRightEighthToOrthogonalUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq1 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kRightEighthToOrthogonalUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq2 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kRightEighthToOrthogonalUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kRightEighthToOrthogonalUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq4 = {
        .clearance = { -32, 64, -48, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightEighthToOrthogonalUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = kLeftEighthToDiagUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthToDiagUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq2 = {
        .clearance = { -32, -32, 32, 16, { 0b1000, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftEighthToDiagUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0010, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftEighthToDiagUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq4 = {
        .clearance = { -64, -32, 32, 24, { 0b0001, 0 }, {} },
        .blockedSegments = kLeftEighthToDiagUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq2 = {
        .clearance = { -32, 32, 32, 16, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq4 = {
        .clearance = { -64, 32, 32, 24, { 0b0010, 0 }, {} },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftEighthBankToDiagUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthBankToDiagUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq2 = {
        .clearance = { -32, -32, -48, 16, { 0b1000, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftEighthBankToDiagUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0010, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftEighthBankToDiagUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq4 = {
        .clearance = { -64, -32, -48, 16, { 0b0001, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftEighthBankToDiagUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kRightEighthBankToDiagUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kRightEighthBankToDiagUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq2 = {
        .clearance = { -32, 32, -48, 16, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightEighthBankToDiagUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightEighthBankToDiagUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b0010, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightEighthBankToDiagUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq4.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq1 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq2 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq4 = {
        .clearance = { -64, 32, 32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq4.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq3.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq4 = {
        .clearance = { -32, 64, 32, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq1 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq2 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq1 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, {} },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq2 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq4 = {
        .clearance = { -32, 64, -48, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, {} },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    constexpr auto kTEDFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::flatCovered,
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::flat,
        .flags = { TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::straight, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatSeq0 } },
    };

    constexpr auto kTEDUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::up25Covered,
        .priceModifier = 79872,
        .mirrorElement = TrackElemType::up25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slope, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kUp25Seq0 } },
    };

    constexpr auto kTEDUp60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 64, 0, 0 },
        .pieceLength = 40,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::up60Covered,
        .priceModifier = 114688,
        .mirrorElement = TrackElemType::up60,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slopeSteepUp, TrackPitch::up60, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kUp60Seq0 } },
    };

    constexpr auto kTEDFlatToUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::flatToUp25Covered,
        .priceModifier = 73728,
        .mirrorElement = TrackElemType::flatToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slope, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kFlatToUp25Seq0 } },
    };

    constexpr auto kTEDUp25ToUp60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 32, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::up25ToUp60Covered,
        .priceModifier = 96256,
        .mirrorElement = TrackElemType::up25ToUp60,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slopeSteepUp, TrackPitch::up60, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<82>,
        .sequenceData = { 1, { kUp25ToUp60Seq0 } },
    };

    constexpr auto kTEDUp60ToUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 32, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::up60ToUp25Covered,
        .priceModifier = 96256,
        .mirrorElement = TrackElemType::up60ToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slopeSteepUp, TrackPitch::up25, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-82>,
        .sequenceData = { 1, { kUp60ToUp25Seq0 } },
    };

    constexpr auto kTEDUp25ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::up25ToFlatCovered,
        .priceModifier = 73728,
        .mirrorElement = TrackElemType::up25ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slope, TrackPitch::none, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kUp25ToFlatSeq0 } },
    };

    constexpr auto kTEDDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::down25Covered,
        .priceModifier = 79872,
        .mirrorElement = TrackElemType::down25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slope, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kDown25Seq0 } },
    };

    constexpr auto kTEDDown60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 64, 0, 0, 0 },
        .pieceLength = 40,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::down60Covered,
        .priceModifier = 114688,
        .mirrorElement = TrackElemType::down60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slopeSteepDown, TrackPitch::down60, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        0 },
        .sequenceData = { 1, { kDown60Seq0 } },
    };

    constexpr auto kTEDFlatToDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::flatToDown25Covered,
        .priceModifier = 73728,
        .mirrorElement = TrackElemType::flatToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slope, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kFlatToDown25Seq0 } },
    };

    constexpr auto kTEDDown25ToDown60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 32, 0, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::down25ToDown60Covered,
        .priceModifier = 96256,
        .mirrorElement = TrackElemType::down25ToDown60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slopeSteepDown, TrackPitch::down60, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<-82>,
        .sequenceData = { 1, { kDown25ToDown60Seq0 } },
    };

    constexpr auto kTEDDown60ToDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 32, 0, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::down60ToDown25Covered,
        .priceModifier = 96256,
        .mirrorElement = TrackElemType::down60ToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slopeSteepDown, TrackPitch::down25, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<82>,
        .sequenceData = { 1, { kDown60ToDown25Seq0 } },
    };

    constexpr auto kTEDDown25ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::down25ToFlatCovered,
        .priceModifier = 73728,
        .mirrorElement = TrackElemType::down25ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slope, TrackPitch::none, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kDown25ToFlatSeq0 } },
    };

    constexpr auto kTEDLeftQuarterTurn5Tiles = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .alternativeType = TrackElemType::leftQuarterTurn5TilesCovered,
        .priceModifier = 257359,
        .mirrorElement = TrackElemType::rightQuarterTurn5Tiles,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::curve, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l8,
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 7,
                          { kLeftQuarterTurn5TilesSeq0, kLeftQuarterTurn5TilesSeq1, kLeftQuarterTurn5TilesSeq2,
                            kLeftQuarterTurn5TilesSeq3, kLeftQuarterTurn5TilesSeq4, kLeftQuarterTurn5TilesSeq5,
                            kLeftQuarterTurn5TilesSeq6 } },
    };

    constexpr auto kTEDRightQuarterTurn5Tiles = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .alternativeType = TrackElemType::rightQuarterTurn5TilesCovered,
        .priceModifier = 257359,
        .mirrorElement = TrackElemType::leftQuarterTurn5Tiles,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::curve, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r8,
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 7,
                          { kRightQuarterTurn5TilesSeq0, kRightQuarterTurn5TilesSeq1, kRightQuarterTurn5TilesSeq2,
                            kRightQuarterTurn5TilesSeq3, kRightQuarterTurn5TilesSeq4, kRightQuarterTurn5TilesSeq5,
                            kRightQuarterTurn5TilesSeq6 } },
    };

    constexpr auto kTEDFlatToLeftBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::flatToRightBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatToLeftBankSeq0 } },
    };

    constexpr auto kTEDFlatToRightBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::flatToLeftBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatToRightBankSeq0 } },
    };

    constexpr auto kTEDLeftBankToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::rightBankToFlat,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::left, 0 },
        .sequenceData = { 1, { kLeftBankToFlatSeq0 } },
    };

    constexpr auto kTEDRightBankToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::leftBankToFlat,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::right, 0 },
        .sequenceData = { 1, { kRightBankToFlatSeq0 } },
    };

    constexpr auto kTEDBankedLeftQuarterTurn5Tiles = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 273443,
        .mirrorElement = TrackElemType::bankedRightQuarterTurn5Tiles,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 7,
                          { kBankedLeftQuarterTurn5TilesSeq0, kBankedLeftQuarterTurn5TilesSeq1,
                            kBankedLeftQuarterTurn5TilesSeq2, kBankedLeftQuarterTurn5TilesSeq3,
                            kBankedLeftQuarterTurn5TilesSeq4, kBankedLeftQuarterTurn5TilesSeq5,
                            kBankedLeftQuarterTurn5TilesSeq6 } },
    };

    constexpr auto kTEDBankedRightQuarterTurn5Tiles = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 273443,
        .mirrorElement = TrackElemType::bankedLeftQuarterTurn5Tiles,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 7,
                          { kBankedRightQuarterTurn5TilesSeq0, kBankedRightQuarterTurn5TilesSeq1,
                            kBankedRightQuarterTurn5TilesSeq2, kBankedRightQuarterTurn5TilesSeq3,
                            kBankedRightQuarterTurn5TilesSeq4, kBankedRightQuarterTurn5TilesSeq5,
                            kBankedRightQuarterTurn5TilesSeq6 } },
    };

    constexpr auto kTEDLeftBankToUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::rightBankToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kLeftBankToUp25Seq0 } },
    };

    constexpr auto kTEDRightBankToUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::leftBankToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kRightBankToUp25Seq0 } },
    };

    constexpr auto kTEDUp25ToLeftBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::up25ToRightBank,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::up25, TrackRoll::left, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kUp25ToLeftBankSeq0 } },
    };

    constexpr auto kTEDUp25ToRightBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::up25ToLeftBank,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::up25, TrackRoll::right, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kUp25ToRightBankSeq0 } },
    };

    constexpr auto kTEDLeftBankToDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::rightBankToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kLeftBankToDown25Seq0 } },
    };

    constexpr auto kTEDRightBankToDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::leftBankToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kRightBankToDown25Seq0 } },
    };

    constexpr auto kTEDDown25ToLeftBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::down25ToRightBank,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::down25, TrackRoll::left, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kDown25ToLeftBankSeq0 } },
    };

    constexpr auto kTEDDown25ToRightBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::down25ToLeftBank,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::down25, TrackRoll::right, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kDown25ToRightBankSeq0 } },
    };

    constexpr auto kTEDLeftBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::rightBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .sequenceData = { 1, { kLeftBankSeq0 } },
    };

    constexpr auto kTEDRightBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::leftBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .sequenceData = { 1, { kRightBankSeq0 } },
    };

    constexpr auto kTEDLeftQuarterTurn5TilesUp25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 64, -64, -64 },
        .pieceLength = 130,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 313656,
        .mirrorElement = TrackElemType::rightQuarterTurn5TilesUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l8,
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 7,
                          { kLeftQuarterTurn5TilesUp25Seq0, kLeftQuarterTurn5TilesUp25Seq1, kLeftQuarterTurn5TilesUp25Seq2,
                            kLeftQuarterTurn5TilesUp25Seq3, kLeftQuarterTurn5TilesUp25Seq4, kLeftQuarterTurn5TilesUp25Seq5,
                            kLeftQuarterTurn5TilesUp25Seq6 } },
    };

    constexpr auto kTEDRightQuarterTurn5TilesUp25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 64, -64, 64 },
        .pieceLength = 130,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 313656,
        .mirrorElement = TrackElemType::leftQuarterTurn5TilesUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r8,
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 7,
                          { kRightQuarterTurn5TilesUp25Seq0, kRightQuarterTurn5TilesUp25Seq1, kRightQuarterTurn5TilesUp25Seq2,
                            kRightQuarterTurn5TilesUp25Seq3, kRightQuarterTurn5TilesUp25Seq4, kRightQuarterTurn5TilesUp25Seq5,
                            kRightQuarterTurn5TilesUp25Seq6 } },
    };

    constexpr auto kTEDLeftQuarterTurn5TilesDown25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 64, 0, -64, -64 },
        .pieceLength = 130,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 313656,
        .mirrorElement = TrackElemType::rightQuarterTurn5TilesDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l8,
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 7,
                          { kLeftQuarterTurn5TilesDown25Seq0, kLeftQuarterTurn5TilesDown25Seq1,
                            kLeftQuarterTurn5TilesDown25Seq2, kLeftQuarterTurn5TilesDown25Seq3,
                            kLeftQuarterTurn5TilesDown25Seq4, kLeftQuarterTurn5TilesDown25Seq5,
                            kLeftQuarterTurn5TilesDown25Seq6 } },
    };

    constexpr auto kTEDRightQuarterTurn5TilesDown25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 64, 0, -64, 64 },
        .pieceLength = 130,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 313656,
        .mirrorElement = TrackElemType::leftQuarterTurn5TilesDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r8,
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 7,
                          { kRightQuarterTurn5TilesDown25Seq0, kRightQuarterTurn5TilesDown25Seq1,
                            kRightQuarterTurn5TilesDown25Seq2, kRightQuarterTurn5TilesDown25Seq3,
                            kRightQuarterTurn5TilesDown25Seq4, kRightQuarterTurn5TilesDown25Seq5,
                            kRightQuarterTurn5TilesDown25Seq6 } },
    };

    constexpr auto kTEDLeftQuarterTurn3Tiles = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, -32, -32 },
        .pieceLength = 75,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .alternativeType = TrackElemType::leftQuarterTurn3TilesCovered,
        .priceModifier = 154415,
        .mirrorElement = TrackElemType::rightQuarterTurn3Tiles,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::curveSmall, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l7,
        .lateralFactor = EvaluatorConst<59>,
        .sequenceData = { 4,
                          { kLeftQuarterTurn3TilesSeq0, kLeftQuarterTurn3TilesSeq1, kLeftQuarterTurn3TilesSeq2,
                            kLeftQuarterTurn3TilesSeq3 } },
    };

    constexpr auto kTEDRightQuarterTurn3Tiles = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, -32, 32 },
        .pieceLength = 75,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .alternativeType = TrackElemType::rightQuarterTurn3TilesCovered,
        .priceModifier = 154415,
        .mirrorElement = TrackElemType::leftQuarterTurn3Tiles,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::curveSmall, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r7,
        .lateralFactor = EvaluatorConst<-59>,
        .sequenceData = { 4,
                          { kRightQuarterTurn3TilesSeq0, kRightQuarterTurn3TilesSeq1, kRightQuarterTurn3TilesSeq2,
                            kRightQuarterTurn3TilesSeq3 } },
    };

    constexpr auto kTEDLeftBankedQuarterTurn3Tiles = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, -32, -32 },
        .pieceLength = 75,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 164065,
        .mirrorElement = TrackElemType::rightBankedQuarterTurn3Tiles,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorConst<100>,
        .lateralFactor = EvaluatorConst<100>,
        .sequenceData = { 4,
                          { kLeftBankedQuarterTurn3TilesSeq0, kLeftBankedQuarterTurn3TilesSeq1,
                            kLeftBankedQuarterTurn3TilesSeq2, kLeftBankedQuarterTurn3TilesSeq3 } },
    };

    constexpr auto kTEDRightBankedQuarterTurn3Tiles = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, -32, 32 },
        .pieceLength = 75,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 164065,
        .mirrorElement = TrackElemType::leftBankedQuarterTurn3Tiles,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorConst<100>,
        .lateralFactor = EvaluatorConst<-100>,
        .sequenceData = { 4,
                          { kRightBankedQuarterTurn3TilesSeq0, kRightBankedQuarterTurn3TilesSeq1,
                            kRightBankedQuarterTurn3TilesSeq2, kRightBankedQuarterTurn3TilesSeq3 } },
    };

    constexpr auto kTEDLeftQuarterTurn3TilesUp25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 32, -32, -32 },
        .pieceLength = 77,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::rightQuarterTurn3TilesUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l7,
        .lateralFactor = EvaluatorConst<59>,
        .sequenceData = { 4,
                          { kLeftQuarterTurn3TilesUp25Seq0, kLeftQuarterTurn3TilesUp25Seq1, kLeftQuarterTurn3TilesUp25Seq2,
                            kLeftQuarterTurn3TilesUp25Seq3 } },
    };

    constexpr auto kTEDRightQuarterTurn3TilesUp25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 32, -32, 32 },
        .pieceLength = 77,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::leftQuarterTurn3TilesUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r7,
        .lateralFactor = EvaluatorConst<-59>,
        .sequenceData = { 4,
                          { kRightQuarterTurn3TilesUp25Seq0, kRightQuarterTurn3TilesUp25Seq1, kRightQuarterTurn3TilesUp25Seq2,
                            kRightQuarterTurn3TilesUp25Seq3 } },
    };

    constexpr auto kTEDLeftQuarterTurn3TilesDown25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 32, 0, -32, -32 },
        .pieceLength = 77,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::rightQuarterTurn3TilesDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l7,
        .lateralFactor = EvaluatorConst<59>,
        .sequenceData = { 4,
                          { kLeftQuarterTurn3TilesDown25Seq0, kLeftQuarterTurn3TilesDown25Seq1,
                            kLeftQuarterTurn3TilesDown25Seq2, kLeftQuarterTurn3TilesDown25Seq3 } },
    };

    constexpr auto kTEDRightQuarterTurn3TilesDown25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 32, 0, -32, 32 },
        .pieceLength = 77,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::leftQuarterTurn3TilesDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r7,
        .lateralFactor = EvaluatorConst<-59>,
        .sequenceData = { 4,
                          { kRightQuarterTurn3TilesDown25Seq0, kRightQuarterTurn3TilesDown25Seq1,
                            kRightQuarterTurn3TilesDown25Seq2, kRightQuarterTurn3TilesDown25Seq3 } },
    };

    constexpr auto kTEDLeftQuarterTurn1Tile = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, 0, 0 },
        .pieceLength = 24,
        .curveChain = { TrackCurve::leftVerySmall, TrackCurve::leftVerySmall },
        .priceModifier = 51471,
        .mirrorElement = TrackElemType::rightQuarterTurn1Tile,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::curveVerySmall, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l5,
        .lateralFactor = EvaluatorConst<45>,
        .sequenceData = { 1, { kLeftQuarterTurn1TileSeq0 } },
    };

    constexpr auto kTEDRightQuarterTurn1Tile = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, 0, 0 },
        .pieceLength = 24,
        .curveChain = { TrackCurve::rightVerySmall, TrackCurve::rightVerySmall },
        .priceModifier = 51471,
        .mirrorElement = TrackElemType::leftQuarterTurn1Tile,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::curveVerySmall, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r5,
        .lateralFactor = EvaluatorConst<-45>,
        .sequenceData = { 1, { kRightQuarterTurn1TileSeq0 } },
    };

    constexpr auto kTEDLeftTwistDownToUp = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_LEFT,
        .coordinates = { 0, 0, 0, 16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::leftTwistUpToDown, TrackCurve::none },
        .priceModifier = 212992,
        .mirrorElement = TrackElemType::rightTwistDownToUp,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::twist, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 3, { kLeftTwistDownToUpSeq0, kLeftTwistDownToUpSeq1, kLeftTwistDownToUpSeq2 } },
    };

    constexpr auto kTEDRightTwistDownToUp = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_RIGHT,
        .coordinates = { 0, 0, 0, 16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::rightTwistUpToDown, TrackCurve::none },
        .priceModifier = 212992,
        .mirrorElement = TrackElemType::leftTwistDownToUp,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::twist, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 3, { kRightTwistDownToUpSeq0, kRightTwistDownToUpSeq1, kRightTwistDownToUpSeq2 } },
    };

    constexpr auto kTEDLeftTwistUpToDown = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_LEFT,
        .coordinates = { 0, 0, 0, -16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::leftTwistDownToUp },
        .priceModifier = 212992,
        .mirrorElement = TrackElemType::rightTwistUpToDown,
        .flags = { TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::twist, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown, 0 },
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 3, { kLeftTwistUpToDownSeq0, kLeftTwistUpToDownSeq1, kLeftTwistUpToDownSeq2 } },
    };

    constexpr auto kTEDRightTwistUpToDown = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_RIGHT,
        .coordinates = { 0, 0, 0, -16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::rightTwistDownToUp },
        .priceModifier = 212992,
        .mirrorElement = TrackElemType::leftTwistUpToDown,
        .flags = { TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::twist, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown, 0 },
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 3, { kRightTwistUpToDownSeq0, kRightTwistUpToDownSeq1, kRightTwistUpToDownSeq2 } },
    };

    constexpr auto kTEDFlatToUp60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 24, 0, 0 },
        .pieceLength = 36,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::flatToUp60,
        .flags = { TrackElementFlag::up, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flatToSteepSlope, TrackPitch::up60, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<56>,
        .sequenceData = { 1, { kFlatToUp60Seq0 } },
    };

    constexpr auto kTEDUp60ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 24, 0, 0 },
        .pieceLength = 36,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::up60ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flatToSteepSlope, TrackPitch::none, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-56>,
        .sequenceData = { 1, { kUp60ToFlatSeq0 } },
    };

    constexpr auto kTEDFlatToDown60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 24, 0, 0, 0 },
        .pieceLength = 36,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::flatToDown60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flatToSteepSlope, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<-56>,
        .sequenceData = { 1, { kFlatToDown60Seq0 } },
    };

    constexpr auto kTEDDown60ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 24, 0, 0, 0 },
        .pieceLength = 36,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::down60ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flatToSteepSlope, TrackPitch::none, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<56>,
        .sequenceData = { 1, { kDown60ToFlatSeq0 } },
    };

    constexpr auto kTEDTowerBase = TrackElementDescriptor{
        .description = STR_ENTRY_EXIT_PLATFORM,
        .coordinates = { 0, 0, 0, 96, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::towerSection, TrackCurve::none },
        .priceModifier = 524288,
        .mirrorElement = TrackElemType::towerBase,
        .flags = {},
        .definition = { TrackGroup::tower, TrackPitch::tower, TrackPitch::tower, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 9,
                          { kTowerBaseSeq0, kTowerBaseSeq1, kTowerBaseSeq2, kTowerBaseSeq3, kTowerBaseSeq4, kTowerBaseSeq5,
                            kTowerBaseSeq6, kTowerBaseSeq7, kTowerBaseSeq8 } },
    };

    constexpr auto kTEDTowerSection = TrackElementDescriptor{
        .description = STR_VERTICAL_TOWER,
        .coordinates = { 0, 0, 0, 32, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::towerSection, TrackCurve::none },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::towerSection,
        .flags = {},
        .definition = { TrackGroup::tower, TrackPitch::tower, TrackPitch::tower, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kTowerSectionSeq0, kTowerSectionSeq1 } },
    };

    constexpr auto kTEDFlatCovered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::flatCovered,
        .flags = {},
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatCoveredSeq0 } },
    };

    constexpr auto kTEDUp25Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::up25Covered,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kUp25CoveredSeq0 } },
    };

    constexpr auto kTEDUp60Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 64, 0, 0 },
        .pieceLength = 40,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 118784,
        .mirrorElement = TrackElemType::up60Covered,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::up60, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kUp60CoveredSeq0 } },
    };

    constexpr auto kTEDFlatToUp25Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::flatToUp25Covered,
        .flags = { TrackElementFlag::up },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kFlatToUp25CoveredSeq0 } },
    };

    constexpr auto kTEDUp25ToUp60Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 32, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 100352,
        .mirrorElement = TrackElemType::up25ToUp60Covered,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::up60, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<82>,
        .sequenceData = { 1, { kUp25ToUp60CoveredSeq0 } },
    };

    constexpr auto kTEDUp60ToUp25Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 32, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 100352,
        .mirrorElement = TrackElemType::up60ToUp25Covered,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-82>,
        .sequenceData = { 1, { kUp60ToUp25CoveredSeq0 } },
    };

    constexpr auto kTEDUp25ToFlatCovered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::up25ToFlatCovered,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kUp25ToFlatCoveredSeq0 } },
    };

    constexpr auto kTEDDown25Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::down25Covered,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kDown25CoveredSeq0 } },
    };

    constexpr auto kTEDDown60Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 64, 0, 0, 0 },
        .pieceLength = 40,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 118784,
        .mirrorElement = TrackElemType::down60Covered,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::down60, TrackPitch::down60, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kDown60CoveredSeq0 } },
    };

    constexpr auto kTEDFlatToDown25Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::flatToDown25Covered,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kFlatToDown25CoveredSeq0 } },
    };

    constexpr auto kTEDDown25ToDown60Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 32, 0, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 100352,
        .mirrorElement = TrackElemType::down25ToDown60Covered,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::down60, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-82>,
        .sequenceData = { 1, { kDown25ToDown60CoveredSeq0 } },
    };

    constexpr auto kTEDDown60ToDown25Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 32, 0, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 100352,
        .mirrorElement = TrackElemType::down60ToDown25Covered,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::down25, TrackPitch::down60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<82>,
        .sequenceData = { 1, { kDown60ToDown25CoveredSeq0 } },
    };

    constexpr auto kTEDDown25ToFlatCovered = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::down25ToFlatCovered,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kDown25ToFlatCoveredSeq0 } },
    };

    constexpr auto kTEDLeftQuarterTurn5TilesCovered = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 261455,
        .mirrorElement = TrackElemType::rightQuarterTurn5TilesCovered,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l8,
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 7,
                          { kLeftQuarterTurn5TilesCoveredSeq0, kLeftQuarterTurn5TilesCoveredSeq1,
                            kLeftQuarterTurn5TilesCoveredSeq2, kLeftQuarterTurn5TilesCoveredSeq3,
                            kLeftQuarterTurn5TilesCoveredSeq4, kLeftQuarterTurn5TilesCoveredSeq5,
                            kLeftQuarterTurn5TilesCoveredSeq6 } },
    };

    constexpr auto kTEDRightQuarterTurn5TilesCovered = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 261455,
        .mirrorElement = TrackElemType::leftQuarterTurn5TilesCovered,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r8,
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 7,
                          { kRightQuarterTurn5TilesCoveredSeq0, kRightQuarterTurn5TilesCoveredSeq1,
                            kRightQuarterTurn5TilesCoveredSeq2, kRightQuarterTurn5TilesCoveredSeq3,
                            kRightQuarterTurn5TilesCoveredSeq4, kRightQuarterTurn5TilesCoveredSeq5,
                            kRightQuarterTurn5TilesCoveredSeq6 } },
    };

    constexpr auto kTEDLeftQuarterTurn3TilesCovered = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, -32, -32 },
        .pieceLength = 75,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 158511,
        .mirrorElement = TrackElemType::rightQuarterTurn3TilesCovered,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l7,
        .lateralFactor = EvaluatorConst<59>,
        .sequenceData = { 4,
                          { kLeftQuarterTurn3TilesCoveredSeq0, kLeftQuarterTurn3TilesCoveredSeq1,
                            kLeftQuarterTurn3TilesCoveredSeq2, kLeftQuarterTurn3TilesCoveredSeq3 } },
    };

    constexpr auto kTEDRightQuarterTurn3TilesCovered = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, -32, 32 },
        .pieceLength = 75,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 158511,
        .mirrorElement = TrackElemType::leftQuarterTurn3TilesCovered,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r7,
        .lateralFactor = EvaluatorConst<-59>,
        .sequenceData = { 4,
                          { kRightQuarterTurn3TilesCoveredSeq0, kRightQuarterTurn3TilesCoveredSeq1,
                            kRightQuarterTurn3TilesCoveredSeq2, kRightQuarterTurn3TilesCoveredSeq3 } },
    };

    constexpr auto kTEDLeftQuarterTurn1TileUp60 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 64, 0, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 126976,
        .mirrorElement = TrackElemType::rightQuarterTurn1TileUp60,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::up60, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l5,
        .lateralFactor = EvaluatorConst<88>,
        .sequenceData = { 1, { kLeftQuarterTurn1TileUp60Seq0 } },
    };

    constexpr auto kTEDRightQuarterTurn1TileUp60 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 64, 0, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 126976,
        .mirrorElement = TrackElemType::leftQuarterTurn1TileUp60,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::up60, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r5,
        .lateralFactor = EvaluatorConst<-88>,
        .sequenceData = { 1, { kRightQuarterTurn1TileUp60Seq0 } },
    };

    constexpr auto kTEDLeftQuarterTurn1TileDown60 = TrackElementDescriptor{
        .coordinates = { 0, 3, 64, 0, 0, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 126976,
        .mirrorElement = TrackElemType::rightQuarterTurn1TileDown60,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::down60, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        0 },
        .spinFunction = SpinFunction::l5,
        .lateralFactor = EvaluatorConst<88>,
        .sequenceData = { 1, { kLeftQuarterTurn1TileDown60Seq0 } },
    };

    constexpr auto kTEDRightQuarterTurn1TileDown60 = TrackElementDescriptor{
        .coordinates = { 0, 1, 64, 0, 0, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 126976,
        .mirrorElement = TrackElemType::leftQuarterTurn1TileDown60,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::down60, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        0 },
        .spinFunction = SpinFunction::r5,
        .lateralFactor = EvaluatorConst<-88>,
        .sequenceData = { 1, { kRightQuarterTurn1TileDown60Seq0 } },
    };

    constexpr auto kTEDBrakes = TrackElementDescriptor{
        .description = STR_BRAKES,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::brakes, TrackElemType::brakes },
        .priceModifier = 90112,
        .mirrorElement = TrackElemType::brakes,
        .flags = {},
        .definition = { TrackGroup::brakes, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kBrakesSeq0 } },
    };

    constexpr auto kTEDBooster = TrackElementDescriptor{
        .description = STR_BOOSTER,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::booster, TrackElemType::booster },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::booster,
        .flags = {},
        .definition = { TrackGroup::booster, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::rc,
        .sequenceData = { 1, { kBoosterSeq0 } },
    };

    constexpr auto kTEDLeftQuarterBankedHelixLargeUp = TrackElementDescriptor{
        .description = STR_QUARTER_HELIX_UP,
        .coordinates = { 0, 3, 0, 16, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::leftQuarterBankedHelixLargeUp, TrackElemType::leftQuarterBankedHelixLargeUp },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::rightQuarterBankedHelixLargeUp,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixUpBankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 7,
                          { kLeftQuarterBankedHelixLargeUpSeq0, kLeftQuarterBankedHelixLargeUpSeq1,
                            kLeftQuarterBankedHelixLargeUpSeq2, kLeftQuarterBankedHelixLargeUpSeq3,
                            kLeftQuarterBankedHelixLargeUpSeq4, kLeftQuarterBankedHelixLargeUpSeq5,
                            kLeftQuarterBankedHelixLargeUpSeq6 } },
    };

    constexpr auto kTEDRightQuarterBankedHelixLargeUp = TrackElementDescriptor{
        .description = STR_QUARTER_HELIX_UP,
        .coordinates = { 0, 1, 0, 16, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::rightQuarterBankedHelixLargeUp, TrackElemType::rightQuarterBankedHelixLargeUp },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::leftQuarterBankedHelixLargeUp,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixUpBankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::right,
                        TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 7,
                          { kRightQuarterBankedHelixLargeUpSeq0, kRightQuarterBankedHelixLargeUpSeq1,
                            kRightQuarterBankedHelixLargeUpSeq2, kRightQuarterBankedHelixLargeUpSeq3,
                            kRightQuarterBankedHelixLargeUpSeq4, kRightQuarterBankedHelixLargeUpSeq5,
                            kRightQuarterBankedHelixLargeUpSeq6 } },
    };

    constexpr auto kTEDLeftQuarterBankedHelixLargeDown = TrackElementDescriptor{
        .description = STR_QUARTER_HELIX_DOWN,
        .coordinates = { 0, 3, 16, 0, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::leftQuarterBankedHelixLargeDown, TrackElemType::leftQuarterBankedHelixLargeDown },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::rightQuarterBankedHelixLargeDown,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixDownBankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::left,
                        TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .reversedRotationOffset = 1,
        .sequenceData = { 7,
                          { kLeftQuarterBankedHelixLargeDownSeq0, kLeftQuarterBankedHelixLargeDownSeq1,
                            kLeftQuarterBankedHelixLargeDownSeq2, kLeftQuarterBankedHelixLargeDownSeq3,
                            kLeftQuarterBankedHelixLargeDownSeq4, kLeftQuarterBankedHelixLargeDownSeq5,
                            kLeftQuarterBankedHelixLargeDownSeq6 } },
    };

    constexpr auto kTEDRightQuarterBankedHelixLargeDown = TrackElementDescriptor{
        .description = STR_QUARTER_HELIX_DOWN,
        .coordinates = { 0, 1, 16, 0, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::rightQuarterBankedHelixLargeDown, TrackElemType::rightQuarterBankedHelixLargeDown },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::leftQuarterBankedHelixLargeDown,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixDownBankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::right,
                        TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .reversedRotationOffset = 3,
        .sequenceData = { 7,
                          { kRightQuarterBankedHelixLargeDownSeq0, kRightQuarterBankedHelixLargeDownSeq1,
                            kRightQuarterBankedHelixLargeDownSeq2, kRightQuarterBankedHelixLargeDownSeq3,
                            kRightQuarterBankedHelixLargeDownSeq4, kRightQuarterBankedHelixLargeDownSeq5,
                            kRightQuarterBankedHelixLargeDownSeq6 } },
    };

    constexpr auto kTEDLeftQuarterHelixLargeUp = TrackElementDescriptor{
        .description = STR_HELIX_UP_LEFT,
        .coordinates = { 0, 3, 0, 16, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::leftQuarterHelixLargeUp, TrackElemType::leftQuarterHelixLargeUp },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::rightQuarterHelixLargeUp,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix },
        .definition = { TrackGroup::helixUpUnbankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::none,
                        TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l8,
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 7,
                          { kLeftQuarterHelixLargeUpSeq0, kLeftQuarterHelixLargeUpSeq1, kLeftQuarterHelixLargeUpSeq2,
                            kLeftQuarterHelixLargeUpSeq3, kLeftQuarterHelixLargeUpSeq4, kLeftQuarterHelixLargeUpSeq5,
                            kLeftQuarterHelixLargeUpSeq6 } },
    };

    constexpr auto kTEDRightQuarterHelixLargeUp = TrackElementDescriptor{
        .description = STR_HELIX_UP_RIGHT,
        .coordinates = { 0, 1, 0, 16, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::rightQuarterHelixLargeUp, TrackElemType::rightQuarterHelixLargeUp },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::leftQuarterHelixLargeUp,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix },
        .definition = { TrackGroup::helixUpUnbankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::none,
                        TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r8,
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 7,
                          { kRightQuarterHelixLargeUpSeq0, kRightQuarterHelixLargeUpSeq1, kRightQuarterHelixLargeUpSeq2,
                            kRightQuarterHelixLargeUpSeq3, kRightQuarterHelixLargeUpSeq4, kRightQuarterHelixLargeUpSeq5,
                            kRightQuarterHelixLargeUpSeq6 } },
    };

    constexpr auto kTEDLeftQuarterHelixLargeDown = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_LEFT,
        .coordinates = { 0, 3, 16, 0, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::leftQuarterHelixLargeDown, TrackElemType::leftQuarterHelixLargeDown },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::rightQuarterHelixLargeDown,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix },
        .definition = { TrackGroup::helixDownUnbankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::none,
                        TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l8,
        .lateralFactor = EvaluatorConst<98>,
        .reversedRotationOffset = 1,
        .sequenceData = { 7,
                          { kLeftQuarterHelixLargeDownSeq0, kLeftQuarterHelixLargeDownSeq1, kLeftQuarterHelixLargeDownSeq2,
                            kLeftQuarterHelixLargeDownSeq3, kLeftQuarterHelixLargeDownSeq4, kLeftQuarterHelixLargeDownSeq5,
                            kLeftQuarterHelixLargeDownSeq6 } },
    };

    constexpr auto kTEDRightQuarterHelixLargeDown = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_RIGHT,
        .coordinates = { 0, 1, 16, 0, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::rightQuarterHelixLargeDown, TrackElemType::rightQuarterHelixLargeDown },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::leftQuarterHelixLargeDown,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix },
        .definition = { TrackGroup::helixDownUnbankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::none,
                        TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r8,
        .lateralFactor = EvaluatorConst<-98>,
        .reversedRotationOffset = 3,
        .sequenceData = { 7,
                          { kRightQuarterHelixLargeDownSeq0, kRightQuarterHelixLargeDownSeq1, kRightQuarterHelixLargeDownSeq2,
                            kRightQuarterHelixLargeDownSeq3, kRightQuarterHelixLargeDownSeq4, kRightQuarterHelixLargeDownSeq5,
                            kRightQuarterHelixLargeDownSeq6 } },
    };

    constexpr auto kTEDUp25LeftBanked = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::up25RightBanked,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::left, 0 },
        .sequenceData = { 1, { kUp25LeftBankedSeq0 } },
    };

    constexpr auto kTEDUp25RightBanked = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::up25LeftBanked,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::up25, TrackRoll::right, TrackRoll::right,
                        0 },
        .sequenceData = { 1, { kUp25RightBankedSeq0 } },
    };

    constexpr auto kTEDOnRidePhoto = TrackElementDescriptor{
        .description = STR_ON_RIDE_PHOTO_SECTION,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 196608,
        .mirrorElement = TrackElemType::onRidePhoto,
        .flags = {},
        .definition = { TrackGroup::onridePhoto, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kOnRidePhotoSeq0 } },
    };

    constexpr auto kTEDDown25LeftBanked = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::down25RightBanked,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::down25, TrackRoll::left, TrackRoll::left,
                        0 },
        .sequenceData = { 1, { kDown25LeftBankedSeq0 } },
    };

    constexpr auto kTEDDown25RightBanked = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::down25LeftBanked,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::down25, TrackRoll::right,
                        TrackRoll::right, 0 },
        .sequenceData = { 1, { kDown25RightBankedSeq0 } },
    };

    constexpr auto kTEDCableLiftHill = TrackElementDescriptor{
        .description = STR_CABLE_LIFT_HILL,
        .coordinates = { 0, 0, 0, -96, -96, 0 },
        .pieceLength = 138,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 643072,
        .mirrorElement = TrackElemType::cableLiftHill,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::liftHillCable, TrackPitch::down60, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 4, { kCableLiftHillSeq0, kCableLiftHillSeq1, kCableLiftHillSeq2, kCableLiftHillSeq3 } },
    };

    constexpr auto kTEDReverseFreefallSlope = TrackElementDescriptor{
        .description = STR_SLOPE_UP_TO_VERTICAL,
        .coordinates = { 0, 0, 0, 240, -160, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::reverseFreefallVertical, TrackCurve::none },
        .priceModifier = 491520,
        .mirrorElement = TrackElemType::reverseFreefallSlope,
        .flags = { TrackElementFlag::up },
        .definition = { TrackGroup::reverseFreefall, TrackPitch::reverseFreefall, TrackPitch::none, TrackRoll::none,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<120>,
        .sequenceData = { 7,
                          { kReverseFreefallSlopeSeq0, kReverseFreefallSlopeSeq1, kReverseFreefallSlopeSeq2,
                            kReverseFreefallSlopeSeq3, kReverseFreefallSlopeSeq4, kReverseFreefallSlopeSeq5,
                            kReverseFreefallSlopeSeq6 } },
    };

    constexpr auto kTEDReverseFreefallVertical = TrackElementDescriptor{
        .description = STR_VERTICAL_TRACK,
        .coordinates = { 0, 0, 0, 80, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::reverseFreefallVertical, TrackElemType::reverseFreefallVertical },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::reverseFreefallVertical,
        .flags = { TrackElementFlag::up },
        .definition = { TrackGroup::reverseFreefall, TrackPitch::reverseFreefall, TrackPitch::reverseFreefall, TrackRoll::none,
                        TrackRoll::none, 0 },
        .sequenceData = { 2, { kReverseFreefallVerticalSeq0, kReverseFreefallVerticalSeq1 } },
    };

    constexpr auto kTEDUp90 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 32, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::up90,
        .flags = { TrackElementFlag::up },
        .definition = { TrackGroup::flat, TrackPitch::up90, TrackPitch::up90, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kUp90Seq0, kUp90Seq1 } },
    };

    constexpr auto kTEDDown90 = TrackElementDescriptor{
        .coordinates = { 0, 0, 32, 0, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::down90,
        .flags = { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flat, TrackPitch::down90, TrackPitch::down90, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kDown90Seq0, kDown90Seq1 } },
    };

    constexpr auto kTEDUp60ToUp90 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 56, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 114688,
        .mirrorElement = TrackElemType::up60ToUp90,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::up90, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<110>,
        .sequenceData = { 2, { kUp60ToUp90Seq0, kUp60ToUp90Seq1 } },
    };

    constexpr auto kTEDDown90ToDown60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 56, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 114688,
        .mirrorElement = TrackElemType::down90ToDown60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flat, TrackPitch::down60, TrackPitch::down90, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<110>,
        .sequenceData = { 1, { kDown90ToDown60Seq0 } },
    };

    constexpr auto kTEDUp90ToUp60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 56, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 114688,
        .mirrorElement = TrackElemType::up90ToUp60,
        .flags = { TrackElementFlag::up },
        .definition = { TrackGroup::flat, TrackPitch::up60, TrackPitch::up90, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-110>,
        .sequenceData = { 1, { kUp90ToUp60Seq0 } },
    };

    constexpr auto kTEDDown60ToDown90 = TrackElementDescriptor{
        .coordinates = { 0, 0, 56, 0, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 114688,
        .mirrorElement = TrackElemType::down60ToDown90,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::flat, TrackPitch::down90, TrackPitch::down60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-110>,
        .sequenceData = { 2, { kDown60ToDown90Seq0, kDown60ToDown90Seq1 } },
    };

    constexpr auto kTEDBrakeForDrop = TrackElementDescriptor{
        .description = STR_HOLDING_BRAKE_FOR_DROP,
        .coordinates = { 0, 0, 24, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 147456,
        .mirrorElement = TrackElemType::brakeForDrop,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::brakeForDrop, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-56>,
        .sequenceData = { 1, { kBrakeForDropSeq0 } },
    };

    constexpr auto kTEDLeftEighthToDiag = TrackElementDescriptor{
        .coordinates = { 0, 7, 0, 0, -64, -32 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 180151,
        .mirrorElement = TrackElemType::rightEighthToDiag,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l9,
        .lateralFactor = EvaluatorConst<137>,
        .sequenceData = { 5,
                          { kLeftEighthToDiagSeq0, kLeftEighthToDiagSeq1, kLeftEighthToDiagSeq2, kLeftEighthToDiagSeq3,
                            kLeftEighthToDiagSeq4 } },
    };

    constexpr auto kTEDRightEighthToDiag = TrackElementDescriptor{
        .coordinates = { 0, 4, 0, 0, -64, 32 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 180151,
        .mirrorElement = TrackElemType::leftEighthToDiag,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r9,
        .lateralFactor = EvaluatorConst<-137>,
        .sequenceData = { 5,
                          { kRightEighthToDiagSeq0, kRightEighthToDiagSeq1, kRightEighthToDiagSeq2, kRightEighthToDiagSeq3,
                            kRightEighthToDiagSeq4 } },
    };

    constexpr auto kTEDLeftEighthToOrthogonal = TrackElementDescriptor{
        .coordinates = { 4, 0, 0, 0, -64, 32 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 180151,
        .mirrorElement = TrackElemType::rightEighthToOrthogonal,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l9,
        .lateralFactor = EvaluatorConst<137>,
        .sequenceData = { 5,
                          { kLeftEighthToOrthogonalSeq0, kLeftEighthToOrthogonalSeq1, kLeftEighthToOrthogonalSeq2,
                            kLeftEighthToOrthogonalSeq3, kLeftEighthToOrthogonalSeq4 } },
    };

    constexpr auto kTEDRightEighthToOrthogonal = TrackElementDescriptor{
        .coordinates = { 4, 1, 0, 0, -32, 64 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 180151,
        .mirrorElement = TrackElemType::leftEighthToOrthogonal,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r9,
        .lateralFactor = EvaluatorConst<-137>,
        .sequenceData = { 5,
                          { kRightEighthToOrthogonalSeq0, kRightEighthToOrthogonalSeq1, kRightEighthToOrthogonalSeq2,
                            kRightEighthToOrthogonalSeq3, kRightEighthToOrthogonalSeq4 } },
    };

    constexpr auto kTEDLeftEighthBankToDiag = TrackElementDescriptor{
        .coordinates = { 0, 7, 0, 0, -64, -32 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 190960,
        .mirrorElement = TrackElemType::rightEighthBankToDiag,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<200>,
        .sequenceData = { 5,
                          { kLeftEighthBankToDiagSeq0, kLeftEighthBankToDiagSeq1, kLeftEighthBankToDiagSeq2,
                            kLeftEighthBankToDiagSeq3, kLeftEighthBankToDiagSeq4 } },
    };

    constexpr auto kTEDRightEighthBankToDiag = TrackElementDescriptor{
        .coordinates = { 0, 4, 0, 0, -64, 32 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 190960,
        .mirrorElement = TrackElemType::leftEighthBankToDiag,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<-200>,
        .sequenceData = { 5,
                          { kRightEighthBankToDiagSeq0, kRightEighthBankToDiagSeq1, kRightEighthBankToDiagSeq2,
                            kRightEighthBankToDiagSeq3, kRightEighthBankToDiagSeq4 } },
    };

    constexpr auto kTEDLeftEighthBankToOrthogonal = TrackElementDescriptor{
        .coordinates = { 4, 0, 0, 0, -64, 32 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 190960,
        .mirrorElement = TrackElemType::rightEighthBankToOrthogonal,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<200>,
        .sequenceData = { 5,
                          { kLeftEighthBankToOrthogonalSeq0, kLeftEighthBankToOrthogonalSeq1, kLeftEighthBankToOrthogonalSeq2,
                            kLeftEighthBankToOrthogonalSeq3, kLeftEighthBankToOrthogonalSeq4 } },
    };

    constexpr auto kTEDRightEighthBankToOrthogonal = TrackElementDescriptor{
        .coordinates = { 4, 1, 0, 0, -32, 64 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 190960,
        .mirrorElement = TrackElemType::leftEighthBankToOrthogonal,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<-200>,
        .sequenceData = { 5,
                          { kRightEighthBankToOrthogonalSeq0, kRightEighthBankToOrthogonalSeq1,
                            kRightEighthBankToOrthogonalSeq2, kRightEighthBankToOrthogonalSeq3,
                            kRightEighthBankToOrthogonalSeq4 } },
    };

    constexpr auto kTEDDiagFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 92681,
        .mirrorElement = TrackElemType::diagFlat,
        .flags = { TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagFlatSeq0, kDiagFlatSeq1, kDiagFlatSeq2, kDiagFlatSeq3 } },
    };

    constexpr auto kTEDDiagUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 111820,
        .mirrorElement = TrackElemType::diagUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlope, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagUp25Seq0, kDiagUp25Seq1, kDiagUp25Seq2, kDiagUp25Seq3 } },
    };

    constexpr auto kTEDDiagUp60 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 64, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 147456,
        .mirrorElement = TrackElemType::diagUp60,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepUp, TrackPitch::up60, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagUp60Seq0, kDiagUp60Seq1, kDiagUp60Seq2, kDiagUp60Seq3 } },
    };

    constexpr auto kTEDDiagFlatToUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 102400,
        .mirrorElement = TrackElemType::diagFlatToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlope, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4, { kDiagFlatToUp25Seq0, kDiagFlatToUp25Seq1, kDiagFlatToUp25Seq2, kDiagFlatToUp25Seq3 } },
    };

    constexpr auto kTEDDiagUp25ToUp60 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 32, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 129024,
        .mirrorElement = TrackElemType::diagUp25ToUp60,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepUp, TrackPitch::up60, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<95>,
        .sequenceData = { 4, { kDiagUp25ToUp60Seq0, kDiagUp25ToUp60Seq1, kDiagUp25ToUp60Seq2, kDiagUp25ToUp60Seq3 } },
    };

    constexpr auto kTEDDiagUp60ToUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 32, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 129024,
        .mirrorElement = TrackElemType::diagUp60ToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepUp, TrackPitch::up25, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-95>,
        .sequenceData = { 4, { kDiagUp60ToUp25Seq0, kDiagUp60ToUp25Seq1, kDiagUp60ToUp25Seq2, kDiagUp60ToUp25Seq3 } },
    };

    constexpr auto kTEDDiagUp25ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 102400,
        .mirrorElement = TrackElemType::diagUp25ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlope, TrackPitch::none, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4, { kDiagUp25ToFlatSeq0, kDiagUp25ToFlatSeq1, kDiagUp25ToFlatSeq2, kDiagUp25ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 111820,
        .mirrorElement = TrackElemType::diagDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlope, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagDown25Seq0, kDiagDown25Seq1, kDiagDown25Seq2, kDiagDown25Seq3 } },
    };

    constexpr auto kTEDDiagDown60 = TrackElementDescriptor{
        .coordinates = { 4, 4, 64, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 147456,
        .mirrorElement = TrackElemType::diagDown60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepDown, TrackPitch::down60, TrackPitch::down60, TrackRoll::none,
                        TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagDown60Seq0, kDiagDown60Seq1, kDiagDown60Seq2, kDiagDown60Seq3 } },
    };

    constexpr auto kTEDDiagFlatToDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 102400,
        .mirrorElement = TrackElemType::diagFlatToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlope, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4, { kDiagFlatToDown25Seq0, kDiagFlatToDown25Seq1, kDiagFlatToDown25Seq2, kDiagFlatToDown25Seq3 } },
    };

    constexpr auto kTEDDiagDown25ToDown60 = TrackElementDescriptor{
        .coordinates = { 4, 4, 32, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 129024,
        .mirrorElement = TrackElemType::diagDown25ToDown60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepDown, TrackPitch::down60, TrackPitch::down25, TrackRoll::none,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-95>,
        .sequenceData = { 4,
                          { kDiagDown25ToDown60Seq0, kDiagDown25ToDown60Seq1, kDiagDown25ToDown60Seq2,
                            kDiagDown25ToDown60Seq3 } },
    };

    constexpr auto kTEDDiagDown60ToDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 32, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 129024,
        .mirrorElement = TrackElemType::diagDown60ToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepDown, TrackPitch::down25, TrackPitch::down60, TrackRoll::none,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<95>,
        .sequenceData = { 4,
                          { kDiagDown60ToDown25Seq0, kDiagDown60ToDown25Seq1, kDiagDown60ToDown25Seq2,
                            kDiagDown60ToDown25Seq3 } },
    };

    constexpr auto kTEDDiagDown25ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 102400,
        .mirrorElement = TrackElemType::diagDown25ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlope, TrackPitch::none, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4, { kDiagDown25ToFlatSeq0, kDiagDown25ToFlatSeq1, kDiagDown25ToFlatSeq2, kDiagDown25ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagFlatToUp60 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 24, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 107008,
        .mirrorElement = TrackElemType::diagFlatToUp60,
        .flags = { TrackElementFlag::up, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepUp, TrackPitch::up60, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<60>,
        .sequenceData = { 4, { kDiagFlatToUp60Seq0, kDiagFlatToUp60Seq1, kDiagFlatToUp60Seq2, kDiagFlatToUp60Seq3 } },
    };

    constexpr auto kTEDDiagUp60ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 24, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 107008,
        .mirrorElement = TrackElemType::diagUp60ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepUp, TrackPitch::none, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-60>,
        .sequenceData = { 4, { kDiagUp60ToFlatSeq0, kDiagUp60ToFlatSeq1, kDiagUp60ToFlatSeq2, kDiagUp60ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagFlatToDown60 = TrackElementDescriptor{
        .coordinates = { 4, 4, 24, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 107008,
        .mirrorElement = TrackElemType::diagFlatToDown60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepDown, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<-60>,
        .sequenceData = { 4, { kDiagFlatToDown60Seq0, kDiagFlatToDown60Seq1, kDiagFlatToDown60Seq2, kDiagFlatToDown60Seq3 } },
    };

    constexpr auto kTEDDiagDown60ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 24, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 107008,
        .mirrorElement = TrackElemType::diagDown60ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepDown, TrackPitch::none, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<60>,
        .sequenceData = { 4, { kDiagDown60ToFlatSeq0, kDiagDown60ToFlatSeq1, kDiagDown60ToFlatSeq2, kDiagDown60ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagFlatToLeftBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::none },
        .priceModifier = 96777,
        .mirrorElement = TrackElemType::diagFlatToRightBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::none, 0 },
        .sequenceData = { 4,
                          { kDiagFlatToLeftBankSeq0, kDiagFlatToLeftBankSeq1, kDiagFlatToLeftBankSeq2,
                            kDiagFlatToLeftBankSeq3 } },
    };

    constexpr auto kTEDDiagFlatToRightBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::none },
        .priceModifier = 96777,
        .mirrorElement = TrackElemType::diagFlatToLeftBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::none, 0 },
        .sequenceData = { 4,
                          { kDiagFlatToRightBankSeq0, kDiagFlatToRightBankSeq1, kDiagFlatToRightBankSeq2,
                            kDiagFlatToRightBankSeq3 } },
    };

    constexpr auto kTEDDiagLeftBankToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::leftLarge },
        .priceModifier = 96777,
        .mirrorElement = TrackElemType::diagRightBankToFlat,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::left, 0 },
        .sequenceData = { 4,
                          { kDiagLeftBankToFlatSeq0, kDiagLeftBankToFlatSeq1, kDiagLeftBankToFlatSeq2,
                            kDiagLeftBankToFlatSeq3 } },
    };

    constexpr auto kTEDDiagRightBankToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::rightLarge },
        .priceModifier = 96777,
        .mirrorElement = TrackElemType::diagLeftBankToFlat,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::right, 0 },
        .sequenceData = { 4,
                          { kDiagRightBankToFlatSeq0, kDiagRightBankToFlatSeq1, kDiagRightBankToFlatSeq2,
                            kDiagRightBankToFlatSeq3 } },
    };

    constexpr auto kTEDDiagLeftBankToUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::leftLarge },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagRightBankToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagLeftBankToUp25Seq0, kDiagLeftBankToUp25Seq1, kDiagLeftBankToUp25Seq2,
                            kDiagLeftBankToUp25Seq3 } },
    };

    constexpr auto kTEDDiagRightBankToUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::rightLarge },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagLeftBankToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagRightBankToUp25Seq0, kDiagRightBankToUp25Seq1, kDiagRightBankToUp25Seq2,
                            kDiagRightBankToUp25Seq3 } },
    };

    constexpr auto kTEDDiagUp25ToLeftBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::none },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagUp25ToRightBank,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::up25, TrackRoll::left, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagUp25ToLeftBankSeq0, kDiagUp25ToLeftBankSeq1, kDiagUp25ToLeftBankSeq2,
                            kDiagUp25ToLeftBankSeq3 } },
    };

    constexpr auto kTEDDiagUp25ToRightBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::none },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagUp25ToLeftBank,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::up25, TrackRoll::right, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagUp25ToRightBankSeq0, kDiagUp25ToRightBankSeq1, kDiagUp25ToRightBankSeq2,
                            kDiagUp25ToRightBankSeq3 } },
    };

    constexpr auto kTEDDiagLeftBankToDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::leftLarge },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagRightBankToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagLeftBankToDown25Seq0, kDiagLeftBankToDown25Seq1, kDiagLeftBankToDown25Seq2,
                            kDiagLeftBankToDown25Seq3 } },
    };

    constexpr auto kTEDDiagRightBankToDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::rightLarge },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagLeftBankToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagRightBankToDown25Seq0, kDiagRightBankToDown25Seq1, kDiagRightBankToDown25Seq2,
                            kDiagRightBankToDown25Seq3 } },
    };

    constexpr auto kTEDDiagDown25ToLeftBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::none },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagDown25ToRightBank,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::down25, TrackRoll::left, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagDown25ToLeftBankSeq0, kDiagDown25ToLeftBankSeq1, kDiagDown25ToLeftBankSeq2,
                            kDiagDown25ToLeftBankSeq3 } },
    };

    constexpr auto kTEDDiagDown25ToRightBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::none },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagDown25ToLeftBank,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::down25, TrackRoll::right, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagDown25ToRightBankSeq0, kDiagDown25ToRightBankSeq1, kDiagDown25ToRightBankSeq2,
                            kDiagDown25ToRightBankSeq3 } },
    };

    constexpr auto kTEDDiagLeftBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 96777,
        .mirrorElement = TrackElemType::diagRightBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .sequenceData = { 4, { kDiagLeftBankSeq0, kDiagLeftBankSeq1, kDiagLeftBankSeq2, kDiagLeftBankSeq3 } },
    };

    constexpr auto kTEDDiagRightBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 96777,
        .mirrorElement = TrackElemType::diagLeftBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right, 0 },
        .sequenceData = { 4, { kDiagRightBankSeq0, kDiagRightBankSeq1, kDiagRightBankSeq2, kDiagRightBankSeq3 } },
    };

    constexpr auto kTEDLogFlumeReverser = TrackElementDescriptor{
        .description = STR_REVERSER_TURNTABLE,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 16,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 282624,
        .mirrorElement = TrackElemType::logFlumeReverser,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::logFlumeReverser, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kLogFlumeReverserSeq0 } },
    };

    constexpr auto kTEDSpinningTunnel = TrackElementDescriptor{
        .description = STR_SPINNING_TUNNEL,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 217088,
        .mirrorElement = TrackElemType::spinningTunnel,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::spinningTunnel, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kSpinningTunnelSeq0 } },
    };

    constexpr auto kTEDLeftBarrelRollUpToDown = TrackElementDescriptor{
        .description = STR_BARREL_ROLL_LEFT,
        .coordinates = { 0, 0, 0, 32, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::leftBarrelRollDownToUp, TrackCurve::none },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::rightBarrelRollUpToDown,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::barrelRoll, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<170>,
        .lateralFactor = EvaluatorConst<115>,
        .sequenceData = { 3, { kLeftBarrelRollUpToDownSeq0, kLeftBarrelRollUpToDownSeq1, kLeftBarrelRollUpToDownSeq2 } },
    };

    constexpr auto kTEDRightBarrelRollUpToDown = TrackElementDescriptor{
        .description = STR_BARREL_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, 32, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::rightBarrelRollDownToUp, TrackCurve::none },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::leftBarrelRollUpToDown,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::barrelRoll, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<170>,
        .lateralFactor = EvaluatorConst<-115>,
        .sequenceData = { 3, { kRightBarrelRollUpToDownSeq0, kRightBarrelRollUpToDownSeq1, kRightBarrelRollUpToDownSeq2 } },
    };

    constexpr auto kTEDLeftBarrelRollDownToUp = TrackElementDescriptor{
        .description = STR_BARREL_ROLL_LEFT,
        .coordinates = { 0, 0, 0, -32, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::leftBarrelRollUpToDown },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::rightBarrelRollDownToUp,
        .flags = { TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::barrelRoll, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown, 0 },
        .verticalFactor = EvaluatorConst<170>,
        .lateralFactor = EvaluatorConst<115>,
        .sequenceData = { 3, { kLeftBarrelRollDownToUpSeq0, kLeftBarrelRollDownToUpSeq1, kLeftBarrelRollDownToUpSeq2 } },
    };

    constexpr auto kTEDRightBarrelRollDownToUp = TrackElementDescriptor{
        .description = STR_BARREL_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, -32, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::rightBarrelRollUpToDown },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::leftBarrelRollDownToUp,
        .flags = { TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::barrelRoll, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown, 0 },
        .verticalFactor = EvaluatorConst<170>,
        .lateralFactor = EvaluatorConst<-115>,
        .sequenceData = { 3, { kRightBarrelRollDownToUpSeq0, kRightBarrelRollDownToUpSeq1, kRightBarrelRollDownToUpSeq2 } },
    };

    constexpr auto kTEDLeftBankToLeftQuarterTurn3TilesUp25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 24, -32, -32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::rightBankToRightQuarterTurn3TilesUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorQuarterTurn3Tiles,
        .lateralFactor = EvaluatorConst<90>,
        .sequenceData = { 4,
                          { kLeftBankToLeftQuarterTurn3TilesUp25Seq0, kLeftBankToLeftQuarterTurn3TilesUp25Seq1,
                            kLeftBankToLeftQuarterTurn3TilesUp25Seq2, kLeftBankToLeftQuarterTurn3TilesUp25Seq3 } },
    };

    constexpr auto kTEDRightBankToRightQuarterTurn3TilesUp25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 24, -32, 32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::leftBankToLeftQuarterTurn3TilesUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorQuarterTurn3Tiles,
        .lateralFactor = EvaluatorConst<-90>,
        .sequenceData = { 4,
                          { kRightBankToRightQuarterTurn3TilesUp25Seq0, kRightBankToRightQuarterTurn3TilesUp25Seq1,
                            kRightBankToRightQuarterTurn3TilesUp25Seq2, kRightBankToRightQuarterTurn3TilesUp25Seq3 } },
    };

    constexpr auto kTEDLeftQuarterTurn3TilesDown25ToLeftBank = TrackElementDescriptor{
        .coordinates = { 0, 3, 24, 0, -32, -32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::rightQuarterTurn3TilesDown25ToRightBank,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::down25, TrackRoll::left, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorQuarterTurn3Tiles,
        .lateralFactor = EvaluatorConst<90>,
        .sequenceData = { 4,
                          { kLeftQuarterTurn3TilesDown25ToLeftBankSeq0, kLeftQuarterTurn3TilesDown25ToLeftBankSeq1,
                            kLeftQuarterTurn3TilesDown25ToLeftBankSeq2, kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 } },
    };

    constexpr auto kTEDRightQuarterTurn3TilesDown25ToRightBank = TrackElementDescriptor{
        .coordinates = { 0, 1, 24, 0, -32, 32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::down25, TrackRoll::right, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorQuarterTurn3Tiles,
        .lateralFactor = EvaluatorConst<-90>,
        .sequenceData = { 4,
                          { kRightQuarterTurn3TilesDown25ToRightBankSeq0, kRightQuarterTurn3TilesDown25ToRightBankSeq1,
                            kRightQuarterTurn3TilesDown25ToRightBankSeq2, kRightQuarterTurn3TilesDown25ToRightBankSeq3 } },
    };

    constexpr auto kTEDPoweredLift = TrackElementDescriptor{
        .description = STR_LAUNCHED_LIFT_HILL,
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::poweredLift, TrackElemType::poweredLift },
        .priceModifier = 96384,
        .mirrorElement = TrackElemType::poweredLift,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::poweredLift, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kPoweredLiftSeq0 } },
    };

    constexpr auto kTEDLeftFlyerTwistUp = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_LEFT,
        .coordinates = { 0, 0, 0, -16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::rightFlyerTwistUp,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::inlineTwistUninverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 3, { kLeftFlyerTwistUpSeq0, kLeftFlyerTwistUpSeq1, kLeftFlyerTwistUpSeq2 } },
    };

    constexpr auto kTEDRightFlyerTwistUp = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_RIGHT,
        .coordinates = { 0, 0, 0, -16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::leftFlyerTwistUp,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::inlineTwistUninverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 3, { kRightFlyerTwistUpSeq0, kRightFlyerTwistUpSeq1, kRightFlyerTwistUpSeq2 } },
    };

    constexpr auto kTEDLeftFlyerTwistDown = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_LEFT,
        .coordinates = { 0, 0, 0, 16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::rightFlyerTwistDown,
        .flags = { TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::inlineTwistInverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 3, { kLeftFlyerTwistDownSeq0, kLeftFlyerTwistDownSeq1, kLeftFlyerTwistDownSeq2 } },
    };

    constexpr auto kTEDRightFlyerTwistDown = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_RIGHT,
        .coordinates = { 0, 0, 0, 16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::leftFlyerTwistDown,
        .flags = { TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::inlineTwistInverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 3, { kRightFlyerTwistDownSeq0, kRightFlyerTwistDownSeq1, kRightFlyerTwistDownSeq2 } },
    };

    constexpr auto kTEDMultiDimInvertedFlatToDown90QuarterLoop = TrackElementDescriptor{
        .description = STR_QUARTER_LOOP,
        .coordinates = { 0, 2, 0, -96, -96, 0 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::multiDimInvertedFlatToDown90QuarterLoop,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::quarterLoopInvertedDown, TrackPitch::down90, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorDown90QuarterLoop,
        .sequenceData = { 4,
                          { kMultiDimInvertedFlatToDown90QuarterLoopSeq0, kMultiDimInvertedFlatToDown90QuarterLoopSeq1,
                            kMultiDimInvertedFlatToDown90QuarterLoopSeq2, kMultiDimInvertedFlatToDown90QuarterLoopSeq3 } },
    };

    constexpr auto kTEDUp90ToInvertedFlatQuarterLoop = TrackElementDescriptor{
        .description = STR_QUARTER_LOOP,
        .coordinates = { 0, 2, 0, 128, 64, 0 },
        .pieceLength = 80,
        .curveChain = { TrackElemType::invertedFlatToDown90QuarterLoop, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::up90ToInvertedFlatQuarterLoop,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::quarterLoop, TrackPitch::none, TrackPitch::up90, TrackRoll::upsideDown, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorUp90QuarterLoop,
        .sequenceData = { 3,
                          { kUp90ToInvertedFlatQuarterLoopSeq0, kUp90ToInvertedFlatQuarterLoopSeq1,
                            kUp90ToInvertedFlatQuarterLoopSeq2 } },
    };

    constexpr auto kTEDInvertedFlatToDown90QuarterLoop = TrackElementDescriptor{
        .description = STR_QUARTER_LOOP,
        .coordinates = { 0, 2, 0, -128, -96, 0 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackElemType::up90ToInvertedFlatQuarterLoop },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::invertedFlatToDown90QuarterLoop,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::quarterLoop, TrackPitch::down90, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        0 },
        .verticalFactor = EvaluatorDown90QuarterLoop,
        .sequenceData = { 4,
                          { kInvertedFlatToDown90QuarterLoopSeq0, kInvertedFlatToDown90QuarterLoopSeq1,
                            kInvertedFlatToDown90QuarterLoopSeq2, kInvertedFlatToDown90QuarterLoopSeq3 } },
    };

    constexpr auto kTEDLeftCurvedLiftHill = TrackElementDescriptor{
        .description = STR_CURVED_LIFT_HILL_LEFT,
        .coordinates = { 0, 3, 0, 16, -32, -32 },
        .pieceLength = 64,
        .curveChain = { TrackElemType::leftCurvedLiftHill, TrackElemType::leftCurvedLiftHill },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::rightCurvedLiftHill,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::up },
        .definition = { TrackGroup::liftHillCurved, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l7,
        .lateralFactor = EvaluatorConst<59>,
        .sequenceData = { 4,
                          { kLeftCurvedLiftHillSeq0, kLeftCurvedLiftHillSeq1, kLeftCurvedLiftHillSeq2,
                            kLeftCurvedLiftHillSeq3 } },
    };

    constexpr auto kTEDRightCurvedLiftHill = TrackElementDescriptor{
        .description = STR_CURVED_LIFT_HILL_RIGHT,
        .coordinates = { 0, 1, 0, 16, -32, 32 },
        .pieceLength = 64,
        .curveChain = { TrackElemType::rightCurvedLiftHill, TrackElemType::rightCurvedLiftHill },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::leftCurvedLiftHill,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::up },
        .definition = { TrackGroup::liftHillCurved, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r7,
        .lateralFactor = EvaluatorConst<-59>,
        .sequenceData = { 4,
                          { kRightCurvedLiftHillSeq0, kRightCurvedLiftHillSeq1, kRightCurvedLiftHillSeq2,
                            kRightCurvedLiftHillSeq3 } },
    };

    constexpr auto kTEDLeftReverser = TrackElementDescriptor{
        .description = STR_REVERSER_LEFT,
        .coordinates = { 0, 0, 0, 0, -64, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 393216,
        .mirrorElement = TrackElemType::rightReverser,
        .flags = {},
        .definition = { TrackGroup::reverser, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 6,
                          { kLeftReverserSeq0, kLeftReverserSeq1, kLeftReverserSeq2, kLeftReverserSeq3, kLeftReverserSeq4,
                            kLeftReverserSeq5 } },
    };

    constexpr auto kTEDRightReverser = TrackElementDescriptor{
        .description = STR_REVERSER_RIGHT,
        .coordinates = { 0, 0, 0, 0, -64, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 393216,
        .mirrorElement = TrackElemType::leftReverser,
        .flags = {},
        .definition = { TrackGroup::reverser, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 6,
                          { kRightReverserSeq0, kRightReverserSeq1, kRightReverserSeq2, kRightReverserSeq3, kRightReverserSeq4,
                            kRightReverserSeq5 } },
    };

    constexpr auto kTEDAirThrustTopCap = TrackElementDescriptor{
        .description = STR_TOP_SECTION,
        .coordinates = { 0, 0, 0, 0, -32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::airThrustVerticalDown, TrackElemType::reverseFreefallVertical },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::airThrustTopCap,
        .flags = {},
        .definition = { TrackGroup::slopeToFlat, TrackPitch::down90, TrackPitch::up90, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-60>,
        .sequenceData = { 4, { kAirThrustTopCapSeq0, kAirThrustTopCapSeq1, kAirThrustTopCapSeq2, kAirThrustTopCapSeq3 } },
    };

    constexpr auto kTEDAirThrustVerticalDown = TrackElementDescriptor{
        .description = STR_VERTICAL_TRACK,
        .coordinates = { 0, 0, 80, 0, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::airThrustVerticalDown, TrackElemType::airThrustVerticalDown },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::airThrustVerticalDown,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::slopeToFlat, TrackPitch::down90, TrackPitch::down90, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kAirThrustVerticalDownSeq0, kAirThrustVerticalDownSeq1 } },
    };

    constexpr auto kTEDAirThrustVerticalDownToLevel = TrackElementDescriptor{
        .description = STR_SLOPE_TO_LEVEL,
        .coordinates = { 0, 0, 240, 0, -160, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackElemType::airThrustVerticalDown },
        .priceModifier = 491520,
        .mirrorElement = TrackElemType::airThrustVerticalDownToLevel,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::slopeToFlat, TrackPitch::none, TrackPitch::down90, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<120>,
        .sequenceData = { 7,
                          { kAirThrustVerticalDownToLevelSeq0, kAirThrustVerticalDownToLevelSeq1,
                            kAirThrustVerticalDownToLevelSeq2, kAirThrustVerticalDownToLevelSeq3,
                            kAirThrustVerticalDownToLevelSeq4, kAirThrustVerticalDownToLevelSeq5,
                            kAirThrustVerticalDownToLevelSeq6 } },
    };

    constexpr auto kTEDBlockBrakes = TrackElementDescriptor{
        .description = STR_BLOCK_BRAKES,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 94208,
        .mirrorElement = TrackElemType::blockBrakes,
        .flags = {},
        .definition = { TrackGroup::blockBrakes, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kBlockBrakesSeq0 } },
    };

    constexpr auto kTEDLeftBankedQuarterTurn3TileUp25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 32, -32, -32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 287115,
        .mirrorElement = TrackElemType::rightBankedQuarterTurn3TileUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<100>,
        .sequenceData = { 4,
                          { kLeftBankedQuarterTurn3TileUp25Seq0, kLeftBankedQuarterTurn3TileUp25Seq1,
                            kLeftBankedQuarterTurn3TileUp25Seq2, kLeftBankedQuarterTurn3TileUp25Seq3 } },
    };

    constexpr auto kTEDRightBankedQuarterTurn3TileUp25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 32, -32, 32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 287115,
        .mirrorElement = TrackElemType::leftBankedQuarterTurn3TileUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::right, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-100>,
        .sequenceData = { 4,
                          { kRightBankedQuarterTurn3TileUp25Seq0, kRightBankedQuarterTurn3TileUp25Seq1,
                            kRightBankedQuarterTurn3TileUp25Seq2, kRightBankedQuarterTurn3TileUp25Seq3 } },
    };

    constexpr auto kTEDLeftBankedQuarterTurn3TileDown25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 32, 0, -32, -32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 287115,
        .mirrorElement = TrackElemType::rightBankedQuarterTurn3TileDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<100>,
        .sequenceData = { 4,
                          { kLeftBankedQuarterTurn3TileDown25Seq0, kLeftBankedQuarterTurn3TileDown25Seq1,
                            kLeftBankedQuarterTurn3TileDown25Seq2, kLeftBankedQuarterTurn3TileDown25Seq3 } },
    };

    constexpr auto kTEDRightBankedQuarterTurn3TileDown25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 32, 0, -32, 32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 287115,
        .mirrorElement = TrackElemType::leftBankedQuarterTurn3TileDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::right, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-100>,
        .sequenceData = { 4,
                          { kRightBankedQuarterTurn3TileDown25Seq0, kRightBankedQuarterTurn3TileDown25Seq1,
                            kRightBankedQuarterTurn3TileDown25Seq2, kRightBankedQuarterTurn3TileDown25Seq3 } },
    };

    constexpr auto kTEDLeftBankedQuarterTurn5TileUp25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 64, -64, -64 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 333259,
        .mirrorElement = TrackElemType::rightBankedQuarterTurn5TileUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 7,
                          { kLeftBankedQuarterTurn5TileUp25Seq0, kLeftBankedQuarterTurn5TileUp25Seq1,
                            kLeftBankedQuarterTurn5TileUp25Seq2, kLeftBankedQuarterTurn5TileUp25Seq3,
                            kLeftBankedQuarterTurn5TileUp25Seq4, kLeftBankedQuarterTurn5TileUp25Seq5,
                            kLeftBankedQuarterTurn5TileUp25Seq6, kLeftBankedQuarterTurn5TileUp25Seq7 } },
    };

    constexpr auto kTEDRightBankedQuarterTurn5TileUp25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 64, -64, 64 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 333259,
        .mirrorElement = TrackElemType::leftBankedQuarterTurn5TileUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::right, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 7,
                          { kRightBankedQuarterTurn5TileUp25Seq0, kRightBankedQuarterTurn5TileUp25Seq1,
                            kRightBankedQuarterTurn5TileUp25Seq2, kRightBankedQuarterTurn5TileUp25Seq3,
                            kRightBankedQuarterTurn5TileUp25Seq4, kRightBankedQuarterTurn5TileUp25Seq5,
                            kRightBankedQuarterTurn5TileUp25Seq6, kRightBankedQuarterTurn5TileUp25Seq7 } },
    };

    constexpr auto kTEDLeftBankedQuarterTurn5TileDown25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 64, 0, -64, -64 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 333259,
        .mirrorElement = TrackElemType::rightBankedQuarterTurn5TileDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 7,
                          { kLeftBankedQuarterTurn5TileDown25Seq0, kLeftBankedQuarterTurn5TileDown25Seq1,
                            kLeftBankedQuarterTurn5TileDown25Seq2, kLeftBankedQuarterTurn5TileDown25Seq3,
                            kLeftBankedQuarterTurn5TileDown25Seq4, kLeftBankedQuarterTurn5TileDown25Seq5,
                            kLeftBankedQuarterTurn5TileDown25Seq6, kLeftBankedQuarterTurn5TileDown25Seq7 } },
    };

    constexpr auto kTEDRightBankedQuarterTurn5TileDown25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 64, 0, -64, 64 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 333259,
        .mirrorElement = TrackElemType::leftBankedQuarterTurn5TileDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::right, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 7,
                          { kRightBankedQuarterTurn5TileDown25Seq0, kRightBankedQuarterTurn5TileDown25Seq1,
                            kRightBankedQuarterTurn5TileDown25Seq2, kRightBankedQuarterTurn5TileDown25Seq3,
                            kRightBankedQuarterTurn5TileDown25Seq4, kRightBankedQuarterTurn5TileDown25Seq5,
                            kRightBankedQuarterTurn5TileDown25Seq6, kRightBankedQuarterTurn5TileDown25Seq7 } },
    };

    constexpr auto kTEDUp25ToLeftBankedUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::up25ToRightBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::none, 0 },
        .sequenceData = { 1, { kUp25ToLeftBankedUp25Seq0 } },
    };

    constexpr auto kTEDUp25ToRightBankedUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::up25ToLeftBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::up25, TrackRoll::right, TrackRoll::none,
                        0 },
        .sequenceData = { 1, { kUp25ToRightBankedUp25Seq0 } },
    };

    constexpr auto kTEDLeftBankedUp25ToUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::rightBankedUp25ToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::left, 0 },
        .sequenceData = { 1, { kLeftBankedUp25ToUp25Seq0 } },
    };

    constexpr auto kTEDRightBankedUp25ToUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::leftBankedUp25ToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::right,
                        0 },
        .sequenceData = { 1, { kRightBankedUp25ToUp25Seq0 } },
    };

    constexpr auto kTEDDown25ToLeftBankedDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::down25ToRightBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::down25, TrackRoll::left, TrackRoll::none,
                        0 },
        .sequenceData = { 1, { kDown25ToLeftBankedDown25Seq0 } },
    };

    constexpr auto kTEDDown25ToRightBankedDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::down25ToLeftBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::down25, TrackRoll::right, TrackRoll::none,
                        0 },
        .sequenceData = { 1, { kDown25ToRightBankedDown25Seq0 } },
    };

    constexpr auto kTEDLeftBankedDown25ToDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::rightBankedDown25ToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::left,
                        0 },
        .sequenceData = { 1, { kLeftBankedDown25ToDown25Seq0 } },
    };

    constexpr auto kTEDRightBankedDown25ToDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::leftBankedDown25ToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::right,
                        0 },
        .sequenceData = { 1, { kRightBankedDown25ToDown25Seq0 } },
    };

    constexpr auto kTEDLeftBankedFlatToLeftBankedUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rightBankedFlatToRightBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kLeftBankedFlatToLeftBankedUp25Seq0 } },
    };

    constexpr auto kTEDRightBankedFlatToRightBankedUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::leftBankedFlatToLeftBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kRightBankedFlatToRightBankedUp25Seq0 } },
    };

    constexpr auto kTEDLeftBankedUp25ToLeftBankedFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rightBankedUp25ToRightBankedFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::up25, TrackRoll::left, TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kLeftBankedUp25ToLeftBankedFlatSeq0 } },
    };

    constexpr auto kTEDRightBankedUp25ToRightBankedFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::leftBankedUp25ToLeftBankedFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::up25, TrackRoll::right, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kRightBankedUp25ToRightBankedFlatSeq0 } },
    };

    constexpr auto kTEDLeftBankedFlatToLeftBankedDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rightBankedFlatToRightBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kLeftBankedFlatToLeftBankedDown25Seq0 } },
    };

    constexpr auto kTEDRightBankedFlatToRightBankedDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::leftBankedFlatToLeftBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kRightBankedFlatToRightBankedDown25Seq0 } },
    };

    constexpr auto kTEDLeftBankedDown25ToLeftBankedFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rightBankedDown25ToRightBankedFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::down25, TrackRoll::left, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kLeftBankedDown25ToLeftBankedFlatSeq0 } },
    };

    constexpr auto kTEDRightBankedDown25ToRightBankedFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::leftBankedDown25ToLeftBankedFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::down25, TrackRoll::right, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kRightBankedDown25ToRightBankedFlatSeq0 } },
    };

    constexpr auto kTEDFlatToLeftBankedUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::flatToRightBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::none, TrackRoll::left, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kFlatToLeftBankedUp25Seq0 } },
    };

    constexpr auto kTEDFlatToRightBankedUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::flatToLeftBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::none, TrackRoll::right, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kFlatToRightBankedUp25Seq0 } },
    };

    constexpr auto kTEDLeftBankedUp25ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rightBankedUp25ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::up25, TrackRoll::none, TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kLeftBankedUp25ToFlatSeq0 } },
    };

    constexpr auto kTEDRightBankedUp25ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::leftBankedUp25ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::up25, TrackRoll::none, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kRightBankedUp25ToFlatSeq0 } },
    };

    constexpr auto kTEDFlatToLeftBankedDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::flatToRightBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::none, TrackRoll::left, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kFlatToLeftBankedDown25Seq0 } },
    };

    constexpr auto kTEDFlatToRightBankedDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::flatToLeftBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::none, TrackRoll::right, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kFlatToRightBankedDown25Seq0 } },
    };

    constexpr auto kTEDLeftBankedDown25ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rightBankedDown25ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::down25, TrackRoll::none, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kLeftBankedDown25ToFlatSeq0 } },
    };

    constexpr auto kTEDRightBankedDown25ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::leftBankedDown25ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::down25, TrackRoll::none, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kRightBankedDown25ToFlatSeq0 } },
    };

    constexpr auto kTEDLeftQuarterTurn1TileUp90 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 96, 0, 32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 151552,
        .mirrorElement = TrackElemType::rightQuarterTurn1TileUp90,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::up90, TrackPitch::up90, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kLeftQuarterTurn1TileUp90Seq0, kLeftQuarterTurn1TileUp90Seq1 } },
    };

    constexpr auto kTEDRightQuarterTurn1TileUp90 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 96, 0, -32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 151552,
        .mirrorElement = TrackElemType::leftQuarterTurn1TileUp90,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::up90, TrackPitch::up90, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kRightQuarterTurn1TileUp90Seq0, kRightQuarterTurn1TileUp90Seq1 } },
    };

    constexpr auto kTEDLeftQuarterTurn1TileDown90 = TrackElementDescriptor{
        .coordinates = { 0, 3, 96, 0, 0, 32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 151552,
        .mirrorElement = TrackElemType::rightQuarterTurn1TileDown90,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::down90, TrackPitch::down90, TrackRoll::none, TrackRoll::none,
                        0 },
        .sequenceData = { 2, { kLeftQuarterTurn1TileDown90Seq0, kLeftQuarterTurn1TileDown90Seq1 } },
    };

    constexpr auto kTEDRightQuarterTurn1TileDown90 = TrackElementDescriptor{
        .coordinates = { 0, 1, 96, 0, 0, -32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 151552,
        .mirrorElement = TrackElemType::leftQuarterTurn1TileDown90,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::down90, TrackPitch::down90, TrackRoll::none, TrackRoll::none,
                        0 },
        .sequenceData = { 2, { kRightQuarterTurn1TileDown90Seq0, kRightQuarterTurn1TileDown90Seq1 } },
    };

    constexpr auto kTEDMultiDimUp90ToInvertedFlatQuarterLoop = TrackElementDescriptor{
        .description = STR_QUARTER_LOOP,
        .coordinates = { 0, 2, 0, 96, 64, 0 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::quarterLoopUninvertedUp, TrackPitch::none, TrackPitch::up90, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorUp90QuarterLoop,
        .sequenceData = { 3,
                          { kMultiDimUp90ToInvertedFlatQuarterLoopSeq0, kMultiDimUp90ToInvertedFlatQuarterLoopSeq1,
                            kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 } },
    };

    constexpr auto kTEDMultiDimFlatToDown90QuarterLoop = TrackElementDescriptor{
        .description = STR_QUARTER_LOOP,
        .coordinates = { 0, 2, 0, -128, -96, 0 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::multiDimFlatToDown90QuarterLoop,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::quarterLoopUninvertedDown, TrackPitch::down90, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorDown90QuarterLoop,
        .sequenceData = { 4,
                          { kMultiDimFlatToDown90QuarterLoopSeq0, kMultiDimFlatToDown90QuarterLoopSeq1,
                            kMultiDimFlatToDown90QuarterLoopSeq2, kMultiDimFlatToDown90QuarterLoopSeq3 } },
    };

    constexpr auto kTEDMultiDimInvertedUp90ToFlatQuarterLoop = TrackElementDescriptor{
        .description = STR_QUARTER_LOOP,
        .coordinates = { 0, 2, 0, 128, 64, 0 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::quarterLoopInvertedUp, TrackPitch::none, TrackPitch::up90, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorUp90QuarterLoop,
        .sequenceData = { 3,
                          { kMultiDimInvertedUp90ToFlatQuarterLoopSeq0, kMultiDimInvertedUp90ToFlatQuarterLoopSeq1,
                            kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 } },
    };

    constexpr auto kTEDRotationControlToggle = TrackElementDescriptor{
        .description = STR_SPINNING_CONTROL_TOGGLE_TRACK,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rotationControlToggle,
        .flags = {},
        .definition = { TrackGroup::rotationControlToggle, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none,
                        0 },
        .sequenceData = { 1, { kRotationControlToggleSeq0 } },
    };

    constexpr auto kTEDLeftEighthToDiagUp25 = TrackElementDescriptor{
        .coordinates = { 0, 7, 0, 48, -64, -32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::rightEighthToDiagUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 16 },
        .spinFunction = SpinFunction::l9,
        .lateralFactor = EvaluatorConst<137>,
        .sequenceData = { 5,
                          { kLeftEighthToDiagUp25Seq0, kLeftEighthToDiagUp25Seq1, kLeftEighthToDiagUp25Seq2,
                            kLeftEighthToDiagUp25Seq3, kLeftEighthToDiagUp25Seq4 } },
    };

    constexpr auto kTEDRightEighthToDiagUp25 = TrackElementDescriptor{
        .coordinates = { 0, 4, 0, 48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::leftEighthToDiagUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 16 },
        .spinFunction = SpinFunction::r9,
        .lateralFactor = EvaluatorConst<-137>,
        .sequenceData = { 5,
                          { kRightEighthToDiagUp25Seq0, kRightEighthToDiagUp25Seq1, kRightEighthToDiagUp25Seq2,
                            kRightEighthToDiagUp25Seq3, kRightEighthToDiagUp25Seq4 } },
    };

    constexpr auto kTEDLeftEighthToDiagDown25 = TrackElementDescriptor{
        .coordinates = { 0, 7, 0, -48, -64, -32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::rightEighthToDiagDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        -16 },
        .spinFunction = SpinFunction::l9,
        .lateralFactor = EvaluatorConst<137>,
        .sequenceData = { 5,
                          { kLeftEighthToDiagDown25Seq0, kLeftEighthToDiagDown25Seq1, kLeftEighthToDiagDown25Seq2,
                            kLeftEighthToDiagDown25Seq3, kLeftEighthToDiagDown25Seq4 } },
    };

    constexpr auto kTEDRightEighthToDiagDown25 = TrackElementDescriptor{
        .coordinates = { 0, 4, 0, -48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::leftEighthToDiagDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        -16 },
        .spinFunction = SpinFunction::r9,
        .lateralFactor = EvaluatorConst<-137>,
        .sequenceData = { 5,
                          { kRightEighthToDiagDown25Seq0, kRightEighthToDiagDown25Seq1, kRightEighthToDiagDown25Seq2,
                            kRightEighthToDiagDown25Seq3, kRightEighthToDiagDown25Seq4 } },
    };

    constexpr auto kTEDLeftEighthToOrthogonalUp25 = TrackElementDescriptor{
        .coordinates = { 4, 0, 0, 48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::rightEighthToOrthogonalUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 16 },
        .spinFunction = SpinFunction::l9,
        .lateralFactor = EvaluatorConst<137>,
        .sequenceData = { 5,
                          { kLeftEighthToOrthogonalUp25Seq0, kLeftEighthToOrthogonalUp25Seq1, kLeftEighthToOrthogonalUp25Seq2,
                            kLeftEighthToOrthogonalUp25Seq3, kLeftEighthToOrthogonalUp25Seq4 } },
    };

    constexpr auto kTEDRightEighthToOrthogonalUp25 = TrackElementDescriptor{
        .coordinates = { 4, 1, 0, 48, -32, 64 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::leftEighthToOrthogonalUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 16 },
        .spinFunction = SpinFunction::r9,
        .lateralFactor = EvaluatorConst<-137>,
        .sequenceData = { 5,
                          { kRightEighthToOrthogonalUp25Seq0, kRightEighthToOrthogonalUp25Seq1,
                            kRightEighthToOrthogonalUp25Seq2, kRightEighthToOrthogonalUp25Seq3,
                            kRightEighthToOrthogonalUp25Seq4 } },
    };

    constexpr auto kTEDLeftEighthToOrthogonalDown25 = TrackElementDescriptor{
        .coordinates = { 4, 0, 0, -48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::rightEighthToOrthogonalDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        -16 },
        .spinFunction = SpinFunction::l9,
        .lateralFactor = EvaluatorConst<137>,
        .sequenceData = { 5,
                          { kLeftEighthToOrthogonalDown25Seq0, kLeftEighthToOrthogonalDown25Seq1,
                            kLeftEighthToOrthogonalDown25Seq2, kLeftEighthToOrthogonalDown25Seq3,
                            kLeftEighthToOrthogonalDown25Seq4 } },
    };

    constexpr auto kTEDRightEighthToOrthogonalDown25 = TrackElementDescriptor{
        .coordinates = { 4, 1, 0, -48, -32, 64 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::leftEighthToOrthogonalDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        -16 },
        .spinFunction = SpinFunction::r9,
        .lateralFactor = EvaluatorConst<-137>,
        .sequenceData = { 5,
                          { kRightEighthToOrthogonalDown25Seq0, kRightEighthToOrthogonalDown25Seq1,
                            kRightEighthToOrthogonalDown25Seq2, kRightEighthToOrthogonalDown25Seq3,
                            kRightEighthToOrthogonalDown25Seq4 } },
    };

    constexpr auto kTEDDiagUp25ToLeftBankedUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagUp25ToRightBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::none,
                        0 },
        .sequenceData = { 4,
                          { kDiagUp25ToLeftBankedUp25Seq0, kDiagUp25ToLeftBankedUp25Seq1, kDiagUp25ToLeftBankedUp25Seq2,
                            kDiagUp25ToLeftBankedUp25Seq3 } },
    };

    constexpr auto kTEDDiagUp25ToRightBankedUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagUp25ToLeftBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::right,
                        TrackRoll::none, 0 },
        .sequenceData = { 4,
                          { kDiagUp25ToRightBankedUp25Seq0, kDiagUp25ToRightBankedUp25Seq1, kDiagUp25ToRightBankedUp25Seq2,
                            kDiagUp25ToRightBankedUp25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedUp25ToUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagRightBankedUp25ToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::left,
                        0 },
        .sequenceData = { 4,
                          { kDiagLeftBankedUp25ToUp25Seq0, kDiagLeftBankedUp25ToUp25Seq1, kDiagLeftBankedUp25ToUp25Seq2,
                            kDiagLeftBankedUp25ToUp25Seq3 } },
    };

    constexpr auto kTEDDiagRightBankedUp25ToUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagLeftBankedUp25ToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::none,
                        TrackRoll::right, 0 },
        .sequenceData = { 4,
                          { kDiagRightBankedUp25ToUp25Seq0, kDiagRightBankedUp25ToUp25Seq1, kDiagRightBankedUp25ToUp25Seq2,
                            kDiagRightBankedUp25ToUp25Seq3 } },
    };

    constexpr auto kTEDDiagDown25ToLeftBankedDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagDown25ToRightBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::left,
                        TrackRoll::none, 0 },
        .sequenceData = { 4,
                          { kDiagDown25ToLeftBankedDown25Seq0, kDiagDown25ToLeftBankedDown25Seq1,
                            kDiagDown25ToLeftBankedDown25Seq2, kDiagDown25ToLeftBankedDown25Seq3 } },
    };

    constexpr auto kTEDDiagDown25ToRightBankedDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagDown25ToLeftBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::right,
                        TrackRoll::none, 0 },
        .sequenceData = { 4,
                          { kDiagDown25ToRightBankedDown25Seq0, kDiagDown25ToRightBankedDown25Seq1,
                            kDiagDown25ToRightBankedDown25Seq2, kDiagDown25ToRightBankedDown25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedDown25ToDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagRightBankedDown25ToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::none,
                        TrackRoll::left, 0 },
        .sequenceData = { 4,
                          { kDiagLeftBankedDown25ToDown25Seq0, kDiagLeftBankedDown25ToDown25Seq1,
                            kDiagLeftBankedDown25ToDown25Seq2, kDiagLeftBankedDown25ToDown25Seq3 } },
    };

    constexpr auto kTEDDiagRightBankedDown25ToDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagLeftBankedDown25ToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::none,
                        TrackRoll::right, 0 },
        .sequenceData = { 4,
                          { kDiagRightBankedDown25ToDown25Seq0, kDiagRightBankedDown25ToDown25Seq1,
                            kDiagRightBankedDown25ToDown25Seq2, kDiagRightBankedDown25ToDown25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedFlatToLeftBankedUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagRightBankedFlatToRightBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagLeftBankedFlatToLeftBankedUp25Seq0, kDiagLeftBankedFlatToLeftBankedUp25Seq1,
                            kDiagLeftBankedFlatToLeftBankedUp25Seq2, kDiagLeftBankedFlatToLeftBankedUp25Seq3 } },
    };

    constexpr auto kTEDDiagRightBankedFlatToRightBankedUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagLeftBankedFlatToLeftBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::none, TrackRoll::right,
                        TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagRightBankedFlatToRightBankedUp25Seq0, kDiagRightBankedFlatToRightBankedUp25Seq1,
                            kDiagRightBankedFlatToRightBankedUp25Seq2, kDiagRightBankedFlatToRightBankedUp25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedUp25ToLeftBankedFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagRightBankedUp25ToRightBankedFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::up25, TrackRoll::left, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagLeftBankedUp25ToLeftBankedFlatSeq0, kDiagLeftBankedUp25ToLeftBankedFlatSeq1,
                            kDiagLeftBankedUp25ToLeftBankedFlatSeq2, kDiagLeftBankedUp25ToLeftBankedFlatSeq3 } },
    };

    constexpr auto kTEDDiagRightBankedUp25ToRightBankedFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagLeftBankedUp25ToLeftBankedFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::up25, TrackRoll::right,
                        TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagRightBankedUp25ToRightBankedFlatSeq0, kDiagRightBankedUp25ToRightBankedFlatSeq1,
                            kDiagRightBankedUp25ToRightBankedFlatSeq2, kDiagRightBankedUp25ToRightBankedFlatSeq3 } },
    };

    constexpr auto kTEDDiagLeftBankedFlatToLeftBankedDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::
            diagRightBankedFlatToRightBankedDown25, // TrackElemType::diagRightBankedFlatToRightBankedDown25
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::none, TrackRoll::left,
                        TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagLeftBankedFlatToLeftBankedDown25Seq0, kDiagLeftBankedFlatToLeftBankedDown25Seq1,
                            kDiagLeftBankedFlatToLeftBankedDown25Seq2, kDiagLeftBankedFlatToLeftBankedDown25Seq3 } },
    };

    constexpr auto kTEDDiagRightBankedFlatToRightBankedDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagLeftBankedFlatToLeftBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::none, TrackRoll::right,
                        TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagRightBankedFlatToRightBankedDown25Seq0, kDiagRightBankedFlatToRightBankedDown25Seq1,
                            kDiagRightBankedFlatToRightBankedDown25Seq2, kDiagRightBankedFlatToRightBankedDown25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedDown25ToLeftBankedFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::
            diagRightBankedDown25ToRightBankedFlat, // TrackElemType::diagRightBankedDown25ToRightBankedFlat
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::down25, TrackRoll::left,
                        TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagLeftBankedDown25ToLeftBankedFlatSeq0, kDiagLeftBankedDown25ToLeftBankedFlatSeq1,
                            kDiagLeftBankedDown25ToLeftBankedFlatSeq2, kDiagLeftBankedDown25ToLeftBankedFlatSeq3 } },
    };

    constexpr auto kTEDDiagRightBankedDown25ToRightBankedFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagLeftBankedDown25ToLeftBankedFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::down25, TrackRoll::right,
                        TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagRightBankedDown25ToRightBankedFlatSeq0, kDiagRightBankedDown25ToRightBankedFlatSeq1,
                            kDiagRightBankedDown25ToRightBankedFlatSeq2, kDiagRightBankedDown25ToRightBankedFlatSeq3 } },
    };

    constexpr auto kTEDDiagFlatToLeftBankedUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagFlatToRightBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::none, TrackRoll::left, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagFlatToLeftBankedUp25Seq0, kDiagFlatToLeftBankedUp25Seq1, kDiagFlatToLeftBankedUp25Seq2,
                            kDiagFlatToLeftBankedUp25Seq3 } },
    };

    constexpr auto kTEDDiagFlatToRightBankedUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagFlatToLeftBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::none, TrackRoll::right,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagFlatToRightBankedUp25Seq0, kDiagFlatToRightBankedUp25Seq1, kDiagFlatToRightBankedUp25Seq2,
                            kDiagFlatToRightBankedUp25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedUp25ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagRightBankedUp25ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::up25, TrackRoll::none, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagLeftBankedUp25ToFlatSeq0, kDiagLeftBankedUp25ToFlatSeq1, kDiagLeftBankedUp25ToFlatSeq2,
                            kDiagLeftBankedUp25ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagRightBankedUp25ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagLeftBankedUp25ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::up25, TrackRoll::none,
                        TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagRightBankedUp25ToFlatSeq0, kDiagRightBankedUp25ToFlatSeq1, kDiagRightBankedUp25ToFlatSeq2,
                            kDiagRightBankedUp25ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagFlatToLeftBankedDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagFlatToRightBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::none, TrackRoll::left,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagFlatToLeftBankedDown25Seq0, kDiagFlatToLeftBankedDown25Seq1, kDiagFlatToLeftBankedDown25Seq2,
                            kDiagFlatToLeftBankedDown25Seq3 } },
    };

    constexpr auto kTEDDiagFlatToRightBankedDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagFlatToLeftBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::none, TrackRoll::right,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagFlatToRightBankedDown25Seq0, kDiagFlatToRightBankedDown25Seq1,
                            kDiagFlatToRightBankedDown25Seq2, kDiagFlatToRightBankedDown25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedDown25ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagRightBankedDown25ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::down25, TrackRoll::none,
                        TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagLeftBankedDown25ToFlatSeq0, kDiagLeftBankedDown25ToFlatSeq1, kDiagLeftBankedDown25ToFlatSeq2,
                            kDiagLeftBankedDown25ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagRightBankedDown25ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagLeftBankedDown25ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::down25, TrackRoll::none,
                        TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagRightBankedDown25ToFlatSeq0, kDiagRightBankedDown25ToFlatSeq1,
                            kDiagRightBankedDown25ToFlatSeq2, kDiagRightBankedDown25ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagUp25LeftBanked = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagUp25RightBanked,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::left,
                        0 },
        .sequenceData = { 4,
                          { kDiagUp25LeftBankedSeq0, kDiagUp25LeftBankedSeq1, kDiagUp25LeftBankedSeq2,
                            kDiagUp25LeftBankedSeq3 } },
    };

    constexpr auto kTEDDiagUp25RightBanked = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagUp25LeftBanked,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::right,
                        TrackRoll::right, 0 },
        .sequenceData = { 4,
                          { kDiagUp25RightBankedSeq0, kDiagUp25RightBankedSeq1, kDiagUp25RightBankedSeq2,
                            kDiagUp25RightBankedSeq3 } },
    };

    constexpr auto kTEDDiagDown25LeftBanked = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagDown25RightBanked,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::left,
                        TrackRoll::left, 0 },
        .sequenceData = { 4,
                          { kDiagDown25LeftBankedSeq0, kDiagDown25LeftBankedSeq1, kDiagDown25LeftBankedSeq2,
                            kDiagDown25LeftBankedSeq3 } },
    };

    constexpr auto kTEDDiagDown25RightBanked = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagDown25LeftBanked,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::right,
                        TrackRoll::right, 0 },
        .sequenceData = { 4,
                          { kDiagDown25RightBankedSeq0, kDiagDown25RightBankedSeq1, kDiagDown25RightBankedSeq2,
                            kDiagDown25RightBankedSeq3 } },
    };

    constexpr auto kTEDLeftEighthBankToDiagUp25 = TrackElementDescriptor{
        .coordinates = { 0, 7, 0, 48, -64, -32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::rightEighthBankToDiagUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::left,
                        16 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<200>,
        .sequenceData = { 5,
                          { kLeftEighthBankToDiagUp25Seq0, kLeftEighthBankToDiagUp25Seq1, kLeftEighthBankToDiagUp25Seq2,
                            kLeftEighthBankToDiagUp25Seq3, kLeftEighthBankToDiagUp25Seq4 } },
    };

    constexpr auto kTEDRightEighthBankToDiagUp25 = TrackElementDescriptor{
        .coordinates = { 0, 4, 0, 48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::leftEighthBankToDiagUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::right,
                        TrackRoll::right, 16 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<-200>,
        .sequenceData = { 5,
                          { kRightEighthBankToDiagUp25Seq0, kRightEighthBankToDiagUp25Seq1, kRightEighthBankToDiagUp25Seq2,
                            kRightEighthBankToDiagUp25Seq3, kRightEighthBankToDiagUp25Seq4 } },
    };

    constexpr auto kTEDLeftEighthBankToDiagDown25 = TrackElementDescriptor{
        .coordinates = { 0, 7, 0, -48, -64, -32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::rightEighthBankToDiagDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::left,
                        TrackRoll::left, -16 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<200>,
        .sequenceData = { 5,
                          { kLeftEighthBankToDiagDown25Seq0, kLeftEighthBankToDiagDown25Seq1, kLeftEighthBankToDiagDown25Seq2,
                            kLeftEighthBankToDiagDown25Seq3, kLeftEighthBankToDiagDown25Seq4 } },
    };

    constexpr auto kTEDRightEighthBankToDiagDown25 = TrackElementDescriptor{
        .coordinates = { 0, 4, 0, -48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::leftEighthBankToDiagDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::right,
                        TrackRoll::right, -16 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<-200>,
        .sequenceData = { 5,
                          { kRightEighthBankToDiagDown25Seq0, kRightEighthBankToDiagDown25Seq1,
                            kRightEighthBankToDiagDown25Seq2, kRightEighthBankToDiagDown25Seq3,
                            kRightEighthBankToDiagDown25Seq4 } },
    };

    constexpr auto kTEDLeftEighthBankToOrthogonalUp25 = TrackElementDescriptor{
        .coordinates = { 4, 0, 0, 48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::rightEighthBankToOrthogonalUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::left,
                        16 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<200>,
        .sequenceData = { 5,
                          { kLeftEighthBankToOrthogonalUp25Seq0, kLeftEighthBankToOrthogonalUp25Seq1,
                            kLeftEighthBankToOrthogonalUp25Seq2, kLeftEighthBankToOrthogonalUp25Seq3,
                            kLeftEighthBankToOrthogonalUp25Seq4 } },
    };

    constexpr auto kTEDRightEighthBankToOrthogonalUp25 = TrackElementDescriptor{
        .coordinates = { 4, 1, 0, 48, -32, 64 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::leftEighthBankToOrthogonalUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::right,
                        TrackRoll::right, 16 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<-200>,
        .sequenceData = { 5,
                          { kRightEighthBankToOrthogonalUp25Seq0, kRightEighthBankToOrthogonalUp25Seq1,
                            kRightEighthBankToOrthogonalUp25Seq2, kRightEighthBankToOrthogonalUp25Seq3,
                            kRightEighthBankToOrthogonalUp25Seq4 } },
    };

    constexpr auto kTEDLeftEighthBankToOrthogonalDown25 = TrackElementDescriptor{
        .coordinates = { 4, 0, 0, -48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::rightEighthBankToOrthogonalDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::left,
                        TrackRoll::left, -16 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<200>,
        .sequenceData = { 5,
                          { kLeftEighthBankToOrthogonalDown25Seq0, kLeftEighthBankToOrthogonalDown25Seq1,
                            kLeftEighthBankToOrthogonalDown25Seq2, kLeftEighthBankToOrthogonalDown25Seq3,
                            kLeftEighthBankToOrthogonalDown25Seq4 } },
    };

    constexpr auto kTEDRightEighthBankToOrthogonalDown25 = TrackElementDescriptor{
        .coordinates = { 4, 1, 0, -48, -32, 64 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::leftEighthBankToOrthogonalDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::right,
                        TrackRoll::right, -16 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<-200>,
        .sequenceData = { 5,
                          { kRightEighthBankToOrthogonalDown25Seq0, kRightEighthBankToOrthogonalDown25Seq1,
                            kRightEighthBankToOrthogonalDown25Seq2, kRightEighthBankToOrthogonalDown25Seq3,
                            kRightEighthBankToOrthogonalDown25Seq4 } },
    };

    constexpr auto kTEDDiagBrakes = TrackElementDescriptor{
        .description = STR_BRAKES,
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackElemType::diagBrakes, TrackElemType::diagBrakes },
        .priceModifier = 123456,
        .mirrorElement = TrackElemType::diagBrakes,
        .flags = {},
        .definition = { TrackGroup::diagBrakes, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagBrakesSeq0, kDiagBrakesSeq1, kDiagBrakesSeq2, kDiagBrakesSeq3 } },
    };

    constexpr auto kTEDDiagBlockBrakes = TrackElementDescriptor{
        .description = STR_BLOCK_BRAKES,
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 123456,
        .mirrorElement = TrackElemType::diagBlockBrakes,
        .flags = {},
        .definition = { TrackGroup::diagBlockBrakes, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagBlockBrakesSeq0, kDiagBlockBrakesSeq1, kDiagBlockBrakesSeq2, kDiagBlockBrakesSeq3 } },
    };

    constexpr auto kTEDDown25Brakes = TrackElementDescriptor{
        .description = STR_BRAKES,
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackElemType::down25Brakes, TrackElemType::down25Brakes },
        .priceModifier = 109824,
        .mirrorElement = TrackElemType::down25Brakes,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::inclinedBrakes, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        0 },
        .sequenceData = { 1, { kDown25Seq0 } },
    };

    constexpr auto kTEDDiagBooster = TrackElementDescriptor{
        .description = STR_BOOSTER,
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackElemType::diagBooster, TrackElemType::diagBooster },
        .priceModifier = 123456,
        .mirrorElement = TrackElemType::diagBooster,
        .flags = {},
        .definition = { TrackGroup::diagBooster, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagFlatSeq0, kDiagFlatSeq1, kDiagFlatSeq2, kDiagFlatSeq3 } },
    };

    constexpr auto kTEDDiagFlatToUp60LongBase = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 88, -96, 96 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 399690,
        .mirrorElement = TrackElemType::diagFlatToUp60LongBase,
        .flags = { TrackElementFlag::up, TrackElementFlag::isSteepUp },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::up60, TrackPitch::none, TrackRoll::none, TrackRoll::none, 16 },
        .verticalFactor = EvaluatorConst<180>,
        .sequenceData = { 10,
                          { kDiagFlatToUp60LongBaseSeq0, kDiagFlatToUp60LongBaseSeq1, kDiagFlatToUp60LongBaseSeq2,
                            kDiagFlatToUp60LongBaseSeq3, kDiagFlatToUp60LongBaseSeq4, kDiagFlatToUp60LongBaseSeq5,
                            kDiagFlatToUp60LongBaseSeq6, kDiagFlatToUp60LongBaseSeq7, kDiagFlatToUp60LongBaseSeq8,
                            kDiagFlatToUp60LongBaseSeq9 } },
    };

    constexpr auto kTEDDiagUp60ToFlatLongBase = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 88, -96, 96 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 399690,
        .mirrorElement = TrackElemType::diagUp60ToFlatLongBase,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::none, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 48 },
        .verticalFactor = EvaluatorConst<-180>,
        .sequenceData = { 10,
                          { kDiagUp60ToFlatLongBaseSeq0, kDiagUp60ToFlatLongBaseSeq1, kDiagUp60ToFlatLongBaseSeq2,
                            kDiagUp60ToFlatLongBaseSeq3, kDiagUp60ToFlatLongBaseSeq4, kDiagUp60ToFlatLongBaseSeq5,
                            kDiagUp60ToFlatLongBaseSeq6, kDiagUp60ToFlatLongBaseSeq7, kDiagUp60ToFlatLongBaseSeq8,
                            kDiagUp60ToFlatLongBaseSeq9 } },
    };

    constexpr auto kTEDDiagFlatToDown60LongBase = TrackElementDescriptor{
        .coordinates = { 4, 4, 88, 0, -96, 96 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 399690,
        .mirrorElement = TrackElemType::diagFlatToDown60LongBase,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::none,
                        -32 },
        .verticalFactor = EvaluatorConst<-180>,
        .sequenceData = { 10,
                          { kDiagFlatToDown60LongBaseSeq0, kDiagFlatToDown60LongBaseSeq1, kDiagFlatToDown60LongBaseSeq2,
                            kDiagFlatToDown60LongBaseSeq3, kDiagFlatToDown60LongBaseSeq4, kDiagFlatToDown60LongBaseSeq5,
                            kDiagFlatToDown60LongBaseSeq6, kDiagFlatToDown60LongBaseSeq7, kDiagFlatToDown60LongBaseSeq8,
                            kDiagFlatToDown60LongBaseSeq9 } },
    };

    constexpr auto kTEDDiagDown60ToFlatLongBase = TrackElementDescriptor{
        .coordinates = { 4, 4, 88, 0, -96, 96 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 399690,
        .mirrorElement = TrackElemType::diagDown60ToFlatLongBase,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::none, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        -24 },
        .verticalFactor = EvaluatorConst<180>,
        .sequenceData = { 10,
                          { kDiagDown60ToFlatLongBaseSeq0, kDiagDown60ToFlatLongBaseSeq1, kDiagDown60ToFlatLongBaseSeq2,
                            kDiagDown60ToFlatLongBaseSeq3, kDiagDown60ToFlatLongBaseSeq4, kDiagDown60ToFlatLongBaseSeq5,
                            kDiagDown60ToFlatLongBaseSeq6, kDiagDown60ToFlatLongBaseSeq7, kDiagDown60ToFlatLongBaseSeq8,
                            kDiagDown60ToFlatLongBaseSeq9 } },
    };

    constexpr auto kTEDDiagDown25Brakes = TrackElementDescriptor{
        .description = STR_BRAKES,
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackElemType::diagDown25Brakes, TrackElemType::diagDown25Brakes },
        .priceModifier = 109824,
        .mirrorElement = TrackElemType::diagDown25Brakes,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::inclinedBrakes, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        0 },
        .sequenceData = { 4, { kDiagDown25Seq0, kDiagDown25Seq1, kDiagDown25Seq2, kDiagDown25Seq3 } },
    };

    static constexpr auto kTrackElementDescriptors = std::to_array<TrackElementDescriptor>({
        kTEDFlat,
        kTEDEndStation,
        kTEDBeginStation,
        kTEDMiddleStation,
        kTEDUp25,
        kTEDUp60,
        kTEDFlatToUp25,
        kTEDUp25ToUp60,
        kTEDUp60ToUp25,
        kTEDUp25ToFlat,
        kTEDDown25,
        kTEDDown60,
        kTEDFlatToDown25,
        kTEDDown25ToDown60,
        kTEDDown60ToDown25,
        kTEDDown25ToFlat,
        kTEDLeftQuarterTurn5Tiles,
        kTEDRightQuarterTurn5Tiles,
        kTEDFlatToLeftBank,
        kTEDFlatToRightBank,
        kTEDLeftBankToFlat,
        kTEDRightBankToFlat,
        kTEDBankedLeftQuarterTurn5Tiles,
        kTEDBankedRightQuarterTurn5Tiles,
        kTEDLeftBankToUp25,
        kTEDRightBankToUp25,
        kTEDUp25ToLeftBank,
        kTEDUp25ToRightBank,
        kTEDLeftBankToDown25,
        kTEDRightBankToDown25,
        kTEDDown25ToLeftBank,
        kTEDDown25ToRightBank,
        kTEDLeftBank,
        kTEDRightBank,
        kTEDLeftQuarterTurn5TilesUp25,
        kTEDRightQuarterTurn5TilesUp25,
        kTEDLeftQuarterTurn5TilesDown25,
        kTEDRightQuarterTurn5TilesDown25,
        kTEDSBendLeft,
        kTEDSBendRight,
        kTEDLeftVerticalLoop,
        kTEDRightVerticalLoop,
        kTEDLeftQuarterTurn3Tiles,
        kTEDRightQuarterTurn3Tiles,
        kTEDLeftBankedQuarterTurn3Tiles,
        kTEDRightBankedQuarterTurn3Tiles,
        kTEDLeftQuarterTurn3TilesUp25,
        kTEDRightQuarterTurn3TilesUp25,
        kTEDLeftQuarterTurn3TilesDown25,
        kTEDRightQuarterTurn3TilesDown25,
        kTEDLeftQuarterTurn1Tile,
        kTEDRightQuarterTurn1Tile,
        kTEDLeftTwistDownToUp,
        kTEDRightTwistDownToUp,
        kTEDLeftTwistUpToDown,
        kTEDRightTwistUpToDown,
        kTEDHalfLoopUp,
        kTEDHalfLoopDown,
        kTEDLeftCorkscrewUp,
        kTEDRightCorkscrewUp,
        kTEDLeftCorkscrewDown,
        kTEDRightCorkscrewDown,
        kTEDFlatToUp60,
        kTEDUp60ToFlat,
        kTEDFlatToDown60,
        kTEDDown60ToFlat,
        kTEDTowerBase,
        kTEDTowerSection,
        kTEDFlatCovered,
        kTEDUp25Covered,
        kTEDUp60Covered,
        kTEDFlatToUp25Covered,
        kTEDUp25ToUp60Covered,
        kTEDUp60ToUp25Covered,
        kTEDUp25ToFlatCovered,
        kTEDDown25Covered,
        kTEDDown60Covered,
        kTEDFlatToDown25Covered,
        kTEDDown25ToDown60Covered,
        kTEDDown60ToDown25Covered,
        kTEDDown25ToFlatCovered,
        kTEDLeftQuarterTurn5TilesCovered,
        kTEDRightQuarterTurn5TilesCovered,
        kTEDSBendLeftCovered,
        kTEDSBendRightCovered,
        kTEDLeftQuarterTurn3TilesCovered,
        kTEDRightQuarterTurn3TilesCovered,
        kTEDLeftHalfBankedHelixUpSmall,
        kTEDRightHalfBankedHelixUpSmall,
        kTEDLeftHalfBankedHelixDownSmall,
        kTEDRightHalfBankedHelixDownSmall,
        kTEDLeftHalfBankedHelixUpLarge,
        kTEDRightHalfBankedHelixUpLarge,
        kTEDLeftHalfBankedHelixDownLarge,
        kTEDRightHalfBankedHelixDownLarge,
        kTEDLeftQuarterTurn1TileUp60,
        kTEDRightQuarterTurn1TileUp60,
        kTEDLeftQuarterTurn1TileDown60,
        kTEDRightQuarterTurn1TileDown60,
        kTEDBrakes,
        kTEDBooster,
        kTEDMaze,
        kTEDLeftQuarterBankedHelixLargeUp,
        kTEDRightQuarterBankedHelixLargeUp,
        kTEDLeftQuarterBankedHelixLargeDown,
        kTEDRightQuarterBankedHelixLargeDown,
        kTEDLeftQuarterHelixLargeUp,
        kTEDRightQuarterHelixLargeUp,
        kTEDLeftQuarterHelixLargeDown,
        kTEDRightQuarterHelixLargeDown,
        kTEDUp25LeftBanked,
        kTEDUp25RightBanked,
        kTEDWaterfall,
        kTEDRapids,
        kTEDOnRidePhoto,
        kTEDDown25LeftBanked,
        kTEDDown25RightBanked,
        kTEDWaterSplash,
        kTEDFlatToUp60LongBase,
        kTEDUp60ToFlatLongBase,
        kTEDWhirlpool,
        kTEDDown60ToFlatLongBase,
        kTEDFlatToDown60LongBase,
        kTEDCableLiftHill,
        kTEDReverseFreefallSlope,
        kTEDReverseFreefallVertical,
        kTEDUp90,
        kTEDDown90,
        kTEDUp60ToUp90,
        kTEDDown90ToDown60,
        kTEDUp90ToUp60,
        kTEDDown60ToDown90,
        kTEDBrakeForDrop,
        kTEDLeftEighthToDiag,
        kTEDRightEighthToDiag,
        kTEDLeftEighthToOrthogonal,
        kTEDRightEighthToOrthogonal,
        kTEDLeftEighthBankToDiag,
        kTEDRightEighthBankToDiag,
        kTEDLeftEighthBankToOrthogonal,
        kTEDRightEighthBankToOrthogonal,
        kTEDDiagFlat,
        kTEDDiagUp25,
        kTEDDiagUp60,
        kTEDDiagFlatToUp25,
        kTEDDiagUp25ToUp60,
        kTEDDiagUp60ToUp25,
        kTEDDiagUp25ToFlat,
        kTEDDiagDown25,
        kTEDDiagDown60,
        kTEDDiagFlatToDown25,
        kTEDDiagDown25ToDown60,
        kTEDDiagDown60ToDown25,
        kTEDDiagDown25ToFlat,
        kTEDDiagFlatToUp60,
        kTEDDiagUp60ToFlat,
        kTEDDiagFlatToDown60,
        kTEDDiagDown60ToFlat,
        kTEDDiagFlatToLeftBank,
        kTEDDiagFlatToRightBank,
        kTEDDiagLeftBankToFlat,
        kTEDDiagRightBankToFlat,
        kTEDDiagLeftBankToUp25,
        kTEDDiagRightBankToUp25,
        kTEDDiagUp25ToLeftBank,
        kTEDDiagUp25ToRightBank,
        kTEDDiagLeftBankToDown25,
        kTEDDiagRightBankToDown25,
        kTEDDiagDown25ToLeftBank,
        kTEDDiagDown25ToRightBank,
        kTEDDiagLeftBank,
        kTEDDiagRightBank,
        kTEDLogFlumeReverser,
        kTEDSpinningTunnel,
        kTEDLeftBarrelRollUpToDown,
        kTEDRightBarrelRollUpToDown,
        kTEDLeftBarrelRollDownToUp,
        kTEDRightBarrelRollDownToUp,
        kTEDLeftBankToLeftQuarterTurn3TilesUp25,
        kTEDRightBankToRightQuarterTurn3TilesUp25,
        kTEDLeftQuarterTurn3TilesDown25ToLeftBank,
        kTEDRightQuarterTurn3TilesDown25ToRightBank,
        kTEDPoweredLift,
        kTEDLeftLargeHalfLoopUp,
        kTEDRightLargeHalfLoopUp,
        kTEDLeftLargeHalfLoopDown,
        kTEDRightLargeHalfLoopDown,
        kTEDLeftFlyerTwistUp,
        kTEDRightFlyerTwistUp,
        kTEDLeftFlyerTwistDown,
        kTEDRightFlyerTwistDown,
        kTEDFlyerHalfLoopUninvertedUp,
        kTEDFlyerHalfLoopInvertedDown,
        kTEDLeftFlyerCorkscrewUp,
        kTEDRightFlyerCorkscrewUp,
        kTEDLeftFlyerCorkscrewDown,
        kTEDRightFlyerCorkscrewDown,
        kTEDHeartLineTransferUp,
        kTEDHeartLineTransferDown,
        kTEDLeftHeartLineRoll,
        kTEDRightHeartLineRoll,
        kTEDMinigolfHoleA,
        kTEDMinigolfHoleB,
        kTEDMinigolfHoleC,
        kTEDMinigolfHoleD,
        kTEDMinigolfHoleE,
        kTEDMultiDimInvertedFlatToDown90QuarterLoop,
        kTEDUp90ToInvertedFlatQuarterLoop,
        kTEDInvertedFlatToDown90QuarterLoop,
        kTEDLeftCurvedLiftHill,
        kTEDRightCurvedLiftHill,
        kTEDLeftReverser,
        kTEDRightReverser,
        kTEDAirThrustTopCap,
        kTEDAirThrustVerticalDown,
        kTEDAirThrustVerticalDownToLevel,
        kTEDBlockBrakes,
        kTEDLeftBankedQuarterTurn3TileUp25,
        kTEDRightBankedQuarterTurn3TileUp25,
        kTEDLeftBankedQuarterTurn3TileDown25,
        kTEDRightBankedQuarterTurn3TileDown25,
        kTEDLeftBankedQuarterTurn5TileUp25,
        kTEDRightBankedQuarterTurn5TileUp25,
        kTEDLeftBankedQuarterTurn5TileDown25,
        kTEDRightBankedQuarterTurn5TileDown25,
        kTEDUp25ToLeftBankedUp25,
        kTEDUp25ToRightBankedUp25,
        kTEDLeftBankedUp25ToUp25,
        kTEDRightBankedUp25ToUp25,
        kTEDDown25ToLeftBankedDown25,
        kTEDDown25ToRightBankedDown25,
        kTEDLeftBankedDown25ToDown25,
        kTEDRightBankedDown25ToDown25,
        kTEDLeftBankedFlatToLeftBankedUp25,
        kTEDRightBankedFlatToRightBankedUp25,
        kTEDLeftBankedUp25ToLeftBankedFlat,
        kTEDRightBankedUp25ToRightBankedFlat,
        kTEDLeftBankedFlatToLeftBankedDown25,
        kTEDRightBankedFlatToRightBankedDown25,
        kTEDLeftBankedDown25ToLeftBankedFlat,
        kTEDRightBankedDown25ToRightBankedFlat,
        kTEDFlatToLeftBankedUp25,
        kTEDFlatToRightBankedUp25,
        kTEDLeftBankedUp25ToFlat,
        kTEDRightBankedUp25ToFlat,
        kTEDFlatToLeftBankedDown25,
        kTEDFlatToRightBankedDown25,
        kTEDLeftBankedDown25ToFlat,
        kTEDRightBankedDown25ToFlat,
        kTEDLeftQuarterTurn1TileUp90,
        kTEDRightQuarterTurn1TileUp90,
        kTEDLeftQuarterTurn1TileDown90,
        kTEDRightQuarterTurn1TileDown90,
        kTEDMultiDimUp90ToInvertedFlatQuarterLoop,
        kTEDMultiDimFlatToDown90QuarterLoop,
        kTEDMultiDimInvertedUp90ToFlatQuarterLoop,
        kTEDRotationControlToggle,
        kTEDFlatTrack1x4A,
        kTEDFlatTrack2x2,
        kTEDFlatTrack4x4,
        kTEDFlatTrack2x4,
        kTEDFlatTrack1x5,
        kTEDFlatTrack1x1A,
        kTEDFlatTrack1x4B,
        kTEDFlatTrack1x1B,
        kTEDFlatTrack1x4C,
        kTEDFlatTrack3x3,
        kTEDLeftLargeCorkscrewUp,
        kTEDRightLargeCorkscrewUp,
        kTEDLeftLargeCorkscrewDown,
        kTEDRightLargeCorkscrewDown,
        kTEDLeftMediumHalfLoopUp,
        kTEDRightMediumHalfLoopUp,
        kTEDLeftMediumHalfLoopDown,
        kTEDRightMediumHalfLoopDown,
        kTEDLeftZeroGRollUp,
        kTEDRightZeroGRollUp,
        kTEDLeftZeroGRollDown,
        kTEDRightZeroGRollDown,
        kTEDLeftLargeZeroGRollUp,
        kTEDRightLargeZeroGRollUp,
        kTEDLeftLargeZeroGRollDown,
        kTEDRightLargeZeroGRollDown,
        kTEDLeftFlyerLargeHalfLoopUninvertedUp,
        kTEDRightFlyerLargeHalfLoopUninvertedUp,
        kTEDLeftFlyerLargeHalfLoopInvertedDown,
        kTEDRightFlyerLargeHalfLoopInvertedDown,
        kTEDLeftFlyerLargeHalfLoopInvertedUp,
        kTEDRightFlyerLargeHalfLoopInvertedUp,
        kTEDLeftFlyerLargeHalfLoopUninvertedDown,
        kTEDRightFlyerLargeHalfLoopUninvertedDown,
        kTEDFlyerHalfLoopInvertedUp,
        kTEDFlyerHalfLoopUninvertedDown,
        kTEDLeftEighthToDiagUp25,
        kTEDRightEighthToDiagUp25,
        kTEDLeftEighthToDiagDown25,
        kTEDRightEighthToDiagDown25,
        kTEDLeftEighthToOrthogonalUp25,
        kTEDRightEighthToOrthogonalUp25,
        kTEDLeftEighthToOrthogonalDown25,
        kTEDRightEighthToOrthogonalDown25,
        kTEDDiagUp25ToLeftBankedUp25,
        kTEDDiagUp25ToRightBankedUp25,
        kTEDDiagLeftBankedUp25ToUp25,
        kTEDDiagRightBankedUp25ToUp25,
        kTEDDiagDown25ToLeftBankedDown25,
        kTEDDiagDown25ToRightBankedDown25,
        kTEDDiagLeftBankedDown25ToDown25,
        kTEDDiagRightBankedDown25ToDown25,
        kTEDDiagLeftBankedFlatToLeftBankedUp25,
        kTEDDiagRightBankedFlatToRightBankedUp25,
        kTEDDiagLeftBankedUp25ToLeftBankedFlat,
        kTEDDiagRightBankedUp25ToRightBankedFlat,
        kTEDDiagLeftBankedFlatToLeftBankedDown25,
        kTEDDiagRightBankedFlatToRightBankedDown25,
        kTEDDiagLeftBankedDown25ToLeftBankedFlat,
        kTEDDiagRightBankedDown25ToRightBankedFlat,
        kTEDDiagFlatToLeftBankedUp25,
        kTEDDiagFlatToRightBankedUp25,
        kTEDDiagLeftBankedUp25ToFlat,
        kTEDDiagRightBankedUp25ToFlat,
        kTEDDiagFlatToLeftBankedDown25,
        kTEDDiagFlatToRightBankedDown25,
        kTEDDiagLeftBankedDown25ToFlat,
        kTEDDiagRightBankedDown25ToFlat,
        kTEDDiagUp25LeftBanked,
        kTEDDiagUp25RightBanked,
        kTEDDiagDown25LeftBanked,
        kTEDDiagDown25RightBanked,
        kTEDLeftEighthBankToDiagUp25,
        kTEDRightEighthBankToDiagUp25,
        kTEDLeftEighthBankToDiagDown25,
        kTEDRightEighthBankToDiagDown25,
        kTEDLeftEighthBankToOrthogonalUp25,
        kTEDRightEighthBankToOrthogonalUp25,
        kTEDLeftEighthBankToOrthogonalDown25,
        kTEDRightEighthBankToOrthogonalDown25,
        kTEDDiagBrakes,
        kTEDDiagBlockBrakes,
        kTEDDown25Brakes,
        kTEDDiagBooster,
        kTEDDiagFlatToUp60LongBase,
        kTEDDiagUp60ToFlatLongBase,
        kTEDDiagFlatToDown60LongBase,
        kTEDDiagDown60ToFlatLongBase,
        kTEDLeftEighthDiveLoopUpToOrthogonal,
        kTEDRightEighthDiveLoopUpToOrthogonal,
        kTEDLeftEighthDiveLoopDownToDiag,
        kTEDRightEighthDiveLoopDownToDiag,
        kTEDDiagDown25Brakes,
    });
    static_assert(kTrackElementDescriptors.size() == EnumValue(TrackElemType::count));

#pragma endregion

    const TrackElementDescriptor& GetTrackElementDescriptor(TrackElemType type)
    {
        if (EnumValue(type) >= kTrackElementDescriptors.size())
            return kTrackElementDescriptors[0];
        return kTrackElementDescriptors[EnumValue(type)];
    }

} // namespace OpenRCT2::TrackMetadata
