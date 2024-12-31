/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "../rct12/Limits.h"

namespace OpenRCT2::RCT2::Limits
{
    using namespace RCT12::Limits;
    constexpr uint8_t kMaxStaff = 200;
    constexpr uint8_t kMaxBanners = 250;
    constexpr uint8_t kMaxTrainsPerRide = 32;
    constexpr uint8_t kMaxVehicleColours = 32;
    constexpr uint8_t kDowntimeHistorySize = 8;
    constexpr uint16_t kMaxEntities = 10000;
    constexpr uint16_t kMaxEntitiesRCTCExtended = 15000; // Used in files marked with “classic flag” 0xF
    constexpr uint32_t kMaxTileElements = 0x30000;
    constexpr uint16_t kMaxAnimatedObjects = 2000;
    constexpr uint8_t kMaxResearchedRideTypeQuads = 8;  // With 32 bits per uint32_t, this means there is room for
                                                        // 256 types.
    constexpr uint8_t kMaxResearchedRideEntryQuads = 8; // With 32 bits per uint32_t, this means there is room for
                                                        // 256 entries.
    constexpr uint8_t kMaxResearchedSceneryItemQuads = 56;
    constexpr const uint16_t kMaxResearchedSceneryItems = (kMaxResearchedSceneryItemQuads * 32); // There are 32
                                                                                                 // bits per
                                                                                                 // quad.
    constexpr uint16_t kMaxResearchItems = 500;

    constexpr uint16_t kTD6MaxTrackElements = 8192;

    constexpr uint8_t kMaxSmallSceneryObjects = 252;
    constexpr uint8_t kMaxLargeSceneryObjects = 128;
    constexpr uint8_t kMaxWallSceneryObjects = 128;
    constexpr uint8_t kMaxBannerObjects = 32;
    constexpr uint8_t kMaxPathObjects = 16;
    constexpr uint8_t kMaxPathAdditionObjects = 15;
    constexpr uint8_t kMaxSceneryGroupObjects = 19;
    constexpr uint8_t kMaxParkEntranceObjects = 1;
    constexpr uint8_t kMaxWaterObjects = 1;
    constexpr uint8_t kMaxScenarioTextObjects = 1;
    constexpr uint16_t kMaxMapSize = 256;
} // namespace OpenRCT2::RCT2::Limits
