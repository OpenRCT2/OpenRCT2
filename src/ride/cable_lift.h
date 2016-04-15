#ifndef _CABLE_LIFT_H_
#define _CABLE_LIFT_H_

#include "../common.h"
#include "vehicle.h"

rct_vehicle *cable_lift_segment_create(int rideIndex, int x, int y, int z, int direction, uint16 var_44, sint32 remaining_distance, bool head);
void cable_lift_update(rct_vehicle *vehicle);
int cable_lift_update_track_motion(rct_vehicle *cableLift);

#endif
