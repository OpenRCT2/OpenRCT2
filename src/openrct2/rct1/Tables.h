/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <vector>
#include "../common.h"

namespace RCT1
{
    struct RCT1VehicleColourSchemeCopyDescriptor {
        int8_t colour1, colour2, colour3;
    };

    colour_t GetColour(colour_t colour);
    uint8_t GetPeepSpriteType(uint8_t rct1SpriteType);
    uint8_t GetTerrain(uint8_t terrain);
    uint8_t GetTerrainEdge(uint8_t terrainEdge);

    uint8_t GetRideType(uint8_t rideType);
    RCT1VehicleColourSchemeCopyDescriptor GetColourSchemeCopyDescriptor(uint8_t vehicleType);
    bool RideTypeUsesVehicles(uint8_t rideType);
    bool PathIsQueue(uint8_t pathType);
    uint8_t NormalisePathAddition(uint8_t pathAdditionType);
    uint8_t GetVehicleSubEntryIndex(uint8_t vehicleSubEntry);

    const char * GetRideTypeObject(uint8_t rideType);
    const char * GetVehicleObject(uint8_t vehicleType);
    const char * GetSmallSceneryObject(uint8_t smallSceneryType);
    const char * GetLargeSceneryObject(uint8_t largeSceneryType);
    const char * GetWallObject(uint8_t wallType);
    const char * GetPathObject(uint8_t pathType);
    const char * GetPathAddtionObject(uint8_t pathAdditionType);
    const char * GetSceneryGroupObject(uint8_t sceneryGroupType);
    const char * GetWaterObject(uint8_t waterType);

    const std::vector<const char *> GetSceneryObjects(uint8_t sceneryType);
} // namespace RCT1
