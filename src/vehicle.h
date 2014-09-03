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

#include "rct2.h"

typedef union {
	struct {
		uint16 width;
		uint16 height;
	};
	uint32 both;
} rct_widthheight;

typedef struct {
	uint8 sprite_identifier;		// 0x00
	uint8 pad_01[0x03];
	uint16 next;					// 0x04
	uint16 previous;				// 0x06
	uint8 var_08;
	uint8 pad_09;
	uint16 sprite_index;			// 0x0A
	uint8 pad_0C[2];
	sint16 x;						// 0x0E
	sint16 y;						// 0x10
	sint16 z;						// 0x12
	uint8 pad_14[0x02];
	rct_widthheight var_16;
	rct_widthheight view;			// 0x1A
	uint16 var_1E;
	uint8 pad_20[0x08];
	sint32 velocity;
	uint8 pad_2C[0x04];
	uint8 ride;						// 0x30
	uint8 var_31;
	uint8 pad_32[0x04];
	sint16 var_36;
	uint8 pad_38[0x06];
	uint16 next_vehicle_on_train;	// 0x3E
	uint8 pad_40[0x08];
	uint16 var_48;
	uint8 pad_4A;
	uint8 var_4B;
	uint8 pad_4C[0x4];
	uint8 status;
	uint8 var_51;
	uint8 pad_52[0x2E];
	uint8 var_BB;
	uint8 var_BC;
	uint8 var_BD;
	uint8 pad_BE[0x0E];
	uint8 var_CC;
	uint8 pad_CD[0x09];
	uint8 var_D6;
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

void vehicle_update_all();

/** Helper macro until rides are stored in this module. */
#define GET_VEHICLE(sprite_index) &(g_sprite_list[sprite_index].vehicle)

#endif