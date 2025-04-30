/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../entity/Peep.h"
#include "RCT1.h"

#include <vector>

namespace OpenRCT2::RCT1
{
    struct VehicleColourSchemeCopyDescriptor
    {
        int8_t colour1, colour2, colour3;
    };

    enum class RideType : uint8_t;
    enum class PeepAnimationGroup : uint8_t;

    colour_t GetColour(colour_t colour);
    RCT12PeepAnimationGroup GetPeepAnimationGroup(PeepAnimationGroup rct1AnimationGroup);

    uint8_t GetRideType(RideType rideType, VehicleType vehicleType);
    VehicleColourSchemeCopyDescriptor GetColourSchemeCopyDescriptor(VehicleType vehicleType);
    bool RideTypeUsesVehicles(RideType rideType);
    bool PathIsQueue(uint8_t pathType);
    uint8_t NormalisePathAddition(uint8_t pathAdditionType);
    uint8_t GetVehicleSubEntryIndex(VehicleType rct1VehicleType, uint8_t vehicleSubEntry);

    std::string_view GetRideTypeObject(RideType rideType, bool isLL);
    std::string_view GetVehicleObject(VehicleType vehicleType);
    std::string_view GetSmallSceneryObject(uint8_t smallSceneryType);
    std::string_view GetLargeSceneryObject(uint8_t largeSceneryType);
    /**
     * Most sloped gates look like their ungated counterpart. This function maps these gates.
     */
    int32_t MapSlopedWall(uint8_t wallType);
    std::string_view GetWallObject(uint8_t wallType);
    std::string_view GetBannerObject(BannerType bannerType);
    std::string_view GetPathSurfaceObject(uint8_t pathType);
    std::string_view GetPathAddtionObject(uint8_t pathAdditionType);
    std::string_view GetFootpathRailingsObject(uint8_t footpathRailingsType);
    std::string_view GetSceneryGroupObject(uint8_t sceneryGroupType);
    std::string_view GetWaterObject(uint8_t waterType);
    std::string_view GetTerrainSurfaceObject(uint8_t terrain);
    std::string_view GetTerrainEdgeObject(uint8_t terrainEdge);

    const std::vector<const char*> GetSceneryObjects(uint8_t sceneryType);

    bool VehicleTypeIsReversed(const VehicleType vehicleType);
} // namespace OpenRCT2::RCT1
