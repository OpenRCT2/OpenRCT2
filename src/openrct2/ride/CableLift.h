/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#ifndef _CABLE_LIFT_H_
#define _CABLE_LIFT_H_

#include "../common.h"
#include "Vehicle.h"

rct_vehicle *cable_lift_segment_create(sint32 rideIndex, sint32 x, sint32 y, sint32 z, sint32 direction, uint16 var_44, sint32 remaining_distance, bool head);
void cable_lift_update(rct_vehicle *vehicle);
sint32 cable_lift_update_track_motion(rct_vehicle *cableLift);

#endif
