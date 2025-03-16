/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../ride/Track.h"
#include "../../world/tile_element/TrackElement.h"
#include "../Paint.h"
#include "../tile_element/Segment.h"

#include <cstdint>

namespace OpenRCT2::BlockedSegments
{
    enum class BlockedSegmentsType : uint8_t
    {
        narrow = 0,
        inverted = 1,
        wide = 2,
        suspendedSwingingTrain = 3,
        wideTrain = 4,

        count = 5,
    };

    static constexpr bool kBlockedSegmentsTypeIsInverted[] = {
        false, // narrow
        true,  // inverted
        false, // wide
        true,  // suspendedSwingingTrain
        false, // wideTrain
    };
    static_assert(std::size(kBlockedSegmentsTypeIsInverted) == EnumValue(BlockedSegmentsType::count));

    static constexpr const std::array<BlockedSegmentsType, EnumValue(TrackGroup::count)> kTrackGroupBlockedSegmentsNarrow =
        []() consteval {
            std::array<BlockedSegmentsType, EnumValue(TrackGroup::count)> array{};
            array.fill(BlockedSegmentsType::narrow);
            return array;
        }();

    static constexpr const std::array<BlockedSegmentsType, EnumValue(TrackGroup::count)> kTrackGroupBlockedSegmentsInverted =
        []() consteval {
            std::array<BlockedSegmentsType, EnumValue(TrackGroup::count)> array{};
            array.fill(BlockedSegmentsType::inverted);
            array[EnumValue(TrackGroup::stationEnd)] = BlockedSegments::BlockedSegmentsType::wide;
            array[EnumValue(TrackGroup::onridePhoto)] = BlockedSegments::BlockedSegmentsType::wide;
            return array;
        }();

    static constexpr const std::array<BlockedSegmentsType, EnumValue(TrackGroup::count)> kTrackGroupBlockedSegmentsWide =
        []() consteval {
            std::array<BlockedSegmentsType, EnumValue(TrackGroup::count)> array{};
            array.fill(BlockedSegmentsType::wide);
            return array;
        }();

    static constexpr const std::array<BlockedSegmentsType, EnumValue(TrackGroup::count)>
        kTrackGroupBlockedSegmentsSuspendedSwingingTrain = []() consteval {
            std::array<BlockedSegmentsType, EnumValue(TrackGroup::count)> array{};
            array.fill(BlockedSegmentsType::suspendedSwingingTrain);
            array[EnumValue(TrackGroup::stationEnd)] = BlockedSegments::BlockedSegmentsType::wide;
            array[EnumValue(TrackGroup::onridePhoto)] = BlockedSegments::BlockedSegmentsType::wide;
            return array;
        }();

    static constexpr const std::array<BlockedSegmentsType, EnumValue(TrackGroup::count)> kTrackGroupBlockedSegmentsWideTrain =
        []() consteval {
            std::array<BlockedSegmentsType, EnumValue(TrackGroup::count)> array{};
            array.fill(BlockedSegmentsType::wideTrain);
            return array;
        }();
} // namespace OpenRCT2::BlockedSegments
