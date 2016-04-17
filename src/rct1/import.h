#pragma once

#include "../common.h"
#include "../core/List.hpp"

namespace RCT1
{
    colour_t GetColour(colour_t colour);
    uint8 GetTerrain(uint8 terrain);
    uint8 GetTerrainEdge(uint8 terrainEdge);

    uint8 GetRideType(uint8 rideType);
    bool RideTypeHasVehicle(uint8 rideType);

    const char * GetRideTypeObject(uint8 rideType);
    const char * GetVehicleObject(uint8 vehicleType);
    const char * GetSmallSceneryObject(uint8 smallSceneryType);
    const char * GetLargeSceneryObject(uint8 largeSceneryType);
    const char * GetWallObject(uint8 wallType);
    const char * GetSceneryGroupObject(uint8 sceneryGroupType);

    const List<const char *> GetSceneryObjects(uint8 sceneryType);
}
