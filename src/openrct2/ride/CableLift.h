/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _CABLE_LIFT_H_
#define _CABLE_LIFT_H_

#include "../common.h"
#include "Vehicle.h"

rct_vehicle *cable_lift_segment_create(int32_t rideIndex, int32_t x, int32_t y, int32_t z, int32_t direction, uint16_t var_44, int32_t remaining_distance, bool head);
void cable_lift_update(rct_vehicle *vehicle);
int32_t cable_lift_update_track_motion(rct_vehicle *cableLift);

#endif
