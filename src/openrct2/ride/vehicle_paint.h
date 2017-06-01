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

#ifndef _VEHICLE_PAINT_H
#define _VEHICLE_PAINT_H

#include "vehicle.h"

typedef struct vehicle_boundbox {
	sint8 offset_x;
	sint8 offset_y;
	sint8 offset_z;
	uint8 length_x;
	uint8 length_y;
	uint8 length_z;
} vehicle_boundbox;

extern const vehicle_boundbox VehicleBoundboxes[16][224];

void vehicle_paint(rct_vehicle *vehicle, sint32 imageDirection);

void vehicle_visual_default(sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_roto_drop(sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_observation_tower(sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_river_rapids(sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_reverser(sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_splash_boats_or_water_coaster(sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_launched_freefall(sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_splash_effect(sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_virginia_reel(sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_submarine(sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_mini_golf_player(sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle);
void vehicle_visual_mini_golf_ball(sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle);

#endif
