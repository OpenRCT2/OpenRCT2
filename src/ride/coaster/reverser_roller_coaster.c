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

#include "../../config.h"
#include "../../interface/viewport.h"
#include "../../world/sprite.h"
#include "../../paint/paint.h"
#include "../vehicle_paint.h"

#ifndef NO_VEHICLES
/**
 *
 *  rct2: 0x006D4453
 */
void vehicle_visual_reverser(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	rct_vehicle *v1 = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
	rct_vehicle *v2 = GET_VEHICLE(vehicle->next_vehicle_on_ride);
	x = (v1->x + v2->x) / 2;
	y = (v1->y + v2->y) / 2;
	z = (v1->z + v2->z) / 2;
	gUnk9DE568 = x;
	gUnk9DE56C = y;
	vehicle_visual_default(x, imageDirection, y, z, vehicle, vehicleEntry);
}
#endif
