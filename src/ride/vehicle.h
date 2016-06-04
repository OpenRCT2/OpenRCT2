#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _VEHICLE_H_
#define _VEHICLE_H_

#include "../common.h"

#pragma pack(push, 1)
/* size: 0x2 */
typedef struct rct_vehicle_colour {
	uint8 body_colour;
	uint8 trim_colour;
} rct_vehicle_colour;
assert_struct_size(rct_vehicle_colour, 2);

/**
 * Ride type vehicle structure.
 * size: 0x65
 */
typedef struct rct_ride_entry_vehicle {
	uint16 rotation_frame_mask;		// 0x00 , 0x1A
	uint8 var_02;					// 0x02 , 0x1C
	uint8 var_03;					// 0x03 , 0x1D
	uint32 spacing;					// 0x04 , 0x1E
	uint16 car_friction;			// 0x08 , 0x22
	sint8 tab_height;				// 0x0A , 0x24
	uint8 num_seats;				// 0x0B , 0x25
	uint16 sprite_flags;			// 0x0C , 0x26
	uint8 sprite_width;				// 0x0E , 0x28
	uint8 sprite_height_negative;	// 0x0F , 0x29
	uint8 sprite_height_positive;	// 0x10 , 0x2A
	uint8 var_11;					// 0x11 , 0x2B
	uint16 flags_a;					// 0x12 , 0x2C
	uint16 flags_b;					// 0x14 , 0x2E
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
	uint8 friction_sound_id;		// 0x57 , 0x71
	uint8 var_58;					// 0x58 , 0x72
	uint8 sound_range;				// 0x59 , 0x73
	uint8 var_5A;					// 0x5A , 0x74
	uint8 powered_acceleration;		// 0x5B , 0x75
	uint8 powered_max_speed;		// 0x5C , 0x76
	uint8 car_visual;				// 0x5D , 0x77
	uint8 effect_visual;
	uint8 draw_order;
	uint8 special_frames;			// 0x60 , 0x7A
	sint8* peep_loading_positions;	// 0x61 , 0x7B
} rct_ride_entry_vehicle;
#ifdef PLATFORM_32BIT
assert_struct_size(rct_ride_entry_vehicle, 0x65);
#endif

typedef struct rct_vehicle {
	uint8 sprite_identifier;		// 0x00
	uint8 is_child;					// 0x01
	uint16 next_in_quadrant;		// 0x02
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 linked_list_type_offset;	// 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
	// Height from center of sprite to bottom
	uint8 sprite_height_negative;	// 0x09
	uint16 sprite_index;			// 0x0A
	uint16 flags;			// 0x0C
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
	uint8 vehicle_sprite_type;		// 0x1F
	uint8 bank_rotation;			// 0x20
	uint8 pad_21[3];
	sint32 remaining_distance;		// 0x24
	sint32 velocity;				// 0x28
	sint32 acceleration;			// 0x2C
	uint8 ride;						// 0x30
	uint8 vehicle_type;				// 0x31
	rct_vehicle_colour colours;		// 0x32
	union {
		uint16 track_progress;			// 0x34
		struct {
			sint8 var_34;
			uint8 var_35;
		};
	};
	union {
		sint16 track_direction;		// 0x36 (0000 0000 0000 0011)
		sint16 track_type;			// 0x36 (0000 0011 1111 1100)
		rct_xy8 boat_location;		// 0x36
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
	union {
		sint16 swinging_car_var_0;	// 0x4C
		sint16 current_time;		// 0x4C
		struct {
			sint8 ferris_wheel_var_0;	// 0x4C
			sint8 ferris_wheel_var_1;	// 0x4D
		};
	};
	sint16 var_4E;
	uint8 status;					// 0x50
	uint8 sub_state;				// 0x51
	uint16 peep[32];				// 0x52
	uint8 peep_tshirt_colours[32];	// 0x92
	uint8 num_seats;				// 0xB2
	uint8 num_peeps;				// 0xB3
	uint8 next_free_seat;			// 0xB4
	uint8 restraints_position;		// 0xB5 0 == Close, 255 == Open
	sint16 var_B6;
	uint16 var_B8;
	uint8 var_BA;
	uint8 sound1_id;				// 0xBB
	uint8 sound1_volume;			// 0xBC
	uint8 sound2_id;				// 0xBD
	uint8 sound2_volume;			// 0xBE
	sint8 var_BF;
	union {
		uint16 var_C0;
		uint16 time_waiting;		// 0xC0
		uint16 cable_lift_target;	// 0xC0
	};
	uint8 speed;					// 0xC2
	uint8 powered_acceleration;		// 0xC3
	uint8 var_C4;
	uint8 var_C5;
	uint8 pad_C6[0x2];
	uint16 var_C8;
	uint16 var_CA;
	uint8 scream_sound_id;			// 0xCC
	uint8 var_CD;
	union {
		uint8 var_CE;
		uint8 num_laps;				// 0xCE
	};
	union {
		uint8 var_CF;
		uint8 break_speed;			// 0xCF
	};
	uint16 lost_time_out;			// 0xD0
	sint8 vertical_drop_countdown;	// 0xD1
	uint8 var_D3;
	uint8 var_D4;
	uint8 mini_golf_flags;			// 0xD5
	uint8 ride_subtype;				// 0xD6
	uint8 colours_extended;			// 0xD7
	uint8 seat_rotation;			// 0xD8
	uint8 target_seat_rotation;		// 0xD9
} rct_vehicle;
assert_struct_size(rct_vehicle, 0xDA);
#pragma pack(pop)

typedef struct train_ref {
	rct_vehicle *head;
	rct_vehicle *tail;
} train_ref;

// Size: 0x09
typedef struct rct_vehicle_info {
	sint16 x;			// 0x00
	sint16 y;			// 0x02
	sint16 z;			// 0x04
	uint8 direction;	// 0x06
	uint8 vehicle_sprite_type;	// 0x07
	uint8 bank_rotation;	// 0x08
} rct_vehicle_info;

enum {
	VEHICLE_ENTRY_FLAG_A_0 = 1 << 0,
	VEHICLE_ENTRY_FLAG_A_1 = 1 << 1,
	VEHICLE_ENTRY_FLAG_A_2 = 1 << 2,
	VEHICLE_ENTRY_FLAG_A_MINI_GOLF = 1 << 3,
	VEHICLE_ENTRY_FLAG_A_4 = 1 << 4,
	VEHICLE_ENTRY_FLAG_A_5 = 1 << 5,
	VEHICLE_ENTRY_FLAG_A_6 = 1 << 6,
	VEHICLE_ENTRY_FLAG_A_7 = 1 << 7,
	VEHICLE_ENTRY_FLAG_A_8 = 1 << 8,
	VEHICLE_ENTRY_FLAG_A_9 = 1 << 9,
	VEHICLE_ENTRY_FLAG_A_10 = 1 << 10,
	VEHICLE_ENTRY_FLAG_A_11 = 1 << 11,
	VEHICLE_ENTRY_FLAG_A_12 = 1 << 12,
	VEHICLE_ENTRY_FLAG_A_13 = 1 << 13,
	VEHICLE_ENTRY_FLAG_A_14 = 1 << 14,
	VEHICLE_ENTRY_FLAG_A_15 = 1 << 15,
};

enum {
	VEHICLE_ENTRY_FLAG_B_0 = 1 << 0,
	VEHICLE_ENTRY_FLAG_B_SWINGING = 1 << 1,
	VEHICLE_ENTRY_FLAG_B_SPINNING = 1 << 2,
	VEHICLE_ENTRY_FLAG_B_3 = 1 << 3,
	VEHICLE_ENTRY_FLAG_B_4 = 1 << 4,
	VEHICLE_ENTRY_FLAG_B_5 = 1 << 5,
	VEHICLE_ENTRY_FLAG_B_6 = 1 << 6,
	VEHICLE_ENTRY_FLAG_B_7 = 1 << 7,
	VEHICLE_ENTRY_FLAG_B_8 = 1 << 8,
	VEHICLE_ENTRY_FLAG_B_9 = 1 << 9,
	VEHICLE_ENTRY_FLAG_B_10 = 1 << 10,
	VEHICLE_ENTRY_FLAG_B_11 = 1 << 11,
	VEHICLE_ENTRY_FLAG_B_12 = 1 << 12,
	VEHICLE_ENTRY_FLAG_B_13 = 1 << 13,
	VEHICLE_ENTRY_FLAG_B_14 = 1 << 14,
	VEHICLE_ENTRY_FLAG_B_15 = 1 << 15,
};

enum {
	VEHICLE_STATUS_MOVING_TO_END_OF_STATION,
	VEHICLE_STATUS_WAITING_FOR_PASSENGERS,
	VEHICLE_STATUS_WAITING_TO_DEPART,
	VEHICLE_STATUS_DEPARTING,
	VEHICLE_STATUS_TRAVELLING,
	VEHICLE_STATUS_ARRIVING,
	VEHICLE_STATUS_UNLOADING_PASSENGERS,
	VEHICLE_STATUS_TRAVELLING_BOAT,
	VEHICLE_STATUS_CRASHING,
	VEHICLE_STATUS_CRASHED,
	VEHICLE_STATUS_TRAVELLING_BUMPER_CARS,
	VEHICLE_STATUS_SWINGING,
	VEHICLE_STATUS_ROTATING,
	VEHICLE_STATUS_FERRIS_WHEEL_ROTATING,
	VEHICLE_STATUS_SIMULATOR_OPERATING,
	VEHICLE_STATUS_SHOWING_FILM,
	VEHICLE_STATUS_SPACE_RINGS_OPERATING,
	VEHICLE_STATUS_TOP_SPIN_OPERATING,
	VEHICLE_STATUS_HAUNTED_HOUSE_OPERATING,
	VEHICLE_STATUS_DOING_CIRCUS_SHOW,
	VEHICLE_STATUS_CROOKED_HOUSE_OPERATING,
	VEHICLE_STATUS_WAITING_FOR_CABLE_LIFT,
	VEHICLE_STATUS_TRAVELLING_CABLE_LIFT,
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
	VEHICLE_UPDATE_FLAG_ON_BREAK_FOR_DROP = (1 << 10),
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
	VEHICLE_VISUAL_SPLASH1_EFFECT,
	VEHICLE_VISUAL_SPLASH2_EFFECT,
	VEHICLE_VISUAL_SPLASH3_EFFECT,
	VEHICLE_VISUAL_SPLASH4_EFFECT,
	VEHICLE_VISUAL_SPLASH5_EFFECT,
	VEHICLE_VISUAL_VIRGINIA_REEL,
	VEHICLE_VISUAL_SUBMARINE
};

enum {
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION = 1 << 0,
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_1 = 1 << 1,
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_2 = 1 << 2,
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_3 = 1 << 3,
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL = 1 << 4,
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_5 = 1 << 5,
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_DERAILED = 1 << 6,
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION = 1 << 7,
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_8 = 1 << 8,
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_9 = 1 << 9,
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_10 = 1 << 10,
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_11 = 1 << 11,
	VEHICLE_UPDATE_MOTION_TRACK_FLAG_12 = 1 << 12,
};

#define VEHICLE_SEAT_PAIR_FLAG	0x80
#define VEHICLE_SEAT_NUM_MASK	0x7F

void vehicle_update_all();
int sub_6BC2F3(rct_vehicle* vehicle);
void vehicle_sounds_update();
void vehicle_get_g_forces(rct_vehicle *vehicle, int *verticalG, int *lateralG);
void vehicle_set_map_toolbar(rct_vehicle *vehicle);
int vehicle_is_used_in_pairs(rct_vehicle *vehicle);
rct_vehicle *vehicle_get_head(rct_vehicle *vehicle);
int vehicle_update_track_motion(rct_vehicle *vehicle, int *outStation);
rct_ride_entry_vehicle *vehicle_get_vehicle_entry(rct_vehicle *vehicle);
int vehicle_get_total_num_peeps(rct_vehicle *vehicle);
void vehicle_invalidate_window(rct_vehicle *vehicle);
void vehicle_update_test_finish(rct_vehicle* vehicle);
void vehicle_test_reset(rct_vehicle* vehicle);
void vehicle_peep_easteregg_here_we_are(rct_vehicle* vehicle);
rct_vehicle *vehicle_get_head(rct_vehicle *vehicle);
rct_vehicle *vehicle_get_tail(rct_vehicle *vehicle);
const rct_vehicle_info *vehicle_get_move_info(int cd, int typeAndDirection, int offset);
uint16 vehicle_get_move_info_size(int cd, int typeAndDirection);
bool vehicle_update_bumper_car_collision(rct_vehicle *vehicle, sint16 x, sint16 y, uint16 *spriteId);

/** Helper macro until rides are stored in this module. */
#define GET_VEHICLE(sprite_index) &(g_sprite_list[sprite_index].vehicle)

#endif
