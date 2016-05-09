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

#include "../../addresses.h"
#include "../../config.h"
#include "../../interface/viewport.h"
#include "../../world/sprite.h"
#include "../../paint/paint.h"
#include "../vehicle_paint.h"

/**
 *
 *  rct2: 0x006D4453
 */
void vehicle_visual_reverser(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	rct_vehicle *v1 = &g_sprite_list[vehicle->prev_vehicle_on_ride].vehicle;
	rct_vehicle *v2 = &g_sprite_list[vehicle->next_vehicle_on_ride].vehicle;
	x = (v1->x + v2->x) / 2;
	y = (v1->y + v2->y) / 2;
	z = (v1->z + v2->z) / 2;
	RCT2_GLOBAL(0x9DE568, sint16) = x;
	RCT2_GLOBAL(0x9DE56C, sint16) = y;
	uint32 rct2VehiclePtrFormat = ((uint32)vehicleEntry) - offsetof(rct_ride_entry, vehicles);
	RCT2_CALLPROC_X(0x006D45F8, x, imageDirection, y, z, (int)vehicle, rct2VehiclePtrFormat, 0);
}
