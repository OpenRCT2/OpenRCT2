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

#include "../../config/Config.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../ride_data.h"
#include "../track_data.h"
#include "../track_paint.h"
#include "../../world/map.h"
#include "../vehicle_paint.h"

enum {
    SPR_MINI_GOLF_FLAT_SW_NE = 14404,
    SPR_MINI_GOLF_FLAT_NW_SE = 14405,
    SPR_MINI_GOLF_FLAT_FENCE_BACK_SW_NE = 14406,
    SPR_MINI_GOLF_FLAT_FENCE_BACK_NW_SE = 14407,
    SPR_MINI_GOLF_FLAT_FENCE_FRONT_SW_NE = 14408,
    SPR_MINI_GOLF_FLAT_FENCE_FRONT_NW_SE = 14409,
    SPR_MINI_GOLF_STATION_FLOOR = 14410,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_SW_NW = 14411,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_NW_NE = 14412,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_NE_SE = 14413,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_SE_SW = 14414,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_BACK_SW_NW = 14415,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_BACK_NW_NE = 14416,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_BACK_NE_SE = 14417,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_BACK_SE_SW = 14418,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_FRONT_SW_NW = 14419,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_FRONT_NW_NE = 14420,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_FRONT_NE_SE = 14421,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_FRONT_SE_SW = 14422,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_INSIDE_SW_NW = 14423,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_INSIDE_NE_SE = 14424,
    SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_SW_NE = 14425,
    SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_NW_SE = 14426,
    SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_NE_SW = 14427,
    SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_SE_NW = 14428,
    SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_SW_NE = 14429,
    SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_NW_SE = 14430,
    SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_NE_SW = 14431,
    SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_SE_NW = 14432,
    SPR_MINI_GOLF_25_DEG_UP_SW_NE = 14433,
    SPR_MINI_GOLF_25_DEG_UP_NW_SE = 14434,
    SPR_MINI_GOLF_25_DEG_UP_NE_SW = 14435,
    SPR_MINI_GOLF_25_DEG_UP_SE_NW = 14436,
    SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_BACK_SW_NE = 14437,
    SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_BACK_NW_SE = 14438,
    SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_BACK_NE_SW = 14439,
    SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_BACK_SE_NW = 14440,
    SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_BACK_SW_NE = 14441,
    SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_BACK_NW_SE = 14442,
    SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_BACK_NE_SW = 14443,
    SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_BACK_SE_NW = 14444,
    SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_FRONT_SW_NE = 14445,
    SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_FRONT_NW_SE = 14446,
    SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_FRONT_NE_SW = 14447,
    SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_FRONT_SE_NW = 14448,
    SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_FRONT_SW_NE = 14449,
    SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_FRONT_NW_SE = 14450,
    SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_FRONT_NE_SW = 14451,
    SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_FRONT_SE_NW = 14452,
    SPR_MINI_GOLF_25_DEG_UP_FENCE_BACK_SW_NE = 14453,
    SPR_MINI_GOLF_25_DEG_UP_FENCE_BACK_NW_SE = 14454,
    SPR_MINI_GOLF_25_DEG_UP_FENCE_BACK_NE_SW = 14455,
    SPR_MINI_GOLF_25_DEG_UP_FENCE_BACK_SE_NW = 14456,
    SPR_MINI_GOLF_25_DEG_UP_FENCE_FRONT_SW_NE = 14457,
    SPR_MINI_GOLF_25_DEG_UP_FENCE_FRONT_NW_SE = 14458,
    SPR_MINI_GOLF_25_DEG_UP_FENCE_FRONT_NE_SW = 14459,
    SPR_MINI_GOLF_25_DEG_UP_FENCE_FRONT_SE_NW = 14460,

    SPR_MINI_GOLF_HOLE_A_BASE_PART_1_SW_NE = 14461,
    SPR_MINI_GOLF_HOLE_A_BASE_PART_2_SW_NE = 14462,
    SPR_MINI_GOLF_HOLE_A_BASE_PART_1_NW_SE = 14463,
    SPR_MINI_GOLF_HOLE_A_BASE_PART_2_NW_SE = 14464,
    SPR_MINI_GOLF_HOLE_A_BASE_PART_1_NE_SW = 14465,
    SPR_MINI_GOLF_HOLE_A_BASE_PART_2_NE_SW = 14466,
    SPR_MINI_GOLF_HOLE_A_BASE_PART_1_SE_NW = 14467,
    SPR_MINI_GOLF_HOLE_A_BASE_PART_2_SE_NW = 14468,
    SPR_MINI_GOLF_HOLE_A_TRIM_PART_1_SW_NE = 14469,
    SPR_MINI_GOLF_HOLE_A_TRIM_PART_2_SW_NE = 14470,
    SPR_MINI_GOLF_HOLE_A_TRIM_PART_1_NW_SE = 14471,
    SPR_MINI_GOLF_HOLE_A_TRIM_PART_2_NW_SE = 14472,
    SPR_MINI_GOLF_HOLE_A_TRIM_PART_1_NE_SW = 14473,
    SPR_MINI_GOLF_HOLE_A_TRIM_PART_2_NE_SW = 14474,
    SPR_MINI_GOLF_HOLE_A_TRIM_PART_1_SE_NW = 14475,
    SPR_MINI_GOLF_HOLE_A_TRIM_PART_2_SE_NW = 14476,

    SPR_MINI_GOLF_HOLE_B_BASE_PART_1_SW_NE = 14477,
    SPR_MINI_GOLF_HOLE_B_BASE_PART_2_SW_NE = 14478,
    SPR_MINI_GOLF_HOLE_B_BASE_PART_1_NW_SE = 14479,
    SPR_MINI_GOLF_HOLE_B_BASE_PART_2_NW_SE = 14480,
    SPR_MINI_GOLF_HOLE_B_BASE_PART_1_NE_SW = 14481,
    SPR_MINI_GOLF_HOLE_B_BASE_PART_2_NE_SW = 14482,
    SPR_MINI_GOLF_HOLE_B_BASE_PART_1_SE_NW = 14483,
    SPR_MINI_GOLF_HOLE_B_BASE_PART_2_SE_NW = 14484,
    SPR_MINI_GOLF_HOLE_B_TRIM_PART_1_SW_NE = 14485,
    SPR_MINI_GOLF_HOLE_B_TRIM_PART_2_SW_NE = 14486,
    SPR_MINI_GOLF_HOLE_B_TRIM_PART_1_NW_SE = 14487,
    SPR_MINI_GOLF_HOLE_B_TRIM_PART_2_NW_SE = 14488,
    SPR_MINI_GOLF_HOLE_B_TRIM_PART_1_NE_SW = 14489,
    SPR_MINI_GOLF_HOLE_B_TRIM_PART_2_NE_SW = 14490,
    SPR_MINI_GOLF_HOLE_B_TRIM_PART_1_SE_NW = 14491,
    SPR_MINI_GOLF_HOLE_B_TRIM_PART_2_SE_NW = 14492,

    SPR_MINI_GOLF_HOLE_C_BASE_PART_1_SW_NE = 14493,
    SPR_MINI_GOLF_HOLE_C_BASE_PART_2_SW_NE = 14494,
    SPR_MINI_GOLF_HOLE_C_BASE_PART_1_NW_SE = 14495,
    SPR_MINI_GOLF_HOLE_C_BASE_PART_2_NW_SE = 14496,
    SPR_MINI_GOLF_HOLE_C_BASE_PART_1_NE_SW = 14497,
    SPR_MINI_GOLF_HOLE_C_BASE_PART_2_NE_SW = 14498,
    SPR_MINI_GOLF_HOLE_C_BASE_PART_1_SE_NW = 14499,
    SPR_MINI_GOLF_HOLE_C_BASE_PART_2_SE_NW = 14500,
    SPR_MINI_GOLF_HOLE_C_TRIM_PART_1_SW_NE = 14501,
    SPR_MINI_GOLF_HOLE_C_TRIM_PART_2_SW_NE = 14502,
    SPR_MINI_GOLF_HOLE_C_TRIM_PART_1_NW_SE = 14503,
    SPR_MINI_GOLF_HOLE_C_TRIM_PART_2_NW_SE = 14504,
    SPR_MINI_GOLF_HOLE_C_TRIM_PART_1_NE_SW = 14505,
    SPR_MINI_GOLF_HOLE_C_TRIM_PART_2_NE_SW = 14506,
    SPR_MINI_GOLF_HOLE_C_TRIM_PART_1_SE_NW = 14507,
    SPR_MINI_GOLF_HOLE_C_TRIM_PART_2_SE_NW = 14508,

    SPR_MINI_GOLF_HOLE_D_BASE_PART_1_SW_SE = 14509,
    SPR_MINI_GOLF_HOLE_D_BASE_PART_2_SW_SE = 14510,
    SPR_MINI_GOLF_HOLE_D_BASE_PART_3_SW_SE = 14511,
    SPR_MINI_GOLF_HOLE_D_BASE_PART_1_NW_SW = 14512,
    SPR_MINI_GOLF_HOLE_D_BASE_PART_2_NW_SW = 14513,
    SPR_MINI_GOLF_HOLE_D_BASE_PART_3_NW_SW = 14514,
    SPR_MINI_GOLF_HOLE_D_BASE_PART_1_NE_NW = 14515,
    SPR_MINI_GOLF_HOLE_D_BASE_PART_2_NE_NW = 14516,
    SPR_MINI_GOLF_HOLE_D_BASE_PART_3_NE_NW = 14517,
    SPR_MINI_GOLF_HOLE_D_BASE_PART_1_SE_NE = 14518,
    SPR_MINI_GOLF_HOLE_D_BASE_PART_2_SE_NE = 14519,
    SPR_MINI_GOLF_HOLE_D_BASE_PART_3_SE_NE = 14520,
    SPR_MINI_GOLF_HOLE_D_TRIM_PART_1_SW_SE = 14521,
    SPR_MINI_GOLF_HOLE_D_TRIM_PART_2_SW_SE = 14522,
    SPR_MINI_GOLF_HOLE_D_TRIM_PART_3_SW_SE = 14523,
    SPR_MINI_GOLF_HOLE_D_TRIM_PART_1_NW_SW = 14524,
    SPR_MINI_GOLF_HOLE_D_TRIM_PART_2_NW_SW = 14525,
    SPR_MINI_GOLF_HOLE_D_TRIM_PART_3_NW_SW = 14526,
    SPR_MINI_GOLF_HOLE_D_TRIM_PART_1_NE_NW = 14527,
    SPR_MINI_GOLF_HOLE_D_TRIM_PART_2_NE_NW = 14528,
    SPR_MINI_GOLF_HOLE_D_TRIM_PART_3_NE_NW = 14529,
    SPR_MINI_GOLF_HOLE_D_TRIM_PART_1_SE_NE = 14530,
    SPR_MINI_GOLF_HOLE_D_TRIM_PART_2_SE_NE = 14531,
    SPR_MINI_GOLF_HOLE_D_TRIM_PART_3_SE_NE = 14532,

    SPR_MINI_GOLF_HOLE_E_BASE_PART_1_SE_SW = 14533,
    SPR_MINI_GOLF_HOLE_E_BASE_PART_2_SE_SW = 14534,
    SPR_MINI_GOLF_HOLE_E_BASE_PART_3_SE_SW = 14535,
    SPR_MINI_GOLF_HOLE_E_BASE_PART_1_NE_SE = 14536,
    SPR_MINI_GOLF_HOLE_E_BASE_PART_2_NE_SE = 14537,
    SPR_MINI_GOLF_HOLE_E_BASE_PART_3_NE_SE = 14538,
    SPR_MINI_GOLF_HOLE_E_BASE_PART_1_NW_NE = 14539,
    SPR_MINI_GOLF_HOLE_E_BASE_PART_2_NW_NE = 14540,
    SPR_MINI_GOLF_HOLE_E_BASE_PART_3_NW_NE = 14541,
    SPR_MINI_GOLF_HOLE_E_BASE_PART_1_SW_NW = 14542,
    SPR_MINI_GOLF_HOLE_E_BASE_PART_2_SW_NW = 14543,
    SPR_MINI_GOLF_HOLE_E_BASE_PART_3_SW_NW = 14544,
    SPR_MINI_GOLF_HOLE_E_TRIM_PART_1_SE_SW = 14545,
    SPR_MINI_GOLF_HOLE_E_TRIM_PART_2_SE_SW = 14546,
    SPR_MINI_GOLF_HOLE_E_TRIM_PART_3_SE_SW = 14547,
    SPR_MINI_GOLF_HOLE_E_TRIM_PART_1_NE_SE = 14548,
    SPR_MINI_GOLF_HOLE_E_TRIM_PART_2_NE_SE = 14549,
    SPR_MINI_GOLF_HOLE_E_TRIM_PART_3_NE_SE = 14550,
    SPR_MINI_GOLF_HOLE_E_TRIM_PART_1_NW_NE = 14551,
    SPR_MINI_GOLF_HOLE_E_TRIM_PART_2_NW_NE = 14552,
    SPR_MINI_GOLF_HOLE_E_TRIM_PART_3_NW_NE = 14553,
    SPR_MINI_GOLF_HOLE_E_TRIM_PART_1_SW_NW = 14554,
    SPR_MINI_GOLF_HOLE_E_TRIM_PART_2_SW_NW = 14555,
    SPR_MINI_GOLF_HOLE_E_TRIM_PART_3_SW_NW = 14556,
};

static const uint32 mini_golf_track_sprites_25_deg_up[][3] = {
    {
        SPR_MINI_GOLF_25_DEG_UP_SW_NE,
        SPR_MINI_GOLF_25_DEG_UP_FENCE_BACK_SW_NE,
        SPR_MINI_GOLF_25_DEG_UP_FENCE_FRONT_SW_NE,
    },
    {
        SPR_MINI_GOLF_25_DEG_UP_NW_SE,
        SPR_MINI_GOLF_25_DEG_UP_FENCE_BACK_NW_SE,
        SPR_MINI_GOLF_25_DEG_UP_FENCE_FRONT_NW_SE,
    },
    {
        SPR_MINI_GOLF_25_DEG_UP_NE_SW,
        SPR_MINI_GOLF_25_DEG_UP_FENCE_BACK_NE_SW,
        SPR_MINI_GOLF_25_DEG_UP_FENCE_FRONT_NE_SW,
    },
    {
        SPR_MINI_GOLF_25_DEG_UP_SE_NW,
        SPR_MINI_GOLF_25_DEG_UP_FENCE_BACK_SE_NW,
        SPR_MINI_GOLF_25_DEG_UP_FENCE_FRONT_SE_NW,
    }
};

static const uint32 mini_golf_track_sprites_flat_to_25_deg_up[][3] = {
    {
        SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_SW_NE,
        SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_BACK_SW_NE,
        SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_FRONT_SW_NE,
    },
    {
        SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_NW_SE,
        SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_BACK_NW_SE,
        SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_FRONT_NW_SE,
    },
    {
        SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_NE_SW,
        SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_BACK_NE_SW,
        SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_FRONT_NE_SW,
    },
    {
        SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_SE_NW,
        SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_BACK_SE_NW,
        SPR_MINI_GOLF_FLAT_TO_25_DEG_UP_FENCE_FRONT_SE_NW,
    }
};


static const uint32 mini_golf_track_sprites_25_deg_up_to_flat[][3] = {
    {
        SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_SW_NE,
        SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_BACK_SW_NE,
        SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_FRONT_SW_NE,
    },
    {
        SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_NW_SE,
        SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_BACK_NW_SE,
        SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_FRONT_NW_SE,
    },
    {
        SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_NE_SW,
        SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_BACK_NE_SW,
        SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_FRONT_NE_SW,
    },
    {
        SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_SE_NW,
        SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_BACK_SE_NW,
        SPR_MINI_GOLF_25_DEG_UP_TO_FLAT_FENCE_FRONT_SE_NW,
    }
};

static const uint32 mini_golf_track_sprites_quarter_turn_1_tile[] = {
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_SW_NW,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_NW_NE,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_NE_SE,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_SE_SW,
};

static const uint32 mini_golf_track_sprites_quarter_turn_1_tile_fence_front[] = {
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_FRONT_SW_NW,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_FRONT_NW_NE,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_FRONT_NE_SE,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_FRONT_SE_SW,
};

static const uint32 mini_golf_track_sprites_hole_a[4][2][2] = {
    {
        {SPR_MINI_GOLF_HOLE_A_BASE_PART_1_SW_NE, SPR_MINI_GOLF_HOLE_A_TRIM_PART_1_SW_NE},
        {SPR_MINI_GOLF_HOLE_A_BASE_PART_2_SW_NE, SPR_MINI_GOLF_HOLE_A_TRIM_PART_2_SW_NE}
    },
    {
        {SPR_MINI_GOLF_HOLE_A_BASE_PART_1_NW_SE, SPR_MINI_GOLF_HOLE_A_TRIM_PART_1_NW_SE},
        {SPR_MINI_GOLF_HOLE_A_BASE_PART_2_NW_SE, SPR_MINI_GOLF_HOLE_A_TRIM_PART_2_NW_SE}
    },
    {
        {SPR_MINI_GOLF_HOLE_A_BASE_PART_1_NE_SW, SPR_MINI_GOLF_HOLE_A_TRIM_PART_1_NE_SW},
        {SPR_MINI_GOLF_HOLE_A_BASE_PART_2_NE_SW, SPR_MINI_GOLF_HOLE_A_TRIM_PART_2_NE_SW}
    },
    {
        {SPR_MINI_GOLF_HOLE_A_BASE_PART_1_SE_NW, SPR_MINI_GOLF_HOLE_A_TRIM_PART_1_SE_NW},
        {SPR_MINI_GOLF_HOLE_A_BASE_PART_2_SE_NW, SPR_MINI_GOLF_HOLE_A_TRIM_PART_2_SE_NW}
    },
};

static const uint32 mini_golf_track_sprites_hole_b[4][2][2] = {
    {
        {SPR_MINI_GOLF_HOLE_B_BASE_PART_1_SW_NE, SPR_MINI_GOLF_HOLE_B_TRIM_PART_1_SW_NE},
        {SPR_MINI_GOLF_HOLE_B_BASE_PART_2_SW_NE, SPR_MINI_GOLF_HOLE_B_TRIM_PART_2_SW_NE}
    },
    {
        {SPR_MINI_GOLF_HOLE_B_BASE_PART_1_NW_SE, SPR_MINI_GOLF_HOLE_B_TRIM_PART_1_NW_SE},
        {SPR_MINI_GOLF_HOLE_B_BASE_PART_2_NW_SE, SPR_MINI_GOLF_HOLE_B_TRIM_PART_2_NW_SE}
    },
    {
        {SPR_MINI_GOLF_HOLE_B_BASE_PART_1_NE_SW, SPR_MINI_GOLF_HOLE_B_TRIM_PART_1_NE_SW},
        {SPR_MINI_GOLF_HOLE_B_BASE_PART_2_NE_SW, SPR_MINI_GOLF_HOLE_B_TRIM_PART_2_NE_SW}
    },
    {
        {SPR_MINI_GOLF_HOLE_B_BASE_PART_1_SE_NW, SPR_MINI_GOLF_HOLE_B_TRIM_PART_1_SE_NW},
        {SPR_MINI_GOLF_HOLE_B_BASE_PART_2_SE_NW, SPR_MINI_GOLF_HOLE_B_TRIM_PART_2_SE_NW}
    },
};

static const uint32 mini_golf_track_sprites_hole_c[][2][2] = {
    {
        {SPR_MINI_GOLF_HOLE_C_BASE_PART_1_SW_NE, SPR_MINI_GOLF_HOLE_C_TRIM_PART_1_SW_NE},
        {SPR_MINI_GOLF_HOLE_C_BASE_PART_2_SW_NE, SPR_MINI_GOLF_HOLE_C_TRIM_PART_2_SW_NE}
    },
    {
        {SPR_MINI_GOLF_HOLE_C_BASE_PART_1_NW_SE, SPR_MINI_GOLF_HOLE_C_TRIM_PART_1_NW_SE},
        {SPR_MINI_GOLF_HOLE_C_BASE_PART_2_NW_SE, SPR_MINI_GOLF_HOLE_C_TRIM_PART_2_NW_SE}
    },
    {
        {SPR_MINI_GOLF_HOLE_C_BASE_PART_1_NE_SW, SPR_MINI_GOLF_HOLE_C_TRIM_PART_1_NE_SW},
        {SPR_MINI_GOLF_HOLE_C_BASE_PART_2_NE_SW, SPR_MINI_GOLF_HOLE_C_TRIM_PART_2_NE_SW}
    },
    {
        {SPR_MINI_GOLF_HOLE_C_BASE_PART_1_SE_NW, SPR_MINI_GOLF_HOLE_C_TRIM_PART_1_SE_NW},
        {SPR_MINI_GOLF_HOLE_C_BASE_PART_2_SE_NW, SPR_MINI_GOLF_HOLE_C_TRIM_PART_2_SE_NW}
    },
};

static const uint32 mini_golf_track_sprites_hole_d[][3][2] = {
    {
        {SPR_MINI_GOLF_HOLE_D_BASE_PART_1_SW_SE, SPR_MINI_GOLF_HOLE_D_TRIM_PART_1_SW_SE},
        {SPR_MINI_GOLF_HOLE_D_BASE_PART_2_SW_SE, SPR_MINI_GOLF_HOLE_D_TRIM_PART_2_SW_SE},
        {SPR_MINI_GOLF_HOLE_D_BASE_PART_3_SW_SE, SPR_MINI_GOLF_HOLE_D_TRIM_PART_3_SW_SE}
    },
    {
        {SPR_MINI_GOLF_HOLE_D_BASE_PART_1_NW_SW, SPR_MINI_GOLF_HOLE_D_TRIM_PART_1_NW_SW},
        {SPR_MINI_GOLF_HOLE_D_BASE_PART_2_NW_SW, SPR_MINI_GOLF_HOLE_D_TRIM_PART_2_NW_SW},
        {SPR_MINI_GOLF_HOLE_D_BASE_PART_3_NW_SW, SPR_MINI_GOLF_HOLE_D_TRIM_PART_3_NW_SW}
    },
    {
        {SPR_MINI_GOLF_HOLE_D_BASE_PART_1_NE_NW, SPR_MINI_GOLF_HOLE_D_TRIM_PART_1_NE_NW},
        {SPR_MINI_GOLF_HOLE_D_BASE_PART_2_NE_NW, SPR_MINI_GOLF_HOLE_D_TRIM_PART_2_NE_NW},
        {SPR_MINI_GOLF_HOLE_D_BASE_PART_3_NE_NW, SPR_MINI_GOLF_HOLE_D_TRIM_PART_3_NE_NW}
    },
    {
        {SPR_MINI_GOLF_HOLE_D_BASE_PART_1_SE_NE, SPR_MINI_GOLF_HOLE_D_TRIM_PART_1_SE_NE},
        {SPR_MINI_GOLF_HOLE_D_BASE_PART_2_SE_NE, SPR_MINI_GOLF_HOLE_D_TRIM_PART_2_SE_NE},
        {SPR_MINI_GOLF_HOLE_D_BASE_PART_3_SE_NE, SPR_MINI_GOLF_HOLE_D_TRIM_PART_3_SE_NE}
    },
};

static const uint32 mini_golf_track_sprites_hole_e[][3][2] = {
    {
        {SPR_MINI_GOLF_HOLE_E_BASE_PART_1_SW_NW, SPR_MINI_GOLF_HOLE_E_TRIM_PART_1_SW_NW},
        {SPR_MINI_GOLF_HOLE_E_BASE_PART_2_SW_NW, SPR_MINI_GOLF_HOLE_E_TRIM_PART_2_SW_NW},
        {SPR_MINI_GOLF_HOLE_E_BASE_PART_3_SW_NW, SPR_MINI_GOLF_HOLE_E_TRIM_PART_3_SW_NW}
    },
    {
        {SPR_MINI_GOLF_HOLE_E_BASE_PART_1_NW_NE, SPR_MINI_GOLF_HOLE_E_TRIM_PART_1_NW_NE},
        {SPR_MINI_GOLF_HOLE_E_BASE_PART_2_NW_NE, SPR_MINI_GOLF_HOLE_E_TRIM_PART_2_NW_NE},
        {SPR_MINI_GOLF_HOLE_E_BASE_PART_3_NW_NE, SPR_MINI_GOLF_HOLE_E_TRIM_PART_3_NW_NE}
    },
    {
        {SPR_MINI_GOLF_HOLE_E_BASE_PART_1_NE_SE, SPR_MINI_GOLF_HOLE_E_TRIM_PART_1_NE_SE},
        {SPR_MINI_GOLF_HOLE_E_BASE_PART_2_NE_SE, SPR_MINI_GOLF_HOLE_E_TRIM_PART_2_NE_SE},
        {SPR_MINI_GOLF_HOLE_E_BASE_PART_3_NE_SE, SPR_MINI_GOLF_HOLE_E_TRIM_PART_3_NE_SE}
    },
    {
        {SPR_MINI_GOLF_HOLE_E_BASE_PART_1_SE_SW, SPR_MINI_GOLF_HOLE_E_TRIM_PART_1_SE_SW},
        {SPR_MINI_GOLF_HOLE_E_BASE_PART_2_SE_SW, SPR_MINI_GOLF_HOLE_E_TRIM_PART_2_SE_SW},
        {SPR_MINI_GOLF_HOLE_E_BASE_PART_3_SE_SW, SPR_MINI_GOLF_HOLE_E_TRIM_PART_3_SE_SW}
    },
};

/** rct2: 0x00933471 */
static const uint8 mini_golf_peep_animation_frames_walk[] = {0, 1, 2, 3, 4, 5};

/** rct2: 0x00933478 */
static const uint8 mini_golf_peep_animation_frames_place_ball_downwards[] = {12, 13, 14, 15};

/** rct2: 0x009334B5 */
static const uint8 mini_golf_peep_animation_frames_swing[] = {31, 31, 31, 31, 31, 31, 31, 31, 31, 32, 33, 33, 33, 33, 34};

/** rct2: 0x0093347D */
static const uint8 mini_golf_peep_animation_frames_swing_left[] = {6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 8, 8, 8, 9};

/** rct2: 0x0093348D */
static const uint8 mini_golf_peep_animation_frames_place_ball_upwards[] = {12, 13, 14, 15, 14, 13, 12};

/** rct2: 0x00933495 */
static const uint8 mini_golf_peep_animation_frames_jump[] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};

/** rct2: 0x009334A5 */
static const uint8 mini_golf_peep_animation_frames_pickup_ball[] = {15, 14, 13, 12};

/** rct2: 0x009334C5 */
static const uint8 mini_golf_peep_animation_frames_put[] = {35, 36, 36, 36, 36, 36, 35, 35, 35, 35};

/** rct2: 0x009334AA */
static const uint8 mini_golf_peep_animation_frames_put_left[] = {10, 11, 11, 11, 11, 11, 10, 10, 10, 10};

/** rct2: 0x008B8F74 */
static const uint8 * mini_golf_peep_animation_frames[] = {
    mini_golf_peep_animation_frames_walk,
    mini_golf_peep_animation_frames_place_ball_downwards,
    mini_golf_peep_animation_frames_swing_left,
    mini_golf_peep_animation_frames_place_ball_upwards,
    mini_golf_peep_animation_frames_jump,
    mini_golf_peep_animation_frames_pickup_ball,
    mini_golf_peep_animation_frames_put_left,
    mini_golf_peep_animation_frames_swing,
    mini_golf_peep_animation_frames_put,
};

const uint8 mini_golf_peep_animation_lengths[] = {
    countof(mini_golf_peep_animation_frames_walk),
    countof(mini_golf_peep_animation_frames_place_ball_downwards),
    countof(mini_golf_peep_animation_frames_swing_left),
    countof(mini_golf_peep_animation_frames_place_ball_upwards),
    countof(mini_golf_peep_animation_frames_jump),
    countof(mini_golf_peep_animation_frames_pickup_ball),
    countof(mini_golf_peep_animation_frames_put_left),
    countof(mini_golf_peep_animation_frames_swing),
    countof(mini_golf_peep_animation_frames_put),
};

static paint_struct * mini_golf_paint_util_7c(
    uint8 direction,
    uint32 image_id,
    sint8 x_offset, sint8 y_offset,
    sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
    sint16 z_offset,
    sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z,
    uint32 rotation
)
{
    if (direction & 1) {
        return sub_98197C(image_id, y_offset, x_offset, bound_box_length_y, bound_box_length_x, bound_box_length_z, z_offset, bound_box_offset_y, bound_box_offset_x, bound_box_offset_z, rotation);
    }

    return sub_98197C(image_id, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset, bound_box_offset_x, bound_box_offset_y, bound_box_offset_z, rotation);
}

static bool mini_golf_paint_util_should_draw_fence(paint_session * session, rct_map_element * mapElement)
{
    if (!session->DidPassSurface) {
        // Should be above ground (have passed surface rendering)
        return false;
    }

    rct_map_element * surfaceElement = session->SurfaceElement;
    if (surfaceElement->base_height != mapElement->base_height) {
        return true;
    }

    if (surfaceElement->properties.surface.slope & 0x1F) {
        return true;
    }

    return false;
}

/** rct2: 0x0087F10C */
static void paint_mini_golf_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    if (direction & 1) {
        imageId = SPR_MINI_GOLF_FLAT_NW_SE | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 20, 32, 1, height, 6, 0, height, get_current_rotation());
        paint_util_push_tunnel_right(session, height, TUNNEL_10);
    } else {
        imageId = SPR_MINI_GOLF_FLAT_SW_NE | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 32, 20, 1, height, 0, 6, height, get_current_rotation());
        paint_util_push_tunnel_left(session, height, TUNNEL_10);
    }

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

    if (mini_golf_paint_util_should_draw_fence(session, mapElement)) {
        if (direction & 1) {
            imageId = SPR_MINI_GOLF_FLAT_FENCE_BACK_NW_SE | gTrackColours[SCHEME_MISC];
            sub_98197C(imageId, 0, 0, 1, 32, 7, height, 10, 0, height + 2, get_current_rotation());

            imageId = SPR_MINI_GOLF_FLAT_FENCE_FRONT_NW_SE | gTrackColours[SCHEME_MISC];
            sub_98197C(imageId, 0, 0, 1, 32, 7, height, 22, 0, height + 2, get_current_rotation());
        } else {
            imageId = SPR_MINI_GOLF_FLAT_FENCE_BACK_SW_NE | gTrackColours[SCHEME_MISC];
            sub_98197C(imageId, 0, 0, 32, 1, 7, height, 0, 10, height + 2, get_current_rotation());

            imageId = SPR_MINI_GOLF_FLAT_FENCE_FRONT_SW_NE | gTrackColours[SCHEME_MISC];
            sub_98197C(imageId, 0, 0, 32, 1, 7, height, 0, 22, height + 2, get_current_rotation());
        }
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0087F11C */
static void paint_mini_golf_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    imageId = mini_golf_track_sprites_25_deg_up[direction][0] | gTrackColours[SCHEME_TRACK];
    mini_golf_paint_util_7c(direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, get_current_rotation());

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

    imageId = mini_golf_track_sprites_25_deg_up[direction][1] | gTrackColours[SCHEME_MISC];
    mini_golf_paint_util_7c(direction, imageId, 0, 0, 32, 1, 15, height, 0, 10, height + 2, get_current_rotation());

    imageId = mini_golf_track_sprites_25_deg_up[direction][2] | gTrackColours[SCHEME_MISC];
    mini_golf_paint_util_7c(direction, imageId, 0, 0, 32, 1, 15, height, 0, 22, height + 2, get_current_rotation());

    switch (direction) {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
            break;
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x0087F12C */
static void paint_mini_golf_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    imageId = mini_golf_track_sprites_flat_to_25_deg_up[direction][0] | gTrackColours[SCHEME_TRACK];
    mini_golf_paint_util_7c(direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, get_current_rotation());

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

    imageId = mini_golf_track_sprites_flat_to_25_deg_up[direction][1] | gTrackColours[SCHEME_MISC];
    mini_golf_paint_util_7c(direction, imageId, 0, 0, 32, 1, 11, height, 0, 10, height + 2, get_current_rotation());

    imageId = mini_golf_track_sprites_flat_to_25_deg_up[direction][2] | gTrackColours[SCHEME_MISC];
    mini_golf_paint_util_7c(direction, imageId, 0, 0, 32, 1, 11, height, 0, 22, height + 2, get_current_rotation());

    switch (direction) {
        case 0:
            paint_util_push_tunnel_left(session, height, TUNNEL_10);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height, TUNNEL_10);
            break;
    }

    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x0087F13C */
static void paint_mini_golf_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    imageId = mini_golf_track_sprites_25_deg_up_to_flat[direction][0] | gTrackColours[SCHEME_TRACK];
    mini_golf_paint_util_7c(direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, get_current_rotation());

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

    imageId = mini_golf_track_sprites_25_deg_up_to_flat[direction][1] | gTrackColours[SCHEME_MISC];
    mini_golf_paint_util_7c(direction, imageId, 0, 0, 32, 1, 11, height, 0, 10, height + 2, get_current_rotation());

    imageId = mini_golf_track_sprites_25_deg_up_to_flat[direction][2] | gTrackColours[SCHEME_MISC];
    mini_golf_paint_util_7c(direction, imageId, 0, 0, 32, 1, 11, height, 0, 22, height + 2, get_current_rotation());

    switch (direction) {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_10);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_10);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_0);
            break;
    }

    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x0087F14C */
static void paint_mini_golf_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_mini_golf_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x0087F15C */
static void paint_mini_golf_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_mini_golf_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x0087F16C */
static void paint_mini_golf_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_mini_golf_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x0087F17C, 0x0087F18C, 0x0087F19C */
static void paint_mini_golf_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = session->MapPosition;
    rct_ride * ride = get_ride(rideIndex);
    const rct_ride_entrance_definition * entranceStyle = &RideEntranceDefinitions[ride->entrance_style];
    uint32 imageId;
    bool hasFence;

    imageId = SPR_MINI_GOLF_STATION_FLOOR | gTrackColours[SCHEME_TRACK];
    mini_golf_paint_util_7c(direction, imageId, 0, 0, 32, 28, 1, height, 0, 0, height, get_current_rotation());

    if (direction & 1) {
        hasFence = track_paint_util_has_fence(EDGE_NE, position, mapElement, ride, get_current_rotation());
        if (hasFence) {
            imageId = SPR_MINI_GOLF_FLAT_FENCE_BACK_NW_SE | gTrackColours[SCHEME_MISC];
            sub_98197C(imageId, -10, 0, 1, 32, 7, height, 0, 0, height + 2, get_current_rotation());
        }

        bool hasSWFence = track_paint_util_has_fence(EDGE_SW, position, mapElement, ride, get_current_rotation());
        if (hasFence) {
            imageId = SPR_MINI_GOLF_FLAT_FENCE_FRONT_NW_SE | gTrackColours[SCHEME_MISC];
            sub_98197C(imageId, 10, 0, 1, 32, 7, height, 31, 0, height + 2, get_current_rotation());
        }

        track_paint_util_draw_station_covers(session, EDGE_NE, hasFence, entranceStyle, direction, height);
        track_paint_util_draw_station_covers(session, EDGE_SW, hasSWFence, entranceStyle, direction, height);

        // Was leftwards tunnel in game, seems odd
        paint_util_push_tunnel_right(session, height, TUNNEL_6);
    } else {
        hasFence = track_paint_util_has_fence(EDGE_NW, position, mapElement, ride, get_current_rotation());
        if (hasFence) {
            imageId = SPR_MINI_GOLF_FLAT_FENCE_BACK_SW_NE | gTrackColours[SCHEME_MISC];
            sub_98197C(imageId, 0, -10, 32, 1, 7, height, 0, 0, height + 2, get_current_rotation());
        }

        bool hasSEFence = track_paint_util_has_fence(EDGE_SE, position, mapElement, ride, get_current_rotation());
        if (hasFence) {
            imageId = SPR_MINI_GOLF_FLAT_FENCE_FRONT_SW_NE | gTrackColours[SCHEME_MISC];
            sub_98197C(imageId, 0, 10, 32, 1, 7, height, 0, 31, height + 2, get_current_rotation());
        }

        track_paint_util_draw_station_covers(session, EDGE_NW, hasFence, entranceStyle, direction, height);
        track_paint_util_draw_station_covers(session, EDGE_SE, hasSEFence, entranceStyle, direction, height);

        paint_util_push_tunnel_left(session, height, TUNNEL_6);
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0087F1AC */
static void paint_mini_golf_track_left_quarter_turn_1_tile(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    track_paint_util_left_quarter_turn_1_tile_paint(session, 1, height, 0, direction, gTrackColours[SCHEME_TRACK], mini_golf_track_sprites_quarter_turn_1_tile, get_current_rotation());

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, gTrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0, direction), 0xFFFF, 0);

    const bool shouldDrawFence = mini_golf_paint_util_should_draw_fence(session, mapElement);

    switch (direction) {
        case 0:
            paint_util_push_tunnel_left(session, height, TUNNEL_10);
            if (!shouldDrawFence) break;

            imageId = SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_BACK_SW_NW | gTrackColours[SCHEME_MISC];
            sub_98199C(imageId, 0, 0, 26, 24, 1, height, 6, 2, height, get_current_rotation());

            break;

        case 1:
            if (!shouldDrawFence) break;

            imageId = SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_BACK_NW_NE | gTrackColours[SCHEME_MISC];
            sub_98199C(imageId, 0, 0, 26, 26, 1, height, 0, 0, height, get_current_rotation());
            break;

        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_10);
            if (!shouldDrawFence) break;

            imageId = SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_BACK_NE_SE | gTrackColours[SCHEME_MISC];
            sub_98199C(imageId, 0, 0, 24, 26, 1, height, 2, 6, height, get_current_rotation());
            break;

        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_10);
            paint_util_push_tunnel_right(session, height, TUNNEL_10);
            if (!shouldDrawFence) break;

            imageId = SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_BACK_SE_SW | gTrackColours[SCHEME_MISC];
            sub_98199C(imageId, 0, 0, 24, 24, 1, height, 6, 6, height, get_current_rotation());
            break;
    }

    if (shouldDrawFence) {
        // TODO: The back fence uses the same x/y offsets, but uses another paint function. See if this occurs more often.
        track_paint_util_left_quarter_turn_1_tile_paint(session, 0, height, 24, direction, gTrackColours[SCHEME_MISC], mini_golf_track_sprites_quarter_turn_1_tile_fence_front, get_current_rotation());

        switch (direction) {
            case 0:
                imageId = SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_INSIDE_SW_NW | gTrackColours[SCHEME_MISC];
                sub_98197C(imageId, 0, 0, 5, 5, 5, height, 24, 0, height + 2, get_current_rotation());
                break;
            case 2:
                imageId = SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_INSIDE_NE_SE | gTrackColours[SCHEME_MISC];
                sub_98197C(imageId, 0, 0, 5, 5, 5, height, 0, 24, height + 2, get_current_rotation());
                break;
        }
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0087F1BC */
static void paint_mini_golf_track_right_quarter_turn_1_tile(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_mini_golf_track_left_quarter_turn_1_tile(session, rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

static void paint_mini_golf_hole_ab(paint_session * session, uint8 trackSequence, uint8 direction, sint32 height, const uint32 sprites[4][2][2])
{
    uint32 imageId;

    bool drewSupports = wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);

    if ((direction == 0 && trackSequence == 0) || (direction == 2 && trackSequence == 1)) {
        paint_util_push_tunnel_left(session, height, TUNNEL_10);
    } else if ((direction == 3 && trackSequence == 0) || (direction == 1 && trackSequence == 1)) {
        paint_util_push_tunnel_right(session, height, TUNNEL_10);
    }

    rct_xy16 boundBox = (direction & 1) ? (rct_xy16) {26, 32} : (rct_xy16) {32, 26};
    rct_xy16 boundBoxOffset = (direction & 1) ? (rct_xy16) {3, 0} : (rct_xy16) {0, 3};

    imageId = sprites[direction][trackSequence][1] | gTrackColours[SCHEME_TRACK];
    sub_98197C(imageId, 0, 0, boundBox.x, boundBox.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 24, get_current_rotation());

    if (drewSupports) {
        imageId = ((direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS) | gTrackColours[SCHEME_SUPPORTS];
        sub_98197C(imageId, 0, 0, boundBox.x, boundBox.y, 1, height, boundBoxOffset.x, boundBoxOffset.y, height, get_current_rotation());

        imageId = sprites[direction][trackSequence][0] | gTrackColours[SCHEME_TRACK];
        sub_98199C(imageId, 0, 0, boundBox.x, boundBox.y, 1, height, boundBoxOffset.x, boundBoxOffset.y, height, get_current_rotation());
    } else {
        imageId = sprites[direction][trackSequence][0] | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, boundBox.x, boundBox.y, 1, height, boundBoxOffset.x, boundBoxOffset.y, height, get_current_rotation());
    }
}

/** rct2: 0x0087F1CC */
static void paint_mini_golf_hole_a(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_mini_golf_hole_ab(session, trackSequence, direction, height, mini_golf_track_sprites_hole_a);
}

/** rct2: 0x0087F1DC */
static void paint_mini_golf_hole_b(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_mini_golf_hole_ab(session, trackSequence, direction, height, mini_golf_track_sprites_hole_b);
}

/** rct2: 0x0087F1EC */
static void paint_mini_golf_hole_c(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    bool drewSupports = wooden_a_supports_paint_setup(session, (direction & 1), 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);

    if ((direction == 0 && trackSequence == 0) || (direction == 2 && trackSequence == 1)) {
        paint_util_push_tunnel_left(session, height, TUNNEL_10);
    } else if ((direction == 3 && trackSequence == 0) || (direction == 1 && trackSequence == 1)) {
        paint_util_push_tunnel_right(session, height, TUNNEL_10);
    }

    rct_xy16 boundBox = (direction & 1) ? (rct_xy16) {26, 32} : (rct_xy16) {32, 26};
    rct_xy16 boundBoxOffset = (direction & 1) ? (rct_xy16) {3, 0} : (rct_xy16) {0, 3};

    imageId = mini_golf_track_sprites_hole_c[direction][trackSequence][1] | gTrackColours[SCHEME_TRACK];

    switch ((direction << 4) | trackSequence) {
        case 0x01:
        case 0x20:
            sub_98197C(imageId, 0, 0, 2, 26, 3, height, 30, 3, height + 4, get_current_rotation());
            break;
        case 0x10:
        case 0x31:
            sub_98197C(imageId, 0, 0, 26, 2, 3, height, 3, 30, height + 4, get_current_rotation());
            break;
        default:
            sub_98197C(imageId, 0, 0, boundBox.x, boundBox.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 24, get_current_rotation());
            break;
    }

    if (drewSupports) {
        imageId = ((direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS) | gTrackColours[SCHEME_SUPPORTS];
        sub_98197C(imageId, 0, 0, boundBox.x, boundBox.y, 1, height, boundBoxOffset.x, boundBoxOffset.y, height, get_current_rotation());

        imageId = mini_golf_track_sprites_hole_c[direction][trackSequence][0] | gTrackColours[SCHEME_TRACK];
        sub_98199C(imageId, 0, 0, boundBox.x, boundBox.y, 1, height, boundBoxOffset.x, boundBoxOffset.y, height, get_current_rotation());
    } else {
        imageId = mini_golf_track_sprites_hole_c[direction][trackSequence][0] | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, boundBox.x, boundBox.y, 1, height, boundBoxOffset.x, boundBoxOffset.y, height, get_current_rotation());
    }
}

/** rct2: 0x0087F1FC */
static void paint_mini_golf_hole_d(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    sint32 supportType = (direction & 1);
    if (trackSequence == 2) supportType = 1 - supportType;
    bool drewSupports = wooden_a_supports_paint_setup(session, supportType, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);

    switch ((direction << 4) | trackSequence) {
        case 0x00:
        case 0x12:
            paint_util_push_tunnel_left(session, height, TUNNEL_10);
            break;

        case 0x02:
        case 0x30:
            paint_util_push_tunnel_right(session, height, TUNNEL_10);
            break;
    }

    rct_xy16 boundBox = (supportType & 1) ? (rct_xy16) {26, 32} : (rct_xy16) {32, 26};
    rct_xy16 boundBoxOffset = (supportType & 1) ? (rct_xy16) {3, 0} : (rct_xy16) {0, 3};

    imageId = mini_golf_track_sprites_hole_d[direction][trackSequence][1] | gTrackColours[SCHEME_TRACK];

    switch ((direction << 4) | trackSequence) {
        case 0x01:
        case 0x32:
            sub_98197C(imageId, 0, 0, 2, 26, 3, height, 30, 3, height + 4, get_current_rotation());
            break;
        case 0x02:
            sub_98197C(imageId, 0, 0, 23, 2, 3, height, 3, 30, height + 4, get_current_rotation());
            break;
        case 0x10:
            sub_98197C(imageId, 0, 0, 2, 24, 3, height, 30, 3, height + 4, get_current_rotation());
            break;
        case 0x20:
        case 0x31:
            sub_98197C(imageId, 0, 0, 26, 2, 3, height, 3, 30, height + 4, get_current_rotation());
            break;
        default:
            sub_98197C(imageId, 0, 0, boundBox.x, boundBox.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 24, get_current_rotation());
            break;
    }

    switch ((direction << 4) | trackSequence) {
        case 0x02: boundBox = (rct_xy16) {23, 32}; break;
        case 0x10: boundBox = (rct_xy16) {24, 32}; break;
    }

    if (drewSupports) {
        imageId = ((supportType & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS) | gTrackColours[SCHEME_SUPPORTS];
        sub_98197C(imageId, 0, 0, boundBox.x, boundBox.y, 1, height, boundBoxOffset.x, boundBoxOffset.y, height, get_current_rotation());

        imageId = mini_golf_track_sprites_hole_d[direction][trackSequence][0] | gTrackColours[SCHEME_TRACK];
        sub_98199C(imageId, 0, 0, boundBox.x, boundBox.y, 1, height, boundBoxOffset.x, boundBoxOffset.y, height, get_current_rotation());
    } else {
        imageId = mini_golf_track_sprites_hole_d[direction][trackSequence][0] | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, boundBox.x, boundBox.y, 1, height, boundBoxOffset.x, boundBoxOffset.y, height, get_current_rotation());
    }
}

/** rct2: 0x0087F1FC */
static void paint_mini_golf_hole_e(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    sint32 supportType = (direction & 1);
    if (trackSequence == 2) supportType = 1 - supportType;
    bool drewSupports = wooden_a_supports_paint_setup(session, supportType, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);

    switch ((direction << 4) | trackSequence) {
        case 0x00:
        case 0x12:
            paint_util_push_tunnel_left(session, height, TUNNEL_10);
            break;

        case 0x02:
        case 0x30:
            paint_util_push_tunnel_right(session, height, TUNNEL_10);
            break;
    }

    rct_xy16 boundBox = (supportType & 1) ? (rct_xy16) {26, 32} : (rct_xy16) {32, 26};
    rct_xy16 boundBoxOffset = (supportType & 1) ? (rct_xy16) {3, 0} : (rct_xy16) {0, 3};

    imageId = mini_golf_track_sprites_hole_e[direction][trackSequence][1] | gTrackColours[SCHEME_TRACK];

    switch ((direction << 4) | trackSequence) {
        case 0x01:
            sub_98197C(imageId, 0, 0, 2, 26, 3, height, 30, 3, height + 4, get_current_rotation());
            break;
        case 0x02:
        case 0x20:
        case 0x31:
            sub_98197C(imageId, 0, 0, 26, 2, 3, height, 3, 30, height + 4, get_current_rotation());
            break;
        case 0x10:
            sub_98197C(imageId, 0, 0, 2, 24, 3, height, 30, 3, height + 4, get_current_rotation());
            break;
        case 0x32:
            sub_98197C(imageId, 0, 0, 2, 23, 3, height, 30, 3, height + 4, get_current_rotation());
            break;
        default:
            sub_98197C(imageId, 0, 0, boundBox.x, boundBox.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 24, get_current_rotation());
            break;
    }

    switch ((direction << 4) | trackSequence) {
        case 0x10: boundBox = (rct_xy16) {24, 32}; break;
        case 0x32: boundBox = (rct_xy16) {32, 23}; break;
    }

    if (drewSupports) {
        imageId = ((supportType & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS) | gTrackColours[SCHEME_SUPPORTS];
        sub_98197C(imageId, 0, 0, boundBox.x, boundBox.y, 1, height, boundBoxOffset.x, boundBoxOffset.y, height, get_current_rotation());

        imageId = mini_golf_track_sprites_hole_e[direction][trackSequence][0] | gTrackColours[SCHEME_TRACK];
        sub_98199C(imageId, 0, 0, boundBox.x, boundBox.y, 1, height, boundBoxOffset.x, boundBoxOffset.y, height, get_current_rotation());
    } else {
        imageId = mini_golf_track_sprites_hole_e[direction][trackSequence][0] | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, boundBox.x, boundBox.y, 1, height, boundBoxOffset.x, boundBoxOffset.y, height, get_current_rotation());
    }
}

/**
 * rct2: 0x0087EDC4
 */
TRACK_PAINT_FUNCTION get_track_paint_function_mini_golf(sint32 trackType, sint32 direction) {
    switch (trackType) {
        case TRACK_ELEM_FLAT:
            return paint_mini_golf_track_flat;

        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
        case TRACK_ELEM_END_STATION:
            return paint_mini_golf_station;

        case TRACK_ELEM_25_DEG_UP:
            return paint_mini_golf_track_25_deg_up;
        case TRACK_ELEM_FLAT_TO_25_DEG_UP:
            return paint_mini_golf_track_flat_to_25_deg_up;
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            return paint_mini_golf_track_25_deg_up_to_flat;

        case TRACK_ELEM_25_DEG_DOWN:
            return paint_mini_golf_track_25_deg_down;
        case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
            return paint_mini_golf_track_flat_to_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
            return paint_mini_golf_track_25_deg_down_to_flat;

        case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
            return paint_mini_golf_track_left_quarter_turn_1_tile;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
            return paint_mini_golf_track_right_quarter_turn_1_tile;

        case TRACK_ELEM_MINI_GOLF_HOLE_A:
            return paint_mini_golf_hole_a;
        case TRACK_ELEM_MINI_GOLF_HOLE_B:
            return paint_mini_golf_hole_b;
        case TRACK_ELEM_MINI_GOLF_HOLE_C:
            return paint_mini_golf_hole_c;
        case TRACK_ELEM_MINI_GOLF_HOLE_D:
            return paint_mini_golf_hole_d;
        case TRACK_ELEM_MINI_GOLF_HOLE_E:
            return paint_mini_golf_hole_e;
    }

    return NULL;
}

/**
 * rct2: 0x006D42F0
 */
void vehicle_visual_mini_golf_player(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle)
{
    if (vehicle->num_peeps == 0) {
        return;
    }

    rct_drawpixelinfo *edi = session->Unk140E9A8;
    if (edi->zoom_level >= 2) {
        return;
    }

    if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_PEEPS) {
        return;
    }

    rct_ride_entry *rideEntry = get_ride_entry(get_ride(vehicle->ride)->subtype);
    rct_sprite *sprite = get_sprite(vehicle->peep[0]);

    uint8 frame = mini_golf_peep_animation_frames[vehicle->mini_golf_current_animation][vehicle->var_C5];
    uint32 ebx = (frame << 2) + (imageDirection >> 3);

    uint32 image_id = rideEntry->vehicles[0].base_image_id + 1 + ebx;
    uint32 peep_palette = sprite->peep.tshirt_colour << 19 | sprite->peep.trousers_colour << 24 | 0x0A0000000;
    sub_98197C(image_id | peep_palette, 0, 0, 1, 1, 11, z, 0, 0, z + 5, get_current_rotation());
}

/**
 * rct2: 0x006D43C6
 */
void vehicle_visual_mini_golf_ball(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle)
{
    if (vehicle->mini_golf_current_animation != 1) {
        return;
    }

    rct_drawpixelinfo *edi = session->Unk140E9A8;
    if (edi->zoom_level >= 1) {
        return;
    }

    if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_PEEPS) {
        return;
    }

    rct_ride *ride = get_ride(vehicle->ride);
    rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);

    uint32 image_id = rideEntry->vehicles[0].base_image_id;
    sub_98197C(image_id, 0, 0, 1, 1, 0, z, 0, 0, z + 3, get_current_rotation());
}
