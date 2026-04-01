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

    static constexpr SequenceDescriptor kEndStationSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::disallowDoors, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kBeginStationSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::disallowDoors, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kMiddleStationSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::disallowDoors, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    constexpr auto kTEDEndStation = TrackElementDescriptor{
        .description = STR_STATION_PLATFORM,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::endStation, TrackElemType::endStation },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::endStation,
        .flags = {},
        .definition = { TrackGroup::stationEnd, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kEndStationSeq0 } },
    };

    constexpr auto kTEDBeginStation = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::endStation, TrackElemType::endStation },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::beginStation,
        .flags = {},
        .definition = { TrackGroup::stationEnd, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kBeginStationSeq0 } },
    };

    constexpr auto kTEDMiddleStation = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::endStation, TrackElemType::endStation },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::middleStation,
        .flags = {},
        .definition = { TrackGroup::stationEnd, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kMiddleStationSeq0 } },
    };
} // namespace OpenRCT2::TrackMetadata
