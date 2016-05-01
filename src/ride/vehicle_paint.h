#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#ifndef _VEHICLE_PAINT_H
#define _VEHICLE_PAINT_H

#include "vehicle.h"

void vehicle_paint(rct_vehicle *vehicle, int imageDirection);

void vehicle_visual_roto_drop(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_observation_tower(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_launched_freefall(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_virginia_reel(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);

#endif