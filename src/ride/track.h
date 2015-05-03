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
	sint16 z;
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
			union{
				uint16 maze_entry;
				struct{
					uint8 unk_2;
					uint8 type;
				};
			};
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

/* Track Entrance entry size: 0x6 */
typedef struct{
	sint8 z;
	uint8 direction;			// 0x01
	sint16 x;					// 0x02
	sint16 y;					// 0x04
}rct_track_entrance;

enum{
	TRACK_ELEMENT_FLAG_CHAIN_LIFT = (1<<7),
	TRACK_ELEMENT_FLAG_INVERTED = (1<<6),
	TRACK_ELEMENT_FLAG_TERMINAL_STATION = (1<<3),
};

#define TRACK_ELEMENT_FLAG_MAGNITUDE_MASK 0x0F
#define TRACK_ELEMENT_FLAG_COLOUR_MASK 0x30
#define TRACK_ELEMENT_FLAG_STATION_NO_MASK 0x02

#define TRACK_PREVIEW_IMAGE_SIZE (370 * 217)

/**
 * Track design structure.
 * size: 0x4E72B
 */
typedef struct {
	uint8 type;										// 0x00
	uint8 vehicle_type;
	union{
		// After loading the track this is converted to
		// a cost but before its a flags register
		money32 cost;								// 0x02
		uint32 flags;								// 0x02
	};
	union{
		// After loading the track this is converted to 
		// a flags register
		uint8 ride_mode;							// 0x06
		uint8 track_flags;							// 0x06
	};
	uint8 version_and_colour_scheme;				// 0x07 0b0000_VVCC
	rct_vehicle_colour vehicle_colours[32];	// 0x08
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
	uint8 depart_flags;								// 0x4B
	uint8 number_of_trains;							// 0x4C
	uint8 number_of_cars_per_train;					// 0x4D
	uint8 min_waiting_time;							// 0x4E
	uint8 max_waiting_time;							// 0x4F
	uint8 var_50;
	sint8 max_speed;								// 0x51
	sint8 average_speed;							// 0x52
	uint16 ride_length;								// 0x53
	uint8 max_positive_vertical_g;					// 0x55
	sint8 max_negative_vertical_g;					// 0x56
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
	money16 upkeep_cost;							// 0x5E
	uint8 track_spine_colour[4];					// 0x60
	uint8 track_rail_colour[4];						// 0x64
	uint8 track_support_colour[4];					// 0x68
	uint32 var_6C;
	rct_object_entry vehicle_object;				// 0x70
	uint8 space_required_x;							// 0x80
	uint8 space_required_y;							// 0x81
	uint8 vehicle_additional_colour[32];			// 0x82
	uint8 lift_hill_speed_num_circuits;				// 0xA2 0bCCCL_LLLL
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

enum {
	TRACK_ELEM_FLAT,
	TRACK_ELEM_END_STATION,
	TRACK_ELEM_BEGIN_STATION,
	TRACK_ELEM_MIDDLE_STATION,
	TRACK_ELEM_25_DEG_UP,
	TRACK_ELEM_60_DEG_UP,
	TRACK_ELEM_FLAT_TO_25_DEG_UP,
	TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP,
	TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP,
	TRACK_ELEM_25_DEG_UP_TO_FLAT,
	TRACK_ELEM_25_DEG_DOWN,
	TRACK_ELEM_60_DEG_DOWN,
	TRACK_ELEM_FLAT_TO_25_DEG_DOWN,
	TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN,
	TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN,
	TRACK_ELEM_25_DEG_DOWN_TO_FLAT,
	TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES,
	TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES,
	TRACK_ELEM_FLAT_TO_LEFT_BANK,
	TRACK_ELEM_FLAT_TO_RIGHT_BANK,
	TRACK_ELEM_LEFT_BANK_TO_FLAT,
	TRACK_ELEM_RIGHT_BANK_TO_FLAT,
	TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES,
	TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES,
	TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP,
	TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP,
	TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK,
	TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK,
	TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN,
	TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN,
	TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK,
	TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK,
	TRACK_ELEM_LEFT_BANK,
	TRACK_ELEM_RIGHT_BANK,
	TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP,
	TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP,
	TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN,
	TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN,
	TRACK_ELEM_S_BEND_LEFT,
	TRACK_ELEM_S_BEND_RIGHT,
	TRACK_ELEM_LEFT_VERTICAL_LOOP,
	TRACK_ELEM_RIGHT_VERTICAL_LOOP,
	TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES,
	TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES,
	TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK,
	TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK,
	TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP,
	TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP,
	TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN,
	TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN,
	TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE,
	TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE,
	TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP,
	TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP,
	TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN,
	TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN,
	TRACK_ELEM_HALF_LOOP_UP,
	TRACK_ELEM_HALF_LOOP_DOWN,
	TRACK_ELEM_LEFT_CORKSCREW_UP,
	TRACK_ELEM_RIGHT_CORKSCREW_UP,
	TRACK_ELEM_LEFT_CORKSCREW_DOWN,
	TRACK_ELEM_RIGHT_CORKSCREW_DOWN,
	TRACK_ELEM_FLAT_TO_60_DEG_UP,
	TRACK_ELEM_60_DEG_UP_TO_FLAT,
	TRACK_ELEM_FLAT_TO_60_DEG_DOWN,
	TRACK_ELEM_60_DEG_DOWN_TO_FLAT,
	TRACK_ELEM_TOWER_BASE,
	TRACK_ELEM_TOWER_SECTION,
	TRACK_ELEM_FLAT_COVERED,
	TRACK_ELEM_25_DEG_UP_COVERED,
	TRACK_ELEM_60_DEG_UP_COVERED,
	TRACK_ELEM_FLAT_TO_25_DEG_UP_COVERED,
	TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED,
	TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED,
	TRACK_ELEM_25_DEG_UP_TO_FLAT_COVERED,
	TRACK_ELEM_25_DEG_DOWN_COVERED,
	TRACK_ELEM_60_DEG_DOWN_COVERED,
	TRACK_ELEM_FLAT_TO_25_DEG_DOWN_COVERED,
	TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED,
	TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED,
	TRACK_ELEM_25_DEG_DOWN_TO_FLAT_COVERED,
	TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED,
	TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED,
	TRACK_ELEM_S_BEND_LEFT_COVERED,
	TRACK_ELEM_S_BEND_RIGHT_COVERED,
	TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED,
	TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED,
	TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL,
	TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL,
	TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL,
	TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL,
	TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE,
	TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE,
	TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE,
	TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE,
	TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP,
	TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP,
	TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN,
	TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN,
	TRACK_ELEM_BRAKES,
	TRACK_ELEM_ROTATION_CONTROL_TOGGLE,
	TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP,
	TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP,
	TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP,
	TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN,
	TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN,
	TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP,
	TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP,
	TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN,
	TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN,
	TRACK_ELEM_25_DEG_UP_LEFT_BANKED,
	TRACK_ELEM_25_DEG_UP_RIGHT_BANKED,
	TRACK_ELEM_WATERFALL,
	TRACK_ELEM_RAPIDS,
	TRACK_ELEM_ON_RIDE_PHOTO,
	TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED,
	TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED,
	TRACK_ELEM_WATER_SPLASH,
	TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE,
	TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE,
	TRACK_ELEM_WHIRLPOOL,
	TRACK_ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE,
	TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122,
	TRACK_ELEM_CABLE_LIFT_HILL,
	TRACK_ELEM_REVERSE_WHOA_BELLY_SLOPE,
	TRACK_ELEM_REVERSE_WHOA_BELLY_VERTICAL,
	TRACK_ELEM_90_DEG_UP,
	TRACK_ELEM_90_DEG_DOWN,
	TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP,
	TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN,
	TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP,
	TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN,
	TRACK_ELEM_BRAKE_FOR_DROP,
	TRACK_ELEM_LEFT_EIGHTH_TO_DIAG,
	TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG,
	TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL,
	TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL,
	TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG,
	TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG,
	TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL,
	TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL,
	TRACK_ELEM_DIAG_FLAT,
	TRACK_ELEM_DIAG_25_DEG_UP,
	TRACK_ELEM_DIAG_60_DEG_UP,
	TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP,
	TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP,
	TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP,
	TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT,
	TRACK_ELEM_DIAG_25_DEG_DOWN,
	TRACK_ELEM_DIAG_60_DEG_DOWN,
	TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN,
	TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN,
	TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN,
	TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT,
	TRACK_ELEM_DIAG_FLAT_TO_60_DEG_UP,
	TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT,
	TRACK_ELEM_DIAG_FLAT_TO_60_DEG_DOWN,
	TRACK_ELEM_DIAG_60_DEG_DOWN_TO_FLAT,
	TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK,
	TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK,
	TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT,
	TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT,
	TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP,
	TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP,
	TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK,
	TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK,
	TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN,
	TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN,
	TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK,
	TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK,
	TRACK_ELEM_DIAG_LEFT_BANK,
	TRACK_ELEM_DIAG_RIGHT_BANK,
	TRACK_ELEM_LOG_FLUME_REVERSER,
	TRACK_ELEM_SPINNING_TUNNEL,
	TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN,
	TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN,
	TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP,
	TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP,
	TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP,
	TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP,
	TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK,
	TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK,
	TRACK_ELEM_POWERED_LIFT,
	TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP,
	TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP,
	TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN,
	TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN
};

extern const rct_trackdefinition *gTrackDefinitions;

void track_load_list(ride_list_item item);
int sub_67726A(const char *path);
rct_track_design *track_get_info(int index, uint8** preview);
rct_track_design *temp_track_get_info(char* path, uint8** preview);
rct_track_td6* load_track_design(const char *path);
int track_rename(const char *text);
int track_delete();
void reset_track_list_cache();
int track_is_connected_by_shape(rct_map_element *a, rct_map_element *b);
int sub_6D01B3(uint8 bl, uint8 rideIndex, int x, int y, int z);
int save_track_design(uint8 rideIndex);
int install_track(char* source_path, char* dest_name);
void window_track_list_format_name(char *dst, const char *src, char colour, char quotes);
void game_command_place_track(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);

#endif
