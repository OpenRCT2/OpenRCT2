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

    static constexpr SequenceDescriptor kMinigolfHoleASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleASeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleBSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleBSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleCSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleCSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleDSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleDSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleDSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleESeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleESeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleESeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    constexpr auto kTEDMinigolfHoleA = TrackElementDescriptor{
        .description = STR_GOLF_HOLE_A,
        .coordinates = { 0, 0, 0, 0, -32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 159744,
        .mirrorElement = TrackElemType::minigolfHoleA,
        .flags = { TrackElementFlag::isGolfHole },
        .definition = { TrackGroup::miniGolfHole, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kMinigolfHoleASeq0, kMinigolfHoleASeq1 } },
    };

    constexpr auto kTEDMinigolfHoleB = TrackElementDescriptor{
        .description = STR_GOLF_HOLE_B,
        .coordinates = { 0, 0, 0, 0, -32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 167936,
        .mirrorElement = TrackElemType::minigolfHoleB,
        .flags = { TrackElementFlag::isGolfHole },
        .definition = { TrackGroup::miniGolfHole, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kMinigolfHoleBSeq0, kMinigolfHoleBSeq1 } },
    };

    constexpr auto kTEDMinigolfHoleC = TrackElementDescriptor{
        .description = STR_GOLF_HOLE_C,
        .coordinates = { 0, 0, 0, 0, -32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 172032,
        .mirrorElement = TrackElemType::minigolfHoleC,
        .flags = { TrackElementFlag::isGolfHole },
        .definition = { TrackGroup::miniGolfHole, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kMinigolfHoleCSeq0, kMinigolfHoleCSeq1 } },
    };

    constexpr auto kTEDMinigolfHoleD = TrackElementDescriptor{
        .description = STR_GOLF_HOLE_D,
        .coordinates = { 0, 1, 0, 0, -32, 32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 184320,
        .mirrorElement = TrackElemType::minigolfHoleE,
        .flags = { TrackElementFlag::isGolfHole },
        .definition = { TrackGroup::miniGolfHole, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 3, { kMinigolfHoleDSeq0, kMinigolfHoleDSeq1, kMinigolfHoleDSeq2 } },
    };

    constexpr auto kTEDMinigolfHoleE = TrackElementDescriptor{
        .description = STR_GOLF_HOLE_E,
        .coordinates = { 0, 3, 0, 0, -32, -32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 184320,
        .mirrorElement = TrackElemType::minigolfHoleD,
        .flags = { TrackElementFlag::isGolfHole },
        .definition = { TrackGroup::miniGolfHole, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 3, { kMinigolfHoleESeq0, kMinigolfHoleESeq1, kMinigolfHoleESeq2 } },
    };
} // namespace OpenRCT2::TrackMetadata
