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

#ifndef _CABLE_LIFT_H_
#define _CABLE_LIFT_H_

#include "../common.h"
#include "vehicle.h"

rct_vehicle *cable_lift_segment_create(sint32 rideIndex, sint32 x, sint32 y, sint32 z, sint32 direction, uint16 var_44, sint32 remaining_distance, bool head);
void cable_lift_update(rct_vehicle *vehicle);
sint32 cable_lift_update_track_motion(rct_vehicle *cableLift);

#endif
