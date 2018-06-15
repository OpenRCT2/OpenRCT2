/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _VEHICLE_PAINT_H
#define _VEHICLE_PAINT_H

#include "../common.h"

struct paint_session;
struct rct_ride_entry_vehicle;
struct rct_vehicle;

struct vehicle_boundbox {
    sint8 offset_x;
    sint8 offset_y;
    sint8 offset_z;
    uint8 length_x;
    uint8 length_y;
    uint8 length_z;
};

extern const vehicle_boundbox VehicleBoundboxes[16][224];

void vehicle_paint(paint_session * session, const rct_vehicle *vehicle, sint32 imageDirection);

void vehicle_visual_default(paint_session * session, sint32 imageDirection, sint32 z, const rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_roto_drop(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, const rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_observation_tower(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, const rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_river_rapids(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, const rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_reverser(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, const rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_splash_boats_or_water_coaster(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, const rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_launched_freefall(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, const rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_splash_effect(paint_session * session, sint32 z, const rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_virginia_reel(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, const rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_submarine(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, const rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry);
void vehicle_visual_mini_golf_player(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, const rct_vehicle *vehicle);
void vehicle_visual_mini_golf_ball(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, const rct_vehicle *vehicle);

#endif
