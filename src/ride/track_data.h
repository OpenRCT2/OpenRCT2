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

#include "../common.h"

/* size 0x0A */
typedef struct{
	sint8 rotation_negative;	// 0x00
	sint8 rotation_positive;	// 0x01
	sint16 z_negative;			// 0x02
	sint16 z_positive;			// 0x04
	sint16 x;					// 0x06
	sint16 y;					// 0x08
}rct_track_coordinates;

// 0x009968BB, 0x009968BC, 0x009968BD, 0x009968BF, 0x009968C1, 0x009968C3
extern const rct_track_coordinates TrackCoordinates[256];

extern const uint64 RideTypePossibleTrackConfigurations[91];
extern const rct_preview_track *TrackBlocks[256];
extern const rct_preview_track *FlatRideTrackBlocks[255];
