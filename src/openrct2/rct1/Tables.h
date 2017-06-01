#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"

#ifdef __cplusplus

#include <vector>

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

    const std::vector<const char *> GetPreferedRideEntryOrder(uint8 rideType);
}

extern "C" {
#endif

    uint8 rct1_get_ride_type(uint8 rideType);
    const char * rct1_get_ride_type_object(uint8 rideType);
    const char * rct1_get_vehicle_object(uint8 vehicleType);

#ifdef __cplusplus
}
#endif
