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

#ifndef _TRACK_H_
#define _TRACK_H_

#include "../common.h"
#include "../object.h"
#include "ride.h"

typedef struct {
	uint8 type;
	uint8 vangle_end;
	uint8 vangle_start;
	uint8 bank_end;
	uint8 bank_start;
	uint8 special;
	uint8 pad[2];
} rct_trackdefinition;

/**
* Size: 0x0A
*/
typedef struct {
	uint8 var_00;
	sint16 x;		// 0x01
	sint16 y;		// 0x03
	uint16 z;
	uint8 pad_07;
	uint8 var_08;
	uint8 var_09;
} rct_preview_track;

/**
* Size: 0x04
*/
typedef struct {
	union {
		uint32 all;
		struct {
			sint8 x;
			sint8 y;
			uint8 unk_2;
			uint8 type;
		};
	};
} rct_maze_element;

/* Size: 0x02 */
typedef struct{
	uint8 type;
	uint8 flags;
}rct_track_element;

/* Track Scenery entry size: 0x16 */
typedef struct{
	rct_object_entry scenery_object; // 0x00
	uint8 x;                        // 0x10
	uint8 y;                        // 0x11
	uint8 z;                        // 0x12
	uint8 flags;                    // 0x13 direction quadrant tertiary colour
	uint8 primary_colour;           // 0x14
	uint8 secondary_colour;         // 0x15
}rct_track_scenery;

enum{
	TRACK_ELEMENT_FLAG_CHAIN_LIFT = (1<<7),
	TRACK_ELEMENT_FLAG_INVERTED = (1<<6),
	TRACK_ELEMENT_FLAG_TERMINAL_STATION = (1<<3),
};

#define TRACK_ELEMENT_FLAG_MAGNITUDE_MASK 0x0F
#define TRACK_ELEMENT_FLAG_COLOUR_MASK 0x30
#define TRACK_ELEMENT_FLAG_STATION_NO_MASK 0x02

#define TRACK_PREVIEW_IMAGE_SIZE (370 * 217)

/* size: 0x2 */
typedef struct{
	uint8 body_colour;
	uint8 trim_colour;
} rct_track_vehicle_colour;

/**
 * Track design structure.
 * size: 0x4E72B
 */
typedef struct {
	uint8 type;										// 0x00
	uint8 vehicle_type;
	money32 cost;									// 0x02
	uint8 var_06;
	uint8 var_07;
	rct_track_vehicle_colour vehicle_colours[32];	// 0x08
	union{
		uint8 pad_48;
		uint8 track_spine_colour_rct1;				// 0x48
	};
	union{
		uint8 entrance_style;						// 0x49
		uint8 track_rail_colour_rct1;				// 0x49
	};
	union{
		uint8 total_air_time;						// 0x4A
		uint8 track_support_colour_rct1;			// 0x4A
	};
	uint8 pad_4B;
	uint8 number_of_trains;							// 0x4C
	uint8 number_of_cars_per_train;					// 0x4D
	uint8 pad_4E[2];
	uint8 var_50;
	uint8 max_speed;								// 0x51
	uint8 average_speed;							// 0x52
	uint16 ride_length;								// 0x53
	uint8 max_positive_vertical_g;					// 0x55
	uint8 max_negitive_vertical_g;					// 0x56
	uint8 max_lateral_g;							// 0x57
	union {
		uint8 inversions;							// 0x58
		uint8 holes;								// 0x58
	};
	uint8 drops;									// 0x59
	uint8 highest_drop_height;						// 0x5A
	uint8 excitement;								// 0x5B
	uint8 intensity;								// 0x5C
	uint8 nausea;									// 0x5D
	uint8 pad_5E[2];
	uint8 track_spine_colour[4];					// 0x60
	uint8 track_rail_colour[4];						// 0x64
	uint8 track_support_colour[4];					// 0x68
	uint32 var_6C;
	rct_object_entry vehicle_object;				// 0x70
	uint8 space_required_x;							// 0x80
	uint8 space_required_y;							// 0x81
	uint8 vehicle_additional_colour[32];			// 0x82
	uint8 var_A2;
} rct_track_td6;

typedef struct{
	rct_track_td6 track_td6;
	uint8 preview[4][TRACK_PREVIEW_IMAGE_SIZE];		// 0xA3
} rct_track_design;


enum {
	TRACK_NONE = 0,

	TRACK_FLAT = 0,
	TRACK_STATION_END = 2,
	TRACK_VERTICAL_LOOP = 7,
	TRACK_S_BEND = 13,
	TRACK_TWIST = 17,
	TRACK_HALF_LOOP = 18,
	TRACK_CORKSCREW = 19,
	TRACK_TOWER_BASE = 20,
	TRACK_HELIX_SMALL= 21,
	TRACK_HELIX_LARGE= 22,
	TRACK_HELIX_LARGE_UNBANKED = 23, 
	TRACK_BRAKES = 24,
	TRACK_ON_RIDE_PHOTO = 26,
	TRACK_WATER_SPLASH = 27,
	TRACK_BARREL_ROLL = 29,
	TRACK_POWERED_LIFT = 30,
	TRACK_HALF_LOOP_2 = 31, // ?
	TRACK_LOG_FLUME_REVERSER = 33,
	TRACK_WHOA_BELLY = 36,
	TRACK_LIFT_HILL = 43,
	TRACK_SPINNING_TUNNEL = 46,
	TRACK_ROTATION_CONTROL_TOGGLE = 47,
	TRACK_RAPIDS = 52,
	TRACK_WATERFALL = 152,
	TRACK_WHIRLPOOL = 152,
	TRACK_BRAKE_FOR_DROP = 172
};

enum {
	TRACK_UP_25 = 2,
	TRACK_UP_60 = 4,
	TRACK_DOWN_25 = 6,
	TRACK_DOWN_60 = 8,
	TRACK_UP_90 = 10,
	TRACK_DOWN_90 = 18,

	TRACK_VANGLE_TOWER = 10,
	TRACK_VANGLE_WHOA_BELLY = 10
};

enum {
	TRACK_BANK_NONE = 0,
	TRACK_BANK_LEFT = 2,
	TRACK_BANK_RIGHT = 4,
	TRACK_BANK_UPSIDE_DOWN = 15,
};

enum {
	TRACK_HALF_LOOP_UP = 64,
	TRACK_HALF_LOOP_DOWN = 192,
	TRACK_UNKNOWN_VERTICAL_LOOP = 208,
	TRACK_CORKSCREW_DOWN = 224
};

extern const rct_trackdefinition *gTrackDefinitions;

void track_load_list(ride_list_item item);
int sub_67726A(const char *path);
rct_track_design *track_get_info(int index, uint8** preview);
rct_track_td6* load_track_design(const char *path);
int track_rename(const char *text);
int track_delete();
void reset_track_list_cache();
int track_is_connected_by_shape(rct_map_element *a, rct_map_element *b);
int sub_6D01B3(int bl, int x, int y, int z);
int sub_6D2804(int al, uint8 rideIndex);

#endif
