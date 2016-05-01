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

#include "../paint/paint.h"
#include "../drawing/drawing.h"
#include "../world/sprite.h"
#include "../ride/ride_data.h"
#include "../interface/viewport.h"
#include "vehicle_paint.h"

/**
 *
 *  rct2: 0x006D4244
 */
void vehicle_paint(rct_vehicle *vehicle, int imageDirection)
{
	rct_ride_entry *rideEntry;
	const rct_ride_entry_vehicle *vehicleEntry;

	int x = vehicle->x;
	int y = vehicle->y;
	int z = vehicle->z;

	if (vehicle->flags & SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE) {
		uint32 ebx = 22965 + vehicle->var_C5;
		sub_98197C(ebx, 0, 0, 1, 1, 0, z, 0, 0, z + 2, get_current_rotation());
		return;
	}

	if (vehicle->ride_subtype == 0xFF) {
		vehicleEntry = &CableLiftVehicle;
	} else {
		rideEntry = get_ride_entry(vehicle->ride_subtype);
		vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

		if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_11) {
			vehicleEntry++;
			z += 16;
		}
	}

	uint32 rct2VehiclePtrFormat = ((uint32)vehicleEntry) - offsetof(rct_ride_entry, vehicles);
	RCT2_GLOBAL(0x00F64DFC, uint32) = rct2VehiclePtrFormat;
	switch (vehicleEntry->car_visual) {
	case VEHICLE_VISUAL_DEFAULT:						RCT2_CALLPROC_X(0x006D45F8, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0); break;
	case VEHICLE_VISUAL_LAUNCHED_FREEFALL:				vehicle_visual_launched_freefall(x, imageDirection, y, z, vehicle, vehicleEntry); break;
	case VEHICLE_VISUAL_OBSERVATION_TOWER:				vehicle_visual_observation_tower(x, imageDirection, y, z, vehicle, vehicleEntry); break;
	case VEHICLE_VISUAL_RIVER_RAPIDS:					RCT2_CALLPROC_X(0x006D5889, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0); break;
	case VEHICLE_VISUAL_MINI_GOLF_PLAYER:				RCT2_CALLPROC_X(0x006D42F0, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0); break;
	case VEHICLE_VISUAL_MINI_GOLF_BALL:					RCT2_CALLPROC_X(0x006D43C6, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0); break;
	case VEHICLE_VISUAL_REVERSER:						RCT2_CALLPROC_X(0x006D4453, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0); break;
	case VEHICLE_VISUAL_SPLASH_BOATS_OR_WATER_COASTER:	RCT2_CALLPROC_X(0x006D4295, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0); break;
	case VEHICLE_VISUAL_ROTO_DROP:						vehicle_visual_roto_drop(x, imageDirection, y, z, vehicle, vehicleEntry); break;
	case 10:											RCT2_CALLPROC_X(0x006D5600, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0); break;
	case 11:											RCT2_CALLPROC_X(0x006D5696, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0); break;
	case 12:											RCT2_CALLPROC_X(0x006D57EE, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0); break;
	case 13:											RCT2_CALLPROC_X(0x006D5783, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0); break;
	case 14:											RCT2_CALLPROC_X(0x006D5701, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0); break;
	case VEHICLE_VISUAL_VIRGINIA_REEL:					vehicle_visual_virginia_reel(x, imageDirection, y, z, vehicle, vehicleEntry); break;
	case VEHICLE_VISUAL_SUBMARINE:						RCT2_CALLPROC_X(0x006D44D5, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0); break;
	}
}

