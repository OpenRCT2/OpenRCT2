/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../config/Config.h"
#include "../../entity/EntityRegistry.h"
#include "../../entity/Guest.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../world/Map.h"
#include "../../world/Surface.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
#include "../VehiclePaint.h"

#include <iterator>

enum
{
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

static constexpr const uint32_t mini_golf_track_sprites_25_deg_up[][3] = {
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
    },
};

static constexpr const uint32_t mini_golf_track_sprites_flat_to_25_deg_up[][3] = {
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
    },
};

static constexpr const uint32_t mini_golf_track_sprites_25_deg_up_to_flat[][3] = {
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
    },
};

static constexpr const uint32_t mini_golf_track_sprites_quarter_turn_1_tile[] = {
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_SW_NW,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_NW_NE,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_NE_SE,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_SE_SW,
};

static constexpr const uint32_t mini_golf_track_sprites_quarter_turn_1_tile_fence_front[] = {
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_FRONT_SW_NW,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_FRONT_NW_NE,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_FRONT_NE_SE,
    SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_FRONT_SE_SW,
};

static constexpr const uint32_t mini_golf_track_sprites_hole_a[4][2][2] = {
    {
        { SPR_MINI_GOLF_HOLE_A_BASE_PART_1_SW_NE, SPR_MINI_GOLF_HOLE_A_TRIM_PART_1_SW_NE },
        { SPR_MINI_GOLF_HOLE_A_BASE_PART_2_SW_NE, SPR_MINI_GOLF_HOLE_A_TRIM_PART_2_SW_NE },
    },
    {
        { SPR_MINI_GOLF_HOLE_A_BASE_PART_1_NW_SE, SPR_MINI_GOLF_HOLE_A_TRIM_PART_1_NW_SE },
        { SPR_MINI_GOLF_HOLE_A_BASE_PART_2_NW_SE, SPR_MINI_GOLF_HOLE_A_TRIM_PART_2_NW_SE },
    },
    {
        { SPR_MINI_GOLF_HOLE_A_BASE_PART_1_NE_SW, SPR_MINI_GOLF_HOLE_A_TRIM_PART_1_NE_SW },
        { SPR_MINI_GOLF_HOLE_A_BASE_PART_2_NE_SW, SPR_MINI_GOLF_HOLE_A_TRIM_PART_2_NE_SW },
    },
    {
        { SPR_MINI_GOLF_HOLE_A_BASE_PART_1_SE_NW, SPR_MINI_GOLF_HOLE_A_TRIM_PART_1_SE_NW },
        { SPR_MINI_GOLF_HOLE_A_BASE_PART_2_SE_NW, SPR_MINI_GOLF_HOLE_A_TRIM_PART_2_SE_NW },
    },
};

static constexpr const uint32_t mini_golf_track_sprites_hole_b[4][2][2] = {
    {
        { SPR_MINI_GOLF_HOLE_B_BASE_PART_1_SW_NE, SPR_MINI_GOLF_HOLE_B_TRIM_PART_1_SW_NE },
        { SPR_MINI_GOLF_HOLE_B_BASE_PART_2_SW_NE, SPR_MINI_GOLF_HOLE_B_TRIM_PART_2_SW_NE },
    },
    {
        { SPR_MINI_GOLF_HOLE_B_BASE_PART_1_NW_SE, SPR_MINI_GOLF_HOLE_B_TRIM_PART_1_NW_SE },
        { SPR_MINI_GOLF_HOLE_B_BASE_PART_2_NW_SE, SPR_MINI_GOLF_HOLE_B_TRIM_PART_2_NW_SE },
    },
    {
        { SPR_MINI_GOLF_HOLE_B_BASE_PART_1_NE_SW, SPR_MINI_GOLF_HOLE_B_TRIM_PART_1_NE_SW },
        { SPR_MINI_GOLF_HOLE_B_BASE_PART_2_NE_SW, SPR_MINI_GOLF_HOLE_B_TRIM_PART_2_NE_SW },
    },
    {
        { SPR_MINI_GOLF_HOLE_B_BASE_PART_1_SE_NW, SPR_MINI_GOLF_HOLE_B_TRIM_PART_1_SE_NW },
        { SPR_MINI_GOLF_HOLE_B_BASE_PART_2_SE_NW, SPR_MINI_GOLF_HOLE_B_TRIM_PART_2_SE_NW },
    },
};

static constexpr const uint32_t mini_golf_track_sprites_hole_c[][2][2] = {
    {
        { SPR_MINI_GOLF_HOLE_C_BASE_PART_1_SW_NE, SPR_MINI_GOLF_HOLE_C_TRIM_PART_1_SW_NE },
        { SPR_MINI_GOLF_HOLE_C_BASE_PART_2_SW_NE, SPR_MINI_GOLF_HOLE_C_TRIM_PART_2_SW_NE },
    },
    {
        { SPR_MINI_GOLF_HOLE_C_BASE_PART_1_NW_SE, SPR_MINI_GOLF_HOLE_C_TRIM_PART_1_NW_SE },
        { SPR_MINI_GOLF_HOLE_C_BASE_PART_2_NW_SE, SPR_MINI_GOLF_HOLE_C_TRIM_PART_2_NW_SE },
    },
    {
        { SPR_MINI_GOLF_HOLE_C_BASE_PART_1_NE_SW, SPR_MINI_GOLF_HOLE_C_TRIM_PART_1_NE_SW },
        { SPR_MINI_GOLF_HOLE_C_BASE_PART_2_NE_SW, SPR_MINI_GOLF_HOLE_C_TRIM_PART_2_NE_SW },
    },
    {
        { SPR_MINI_GOLF_HOLE_C_BASE_PART_1_SE_NW, SPR_MINI_GOLF_HOLE_C_TRIM_PART_1_SE_NW },
        { SPR_MINI_GOLF_HOLE_C_BASE_PART_2_SE_NW, SPR_MINI_GOLF_HOLE_C_TRIM_PART_2_SE_NW },
    },
};

static constexpr const uint32_t mini_golf_track_sprites_hole_d[][3][2] = {
    {
        { SPR_MINI_GOLF_HOLE_D_BASE_PART_1_SW_SE, SPR_MINI_GOLF_HOLE_D_TRIM_PART_1_SW_SE },
        { SPR_MINI_GOLF_HOLE_D_BASE_PART_2_SW_SE, SPR_MINI_GOLF_HOLE_D_TRIM_PART_2_SW_SE },
        { SPR_MINI_GOLF_HOLE_D_BASE_PART_3_SW_SE, SPR_MINI_GOLF_HOLE_D_TRIM_PART_3_SW_SE },
    },
    {
        { SPR_MINI_GOLF_HOLE_D_BASE_PART_1_NW_SW, SPR_MINI_GOLF_HOLE_D_TRIM_PART_1_NW_SW },
        { SPR_MINI_GOLF_HOLE_D_BASE_PART_2_NW_SW, SPR_MINI_GOLF_HOLE_D_TRIM_PART_2_NW_SW },
        { SPR_MINI_GOLF_HOLE_D_BASE_PART_3_NW_SW, SPR_MINI_GOLF_HOLE_D_TRIM_PART_3_NW_SW },
    },
    {
        { SPR_MINI_GOLF_HOLE_D_BASE_PART_1_NE_NW, SPR_MINI_GOLF_HOLE_D_TRIM_PART_1_NE_NW },
        { SPR_MINI_GOLF_HOLE_D_BASE_PART_2_NE_NW, SPR_MINI_GOLF_HOLE_D_TRIM_PART_2_NE_NW },
        { SPR_MINI_GOLF_HOLE_D_BASE_PART_3_NE_NW, SPR_MINI_GOLF_HOLE_D_TRIM_PART_3_NE_NW },
    },
    {
        { SPR_MINI_GOLF_HOLE_D_BASE_PART_1_SE_NE, SPR_MINI_GOLF_HOLE_D_TRIM_PART_1_SE_NE },
        { SPR_MINI_GOLF_HOLE_D_BASE_PART_2_SE_NE, SPR_MINI_GOLF_HOLE_D_TRIM_PART_2_SE_NE },
        { SPR_MINI_GOLF_HOLE_D_BASE_PART_3_SE_NE, SPR_MINI_GOLF_HOLE_D_TRIM_PART_3_SE_NE },
    },
};

static constexpr const uint32_t mini_golf_track_sprites_hole_e[][3][2] = {
    {
        { SPR_MINI_GOLF_HOLE_E_BASE_PART_1_SW_NW, SPR_MINI_GOLF_HOLE_E_TRIM_PART_1_SW_NW },
        { SPR_MINI_GOLF_HOLE_E_BASE_PART_2_SW_NW, SPR_MINI_GOLF_HOLE_E_TRIM_PART_2_SW_NW },
        { SPR_MINI_GOLF_HOLE_E_BASE_PART_3_SW_NW, SPR_MINI_GOLF_HOLE_E_TRIM_PART_3_SW_NW },
    },
    {
        { SPR_MINI_GOLF_HOLE_E_BASE_PART_1_NW_NE, SPR_MINI_GOLF_HOLE_E_TRIM_PART_1_NW_NE },
        { SPR_MINI_GOLF_HOLE_E_BASE_PART_2_NW_NE, SPR_MINI_GOLF_HOLE_E_TRIM_PART_2_NW_NE },
        { SPR_MINI_GOLF_HOLE_E_BASE_PART_3_NW_NE, SPR_MINI_GOLF_HOLE_E_TRIM_PART_3_NW_NE },
    },
    {
        { SPR_MINI_GOLF_HOLE_E_BASE_PART_1_NE_SE, SPR_MINI_GOLF_HOLE_E_TRIM_PART_1_NE_SE },
        { SPR_MINI_GOLF_HOLE_E_BASE_PART_2_NE_SE, SPR_MINI_GOLF_HOLE_E_TRIM_PART_2_NE_SE },
        { SPR_MINI_GOLF_HOLE_E_BASE_PART_3_NE_SE, SPR_MINI_GOLF_HOLE_E_TRIM_PART_3_NE_SE },
    },
    {
        { SPR_MINI_GOLF_HOLE_E_BASE_PART_1_SE_SW, SPR_MINI_GOLF_HOLE_E_TRIM_PART_1_SE_SW },
        { SPR_MINI_GOLF_HOLE_E_BASE_PART_2_SE_SW, SPR_MINI_GOLF_HOLE_E_TRIM_PART_2_SE_SW },
        { SPR_MINI_GOLF_HOLE_E_BASE_PART_3_SE_SW, SPR_MINI_GOLF_HOLE_E_TRIM_PART_3_SE_SW },
    },
};

/** rct2: 0x00933471 */
// clang-format off
static constexpr const uint8_t mini_golf_peep_animation_frames_walk[] = {
    0, 1, 2, 3, 4, 5,
};

/** rct2: 0x00933478 */
static constexpr const uint8_t mini_golf_peep_animation_frames_place_ball_downwards[] = {
    12, 13, 14, 15,
};

/** rct2: 0x009334B5 */
static constexpr const uint8_t mini_golf_peep_animation_frames_swing[] = {
    31, 31, 31, 31, 31, 31, 31, 31, 31, 32, 33, 33, 33, 33, 34,
};

/** rct2: 0x0093347D */
static constexpr const uint8_t mini_golf_peep_animation_frames_swing_left[] = {
    6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 8, 8, 8, 9,
};

/** rct2: 0x0093348D */
static constexpr const uint8_t mini_golf_peep_animation_frames_pickup_ball[] = {
    12, 13, 14, 15, 14, 13, 12,
};

/** rct2: 0x00933495 */
static constexpr const uint8_t mini_golf_peep_animation_frames_jump[] = {
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
};

/** rct2: 0x009334A5 */
static constexpr const uint8_t mini_golf_peep_animation_frames_place_ball_upwards[] = {
    15, 14, 13, 12,
};

/** rct2: 0x009334C5 */
static constexpr const uint8_t mini_golf_peep_animation_frames_putt[] = {
    35, 36, 36, 36, 36, 36, 35, 35, 35, 35,
};

/** rct2: 0x009334AA */
static constexpr const uint8_t mini_golf_peep_animation_frames_putt_left[] = {
    10, 11, 11, 11, 11, 11, 10, 10, 10, 10,
};

/** rct2: 0x008B8F74 */
static constexpr const uint8_t* mini_golf_peep_animation_frames[] = {
    mini_golf_peep_animation_frames_walk,
    mini_golf_peep_animation_frames_place_ball_downwards,
    mini_golf_peep_animation_frames_swing_left,
    mini_golf_peep_animation_frames_pickup_ball,
    mini_golf_peep_animation_frames_jump,
    mini_golf_peep_animation_frames_place_ball_upwards,
    mini_golf_peep_animation_frames_putt_left,
    mini_golf_peep_animation_frames_swing,
    mini_golf_peep_animation_frames_putt,
};

const size_t mini_golf_peep_animation_lengths[] = {
    std::size(mini_golf_peep_animation_frames_walk),
    std::size(mini_golf_peep_animation_frames_place_ball_downwards),
    std::size(mini_golf_peep_animation_frames_swing_left),
    std::size(mini_golf_peep_animation_frames_pickup_ball),
    std::size(mini_golf_peep_animation_frames_jump),
    std::size(mini_golf_peep_animation_frames_place_ball_upwards),
    std::size(mini_golf_peep_animation_frames_putt_left),
    std::size(mini_golf_peep_animation_frames_swing),
    std::size(mini_golf_peep_animation_frames_putt),
};
// clang-format on

static paint_struct* mini_golf_paint_util_7c(
    paint_session& session, uint8_t direction, uint32_t image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x,
    int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset, int16_t bound_box_offset_x,
    int16_t bound_box_offset_y, int16_t bound_box_offset_z, uint32_t rotation)
{
    if (direction & 1)
    {
        return PaintAddImageAsParent(
            session, image_id, { y_offset, x_offset, z_offset }, { bound_box_length_y, bound_box_length_x, bound_box_length_z },
            { bound_box_offset_y, bound_box_offset_x, bound_box_offset_z });
    }

    return PaintAddImageAsParent(
        session, image_id, { x_offset, y_offset, z_offset }, { bound_box_length_x, bound_box_length_y, bound_box_length_z },
        { bound_box_offset_x, bound_box_offset_y, bound_box_offset_z });
}

static bool mini_golf_paint_util_should_draw_fence(paint_session& session, const TrackElement& trackElement)
{
    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        // Should be above ground (have passed surface rendering)
        return false;
    }

    const TileElement* surfaceElement = session.SurfaceElement;
    if (surfaceElement->base_height != trackElement.base_height)
    {
        return true;
    }

    if (surfaceElement->AsSurface()->GetSlope() != TILE_ELEMENT_SLOPE_FLAT)
    {
        return true;
    }

    return false;
}

/** rct2: 0x0087F10C */
static void paint_mini_golf_track_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId;

    if (direction & 1)
    {
        imageId = SPR_MINI_GOLF_FLAT_NW_SE | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 20, 32, 1 }, { 6, 0, height });
        paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }
    else
    {
        imageId = SPR_MINI_GOLF_FLAT_SW_NE | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height });
        paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

    if (mini_golf_paint_util_should_draw_fence(session, trackElement))
    {
        if (direction & 1)
        {
            imageId = SPR_MINI_GOLF_FLAT_FENCE_BACK_NW_SE | session.TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 7 }, { 10, 0, height + 2 });

            imageId = SPR_MINI_GOLF_FLAT_FENCE_FRONT_NW_SE | session.TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 7 }, { 22, 0, height + 2 });
        }
        else
        {
            imageId = SPR_MINI_GOLF_FLAT_FENCE_BACK_SW_NE | session.TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 7 }, { 0, 10, height + 2 });

            imageId = SPR_MINI_GOLF_FLAT_FENCE_FRONT_SW_NE | session.TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 7 }, { 0, 22, height + 2 });
        }
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0087F11C */
static void paint_mini_golf_track_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId;

    imageId = mini_golf_track_sprites_25_deg_up[direction][0] | session.TrackColours[SCHEME_TRACK];
    mini_golf_paint_util_7c(session, direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

    imageId = mini_golf_track_sprites_25_deg_up[direction][1] | session.TrackColours[SCHEME_MISC];
    mini_golf_paint_util_7c(session, direction, imageId, 0, 0, 32, 1, 15, height, 0, 10, height + 2, session.CurrentRotation);

    imageId = mini_golf_track_sprites_25_deg_up[direction][2] | session.TrackColours[SCHEME_MISC];
    mini_golf_paint_util_7c(session, direction, imageId, 0, 0, 32, 1, 15, height, 0, 22, height + 2, session.CurrentRotation);

    switch (direction)
    {
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
static void paint_mini_golf_track_flat_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId;

    imageId = mini_golf_track_sprites_flat_to_25_deg_up[direction][0] | session.TrackColours[SCHEME_TRACK];
    mini_golf_paint_util_7c(session, direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

    imageId = mini_golf_track_sprites_flat_to_25_deg_up[direction][1] | session.TrackColours[SCHEME_MISC];
    mini_golf_paint_util_7c(session, direction, imageId, 0, 0, 32, 1, 11, height, 0, 10, height + 2, session.CurrentRotation);

    imageId = mini_golf_track_sprites_flat_to_25_deg_up[direction][2] | session.TrackColours[SCHEME_MISC];
    mini_golf_paint_util_7c(session, direction, imageId, 0, 0, 32, 1, 11, height, 0, 22, height + 2, session.CurrentRotation);

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            break;
    }

    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x0087F13C */
static void paint_mini_golf_track_25_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId;

    imageId = mini_golf_track_sprites_25_deg_up_to_flat[direction][0] | session.TrackColours[SCHEME_TRACK];
    mini_golf_paint_util_7c(session, direction, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);

    imageId = mini_golf_track_sprites_25_deg_up_to_flat[direction][1] | session.TrackColours[SCHEME_MISC];
    mini_golf_paint_util_7c(session, direction, imageId, 0, 0, 32, 1, 11, height, 0, 10, height + 2, session.CurrentRotation);

    imageId = mini_golf_track_sprites_25_deg_up_to_flat[direction][2] | session.TrackColours[SCHEME_MISC];
    mini_golf_paint_util_7c(session, direction, imageId, 0, 0, 32, 1, 11, height, 0, 22, height + 2, session.CurrentRotation);

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_PATH_AND_MINI_GOLF);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_PATH_AND_MINI_GOLF);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_0);
            break;
    }

    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x0087F14C */
static void paint_mini_golf_track_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_mini_golf_track_25_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x0087F15C */
static void paint_mini_golf_track_flat_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_mini_golf_track_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x0087F16C */
static void paint_mini_golf_track_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_mini_golf_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x0087F17C, 0x0087F18C, 0x0087F19C */
static void paint_mini_golf_station(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const auto* stationObj = ride.GetStationObject();
    uint32_t imageId;
    bool hasFence;

    imageId = SPR_MINI_GOLF_STATION_FLOOR | session.TrackColours[SCHEME_TRACK];
    mini_golf_paint_util_7c(session, direction, imageId, 0, 0, 32, 28, 1, height, 0, 0, height, session.CurrentRotation);

    if (direction & 1)
    {
        hasFence = track_paint_util_has_fence(EDGE_NE, session.MapPosition, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = SPR_MINI_GOLF_FLAT_FENCE_BACK_NW_SE | session.TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, { -10, 0, height }, { 1, 32, 7 }, { 0, 0, height + 2 });
        }

        bool hasSWFence = track_paint_util_has_fence(EDGE_SW, session.MapPosition, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = SPR_MINI_GOLF_FLAT_FENCE_FRONT_NW_SE | session.TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, { 10, 0, height }, { 1, 32, 7 }, { 31, 0, height + 2 });
        }

        track_paint_util_draw_station_covers(session, EDGE_NE, hasFence, stationObj, height);
        track_paint_util_draw_station_covers(session, EDGE_SW, hasSWFence, stationObj, height);

        // Was leftwards tunnel in game, seems odd
        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        hasFence = track_paint_util_has_fence(EDGE_NW, session.MapPosition, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = SPR_MINI_GOLF_FLAT_FENCE_BACK_SW_NE | session.TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, { 0, -10, height }, { 32, 1, 7 }, { 0, 0, height + 2 });
        }

        bool hasSEFence = track_paint_util_has_fence(EDGE_SE, session.MapPosition, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = SPR_MINI_GOLF_FLAT_FENCE_FRONT_SW_NE | session.TrackColours[SCHEME_MISC];
            PaintAddImageAsParent(session, imageId, { 0, 10, height }, { 32, 1, 7 }, { 0, 31, height + 2 });
        }

        track_paint_util_draw_station_covers(session, EDGE_NW, hasFence, stationObj, height);
        track_paint_util_draw_station_covers(session, EDGE_SE, hasSEFence, stationObj, height);

        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0087F1AC */
static void paint_mini_golf_track_left_quarter_turn_1_tile(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId;

    track_paint_util_left_quarter_turn_1_tile_paint(
        session, 1, height, 0, direction, session.TrackColours[SCHEME_TRACK], mini_golf_track_sprites_quarter_turn_1_tile);

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0, direction), 0xFFFF, 0);

    const bool shouldDrawFence = mini_golf_paint_util_should_draw_fence(session, trackElement);

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            if (!shouldDrawFence)
                break;

            imageId = SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_BACK_SW_NW | session.TrackColours[SCHEME_MISC];
            PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 26, 24, 1 }, { 6, 2, height });

            break;

        case 1:
            if (!shouldDrawFence)
                break;

            imageId = SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_BACK_NW_NE | session.TrackColours[SCHEME_MISC];
            PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 26, 26, 1 }, { 0, 0, height });
            break;

        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            if (!shouldDrawFence)
                break;

            imageId = SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_BACK_NE_SE | session.TrackColours[SCHEME_MISC];
            PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 24, 26, 1 }, { 2, 6, height });
            break;

        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            if (!shouldDrawFence)
                break;

            imageId = SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_BACK_SE_SW | session.TrackColours[SCHEME_MISC];
            PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 24, 24, 1 }, { 6, 6, height });
            break;
    }

    if (shouldDrawFence)
    {
        // TODO: The back fence uses the same x/y offsets, but uses another paint function. See if this occurs more often.
        track_paint_util_left_quarter_turn_1_tile_paint(
            session, 0, height, 24, direction, session.TrackColours[SCHEME_MISC],
            mini_golf_track_sprites_quarter_turn_1_tile_fence_front);

        switch (direction)
        {
            case 0:
                imageId = SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_INSIDE_SW_NW | session.TrackColours[SCHEME_MISC];
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 5, 5, 5 }, { 24, 0, height + 2 });
                break;
            case 2:
                imageId = SPR_MINI_GOLF_QUARTER_TURN_1_TILE_FENCE_INSIDE_NE_SE | session.TrackColours[SCHEME_MISC];
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 5, 5, 5 }, { 0, 24, height + 2 });
                break;
        }
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0087F1BC */
static void paint_mini_golf_track_right_quarter_turn_1_tile(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_mini_golf_track_left_quarter_turn_1_tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

static void paint_mini_golf_hole_ab(
    paint_session& session, uint8_t trackSequence, uint8_t direction, int32_t height, const uint32_t sprites[4][2][2])
{
    uint32_t imageId;
    CoordsXY boundBox, boundBoxOffset;

    bool drewSupports = wooden_a_supports_paint_setup(
        session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);

    if ((direction == 0 && trackSequence == 0) || (direction == 2 && trackSequence == 1))
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }
    else if ((direction == 3 && trackSequence == 0) || (direction == 1 && trackSequence == 1))
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }

    if (direction & 1)
    {
        boundBox = { 26, 32 };
        boundBoxOffset = { 3, 0 };
    }
    else
    {
        boundBox = { 32, 26 };
        boundBoxOffset = { 0, 3 };
    }

    imageId = sprites[direction][trackSequence][1] | session.TrackColours[SCHEME_TRACK];
    PaintAddImageAsParent(
        session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 0 }, { boundBoxOffset.x, boundBoxOffset.y, height + 24 });

    if (drewSupports)
    {
        imageId = ((direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS) | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });

        imageId = sprites[direction][trackSequence][0] | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsChild(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
    else
    {
        imageId = sprites[direction][trackSequence][0] | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
}

/** rct2: 0x0087F1CC */
static void paint_mini_golf_hole_a(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_mini_golf_hole_ab(session, trackSequence, direction, height, mini_golf_track_sprites_hole_a);
}

/** rct2: 0x0087F1DC */
static void paint_mini_golf_hole_b(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_mini_golf_hole_ab(session, trackSequence, direction, height, mini_golf_track_sprites_hole_b);
}

/** rct2: 0x0087F1EC */
static void paint_mini_golf_hole_c(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId;
    CoordsXY boundBox, boundBoxOffset;

    bool drewSupports = wooden_a_supports_paint_setup(
        session, (direction & 1), 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);

    if ((direction == 0 && trackSequence == 0) || (direction == 2 && trackSequence == 1))
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }
    else if ((direction == 3 && trackSequence == 0) || (direction == 1 && trackSequence == 1))
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }

    if (direction & 1)
    {
        boundBox = { 26, 32 };
        boundBoxOffset = { 3, 0 };
    }
    else
    {
        boundBox = { 32, 26 };
        boundBoxOffset = { 0, 3 };
    }

    imageId = mini_golf_track_sprites_hole_c[direction][trackSequence][1] | session.TrackColours[SCHEME_TRACK];

    switch ((direction << 4) | trackSequence)
    {
        case 0x01:
        case 0x20:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 26, 3 }, { 30, 3, height + 4 });
            break;
        case 0x10:
        case 0x31:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 2, 3 }, { 3, 30, height + 4 });
            break;
        default:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 0 },
                { boundBoxOffset.x, boundBoxOffset.y, height + 24 });
            break;
    }

    if (drewSupports)
    {
        imageId = ((direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS) | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });

        imageId = mini_golf_track_sprites_hole_c[direction][trackSequence][0] | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsChild(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
    else
    {
        imageId = mini_golf_track_sprites_hole_c[direction][trackSequence][0] | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
}

/** rct2: 0x0087F1FC */
static void paint_mini_golf_hole_d(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId;
    CoordsXY boundBox, boundBoxOffset;

    int32_t supportType = (direction & 1);
    if (trackSequence == 2)
        supportType = 1 - supportType;
    bool drewSupports = wooden_a_supports_paint_setup(session, supportType, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);

    switch ((direction << 4) | trackSequence)
    {
        case 0x00:
        case 0x12:
            paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            break;

        case 0x02:
        case 0x30:
            paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            break;
    }

    if (supportType & 1)
    {
        boundBox = { 26, 32 };
        boundBoxOffset = { 3, 0 };
    }
    else
    {
        boundBox = { 32, 26 };
        boundBoxOffset = { 0, 3 };
    }

    imageId = mini_golf_track_sprites_hole_d[direction][trackSequence][1] | session.TrackColours[SCHEME_TRACK];

    switch ((direction << 4) | trackSequence)
    {
        case 0x01:
        case 0x32:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 26, 3 }, { 30, 3, height + 4 });
            break;
        case 0x02:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 23, 2, 3 }, { 3, 30, height + 4 });
            break;
        case 0x10:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 24, 3 }, { 30, 3, height + 4 });
            break;
        case 0x20:
        case 0x31:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 2, 3 }, { 3, 30, height + 4 });
            break;
        default:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 0 },
                { boundBoxOffset.x, boundBoxOffset.y, height + 24 });
            break;
    }

    switch ((direction << 4) | trackSequence)
    {
        case 0x02:
            boundBox = { 23, 32 };
            break;
        case 0x10:
            boundBox = { 24, 32 };
            break;
    }

    if (drewSupports)
    {
        imageId = ((supportType & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS) | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });

        imageId = mini_golf_track_sprites_hole_d[direction][trackSequence][0] | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsChild(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
    else
    {
        imageId = mini_golf_track_sprites_hole_d[direction][trackSequence][0] | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
}

/** rct2: 0x0087F1FC */
static void paint_mini_golf_hole_e(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId;
    CoordsXY boundBox, boundBoxOffset;

    int32_t supportType = (direction & 1);
    if (trackSequence == 2)
        supportType = 1 - supportType;
    bool drewSupports = wooden_a_supports_paint_setup(session, supportType, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);

    switch ((direction << 4) | trackSequence)
    {
        case 0x00:
        case 0x12:
            paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            break;

        case 0x02:
        case 0x30:
            paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
            break;
    }

    if (supportType & 1)
    {
        boundBox = { 26, 32 };
        boundBoxOffset = { 3, 0 };
    }
    else
    {
        boundBox = { 32, 26 };
        boundBoxOffset = { 0, 3 };
    }

    imageId = mini_golf_track_sprites_hole_e[direction][trackSequence][1] | session.TrackColours[SCHEME_TRACK];

    switch ((direction << 4) | trackSequence)
    {
        case 0x01:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 26, 3 }, { 30, 3, height + 4 });
            break;
        case 0x02:
        case 0x20:
        case 0x31:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 2, 3 }, { 3, 30, height + 4 });
            break;
        case 0x10:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 24, 3 }, { 30, 3, height + 4 });
            break;
        case 0x32:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 2, 23, 3 }, { 30, 3, height + 4 });
            break;
        default:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 0 },
                { boundBoxOffset.x, boundBoxOffset.y, height + 24 });
            break;
    }

    switch ((direction << 4) | trackSequence)
    {
        case 0x10:
            boundBox = { 24, 32 };
            break;
        case 0x32:
            boundBox = { 32, 23 };
            break;
    }

    if (drewSupports)
    {
        imageId = ((supportType & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS) | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });

        imageId = mini_golf_track_sprites_hole_e[direction][trackSequence][0] | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsChild(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
    else
    {
        imageId = mini_golf_track_sprites_hole_e[direction][trackSequence][0] | session.TrackColours[SCHEME_TRACK];
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { boundBox.x, boundBox.y, 1 }, { boundBoxOffset.x, boundBoxOffset.y, height });
    }
}

/**
 * rct2: 0x0087EDC4
 */
TRACK_PAINT_FUNCTION get_track_paint_function_mini_golf(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return paint_mini_golf_track_flat;

        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
        case TrackElemType::EndStation:
            return paint_mini_golf_station;

        case TrackElemType::Up25:
            return paint_mini_golf_track_25_deg_up;
        case TrackElemType::FlatToUp25:
            return paint_mini_golf_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return paint_mini_golf_track_25_deg_up_to_flat;

        case TrackElemType::Down25:
            return paint_mini_golf_track_25_deg_down;
        case TrackElemType::FlatToDown25:
            return paint_mini_golf_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return paint_mini_golf_track_25_deg_down_to_flat;

        case TrackElemType::LeftQuarterTurn1Tile:
            return paint_mini_golf_track_left_quarter_turn_1_tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return paint_mini_golf_track_right_quarter_turn_1_tile;

        case TrackElemType::MinigolfHoleA:
            return paint_mini_golf_hole_a;
        case TrackElemType::MinigolfHoleB:
            return paint_mini_golf_hole_b;
        case TrackElemType::MinigolfHoleC:
            return paint_mini_golf_hole_c;
        case TrackElemType::MinigolfHoleD:
            return paint_mini_golf_hole_d;
        case TrackElemType::MinigolfHoleE:
            return paint_mini_golf_hole_e;
    }

    return nullptr;
}

/**
 * rct2: 0x006D42F0
 */
void vehicle_visual_mini_golf_player(
    paint_session& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle)
{
    if (vehicle->num_peeps == 0)
    {
        return;
    }

    rct_drawpixelinfo* edi = &session.DPI;
    if (edi->zoom_level >= ZoomLevel{ 2 })
    {
        return;
    }

    auto ride = vehicle->GetRide();
    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    auto* peep = GetEntity<Guest>(vehicle->peep[0]);
    if (peep == nullptr)
        return;

    uint8_t frame = mini_golf_peep_animation_frames[EnumValue(vehicle->mini_golf_current_animation)][vehicle->animation_frame];
    uint32_t ebx = (frame << 2) + OpenRCT2::Entity::Yaw::YawTo4(imageDirection);

    uint32_t image_id = rideEntry->vehicles[0].base_image_id + 1 + ebx;
    uint32_t peep_palette = peep->TshirtColour << 19 | peep->TrousersColour << 24 | 0x0A0000000;
    PaintAddImageAsParent(session, image_id | peep_palette, { 0, 0, z }, { 1, 1, 11 }, { 0, 0, z + 5 });
}

/**
 * rct2: 0x006D43C6
 */
void vehicle_visual_mini_golf_ball(
    paint_session& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle)
{
    if (vehicle->mini_golf_current_animation != MiniGolfAnimation::PlaceBallDown)
    {
        return;
    }

    rct_drawpixelinfo* edi = &session.DPI;
    if (edi->zoom_level >= ZoomLevel{ 1 })
    {
        return;
    }

    auto ride = vehicle->GetRide();
    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    uint32_t image_id = rideEntry->vehicles[0].base_image_id;
    PaintAddImageAsParent(session, image_id, { 0, 0, z }, { 1, 1, 0 }, { 0, 0, z + 3 });
}
