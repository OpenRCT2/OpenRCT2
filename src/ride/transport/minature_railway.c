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

#include "../../common.h"
#include "../track_paint.h"
#include "../track.h"
#include "../vehicle_paint.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../ride_data.h"
#include "../../world/map.h"

enum
{
	SPR_MINATURE_RAILWAY_FLAT_SW_NE = 23341,
	SPR_MINATURE_RAILWAY_FLAT_NW_SE = 23342,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_0 = 23343,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_1 = 23344,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_2 = 23345,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_3 = 23346,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_4 = 23347,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_0 = 23348,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_1 = 23349,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_2 = 23350,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_3 = 23351,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_4 = 23352,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_0 = 23353,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_1 = 23354,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_2 = 23355,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_3 = 23356,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_4 = 23357,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_0 = 23358,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_1 = 23359,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_2 = 23360,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_3 = 23361,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_4 = 23362,
	SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_0 = 23363,
	SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_1 = 23364,
	SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_2 = 23365,
	SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_3 = 23366,
	SPR_MINATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_0 = 23367,
	SPR_MINATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_1 = 23368,
	SPR_MINATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_2 = 23369,
	SPR_MINATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_3 = 23370,
	SPR_MINATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_0 = 23371,
	SPR_MINATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_1 = 23372,
	SPR_MINATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_2 = 23373,
	SPR_MINATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_3 = 23374,
	SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_0 = 23375,
	SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_1 = 23376,
	SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_2 = 23377,
	SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_3 = 23378,
	SPR_MINATURE_RAILWAY_25_DEG_UP_SW_NE = 23379,
	SPR_MINATURE_RAILWAY_FLAT_TO_25_DEG_UP_SW_NE = 23380,
	SPR_MINATURE_RAILWAY_25_DEG_UP_NE_SW = 23381,
	SPR_MINATURE_RAILWAY_FLAT_TO_25_DEG_UP_NE_SW = 23382,
	SPR_MINATURE_RAILWAY_25_DEG_UP_TO_FLAT_NE_SW = 23383,
	SPR_MINATURE_RAILWAY_25_DEG_UP_TO_FLAT_SW_NE = 23384,
	SPR_MINATURE_RAILWAY_25_DEG_UP_NW_SE = 23385,
	SPR_MINATURE_RAILWAY_25_DEG_UP_SE_NW = 23386,
	SPR_MINATURE_RAILWAY_FLAT_TO_25_DEG_UP_NW_SE = 23387,
	SPR_MINATURE_RAILWAY_FLAT_TO_25_DEG_UP_SE_NW = 23388,
	SPR_MINATURE_RAILWAY_25_DEG_UP_TO_FLAT_SE_NW = 23389,
	SPR_MINATURE_RAILWAY_25_DEG_UP_TO_FLAT_NW_SE = 23390,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_0 = 23391,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_1 = 23392,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_2 = 23393,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_0 = 23394,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_1 = 23395,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_2 = 23396,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_0 = 23397,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_1 = 23398,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_2 = 23399,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_0 = 23400,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_1 = 23401,
	SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_2 = 23402,
	SPR_MINATURE_RAILWAY_FLAT_NO_BASE_SW_NE = 23403,
	SPR_MINATURE_RAILWAY_FLAT_NO_BASE_NW_SE = 23404,	
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_0 = 23413,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_1 = 23414,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_2 = 23415,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_3 = 23416,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_0 = 23417,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_1 = 23418,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_2 = 23419,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_3 = 23420,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_0 = 23421,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_1 = 23422,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_2 = 23423,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_3 = 23424,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_0 = 23425,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_1 = 23426,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_2 = 23427,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_3 = 23428,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_0 = 23421,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_1 = 23422,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_2 = 23423,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_3 = 23424,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_0 = 23425,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_1 = 23426,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_2 = 23427,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_3 = 23428,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_0 = 23429,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_1 = 23430,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_2 = 23431,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_3 = 23432,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_0 = 23433,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_1 = 23434,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_2 = 23435,
	SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_3 = 23436,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_W_E = 23445,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_N_S = 23446,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_E_W = 23447,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_S_N = 23448,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_W_E = 23449,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_N_S = 23450,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_E_W = 23451,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_S_N = 23452,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_W_E = 23453,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_N_S = 23454,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_E_W = 23455,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_S_N = 23456,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_W_E = 23457,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_N_S = 23458,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_E_W = 23459,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_S_N = 23460,
};

static const uint32 minature_railway_track_floor[4] = {
	SPR_FLOOR_PLANKS,
	SPR_FLOOR_PLANKS_90_DEG,
	SPR_FLOOR_PLANKS,
	SPR_FLOOR_PLANKS_90_DEG
};

static const uint32 minature_railway_track_pieces_flat[4] = {
	SPR_MINATURE_RAILWAY_FLAT_SW_NE,
	SPR_MINATURE_RAILWAY_FLAT_NW_SE,
	SPR_MINATURE_RAILWAY_FLAT_SW_NE,
	SPR_MINATURE_RAILWAY_FLAT_NW_SE,
};

static const uint32 minature_railway_station_floor[4] = {
	SPR_STATION_BASE_A_SW_NE,
	SPR_STATION_BASE_A_NW_SE,
	SPR_STATION_BASE_A_SW_NE,
	SPR_STATION_BASE_A_NW_SE
};

static const uint32 minature_railway_track_pieces_flat_station[4] = {
	SPR_MINATURE_RAILWAY_FLAT_NO_BASE_SW_NE,
	SPR_MINATURE_RAILWAY_FLAT_NO_BASE_NW_SE,
	SPR_MINATURE_RAILWAY_FLAT_NO_BASE_SW_NE,
	SPR_MINATURE_RAILWAY_FLAT_NO_BASE_NW_SE,
};

static const uint32 minature_railway_track_pieces_flat_quarter_turn_5_tiles[4][5] = {
	{
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_0,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_1,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_2,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_3,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_4,
	},
	{
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_0,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_1,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_2,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_3,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_4,
	},
	{
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_0,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_1,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_2,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_3,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_4,
	},
	{
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_0,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_1,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_2,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_3,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_4,
	}
};

static const uint32 minature_railway_track_pieces_25_deg_up[4] = {
	SPR_MINATURE_RAILWAY_25_DEG_UP_SW_NE,
	SPR_MINATURE_RAILWAY_25_DEG_UP_NW_SE,
	SPR_MINATURE_RAILWAY_25_DEG_UP_NE_SW,
	SPR_MINATURE_RAILWAY_25_DEG_UP_SE_NW,
};

static const uint32 minature_railway_track_pieces_flat_to_25_deg_up[4] = {
	SPR_MINATURE_RAILWAY_FLAT_TO_25_DEG_UP_SW_NE,
	SPR_MINATURE_RAILWAY_FLAT_TO_25_DEG_UP_NW_SE,
	SPR_MINATURE_RAILWAY_FLAT_TO_25_DEG_UP_NE_SW,
	SPR_MINATURE_RAILWAY_FLAT_TO_25_DEG_UP_SE_NW,
};

static const uint32 minature_railway_track_pieces_25_deg_up_to_flat[4] = {
	SPR_MINATURE_RAILWAY_25_DEG_UP_TO_FLAT_SW_NE,
	SPR_MINATURE_RAILWAY_25_DEG_UP_TO_FLAT_NW_SE,
	SPR_MINATURE_RAILWAY_25_DEG_UP_TO_FLAT_NE_SW,
	SPR_MINATURE_RAILWAY_25_DEG_UP_TO_FLAT_SE_NW,
};

static const uint32 minature_railway_track_pieces_s_bend_left[2][4] = {
	{
		SPR_MINATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_0,
		SPR_MINATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_1,
		SPR_MINATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_2,
		SPR_MINATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_3,
	},
	{
		SPR_MINATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_3,
		SPR_MINATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_2,
		SPR_MINATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_1,
		SPR_MINATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_0,
	}
};

static const uint32 minature_railway_track_pieces_s_bend_right[2][4] = {
	{
		SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_0,
		SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_1,
		SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_2,
		SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_3,
	},
	{
		SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_3,
		SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_2,
		SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_1,
		SPR_MINATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_0,
	}
};

static const uint32 minature_railway_track_pieces_flat_quarter_turn_3_tiles[4][3] = {
	{
		SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_0,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_1,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_2
	},
	{
		SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_0,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_1,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_2
	},
	{
		SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_0,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_1,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_2
	},
	{
		SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_0,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_1,
		SPR_MINATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_2
	}
};

static const uint32 minature_railway_track_pieces_right_eight_to_diag[4][4] = {
	{
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_0,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_1,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_2,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_3,
	},
	{
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_0,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_1,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_2,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_3,
	},
	{
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_0,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_1,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_2,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_3,
	},
	{
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_0,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_1,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_2,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_3,
	},
};

static const rct_xy16 minature_railway_track_pieces_right_eight_to_diag_bounds[4][4] = {
	{
		{32, 20},
		{32, 16},
		{16, 16},
		{16, 16},
	},
	{
		{20, 32},
		{16, 32},
		{16, 16},
		{16, 16},
	},
	{
		{32, 20},
		{34, 16},
		{28, 28},
		{16, 18},
	},
	{
		{20, 32},
		{16, 32},
		{16, 16},
		{16, 16},
	},
};

static const rct_xy16 minature_railway_track_pieces_right_eight_to_diag_offset[4][4] = {
	{
		{0, 6},
		{0,  16},
		{0,  0},
		{16, 0},
	},
	{
		{6, 0},
		{16, 0},
		{0,  16},
		{0,  0},
	},
	{
		{0, 6},
		{0,  0},
		{4,  4},
		{0,  16},
	},
	{
		{6, 0},
		{0,  0},
		{16, 0},
		{16, 16},
	},
};

static const uint32 minature_railway_track_pieces_left_eight_to_diag[4][4] = {
	{
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_0,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_1,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_2,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_3,
	},
	{
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_0,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_1,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_2,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_3,
	},
	{
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_0,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_1,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_2,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_3,
	},
	{
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_0,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_1,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_2,
		SPR_MINATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_3,
	},
};

static const rct_xyz16 minature_railway_track_pieces_left_eight_to_diag_bounds[4][4] = {
	{
		{32, 32, 2},
		{32, 16, 2},
		{16, 16, 2},
		{30, 30, 0},
	},
	{
		{32, 32, 2},
		{16, 34, 2},
		{14, 14, 2},
		{34, 32, 0},
	},
	{
		{32, 32, 2},
		{32, 16, 2},
		{16, 16, 2},
		{16, 16, 2},
	},
	{
		{32, 32, 2},
		{16, 32, 2},
		{16, 16, 2},
		{32, 32, 0},
	},
};

static const rct_xy16 minature_railway_track_pieces_left_eight_to_diag_offset[4][4] = {
	{
		{0, 0},
		{0,  0},
		{0,  16},
		{0, 0},
	},
	{
		{0, 0},
		{0,  0},
		{16, 16},
		{0, 0},
	},
	{
		{0, 0},
		{0,  16},
		{16, 0},
		{0,  0},
	},
	{
		{0, 0},
		{16, 0},
		{0,  0},
		{0,  0},
	},
};

static const uint32 minature_railway_track_pieces_diag_flat[4] = {
	SPR_MINATURE_RAILWAY_DIAG_FLAT_W_E,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_N_S,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_E_W,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_S_N,
};

static const uint32 minature_railway_track_pieces_diag_flat_to_25_deg_up[4] = {
	SPR_MINATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_W_E,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_N_S,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_E_W,
	SPR_MINATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_S_N,
};

static const uint32 minature_railway_track_pieces_diag_25_deg_up_to_flat[4] = {
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_W_E,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_N_S,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_E_W,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_S_N,
};

static const uint32 minature_railway_track_pieces_diag_25_deg_up[4] = {
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_W_E,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_N_S,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_E_W,
	SPR_MINATURE_RAILWAY_DIAG_25_DEG_UP_S_N,
};

/** rct2: 0x008AD0C0 */
static void paint_minature_railway_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_xy16 position = {gPaintMapPosition.x, gPaintMapPosition.y};

	bool isSupported = wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
	uint32 imageId;

	if (isSupported) {
		imageId = minature_railway_track_floor[direction] | gTrackColours[SCHEME_SUPPORTS];
		sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);

		imageId = minature_railway_track_pieces_flat[direction] | gTrackColours[SCHEME_TRACK];	
		sub_98199C_rotated(direction, imageId, 0, 6, 32, 20, 2, height, 0, 6, height);
	}
	else {
		imageId = minature_railway_track_pieces_flat[direction] | gTrackColours[SCHEME_TRACK];
		sub_98197C_rotated(direction, imageId, 0, 6, 32, 20, 2, height, 0, 6, height);
	}

	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AD170, 0x008AD180, 0x008AD190 */
static void paint_minature_railway_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;

	wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	imageId = minature_railway_station_floor[direction] | gTrackColours[SCHEME_MISC];
	sub_98197C_rotated(direction, imageId, 0, 0, 32, 28, 2, height - 2, 0, 2, height);

	imageId = minature_railway_track_pieces_flat_station[direction] | gTrackColours[SCHEME_TRACK];
	sub_98199C_rotated(direction, imageId, 0, 6, 32, 20, 2, height, 0, 0, height);

	paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

	track_paint_util_draw_station(rideIndex, trackSequence, direction, height + 2, mapElement);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 30, 0x20);
}

/** rct2: 0x008AE1BC */
static void paint_minature_railway_track_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_xy16 position = {gPaintMapPosition.x, gPaintMapPosition.y};

	uint32 imageId = minature_railway_track_pieces_25_deg_up[direction] | gTrackColours[SCHEME_TRACK];

	sub_98197C_rotated(direction, imageId, 0, 2, 32, 25, 2, height, 0, 3, height);

	switch (direction) {
		case 0: paint_util_push_tunnel_left(height - 8, TUNNEL_7); break;
		case 1: paint_util_push_tunnel_right(height + 8, TUNNEL_8); break;
		case 2: paint_util_push_tunnel_left(height + 8, TUNNEL_8); break;
		case 3: paint_util_push_tunnel_right(height - 8, TUNNEL_7); break;
	}

	wooden_a_supports_paint_setup(direction & 1, 45 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);
	
	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AE1CC */
static void paint_minature_railway_track_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_xy16 position = {gPaintMapPosition.x, gPaintMapPosition.y};

	uint32 imageId = minature_railway_track_pieces_flat_to_25_deg_up[direction] | gTrackColours[SCHEME_TRACK];

	sub_98197C_rotated(direction, imageId, 0, 2, 32, 25, 2, height, 0, 3, height);

	switch (direction) {
		case 0: paint_util_push_tunnel_left(height, TUNNEL_6); break;
		case 1: paint_util_push_tunnel_right(height, TUNNEL_8); break;
		case 2: paint_util_push_tunnel_left(height, TUNNEL_8); break;
		case 3: paint_util_push_tunnel_right(height, TUNNEL_6); break;
	}

	wooden_a_supports_paint_setup(direction & 1, 37 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AE1DC */
static void paint_minature_railway_track_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	rct_xy16 position = {gPaintMapPosition.x, gPaintMapPosition.y};

	uint32 imageId = minature_railway_track_pieces_25_deg_up_to_flat[direction] | gTrackColours[SCHEME_TRACK];

	sub_98197C_rotated(direction, imageId, 0, 2, 32, 25, 2, height, 0, 3, height);
	
	switch (direction) {
		case 0: paint_util_push_tunnel_left(height - 8, TUNNEL_6); break;
		case 1: paint_util_push_tunnel_right(height + 8, TUNNEL_14); break;
		case 2: paint_util_push_tunnel_left(height + 8, TUNNEL_14); break;
		case 3: paint_util_push_tunnel_right(height - 8, TUNNEL_6); break;
	}

	wooden_a_supports_paint_setup(direction & 1, 41 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
	paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008AE1EC */
static void paint_minature_railway_track_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	paint_minature_railway_track_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008AE1FC */
static void paint_minature_railway_track_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	paint_minature_railway_track_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008AE20C */
static void paint_minature_railway_track_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	paint_minature_railway_track_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

const rct_xy16 minature_railway_right_quarter_turn_5_tiles_offsets[4][5] = {
	{
		{0, 2},
		{0,  16},
		{0,  0},
		{16, 0},
		{2, 0},
	},
	{
		{2, 0},
		{16, 0},
		{0,  16},
		{0,  0},
		{0, 2},
	},
	{
		{0, 2},
		{0,  0},
		{16, 16},
		{0,  0},
		{2, 0},
	},
	{
		{2, 0},
		{0,  0},
		{16, 0},
		{0,  16},
		{0, 2},
	}
};

const rct_xyz16 minature_railway_right_quarter_turn_5_tiles_bound_offsets[4][5] = {
	{
		{0, 2, 0},
		{0,  16, 0},
		{0,  0, 0},
		{16, 0, 0},
		{2, 0, 0},
	},
	{
		{2, 0, 0},
		{16, 0, 0},
		{0,  16, 0},
		{0,  0, 0},
		{0, 2, 0},
	},
	{
		{0, 2, 0},
		{0,  0, 0},
		{16, 16, 0},
		{0,  0, 0},
		{2, 0, 0},
	},
	{
		{2, 0, 0},
		{0,  0, 0},
		{16, 0, 0},
		{0,  16, 0},
		{0, 2, 0},
	}
};

const rct_xy16 minature_railway_right_quarter_turn_5_tiles_bound_lengths[4][5] = {
	{
		{32, 32},
		{32, 16},
		{16, 16},
		{16, 32},
		{32, 32},
	},
	{
		{32, 32},
		{16, 34},
		{16, 16},
		{32, 16},
		{32, 27},
	},
	{
		{32, 27},
		{32, 16},
		{16, 16},
		{16, 32},
		{27, 32},
	},
	{
		{27, 32},
		{16, 32},
		{16, 16},
		{32, 16},
		{32, 32},
	}
};

static const uint32 minature_railway_right_quarter_turn_5_tiles_track_floor[4][5] = {
	{
		SPR_FLOOR_PLANKS,
		SPR_FLOOR_PLANKS_S_SEGMENT,
		SPR_FLOOR_PLANKS_N_SEGMENT,
		SPR_FLOOR_PLANKS_S_SEGMENT,
		SPR_FLOOR_PLANKS_90_DEG
	},
	{
		SPR_FLOOR_PLANKS_90_DEG,
		SPR_FLOOR_PLANKS_W_SEGMENT,
		SPR_FLOOR_PLANKS_E_SEGMENT,
		SPR_FLOOR_PLANKS_W_SEGMENT,
		SPR_FLOOR_PLANKS
	},
	{
		SPR_FLOOR_PLANKS,
		SPR_FLOOR_PLANKS_N_SEGMENT,
		SPR_FLOOR_PLANKS_S_SEGMENT,
		SPR_FLOOR_PLANKS_N_SEGMENT,
		SPR_FLOOR_PLANKS_90_DEG
	},
	{
		SPR_FLOOR_PLANKS_90_DEG,
		SPR_FLOOR_PLANKS_E_SEGMENT,
		SPR_FLOOR_PLANKS_W_SEGMENT,
		SPR_FLOOR_PLANKS_E_SEGMENT,
		SPR_FLOOR_PLANKS
	},
};

static const sint8 right_quarter_turn_5_supports_type[4][7] = {
	{0, -1, 4, 2, -1, 4, 1},
	{1, -1, 5, 3, -1, 5, 0},
	{0, -1, 2, 4, -1, 2, 1},
	{1, -1, 3, 5, -1, 3, 0}
};

static const sint8 right_quarter_turn_5_tiles_sprite_map[] = {0, -1, 1, 2, -1, 3, 4};

/** rct2: 0x008AD140 */
static void paint_minature_railway_track_right_quarter_turn_5_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{

	if (right_quarter_turn_5_supports_type[direction][trackSequence] != -1){
		bool isSupported = wooden_a_supports_paint_setup(right_quarter_turn_5_supports_type[direction][trackSequence], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

		if (isSupported == false || (trackSequence == 3 && direction == 2)) {
			track_paint_util_right_quarter_turn_5_tiles_paint(2, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], minature_railway_track_pieces_flat_quarter_turn_5_tiles, minature_railway_right_quarter_turn_5_tiles_offsets, minature_railway_right_quarter_turn_5_tiles_bound_lengths, NULL, get_current_rotation());
		}
		else {
			track_paint_util_right_quarter_turn_5_tiles_paint(2, height, direction, trackSequence, gTrackColours[SCHEME_SUPPORTS], minature_railway_right_quarter_turn_5_tiles_track_floor, NULL, minature_railway_right_quarter_turn_5_tiles_bound_lengths, minature_railway_right_quarter_turn_5_tiles_bound_offsets, get_current_rotation());
			
			int index = right_quarter_turn_5_tiles_sprite_map[trackSequence];
			uint32 imageId = minature_railway_track_pieces_flat_quarter_turn_5_tiles[direction][index] | gTrackColours[SCHEME_TRACK];
			rct_xy16 offset = minature_railway_right_quarter_turn_5_tiles_offsets[direction][index];
			rct_xy16 boundsLength = minature_railway_right_quarter_turn_5_tiles_bound_lengths[direction][index];
			rct_xyz16 boundsOffset = { .x = offset.x, .y = offset.y, .z = 0 };

			sub_98199C(imageId, (sint8) offset.x, (sint8) offset.y, boundsLength.x, boundsLength.y, 2, height, boundsOffset.x, boundsOffset.y, height + boundsOffset.z, get_current_rotation());
		}
	}
	if (direction == 0 && trackSequence == 0) {
		paint_util_push_tunnel_left(height, TUNNEL_6);
	}

	if (direction == 0 && trackSequence == 6) {
		paint_util_push_tunnel_right(height, TUNNEL_6);
	}

	if (direction == 1 && trackSequence == 6) {
		paint_util_push_tunnel_left(height, TUNNEL_6);
	}

	if (direction == 3 && trackSequence == 0) {
		paint_util_push_tunnel_right(height, TUNNEL_6);
	}

	int blockedSegments = 0;
	switch (trackSequence) {
		case 0: blockedSegments = SEGMENTS_ALL; break;
		case 1: blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC; break;
		case 2: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC; break;
		case 3: blockedSegments = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4; break;
		case 4: blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC; break;
		case 5: blockedSegments = SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8; break;
		case 6: blockedSegments = SEGMENTS_ALL; break;
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);

	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AE21C */
static void paint_minature_railway_track_left_quarter_turn_5_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
	paint_minature_railway_track_right_quarter_turn_5_tiles(rideIndex, trackSequence, (direction + 1) % 4, height, mapElement);
}

static const sint8 s_bend_left_supports_type[4][4] = {
	{0, 5, 3, 0},
	{1, 2, 4, 1},
	{0, 5, 3, 0},
	{1, 2, 4, 1}
};

static const uint32 minature_railway_s_bend_left_tiles_track_floor[2][4] = {
	{
		SPR_FLOOR_PLANKS,
		SPR_FLOOR_PLANKS_W_SEGMENT,
		SPR_FLOOR_PLANKS_E_SEGMENT,
		SPR_FLOOR_PLANKS
	},
	{
		SPR_FLOOR_PLANKS_90_DEG,
		SPR_FLOOR_PLANKS_N_SEGMENT,
		SPR_FLOOR_PLANKS_S_SEGMENT,
		SPR_FLOOR_PLANKS_90_DEG,
	}
};

/** rct2: 0x8AD150 */
static void paint_minature_railway_track_s_bend_left(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (direction == 2 || direction == 3) {
		trackSequence = 3 - trackSequence;
	}

	bool isSupported = wooden_a_supports_paint_setup(s_bend_left_supports_type[direction][trackSequence], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	const rct_xy16 offsetList[] = {
		{0, 2},
		{0, 0},
		{0, 6},
		{0, 2},
	};

	const rct_xy16 boundsList[] = {
		{32, 27},
		{32, 26},
		{32, 26},
		{32, 27},
	};

	uint32 imageId = minature_railway_track_pieces_s_bend_left[direction & 1][trackSequence] | gTrackColours[SCHEME_TRACK];
	rct_xy16 offset = offsetList[trackSequence];
	rct_xy16 bounds = boundsList[trackSequence];
	
	if (isSupported == false) {
		sub_98197C_rotated(direction, imageId, (sint8)offset.x, (sint8)offset.y, bounds.x, bounds.y, 2, height, offset.x, offset.y, height);
	}
	else {
		imageId = minature_railway_s_bend_left_tiles_track_floor[direction & 1][trackSequence] | gTrackColours[SCHEME_SUPPORTS];
		sub_98197C_rotated(direction, imageId, 0, 0, bounds.x, bounds.y, 2, height, offset.x, offset.y, height);

		imageId = minature_railway_track_pieces_s_bend_left[direction & 1][trackSequence] | gTrackColours[SCHEME_TRACK];
		sub_98199C_rotated(direction, imageId, (sint8)offset.x, (sint8)offset.y, bounds.x, bounds.y, 2, height, offset.x, offset.y, height);
	}
	if (direction == 0 || direction == 2) {
		if (trackSequence == 0) {
			paint_util_push_tunnel_left(height, TUNNEL_6);
		}
	} else {
		if (trackSequence == 3) {
			paint_util_push_tunnel_right(height, TUNNEL_6);
		}
	}

	int blockedSegments = 0;
	switch (trackSequence) {
		case 0: blockedSegments = SEGMENTS_ALL; break;
		case 1: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4; break;
		case 2: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC; break;
		case 3: blockedSegments = SEGMENTS_ALL; break;
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction & 1), 0xFFFF, 0);

	paint_util_set_general_support_height(height + 32, 0x20);
}

static const sint8 s_bend_right_supports_type[4][4] = {
	{0, 4, 2, 0},
	{1, 5, 3, 1},
	{0, 4, 2, 0},
	{1, 5, 3, 1}
};

static const uint32 minature_railway_s_bend_right_tiles_track_floor[2][4] = {
	{
		SPR_FLOOR_PLANKS,
		SPR_FLOOR_PLANKS_S_SEGMENT,
		SPR_FLOOR_PLANKS_N_SEGMENT,
		SPR_FLOOR_PLANKS
	},
	{
		SPR_FLOOR_PLANKS_90_DEG,
		SPR_FLOOR_PLANKS_W_SEGMENT,
		SPR_FLOOR_PLANKS_E_SEGMENT,
		SPR_FLOOR_PLANKS_90_DEG,
	}
};

/** rct2: 0x008AD160 */
static void paint_minature_railway_track_s_bend_right(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (direction == 2 || direction == 3) {
		trackSequence = 3 - trackSequence;
	}

	bool isSupported = wooden_a_supports_paint_setup(s_bend_right_supports_type[direction][trackSequence], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	const rct_xy16 offsetList[] = {
		{0, 2},
		{0, 6},
		{0, 0},
		{0, 2},
	};

	const rct_xy16 boundsList[] = {
		{32, 27},
		{32, 26},
		{32, 26},
		{32, 27},
	};

	uint32 imageId = minature_railway_track_pieces_s_bend_right[direction&1][trackSequence] | gTrackColours[SCHEME_TRACK];
	rct_xy16 offset = offsetList[trackSequence];
	rct_xy16 bounds = boundsList[trackSequence];
	if (isSupported == false) {
		sub_98197C_rotated(direction, imageId, (sint8)offset.x, (sint8)offset.y, bounds.x, bounds.y, 2, height, offset.x, offset.y, height);
	}
	else {
		imageId = minature_railway_s_bend_right_tiles_track_floor[direction & 1][trackSequence] | gTrackColours[SCHEME_SUPPORTS];
		sub_98197C_rotated(direction, imageId, 0, 0, bounds.x, bounds.y, 2, height, offset.x, offset.y, height);

		imageId = minature_railway_track_pieces_s_bend_right[direction & 1][trackSequence] | gTrackColours[SCHEME_TRACK];
		sub_98199C_rotated(direction, imageId, (sint8)offset.x, (sint8)offset.y, bounds.x, bounds.y, 2, height, offset.x, offset.y, height);
	}

	if (direction == 0 || direction == 2) {
		if (trackSequence == 0) {
			paint_util_push_tunnel_left(height, TUNNEL_6);
		}
	} else {
		if (trackSequence == 3) {
			paint_util_push_tunnel_right(height, TUNNEL_6);
		}
	}

	int blockedSegments = 0;
	switch (trackSequence) {
		case 0: blockedSegments = SEGMENTS_ALL; break;
		case 1: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC; break;
		case 2: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4; break;
		case 3: blockedSegments = SEGMENTS_ALL; break;
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction & 1), 0xFFFF, 0);

	paint_util_set_general_support_height(height + 32, 0x20);
}

static const uint32 minature_railway_right_quarter_turn_3_tile_track_floor[4][3] = {
	{
		SPR_FLOOR_PLANKS_S_SEGMENT,
		0,
		SPR_FLOOR_PLANKS_S_SEGMENT,
	},
	{
		SPR_FLOOR_PLANKS_W_SEGMENT,
		0,
		SPR_FLOOR_PLANKS_W_SEGMENT,
	},
	{
		SPR_FLOOR_PLANKS_N_SEGMENT,
		0,
		SPR_FLOOR_PLANKS_N_SEGMENT,
	},
	{
		SPR_FLOOR_PLANKS_E_SEGMENT,
		0,
		SPR_FLOOR_PLANKS_E_SEGMENT,
	}
};

const rct_xyz16 minature_railway_right_quarter_turn_3_tile_bound_offsets[4][3] = {
	{
		{0, 6, 0},
		{16, 16, 0},
		{6, 0, 0},
	},
	{
		{6, 0, 0},
		{16, 0, 0},
		{0, 6, 0},
	},
	{
		{0, 6, 0},
		{0,  0, 0},
		{6, 0, 0},
	},
	{
		{6, 0, 0},
		{0,  16, 0},
		{0, 6, 0},
	}
};

/** rct2: 0x008AD1B0 */
static void paint_minature_railway_track_right_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	bool isSupported = false;
	if (trackSequence != 1 && trackSequence != 2)
	{
		const uint8 supportType[] = { 4, 5, 2, 3 };
		isSupported = wooden_a_supports_paint_setup(supportType[direction], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
	}
	if (isSupported == false) {
		track_paint_util_right_quarter_turn_3_tiles_paint(3, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], minature_railway_track_pieces_flat_quarter_turn_3_tiles, defaultRightQuarterTurn3TilesOffsets, defaultRightQuarterTurn3TilesBoundLengths, NULL, get_current_rotation());
	}
	else {
		track_paint_util_right_quarter_turn_3_tiles_paint(3, height, direction, trackSequence, gTrackColours[SCHEME_SUPPORTS], minature_railway_right_quarter_turn_3_tile_track_floor, NULL, defaultRightQuarterTurn3TilesBoundLengths, minature_railway_right_quarter_turn_3_tile_bound_offsets, get_current_rotation());
	
		static const sint8 right_quarter_turn_3_tiles_sprite_map[] = {0, -1, 1, 2};

		int index = right_quarter_turn_3_tiles_sprite_map[trackSequence];

		uint32 imageId = minature_railway_track_pieces_flat_quarter_turn_3_tiles[direction][index] | gTrackColours[SCHEME_TRACK];
		rct_xy16 offset = defaultRightQuarterTurn3TilesOffsets[direction][index];
		rct_xy16 boundsLength = defaultRightQuarterTurn3TilesBoundLengths[direction][index];
		rct_xyz16 boundsOffset = { .x = offset.x, .y = offset.y, .z = 0 };

		sub_98199C(imageId, (sint8) offset.x, (sint8) offset.y, boundsLength.x, boundsLength.y, 3, height, boundsOffset.x, boundsOffset.y, height + boundsOffset.z, get_current_rotation());
	}
	track_paint_util_right_quarter_turn_3_tiles_tunnel(height, direction, trackSequence, TUNNEL_6);

	int blockedSegments = 0;
	switch (trackSequence) {
		case 0: blockedSegments = SEGMENTS_ALL; break;
		case 2: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0; break;
		case 3: blockedSegments = SEGMENTS_ALL; break;
	}
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);

	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AE28C */
static void paint_minature_railway_track_left_quarter_turn_3_tiles(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
	paint_minature_railway_track_right_quarter_turn_3_tiles(rideIndex, trackSequence, (direction + 1) % 4, height, mapElement);
}

static const sint8 paint_minature_railway_eighth_to_diag_index[] = {0, 1, 2, -1, 3};

/** rct2: 0x008AE31C */
static void paint_minature_railway_track_left_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	const uint8 supportType[4][5] = {
		{ 0, 0, 3, 5, 1 },
		{ 1, 1, 4, 2, 0 },
		{ 0, 0, 5, 3, 1 },
		{ 1, 1, 2, 4, 0 }
	};

	bool isSupported = wooden_a_supports_paint_setup(supportType[direction][trackSequence], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

	uint32 imageId;
	sint8 index = paint_minature_railway_eighth_to_diag_index[trackSequence];
	if (index >= 0) {
		imageId = minature_railway_track_pieces_left_eight_to_diag[direction][index] | gTrackColours[SCHEME_TRACK];
		const rct_xy16 offset = minature_railway_track_pieces_left_eight_to_diag_offset[direction][index];
		const rct_xyz16 bounds = minature_railway_track_pieces_left_eight_to_diag_bounds[direction][index];
		sub_98197C(imageId, 0, 0, bounds.x, bounds.y, (sint8)bounds.z, height, offset.x, offset.y, height, get_current_rotation());
	}

	if (direction == 0 && trackSequence == 0) {
		paint_util_push_tunnel_left(height, TUNNEL_6);
	}
	if (direction == 3 && trackSequence == 0) {
		paint_util_push_tunnel_right(height, TUNNEL_6);
	}

	int blockedSegments;
	switch (trackSequence) {
		case 0: blockedSegments = SEGMENTS_ALL; break;
		case 1: blockedSegments = SEGMENTS_ALL; break;
		case 2: blockedSegments = SEGMENTS_ALL; break;
		case 3: blockedSegments = SEGMENTS_ALL; break;
		case 4: blockedSegments = SEGMENTS_ALL; break;
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AE32C */
static void paint_minature_railway_track_right_eighth_to_diag(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	uint32 imageId;
	sint8 index = paint_minature_railway_eighth_to_diag_index[trackSequence];
	if (index >= 0) {
		imageId = minature_railway_track_pieces_right_eight_to_diag[direction][index] | gTrackColours[SCHEME_TRACK];
		const rct_xy16 offset = minature_railway_track_pieces_right_eight_to_diag_offset[direction][index];
		const rct_xy16 bounds = minature_railway_track_pieces_right_eight_to_diag_bounds[direction][index];
		sub_98197C(imageId, 0, 0, bounds.x, bounds.y, 2, height, offset.x, offset.y, height, get_current_rotation());
	}

	switch (trackSequence) {
		case 0:
			metal_a_supports_paint_setup(3, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 4:
			if (direction == 0) metal_a_supports_paint_setup(3, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			if (direction == 1) metal_a_supports_paint_setup(3, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			if (direction == 2) metal_a_supports_paint_setup(3, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			if (direction == 3) metal_a_supports_paint_setup(3, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
	}

	if (direction == 0 && trackSequence == 0) {
		paint_util_push_tunnel_left(height, TUNNEL_0);
	}
	if (direction == 3 && trackSequence == 0) {
		paint_util_push_tunnel_right(height, TUNNEL_0);
	}

	int blockedSegments;
	switch (trackSequence) {
		case 0: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC; break;
		case 1: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC; break;
		case 2: blockedSegments = SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4; break;
		case 3: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_C0 | SEGMENT_D4; break;
		case 4: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_D4; break;
	}

	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AE33C */
static void paint_minature_railway_track_left_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	const uint8 map[] = {4, 2, 3, 1, 0};
	trackSequence = map[trackSequence];
	paint_minature_railway_track_right_eighth_to_diag(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008AE34C */
static void paint_minature_railway_track_right_eighth_to_orthogonal(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	const uint8 map[] = {4, 2, 3, 1, 0};
	trackSequence = map[trackSequence];
	paint_minature_railway_track_left_eighth_to_diag(rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

static void paint_minature_railway_util_diag_supports(uint8 direction, int height)
{
	switch (direction) {
		case 0:
			metal_a_supports_paint_setup(3, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 1:
			metal_a_supports_paint_setup(3, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 2:
			metal_a_supports_paint_setup(3, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
		case 3:
			metal_a_supports_paint_setup(3, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
			break;
	}
}

static const bool minature_railway_diag_image_segment[][4] = {
	{false, true,  false, false},
	{false, false, false, true},
	{false, false, true,  false},
	{true,  false, false, false},
};

static const uint8 minature_railway_diag_support_segment[] = {1, 0, 2, 3};

static const int minature_railway_diag_blocked_segments[] = {
	SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4 | SEGMENT_BC,
	SEGMENT_C4 | SEGMENT_CC | SEGMENT_C8 | SEGMENT_B4,
	SEGMENT_D0 | SEGMENT_C4 | SEGMENT_C0 | SEGMENT_D4,
	SEGMENT_D0 | SEGMENT_C4 | SEGMENT_B8 | SEGMENT_C8
};

/** rct2: 0x008AE2AC */
static void paint_minature_railway_track_diag_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (minature_railway_diag_image_segment[direction][trackSequence]) {
		uint32 imageId = minature_railway_track_pieces_diag_flat[direction] | gTrackColours[SCHEME_TRACK];
		sub_98197C(imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
	}

	if (trackSequence == 3) {
		metal_a_supports_paint_setup(3, minature_railway_diag_support_segment[direction], 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = minature_railway_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AE2DC */
static void paint_minature_railway_track_diag_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (minature_railway_diag_image_segment[direction][trackSequence]) {
		uint32 imageId = minature_railway_track_pieces_diag_25_deg_up[direction] | gTrackColours[SCHEME_TRACK];
		sub_98197C(imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
	}

	if (trackSequence == 3) {
		metal_b_supports_paint_setup(3, minature_railway_diag_support_segment[direction], 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = minature_railway_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AE2BC */
static void paint_minature_railway_track_diag_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (minature_railway_diag_image_segment[direction][trackSequence]) {
		uint32 imageId = minature_railway_track_pieces_diag_flat_to_25_deg_up[direction] | gTrackColours[SCHEME_TRACK];
		sub_98197C(imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
	}

	if (trackSequence == 3) {
		metal_b_supports_paint_setup(3, minature_railway_diag_support_segment[direction], 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = minature_railway_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AE2CC */
static void paint_minature_railway_track_diag_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (minature_railway_diag_image_segment[direction][trackSequence]) {
		uint32 imageId = minature_railway_track_pieces_diag_25_deg_up_to_flat[direction] | gTrackColours[SCHEME_TRACK];
		sub_98197C(imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
	}

	if (trackSequence == 3) {
		metal_b_supports_paint_setup(3, minature_railway_diag_support_segment[direction], 4, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = minature_railway_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AE30C */
static void paint_minature_railway_track_diag_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (minature_railway_diag_image_segment[direction][trackSequence]) {
		uint32 imageId = minature_railway_track_pieces_diag_25_deg_up[(direction + 2) % 4] | gTrackColours[SCHEME_TRACK];
		sub_98197C(imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
	}

	if (trackSequence == 3) {
		metal_b_supports_paint_setup(3, minature_railway_diag_support_segment[direction], 8, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = minature_railway_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AE2EC */
static void paint_minature_railway_track_diag_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (minature_railway_diag_image_segment[direction][trackSequence]) {
		uint32 imageId = minature_railway_track_pieces_diag_25_deg_up_to_flat[(direction + 2) % 4] | gTrackColours[SCHEME_TRACK];
		sub_98197C(imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
	}

	if (trackSequence == 3) {
		metal_b_supports_paint_setup(3, minature_railway_diag_support_segment[direction], 4, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = minature_railway_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AE2FC */
static void paint_minature_railway_track_diag_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
	if (minature_railway_diag_image_segment[direction][trackSequence]) {
		uint32 imageId = minature_railway_track_pieces_diag_flat_to_25_deg_up[(direction + 2) % 4] | gTrackColours[SCHEME_TRACK];
		sub_98197C(imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
	}

	if (trackSequence == 3) {
		metal_b_supports_paint_setup(3, minature_railway_diag_support_segment[direction], 0, height, gTrackColours[SCHEME_SUPPORTS]);
	}

	int blockedSegments = minature_railway_diag_blocked_segments[trackSequence];
	paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
	paint_util_set_general_support_height(height + 48, 0x20);
}

/**
 * rct2: 0x008ACE48
 */
TRACK_PAINT_FUNCTION get_track_paint_function_minature_railway(int trackType, int direction)
{
	switch (trackType) {
		case TRACK_ELEM_FLAT:
			return paint_minature_railway_track_flat;

		case TRACK_ELEM_END_STATION:
		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
			return paint_minature_railway_station;

		case TRACK_ELEM_25_DEG_UP:
			return paint_minature_railway_track_25_deg_up;
		case TRACK_ELEM_FLAT_TO_25_DEG_UP:
			return paint_minature_railway_track_flat_to_25_deg_up;
		case TRACK_ELEM_25_DEG_UP_TO_FLAT:
			return paint_minature_railway_track_25_deg_up_to_flat;

		case TRACK_ELEM_25_DEG_DOWN:
			return paint_minature_railway_track_25_deg_down;
		case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
			return paint_minature_railway_track_flat_to_25_deg_down;
		case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
			return paint_minature_railway_track_25_deg_down_to_flat;

		case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
			return paint_minature_railway_track_left_quarter_turn_5_tiles;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
			return paint_minature_railway_track_right_quarter_turn_5_tiles;

		case TRACK_ELEM_S_BEND_LEFT:
			return paint_minature_railway_track_s_bend_left;
		case TRACK_ELEM_S_BEND_RIGHT:
			return paint_minature_railway_track_s_bend_right;

		case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
			return paint_minature_railway_track_left_quarter_turn_3_tiles;
		case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
			return paint_minature_railway_track_right_quarter_turn_3_tiles;

		case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
			return paint_minature_railway_track_left_eighth_to_diag;
		//case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
		//	return paint_minature_railway_track_right_eighth_to_diag;
		//case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
		//	return paint_minature_railway_track_left_eighth_to_orthogonal;
		//case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
		//	return paint_minature_railway_track_right_eighth_to_orthogonal;

		//case TRACK_ELEM_DIAG_FLAT:
		//	return paint_minature_railway_track_diag_flat;

		//case TRACK_ELEM_DIAG_25_DEG_UP:
		//	return paint_minature_railway_track_diag_25_deg_up;
		//case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
		//	return paint_minature_railway_track_diag_flat_to_25_deg_up;
		//case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
		//	return paint_minature_railway_track_diag_25_deg_up_to_flat;

		//case TRACK_ELEM_DIAG_25_DEG_DOWN:
		//	return paint_minature_railway_track_diag_25_deg_down;
		//case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
		//	return paint_minature_railway_track_diag_flat_to_25_deg_down;
		//case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
		//	return paint_minature_railway_track_diag_25_deg_down_to_flat;
	}

	return NULL;
}
