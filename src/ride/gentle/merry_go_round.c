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

#include "../ride_type.h"

ride_type_desc RideTypeDescMerryGoRound = {
	.flags						= RIDE_TYPE_DESC_FLAG_MUSIC_ON_DEFAULT |
								  RIDE_TYPE_DESC_FLAG_ALLOW_MUSIC |
								  RIDE_TYPE_DESC_FLAG_HAS_ENTRANCE_EXIT |
								  RIDE_TYPE_DESC_FLAG_SINGLE_SESSION |
								  RIDE_TYPE_DESC_FLAG_FLAG4_13,

	.operating_modes			= (1 << RIDE_MODE_ROTATION),
	.breakdowns					= (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_CONTROL_FAILURE),

	.default_music_style		= MUSIC_STYLE_FAIRGROUND_ORGAN,

	.max_height					= 12,
	.clearance_height			= 64,
	.z_offset					= 3,
	.max_friction				= 255,
	.z							= 2,
	.typical_price_multiplier	= 1,
	.bonus_value				= 45,

	.lift_hill_sound			= 255,
	.lift_hill_min_speed		= 5,
	.lift_hill_max_speed		= 5,

	// Prices
	.initial_upkeep_cost		= 50,
	.cost_per_track_piece		= 0,
	.price_primary				= 10,
	.price_secondary			= 0,
	.price_track				= 115,
	.price_supports				= 2,

	.value_rating_multiplier	= { 50, 10, 0 },

	// Strings
	.vehicle_name				= 1264,
	.structure_name				= 1278,
	.station_name				= 1257
};
