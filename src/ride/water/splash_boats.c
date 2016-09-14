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
 *  rct2: 0x006D4295
 */
void vehicle_visual_splash_boats_or_water_coaster(int x, int imageDirection, int y, int z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
	if (vehicle->is_child) {
		vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
	} else {
		vehicle = GET_VEHICLE(vehicle->next_vehicle_on_ride);
	}
	g_currently_drawn_item = vehicle;
	imageDirection = ((get_current_rotation() * 8) + vehicle->sprite_direction) & 0x1F;
	gUnk9DE568 = vehicle->x;
	gUnk9DE56C = vehicle->y;
	vehicle_paint(vehicle, imageDirection);
}
