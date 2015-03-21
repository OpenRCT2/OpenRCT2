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

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 var_01;
	uint16 next_in_quadrant;		// 0x02
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 linked_list_type_offset;	// 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
	// Height from center of sprite to bottom
	uint8 sprite_height_negative;	// 0x09
	uint16 sprite_index;			// 0x0A
	uint8 pad_0C[2];
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
	uint8 pad_20[0x08];
	sint32 velocity;				// 0x28
	uint8 pad_2C[0x04];
	uint8 ride;						// 0x30
	uint8 vehicle_type;				// 0x31
	uint8 pad_32[0x02];
	uint16 var_34;
	sint16 var_36;
	uint16 var_38;
	uint16 var_3A;
	uint16 var_3C;
	uint16 next_vehicle_on_train;	// 0x3E
	uint16 prev_vehicle_on_train;	// 0x40
	uint16 pad_42;
	uint16 var_44;
	uint16 var_46;
	uint16 var_48;
	uint8 pad_4A;
	uint8 current_station;			// 0x4B
	uint8 pad_4C[0x4];
	uint8 status;					// 0x50
	uint8 var_51;
	uint16 peep[32];				// 0x52
	uint8 peep_tshirt_colours[32];	// 0x92
	uint8 num_seats;				// 0xB2
	uint8 num_peeps;				// 0xB3
	uint8 next_free_seat;		// 0xB4
	uint8 pad_B5[0x06];
	uint8 sound1_id;				// 0xBB
	uint8 sound1_volume;			// 0xBC
	uint8 sound2_id;				// 0xBD
	uint8 sound2_volume;			// 0xBE
	sint8 var_BF;
	uint8 pad_C0[0x02];
	uint8 speed;					// 0xC2
	uint8 pad_C3[0x09];
	uint8 var_CC;
	uint8 var_CD;
	union {
		uint8 var_CE;
		uint8 num_laps;				// 0xCE
	};
	uint8 pad_CF[0x06];
	uint8 var_D5;
	uint8 ride_subtype;				// 0xD6
} rct_vehicle;

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

/** Helper macro until rides are stored in this module. */
#define GET_VEHICLE(sprite_index) &(g_sprite_list[sprite_index].vehicle)

#endif