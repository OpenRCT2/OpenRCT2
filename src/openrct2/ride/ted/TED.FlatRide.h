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

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq2 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq3 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq2 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq3 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq4 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq5 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq6 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq7 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq8 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq9 = {
        .clearance = { 64, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq10 = {
        .clearance = { 64, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq11 = {
        .clearance = { 64, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq12 = {
        .clearance = { 96, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq13 = {
        .clearance = { 96, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq14 = {
        .clearance = { 96, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq15 = {
        .clearance = { 96, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq2 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq3 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq4 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq5 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq6 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq7 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq4 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x1ASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::trackOrigin, SequenceFlag::connectsToPath,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::trackOrigin, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x1BSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin, SequenceFlag::connectsToPath,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq1 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq4 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq5 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq6 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq7 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq8 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    constexpr auto kTEDFlatTrack1x4A = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 262144,
        .mirrorElement = TrackElemType::flatTrack1x4A,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kFlatTrack1x4ASeq0, kFlatTrack1x4ASeq1, kFlatTrack1x4ASeq2, kFlatTrack1x4ASeq3 } },
    };

    constexpr auto kTEDFlatTrack2x2 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_2_X_2,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 262144,
        .mirrorElement = TrackElemType::flatTrack2x2,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kFlatTrack2x2Seq0, kFlatTrack2x2Seq1, kFlatTrack2x2Seq2, kFlatTrack2x2Seq3 } },
    };

    constexpr auto kTEDFlatTrack4x4 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_4_X_4,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 1048576, // TrackElemType::flatTrack4x4
        .mirrorElement = TrackElemType::flatTrack4x4,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 16,
                          { kFlatTrack4x4Seq0, kFlatTrack4x4Seq1, kFlatTrack4x4Seq2, kFlatTrack4x4Seq3, kFlatTrack4x4Seq4,
                            kFlatTrack4x4Seq5, kFlatTrack4x4Seq6, kFlatTrack4x4Seq7, kFlatTrack4x4Seq8, kFlatTrack4x4Seq9,
                            kFlatTrack4x4Seq10, kFlatTrack4x4Seq11, kFlatTrack4x4Seq12, kFlatTrack4x4Seq13, kFlatTrack4x4Seq14,
                            kFlatTrack4x4Seq15 } },
    };

    constexpr auto kTEDFlatTrack2x4 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_2_X_4,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 524288,
        .mirrorElement = TrackElemType::flatTrack2x4,
        .flags = { TrackElementFlag::onlyAboveGround },
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 8,
                          { kFlatTrack2x4Seq0, kFlatTrack2x4Seq1, kFlatTrack2x4Seq2, kFlatTrack2x4Seq3, kFlatTrack2x4Seq4,
                            kFlatTrack2x4Seq5, kFlatTrack2x4Seq6, kFlatTrack2x4Seq7 } },
    };

    constexpr auto kTEDFlatTrack1x5 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_5_X_1,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 327680,
        .mirrorElement = TrackElemType::flatTrack1x5,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 5,
                          { kFlatTrack1x5Seq0, kFlatTrack1x5Seq1, kFlatTrack1x5Seq2, kFlatTrack1x5Seq3, kFlatTrack1x5Seq4 } },
    };

    constexpr auto kTEDFlatTrack1x1A = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::flatTrack1x1A,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatTrack1x1ASeq0 } },
    };

    constexpr auto kTEDFlatTrack1x4B = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 262144,
        .mirrorElement = TrackElemType::flatTrack1x4B,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kFlatTrack1x4BSeq0, kFlatTrack1x4BSeq1, kFlatTrack1x4BSeq2, kFlatTrack1x4BSeq3 } },
    };

    constexpr auto kTEDFlatTrack1x1B = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::flatTrack1x1B,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatTrack1x1BSeq0 } },
    };

    constexpr auto kTEDFlatTrack1x4C = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 262144,
        .mirrorElement = TrackElemType::flatTrack1x4C,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, -40 },
        .sequenceData = { 4, { kFlatTrack1x4CSeq0, kFlatTrack1x4CSeq1, kFlatTrack1x4CSeq2, kFlatTrack1x4CSeq3 } },
    };

    constexpr auto kTEDFlatTrack3x3 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 96, 32, 0 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 524288,
        .mirrorElement = TrackElemType::flatTrack3x3,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 9,
                          { kFlatTrack3x3Seq0, kFlatTrack3x3Seq1, kFlatTrack3x3Seq2, kFlatTrack3x3Seq3, kFlatTrack3x3Seq4,
                            kFlatTrack3x3Seq5, kFlatTrack3x3Seq6, kFlatTrack3x3Seq7, kFlatTrack3x3Seq8 } },
    };

} // namespace OpenRCT2::TrackMetadata
