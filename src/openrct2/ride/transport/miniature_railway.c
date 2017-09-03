#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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
    SPR_MINIATURE_RAILWAY_FLAT_SW_NE = 23341,
    SPR_MINIATURE_RAILWAY_FLAT_NW_SE = 23342,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_0 = 23343,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_1 = 23344,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_2 = 23345,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_3 = 23346,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_4 = 23347,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_0 = 23348,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_1 = 23349,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_2 = 23350,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_3 = 23351,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_4 = 23352,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_0 = 23353,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_1 = 23354,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_2 = 23355,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_3 = 23356,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_4 = 23357,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_0 = 23358,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_1 = 23359,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_2 = 23360,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_3 = 23361,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_4 = 23362,
    SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_0 = 23363,
    SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_1 = 23364,
    SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_2 = 23365,
    SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_3 = 23366,
    SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_0 = 23367,
    SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_1 = 23368,
    SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_2 = 23369,
    SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_3 = 23370,
    SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_0 = 23371,
    SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_1 = 23372,
    SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_2 = 23373,
    SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_3 = 23374,
    SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_0 = 23375,
    SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_1 = 23376,
    SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_2 = 23377,
    SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_3 = 23378,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_SW_NE = 23379,
    SPR_MINIATURE_RAILWAY_FLAT_TO_25_DEG_UP_SW_NE = 23380,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_NE_SW = 23381,
    SPR_MINIATURE_RAILWAY_FLAT_TO_25_DEG_UP_NE_SW = 23382,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_TO_FLAT_NE_SW = 23383,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_TO_FLAT_SW_NE = 23384,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_NW_SE = 23385,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_SE_NW = 23386,
    SPR_MINIATURE_RAILWAY_FLAT_TO_25_DEG_UP_NW_SE = 23387,
    SPR_MINIATURE_RAILWAY_FLAT_TO_25_DEG_UP_SE_NW = 23388,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_TO_FLAT_SE_NW = 23389,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_TO_FLAT_NW_SE = 23390,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_0 = 23391,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_1 = 23392,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_2 = 23393,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_0 = 23394,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_1 = 23395,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_2 = 23396,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_0 = 23397,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_1 = 23398,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_2 = 23399,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_0 = 23400,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_1 = 23401,
    SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_2 = 23402,
    SPR_MINIATURE_RAILWAY_FLAT_NO_BASE_SW_NE = 23403,
    SPR_MINIATURE_RAILWAY_FLAT_NO_BASE_NW_SE = 23404,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_0 = 23405,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_1 = 23406,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_2 = 23407,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_3 = 23408,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_0 = 23409,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_1 = 23410,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_2 = 23411,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_3 = 23412,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_0 = 23413,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_1 = 23414,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_2 = 23415,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_3 = 23416,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_0 = 23417,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_1 = 23418,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_2 = 23419,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_3 = 23420,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_0 = 23421,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_1 = 23422,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_2 = 23423,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_3 = 23424,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_0 = 23425,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_1 = 23426,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_2 = 23427,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_3 = 23428,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_0 = 23429,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_1 = 23430,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_2 = 23431,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_3 = 23432,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_0 = 23433,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_1 = 23434,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_2 = 23435,
    SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_3 = 23436,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_W_E = 23437,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_N_S = 23438,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_E_W = 23439,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_S_N = 23440,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_W_E = 23441,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_N_S = 23442,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_E_W = 23443,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_S_N = 23444,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_W_E = 23445,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_N_S = 23446,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_E_W = 23447,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_S_N = 23448,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_W_E = 23449,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_N_S = 23450,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_E_W = 23451,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_S_N = 23452,
};

static const uint32 miniature_railway_track_floor[4] = {
    SPR_FLOOR_PLANKS,
    SPR_FLOOR_PLANKS_90_DEG,
    SPR_FLOOR_PLANKS,
    SPR_FLOOR_PLANKS_90_DEG
};

static const uint32 miniature_railway_track_pieces_flat[4] = {
    SPR_MINIATURE_RAILWAY_FLAT_SW_NE,
    SPR_MINIATURE_RAILWAY_FLAT_NW_SE,
    SPR_MINIATURE_RAILWAY_FLAT_SW_NE,
    SPR_MINIATURE_RAILWAY_FLAT_NW_SE,
};

static const uint32 miniature_railway_station_floor[4] = {
    SPR_STATION_BASE_A_SW_NE,
    SPR_STATION_BASE_A_NW_SE,
    SPR_STATION_BASE_A_SW_NE,
    SPR_STATION_BASE_A_NW_SE
};

static const uint32 miniature_railway_track_pieces_flat_station[4] = {
    SPR_MINIATURE_RAILWAY_FLAT_NO_BASE_SW_NE,
    SPR_MINIATURE_RAILWAY_FLAT_NO_BASE_NW_SE,
    SPR_MINIATURE_RAILWAY_FLAT_NO_BASE_SW_NE,
    SPR_MINIATURE_RAILWAY_FLAT_NO_BASE_NW_SE,
};

static const uint32 miniature_railway_track_pieces_flat_quarter_turn_5_tiles[4][5] = {
    {
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_0,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_1,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_2,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_3,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SW_SE_PART_4,
    },
    {
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_0,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_1,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_2,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_3,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NW_SW_PART_4,
    },
    {
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_0,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_1,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_2,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_3,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_NE_NW_PART_4,
    },
    {
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_0,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_1,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_2,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_3,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_5_TILES_SE_NE_PART_4,
    }
};

static const uint32 miniature_railway_track_pieces_25_deg_up[4] = {
    SPR_MINIATURE_RAILWAY_25_DEG_UP_SW_NE,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_NW_SE,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_NE_SW,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_SE_NW,
};

static const uint32 miniature_railway_track_pieces_flat_to_25_deg_up[4] = {
    SPR_MINIATURE_RAILWAY_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_MINIATURE_RAILWAY_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_MINIATURE_RAILWAY_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_MINIATURE_RAILWAY_FLAT_TO_25_DEG_UP_SE_NW,
};

static const uint32 miniature_railway_track_pieces_25_deg_up_to_flat[4] = {
    SPR_MINIATURE_RAILWAY_25_DEG_UP_TO_FLAT_SW_NE,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_TO_FLAT_NW_SE,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_TO_FLAT_NE_SW,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_TO_FLAT_SE_NW,
};

static const uint32 miniature_railway_track_pieces_s_bend_left[2][4] = {
    {
        SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_0,
        SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_1,
        SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_2,
        SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SW_NE_PART_3,
    },
    {
        SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_3,
        SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_2,
        SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_1,
        SPR_MINIATURE_RAILWAY_S_BEND_LEFT_SE_NW_PART_0,
    }
};

static const uint32 miniature_railway_track_pieces_s_bend_right[2][4] = {
    {
        SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_0,
        SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_1,
        SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_2,
        SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SW_NE_PART_3,
    },
    {
        SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_3,
        SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_2,
        SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_1,
        SPR_MINIATURE_RAILWAY_S_BEND_RIGHT_SE_NW_PART_0,
    }
};

static const uint32 miniature_railway_track_pieces_flat_quarter_turn_3_tiles[4][3] = {
    {
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_2
    },
    {
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_2
    },
    {
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_2
    },
    {
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_2
    }
};

static const uint32 miniature_railway_track_pieces_right_eight_to_diag[4][4] = {
    {
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_0,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_1,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_2,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_E_PART_3,
    },
    {
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_0,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_1,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_2,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_S_PART_3,
    },
    {
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_0,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_1,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_2,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_W_PART_3,
    },
    {
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_0,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_1,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_2,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_N_PART_3,
    },
};

static const rct_xyz16 miniature_railway_track_pieces_right_eight_to_diag_bounds[4][4] = {
    {
        {32, 32, 2},
        {32, 16, 2},
        {16, 16, 2},
        {32, 32, 0},
    },
    {
        {32, 32, 2},
        {16, 32, 2},
        {16, 16, 2},
        {16, 16, 2},
    },
    {
        {32, 32, 2},
        {34, 16, 2},
        {28, 28, 2},
        {32, 34, 0},
    },
    {
        {32, 32, 2},
        {16, 32, 2},
        {16, 16, 2},
        {30, 30, 0},
    },
};

static const rct_xy16 miniature_railway_track_pieces_right_eight_to_diag_offset[4][4] = {
    {
        {0, 0},
        {0,  16},
        {0,  0},
        {0, 0},
    },
    {
        {0, 0},
        {16, 0},
        {0,  16},
        {0,  0},
    },
    {
        {0, 0},
        {0,  0},
        {4,  4},
        {0,  0},
    },
    {
        {0, 0},
        {0,  0},
        {16, 0},
        {0, 0},
    },
};

static const uint32 miniature_railway_track_pieces_left_eight_to_diag[4][4] = {
    {
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_0,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_1,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_2,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SW_N_PART_3,
    },
    {
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_0,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_1,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_2,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NW_E_PART_3,
    },
    {
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_0,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_1,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_2,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_NE_S_PART_3,
    },
    {
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_0,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_1,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_2,
        SPR_MINIATURE_RAILWAY_EIGHT_TO_DIAG_SE_W_PART_3,
    },
};

static const rct_xyz16 miniature_railway_track_pieces_left_eight_to_diag_bounds[4][4] = {
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

static const rct_xy16 miniature_railway_track_pieces_left_eight_to_diag_offset[4][4] = {
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

static const rct_xyz16 miniature_railway_track_pieces_right_eight_to_orthog_bounds[4][4] = {
    {
        {32, 32, 2},
        {32, 16, 2},
        {16, 16, 2},
        {14, 14, 2},
    },
    {
        {32, 32, 2},
        {16, 34, 2},
        {14, 14, 2},
        {18, 16, 2},
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
        {16, 16, 2},
    },
};

static const rct_xy16 miniature_railway_track_pieces_right_eight_to_orthog_offset[4][4] = {
    {
        {0, 0},
        {0,  0},
        {0,  16},
        {16, 16},
    },
    {
        {0, 0},
        {0,  0},
        {16, 16},
        {16, 0},
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
        {0,  16},
    },
};

static const rct_xyz16 miniature_railway_track_pieces_left_eight_to_orthog_bounds[4][4] = {
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
        {16, 16, 2},
    },
    {
        {32, 32, 2},
        {34, 16, 2},
        {28, 28, 2},
        {16, 18, 2},
    },
    {
        {32, 32, 2},
        {16, 32, 2},
        {16, 16, 2},
        {14, 14, 2},
    },
};

static const rct_xy16 miniature_railway_track_pieces_left_eight_to_orthog_offset[4][4] = {
    {
        {0, 0},
        {0,  16},
        {0,  0},
        {16, 0},
    },
    {
        {0, 0},
        {16, 0},
        {0,  16},
        {0,  0},
    },
    {
        {0, 0},
        {0,  0},
        {4,  4},
        {0,  16},
    },
    {
        {0, 0},
        {0,  0},
        {16, 0},
        {16, 16},
    },
};

static const uint32 miniature_railway_track_pieces_diag_flat[4] = {
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_W_E,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_N_S,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_E_W,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_S_N,
};

static const uint32 miniature_railway_track_pieces_diag_flat_to_25_deg_up[4] = {
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_W_E,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_N_S,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_E_W,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_S_N,
};

static const uint32 miniature_railway_track_pieces_diag_25_deg_up_to_flat[4] = {
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_W_E,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_N_S,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_E_W,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_S_N,
};

static const uint32 miniature_railway_track_pieces_diag_25_deg_up[4] = {
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_W_E,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_N_S,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_E_W,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_S_N,
};

/** rct2: 0x008AD0C0 */
static void paint_miniature_railway_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{

    bool isSupported = wooden_a_supports_paint_setup(session, direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    uint32 imageId;

    if (isSupported) {
        imageId = miniature_railway_track_floor[direction] | gTrackColours[SCHEME_SUPPORTS];
        sub_98197C_rotated(direction, imageId, 0, 0, 32, 20, 2, height, 0, 6, height);

        imageId = miniature_railway_track_pieces_flat[direction] | gTrackColours[SCHEME_TRACK];
        sub_98199C_rotated(direction, imageId, 0, 6, 32, 20, 2, height, 0, 6, height);
    }
    else {
        imageId = miniature_railway_track_pieces_flat[direction] | gTrackColours[SCHEME_TRACK];
        sub_98197C_rotated(direction, imageId, 0, 6, 32, 20, 2, height, 0, 6, height);
    }

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AD170, 0x008AD180, 0x008AD190 */
static void paint_miniature_railway_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    wooden_a_supports_paint_setup(session, direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    imageId = miniature_railway_station_floor[direction] | gTrackColours[SCHEME_MISC];
    sub_98197C_rotated(direction, imageId, 0, 0, 32, 28, 2, height - 2, 0, 2, height);

    imageId = miniature_railway_track_pieces_flat_station[direction] | gTrackColours[SCHEME_TRACK];
    sub_98199C_rotated(direction, imageId, 0, 6, 32, 20, 2, height, 0, 0, height);

    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);

    track_paint_util_draw_station_3(rideIndex, trackSequence, direction, height + 2, height, mapElement);
    // covers shouldn't be offset by +2

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 30, 0x20);
}

/** rct2: 0x008AD0D0 */
static void paint_miniature_railway_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{

    uint32 imageId = miniature_railway_track_pieces_25_deg_up[direction] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 2, 32, 25, 2, height, 0, 3, height);

    switch (direction) {
        case 0: paint_util_push_tunnel_left(height - 8, TUNNEL_7); break;
        case 1: paint_util_push_tunnel_right(height + 8, TUNNEL_8); break;
        case 2: paint_util_push_tunnel_left(height + 8, TUNNEL_8); break;
        case 3: paint_util_push_tunnel_right(height - 8, TUNNEL_7); break;
    }

    wooden_a_supports_paint_setup(session, direction & 1, 45 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AD0E0 */
static void paint_miniature_railway_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{

    uint32 imageId = miniature_railway_track_pieces_flat_to_25_deg_up[direction] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 2, 32, 25, 2, height, 0, 3, height);

    switch (direction) {
        case 0: paint_util_push_tunnel_left(height, TUNNEL_6); break;
        case 1: paint_util_push_tunnel_right(height, TUNNEL_8); break;
        case 2: paint_util_push_tunnel_left(height, TUNNEL_8); break;
        case 3: paint_util_push_tunnel_right(height, TUNNEL_6); break;
    }

    wooden_a_supports_paint_setup(session, direction & 1, 37 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AD0F0 */
static void paint_miniature_railway_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{

    uint32 imageId = miniature_railway_track_pieces_25_deg_up_to_flat[direction] | gTrackColours[SCHEME_TRACK];

    sub_98197C_rotated(direction, imageId, 0, 2, 32, 25, 2, height, 0, 3, height);

    switch (direction) {
        case 0: paint_util_push_tunnel_left(height - 8, TUNNEL_6); break;
        case 1: paint_util_push_tunnel_right(height + 8, TUNNEL_14); break;
        case 2: paint_util_push_tunnel_left(height + 8, TUNNEL_14); break;
        case 3: paint_util_push_tunnel_right(height - 8, TUNNEL_6); break;
    }

    wooden_a_supports_paint_setup(session, direction & 1, 41 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008AD100 */
static void paint_miniature_railway_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_miniature_railway_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008AD110 */
static void paint_miniature_railway_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_miniature_railway_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008AD120 */
static void paint_miniature_railway_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_miniature_railway_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

const rct_xy16 miniature_railway_right_quarter_turn_5_tiles_offsets[4][5] = {
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

const rct_xyz16 miniature_railway_right_quarter_turn_5_tiles_bound_offsets[4][5] = {
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

const rct_xy16 miniature_railway_right_quarter_turn_5_tiles_bound_lengths[4][5] = {
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

static const uint32 miniature_railway_right_quarter_turn_5_tiles_track_floor[4][5] = {
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

static const sint8 miniature_railway_right_quarter_turn_5_tiles_sprite_map[] = {0, -1, 1, 2, -1, 3, 4};

/** rct2: 0x008AD140 */
static void paint_miniature_railway_track_right_quarter_turn_5_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{

    if (right_quarter_turn_5_supports_type[direction][trackSequence] != -1){
        bool isSupported = wooden_a_supports_paint_setup(session, right_quarter_turn_5_supports_type[direction][trackSequence], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

        if (isSupported == false || (trackSequence == 3 && direction == 2)) {
            track_paint_util_right_quarter_turn_5_tiles_paint(2, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], miniature_railway_track_pieces_flat_quarter_turn_5_tiles, miniature_railway_right_quarter_turn_5_tiles_offsets, miniature_railway_right_quarter_turn_5_tiles_bound_lengths, NULL, get_current_rotation());
        }
        else {
            track_paint_util_right_quarter_turn_5_tiles_paint(2, height, direction, trackSequence, gTrackColours[SCHEME_SUPPORTS], miniature_railway_right_quarter_turn_5_tiles_track_floor, NULL, miniature_railway_right_quarter_turn_5_tiles_bound_lengths, miniature_railway_right_quarter_turn_5_tiles_bound_offsets, get_current_rotation());

            sint32 index = miniature_railway_right_quarter_turn_5_tiles_sprite_map[trackSequence];
            uint32 imageId = miniature_railway_track_pieces_flat_quarter_turn_5_tiles[direction][index] | gTrackColours[SCHEME_TRACK];
            rct_xy16 offset = miniature_railway_right_quarter_turn_5_tiles_offsets[direction][index];
            rct_xy16 boundsLength = miniature_railway_right_quarter_turn_5_tiles_bound_lengths[direction][index];
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

    sint32 blockedSegments = 0;
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

/** rct2: 0x008AD130 */
static void paint_miniature_railway_track_left_quarter_turn_5_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    paint_miniature_railway_track_right_quarter_turn_5_tiles(session, rideIndex, trackSequence, (direction + 1) % 4, height, mapElement);
}

static const sint8 s_bend_left_supports_type[4][4] = {
    {0, 5, 3, 0},
    {1, 2, 4, 1},
    {0, 5, 3, 0},
    {1, 2, 4, 1}
};

static const uint32 miniature_railway_s_bend_left_tiles_track_floor[2][4] = {
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
static void paint_miniature_railway_track_s_bend_left(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (direction == 2 || direction == 3) {
        trackSequence = 3 - trackSequence;
    }

    bool isSupported = wooden_a_supports_paint_setup(session, s_bend_left_supports_type[direction][trackSequence], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

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

    uint32 imageId = miniature_railway_track_pieces_s_bend_left[direction & 1][trackSequence] | gTrackColours[SCHEME_TRACK];
    rct_xy16 offset = offsetList[trackSequence];
    rct_xy16 bounds = boundsList[trackSequence];

    if (isSupported == false) {
        sub_98197C_rotated(direction, imageId, (sint8)offset.x, (sint8)offset.y, bounds.x, bounds.y, 2, height, offset.x, offset.y, height);
    }
    else {
        imageId = miniature_railway_s_bend_left_tiles_track_floor[direction & 1][trackSequence] | gTrackColours[SCHEME_SUPPORTS];
        sub_98197C_rotated(direction, imageId, 0, 0, bounds.x, bounds.y, 2, height, offset.x, offset.y, height);

        imageId = miniature_railway_track_pieces_s_bend_left[direction & 1][trackSequence] | gTrackColours[SCHEME_TRACK];
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

    sint32 blockedSegments = 0;
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

static const uint32 miniature_railway_s_bend_right_tiles_track_floor[2][4] = {
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
static void paint_miniature_railway_track_s_bend_right(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (direction == 2 || direction == 3) {
        trackSequence = 3 - trackSequence;
    }

    bool isSupported = wooden_a_supports_paint_setup(session, s_bend_right_supports_type[direction][trackSequence], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

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

    uint32 imageId = miniature_railway_track_pieces_s_bend_right[direction&1][trackSequence] | gTrackColours[SCHEME_TRACK];
    rct_xy16 offset = offsetList[trackSequence];
    rct_xy16 bounds = boundsList[trackSequence];
    if (isSupported == false) {
        sub_98197C_rotated(direction, imageId, (sint8)offset.x, (sint8)offset.y, bounds.x, bounds.y, 2, height, offset.x, offset.y, height);
    }
    else {
        imageId = miniature_railway_s_bend_right_tiles_track_floor[direction & 1][trackSequence] | gTrackColours[SCHEME_SUPPORTS];
        sub_98197C_rotated(direction, imageId, 0, 0, bounds.x, bounds.y, 2, height, offset.x, offset.y, height);

        imageId = miniature_railway_track_pieces_s_bend_right[direction & 1][trackSequence] | gTrackColours[SCHEME_TRACK];
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

    sint32 blockedSegments = 0;
    switch (trackSequence) {
        case 0: blockedSegments = SEGMENTS_ALL; break;
        case 1: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC; break;
        case 2: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4; break;
        case 3: blockedSegments = SEGMENTS_ALL; break;
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction & 1), 0xFFFF, 0);

    paint_util_set_general_support_height(height + 32, 0x20);
}

static const uint32 miniature_railway_right_quarter_turn_3_tile_track_floor[4][3] = {
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

const rct_xyz16 miniature_railway_right_quarter_turn_3_tile_bound_offsets[4][3] = {
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
static void paint_miniature_railway_track_right_quarter_turn_3_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool isSupported = false;
    if (trackSequence != 1 && trackSequence != 2)
    {
        const uint8 supportType[] = { 4, 5, 2, 3 };
        isSupported = wooden_a_supports_paint_setup(session, supportType[direction], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    }
    if (isSupported == false) {
        track_paint_util_right_quarter_turn_3_tiles_paint(3, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], miniature_railway_track_pieces_flat_quarter_turn_3_tiles, defaultRightQuarterTurn3TilesOffsets, defaultRightQuarterTurn3TilesBoundLengths, NULL, get_current_rotation());
    }
    else {
        track_paint_util_right_quarter_turn_3_tiles_paint(3, height, direction, trackSequence, gTrackColours[SCHEME_SUPPORTS], miniature_railway_right_quarter_turn_3_tile_track_floor, NULL, defaultRightQuarterTurn3TilesBoundLengths, miniature_railway_right_quarter_turn_3_tile_bound_offsets, get_current_rotation());

        static const sint8 right_quarter_turn_3_tiles_sprite_map[] = {0, -1, 1, 2};

        sint32 index = right_quarter_turn_3_tiles_sprite_map[trackSequence];

        uint32 imageId = miniature_railway_track_pieces_flat_quarter_turn_3_tiles[direction][index] | gTrackColours[SCHEME_TRACK];
        rct_xy16 offset = defaultRightQuarterTurn3TilesOffsets[direction][index];
        rct_xy16 boundsLength = defaultRightQuarterTurn3TilesBoundLengths[direction][index];
        rct_xyz16 boundsOffset = { .x = offset.x, .y = offset.y, .z = 0 };

        sub_98199C(imageId, (sint8) offset.x, (sint8) offset.y, boundsLength.x, boundsLength.y, 3, height, boundsOffset.x, boundsOffset.y, height + boundsOffset.z, get_current_rotation());
    }
    track_paint_util_right_quarter_turn_3_tiles_tunnel(height, direction, trackSequence, TUNNEL_6);

    sint32 blockedSegments = 0;
    switch (trackSequence) {
        case 0: blockedSegments = SEGMENTS_ALL; break;
        case 2: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0; break;
        case 3: blockedSegments = SEGMENTS_ALL; break;
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);

    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AD1A0 */
static void paint_miniature_railway_track_left_quarter_turn_3_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    paint_miniature_railway_track_right_quarter_turn_3_tiles(session, rideIndex, trackSequence, (direction + 1) % 4, height, mapElement);
}

static const sint8 paint_miniature_railway_eighth_to_diag_index[] = {0, 1, 2, -1, 3};

static const uint32 miniature_railway_floor_track_pieces_left_eight_to_diag[4][5] = {
    {
        SPR_FLOOR_PLANKS,
        SPR_FLOOR_PLANKS,
        SPR_FLOOR_PLANKS_E_SEGMENT,
        SPR_FLOOR_PLANKS_W_SEGMENT,
        SPR_FLOOR_PLANKS_90_DEG
    },
    {
        SPR_FLOOR_PLANKS_90_DEG,
        SPR_FLOOR_PLANKS_90_DEG,
        SPR_FLOOR_PLANKS_S_SEGMENT,
        SPR_FLOOR_PLANKS_N_SEGMENT,
        SPR_FLOOR_PLANKS
    },
    {
        SPR_FLOOR_PLANKS,
        SPR_FLOOR_PLANKS,
        SPR_FLOOR_PLANKS_W_SEGMENT,
        SPR_FLOOR_PLANKS_E_SEGMENT,
        SPR_FLOOR_PLANKS_90_DEG
    },
    {
        SPR_FLOOR_PLANKS_90_DEG,
        SPR_FLOOR_PLANKS_90_DEG,
        SPR_FLOOR_PLANKS_N_SEGMENT,
        SPR_FLOOR_PLANKS_S_SEGMENT,
        SPR_FLOOR_PLANKS
    },
};

static const rct_xyz16 miniature_railway_track_floor_pieces_left_eight_to_diag_bounds[4][5] = {
    {
        {32, 32, 2},
        {32, 16, 2},
        {16, 16, 2},
        {16, 16, 0},
        {30, 30, 0},
    },
    {
        {32, 32, 2},
        {16, 34, 2},
        {14, 14, 2},
        {16, 16, 0},
        {34, 32, 0},
    },
    {
        {32, 32, 2},
        {32, 16, 2},
        {16, 16, 2},
        {16, 16, 0},
        {16, 16, 2},
    },
    {
        {32, 32, 2},
        {16, 32, 2},
        {16, 16, 2},
        {16, 16, 0},
        {32, 32, 0},
    },
};

static const rct_xy16 miniature_railway_track_floor_pieces_left_eight_to_diag_offset[4][5] = {
    {
        {0, 0},
        {0,  0},
        {0,  16},
        {16, 0},
        {0, 0},
    },
    {
        {0, 0},
        {0,  0},
        {16, 16},
        {0,  0},
        {0, 0},
    },
    {
        {0, 0},
        {0,  16},
        {16, 0},
        {0,  16},
        {0,  0},
    },
    {
        {0, 0},
        {16, 0},
        {0,  0},
        {16, 16},
        {0,  0},
    },
};

/** rct2: 0x008AD1C0 */
static void paint_miniature_railway_track_left_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    const uint8 supportType[4][5] = {
        { 0, 0, 3, 5, 1 },
        { 1, 1, 4, 2, 0 },
        { 0, 0, 5, 3, 1 },
        { 1, 1, 2, 4, 0 }
    };

    bool isSupported = false;
    bool isRightEighthToOrthog = mapElement->properties.track.type == TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL;
    // Right eighth to orthogonal calls this function but we do not want to have a support call for it
    // for track sequence 4
    if (trackSequence != 4 || !isRightEighthToOrthog) {
        isSupported = wooden_a_supports_paint_setup(session, supportType[direction][trackSequence], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    }
    uint32 imageId;
    if (isSupported == false) {
        sint8 index = paint_miniature_railway_eighth_to_diag_index[trackSequence];
        if (index >= 0) {
            imageId = miniature_railway_track_pieces_left_eight_to_diag[direction][index] | gTrackColours[SCHEME_TRACK];
            rct_xy16 offset = miniature_railway_track_pieces_left_eight_to_diag_offset[direction][index];
            rct_xyz16 bounds = miniature_railway_track_pieces_left_eight_to_diag_bounds[direction][index];
            if (isRightEighthToOrthog) {
                bounds = miniature_railway_track_pieces_right_eight_to_orthog_bounds[direction][index];
                offset =  miniature_railway_track_pieces_right_eight_to_orthog_offset[direction][index];
            }
            sub_98197C(imageId, 0, 0, bounds.x, bounds.y, (sint8)bounds.z, height, offset.x, offset.y, height, get_current_rotation());
        }
    }
    else {
        imageId = miniature_railway_floor_track_pieces_left_eight_to_diag[direction][trackSequence] | gTrackColours[SCHEME_SUPPORTS];
        rct_xy16 offset = miniature_railway_track_floor_pieces_left_eight_to_diag_offset[direction][trackSequence];
        rct_xyz16 bounds = miniature_railway_track_floor_pieces_left_eight_to_diag_bounds[direction][trackSequence];
        sub_98197C(imageId, 0, 0, bounds.x, bounds.y, (sint8)bounds.z, height, offset.x, offset.y, height, get_current_rotation());

        sint8 index = paint_miniature_railway_eighth_to_diag_index[trackSequence];
        if (index >= 0) {
            imageId = miniature_railway_track_pieces_left_eight_to_diag[direction][index] | gTrackColours[SCHEME_TRACK];
            offset = miniature_railway_track_pieces_left_eight_to_diag_offset[direction][index];
            bounds = miniature_railway_track_pieces_left_eight_to_diag_bounds[direction][index];
            sub_98199C(imageId, 0, 0, bounds.x, bounds.y, (sint8)bounds.z, height, offset.x, offset.y, height, get_current_rotation());
        }
    }

    if (direction == 0 && trackSequence == 0) {
        paint_util_push_tunnel_left(height, TUNNEL_6);
    }
    if (direction == 3 && trackSequence == 0) {
        paint_util_push_tunnel_right(height, TUNNEL_6);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static const uint32 miniature_railway_floor_track_pieces_right_eight_to_diag[4][5] = {
    {
        SPR_FLOOR_PLANKS_90_DEG,
        SPR_FLOOR_PLANKS_90_DEG,
        SPR_FLOOR_PLANKS_N_SEGMENT,
        SPR_FLOOR_PLANKS_S_SEGMENT,
        SPR_FLOOR_PLANKS_90_DEG
    },
    {
        SPR_FLOOR_PLANKS_90_DEG,
        SPR_FLOOR_PLANKS_90_DEG,
        SPR_FLOOR_PLANKS_E_SEGMENT,
        SPR_FLOOR_PLANKS_W_SEGMENT,
        SPR_FLOOR_PLANKS
    },
    {
        SPR_FLOOR_PLANKS,
        SPR_FLOOR_PLANKS,
        SPR_FLOOR_PLANKS_S_SEGMENT,
        SPR_FLOOR_PLANKS_N_SEGMENT,
        SPR_FLOOR_PLANKS_90_DEG
    },
    {
        SPR_FLOOR_PLANKS_90_DEG,
        SPR_FLOOR_PLANKS_90_DEG,
        SPR_FLOOR_PLANKS_W_SEGMENT,
        SPR_FLOOR_PLANKS_E_SEGMENT,
        SPR_FLOOR_PLANKS
    },
};

static const rct_xyz16 miniature_railway_track_floor_pieces_right_eight_to_diag_bounds[4][5] = {
    {
        {32, 32, 2},
        {32, 16, 2},
        {16, 16, 2},
        {16, 16, 0},
        {32, 32, 0},
    },
    {
        {32, 32, 2},
        {16, 32, 2},
        {16, 16, 2},
        {16, 16, 0},
        {16, 16, 2},
    },
    {
        {32, 32, 2},
        {34, 16, 2},
        {28, 28, 2},
        {16, 16, 0},
        {32, 34, 0},
    },
    {
        {32, 32, 2},
        {16, 32, 2},
        {16, 16, 2},
        {16, 16, 0},
        {30, 30, 0},
    },
};

static const rct_xy16 miniature_railway_track_floor_pieces_right_eight_to_diag_offset[4][5] = {
    {
        {0, 0},
        {0,  16},
        {0, 0},
        {16,  16},
        {0,  0},
    },
    {
        {0, 0},
        {16, 0},
        {0,  16},
        {16, 0},
        {0,  0},
    },
    {
        {0, 0},
        {0,  0},
        {4,  4},
        {0, 0},
        {0, 0},
    },
    {
        {0, 0},
        {0,  0},
        {16, 0},
        {0,  16},
        {0, 0},
    },
};

/** rct2: 0x008AD1D0 */
static void paint_miniature_railway_track_right_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    const uint8 supportType[4][5] = {
        { 0, 0, 2, 4, 1 },
        { 1, 1, 3, 5, 0 },
        { 0, 0, 4, 2, 1 },
        { 1, 1, 5, 3, 0 }
    };

    bool isSupported = false;
    bool isLeftEighthToOrthog = mapElement->properties.track.type == TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL;
    // Right eighth to orthogonal calls this function but we do not want to have a support call for it
    // for track sequence 4
    if (trackSequence != 4 || !isLeftEighthToOrthog) {
        isSupported = wooden_a_supports_paint_setup(session, supportType[direction][trackSequence], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    }

    uint32 imageId;
    if (isSupported == false) {
        sint8 index = paint_miniature_railway_eighth_to_diag_index[trackSequence];
        if (index >= 0) {
            imageId = miniature_railway_track_pieces_right_eight_to_diag[direction][index] | gTrackColours[SCHEME_TRACK];
            rct_xy16 offset = miniature_railway_track_pieces_right_eight_to_diag_offset[direction][index];
            rct_xyz16 bounds = miniature_railway_track_pieces_right_eight_to_diag_bounds[direction][index];
            if (isLeftEighthToOrthog) {
                bounds = miniature_railway_track_pieces_left_eight_to_orthog_bounds[direction][index];
                offset =  miniature_railway_track_pieces_left_eight_to_orthog_offset[direction][index];
            }
            sub_98197C(imageId, 0, 0, bounds.x, bounds.y, (sint8)bounds.z, height, offset.x, offset.y, height, get_current_rotation());
        }
    }
    else {
        imageId = miniature_railway_floor_track_pieces_right_eight_to_diag[direction][trackSequence] | gTrackColours[SCHEME_SUPPORTS];
        rct_xy16 offset = miniature_railway_track_floor_pieces_right_eight_to_diag_offset[direction][trackSequence];
        rct_xyz16 bounds = miniature_railway_track_floor_pieces_right_eight_to_diag_bounds[direction][trackSequence];
        sub_98197C(imageId, 0, 0, bounds.x, bounds.y, (sint8)bounds.z, height, offset.x, offset.y, height, get_current_rotation());

        sint8 index = paint_miniature_railway_eighth_to_diag_index[trackSequence];
        if (index >= 0) {
            imageId = miniature_railway_track_pieces_right_eight_to_diag[direction][index] | gTrackColours[SCHEME_TRACK];
            offset = miniature_railway_track_pieces_right_eight_to_diag_offset[direction][index];
            bounds = miniature_railway_track_pieces_right_eight_to_diag_bounds[direction][index];
            sub_98199C(imageId, 0, 0, bounds.x, bounds.y, (sint8)bounds.z, height, offset.x, offset.y, height, get_current_rotation());
        }
    }

    if (direction == 0 && trackSequence == 0) {
        paint_util_push_tunnel_left(height, TUNNEL_6);
    }
    if (direction == 3 && trackSequence == 0) {
        paint_util_push_tunnel_right(height, TUNNEL_6);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AD1E0 */
static void paint_miniature_railway_track_left_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    paint_miniature_railway_track_right_eighth_to_diag(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008AD1F0 */
static void paint_miniature_railway_track_right_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    paint_miniature_railway_track_left_eighth_to_diag(session, rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

static const bool miniature_railway_diag_image_segment[][4] = {
    {false, true,  false, false},
    {false, false, false, true},
    {false, false, true,  false},
    {true,  false, false, false},
};

enum {
    SUPPORT_PLAIN = 0,
    SUPPORT_PLAIN_90_DEG = 1,
    SUPPORT_PLAIN_N = 2,
    SUPPORT_PLAIN_E = 3,
    SUPPORT_PLAIN_S = 4,
    SUPPORT_PLAIN_W = 5,
};

static const sint16 monorail_diag_support_types[][4] = {
    {-1, SUPPORT_PLAIN_N, SUPPORT_PLAIN_S, SUPPORT_PLAIN},
    {-1, SUPPORT_PLAIN_E, SUPPORT_PLAIN_W, SUPPORT_PLAIN_90_DEG},
    {-1, SUPPORT_PLAIN_S, SUPPORT_PLAIN_N, SUPPORT_PLAIN},
    {-1, SUPPORT_PLAIN_W, SUPPORT_PLAIN_E, SUPPORT_PLAIN_90_DEG},
};

typedef struct floor_desc {
    uint32 image_id;
    rct_xy16 bound_size;
    rct_xy16 bound_offset;
} floor_desc;

static const floor_desc floors[] = {
    {SPR_FLOOR_PLANKS,           {32, 32}, {0,   0}},
    {SPR_FLOOR_PLANKS_90_DEG,    {32, 32}, {0,   0}}, // -16,-16(1:3)
    {SPR_FLOOR_PLANKS_N_SEGMENT, {32, 32}, {-16, -16}},
    {SPR_FLOOR_PLANKS_E_SEGMENT, {16, 16}, {0,   16}},
    {SPR_FLOOR_PLANKS_S_SEGMENT, {16, 16}, {16,  16}},
    {SPR_FLOOR_PLANKS_W_SEGMENT, {16, 16}, {16,  0}},
};

/** rct2: 0x008AD200 */
static void miniature_railway_track_diag_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    bool isSupported = false;
    sint16 supportType = monorail_diag_support_types[direction][trackSequence];

    uint32 floorImage = 0;
    rct_xy16 floorBoundSize = { 0 };
    rct_xy16 floorBoundOffset = { 0 };

    if (supportType != -1) {
        floorImage = floors[supportType].image_id;
        floorBoundSize = floors[supportType].bound_size;
        floorBoundOffset = floors[supportType].bound_offset;
        isSupported = wooden_a_supports_paint_setup(session, supportType, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    }

    if (direction == 1 && trackSequence == 3) {
        floorBoundOffset = (rct_xy16) {-16, -16};
    }

    uint32 imageId = miniature_railway_track_pieces_diag_flat[direction];
    bool drawRail = miniature_railway_diag_image_segment[direction][trackSequence];

    if (isSupported) {
        sub_98197C(
            floorImage | gTrackColours[SCHEME_SUPPORTS],
            0, 0,
            floorBoundSize.x, floorBoundSize.y, (drawRail ? 2 : 0),
            height,
            floorBoundOffset.x, floorBoundOffset.y, height,
            get_current_rotation()
        );
        if (drawRail) {
            sub_98199C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
        }
    } else if (drawRail) {
        sub_98197C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

enum {
    WOOD_A = 0,
    WOOD_B = 1,
};

static bool wooden_supports_paint_setup(paint_session * session, sint32 woodType, sint32 supportType, sint32 special, sint32 height, uint32 imageColourFlags, bool *underground) {
    switch (woodType) {
        case WOOD_A:
            return wooden_a_supports_paint_setup(session, supportType, special, height, imageColourFlags, underground);

        case WOOD_B:
            return wooden_b_supports_paint_setup(session, supportType, special, height, imageColourFlags, underground);
    }

    return false;
}

/** rct2: 0x008AD230 */
static void miniature_railway_track_diag_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    // TODO: The supports are inconsistent for different rotations

    sint8 supportFunction = WOOD_B;
    sint16 supportType = monorail_diag_support_types[direction][trackSequence];
    if (supportType == SUPPORT_PLAIN || supportType == SUPPORT_PLAIN_90_DEG) {
        supportFunction = WOOD_A;
    }

    sint8 heightDiffs[] = {+8, -8, +8, -8};
    sint8 heightDiff = heightDiffs[direction];
    if (trackSequence == 3) {
        heightDiff = 8;
    }

    bool hasSupports = false;

    uint32 floorImage = 0;
    rct_xy16 floorBoundSize = { 0 };
    rct_xy16 floorBoundOffset = { 0 };

    if (supportType != -1) {
        floorImage = floors[supportType].image_id;
        floorBoundSize = floors[supportType].bound_size;
        floorBoundOffset = floors[supportType].bound_offset;
        hasSupports = wooden_supports_paint_setup(session, supportFunction, supportType, 0, height + heightDiff, gTrackColours[SCHEME_SUPPORTS], NULL);
    }

    if (direction == 1 && trackSequence == 3) {
        floorBoundOffset = (rct_xy16) {-16, -16};
    }

    const sint8 offsetsB[4][4][2] = {
        {{0, 0}, {+8, +16}, {+8, +8},  {+8, +8}},
        {{0, 0}, {-8, -8},  {-8, -8},  {+8, +8}},
        {{0, 0}, {+8, +8},  {+8, +16}, {+8, +8}},
        {{0, 0}, {-8, -8},  {-8, -8},  {+8, +8}},
    };

    uint32 imageId = miniature_railway_track_pieces_diag_25_deg_up[direction];
    bool drawRail = miniature_railway_diag_image_segment[direction][trackSequence];
    static const sint8 offsetB[] = {+8, 0, +8, +8};
    if (hasSupports) {
        sub_98197C(
            floorImage | gTrackColours[SCHEME_SUPPORTS],
            0, 0,
            floorBoundSize.x, floorBoundSize.y, (drawRail ? 2 : 0),
            height + offsetsB[direction][trackSequence][0],
            floorBoundOffset.x, floorBoundOffset.y, height + offsetsB[direction][trackSequence][1],
            get_current_rotation()
        );
        if (drawRail) {
            sub_98199C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height + offsetB[direction], get_current_rotation());
        }
    } else if (drawRail) {
        sub_98197C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height + offsetB[direction], get_current_rotation());
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
    return;
}

/** rct2: 0x008AD210 */
static void miniature_railway_track_diag_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool hasSupports = false;

    uint32 floorImage = 0;
    rct_xy16 floorBoundSize = { 0 };
    rct_xy16 floorBoundOffset = { 0 };

    sint16 supportType = monorail_diag_support_types[direction][trackSequence];
    if (supportType != -1) {
        floorImage = floors[supportType].image_id;
        floorBoundSize = floors[supportType].bound_size;
        floorBoundOffset = floors[supportType].bound_offset;
        hasSupports = wooden_a_supports_paint_setup(session, supportType, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    }

    if (direction == 1 && trackSequence == 3) {
        floorBoundOffset = (rct_xy16) {-16, -16};
    }

    uint32 imageId = miniature_railway_track_pieces_diag_flat_to_25_deg_up[direction];
    bool drawRail = miniature_railway_diag_image_segment[direction][trackSequence];

    if (hasSupports) {
        sub_98197C(
            floorImage | gTrackColours[SCHEME_SUPPORTS],
            0, 0,
            floorBoundSize.x, floorBoundSize.y, (drawRail ? 2 : 0),
            height,
            floorBoundOffset.x, floorBoundOffset.y, height,
            get_current_rotation()
        );
        if (drawRail) {
            sub_98199C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
        }
    } else if (drawRail) {
        sub_98197C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AD220 */
static void miniature_railway_track_diag_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element *mapElement)
{
    const sint8 supportOffsets[][4] = {
        {0, +8, +8, +8},
        {0, -8, -8, 0},
        {0, +8, +8, +8},
        {0, -8, -8, +8},
    };

    sint8 supportFunction = WOOD_B;
    if (trackSequence == 3) {
        supportFunction = WOOD_A;
    }

    bool hasSupports = false;
    sint16 supportType = monorail_diag_support_types[direction][trackSequence];

    uint32 floorImage = 0;
    rct_xy16 floorBoundSize = { 0 };
    rct_xy16 floorBoundOffset = { 0 };

    if (supportType != -1) {
        floorImage = floors[supportType].image_id;
        floorBoundSize = floors[supportType].bound_size;
        floorBoundOffset = floors[supportType].bound_offset;
        hasSupports = wooden_supports_paint_setup(session, supportFunction, supportType, 0, height + supportOffsets[direction][trackSequence], gTrackColours[SCHEME_SUPPORTS], NULL);
    }

    if (direction == 1 && trackSequence == 3) {
        floorBoundOffset = (rct_xy16) {-16, -16};
    }

    const sint8 offsetsB[4][4][2] = {
        {{0, 0}, {+8, +16}, {+8, +8},  {+8, +8}},
        {{0, 0}, {-8, -8},  {-8, -8},  {+8, +8}},
        {{0, 0}, {+8, +8},  {+8, +16}, {+8, +8}},
        {{0, 0}, {-8, -8},  {-8, -8},  {+8, +8}},
    };

    uint32 imageId = miniature_railway_track_pieces_diag_25_deg_up_to_flat[direction];
    bool drawRail = miniature_railway_diag_image_segment[direction][trackSequence];

    const sint8 railOffsets[] = {+8, 0, +8, +8};

    if (hasSupports) {
        sub_98197C(
            floorImage | gTrackColours[SCHEME_SUPPORTS],
            0, 0,
            floorBoundSize.x, floorBoundSize.y, (drawRail ? 2 : 0),
            height + offsetsB[direction][trackSequence][0],
            floorBoundOffset.x, floorBoundOffset.y, height + offsetsB[direction][trackSequence][1],
            get_current_rotation()
        );
        if (drawRail) {
            sub_98199C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height + railOffsets[direction], get_current_rotation());
        }
    } else if (drawRail) {
        sub_98197C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height + railOffsets[direction], get_current_rotation());
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AD260 */
static void miniature_railway_track_diag_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    const sint8 supportOffsets[][4] = {
        {0, +16, +16, 0},
        {0, -8,  -8,  0},
        {0, +16, +16, 0},
        {0, -8,  -8,  -8},
    };

    sint8 supportFunction = WOOD_B;
    if (trackSequence == 3) {
        supportFunction = WOOD_A;
    }

    bool hasSupports = false;

    uint32 floorImage = 0;
    rct_xy16 floorBoundSize = { 0 };
    rct_xy16 floorBoundOffset = { 0 };
    sint16 supportType = monorail_diag_support_types[direction][trackSequence];

    if (supportType != -1) {
        floorImage = floors[supportType].image_id;
        floorBoundSize = floors[supportType].bound_size;
        floorBoundOffset = floors[supportType].bound_offset;
        hasSupports = wooden_supports_paint_setup(session, supportFunction, supportType, 0, height + supportOffsets[direction][trackSequence], gTrackColours[SCHEME_SUPPORTS], NULL);
    }

    if (direction == 1 && trackSequence == 3) {
        floorBoundOffset = (rct_xy16) {-16, -16};
    }

    uint32 imageId = miniature_railway_track_pieces_diag_25_deg_up[(direction + 2) % 4];
    bool drawRail = miniature_railway_diag_image_segment[direction][trackSequence];
    const sint8 railOffsets[] = {0, +8, +8, +8};

    const sint8 offsetsB[4][4][2] = {
        {{0, 0}, {+8, +8}, {+8, +8},  {-8, -8}},
        {{0, 0}, {-8, -8}, {-8, -8},  {-8, 0}},
        {{0, 0}, {+8, +8}, {+8, +16}, {-8, -8}},
        {{0, 0}, {-8, -8}, {-8, -8},  {-8, -8}},
    };

    if (hasSupports) {
        sub_98197C(
            floorImage | gTrackColours[SCHEME_SUPPORTS],
            0, 0,
            floorBoundSize.x, floorBoundSize.y, (drawRail ? 2 : 0),
            height + offsetsB[direction][trackSequence][0],
            floorBoundOffset.x, floorBoundOffset.y, height + offsetsB[direction][trackSequence][1],
            get_current_rotation()
        );
        if (drawRail) {
            sub_98199C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height + railOffsets[direction], get_current_rotation());
        }
    } else if (drawRail) {
        sub_98197C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height + railOffsets[direction], get_current_rotation());
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AD240 */
static void miniature_railway_track_diag_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    const sint8 supportOffsets[][4] = {
        {0, +16, +16, 0},
        {0, -8,  -8,  -8},
        {0, +8,  +8,  0},
        {0, -8,  -8,  -8},
    };

    sint8 supportFunction = WOOD_B;
    if (trackSequence == 3) {
        supportFunction = WOOD_A;
    }

    bool hasSupports = false;
    uint32 floorImage = 0;
    rct_xy16 floorBoundSize = { 0 };
    rct_xy16 floorBoundOffset = { 0 };
    sint16 supportType = monorail_diag_support_types[direction][trackSequence];

    if (supportType != -1) {
        floorImage = floors[supportType].image_id;
        floorBoundSize = floors[supportType].bound_size;
        floorBoundOffset = floors[supportType].bound_offset;
        hasSupports = wooden_supports_paint_setup(session, supportFunction, supportType, 0, height + supportOffsets[direction][trackSequence], gTrackColours[SCHEME_SUPPORTS], NULL);
    }

    if (direction == 1 && trackSequence == 3) {
        floorBoundOffset = (rct_xy16) {-16, -16};
    }

    const sint8 offsetsB[4][4][2] = {
        {{0, 0}, {+8, +8}, {+8, +8},  {-8, -8}},
        {{0, 0}, {-8, -8}, {-8, -8},  {-8, 0}},
        {{0, 0}, {+8, +8}, {+8, +16}, {-8, -8}},
        {{0, 0}, {-8, -8}, {-8, -8},  {-8, -8}},
    };

    uint32 imageId = miniature_railway_track_pieces_diag_25_deg_up_to_flat[(direction + 2) % 4];
    bool drawRail = miniature_railway_diag_image_segment[direction][trackSequence];
    const sint8 railOffsets[] = {0, +8, +8, +8};

    if (hasSupports) {
        sub_98197C(
            floorImage | gTrackColours[SCHEME_SUPPORTS],
            0, 0,
            floorBoundSize.x, floorBoundSize.y, (drawRail ? 2 : 0),
            height + offsetsB[direction][trackSequence][0],
            floorBoundOffset.x, floorBoundOffset.y, height + offsetsB[direction][trackSequence][1],
            get_current_rotation()
        );
        if (drawRail) {
            sub_98199C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height + railOffsets[direction], get_current_rotation());
        }
    } else if (drawRail) {
        sub_98197C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height + railOffsets[direction], get_current_rotation());
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

static void miniature_railway_track_diag_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    bool hasSupports = false;
    uint32 floorImage = 0;
    rct_xy16 floorBoundSize = { 0 };
    rct_xy16 floorBoundOffset = { 0 };
    sint16 supportType = monorail_diag_support_types[direction][trackSequence];

    if (supportType != -1) {
        floorImage = floors[supportType].image_id;
        floorBoundSize = floors[supportType].bound_size;
        floorBoundOffset = floors[supportType].bound_offset;
        hasSupports = wooden_a_supports_paint_setup(session, supportType, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    }

    if (direction == 1 && trackSequence == 3) {
        floorBoundOffset = (rct_xy16) {-16, -16};
    }

    uint32 imageId = miniature_railway_track_pieces_diag_flat_to_25_deg_up[(direction + 2) % 4];
    bool drawRail = miniature_railway_diag_image_segment[direction][trackSequence];

    if (hasSupports) {
        sub_98197C(
            floorImage | gTrackColours[SCHEME_SUPPORTS],
            0, 0,
            floorBoundSize.x, floorBoundSize.y, (drawRail ? 2 : 0),
            height,
            floorBoundOffset.x, floorBoundOffset.y, height,
            get_current_rotation()
        );
        if (drawRail) {
            sub_98199C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
        }
    } else if (drawRail) {
        sub_98197C(imageId | gTrackColours[SCHEME_TRACK], -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/**
 * rct2: 0x008ACE48
 */
TRACK_PAINT_FUNCTION get_track_paint_function_miniature_railway(sint32 trackType, sint32 direction)
{
    switch (trackType) {
        case TRACK_ELEM_FLAT:
            return paint_miniature_railway_track_flat;

        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            return paint_miniature_railway_station;

        case TRACK_ELEM_25_DEG_UP:
            return paint_miniature_railway_track_25_deg_up;
        case TRACK_ELEM_FLAT_TO_25_DEG_UP:
            return paint_miniature_railway_track_flat_to_25_deg_up;
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            return paint_miniature_railway_track_25_deg_up_to_flat;

        case TRACK_ELEM_25_DEG_DOWN:
            return paint_miniature_railway_track_25_deg_down;
        case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
            return paint_miniature_railway_track_flat_to_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
            return paint_miniature_railway_track_25_deg_down_to_flat;

        case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
            return paint_miniature_railway_track_left_quarter_turn_5_tiles;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
            return paint_miniature_railway_track_right_quarter_turn_5_tiles;

        case TRACK_ELEM_S_BEND_LEFT:
            return paint_miniature_railway_track_s_bend_left;
        case TRACK_ELEM_S_BEND_RIGHT:
            return paint_miniature_railway_track_s_bend_right;

        case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
            return paint_miniature_railway_track_left_quarter_turn_3_tiles;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
            return paint_miniature_railway_track_right_quarter_turn_3_tiles;

        case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
            return paint_miniature_railway_track_left_eighth_to_diag;
        case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
            return paint_miniature_railway_track_right_eighth_to_diag;
        case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
            return paint_miniature_railway_track_left_eighth_to_orthogonal;
        case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
            return paint_miniature_railway_track_right_eighth_to_orthogonal;

        case TRACK_ELEM_DIAG_FLAT:
            return miniature_railway_track_diag_flat;
        case TRACK_ELEM_DIAG_25_DEG_UP:
            return miniature_railway_track_diag_25_deg_up;
        case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
            return miniature_railway_track_diag_flat_to_25_deg_up;
        case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
            return miniature_railway_track_diag_25_deg_up_to_flat;
        case TRACK_ELEM_DIAG_25_DEG_DOWN:
            return miniature_railway_track_diag_25_deg_down;
        case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
            return miniature_railway_track_diag_flat_to_25_deg_down;
        case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
            return miniature_railway_track_diag_25_deg_down_to_flat;
    }

    return NULL;
}
