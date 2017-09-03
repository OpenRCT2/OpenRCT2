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
    SPR_MONORAIL_FLAT_SW_NE = 23231,
    SPR_MONORAIL_FLAT_NW_SE = 23232,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_SW_SE_PART_0 = 23233,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_SW_SE_PART_1 = 23234,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_SW_SE_PART_2 = 23235,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_SW_SE_PART_3 = 23236,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_SW_SE_PART_4 = 23237,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_NW_SW_PART_0 = 23238,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_NW_SW_PART_1 = 23239,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_NW_SW_PART_2 = 23240,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_NW_SW_PART_3 = 23241,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_NW_SW_PART_4 = 23242,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_NE_NW_PART_0 = 23243,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_NE_NW_PART_1 = 23244,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_NE_NW_PART_2 = 23245,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_NE_NW_PART_3 = 23246,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_NE_NW_PART_4 = 23247,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_SE_NE_PART_0 = 23248,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_SE_NE_PART_1 = 23249,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_SE_NE_PART_2 = 23250,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_SE_NE_PART_3 = 23251,
    SPR_MONORAIL_QUARTER_TURN_5_TILES_SE_NE_PART_4 = 23252,
    SPR_MONORAIL_25_DEG_UP_SW_NE = 23253,
    SPR_MONORAIL_FLAT_TO_25_DEG_UP_SW_NE = 23254,
    SPR_MONORAIL_25_DEG_UP_NE_SW = 23255,
    SPR_MONORAIL_FLAT_TO_25_DEG_UP_NE_SW = 23256,
    SPR_MONORAIL_25_DEG_UP_TO_FLAT_NE_SW = 23257,
    SPR_MONORAIL_25_DEG_UP_TO_FLAT_SW_NE = 23258,
    SPR_MONORAIL_25_DEG_UP_NW_SE = 23259,
    SPR_MONORAIL_25_DEG_UP_SE_NW = 23260,
    SPR_MONORAIL_FLAT_TO_25_DEG_UP_NW_SE = 23261,
    SPR_MONORAIL_FLAT_TO_25_DEG_UP_SE_NW = 23262,
    SPR_MONORAIL_25_DEG_UP_TO_FLAT_SE_NW = 23263,
    SPR_MONORAIL_25_DEG_UP_TO_FLAT_NW_SE = 23264,
    SPR_MONORAIL_S_BEND_RIGHT_SW_NE_PART_0 = 23265,
    SPR_MONORAIL_S_BEND_RIGHT_SW_NE_PART_1 = 23266,
    SPR_MONORAIL_S_BEND_RIGHT_SW_NE_PART_2 = 23267,
    SPR_MONORAIL_S_BEND_RIGHT_SW_NE_PART_3 = 23268,
    SPR_MONORAIL_S_BEND_LEFT_SW_NE_PART_0 = 23269,
    SPR_MONORAIL_S_BEND_LEFT_SW_NE_PART_1 = 23270,
    SPR_MONORAIL_S_BEND_LEFT_SW_NE_PART_2 = 23271,
    SPR_MONORAIL_S_BEND_LEFT_SW_NE_PART_3 = 23272,
    SPR_MONORAIL_S_BEND_LEFT_SE_NW_PART_0 = 23273,
    SPR_MONORAIL_S_BEND_LEFT_SE_NW_PART_1 = 23274,
    SPR_MONORAIL_S_BEND_LEFT_SE_NW_PART_2 = 23275,
    SPR_MONORAIL_S_BEND_LEFT_SE_NW_PART_3 = 23276,
    SPR_MONORAIL_S_BEND_RIGHT_SE_NW_PART_0 = 23277,
    SPR_MONORAIL_S_BEND_RIGHT_SE_NW_PART_1 = 23278,
    SPR_MONORAIL_S_BEND_RIGHT_SE_NW_PART_2 = 23279,
    SPR_MONORAIL_S_BEND_RIGHT_SE_NW_PART_3 = 23280,
    SPR_MONORAIL_QUARTER_TURN_3_TILES_SW_SE_PART_0 = 23281,
    SPR_MONORAIL_QUARTER_TURN_3_TILES_SW_SE_PART_1 = 23282,
    SPR_MONORAIL_QUARTER_TURN_3_TILES_SW_SE_PART_2 = 23283,
    SPR_MONORAIL_QUARTER_TURN_3_TILES_NW_SW_PART_0 = 23284,
    SPR_MONORAIL_QUARTER_TURN_3_TILES_NW_SW_PART_1 = 23285,
    SPR_MONORAIL_QUARTER_TURN_3_TILES_NW_SW_PART_2 = 23286,
    SPR_MONORAIL_QUARTER_TURN_3_TILES_NE_NW_PART_0 = 23287,
    SPR_MONORAIL_QUARTER_TURN_3_TILES_NE_NW_PART_1 = 23288,
    SPR_MONORAIL_QUARTER_TURN_3_TILES_NE_NW_PART_2 = 23289,
    SPR_MONORAIL_QUARTER_TURN_3_TILES_SE_NE_PART_0 = 23290,
    SPR_MONORAIL_QUARTER_TURN_3_TILES_SE_NE_PART_1 = 23291,
    SPR_MONORAIL_QUARTER_TURN_3_TILES_SE_NE_PART_2 = 23292,
    SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_0 = 23293,
    SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_1 = 23294,
    SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_2 = 23295,
    SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_3 = 23296,
    SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_0 = 23297,
    SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_1 = 23298,
    SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_2 = 23299,
    SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_3 = 23300,
    SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_0 = 23301,
    SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_1 = 23302,
    SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_2 = 23303,
    SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_3 = 23304,
    SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_0 = 23305,
    SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_1 = 23306,
    SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_2 = 23307,
    SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_3 = 23308,
    SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_0 = 23309,
    SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_1 = 23310,
    SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_2 = 23311,
    SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_3 = 23312,
    SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_0 = 23313,
    SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_1 = 23314,
    SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_2 = 23315,
    SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_3 = 23316,
    SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_0 = 23317,
    SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_1 = 23318,
    SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_2 = 23319,
    SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_3 = 23320,
    SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_0 = 23321,
    SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_1 = 23322,
    SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_2 = 23323,
    SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_3 = 23324,
    SPR_MONORAIL_DIAG_FLAT_W_E = 23325,
    SPR_MONORAIL_DIAG_FLAT_N_S = 23326,
    SPR_MONORAIL_DIAG_FLAT_E_W = 23327,
    SPR_MONORAIL_DIAG_FLAT_S_N = 23328,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_W_E = 23329,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_N_S = 23330,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_E_W = 23331,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_S_N = 23332,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_W_E = 23333,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_N_S = 23334,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_E_W = 23335,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_S_N = 23336,
    SPR_MONORAIL_DIAG_25_DEG_UP_W_E = 23337,
    SPR_MONORAIL_DIAG_25_DEG_UP_N_S = 23338,
    SPR_MONORAIL_DIAG_25_DEG_UP_E_W = 23339,
    SPR_MONORAIL_DIAG_25_DEG_UP_S_N = 23340,
};

static const uint32 monorail_track_pieces_flat[4] = {
    SPR_MONORAIL_FLAT_SW_NE,
    SPR_MONORAIL_FLAT_NW_SE,
    SPR_MONORAIL_FLAT_SW_NE,
    SPR_MONORAIL_FLAT_NW_SE,
};

static const uint32 monorail_track_pieces_flat_quarter_turn_5_tiles[4][5] = {
    {
        SPR_MONORAIL_QUARTER_TURN_5_TILES_SW_SE_PART_0,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_SW_SE_PART_1,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_SW_SE_PART_2,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_SW_SE_PART_3,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_SW_SE_PART_4,
    },
    {
        SPR_MONORAIL_QUARTER_TURN_5_TILES_NW_SW_PART_0,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_NW_SW_PART_1,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_NW_SW_PART_2,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_NW_SW_PART_3,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_NW_SW_PART_4,
    },
    {
        SPR_MONORAIL_QUARTER_TURN_5_TILES_NE_NW_PART_0,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_NE_NW_PART_1,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_NE_NW_PART_2,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_NE_NW_PART_3,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_NE_NW_PART_4,
    },
    {
        SPR_MONORAIL_QUARTER_TURN_5_TILES_SE_NE_PART_0,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_SE_NE_PART_1,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_SE_NE_PART_2,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_SE_NE_PART_3,
        SPR_MONORAIL_QUARTER_TURN_5_TILES_SE_NE_PART_4,
    }
};

static const uint32 monorail_track_pieces_25_deg_up[4] = {
    SPR_MONORAIL_25_DEG_UP_SW_NE,
    SPR_MONORAIL_25_DEG_UP_NW_SE,
    SPR_MONORAIL_25_DEG_UP_NE_SW,
    SPR_MONORAIL_25_DEG_UP_SE_NW,
};

static const uint32 monorail_track_pieces_flat_to_25_deg_up[4] = {
    SPR_MONORAIL_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_MONORAIL_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_MONORAIL_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_MONORAIL_FLAT_TO_25_DEG_UP_SE_NW,
};

static const uint32 monorail_track_pieces_25_deg_up_to_flat[4] = {
    SPR_MONORAIL_25_DEG_UP_TO_FLAT_SW_NE,
    SPR_MONORAIL_25_DEG_UP_TO_FLAT_NW_SE,
    SPR_MONORAIL_25_DEG_UP_TO_FLAT_NE_SW,
    SPR_MONORAIL_25_DEG_UP_TO_FLAT_SE_NW,
};

static const uint32 monorail_track_pieces_s_bend_left[2][4] = {
    {
        SPR_MONORAIL_S_BEND_LEFT_SW_NE_PART_0,
        SPR_MONORAIL_S_BEND_LEFT_SW_NE_PART_1,
        SPR_MONORAIL_S_BEND_LEFT_SW_NE_PART_2,
        SPR_MONORAIL_S_BEND_LEFT_SW_NE_PART_3,
    },
    {
        SPR_MONORAIL_S_BEND_LEFT_SE_NW_PART_3,
        SPR_MONORAIL_S_BEND_LEFT_SE_NW_PART_2,
        SPR_MONORAIL_S_BEND_LEFT_SE_NW_PART_1,
        SPR_MONORAIL_S_BEND_LEFT_SE_NW_PART_0,
    }
};

static const uint32 monorail_track_pieces_s_bend_right[2][4] = {
    {
        SPR_MONORAIL_S_BEND_RIGHT_SW_NE_PART_0,
        SPR_MONORAIL_S_BEND_RIGHT_SW_NE_PART_1,
        SPR_MONORAIL_S_BEND_RIGHT_SW_NE_PART_2,
        SPR_MONORAIL_S_BEND_RIGHT_SW_NE_PART_3,
    },
    {
        SPR_MONORAIL_S_BEND_RIGHT_SE_NW_PART_3,
        SPR_MONORAIL_S_BEND_RIGHT_SE_NW_PART_2,
        SPR_MONORAIL_S_BEND_RIGHT_SE_NW_PART_1,
        SPR_MONORAIL_S_BEND_RIGHT_SE_NW_PART_0,
    }
};

static const uint32 monorail_track_pieces_flat_quarter_turn_3_tiles[4][3] = {
    {
        SPR_MONORAIL_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_SW_SE_PART_2
    },
    {
        SPR_MONORAIL_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_NW_SW_PART_2
    },
    {
        SPR_MONORAIL_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_NE_NW_PART_2
    },
    {
        SPR_MONORAIL_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_SE_NE_PART_2
    }
};

static const uint32 ghost_train_track_pieces_right_eight_to_diag[4][4] = {
    {
        SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_0,
        SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_1,
        SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_2,
        SPR_MONORAIL_EIGHT_TO_DIAG_SW_E_PART_3,
    },
    {
        SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_0,
        SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_1,
        SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_2,
        SPR_MONORAIL_EIGHT_TO_DIAG_NW_S_PART_3,
    },
    {
        SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_0,
        SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_1,
        SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_2,
        SPR_MONORAIL_EIGHT_TO_DIAG_NE_W_PART_3,
    },
    {
        SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_0,
        SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_1,
        SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_2,
        SPR_MONORAIL_EIGHT_TO_DIAG_SE_N_PART_3,
    },
};

static const rct_xy16 ghost_train_track_pieces_right_eight_to_diag_bounds[4][4] = {
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

static const rct_xy16 ghost_train_track_pieces_right_eight_to_diag_offset[4][4] = {
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

static const uint32 ghost_train_track_pieces_left_eight_to_diag[4][4] = {
    {
        SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_0,
        SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_1,
        SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_2,
        SPR_MONORAIL_EIGHT_TO_DIAG_SW_N_PART_3,
    },
    {
        SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_0,
        SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_1,
        SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_2,
        SPR_MONORAIL_EIGHT_TO_DIAG_NW_E_PART_3,
    },
    {
        SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_0,
        SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_1,
        SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_2,
        SPR_MONORAIL_EIGHT_TO_DIAG_NE_S_PART_3,
    },
    {
        SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_0,
        SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_1,
        SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_2,
        SPR_MONORAIL_EIGHT_TO_DIAG_SE_W_PART_3,
    },
};

static const rct_xy16 ghost_train_track_pieces_left_eight_to_diag_bounds[4][4] = {
    {
        {32, 20},
        {32, 16},
        {16, 16},
        {16, 16},
    },
    {
        {20, 32},
        {16, 34},
        {16, 16},
        {18, 16},
    },
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
};

static const rct_xy16 ghost_train_track_pieces_left_eight_to_diag_offset[4][4] = {
    {
        {0, 6},
        {0,  0},
        {0,  16},
        {16, 16},
    },
    {
        {6, 0},
        {0,  0},
        {16, 16},
        {16, 0},
    },
    {
        {0, 6},
        {0,  16},
        {16, 0},
        {0,  0},
    },
    {
        {6, 0},
        {16, 0},
        {0,  0},
        {0,  16},
    },
};

static const uint32 monorail_track_pieces_diag_flat[4] = {
    SPR_MONORAIL_DIAG_FLAT_W_E,
    SPR_MONORAIL_DIAG_FLAT_N_S,
    SPR_MONORAIL_DIAG_FLAT_E_W,
    SPR_MONORAIL_DIAG_FLAT_S_N,
};

static const uint32 monorail_track_pieces_diag_flat_to_25_deg_up[4] = {
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_W_E,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_N_S,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_E_W,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_S_N,
};

static const uint32 monorail_track_pieces_diag_25_deg_up_to_flat[4] = {
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_W_E,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_N_S,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_E_W,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_S_N,
};

static const uint32 monorail_track_pieces_diag_25_deg_up[4] = {
    SPR_MONORAIL_DIAG_25_DEG_UP_W_E,
    SPR_MONORAIL_DIAG_25_DEG_UP_N_S,
    SPR_MONORAIL_DIAG_25_DEG_UP_E_W,
    SPR_MONORAIL_DIAG_25_DEG_UP_S_N,
};

/** rct2: 0x008AE1AC */
static void paint_monorail_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;

    uint32 imageId = monorail_track_pieces_flat[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98196C(session, imageId, 0, 6, 32, 20, 3, height, get_current_rotation());
    } else {
        sub_98196C(session, imageId, 6, 0, 20, 32, 3, height, get_current_rotation());
    }

    if (direction == 0 || direction == 2) {
        paint_util_push_tunnel_left(session, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_right(session, height, TUNNEL_6);
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AE25C, 0x008AE26C, 0x008AE27C */
static void paint_monorail_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    if (direction == 0 || direction == 2) {
        imageId = SPR_STATION_BASE_B_SW_NE | gTrackColours[SCHEME_MISC];
        sub_98197C(session, imageId, 0, 0, 32, 28, 2, height - 2, 0, 2, height, get_current_rotation());
    } else if (direction == 1 || direction == 3) {
        imageId = SPR_STATION_BASE_B_NW_SE | gTrackColours[SCHEME_MISC];
        sub_98197C(session, imageId, 0, 0, 28, 32, 2, height - 2, 2, 0, height, get_current_rotation());
    }

    imageId = monorail_track_pieces_flat[direction] | gTrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2) {
        sub_98199C(session, imageId, 0, 6, 32, 20, 2, height, 0, 0, height, get_current_rotation());
    } else {
        sub_98199C(session, imageId, 6, 0, 20, 32, 2, height, 0, 0, height, get_current_rotation());
    }

    if (direction == 0 || direction == 2) {
        paint_util_push_tunnel_left(session, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_right(session, height, TUNNEL_6);
    }

    if (direction == 0 || direction == 2) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    } else {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    track_paint_util_draw_station(session, rideIndex, trackSequence, direction, height, mapElement);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AE1BC */
static void paint_monorail_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;

    uint32 imageId = monorail_track_pieces_25_deg_up[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98196C(session, imageId, 0, 6, 32, 20, 3, height, get_current_rotation());
    } else {
        sub_98196C(session, imageId, 6, 0, 20, 32, 3, height, get_current_rotation());
    }

    switch (direction) {
        case 0: paint_util_push_tunnel_left(session, height - 8, TUNNEL_7); break;
        case 1: paint_util_push_tunnel_right(session, height + 8, TUNNEL_8); break;
        case 2: paint_util_push_tunnel_left(session, height + 8, TUNNEL_8); break;
        case 3: paint_util_push_tunnel_right(session, height - 8, TUNNEL_7); break;
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008AE1CC */
static void paint_monorail_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;

    uint32 imageId = monorail_track_pieces_flat_to_25_deg_up[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98196C(session, imageId, 0, 6, 32, 20, 3, height, get_current_rotation());
    } else {
        sub_98196C(session, imageId, 6, 0, 20, 32, 3, height, get_current_rotation());
    }

    switch (direction) {
        case 0: paint_util_push_tunnel_left(session, height, TUNNEL_6); break;
        case 1: paint_util_push_tunnel_right(session, height, TUNNEL_8); break;
        case 2: paint_util_push_tunnel_left(session, height, TUNNEL_8); break;
        case 3: paint_util_push_tunnel_right(session, height, TUNNEL_6); break;
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 3, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x008AE1DC */
static void paint_monorail_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;

    uint32 imageId = monorail_track_pieces_25_deg_up_to_flat[direction] | gTrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2) {
        sub_98196C(session, imageId, 0, 6, 32, 20, 3, height, get_current_rotation());
    } else {
        sub_98196C(session, imageId, 6, 0, 20, 32, 3, height, get_current_rotation());
    }

    switch (direction) {
        case 0: paint_util_push_tunnel_left(session, height - 8, TUNNEL_6); break;
        case 1: paint_util_push_tunnel_right(session, height + 8, TUNNEL_14); break;
        case 2: paint_util_push_tunnel_left(session, height + 8, TUNNEL_14); break;
        case 3: paint_util_push_tunnel_right(session, height - 8, TUNNEL_6); break;
    }

    if (track_paint_util_should_paint_supports(position)) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 6, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x008AE1EC */
static void paint_monorail_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_monorail_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008AE1FC */
static void paint_monorail_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_monorail_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008AE20C */
static void paint_monorail_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_monorail_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008AE22C */
static void paint_monorail_track_right_quarter_turn_5_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    track_paint_util_right_quarter_turn_5_tiles_paint(session, 3, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], monorail_track_pieces_flat_quarter_turn_5_tiles, defaultRightQuarterTurn5TilesOffsets, defaultRightQuarterTurn5TilesBoundLengths, NULL, get_current_rotation());

    switch (trackSequence) {
        case 0:
        case 6:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
    }

    if (direction == 0 && trackSequence == 0) {
        paint_util_push_tunnel_left(session, height, TUNNEL_6);
    }

    if (direction == 0 && trackSequence == 6) {
        paint_util_push_tunnel_right(session, height, TUNNEL_6);
    }

    if (direction == 1 && trackSequence == 6) {
        paint_util_push_tunnel_left(session, height, TUNNEL_6);
    }

    if (direction == 3 && trackSequence == 0) {
        paint_util_push_tunnel_right(session, height, TUNNEL_6);
    }

    sint32 blockedSegments = 0;
    switch (trackSequence) {
        case 0: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC; break;
        case 2: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC; break;
        case 3: blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_C4; break;
        case 5: blockedSegments = SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8; break;
        case 6: blockedSegments = SEGMENT_D4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_B8; break;
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);


    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AE21C */
static void paint_monorail_track_left_quarter_turn_5_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    paint_monorail_track_right_quarter_turn_5_tiles(session, rideIndex, trackSequence, (direction + 1) % 4, height, mapElement);
}

/** rct2: 0x */
static void paint_monorail_track_s_bend_left(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (direction == 2 || direction == 3) {
        trackSequence = 3 - trackSequence;
    }

    const rct_xy16 offsetList[] = {
        {0, 6},
        {0, 0},
        {0, 6},
        {0, 6},
    };

    const rct_xy16 boundsList[] = {
        {32, 20},
        {32, 26},
        {32, 26},
        {32, 20},
    };

    uint32 imageId = monorail_track_pieces_s_bend_left[direction & 1][trackSequence] | gTrackColours[SCHEME_TRACK];
    rct_xy16 offset = offsetList[trackSequence];
    rct_xy16 bounds = boundsList[trackSequence];
    if (direction == 0 || direction == 2) {
        sub_98196C(session, imageId, (sint8) offset.x, (sint8) offset.y, bounds.x, bounds.y, 3, height, get_current_rotation());
    } else {
        sub_98196C(session, imageId, (sint8) offset.y, (sint8) offset.x, bounds.y, bounds.x, 3, height, get_current_rotation());
    }

    if (direction == 0 || direction == 2) {
        if (trackSequence == 0) {
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
        }

        switch (trackSequence) {
            case 0: metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]); break;
            case 1: metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height, gTrackColours[SCHEME_SUPPORTS]); break;
            case 3: metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]); break;
        }
    } else {
        if (trackSequence == 3) {
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
        }

        switch (trackSequence) {
            case 0: metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]); break;
            case 1: metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height, gTrackColours[SCHEME_SUPPORTS]); break;
            case 3: metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]); break;
        }
    }

    sint32 blockedSegments = 0;
    switch (trackSequence) {
        case 0: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4; break;
        case 1: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4; break;
        case 2: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC; break;
        case 3: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0; break;
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction & 1), 0xFFFF, 0);

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AE24C */
static void paint_monorail_track_s_bend_right(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (direction == 2 || direction == 3) {
        trackSequence = 3 - trackSequence;
    }

    const rct_xy16 offsetList[] = {
        {0, 6},
        {0, 6},
        {0, 0},
        {0, 6},
    };

    const rct_xy16 boundsList[] = {
        {32, 20},
        {32, 26},
        {32, 26},
        {32, 20},
    };

    uint32 imageId = monorail_track_pieces_s_bend_right[direction&1][trackSequence] | gTrackColours[SCHEME_TRACK];
    rct_xy16 offset = offsetList[trackSequence];
    rct_xy16 bounds = boundsList[trackSequence];
    if (direction == 0 || direction == 2) {
        sub_98196C(session, imageId, (sint8) offset.x, (sint8) offset.y, bounds.x, bounds.y, 3, height, get_current_rotation());
    } else {
        sub_98196C(session, imageId, (sint8) offset.y, (sint8) offset.x, bounds.y, bounds.x, 3, height, get_current_rotation());
    }

    if (direction == 0 || direction == 2) {
        if (trackSequence == 0) {
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
        }

        switch (trackSequence) {
            case 0: metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]); break;
            case 1: metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height, gTrackColours[SCHEME_SUPPORTS]); break;
            case 3: metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]); break;
        }
    } else {
        if (trackSequence == 3) {
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
        }

        switch (trackSequence) {
            case 0: metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]); break;
            case 1: metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height, gTrackColours[SCHEME_SUPPORTS]); break;
            case 3: metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]); break;
        }
    }

    sint32 blockedSegments = 0;
    switch (trackSequence) {
        case 0: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC; break;
        case 1: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC; break;
        case 2: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4; break;
        case 3: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8; break;
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction & 1), 0xFFFF, 0);

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AE29C */
static void paint_monorail_track_right_quarter_turn_3_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    track_paint_util_right_quarter_turn_3_tiles_paint(session, 3, height, direction, trackSequence, gTrackColours[SCHEME_TRACK], monorail_track_pieces_flat_quarter_turn_3_tiles, defaultRightQuarterTurn3TilesOffsets, defaultRightQuarterTurn3TilesBoundLengths, NULL, get_current_rotation());
    track_paint_util_right_quarter_turn_3_tiles_tunnel(session, height, direction, trackSequence, TUNNEL_6);

    switch (trackSequence) {
        case 0:
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
    }

    sint32 blockedSegments = 0;
    switch (trackSequence) {
        case 0: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC; break;
        case 2: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0; break;
        case 3: blockedSegments = SEGMENT_D4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_B8; break;
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AE28C */
static void paint_monorail_track_left_quarter_turn_3_tiles(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    paint_monorail_track_right_quarter_turn_3_tiles(session, rideIndex, trackSequence, (direction + 1) % 4, height, mapElement);
}

static const sint8 paint_monorail_eighth_to_diag_index[] = {0, 1, 2, -1, 3};

/** rct2: 0x008AE31C */
static void paint_monorail_track_left_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    sint8 index = paint_monorail_eighth_to_diag_index[trackSequence];
    if (index >= 0) {
        uint32 imageId = ghost_train_track_pieces_left_eight_to_diag[direction][index] | gTrackColours[SCHEME_TRACK];
        const rct_xy16 offset = ghost_train_track_pieces_left_eight_to_diag_offset[direction][index];
        const rct_xy16 bounds = ghost_train_track_pieces_left_eight_to_diag_bounds[direction][index];
        sub_98197C(session, imageId, 0, 0, bounds.x, bounds.y, 2, height, offset.x, offset.y, height, get_current_rotation());
    }

    switch (trackSequence) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 4:
            if (direction == 0) metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            if (direction == 1) metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            if (direction == 2) metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            if (direction == 3) metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
    }

    if (direction == 0 && trackSequence == 0) {
        paint_util_push_tunnel_left(session, height, TUNNEL_0);
    }
    if (direction == 3 && trackSequence == 0) {
        paint_util_push_tunnel_right(session, height, TUNNEL_0);
    }

    sint32 blockedSegments = 0;
    switch (trackSequence) {
        case 0: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC; break;
        case 1: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4; break;
        case 2: blockedSegments = SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC; break;
        case 3: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_B8 | SEGMENT_C8; break;
        case 4: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C8 | SEGMENT_C0 | SEGMENT_D4; break;
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AE32C */
static void paint_monorail_track_right_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    sint8 index = paint_monorail_eighth_to_diag_index[trackSequence];
    if (index >= 0) {
        uint32 imageId = ghost_train_track_pieces_right_eight_to_diag[direction][index] | gTrackColours[SCHEME_TRACK];
        const rct_xy16 offset = ghost_train_track_pieces_right_eight_to_diag_offset[direction][index];
        const rct_xy16 bounds = ghost_train_track_pieces_right_eight_to_diag_bounds[direction][index];
        sub_98197C(session, imageId, 0, 0, bounds.x, bounds.y, 2, height, offset.x, offset.y, height, get_current_rotation());
    }

    switch (trackSequence) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 4:
            if (direction == 0) metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            if (direction == 1) metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            if (direction == 2) metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            if (direction == 3) metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
    }

    if (direction == 0 && trackSequence == 0) {
        paint_util_push_tunnel_left(session, height, TUNNEL_0);
    }
    if (direction == 3 && trackSequence == 0) {
        paint_util_push_tunnel_right(session, height, TUNNEL_0);
    }

    sint32 blockedSegments = 0;
    switch (trackSequence) {
        case 0: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC; break;
        case 1: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC; break;
        case 2: blockedSegments = SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4; break;
        case 3: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_C0 | SEGMENT_D4; break;
        case 4: blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_D4; break;
    }

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AE33C */
static void paint_monorail_track_left_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    const uint8 map[] = {4, 2, 3, 1, 0};
    trackSequence = map[trackSequence];
    paint_monorail_track_right_eighth_to_diag(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x008AE34C */
static void paint_monorail_track_right_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    const uint8 map[] = {4, 2, 3, 1, 0};
    trackSequence = map[trackSequence];
    paint_monorail_track_left_eighth_to_diag(session, rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

static void paint_monorail_util_diag_supports(paint_session * session, uint8 direction, sint32 height)
{
    switch (direction) {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 1, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 0, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 2, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 3, 0, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
    }
}

static const bool monorail_diag_image_segment[][4] = {
    {false, true,  false, false},
    {false, false, false, true},
    {false, false, true,  false},
    {true,  false, false, false},
};

static const uint8 monorail_diag_support_segment[] = {1, 0, 2, 3};

static const sint32 monorail_diag_blocked_segments[] = {
    SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4 | SEGMENT_BC,
    SEGMENT_C4 | SEGMENT_CC | SEGMENT_C8 | SEGMENT_B4,
    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_C0 | SEGMENT_D4,
    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_B8 | SEGMENT_C8
};

/** rct2: 0x008AE2AC */
static void paint_monorail_track_diag_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (monorail_diag_image_segment[direction][trackSequence]) {
        uint32 imageId = monorail_track_pieces_diag_flat[direction] | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
    }

    if (trackSequence == 3) {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, monorail_diag_support_segment[direction], 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    sint32 blockedSegments = monorail_diag_blocked_segments[trackSequence];
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008AE2DC */
static void paint_monorail_track_diag_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (monorail_diag_image_segment[direction][trackSequence]) {
        uint32 imageId = monorail_track_pieces_diag_25_deg_up[direction] | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
    }

    if (trackSequence == 3) {
        metal_b_supports_paint_setup(session, METAL_SUPPORTS_BOXED, monorail_diag_support_segment[direction], 8, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    sint32 blockedSegments = monorail_diag_blocked_segments[trackSequence];
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008AE2BC */
static void paint_monorail_track_diag_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (monorail_diag_image_segment[direction][trackSequence]) {
        uint32 imageId = monorail_track_pieces_diag_flat_to_25_deg_up[direction] | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
    }

    if (trackSequence == 3) {
        metal_b_supports_paint_setup(session, METAL_SUPPORTS_BOXED, monorail_diag_support_segment[direction], 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    sint32 blockedSegments = monorail_diag_blocked_segments[trackSequence];
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x008AE2CC */
static void paint_monorail_track_diag_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (monorail_diag_image_segment[direction][trackSequence]) {
        uint32 imageId = monorail_track_pieces_diag_25_deg_up_to_flat[direction] | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
    }

    if (trackSequence == 3) {
        metal_b_supports_paint_setup(session, METAL_SUPPORTS_BOXED, monorail_diag_support_segment[direction], 4, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    sint32 blockedSegments = monorail_diag_blocked_segments[trackSequence];
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008AE30C */
static void paint_monorail_track_diag_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (monorail_diag_image_segment[direction][trackSequence]) {
        uint32 imageId = monorail_track_pieces_diag_25_deg_up[(direction + 2) % 4] | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
    }

    if (trackSequence == 3) {
        metal_b_supports_paint_setup(session, METAL_SUPPORTS_BOXED, monorail_diag_support_segment[direction], 8, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    sint32 blockedSegments = monorail_diag_blocked_segments[trackSequence];
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008AE2EC */
static void paint_monorail_track_diag_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (monorail_diag_image_segment[direction][trackSequence]) {
        uint32 imageId = monorail_track_pieces_diag_25_deg_up_to_flat[(direction + 2) % 4] | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
    }

    if (trackSequence == 3) {
        metal_b_supports_paint_setup(session, METAL_SUPPORTS_BOXED, monorail_diag_support_segment[direction], 4, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    sint32 blockedSegments = monorail_diag_blocked_segments[trackSequence];
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008AE2FC */
static void paint_monorail_track_diag_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (monorail_diag_image_segment[direction][trackSequence]) {
        uint32 imageId = monorail_track_pieces_diag_flat_to_25_deg_up[(direction + 2) % 4] | gTrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, -16, -16, 32, 32, 2, height, -16, -16, height, get_current_rotation());
    }

    if (trackSequence == 3) {
        metal_b_supports_paint_setup(session, METAL_SUPPORTS_BOXED, monorail_diag_support_segment[direction], 0, height, gTrackColours[SCHEME_SUPPORTS]);
    }

    sint32 blockedSegments = monorail_diag_blocked_segments[trackSequence];
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/**
 * rct2: 0x008ADF34
 */
TRACK_PAINT_FUNCTION get_track_paint_function_monorail(sint32 trackType, sint32 direction)
{
    switch (trackType) {
        case TRACK_ELEM_FLAT:
            return paint_monorail_track_flat;

        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            return paint_monorail_station;

        case TRACK_ELEM_25_DEG_UP:
            return paint_monorail_track_25_deg_up;
        case TRACK_ELEM_FLAT_TO_25_DEG_UP:
            return paint_monorail_track_flat_to_25_deg_up;
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            return paint_monorail_track_25_deg_up_to_flat;

        case TRACK_ELEM_25_DEG_DOWN:
            return paint_monorail_track_25_deg_down;
        case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
            return paint_monorail_track_flat_to_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
            return paint_monorail_track_25_deg_down_to_flat;

        case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
            return paint_monorail_track_left_quarter_turn_5_tiles;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
            return paint_monorail_track_right_quarter_turn_5_tiles;

        case TRACK_ELEM_S_BEND_LEFT:
            return paint_monorail_track_s_bend_left;
        case TRACK_ELEM_S_BEND_RIGHT:
            return paint_monorail_track_s_bend_right;

        case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
            return paint_monorail_track_left_quarter_turn_3_tiles;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
            return paint_monorail_track_right_quarter_turn_3_tiles;

        case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
            return paint_monorail_track_left_eighth_to_diag;
        case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
            return paint_monorail_track_right_eighth_to_diag;
        case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
            return paint_monorail_track_left_eighth_to_orthogonal;
        case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
            return paint_monorail_track_right_eighth_to_orthogonal;

        case TRACK_ELEM_DIAG_FLAT:
            return paint_monorail_track_diag_flat;

        case TRACK_ELEM_DIAG_25_DEG_UP:
            return paint_monorail_track_diag_25_deg_up;
        case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
            return paint_monorail_track_diag_flat_to_25_deg_up;
        case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
            return paint_monorail_track_diag_25_deg_up_to_flat;

        case TRACK_ELEM_DIAG_25_DEG_DOWN:
            return paint_monorail_track_diag_25_deg_down;
        case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
            return paint_monorail_track_diag_flat_to_25_deg_down;
        case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
            return paint_monorail_track_diag_25_deg_down_to_flat;
    }

    return NULL;
}
