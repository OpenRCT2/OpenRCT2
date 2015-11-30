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

#ifndef _VEHICLE_H_
#define _VEHICLE_H_

#include "../common.h"

/* size: 0x2 */
typedef struct{
	uint8 body_colour;
	uint8 trim_colour;
} rct_vehicle_colour;

/**
 * Ride type vehicle structure.
 * size: 0x65
 */
typedef struct {
	uint16 rotation_frame_mask;		// 0x00 , 0x1A
	uint8 var_02;					// 0x02 , 0x1C
	uint8 var_03;					// 0x03 , 0x1D
	uint32 var_04;					// 0x04 , 0x1E
	uint16 car_friction;			// 0x08 , 0x22
	sint8 tab_height;				// 0x0A , 0x24
	uint8 num_seats;				// 0x0B , 0x25
	uint16 sprite_flags;			// 0x0C , 0x26
	uint8 sprite_width;				// 0x0E , 0x28
	uint8 sprite_height_negative;	// 0x0F , 0x29
	uint8 sprite_height_positive;	// 0x10 , 0x2A
	uint8 var_11;					// 0x11 , 0x2B
	uint16 var_12;					// 0x12 , 0x2C
	uint16 var_14;					// 0x14 , 0x2E
	uint16 var_16;					// 0x16 , 0x30
	uint32 base_image_id;			// 0x18 , 0x32
	uint32 var_1C;					// 0x1C , 0x36
	uint32 var_20;					// 0x20 , 0x3A
	uint32 var_24;					// 0x24 , 0x3E
	uint32 var_28;					// 0x28 , 0x42
	uint32 var_2C;					// 0x2C , 0x46
	uint32 var_30;					// 0x30 , 0x4A
	uint32 var_34;					// 0x34 , 0x4E
	uint32 var_38;					// 0x38 , 0x52
	uint32 var_3C;					// 0x3C , 0x56
	uint32 var_40;					// 0x40 , 0x5A
	uint32 var_44;					// 0x44 , 0x5E
	uint32 var_48;					// 0x48 , 0x62
	uint32 var_4C;					// 0x4C , 0x66
	uint32 no_vehicle_images;		// 0x50 , 0x6A
	uint8 no_seating_rows;			// 0x54 , 0x6E
	uint8 spinning_inertia;			// 0x55 , 0x6F
	uint8 spinning_friction;		// 0x56 , 0x70
	uint8 pad_57[0x3];
	uint8 var_5A;					// 0x5A , 0x74
	uint8 powered_acceleration;		// 0x5B , 0x75
	uint8 powered_max_speed;		// 0x5C , 0x76
	uint8 car_visual;				// 0x5D , 0x77
	uint8 pad_5E;
	uint8 draw_order;
	uint8 special_frames;			// 0x60 , 0x7A
	sint8* peep_loading_positions;	// 0x61 , 0x7B
} rct_ride_type_vehicle;

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 is_child;					// 0x01
	uint16 next_in_quadrant;		// 0x02
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 linked_list_type_offset;	// 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
	// Height from center of sprite to bottom
	uint8 sprite_height_negative;	// 0x09
	uint16 sprite_index;			// 0x0A
	uint16 var_0C;
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
	// Width from center of sprite to edge
	uint8 sprite_width;				// 0x14
	// Height from center of sprite to top
	uint8 sprite_height_positive;	// 0x15
	sint16 sprite_left;				// 0x16
	sint16 sprite_top;				// 0x18
	sint16 sprite_right;			// 0x1A
	sint16 sprite_bottom;			// 0x1C
	uint8 sprite_direction;			// 0x1E
	uint8 var_1F;
	uint8 var_20;
	uint8 pad_21[3];
	uint32 var_24;
	sint32 velocity;				// 0x28
	uint32 var_2C;
	uint8 ride;						// 0x30
	uint8 vehicle_type;				// 0x31
	rct_vehicle_colour colours;		// 0x32
	uint16 var_34;
	union {
		sint16 track_direction;		// 0x36 (0000 0000 0000 0011)
		sint16 track_type;			// 0x36 (0000 0011 1111 1100)
	};
	uint16 track_x;					// 0x38
	uint16 track_y;					// 0x3A
	uint16 track_z;					// 0x3C
	uint16 next_vehicle_on_train;	// 0x3E

	// The previous vehicle on the same train or the last vehicle on the previous or only train.
	uint16 prev_vehicle_on_ride;	// 0x40

	// The next vehicle on the same train or the first vehicle on the next or only train
	uint16 next_vehicle_on_ride;	// 0x42

	uint16 var_44;
	uint16 friction;				// 0x46
	uint16 update_flags;			// 0x48
	uint8 var_4A;
	uint8 current_station;			// 0x4B
	uint16 var_4C;
	uint16 var_4E;
	uint8 status;					// 0x50
	uint8 var_51;
	uint16 peep[32];				// 0x52
	uint8 peep_tshirt_colours[32];	// 0x92
	uint8 num_seats;				// 0xB2
	uint8 num_peeps;				// 0xB3
	uint8 next_free_seat;			// 0xB4
	uint8 var_B5;
	uint16 var_B6;
	uint16 var_B8;
	uint8 var_BA;
	uint8 sound1_id;				// 0xBB
	uint8 sound1_volume;			// 0xBC
	uint8 sound2_id;				// 0xBD
	uint8 sound2_volume;			// 0xBE
	sint8 var_BF;
	uint8 pad_C0[2];
	uint8 speed;					// 0xC2
	uint8 acceleration;				// 0xC3
	uint8 var_C4;
	uint8 var_C5;
	uint8 pad_C6[2];
	uint32 var_C8;
	uint8 scream_sound_id;			// 0xCC
	uint8 var_CD;
	union {
		uint8 var_CE;
		uint8 num_laps;				// 0xCE
	};
	uint8 pad_CF[0x03];
	sint8 var_D2;
	uint8 var_D3;
	uint8 var_D4;
	uint8 var_D5;
	uint8 ride_subtype;				// 0xD6
	uint8 colours_extended;			// 0xD7
	uint8 var_D8;
	uint8 var_D9;
} rct_vehicle;

typedef struct {
	rct_vehicle *head;
	rct_vehicle *tail;
} train_ref;

enum {
	VEHICLE_STATUS_MOVING_TO_END_OF_STATION,
	VEHICLE_STATUS_WAITING_FOR_PASSENGERS,
	VEHICLE_STATUS_WAITING_TO_DEPART,
	VEHICLE_STATUS_DEPARTING,
	VEHICLE_STATUS_TRAVELLING,
	VEHICLE_STATUS_ARRIVING,
	VEHICLE_STATUS_UNLOADING_PASSENGERS,
	VEHICLE_STATUS_TRAVELLING_07,
	VEHICLE_STATUS_CRASHING,
	VEHICLE_STATUS_CRASHED,
	VEHICLE_STATUS_TRAVELLING_0A,
	VEHICLE_STATUS_SWINGING,
	VEHICLE_STATUS_ROTATING,
	VEHICLE_STATUS_ROTATING_0D,
	VEHICLE_STATUS_OPERATING,
	VEHICLE_STATUS_SHOWING_FILM,
	VEHICLE_STATUS_ROTATING_10,
	VEHICLE_STATUS_OPERATING_11,
	VEHICLE_STATUS_OPERATING_12,
	VEHICLE_STATUS_DOING_CIRCUS_SHOW,
	VEHICLE_STATUS_OPERATING_13,
	VEHICLE_STATUS_WAITING_FOR_CABLE_LIFT,
	VEHICLE_STATUS_TRAVELLING_15,
	VEHICLE_STATUS_STOPPING,
	VEHICLE_STATUS_WAITING_FOR_PASSENGERS_17,
	VEHICLE_STATUS_WAITING_TO_START,
	VEHICLE_STATUS_STARTING,
	VEHICLE_STATUS_OPERATING_1A,
	VEHICLE_STATUS_STOPPING_1B,
	VEHICLE_STATUS_UNLOADING_PASSENGERS_1C,
	VEHICLE_STATUS_STOPPED_BY_BLOCK_BRAKES
};

enum{
	VEHICLE_UPDATE_FLAG_0 = (1 << 0),
	VEHICLE_UPDATE_FLAG_1 = (1 << 1),
	VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT = (1 << 2),
	VEHICLE_UPDATE_FLAG_3 = (1 << 3),
	VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART = (1 << 4),
	VEHICLE_UPDATE_FLAG_TESTING = (1 << 5),
	VEHICLE_UPDATE_FLAG_6 = (1 << 6),
	VEHICLE_UPDATE_FLAG_7 = (1 << 7),
	VEHICLE_UPDATE_FLAG_BROKEN_CAR = (1 << 8),
	VEHICLE_UPDATE_FLAG_BROKEN_TRAIN = (1 << 9),
	VEHICLE_UPDATE_FLAG_10 = (1 << 10),
	VEHICLE_UPDATE_FLAG_11 = (1 << 11),
	VEHICLE_UPDATE_FLAG_12 = (1 << 12),
	VEHICLE_UPDATE_FLAG_13 = (1 << 13),
	VEHICLE_UPDATE_FLAG_14 = (1 << 14),
	VEHICLE_UPDATE_FLAG_15 = (1 << 15)
};

enum {
	VEHICLE_SPRITE_FLAG_FLAT = (1 << 0),
	VEHICLE_SPRITE_FLAG_GENTLE_SLOPES = (1 << 1),
	VEHICLE_SPRITE_FLAG_STEEP_SLOPES = (1 << 2),
	VEHICLE_SPRITE_FLAG_VERTICAL_SLOPES = (1 << 3),
	VEHICLE_SPRITE_FLAG_DIAGONAL_SLOPES = (1 << 4),
	VEHICLE_SPRITE_FLAG_FLAT_BANKED = (1 << 5),
	VEHICLE_SPRITE_FLAG_INLINE_TWISTS = (1 << 6),
	VEHICLE_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_BANKED_TRANSITIONS = (1 << 7),
	VEHICLE_SPRITE_FLAG_DIAGONAL_GENTLE_SLOPE_BANKED_TRANSITIONS = (1 << 8),
	VEHICLE_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TRANSITIONS = (1 << 9),
	VEHICLE_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TURNS = (1 << 10),
	VEHICLE_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_WHILE_BANKED_TRANSITIONS = (1 << 11),
	VEHICLE_SPRITE_FLAG_CORKSCREWS = (1 << 12),
	VEHICLE_SPRITE_FLAG_RESTRAINT_ANIMATION = (1 << 13),
	VEHICLE_SPRITE_FLAG_14 = (1 << 14),
	VEHICLE_SPRITE_FLAG_15 = (1 << 15),
};

enum {
	VEHICLE_VISUAL_DEFAULT,
	VEHICLE_VISUAL_FLAT_RIDE_OR_CAR_RIDE,
	VEHICLE_VISUAL_LAUNCHED_FREEFALL,
	VEHICLE_VISUAL_OBSERVATION_TOWER,
	VEHICLE_VISUAL_RIVER_RAPIDS,
	VEHICLE_VISUAL_MINI_GOLF_PLAYER,
	VEHICLE_VISUAL_MINI_GOLF_BALL,
	VEHICLE_VISUAL_REVERSER,
	VEHICLE_VISUAL_SPLASH_BOATS_OR_WATER_COASTER,
	VEHICLE_VISUAL_ROTO_DROP,
	VEHICLE_VISUAL_VIRGINIA_REEL = 15,
	VEHICLE_VISUAL_SUBMARINE
};

#define VEHICLE_SEAT_PAIR_FLAG	0x80
#define VEHICLE_SEAT_NUM_MASK	0x7F

void vehicle_update_all();
int sub_6BC2F3(rct_vehicle* vehicle);
void sub_6BB9FF(rct_vehicle* vehicle);
void vehicle_sounds_update();
void vehicle_get_g_forces(rct_vehicle *vehicle, int *verticalG, int *lateralG);
void vehicle_set_map_toolbar(rct_vehicle *vehicle);
int vehicle_is_used_in_pairs(rct_vehicle *vehicle);
rct_vehicle *vehicle_get_head(rct_vehicle *vehicle);
void sub_6DEF56(rct_vehicle *cableLift);
rct_vehicle *cable_lift_segment_create(int rideIndex, int x, int y, int z, int direction, uint16 var_44, uint32 var_24, bool head);
bool sub_6DD365(rct_vehicle *vehicle);
int sub_6DAB4C(rct_vehicle *vehicle, int *outStation);
rct_ride_type_vehicle *vehicle_get_vehicle_entry(rct_vehicle *vehicle);

/** Helper macro until rides are stored in this module. */
#define GET_VEHICLE(sprite_index) &(g_sprite_list[sprite_index].vehicle)

#endif