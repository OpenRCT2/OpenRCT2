#ifndef _TRACK_PAINT_H
#define _TRACK_PAINT_H

#include "../common.h"

typedef void (*TRACK_PAINT_FUNCTION)(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement);

extern TRACK_PAINT_FUNCTION* top_spin_track_paint_functions[];
extern TRACK_PAINT_FUNCTION* shop_track_paint_functions[];

#endif