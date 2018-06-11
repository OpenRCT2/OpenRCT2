/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#pragma once

#include <vector>
#include "../common.h"

namespace RCT1
{
    struct RCT1VehicleColourSchemeCopyDescriptor {
        sint8 colour1, colour2, colour3;
    };

    colour_t GetColour(colour_t colour);
    uint8 GetPeepSpriteType(uint8 rct1SpriteType);
    uint8 GetTerrain(uint8 terrain);
    uint8 GetTerrainEdge(uint8 terrainEdge);

    uint8 GetRideType(uint8 rideType);
    RCT1VehicleColourSchemeCopyDescriptor GetColourSchemeCopyDescriptor(uint8 vehicleType);
    bool RideTypeUsesVehicles(uint8 rideType);
    bool PathIsQueue(uint8 pathType);
    uint8 NormalisePathAddition(uint8 pathAdditionType);
    uint8 GetVehicleSubEntryIndex(uint8 vehicleSubEntry);

    const char * GetRideTypeObject(uint8 rideType);
    const char * GetVehicleObject(uint8 vehicleType);
    const char * GetSmallSceneryObject(uint8 smallSceneryType);
    const char * GetLargeSceneryObject(uint8 largeSceneryType);
    const char * GetWallObject(uint8 wallType);
    const char * GetPathObject(uint8 pathType);
    const char * GetPathAddtionObject(uint8 pathAdditionType);
    const char * GetSceneryGroupObject(uint8 sceneryGroupType);
    const char * GetWaterObject(uint8 waterType);

    const std::vector<const char *> GetSceneryObjects(uint8 sceneryType);
} // namespace RCT1
