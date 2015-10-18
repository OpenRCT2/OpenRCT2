/*****************************************************************************
* Copyright (c) 2014 Ted John
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* This file is part of OpenRCT2.
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/


#include "../addresses.h"
#include "../config.h"
#include "../drawing/drawing.h"
#include "../localisation/localisation.h"
#include "ride_data.h"
#include "track_data.h"
#include "../sprites.h"
#include "../world/map.h"
#include "../world/sprite.h"
#include "../interface/viewport.h"
#include "../interface/window.h"
#include "track_paint.h"

void dummy_paint_setup_function(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement) {}

TRACK_PAINT_FUNCTION* dummy_rotation[] = {
	dummy_paint_setup_function,
	dummy_paint_setup_function,
	dummy_paint_setup_function,
	dummy_paint_setup_function,
};

void top_spin_paint_setup_rot_0(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement);
void top_spin_paint_setup_rot_1(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement);
void top_spin_paint_setup_rot_2(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement);
void top_spin_paint_setup_rot_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement);

/* 0x0076679C */
TRACK_PAINT_FUNCTION* top_spin_base_functions[] = {
	top_spin_paint_setup_rot_0,
	top_spin_paint_setup_rot_1,
	top_spin_paint_setup_rot_2,
	top_spin_paint_setup_rot_3,
};

TRACK_PAINT_FUNCTION** top_spin_track_paint_functions[] = {
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	dummy_rotation,
	top_spin_base_functions  // 123
};



/* rct2: 0x007667BC 
 */
void top_spin_paint_setup_rot_0(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement) {
	return;
	RCT2_CALLPROC_X(0x007667BC, 0, 0, 0, 0, 0, 0, 0);
}

/* rct2: 0x007667EC
*/
void top_spin_paint_setup_rot_1(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement) {
	return;
	RCT2_CALLPROC_X(0x007667EC, 0, 0, 0, 0, 0, 0, 0);
}

/* rct2: 0x0076671C
*/
void top_spin_paint_setup_rot_2(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement) {
	return;
	RCT2_CALLPROC_X(0x0076671C, 0, 0, 0, 0, 0, 0, 0);
}

/* rct2: 0x0076674C
*/
void top_spin_paint_setup_rot_3(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement) {
	return;
	RCT2_CALLPROC_X(0x0076674C, 0, 0, 0, 0, 0, 0, 0);
}