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

#ifndef _RIDE_TYPE_H_
#define _RIDE_TYPE_H_

#include "../common.h"
#include "ride.h"

typedef struct {
	uint64 flags;					// RIDE_TYPE_DESC_FLAG
	uint64 operating_modes;			// RIDE_MODE as flags
	uint16 breakdowns;				// BREAKDOWN as flags
	uint8 default_music_style;		// MUSIC_STYLE

	uint8 max_height;
	uint8 clearance_height;
	sint8 z_offset;
	uint8 max_friction;
	uint8 z;
	uint8 typical_price_multiplier;
	uint8 bonus_value;				// Deprecated. Use rideBonusValue instead

	uint8 lift_hill_sound;
	uint8 lift_hill_min_speed;
	uint8 lift_hill_max_speed;

	// Prices
	uint8 initial_upkeep_cost;
	uint8 cost_per_track_piece;
	money8 price_primary;
	money8 price_secondary;
	money16 price_track;
	money16 price_supports;

	rating_tuple value_rating_multiplier;

	// Strings
	rct_string_id vehicle_name;
	rct_string_id structure_name;
	rct_string_id station_name;
} ride_type_desc;

enum {
	// All rides that have more than one piece than can be built for them,
	// with the exception of BOAT_HIRE and MAZE
	RIDE_TYPE_DESC_FLAG_HAS_RUNNING_TRACK				= 1 << 0,

	// The following flags are taken from RIDE_TYPE_FLAG4
	RIDE_TYPE_DESC_FLAG_FLAG4_0							= 1 << 0,		// FLAG4:0
	RIDE_TYPE_DESC_FLAG_MUSIC_ON_DEFAULT				= 1 << 1,		// FLAG4:1
	RIDE_TYPE_DESC_FLAG_ALLOW_MUSIC						= 1 << 2,		// FLAG4:2
	RIDE_TYPE_DESC_FLAG_FLAG4_3							= 1 << 3,		// FLAG4:3
	RIDE_TYPE_DESC_FLAG_PEEP_CHECK_GFORCES				= 1 << 4,		// FLAG4:4
	RIDE_TYPE_DESC_FLAG_HAS_ENTRANCE_EXIT				= 1 << 5,		// FLAG4:5
	RIDE_TYPE_DESC_FLAG_FLAG4_6							= 1 << 6,		// FLAG4:6
	RIDE_TYPE_DESC_FLAG_HAS_AIR_TIME					= 1 << 7,		// FLAG4:7
	RIDE_TYPE_DESC_FLAG_SINGLE_SESSION					= 1 << 8,		// FLAG4:8
	RIDE_TYPE_DESC_FLAG_ALLOW_MULTIPLE_CIRCUITS			= 1 << 9,		// FLAG4:9
	RIDE_TYPE_DESC_FLAG_FLAG4_10						= 1 << 10,		// FLAG4:A
	RIDE_TYPE_DESC_FLAG_FLAG4_11						= 1 << 11,		// FLAG4:B
	RIDE_TYPE_DESC_FLAG_TRANSPORT_RIDE					= 1 << 12,		// FLAG4:C
	RIDE_TYPE_DESC_FLAG_FLAG4_13						= 1 << 13,		// FLAG4:D
	RIDE_TYPE_DESC_FLAG_FLAG4_14						= 1 << 14,		// FLAG4:E
	RIDE_TYPE_DESC_FLAG_FLAG4_15						= 1 << 15,		// FLAG4:F

};

ride_type_desc *get_ride_type_desc(uint8 rideType);

#endif
