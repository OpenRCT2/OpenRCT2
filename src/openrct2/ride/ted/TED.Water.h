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

    static int32_t EvaluatorWaterSplash(const int16_t progress)
    {
        if (progress < 32)
            return -150;
        if (progress < 64)
            return 150;
        if (progress < 96)
            return 0;
        if (progress < 128)
            return 150;
        return -150;
    }

    static constexpr SequenceDescriptor kWaterfallSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWaterSplashSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWaterSplashSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWaterSplashSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWaterSplashSeq3 = {
        .clearance = { -96, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWaterSplashSeq4 = {
        .clearance = { -128, 0, 0, 16, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWhirlpoolSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kRapidsSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatStraightBlockedSegments,
    };

    constexpr auto kTEDWaterfall = TrackElementDescriptor{
        .description = STR_WATERFALLS,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::waterfall, TrackElemType::waterfall },
        .priceModifier = 143360,
        .mirrorElement = TrackElemType::waterfall,
        .flags = {},
        .definition = { TrackGroup::waterfall, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kWaterfallSeq0 } },
    };

    constexpr auto kTEDRapids = TrackElementDescriptor{
        .description = STR_RAPIDS,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::rapids, TrackElemType::rapids },
        .priceModifier = 118784,
        .mirrorElement = TrackElemType::rapids,
        .flags = {},
        .definition = { TrackGroup::rapids, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::sp,
        .sequenceData = { 1, { kRapidsSeq0 } },
    };

    constexpr auto kTEDWaterSplash = TrackElementDescriptor{
        .description = STR_WATER_SPLASH,
        .coordinates = { 0, 0, 16, 16, -128, 0 },
        .pieceLength = 128,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 393216,
        .mirrorElement = TrackElemType::waterSplash,
        .flags = {},
        .definition = { TrackGroup::waterSplash, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorWaterSplash,
        .sequenceData = { 5, { kWaterSplashSeq0, kWaterSplashSeq1, kWaterSplashSeq2, kWaterSplashSeq3, kWaterSplashSeq4 } },
    };

    constexpr auto kTEDWhirlpool = TrackElementDescriptor{
        .description = STR_WHIRLPOOL,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 126976,
        .mirrorElement = TrackElemType::whirlpool,
        .flags = {},
        .definition = { TrackGroup::whirlpool, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r5,
        .sequenceData = { 1, { kWhirlpoolSeq0 } },
    };
} // namespace OpenRCT2::TrackMetadata
