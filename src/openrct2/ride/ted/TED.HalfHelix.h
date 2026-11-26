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
            { PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsAll,                                                       // wide
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
            { PS::left, PS::centre, PS::topLeft, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                                // inverted
            { PS::left, PS::centre, PS::topLeft, PS::bottomLeft }, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq3 = {
        .clearance = { -32, -32, 0, 4, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 3,
        .blockedSegments = { {
            { PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                                   // inverted
            kSegmentsAll,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq4 = {
        .clearance = { -32, -64, 8, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 3,
        .blockedSegments = { {
            { PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                                 // inverted
            kSegmentsAll,                                                           // wide
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
            { PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                      // inverted
            { PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight }, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq7 = {
        .clearance = { 0, -96, 8, 4, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            { PS::right, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                                   // inverted
            kSegmentsAll,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            { PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                // narrow
            kSegmentsUnimplemented,                       // inverted
            { PS::right, PS::topRight, PS::bottomRight }, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            { PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft }, // narrow
            kSegmentsUnimplemented,                                                       // inverted
            { PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft }, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            { PS::right, PS::centre, PS::topRight, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                   // inverted
            { PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight }, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq4 = {
        .clearance = { -32, -64, 0, 4, { 0b1000, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                // narrow
            kSegmentsUnimplemented,                       // inverted
            { PS::right, PS::topRight, PS::bottomRight }, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq5 = {
        .clearance = { -64, -32, 0, 4, { 0b0111, 0 }, {} },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            { PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                                             // inverted
            { PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight }, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq6 = {
        .clearance = { -64, -64, 0, 4, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 3,
        .blockedSegments = { {
            { PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                                   // inverted
            kSegmentsAll,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq7 = {
        .clearance = { -64, -96, 8, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 3,
        .blockedSegments = { {
            { PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                                 // inverted
            kSegmentsAll,                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq8 = {
        .clearance = { -32, -96, 8, 0, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            kSegmentsNone,                          // narrow
            kSegmentsUnimplemented,                 // inverted
            { PS::top, PS::topLeft, PS::topRight }, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq9 = {
        .clearance = { -64, -128, 8, 0, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            { PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                                             // inverted
            { PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight }, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq10 = {
        .clearance = { -32, -128, 8, 0, { 0b1110, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            { PS::top, PS::centre, PS::topLeft, PS::topRight }, // narrow
            kSegmentsUnimplemented,                             // inverted
            { PS::top, PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight }, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq11 = {
        .clearance = { 0, -128, 8, 4, { 0b0100, 0 }, { ClearanceFlag::flag0 } },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            kSegmentsNone,                          // narrow
            kSegmentsUnimplemented,                 // inverted
            { PS::top, PS::topLeft, PS::topRight }, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq12 = {
        .clearance = { -32, -160, 8, 4, { 0b1011, 0 }, {} },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            { PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                                               // inverted
            { PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight }, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq13 = {
        .clearance = { 0, -160, 8, 4, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            { PS::right, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight }, // narrow
            kSegmentsUnimplemented,                                                   // inverted
            kSegmentsAll,                                                             // wide
        } },
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
        .sequenceData = kTEDLeftHalfBankedHelixUpSmall.sequenceData.flipXAxis(),
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
        .sequenceData =
            [] { // TED sequence override! (vanilla bug) (has no effect in RCT2 as it gets rounded down to a multiple of 8)
                auto sequenceData = kTEDRightHalfBankedHelixUpSmall.sequenceData.offset({ 0, -96, 0 })
                                        .reverse({ 7, 5, 6, 4, 3, 1, 2, 0 });
                sequenceData.sequences[1].clearance.clearanceZ = 4;
                sequenceData.sequences[2].clearance.clearanceZ = 0;
                sequenceData.sequences[5].clearance.clearanceZ = 4;
                sequenceData.sequences[6].clearance.clearanceZ = 0;
                return sequenceData;
            }(),
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
        .sequenceData = kTEDLeftHalfBankedHelixDownSmall.sequenceData.flipXAxis(),
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
        .sequenceData = kTEDLeftHalfBankedHelixUpLarge.sequenceData.flipXAxis(),
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
        .sequenceData = kTEDRightHalfBankedHelixUpLarge.sequenceData.offset({ 0, -160, 0 })
                            .reverse({ 13, 11, 12, 10, 8, 9, 7, 6, 4, 5, 3, 1, 2, 0 }),
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
        .sequenceData = kTEDLeftHalfBankedHelixDownLarge.sequenceData.flipXAxis(),
    };
} // namespace OpenRCT2::TrackMetadata
