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

#include "../../drawing/drawing.h"
#include "../../interface/viewport.h"
#include "../../paint/map_element/map_element.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../../sprites.h"
#include "../../world/map.h"
#include "../../world/sprite.h"
#include "../ride_data.h"
#include "../track_data.h"
#include "../track_paint.h"

typedef struct sprite_bb_2 {
    uint32 sprite_id_a;
    uint32 sprite_id_b;
    rct_xyz16 offset;
    rct_xyz16 bb_offset;
    rct_xyz16 bb_size;
} sprite_bb_2;

enum {
    SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_SW_NE = 23497,
    SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_NW_SE = 23498,
    SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_NE_SW = 23499,
    SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_SE_NW = 23500,
    SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_SW_NE = 23501,
    SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_NW_SE = 23502,
    SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_NE_SW = 23503,
    SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_SE_NW = 23504,
    SPR_WOODEN_RC_LEFT_BANK_SW_NE = 23505,
    SPR_WOODEN_RC_LEFT_BANK_NW_SE = 23506,
    SPR_WOODEN_RC_LEFT_BANK_NE_SW = 23507,
    SPR_WOODEN_RC_LEFT_BANK_SE_NW = 23508,
    SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_SW_NE = 23509,
    SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_NW_SE = 23510,
    SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_NE_SW = 23511,
    SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_SE_NW = 23512,
    SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_SW_NE = 23513,
    SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_NW_SE = 23514,
    SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_NE_SW = 23515,
    SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_SE_NW = 23516,
    SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_SW_NE = 23517,
    SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_NW_SE = 23518,
    SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_NE_SW = 23519,
    SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_SE_NW = 23520,
    SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_SW_NE = 23521,
    SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_NW_SE = 23522,
    SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_NE_SW = 23523,
    SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_SE_NW = 23524,
    SPR_WOODEN_RC_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE = 23525,
    SPR_WOODEN_RC_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW = 23526,
    SPR_WOODEN_RC_RC_FLAT_TO_RIGHT_BANK_FRONT_NE_SW = 23527,
    SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_FRONT_NW_SE = 23528,
    SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_FRONT_NE_SW = 23529,
    SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_FRONT_NW_SE = 23530,
    SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_FRONT_NE_SW = 23531,
    SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_FRONT_NW_SE = 23532,
    SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_FRONT_NE_SW = 23533,
    SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_FRONT_NW_SE = 23534,
    SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_FRONT_NE_SW = 23535,
    SPR_WOODEN_RC_RC_FLAT_TO_RIGHT_BANK_FRONT_SW_NE = 23536,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_SW_NE = 23537,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_NW_SE = 23538,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_NE_SW = 23539,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_SE_NW = 23540,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_SW_NE = 23541,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_NW_SE = 23542,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_NE_SW = 23543,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_SE_NW = 23544,
    SPR_WOODEN_RC_25_DEG_SW_NE = 23545,
    SPR_WOODEN_RC_25_DEG_NW_SE = 23546,
    SPR_WOODEN_RC_25_DEG_NE_SW = 23547,
    SPR_WOODEN_RC_25_DEG_SE_NW = 23548,
    SPR_WOODEN_RC_25_DEG_TO_60_DEG_SW_NE = 23549,
    SPR_WOODEN_RC_25_DEG_TO_60_DEG_FRONT_NW_SE = 23550,
    SPR_WOODEN_RC_25_DEG_TO_60_DEG_NW_SE = 23551,
    SPR_WOODEN_RC_25_DEG_TO_60_DEG_FRONT_NE_SW = 23552,
    SPR_WOODEN_RC_25_DEG_TO_60_DEG_NE_SW = 23553,
    SPR_WOODEN_RC_25_DEG_TO_60_DEG_SE_NW = 23554,
    SPR_WOODEN_RC_60_DEG_TO_25_DEG_SW_NE = 23555,
    SPR_WOODEN_RC_60_DEG_TO_25_DEG_FRONT_NW_SE = 23556,
    SPR_WOODEN_RC_60_DEG_TO_25_DEG_NW_SE = 23557,
    SPR_WOODEN_RC_60_DEG_TO_25_DEG_FRONT_NE_SW = 23558,
    SPR_WOODEN_RC_60_DEG_TO_25_DEG_NE_SW = 23559,
    SPR_WOODEN_RC_60_DEG_TO_25_DEG_SE_NW = 23560,
    SPR_WOODEN_RC_60_DEG_SW_NE = 23561,
    SPR_WOODEN_RC_60_DEG_CHAIN_SW_NE = 23561,
    SPR_WOODEN_RC_60_DEG_NW_SE = 23562,
    SPR_WOODEN_RC_60_DEG_CHAIN_NW_SE = 23562,
    SPR_WOODEN_RC_60_DEG_NE_SW = 23563,
    SPR_WOODEN_RC_60_DEG_CHAIN_NE_SW = 23563,
    SPR_WOODEN_RC_60_DEG_SE_NW = 23564,
    SPR_WOODEN_RC_60_DEG_CHAIN_SE_NW = 23564,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_FRONT_NW_SE = 23565,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_FRONT_NE_SW = 23566,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_FRONT_NW_SE = 23567,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_FRONT_NE_SW = 23568,
    SPR_WOODEN_RC_25_DEG_FRONT_NW_SE = 23569,
    SPR_WOODEN_RC_60_DEG_FRONT_NW_SE = 23569,
    SPR_WOODEN_RC_25_DEG_FRONT_NE_SW = 23570,
    SPR_WOODEN_RC_60_DEG_FRONT_NE_SW = 23570,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_SW_NE = 23571,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_NW_SE = 23572,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_NE_SW = 23573,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_SE_NW = 23574,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_SW_NE = 23575,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_NW_SE = 23576,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_NE_SW = 23577,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_SE_NW = 23578,
    SPR_WOODEN_RC_25_DEG_CHAIN_SW_NE = 23579,
    SPR_WOODEN_RC_25_DEG_CHAIN_NW_SE = 23580,
    SPR_WOODEN_RC_25_DEG_CHAIN_NE_SW = 23581,
    SPR_WOODEN_RC_25_DEG_CHAIN_SE_NW = 23582,

    SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_FRONT_NW_SE = 23599,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_FRONT_NE_SW = 23600,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_FRONT_NW_SE = 23601,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_FRONT_NE_SW = 23602,
    SPR_WOODEN_RC_25_DEG_CHAIN_FRONT_NW_SE = 23603,
    SPR_WOODEN_RC_60_DEG_CHAIN_FRONT_NW_SE = 23603,
    SPR_WOODEN_RC_25_DEG_CHAIN_FRONT_NE_SW = 23604,
    SPR_WOODEN_RC_60_DEG_CHAIN_FRONT_NE_SW = 23604,
    SPR_WOODEN_RC_QUARTER_TURN_5_SW_SE_SEQ_0 = 23605,
    SPR_WOODEN_RC_QUARTER_TURN_5_SW_SE_SEQ_2 = 23606,
    SPR_WOODEN_RC_QUARTER_TURN_5_SW_SE_SEQ_3 = 23607,
    SPR_WOODEN_RC_QUARTER_TURN_5_SW_SE_SEQ_5 = 23608,
    SPR_WOODEN_RC_QUARTER_TURN_5_SW_SE_SEQ_6 = 23609,
    SPR_WOODEN_RC_QUARTER_TURN_5_NW_SW_SEQ_0 = 23610,
    SPR_WOODEN_RC_QUARTER_TURN_5_NW_SW_SEQ_2 = 23611,
    SPR_WOODEN_RC_QUARTER_TURN_5_NW_SW_SEQ_3 = 23612,
    SPR_WOODEN_RC_QUARTER_TURN_5_NW_SW_SEQ_5 = 23613,
    SPR_WOODEN_RC_QUARTER_TURN_5_NW_SW_SEQ_6 = 23614,
    SPR_WOODEN_RC_QUARTER_TURN_5_NE_NW_SEQ_0 = 23615,
    SPR_WOODEN_RC_QUARTER_TURN_5_NE_NW_SEQ_2 = 23616,
    SPR_WOODEN_RC_QUARTER_TURN_5_NE_NW_SEQ_3 = 23617,
    SPR_WOODEN_RC_QUARTER_TURN_5_NE_NW_SEQ_5 = 23618,
    SPR_WOODEN_RC_QUARTER_TURN_5_NE_NW_SEQ_6 = 23619,
    SPR_WOODEN_RC_QUARTER_TURN_5_SE_NE_SEQ_0 = 23620,
    SPR_WOODEN_RC_QUARTER_TURN_5_SE_NE_SEQ_2 = 23621,
    SPR_WOODEN_RC_QUARTER_TURN_5_SE_NE_SEQ_3 = 23622,
    SPR_WOODEN_RC_QUARTER_TURN_5_SE_NE_SEQ_5 = 23623,
    SPR_WOODEN_RC_QUARTER_TURN_5_SE_NE_SEQ_6 = 23624,
    SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_SW_SE_SEQ_0 = 23625,
    SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_SW_SE_SEQ_2 = 23626,
    SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_SW_SE_SEQ_3 = 23627,
    SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_SW_SE_SEQ_5 = 23628,
    SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_SW_SE_SEQ_6 = 23629,
    SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_NE_NW_SEQ_0 = 23630,
    SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_NE_NW_SEQ_2 = 23631,
    SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_NE_NW_SEQ_3 = 23632,
    SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_NE_NW_SEQ_5 = 23633,
    SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_NE_NW_SEQ_6 = 23634,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_0 = 23635,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_2 = 23636,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_3 = 23637,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_5 = 23638,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_6 = 23639,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_0 = 23640,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_2 = 23641,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_3 = 23642,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_5 = 23643,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_6 = 23644,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_0 = 23645,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_2 = 23646,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_3 = 23647,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_5 = 23648,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_6 = 23649,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_0 = 23650,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_2 = 23651,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_3 = 23652,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_5 = 23653,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_6 = 23654,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_0 = 23655,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_2 = 23656,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_3 = 23657,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_5 = 23658,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_6 = 23659,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_0 = 23660,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_2 = 23661,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_3 = 23662,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_5 = 23663,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_6 = 23664,

    SPR_WOODEN_RC_FLAT_CHAIN_SW_NE = 23749,
    SPR_WOODEN_RC_FLAT_CHAIN_NW_SE = 23750,
    SPR_WOODEN_RC_FLAT_CHAIN_NE_SW = 23751,
    SPR_WOODEN_RC_FLAT_CHAIN_SE_NW = 23752,
    SPR_WOODEN_RC_FLAT_SW_NE = 23753,
    SPR_WOODEN_RC_FLAT_NW_SE = 23754,
    SPR_WOODEN_RC_BRAKES_SW_NE = 23755,
    SPR_WOODEN_RC_BRAKES_NW_SE = 23756,
    SPR_WOODEN_RC_BLOCK_BRAKES_SW_NE = 23757,
    SPR_WOODEN_RC_BLOCK_BRAKES_NW_SE = 23758,

    SPR_WOODEN_RC_STATION_SW_NE = 23973,
    SPR_WOODEN_RC_STATION_NW_SE = 23974,

    SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_RAILS_SW_NE = 24363,
    SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_RAILS_NW_SE = 24364,
    SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_RAILS_NE_SW = 24365,
    SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_RAILS_SE_NW = 24366,
    SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_RAILS_SW_NE = 24367,
    SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_RAILS_NW_SE = 24368,
    SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_RAILS_NE_SW = 24369,
    SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_RAILS_SE_NW = 24370,
    SPR_WOODEN_RC_LEFT_BANK_RAILS_SW_NE = 24371,
    SPR_WOODEN_RC_LEFT_BANK_RAILS_NW_SE = 24372,
    SPR_WOODEN_RC_LEFT_BANK_RAILS_NE_SW = 24373,
    SPR_WOODEN_RC_LEFT_BANK_RAILS_SE_NW = 24374,
    SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_RAILS_SW_NE = 24375,
    SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_RAILS_NW_SE = 24376,
    SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_RAILS_NE_SW = 24377,
    SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_RAILS_SE_NW = 24378,
    SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_RAILS_SW_NE = 24379,
    SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_RAILS_NW_SE = 24380,
    SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_RAILS_NE_SW = 24381,
    SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_RAILS_SE_NW = 24382,
    SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_RAILS_SW_NE = 24383,
    SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_RAILS_NW_SE = 24384,
    SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_RAILS_NE_SW = 24385,
    SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_RAILS_SE_NW = 24386,
    SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_RAILS_SW_NE = 24387,
    SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_RAILS_NW_SE = 24388,
    SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_RAILS_NE_SW = 24389,
    SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_RAILS_SE_NW = 24390,
    SPR_WOODEN_RC_RC_FLAT_TO_LEFT_BANK_RAILS_FRONT_NW_SE = 24391,
    SPR_WOODEN_RC_RC_FLAT_TO_LEFT_BANK_RAILS_FRONT_SE_NW = 24392,
    SPR_WOODEN_RC_RC_FLAT_TO_RIGHT_BANK_RAILS_FRONT_NE_SW = 24393,
    SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_RAILS_FRONT_NW_SE = 24394,
    SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_RAILS_FRONT_NE_SW = 24395,
    SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_RAILS_FRONT_NW_SE = 24396,
    SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_RAILS_FRONT_NE_SW = 24397,
    SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_RAILS_FRONT_NW_SE = 24398,
    SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_RAILS_FRONT_NE_SW = 24399,
    SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_RAILS_FRONT_NW_SE = 24400,
    SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_RAILS_FRONT_NE_SW = 24401,
    SPR_WOODEN_RC_RC_FLAT_TO_RIGHT_BANK_RAILS_FRONT_SW_NE = 24402,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_SW_NE = 24403,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_NW_SE = 24404,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_NE_SW = 24405,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_SE_NW = 24406,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_SW_NE = 24407,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_NW_SE = 24408,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_NE_SW = 24409,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_SE_NW = 24410,
    SPR_WOODEN_RC_25_DEG_RAILS_SW_NE = 24411,
    SPR_WOODEN_RC_25_DEG_RAILS_NW_SE = 24412,
    SPR_WOODEN_RC_25_DEG_RAILS_NE_SW = 24413,
    SPR_WOODEN_RC_25_DEG_RAILS_SE_NW = 24414,
    SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_SW_NE = 24415,
    SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_FRONT_NW_SE = 24416,
    SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_NW_SE = 24417,
    SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_FRONT_NE_SW = 24418,
    SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_NE_SW = 24419,
    SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_SE_NW = 24420,
    SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_SW_NE = 24421,
    SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_FRONT_NW_SE = 24422,
    SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_NW_SE = 24423,
    SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_FRONT_NE_SW = 24424,
    SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_NE_SW = 24425,
    SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_SE_NW = 24426,
    SPR_WOODEN_RC_60_DEG_RAILS_SW_NE = 24427,
    SPR_WOODEN_RC_60_DEG_CHAIN_RAILS_SW_NE = 24427,
    SPR_WOODEN_RC_60_DEG_RAILS_NW_SE = 24428,
    SPR_WOODEN_RC_60_DEG_CHAIN_RAILS_NW_SE = 24428,
    SPR_WOODEN_RC_60_DEG_RAILS_NE_SW = 24429,
    SPR_WOODEN_RC_60_DEG_CHAIN_RAILS_NE_SW = 24429,
    SPR_WOODEN_RC_60_DEG_RAILS_SE_NW = 24430,
    SPR_WOODEN_RC_60_DEG_CHAIN_RAILS_SE_NW = 24430,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_FRONT_NW_SE = 24431,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_FRONT_NE_SW = 24432,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_FRONT_NW_SE = 24433,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_FRONT_NE_SW = 24434,
    SPR_WOODEN_RC_25_DEG_RAILS_FRONT_NW_SE = 24435,
    SPR_WOODEN_RC_60_DEG_RAILS_FRONT_NW_SE = 24435,
    SPR_WOODEN_RC_25_DEG_RAILS_FRONT_NE_SW = 24436,
    SPR_WOODEN_RC_60_DEG_RAILS_FRONT_NE_SW = 24436,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_SW_NE = 24437,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_NW_SE = 24438,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_NE_SW = 24439,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_SE_NW = 24440,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_SW_NE = 24441,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_NW_SE = 24442,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_NE_SW = 24443,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_SE_NW = 24444,
    SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_SW_NE = 24445,
    SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_NW_SE = 24446,
    SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_NE_SW = 24447,
    SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_SE_NW = 24448,

    SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_FRONT_NW_SE = 24465,
    SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_FRONT_NE_SW = 24466,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_FRONT_NW_SE = 24467,
    SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_FRONT_NE_SW = 24468,
    SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_FRONT_NW_SE = 24469,
    SPR_WOODEN_RC_60_DEG_CHAIN_RAILS_FRONT_NW_SE = 24469,
    SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_FRONT_NE_SW = 24470,
    SPR_WOODEN_RC_60_DEG_CHAIN_RAILS_FRONT_NE_SW = 24470,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SW_SE_SEQ_0 = 24471,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SW_SE_SEQ_2 = 24472,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SW_SE_SEQ_3 = 24473,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SW_SE_SEQ_5 = 24474,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SW_SE_SEQ_6 = 24475,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NW_SW_SEQ_0 = 24476,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NW_SW_SEQ_2 = 24477,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NW_SW_SEQ_3 = 24478,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NW_SW_SEQ_5 = 24479,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NW_SW_SEQ_6 = 24480,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NE_NW_SEQ_0 = 24481,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NE_NW_SEQ_2 = 24482,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NE_NW_SEQ_3 = 24483,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NE_NW_SEQ_5 = 24484,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NE_NW_SEQ_6 = 24485,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SE_NE_SEQ_0 = 24486,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SE_NE_SEQ_2 = 24487,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SE_NE_SEQ_3 = 24488,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SE_NE_SEQ_5 = 24489,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SE_NE_SEQ_6 = 24490,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_0 = 24491,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_2 = 24492,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_3 = 24493,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_5 = 24494,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_6 = 24495,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_0 = 24496,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_2 = 24497,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_3 = 24498,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_5 = 24499,
    SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_6 = 24500,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SW_SE_SEQ_0 = 24501,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SW_SE_SEQ_2 = 24502,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SW_SE_SEQ_3 = 24503,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SW_SE_SEQ_5 = 24504,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SW_SE_SEQ_6 = 24505,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NW_SW_SEQ_0 = 24506,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NW_SW_SEQ_2 = 24507,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NW_SW_SEQ_3 = 24508,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NW_SW_SEQ_5 = 24509,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NW_SW_SEQ_6 = 24510,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NE_NW_SEQ_0 = 24511,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NE_NW_SEQ_2 = 24512,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NE_NW_SEQ_3 = 24513,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NE_NW_SEQ_5 = 24514,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NE_NW_SEQ_6 = 24515,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SE_NE_SEQ_0 = 24516,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SE_NE_SEQ_2 = 24517,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SE_NE_SEQ_3 = 24518,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SE_NE_SEQ_5 = 24519,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SE_NE_SEQ_6 = 24520,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_0 = 24521,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_2 = 24522,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_3 = 24523,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_5 = 24524,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_6 = 24525,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_0 = 24526,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_2 = 24527,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_3 = 24528,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_5 = 24529,
    SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_6 = 24530,

    SPR_WOODEN_RC_FLAT_CHAIN_RAILS_SW_NE = 24615,
    SPR_WOODEN_RC_FLAT_CHAIN_RAILS_NW_SE = 24616,
    SPR_WOODEN_RC_FLAT_CHAIN_RAILS_NE_SW = 24617,
    SPR_WOODEN_RC_FLAT_CHAIN_RAILS_SE_NW = 24618,
    SPR_WOODEN_RC_FLAT_RAILS_SW_NE = 24619,
    SPR_WOODEN_RC_FLAT_RAILS_NW_SE = 24620,
    SPR_WOODEN_RC_BRAKES_RAILS_SW_NE = 24621,
    SPR_WOODEN_RC_BRAKES_RAILS_NW_SE = 24622,
    SPR_WOODEN_RC_BLOCK_BRAKES_RAILS_SW_NE = 24623,
    SPR_WOODEN_RC_BLOCK_BRAKES_RAILS_NW_SE = 24624,

    SPR_WOODEN_RC_STATION_RAILS_SW_NE = 24839,
    SPR_WOODEN_RC_STATION_RAILS_NW_SE = 24840,
};

static const uint32 _wooden_rc_block_brakes_image_ids[4][2] = {
    { SPR_WOODEN_RC_BLOCK_BRAKES_SW_NE, SPR_WOODEN_RC_BLOCK_BRAKES_RAILS_SW_NE },
    { SPR_WOODEN_RC_BLOCK_BRAKES_NW_SE, SPR_WOODEN_RC_BLOCK_BRAKES_RAILS_NW_SE },
    { SPR_WOODEN_RC_BLOCK_BRAKES_SW_NE, SPR_WOODEN_RC_BLOCK_BRAKES_RAILS_SW_NE },
    { SPR_WOODEN_RC_BLOCK_BRAKES_NW_SE, SPR_WOODEN_RC_BLOCK_BRAKES_RAILS_NW_SE },
};

static uint32 wooden_rc_get_track_colour()
{
    return (gTrackColours[SCHEME_TRACK] & ~0xF80000) | gTrackColours[SCHEME_SUPPORTS];
}

static uint32 wooden_rc_get_rails_colour()
{
    return gTrackColours[SCHEME_TRACK];
}

static paint_struct * wooden_rc_track_paint(
    uint32 imageIdTrack,
    uint32 imageIdRails,
    uint8 direction,
    sint8 x_offset, sint8 y_offset,
    sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
    sint16 z_offset,
    sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z
) {
    uint32 imageId = imageIdTrack | wooden_rc_get_track_colour();
    uint32 railsImageId = imageIdRails | wooden_rc_get_rails_colour();

    sub_98197C_rotated(direction, imageId, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset, bound_box_offset_x, bound_box_offset_y, bound_box_offset_z);
    return sub_98199C_rotated(direction, railsImageId, x_offset, y_offset, bound_box_length_x, bound_box_length_y, bound_box_length_z, z_offset, bound_box_offset_x, bound_box_offset_y, bound_box_offset_z);
}

static void wooden_rc_track_paint_bb(const sprite_bb_2 *bb, sint16 height)
{
    if (bb->sprite_id_a == 0) return;

    uint32 imageId = bb->sprite_id_a | wooden_rc_get_track_colour();
    uint32 railsImageId = bb->sprite_id_b | wooden_rc_get_rails_colour();

    sub_98197C(imageId, (sint8)bb->offset.x, (sint8)bb->offset.y, bb->bb_size.x, bb->bb_size.y, (sint8)bb->bb_size.z, height + bb->offset.z, bb->bb_offset.x, bb->bb_offset.y, height + bb->bb_offset.z, get_current_rotation());
    sub_98199C(railsImageId, (sint8)bb->offset.x, (sint8)bb->offset.y, bb->bb_size.x, bb->bb_size.y, (sint8)bb->bb_size.z, height + bb->offset.z, bb->bb_offset.x, bb->bb_offset.y, height + bb->bb_offset.z, get_current_rotation());
}

/** rct2: 0x008AC568 */
static void wooden_rc_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] = {
        { SPR_WOODEN_RC_FLAT_SW_NE, SPR_WOODEN_RC_FLAT_CHAIN_SW_NE },
        { SPR_WOODEN_RC_FLAT_NW_SE, SPR_WOODEN_RC_FLAT_CHAIN_NW_SE },
        { SPR_WOODEN_RC_FLAT_SW_NE, SPR_WOODEN_RC_FLAT_CHAIN_NE_SW },
        { SPR_WOODEN_RC_FLAT_NW_SE, SPR_WOODEN_RC_FLAT_CHAIN_SE_NW },
    };
    static const uint32 railsImageIds[4][2] = {
        { SPR_WOODEN_RC_FLAT_RAILS_SW_NE, SPR_WOODEN_RC_FLAT_CHAIN_RAILS_SW_NE },
        { SPR_WOODEN_RC_FLAT_RAILS_NW_SE, SPR_WOODEN_RC_FLAT_CHAIN_RAILS_NW_SE },
        { SPR_WOODEN_RC_FLAT_RAILS_SW_NE, SPR_WOODEN_RC_FLAT_CHAIN_RAILS_NE_SW },
        { SPR_WOODEN_RC_FLAT_RAILS_NW_SE, SPR_WOODEN_RC_FLAT_CHAIN_RAILS_SE_NW },
    };

    uint8 isChained = track_element_is_lift_hill(mapElement) ? 1 : 0;
    wooden_rc_track_paint(imageIds[direction][isChained], railsImageIds[direction][isChained], direction, 0, 2, 32, 25, 2, height, 0, 3, height);
    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static void wooden_rc_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 stationImageIds[4][2] = {
        { SPR_WOODEN_RC_STATION_SW_NE, SPR_WOODEN_RC_STATION_RAILS_SW_NE },
        { SPR_WOODEN_RC_STATION_NW_SE, SPR_WOODEN_RC_STATION_RAILS_NW_SE },
        { SPR_WOODEN_RC_STATION_SW_NE, SPR_WOODEN_RC_STATION_RAILS_SW_NE },
        { SPR_WOODEN_RC_STATION_NW_SE, SPR_WOODEN_RC_STATION_RAILS_NW_SE },
    };

    sint32 trackType = mapElement->properties.track.type;
    if (trackType == TRACK_ELEM_END_STATION) {
        wooden_rc_track_paint(
            _wooden_rc_block_brakes_image_ids[direction][0],
            _wooden_rc_block_brakes_image_ids[direction][1], direction, 0, 2, 32, 27, 2, height, 0, 2, height);
    } else {
        wooden_rc_track_paint(
            stationImageIds[direction][0],
            stationImageIds[direction][1], direction, 0, 2, 32, 27, 2, height, 0, 2, height);
    }
    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    track_paint_util_draw_station_2(rideIndex, trackSequence, direction, height, mapElement, 9, 11);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AC578 */
static void wooden_rc_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[2][4][4] = {
        {
            { SPR_WOODEN_RC_25_DEG_SW_NE, SPR_WOODEN_RC_25_DEG_RAILS_SW_NE, 0, 0 },
            { SPR_WOODEN_RC_25_DEG_NW_SE, SPR_WOODEN_RC_25_DEG_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_RAILS_FRONT_NW_SE },
            { SPR_WOODEN_RC_25_DEG_NE_SW, SPR_WOODEN_RC_25_DEG_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_RAILS_FRONT_NE_SW },
            { SPR_WOODEN_RC_25_DEG_SE_NW, SPR_WOODEN_RC_25_DEG_RAILS_SE_NW, 0, 0 },
        },
        {
            { SPR_WOODEN_RC_25_DEG_CHAIN_SW_NE, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_SW_NE, 0, 0 },
            { SPR_WOODEN_RC_25_DEG_CHAIN_NW_SE, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_CHAIN_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_FRONT_NW_SE },
            { SPR_WOODEN_RC_25_DEG_CHAIN_NE_SW, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_CHAIN_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_FRONT_NE_SW },
            { SPR_WOODEN_RC_25_DEG_CHAIN_SE_NW, SPR_WOODEN_RC_25_DEG_CHAIN_RAILS_SE_NW, 0, 0 },
        }
    };

    uint8 isChained = track_element_is_lift_hill(mapElement) ? 1 : 0;
    wooden_rc_track_paint(imageIds[isChained][direction][0], imageIds[isChained][direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    if (direction == 1 || direction == 2) {
        wooden_rc_track_paint(imageIds[isChained][direction][2], imageIds[isChained][direction][3], direction, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
    }
    wooden_a_supports_paint_setup(direction & 1, 9 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AC588 */
static void wooden_rc_track_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] = {
        { SPR_WOODEN_RC_60_DEG_SW_NE, SPR_WOODEN_RC_60_DEG_RAILS_SW_NE },
        { SPR_WOODEN_RC_60_DEG_NW_SE, SPR_WOODEN_RC_60_DEG_RAILS_NW_SE },
        { SPR_WOODEN_RC_60_DEG_NE_SW, SPR_WOODEN_RC_60_DEG_RAILS_NE_SW },
        { SPR_WOODEN_RC_60_DEG_SE_NW, SPR_WOODEN_RC_60_DEG_RAILS_SE_NW },
    };

    if (direction == 0 || direction == 3) {
        wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    } else {
        gPaintSession.WoodenSupportsPrependTo = wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 2, 24, 93, height, 28, 4, height - 16);
    }
    wooden_a_supports_paint_setup(direction & 1, 21 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 56, TUNNEL_8);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 104, 0x20);
}

/** rct2: 0x008AC598 */
static void wooden_rc_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[2][4][4] = {
        {
            { SPR_WOODEN_RC_FLAT_TO_25_DEG_SW_NE, SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_SW_NE, 0, 0 },
            { SPR_WOODEN_RC_FLAT_TO_25_DEG_NW_SE, SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_NW_SE, SPR_WOODEN_RC_FLAT_TO_25_DEG_FRONT_NW_SE, SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_FRONT_NW_SE },
            { SPR_WOODEN_RC_FLAT_TO_25_DEG_NE_SW, SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_NE_SW, SPR_WOODEN_RC_FLAT_TO_25_DEG_FRONT_NE_SW, SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_FRONT_NE_SW },
            { SPR_WOODEN_RC_FLAT_TO_25_DEG_SE_NW, SPR_WOODEN_RC_FLAT_TO_25_DEG_RAILS_SE_NW, 0, 0 },
        },
        {
            { SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_SW_NE, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_SW_NE, 0, 0 },
            { SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_NW_SE, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_NW_SE, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_FRONT_NW_SE, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_FRONT_NW_SE },
            { SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_NE_SW, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_NE_SW, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_FRONT_NE_SW, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_FRONT_NE_SW },
            { SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_SE_NW, SPR_WOODEN_RC_FLAT_TO_25_DEG_CHAIN_RAILS_SE_NW, 0, 0 },
        }
    };

    uint8 isChained = track_element_is_lift_hill(mapElement) ? 1 : 0;
    wooden_rc_track_paint(imageIds[isChained][direction][0], imageIds[isChained][direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    if (direction == 1 || direction == 2) {
        wooden_rc_track_paint(imageIds[isChained][direction][2], imageIds[isChained][direction][3], direction, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
    }
    wooden_a_supports_paint_setup(direction & 1, 1 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AC5A8 */
static void wooden_rc_track_25_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][4] = {
        { SPR_WOODEN_RC_25_DEG_TO_60_DEG_SW_NE, SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_SW_NE, 0, 0 },
        { SPR_WOODEN_RC_25_DEG_TO_60_DEG_NW_SE, SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_TO_60_DEG_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_FRONT_NW_SE },
        { SPR_WOODEN_RC_25_DEG_TO_60_DEG_NE_SW, SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_TO_60_DEG_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_FRONT_NE_SW },
        { SPR_WOODEN_RC_25_DEG_TO_60_DEG_SE_NW, SPR_WOODEN_RC_25_DEG_TO_60_DEG_RAILS_SE_NW, 0, 0 },
    };

    if (direction == 0 || direction == 3) {
        wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    } else {
        gPaintSession.WoodenSupportsPrependTo = wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 2, 24, 43, height, 28, 4, height + 2);
        wooden_rc_track_paint(imageIds[direction][2], imageIds[direction][3], direction, 0, 0, 32, 2, 43, height, 0, 4, height);
    }
    wooden_a_supports_paint_setup(direction & 1, 13 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_8);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x008AC5B8 */
static void wooden_rc_track_60_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][4] = {
        { SPR_WOODEN_RC_60_DEG_TO_25_DEG_SW_NE, SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_SW_NE, 0, 0 },
        { SPR_WOODEN_RC_60_DEG_TO_25_DEG_NW_SE, SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_NW_SE, SPR_WOODEN_RC_60_DEG_TO_25_DEG_FRONT_NW_SE, SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_FRONT_NW_SE },
        { SPR_WOODEN_RC_60_DEG_TO_25_DEG_NE_SW, SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_NE_SW, SPR_WOODEN_RC_60_DEG_TO_25_DEG_FRONT_NE_SW, SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_FRONT_NE_SW },
        { SPR_WOODEN_RC_60_DEG_TO_25_DEG_SE_NW, SPR_WOODEN_RC_60_DEG_TO_25_DEG_RAILS_SE_NW, 0, 0 },
    };

    if (direction == 0 || direction == 3) {
        wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    } else {
        gPaintSession.WoodenSupportsPrependTo = wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 24, 1, 61, height, 4, 28, height - 16);
        wooden_rc_track_paint(imageIds[direction][2], imageIds[direction][3], direction, 0, 0, 32, 2, 43, height, 0, 4, height);
    }
    wooden_a_supports_paint_setup(direction & 1, 17 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 24, TUNNEL_8);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 72, 0x20);
}

/** rct2: 0x008AC5C8 */
static void wooden_rc_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[2][4][4] = {
        {
            { SPR_WOODEN_RC_25_DEG_TO_FLAT_SW_NE, SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_SW_NE, 0, 0 },
            { SPR_WOODEN_RC_25_DEG_TO_FLAT_NW_SE, SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_TO_FLAT_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_FRONT_NW_SE },
            { SPR_WOODEN_RC_25_DEG_TO_FLAT_NE_SW, SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_TO_FLAT_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_FRONT_NE_SW },
            { SPR_WOODEN_RC_25_DEG_TO_FLAT_SE_NW, SPR_WOODEN_RC_25_DEG_TO_FLAT_RAILS_SE_NW, 0, 0 },
        },
        {
            { SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_SW_NE, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_SW_NE, 0, 0 },
            { SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_NW_SE, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_FRONT_NW_SE },
            { SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_NE_SW, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_FRONT_NE_SW },
            { SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_SE_NW, SPR_WOODEN_RC_25_DEG_TO_FLAT_CHAIN_RAILS_SE_NW, 0, 0 },
        }
    };

    uint8 isChained = track_element_is_lift_hill(mapElement) ? 1 : 0;
    wooden_rc_track_paint(imageIds[isChained][direction][0], imageIds[isChained][direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    if (direction == 1 || direction == 2) {
        wooden_rc_track_paint(imageIds[isChained][direction][2], imageIds[isChained][direction][3], direction, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
    }
    wooden_a_supports_paint_setup(direction & 1, 5 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);

    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008AC5D8 */
static void wooden_rc_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC5E8 */
static void wooden_rc_track_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC5F8 */
static void wooden_rc_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC608 */
static void wooden_rc_track_25_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC618 */
static void wooden_rc_track_60_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC628 */
static void wooden_rc_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void wooden_rc_track_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const sprite_bb_2 imageIds[2][4][7] = {
        {
            {
                { SPR_WOODEN_RC_QUARTER_TURN_5_SW_SE_SEQ_0, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SW_SE_SEQ_0, {  0,  2, 0 }, {  0,  2, 0 }, { 32, 32, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_SW_SE_SEQ_2, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SW_SE_SEQ_2, {  0, 16, 0 }, {  0, 16, 0 }, { 32, 16, 2 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_SW_SE_SEQ_3, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SW_SE_SEQ_3, {  0,  0, 0 }, {  0,  0, 0 }, { 16, 16, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_SW_SE_SEQ_5, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SW_SE_SEQ_5, { 16,  0, 0 }, { 16,  0, 0 }, { 16, 32, 2 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_SW_SE_SEQ_6, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SW_SE_SEQ_6, {  2,  0, 0 }, {  2,  0, 0 }, { 32, 32, 2 } },
            },
            {
                { SPR_WOODEN_RC_QUARTER_TURN_5_NW_SW_SEQ_0, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NW_SW_SEQ_0, {  2,  0, 0 }, {  2,  0, 0 }, { 32, 32, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_NW_SW_SEQ_2, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NW_SW_SEQ_2, { 16,  0, 0 }, {  16, 0, 0 }, { 16, 34, 2 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_NW_SW_SEQ_3, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NW_SW_SEQ_3, {  0, 16, 0 }, {  0, 16, 0 }, { 16, 16, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_NW_SW_SEQ_5, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NW_SW_SEQ_5, {  0,  0, 0 }, {  0,  0, 0 }, { 32, 16, 2 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_NW_SW_SEQ_6, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NW_SW_SEQ_6, {  0,  2, 0 }, {  0,  2, 0 }, { 32, 27, 2 } },
            },
            {
                { SPR_WOODEN_RC_QUARTER_TURN_5_NE_NW_SEQ_0, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NE_NW_SEQ_0, {  0,  2, 0 }, {  0,  2, 0 }, { 32, 27, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_NE_NW_SEQ_2, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NE_NW_SEQ_2, {  0,  0, 0 }, {  0,  0, 0 }, { 32, 16, 2 }},
                { SPR_WOODEN_RC_QUARTER_TURN_5_NE_NW_SEQ_3, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NE_NW_SEQ_3, { 16, 16, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_NE_NW_SEQ_5, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NE_NW_SEQ_5, {  0,  0, 0 }, {  0,  0, 0 }, { 16, 32, 2 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_NE_NW_SEQ_6, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_NE_NW_SEQ_6, {  2,  0, 0 }, {  2,  0, 0 }, { 27, 32, 2 } },
            },
            {
                { SPR_WOODEN_RC_QUARTER_TURN_5_SE_NE_SEQ_0, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SE_NE_SEQ_0, {  2,  0, 0 }, {  2,  0, 0 }, { 27, 32, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_SE_NE_SEQ_2, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SE_NE_SEQ_2, {  0,  0, 0 }, {  0,  0, 0 }, { 16, 32, 2 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_SE_NE_SEQ_3, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SE_NE_SEQ_3, { 16,  0, 0 }, { 16,  0, 0 }, { 16, 16, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_SE_NE_SEQ_5, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SE_NE_SEQ_5, {  0, 16, 0 }, {  0, 16, 0 }, { 32, 16, 2 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_SE_NE_SEQ_6, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_SE_NE_SEQ_6, {  0,  2, 0 }, {  0,  2, 0 }, { 32, 32, 2 } },
            }
        },
        {
            {
                { SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_SW_SE_SEQ_0, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_0, {  0,  2, 0 }, {  0,  2, 27 }, { 32, 32, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_SW_SE_SEQ_2, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_2, {  0, 16, 0 }, {  0, 16, 27 }, { 32, 16, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_SW_SE_SEQ_3, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_3, {  0,  0, 0 }, {  0,  0, 27 }, { 16, 16, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_SW_SE_SEQ_5, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_5, { 16,  0, 0 }, { 16,  0, 27 }, { 16, 32, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_SW_SE_SEQ_6, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_6, {  2,  0, 0 }, {  2,  0, 27 }, { 32, 32, 0 } },
            },
            {
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
            },
            {
                { SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_NE_NW_SEQ_0, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_0, {  0,  2, 0 }, {  0,  2, 27 }, { 32, 27, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_NE_NW_SEQ_2, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_2, {  0,  0, 0 }, {  0,  0, 27 }, { 32, 16, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_NE_NW_SEQ_3, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_3, { 16, 16, 0 }, { 16, 16, 27 }, { 16, 16, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_NE_NW_SEQ_5, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_5, {  0,  0, 0 }, {  0,  0, 27 }, { 16, 32, 0 } },
                { SPR_WOODEN_RC_QUARTER_TURN_5_FRONT_NE_NW_SEQ_6, SPR_WOODEN_RC_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_6, {  2,  0, 0 }, {  2,  0, 27 }, { 27, 32, 0 } },
            },
            {
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
            }
        },
    };
    static sint8 supportType[4][7] = {
        { 0, -1, 4, 2, -1, 4, 1 },
        { 1, -1, 5, 3, -1, 5, 0 },
        { 0, -1, 2, 4, -1, 2, 1 },
        { 1, -1, 3, 5, -1, 3, 0 },
    };

    wooden_rc_track_paint_bb(&imageIds[0][direction][trackSequence], height);
    wooden_rc_track_paint_bb(&imageIds[1][direction][trackSequence], height);
    track_paint_util_right_quarter_turn_5_tiles_tunnel(height, direction, trackSequence, TUNNEL_6);

    if (supportType[direction][trackSequence] != -1) {
        wooden_a_supports_paint_setup(supportType[direction][trackSequence], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    }

    sint32 blockedSegments = 0;
    switch (trackSequence) {
    case 0: blockedSegments = SEGMENTS_ALL; break;
    case 1: blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC; break;
    case 2: blockedSegments = SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4; break;
    case 3: blockedSegments = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4; break;
    case 4: blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC; break;
    case 5: blockedSegments = SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4; break;
    case 6: blockedSegments = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4; break;
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static void wooden_rc_track_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    wooden_rc_track_right_quarter_turn_5(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008AC658 */
static void wooden_rc_track_flat_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][4] = {
        { SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_SW_NE, SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_RAILS_SW_NE, 0, 0 },
        { SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_NW_SE, SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_RAILS_NW_SE, SPR_WOODEN_RC_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE, SPR_WOODEN_RC_RC_FLAT_TO_LEFT_BANK_RAILS_FRONT_NW_SE },
        { SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_NE_SW, SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_RAILS_NE_SW, 0, 0 },
        { SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_SE_NW, SPR_WOODEN_RC_FLAT_TO_LEFT_BANK_RAILS_SE_NW, SPR_WOODEN_RC_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW, SPR_WOODEN_RC_RC_FLAT_TO_LEFT_BANK_RAILS_FRONT_SE_NW },
    };

    wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    if (direction == 1 || direction == 3) {
        wooden_rc_track_paint(imageIds[direction][2], imageIds[direction][3], direction, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
    }
    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AC668 */
static void wooden_rc_track_flat_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][4] = {
        { SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_SW_NE, SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_RAILS_SW_NE, SPR_WOODEN_RC_RC_FLAT_TO_RIGHT_BANK_FRONT_SW_NE, SPR_WOODEN_RC_RC_FLAT_TO_RIGHT_BANK_RAILS_FRONT_SW_NE },
        { SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_NW_SE, SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_RAILS_NW_SE, 0, 0 },
        { SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_NE_SW, SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_RAILS_NE_SW, SPR_WOODEN_RC_RC_FLAT_TO_RIGHT_BANK_FRONT_NE_SW, SPR_WOODEN_RC_RC_FLAT_TO_RIGHT_BANK_RAILS_FRONT_NE_SW },
        { SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_SE_NW, SPR_WOODEN_RC_FLAT_TO_RIGHT_BANK_RAILS_SE_NW, 0, 0 },
    };

    wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    if (direction == 0 || direction == 2) {
        wooden_rc_track_paint(imageIds[direction][2], imageIds[direction][3], direction, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
    }
    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AC678 */
static void wooden_rc_track_left_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_flat_to_right_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC688 */
static void wooden_rc_track_right_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_flat_to_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

static void wooden_rc_track_banked_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const sprite_bb_2 imageIds[2][4][7] = {
        {
            {
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_0, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SW_SE_SEQ_0, { 0, 0, 0 }, {  0,  2, 0 }, { 32, 32, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_2, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SW_SE_SEQ_2, { 0, 0, 0 }, {  0, 16, 0 }, { 32, 16, 2 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_3, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SW_SE_SEQ_3, { 0, 0, 0 }, {  0,  0, 0 }, { 16, 16, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_5, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SW_SE_SEQ_5, { 0, 0, 0 }, { 16,  0, 0 }, { 16, 32, 2 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_6, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SW_SE_SEQ_6, { 0, 0, 0 }, {  2,  0, 0 }, { 32, 32, 2 } },
            },
            {
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_0, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NW_SW_SEQ_0, { 0, 0, 0 }, {  2,  0, 0 }, { 32, 32, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_2, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NW_SW_SEQ_2, { 0, 0, 0 }, {  16, 0, 0 }, { 16, 32, 2 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_3, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NW_SW_SEQ_3, { 0, 0, 0 }, {  0, 16, 0 }, { 16, 16, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_5, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NW_SW_SEQ_5, { 0, 0, 0 }, {  0,  0, 0 }, { 32, 16, 2 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_6, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NW_SW_SEQ_6, { 0, 0, 0 }, {  0,  2, 0 }, { 32, 27, 2 } },
            },
            {
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_0, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NE_NW_SEQ_0, { 0, 0, 0 }, {  0,  2, 0 }, { 32, 27, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_2, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NE_NW_SEQ_2, { 0, 0, 0 }, {  0,  0, 0 }, { 32, 16, 2 }},
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_3, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NE_NW_SEQ_3, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_5, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NE_NW_SEQ_5, { 0, 0, 0 }, {  0,  0, 0 }, { 16, 32, 2 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_6, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_NE_NW_SEQ_6, { 0, 0, 0 }, {  2,  0, 0 }, { 27, 32, 2 } },
            },
            {
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_0, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SE_NE_SEQ_0, { 0, 0, 0 }, {  2,  0, 0 }, { 27, 32, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_2, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SE_NE_SEQ_2, { 0, 0, 0 }, {  0,  0, 0 }, { 16, 32, 2 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_3, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SE_NE_SEQ_3, { 0, 0, 0 }, { 16,  0, 0 }, { 16, 16, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_5, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SE_NE_SEQ_5, { 0, 0, 0 }, {  0, 16, 0 }, { 32, 16, 2 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_6, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_SE_NE_SEQ_6, { 0, 0, 0 }, {  0,  2, 0 }, { 32, 32, 2 } },
            }
        },
        {
            {
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_0, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_0, { 0, 0, 0 }, {  0,  2, 27 }, { 32, 32, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_2, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_2, { 0, 0, 0 }, {  0, 16, 27 }, { 32, 16, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_3, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_3, { 0, 0, 0 }, {  0,  0, 27 }, { 16, 16, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_5, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_5, { 0, 0, 0 }, { 16,  0, 27 }, { 16, 32, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_6, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_SW_SE_SEQ_6, { 0, 0, 0 }, {  2,  0, 27 }, { 32, 32, 0 } },
            },
            {
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
            },
            {
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_0, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_0, { 0, 0, 0 }, {  0,  2, 27 }, { 32, 27, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_2, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_2, { 0, 0, 0 }, {  0,  0, 27 }, { 32, 16, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_3, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_3, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_5, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_5, { 0, 0, 0 }, {  0,  0, 27 }, { 16, 32, 0 } },
                { SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_6, SPR_WOODEN_RC_BANKED_QUARTER_TURN_5_RAILS_FRONT_NE_NW_SEQ_6, { 0, 0, 0 }, {  2,  0, 27 }, { 27, 32, 0 } },
            },
            {
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
            }
        },
    };
    static sint8 supportType[4][7] = {
        { 0, -1, 4, 2, -1, 4, 1 },
        { 1, -1, 5, 3, -1, 5, 0 },
        { 0, -1, 2, 4, -1, 2, 1 },
        { 1, -1, 3, 5, -1, 3, 0 },
    };

    wooden_rc_track_paint_bb(&imageIds[0][direction][trackSequence], height);
    wooden_rc_track_paint_bb(&imageIds[1][direction][trackSequence], height);
    track_paint_util_right_quarter_turn_5_tiles_tunnel(height, direction, trackSequence, TUNNEL_6);

    if (supportType[direction][trackSequence] != -1) {
        wooden_a_supports_paint_setup(supportType[direction][trackSequence], 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    }

    sint32 blockedSegments = 0;
    switch (trackSequence) {
    case 0: blockedSegments = SEGMENTS_ALL; break;
    case 1: blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC; break;
    case 2: blockedSegments = SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4; break;
    case 3: blockedSegments = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4; break;
    case 4: blockedSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC; break;
    case 5: blockedSegments = SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4; break;
    case 6: blockedSegments = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4; break;
    }
    paint_util_set_segment_support_height(paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

static void wooden_rc_track_banked_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    wooden_rc_track_banked_right_quarter_turn_5(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008AC6B8 */
static void wooden_rc_track_left_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][4] = {
        { SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_SW_NE, SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_RAILS_SW_NE, 0, 0 },
        { SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_NW_SE, SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_RAILS_NW_SE, SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_FRONT_NW_SE, SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_RAILS_FRONT_NW_SE },
        { SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_NE_SW, SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_RAILS_NE_SW, SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_FRONT_NE_SW, SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_RAILS_FRONT_NE_SW },
        { SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_SE_NW, SPR_WOODEN_RC_LEFT_BANK_TO_25_DEG_RAILS_SE_NW, 0, 0 },
    };

    wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    if (direction == 1 || direction == 2) {
        wooden_rc_track_paint(imageIds[direction][2], imageIds[direction][3], direction, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
    }
    wooden_a_supports_paint_setup(direction & 1, 1 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AC6C8 */
static void wooden_rc_track_right_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][4] = {
        { SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_SW_NE, SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_RAILS_SW_NE, 0, 0 },
        { SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_NW_SE, SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_RAILS_NW_SE, SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_FRONT_NW_SE, SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_RAILS_FRONT_NW_SE },
        { SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_NE_SW, SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_RAILS_NE_SW, SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_FRONT_NE_SW, SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_RAILS_FRONT_NE_SW },
        { SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_SE_NW, SPR_WOODEN_RC_RIGHT_BANK_TO_25_DEG_RAILS_SE_NW, 0, 0 },
    };

    wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    if (direction == 1 || direction == 2) {
        wooden_rc_track_paint(imageIds[direction][2], imageIds[direction][3], direction, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
    }
    wooden_a_supports_paint_setup(direction & 1, 1 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008AC6D8 */
static void wooden_rc_track_25_deg_up_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][4] = {
        { SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_SW_NE, SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_RAILS_SW_NE, 0, 0 },
        { SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_NW_SE, SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_RAILS_FRONT_NW_SE },
        { SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_NE_SW, SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_RAILS_FRONT_NE_SW },
        { SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_SE_NW, SPR_WOODEN_RC_25_DEG_TO_LEFT_BANK_RAILS_SE_NW, 0, 0 },
    };

    wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    if (direction == 1 || direction == 2) {
        wooden_rc_track_paint(imageIds[direction][2], imageIds[direction][3], direction, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
    }
    wooden_a_supports_paint_setup(direction & 1, 5 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008AC6E8 */
static void wooden_rc_track_25_deg_up_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][4] = {
        { SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_SW_NE, SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_RAILS_SW_NE, 0, 0 },
        { SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_NW_SE, SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_RAILS_NW_SE, SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_FRONT_NW_SE, SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_RAILS_FRONT_NW_SE },
        { SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_NE_SW, SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_RAILS_NE_SW, SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_FRONT_NE_SW, SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_RAILS_FRONT_NE_SW },
        { SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_SE_NW, SPR_WOODEN_RC_25_DEG_TO_RIGHT_BANK_RAILS_SE_NW, 0, 0 },
    };

    wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    if (direction == 1 || direction == 2) {
        wooden_rc_track_paint(imageIds[direction][2], imageIds[direction][3], direction, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
    }
    wooden_a_supports_paint_setup(direction & 1, 5 + direction, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008AC6F8 */
static void wooden_rc_track_left_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_25_deg_up_to_right_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC708 */
static void wooden_rc_track_right_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_25_deg_up_to_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC718 */
static void wooden_rc_track_25_deg_down_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_right_bank_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC728 */
static void wooden_rc_track_25_deg_down_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_left_bank_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC738 */
static void wooden_rc_track_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] = {
        { SPR_WOODEN_RC_LEFT_BANK_SW_NE, SPR_WOODEN_RC_LEFT_BANK_RAILS_SW_NE },
        { SPR_WOODEN_RC_LEFT_BANK_NW_SE, SPR_WOODEN_RC_LEFT_BANK_RAILS_NW_SE },
        { SPR_WOODEN_RC_LEFT_BANK_NE_SW, SPR_WOODEN_RC_LEFT_BANK_RAILS_NE_SW },
        { SPR_WOODEN_RC_LEFT_BANK_SE_NW, SPR_WOODEN_RC_LEFT_BANK_RAILS_SE_NW },
    };

    wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 0, 32, 25, 2, height, 0, 3, height);
    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008AC748 */
static void wooden_rc_track_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC758 */
static void wooden_rc_track_left_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23781, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24647, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23786, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24652, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23812, 0, 0, 32, 27, 0, height, 0, 2, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24678, 0, 0, 32, 27, 0, height, 0, 2, height + 67);
            wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23791, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24657, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23796, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24662, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23782, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24648, 0, 0, 32, 16, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23787, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24653, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23813, 0, 0, 32, 16, 0, height, 0, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24679, 0, 0, 32, 16, 0, height, 0, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23792, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24658, 0, 0, 32, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23797, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24663, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23819, 0, 0, 32, 16, 0, height, 0, 16, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24685, 0, 0, 32, 16, 0, height, 0, 16, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23783, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24649, 0, 0, 16, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23788, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24654, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23814, 0, 0, 16, 16, 0, height, 16, 16, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24680, 0, 0, 16, 16, 0, height, 16, 16, height + 59);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23793, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24659, 0, 0, 16, 16, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23798, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24664, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23820, 0, 0, 16, 16, 0, height, 0, 0, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24686, 0, 0, 16, 16, 0, height, 0, 0, height + 59);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23784, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24650, 0, 0, 16, 32, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23789, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24655, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23815, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24681, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23794, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24660, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23817, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24683, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23799, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24665, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23821, 0, 0, 16, 32, 0, height, 16, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24687, 0, 0, 16, 32, 0, height, 16, 0, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23785, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24651, 0, 0, 27, 32, 2, height, 2, 0, height);
            wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23790, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24656, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23816, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24682, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23795, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24661, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23818, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24684, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23800, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24666, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23822, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24688, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_8);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AC768 */
static void wooden_rc_track_right_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23761, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24627, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23766, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24632, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23771, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24637, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23807, 0, 0, 32, 27, 0, height, 0, 2, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24673, 0, 0, 32, 27, 0, height, 0, 2, height + 67);
            wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23776, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24642, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23762, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24628, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23801, 0, 0, 32, 16, 0, height, 0, 16, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24667, 0, 0, 32, 16, 0, height, 0, 16, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23767, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24633, 0, 0, 32, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23772, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24638, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23808, 0, 0, 32, 16, 0, height, 0, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24674, 0, 0, 32, 16, 0, height, 0, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23777, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24643, 0, 0, 32, 16, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23763, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24629, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23802, 0, 0, 16, 16, 0, height, 0, 0, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24668, 0, 0, 16, 16, 0, height, 0, 0, height + 59);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23768, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24634, 0, 0, 16, 16, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23773, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24639, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23809, 0, 0, 16, 16, 0, height, 16, 16, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24675, 0, 0, 16, 16, 0, height, 16, 16, height + 59);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23778, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24644, 0, 0, 16, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23764, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24630, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23803, 0, 0, 16, 32, 0, height, 16, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24669, 0, 0, 16, 32, 0, height, 16, 0, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23769, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24635, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23805, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24671, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23774, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24640, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23810, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24676, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23779, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24645, 0, 0, 16, 32, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23765, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24631, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23804, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24670, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23770, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24636, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23806, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24672, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23775, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24641, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23811, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24677, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23780, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24646, 0, 0, 27, 32, 2, height, 2, 0, height);
            wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_8);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AC778 */
static void wooden_rc_track_left_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    wooden_rc_track_right_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008AC788 */
static void wooden_rc_track_right_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    wooden_rc_track_left_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AC798 */
static void wooden_rc_track_s_bend_left(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23725, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24591, 0, 0, 32, 25, 2, height, 0, 3, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23729, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24595, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23741, 0, 0, 32, 25, 0, height, 0, 3, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24607, 0, 0, 32, 25, 0, height, 0, 3, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23728, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24594, 0, 0, 32, 25, 2, height, 0, 3, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23732, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24598, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23744, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24610, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23726, 0, 0, 32, 26, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24592, 0, 0, 32, 26, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23730, 0, 0, 32, 26, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24596, 0, 0, 32, 26, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23742, 0, 0, 32, 26, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24608, 0, 0, 32, 26, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23727, 0, 0, 32, 26, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24593, 0, 0, 32, 26, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23731, 0, 0, 32, 26, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24597, 0, 0, 32, 26, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23743, 0, 0, 32, 26, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24609, 0, 0, 32, 26, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23727, 0, 0, 32, 26, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24593, 0, 0, 32, 26, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23731, 0, 0, 32, 26, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24597, 0, 0, 32, 26, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23743, 0, 0, 32, 26, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24609, 0, 0, 32, 26, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23726, 0, 0, 32, 26, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24592, 0, 0, 32, 26, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23730, 0, 0, 32, 26, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24596, 0, 0, 32, 26, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23742, 0, 0, 32, 26, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24608, 0, 0, 32, 26, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23728, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24594, 0, 0, 32, 25, 2, height, 0, 3, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23732, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24598, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23744, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24610, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23725, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24591, 0, 0, 32, 25, 2, height, 0, 3, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23729, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24595, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23741, 0, 0, 32, 25, 0, height, 0, 3, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24607, 0, 0, 32, 25, 0, height, 0, 3, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_6);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008AC7A8 */
static void wooden_rc_track_s_bend_right(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23733, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24599, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23745, 0, 0, 32, 25, 0, height, 0, 3, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24611, 0, 0, 32, 25, 0, height, 0, 3, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23737, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24603, 0, 0, 32, 25, 2, height, 0, 3, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23736, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24602, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23748, 0, 0, 32, 25, 0, height, 0, 3, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24614, 0, 0, 32, 25, 0, height, 0, 3, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23740, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24606, 0, 0, 32, 25, 2, height, 0, 3, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23734, 0, 0, 32, 26, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24600, 0, 0, 32, 26, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23746, 0, 0, 32, 26, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24612, 0, 0, 32, 26, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23738, 0, 0, 32, 26, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24604, 0, 0, 32, 26, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23735, 0, 0, 32, 26, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24601, 0, 0, 32, 26, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23747, 0, 0, 32, 26, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24613, 0, 0, 32, 26, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23739, 0, 0, 32, 26, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24605, 0, 0, 32, 26, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23735, 0, 0, 32, 26, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24601, 0, 0, 32, 26, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23747, 0, 0, 32, 26, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24613, 0, 0, 32, 26, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23739, 0, 0, 32, 26, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24605, 0, 0, 32, 26, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23734, 0, 0, 32, 26, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24600, 0, 0, 32, 26, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23746, 0, 0, 32, 26, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24612, 0, 0, 32, 26, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23738, 0, 0, 32, 26, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24604, 0, 0, 32, 26, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23736, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24602, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23748, 0, 0, 32, 25, 0, height, 0, 3, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24614, 0, 0, 32, 25, 0, height, 0, 3, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23740, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24606, 0, 0, 32, 25, 2, height, 0, 3, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23733, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24599, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23745, 0, 0, 32, 25, 0, height, 0, 3, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24611, 0, 0, 32, 25, 0, height, 0, 3, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23737, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24603, 0, 0, 32, 25, 2, height, 0, 3, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height, TUNNEL_6);
            break;
        case 2:
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008ACE18 */
static void wooden_rc_track_left_vertical_loop(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23453, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23461, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23460, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23468, 0, 6, 32, 20, 7, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23454, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23462, 0, 14, 32, 2, 63, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23459, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23467, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23455, 16, 0, 5, 16, 119, height);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23463, 12, 0, 3, 16, 119, height);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23458, 10, 16, 4, 16, 119, height);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23466, 16, 16, 4, 16, 119, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23456, 0, 0, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23464, 0, 0, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23457, 0, 16, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23465, 0, 16, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 5:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23457, 0, 16, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23465, 0, 16, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23456, 0, 0, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23464, 0, 0, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23458, 10, 16, 4, 16, 119, height);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23466, 16, 16, 4, 16, 119, height);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23455, 16, 0, 5, 16, 119, height);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23463, 12, 0, 3, 16, 119, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 8:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23459, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23467, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23454, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23462, 0, 14, 32, 2, 63, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23460, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23468, 0, 6, 32, 20, 7, height);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23453, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23461, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height - 8, TUNNEL_7);
            break;
        case 2:
            paint_util_push_tunnel_left(height - 8, TUNNEL_7);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008ACE28 */
static void wooden_rc_track_right_vertical_loop(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23484, 0, 6, 32, 20, 7, height);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23476, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23477, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23469, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
        }
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23483, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23475, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23478, 0, 14, 32, 2, 63, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23470, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23482, 16, 16, 4, 16, 119, height);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23474, 10, 16, 4, 16, 119, height);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23479, 12, 0, 3, 16, 119, height);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23471, 16, 0, 5, 16, 119, height);
            break;
        }
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23481, 0, 16, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23473, 0, 16, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23480, 0, 0, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23472, 0, 0, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 5:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23480, 0, 0, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23472, 0, 0, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23481, 0, 16, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23473, 0, 16, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23479, 12, 0, 3, 16, 119, height);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23471, 16, 0, 5, 16, 119, height);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23482, 16, 16, 4, 16, 119, height);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23474, 10, 16, 4, 16, 119, height);
            break;
        }
        paint_util_set_general_support_height(height + 168, 0x20);
        break;
    case 8:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23478, 0, 14, 32, 2, 63, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 9, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23470, 0, 0, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 20, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23483, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23475, 0, 6, 32, 26, 3, height);
            metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 16, height, gTrackColours[SCHEME_SUPPORTS]);
            break;
        }
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23477, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23469, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23484, 0, 6, 32, 20, 7, height);
            break;
        case 3:
            sub_98196C_rotated(direction, wooden_rc_get_rails_colour() | 23476, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(METAL_SUPPORTS_BOXED, 4, 8, height, gTrackColours[SCHEME_SUPPORTS]);
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(height - 8, TUNNEL_7);
            break;
        case 2:
            paint_util_push_tunnel_left(height - 8, TUNNEL_7);
            break;
        }
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }

    track_paint_util_right_vertical_loop_segments(direction, trackSequence);
}

/** rct2: 0x008AC7E8 */
static void wooden_rc_track_left_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23828, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24694, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23831, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24697, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23840, 0, 6, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24706, 0, 6, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23834, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24700, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23825, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24691, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23837, 0, 6, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24703, 0, 6, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23827, 16, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24693, 16, 0, 16, 16, 2, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23830, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24696, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23839, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24705, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23833, 0, 16, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24699, 0, 16, 16, 16, 2, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23824, 16, 16, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24690, 16, 16, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23836, 16, 16, 16, 16, 0, height, 16, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24702, 16, 16, 16, 16, 0, height, 16, 16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23826, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24692, 6, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23829, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24695, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23838, 6, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24704, 6, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23832, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24698, 6, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23823, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24689, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23835, 6, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24701, 6, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008AC7F8 */
static void wooden_rc_track_right_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    wooden_rc_track_left_quarter_turn_3(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AC808 */
static void wooden_rc_track_left_quarter_turn_3_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23846, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24712, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23849, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24715, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23858, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24724, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23852, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24718, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23843, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24709, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23855, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24721, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23845, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24711, 0, 0, 16, 16, 2, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23848, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24714, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23857, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24723, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23851, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24717, 0, 0, 16, 16, 2, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23842, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24708, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23854, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24720, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23844, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24710, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23847, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24713, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23856, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24722, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23850, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24716, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23841, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24707, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23853, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24719, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008AC818 */
static void wooden_rc_track_right_quarter_turn_3_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    wooden_rc_track_left_quarter_turn_3_bank(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AC828 */
static void wooden_rc_track_left_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23906, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24772, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23908, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24774, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23919, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24785, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23910, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24776, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23904, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24770, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23917, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24783, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23905, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24771, 6, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23907, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24773, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23918, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24784, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23909, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24775, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23920, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24786, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23903, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24769, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23916, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24782, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_8);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AC838 */
static void wooden_rc_track_right_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23895, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24761, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23911, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24777, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23897, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24763, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23899, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24765, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23914, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24780, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23901, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24767, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23896, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24762, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23912, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24778, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23898, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24764, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23913, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24779, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23900, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24766, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23915, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24781, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23902, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24768, 6, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_8);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AC848 */
static void wooden_rc_track_left_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    wooden_rc_track_right_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008AC858 */
static void wooden_rc_track_right_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    wooden_rc_track_left_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008ACAB8 */
static void wooden_rc_track_left_half_banked_helix_up_small(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23882, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24748, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23885, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24751, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23894, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24760, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23888, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24754, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23879, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24745, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23891, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24757, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23881, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24747, 0, 0, 16, 16, 2, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23884, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24750, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23893, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24759, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23887, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24753, 0, 0, 16, 16, 2, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23878, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24744, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23890, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24756, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23880, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24746, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23883, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24749, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23892, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24758, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23886, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24752, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23877, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24743, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23889, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24755, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23879, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24745, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23891, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24757, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23882, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24748, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23885, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24751, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23894, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24760, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23888, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24754, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height, TUNNEL_6);
            break;
        case 1:
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23878, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24744, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23890, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24756, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23881, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24747, 0, 0, 16, 16, 2, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23884, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24750, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23893, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24759, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23887, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24753, 0, 0, 16, 16, 2, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23877, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24743, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23889, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24755, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23880, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24746, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23883, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24749, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23892, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24758, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23886, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24752, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_6);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008ACAC8 */
static void wooden_rc_track_right_half_banked_helix_up_small(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23859, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24725, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23871, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24737, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23862, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24728, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23865, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24731, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23874, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24740, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23868, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24734, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23860, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24726, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23872, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24738, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23863, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24729, 0, 0, 16, 16, 2, height, 0, 16, height);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23866, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24732, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23875, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24741, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23869, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24735, 0, 0, 16, 16, 2, height, 16, 0, height);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23861, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24727, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23873, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24739, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23864, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24730, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23867, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24733, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23876, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24742, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23870, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24736, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_6);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23862, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24728, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23865, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24731, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23874, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24740, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23868, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24734, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23859, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24725, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23871, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24737, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23863, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24729, 0, 0, 16, 16, 2, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23866, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24732, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23875, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24741, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23869, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24735, 0, 0, 16, 16, 2, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23860, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24726, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23872, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24738, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23864, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24730, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23867, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24733, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23876, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24742, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23870, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24736, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23861, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24727, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23873, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24739, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_6);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008ACAD8 */
static void wooden_rc_track_left_half_banked_helix_down_small(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 4) {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    wooden_rc_track_right_half_banked_helix_up_small(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008ACAE8 */
static void wooden_rc_track_right_half_banked_helix_down_small(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 4) {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    wooden_rc_track_left_half_banked_helix_up_small(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008ACAF8 */
static void wooden_rc_track_left_half_banked_helix_up_large(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23704, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24570, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23709, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24575, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23724, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24590, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23714, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24580, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23699, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24565, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23719, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24585, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23703, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24569, 0, 0, 32, 16, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23708, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24574, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23723, 0, 0, 32, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24589, 0, 0, 32, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23713, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24579, 0, 0, 32, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23698, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24564, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23718, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24584, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23702, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24568, 0, 0, 16, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23707, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24573, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23722, 0, 0, 16, 16, 0, height, 16, 16, height + 29);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24588, 0, 0, 16, 16, 0, height, 16, 16, height + 29);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23712, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24578, 0, 0, 16, 16, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23697, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24563, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23717, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24583, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_D0, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23701, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24567, 0, 0, 16, 32, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23706, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24572, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23721, 0, 0, 16, 32, 0, height, 0, 0, height + 33);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24587, 0, 0, 16, 32, 0, height, 0, 0, height + 33);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23711, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24577, 0, 0, 16, 32, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23696, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24562, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23716, 0, 0, 16, 32, 0, height, 16, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24582, 0, 0, 16, 32, 0, height, 16, 0, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23700, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24566, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23705, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24571, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23720, 0, 0, 27, 32, 0, height, 2, 0, height + 33);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24586, 0, 0, 27, 32, 0, height, 2, 0, height + 33);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23710, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24576, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23695, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24561, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23715, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24581, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23699, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24565, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23719, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24585, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23704, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24570, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23709, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24575, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23724, 0, 0, 27, 32, 0, height, 2, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24590, 0, 0, 27, 32, 0, height, 2, 0, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23714, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24580, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height, TUNNEL_6);
            break;
        case 1:
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 8:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23698, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24564, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23718, 0, 0, 16, 32, 0, height, 16, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24584, 0, 0, 16, 32, 0, height, 16, 0, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23703, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24569, 0, 0, 16, 32, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23708, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24574, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23723, 0, 0, 16, 32, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24589, 0, 0, 16, 32, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23713, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24579, 0, 0, 16, 32, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 10:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23697, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24563, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23717, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24583, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23702, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24568, 0, 0, 16, 16, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23707, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24573, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23722, 0, 0, 16, 16, 0, height, 16, 16, height + 29);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24588, 0, 0, 16, 16, 0, height, 16, 16, height + 29);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23712, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24578, 0, 0, 16, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 11:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 12:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23696, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24562, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23716, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24582, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23701, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24567, 0, 0, 32, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23706, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24572, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23721, 0, 0, 32, 16, 0, height, 0, 0, height + 33);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24587, 0, 0, 32, 16, 0, height, 0, 0, height + 33);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23711, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24577, 0, 0, 32, 16, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 13:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23695, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24561, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23715, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24581, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23700, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24566, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23705, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24571, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23720, 0, 0, 32, 27, 0, height, 0, 2, height + 33);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24586, 0, 0, 32, 27, 0, height, 0, 2, height + 33);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23710, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24576, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_6);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008ACB08 */
static void wooden_rc_track_right_half_banked_helix_up_large(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23665, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24531, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23685, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24551, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23670, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24536, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23675, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24541, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23690, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24556, 0, 0, 32, 27, 0, height, 0, 2, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23680, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24546, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23666, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24532, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23686, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24552, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23671, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24537, 0, 0, 32, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23676, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24542, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23691, 0, 0, 32, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24557, 0, 0, 32, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23681, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24547, 0, 0, 32, 16, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23667, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24533, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23687, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24553, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23672, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24538, 0, 0, 16, 16, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23677, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24543, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23692, 0, 0, 16, 16, 0, height, 16, 16, height + 29);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24558, 0, 0, 16, 16, 0, height, 16, 16, height + 29);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23682, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24548, 0, 0, 16, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23668, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24534, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23688, 0, 0, 16, 32, 0, height, 16, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24554, 0, 0, 16, 32, 0, height, 16, 0, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23673, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24539, 0, 0, 16, 32, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23678, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24544, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23693, 0, 0, 16, 32, 0, height, 0, 0, height + 33);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24559, 0, 0, 16, 32, 0, height, 0, 0, height + 33);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23683, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24549, 0, 0, 16, 32, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23669, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24535, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23689, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24555, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23674, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24540, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23679, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24545, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23694, 0, 0, 27, 32, 0, height, 2, 0, height + 33);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24560, 0, 0, 27, 32, 0, height, 2, 0, height + 33);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23684, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24550, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_6);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 7:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23670, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24536, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23675, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24541, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23690, 0, 0, 27, 32, 0, height, 2, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24556, 0, 0, 27, 32, 0, height, 2, 0, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23680, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24546, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23665, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24531, 0, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23685, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24551, 0, 0, 20, 32, 0, height, 6, 0, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_CC, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 8:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 9:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23671, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24537, 0, 0, 16, 32, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23676, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24542, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23691, 0, 0, 16, 32, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24557, 0, 0, 16, 32, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23681, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24547, 0, 0, 16, 32, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23666, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24532, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23686, 0, 0, 16, 32, 0, height, 16, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24552, 0, 0, 16, 32, 0, height, 16, 0, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 10:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23672, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24538, 0, 0, 16, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23677, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24543, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23692, 0, 0, 16, 16, 0, height, 16, 16, height + 29);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24558, 0, 0, 16, 16, 0, height, 16, 16, height + 29);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23682, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24548, 0, 0, 16, 16, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23667, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24533, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23687, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24553, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_D0, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 11:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 12:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23673, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24539, 0, 0, 32, 16, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23678, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24544, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23693, 0, 0, 32, 16, 0, height, 0, 0, height + 33);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24559, 0, 0, 32, 16, 0, height, 0, 0, height + 33);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23683, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24549, 0, 0, 32, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23668, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24534, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23688, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24554, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 13:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23674, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24540, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23679, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24545, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23694, 0, 0, 32, 27, 0, height, 0, 2, height + 33);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24560, 0, 0, 32, 27, 0, height, 0, 2, height + 33);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23684, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24550, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23669, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24535, 0, 0, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23689, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24555, 0, 0, 32, 20, 0, height, 0, 6, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_6);
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_D4, direction), 48, 0x20);
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008ACB18 */
static void wooden_rc_track_left_half_banked_helix_down_large(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 7) {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    wooden_rc_track_right_half_banked_helix_up_large(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008ACB28 */
static void wooden_rc_track_right_half_banked_helix_down_large(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (trackSequence >= 7) {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    wooden_rc_track_left_half_banked_helix_up_large(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008ACB98 */
static void wooden_rc_track_left_quarter_turn_1_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24209, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25075, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24217, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25083, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24210, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25076, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24218, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25084, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24211, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25077, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24219, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25085, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24212, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25078, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24220, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25086, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    }
    track_paint_util_left_quarter_turn_1_tile_tunnel(direction, height, -8, TUNNEL_7, +56, TUNNEL_8);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 104, 0x20);
}

/** rct2: 0x008ACB78 */
static void wooden_rc_track_right_quarter_turn_1_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24213, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25079, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24221, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25087, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24214, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25080, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24222, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25088, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24215, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25081, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24223, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25089, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24216, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25082, 0, 0, 28, 28, 3, height, 2, 2, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24224, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25090, 0, 0, 28, 28, 1, height, 2, 2, height + 99);
        break;
    }
    track_paint_util_right_quarter_turn_1_tile_tunnel(direction, height, -8, TUNNEL_7, +56, TUNNEL_8);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 104, 0x20);
}

/** rct2: 0x008ACB88 */
static void wooden_rc_track_left_quarter_turn_1_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_right_quarter_turn_1_60_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008ACBA8 */
static void wooden_rc_track_right_quarter_turn_1_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_left_quarter_turn_1_60_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008AC868 */
static void wooden_rc_track_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][2] = {
        { SPR_WOODEN_RC_BRAKES_SW_NE, SPR_WOODEN_RC_BRAKES_RAILS_SW_NE },
        { SPR_WOODEN_RC_BRAKES_NW_SE, SPR_WOODEN_RC_BRAKES_RAILS_NW_SE },
        { SPR_WOODEN_RC_BRAKES_SW_NE, SPR_WOODEN_RC_BRAKES_RAILS_SW_NE },
        { SPR_WOODEN_RC_BRAKES_NW_SE, SPR_WOODEN_RC_BRAKES_RAILS_NW_SE },
    };

    wooden_rc_track_paint(imageIds[direction][0], imageIds[direction][1], direction, 0, 2, 32, 25, 2, height, 0, 3, height);
    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008ACC78 */
static void wooden_rc_track_25_deg_up_left_banked(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24249, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25115, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24250, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25116, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24257, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25123, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24251, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25117, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24258, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25124, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24252, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25118, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008ACC88 */
static void wooden_rc_track_25_deg_up_right_banked(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24253, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25119, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24254, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25120, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24259, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25125, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24255, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25121, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24260, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25126, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24256, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25122, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AC878 */
static void wooden_rc_track_on_ride_photo(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23753, 0, 2, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24619, 0, 2, 32, 25, 2, height, 0, 3, height);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23754, 0, 2, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24620, 0, 2, 32, 25, 2, height, 0, 3, height);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23753, 0, 2, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24619, 0, 2, 32, 25, 2, height, 0, 3, height);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23754, 0, 2, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24620, 0, 2, 32, 25, 2, height, 0, 3, height);
        break;
    }
    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    track_paint_util_onride_photo_small_paint(direction, height + 16, mapElement);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_9);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008ACC98 */
static void wooden_rc_track_25_deg_down_left_banked(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_25_deg_up_right_banked(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACCA8 */
static void wooden_rc_track_25_deg_down_right_banked(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_25_deg_up_left_banked(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACE08 */
static void wooden_rc_track_water_splash(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23989, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24855, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23997, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24863, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23993, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24859, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23990, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24856, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23998, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24864, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23994, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24860, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23987, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24853, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23995, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24861, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23991, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24857, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23988, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24854, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23996, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24862, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23992, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24858, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        }
        wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        paint_util_push_tunnel_rotated(direction, height + 16, TUNNEL_6);
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23977, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24843, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23985, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24851, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23981, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24847, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23978, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24844, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23986, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24852, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23982, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24848, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23975, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24841, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23983, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24849, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23979, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24845, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23976, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24842, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23984, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24850, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23980, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24846, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        }
        wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23999, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24865, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24003, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24869, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 24001, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24867, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24000, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24866, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24004, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24870, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 24002, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24868, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        }
        wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23975, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24841, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23983, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24849, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23979, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24845, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23976, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24842, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23984, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24850, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23980, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24846, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23977, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24843, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23985, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24851, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23981, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24847, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23978, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24844, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23986, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24852, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23982, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24848, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        }
        wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23987, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24853, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23995, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24861, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23991, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24857, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23988, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24854, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23996, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24862, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23992, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24858, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23989, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24855, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23997, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24863, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23993, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24859, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23990, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24856, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, 0x61000000 | 5048, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, 0x00000000 | 5053, 0, 0, 32, 25, 2, height + 16, 0, 3, height + 16);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 23998, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, gTrackColours[SCHEME_SUPPORTS] | 24864, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_track_colour() | 23994, 0, 0, 32, 25, 2, height, 0, 3, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24860, 0, 0, 32, 25, 2, height, 0, 3, height);
            break;
        }
        wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        paint_util_push_tunnel_rotated(direction, height + 16, TUNNEL_6);
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AC958 */
static void wooden_rc_track_left_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24137, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25003, 0, 0, 32, 32, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24141, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25007, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24153, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25019, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24145, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25011, 0, 0, 32, 32, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24149, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25015, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24157, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25023, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24138, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25004, 0, 0, 32, 16, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24142, 0, 0, 34, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25008, 0, 0, 34, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24154, 0, 0, 32, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25020, 0, 0, 32, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24146, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25012, 0, 0, 32, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24150, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25016, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24158, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25024, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24139, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25005, 0, 0, 16, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24143, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25009, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24155, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25021, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24147, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25013, 0, 0, 16, 16, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24151, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25017, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24159, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25025, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24140, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25006, 0, 0, 16, 16, 2, height, 16, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24144, 0, 0, 16, 18, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25010, 0, 0, 16, 18, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24156, 0, 0, 16, 16, 0, height, 0, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25022, 0, 0, 16, 16, 0, height, 0, 16, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24148, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25014, 0, 0, 16, 16, 2, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24152, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25018, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24160, 0, 0, 16, 16, 0, height, 16, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25026, 0, 0, 16, 16, 0, height, 16, 0, height + 27);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008AC968 */
static void wooden_rc_track_right_eighth_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24113, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24979, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24129, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24995, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24117, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24983, 0, 0, 32, 32, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24121, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24987, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24133, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24999, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24125, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24991, 0, 0, 32, 32, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24114, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24980, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24130, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24996, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24118, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24984, 0, 0, 32, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24122, 0, 0, 34, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24988, 0, 0, 34, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24134, 0, 0, 32, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25000, 0, 0, 32, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24126, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24992, 0, 0, 32, 16, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24115, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24981, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24131, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24997, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24119, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24985, 0, 0, 16, 16, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24123, 0, 0, 28, 28, 2, height, 4, 4, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24989, 0, 0, 28, 28, 2, height, 4, 4, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24135, 0, 0, 28, 28, 0, height, 4, 4, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25001, 0, 0, 28, 28, 0, height, 4, 4, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24127, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24993, 0, 0, 16, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24116, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24982, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24132, 0, 0, 16, 16, 0, height, 16, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24998, 0, 0, 16, 16, 0, height, 16, 0, height + 27);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24120, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24986, 0, 0, 16, 16, 2, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24124, 0, 0, 16, 18, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24990, 0, 0, 16, 18, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24136, 0, 0, 16, 16, 0, height, 0, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25002, 0, 0, 16, 16, 0, height, 0, 16, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24128, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24994, 0, 0, 16, 16, 2, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008AC978 */
static void wooden_rc_track_left_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    wooden_rc_track_right_eighth_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC988 */
static void wooden_rc_track_right_eighth_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    wooden_rc_track_left_eighth_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008AC998 */
static void wooden_rc_track_left_eighth_bank_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24185, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25051, 0, 0, 32, 32, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24189, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25055, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24201, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25067, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24193, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25059, 0, 0, 32, 32, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24197, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25063, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24205, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25071, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24186, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25052, 0, 0, 32, 16, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24190, 0, 0, 34, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25056, 0, 0, 34, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24202, 0, 0, 32, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25068, 0, 0, 32, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24194, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25060, 0, 0, 32, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24198, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25064, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24206, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25072, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24187, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25053, 0, 0, 16, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24191, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25057, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24203, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25069, 0, 0, 16, 16, 0, height, 16, 16, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24195, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25061, 0, 0, 16, 16, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24199, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25065, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24207, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25073, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24188, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25054, 0, 0, 16, 16, 2, height, 16, 16, height);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24192, 0, 0, 16, 18, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25058, 0, 0, 16, 18, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24204, 0, 0, 16, 16, 0, height, 0, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25070, 0, 0, 16, 16, 0, height, 0, 16, height + 27);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24196, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25062, 0, 0, 16, 16, 2, height, 0, 0, height);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24200, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25066, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24208, 0, 0, 16, 16, 0, height, 16, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25074, 0, 0, 16, 16, 0, height, 16, 0, height + 27);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008AC9A8 */
static void wooden_rc_track_right_eighth_bank_to_diag(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24161, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25027, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24177, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25043, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24165, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25031, 0, 0, 32, 32, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24169, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25035, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24181, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25047, 0, 0, 32, 32, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24173, 0, 0, 32, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25039, 0, 0, 32, 32, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24162, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25028, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24178, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25044, 0, 0, 32, 16, 0, height, 0, 16, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24166, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25032, 0, 0, 32, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24170, 0, 0, 34, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25036, 0, 0, 34, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24182, 0, 0, 32, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25048, 0, 0, 32, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(0, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24174, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25040, 0, 0, 32, 16, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24163, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25029, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24179, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25045, 0, 0, 16, 16, 0, height, 0, 0, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24167, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25033, 0, 0, 16, 16, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24171, 0, 0, 28, 28, 2, height, 4, 4, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25037, 0, 0, 28, 28, 2, height, 4, 4, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24183, 0, 0, 28, 28, 0, height, 4, 4, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25049, 0, 0, 28, 28, 0, height, 4, 4, height + 27);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24175, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25041, 0, 0, 16, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24164, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25030, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24180, 0, 0, 16, 16, 0, height, 16, 0, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25046, 0, 0, 16, 16, 0, height, 16, 0, height + 27);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24168, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25034, 0, 0, 16, 16, 2, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24172, 0, 0, 16, 18, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25038, 0, 0, 16, 18, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24184, 0, 0, 16, 16, 0, height, 0, 16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25050, 0, 0, 16, 16, 0, height, 0, 16, height + 27);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24176, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25042, 0, 0, 16, 16, 2, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008AC9B8 */
static void wooden_rc_track_left_eighth_bank_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    wooden_rc_track_right_eighth_bank_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008AC9C8 */
static void wooden_rc_track_right_eighth_bank_to_orthogonal(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    wooden_rc_track_left_eighth_bank_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, mapElement);
}

/** rct2: 0x008AC888 */
static void wooden_rc_track_diag_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24050, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24916, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24008, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24874, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24047, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24913, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24051, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24917, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
                wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24005, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24871, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24009, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24875, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
                wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24049, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24915, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24052, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24918, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
                wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24007, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24873, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24010, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24876, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
                wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24048, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24914, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24006, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24872, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008AC8B8 */
static void wooden_rc_track_diag_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24068, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24934, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24026, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24892, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24065, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24931, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24069, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24935, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24023, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24889, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24027, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24893, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24067, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24933, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24070, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24936, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24025, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24891, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24028, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24894, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24066, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24932, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24024, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24890, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008AC8E8 */
static void wooden_rc_track_diag_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24044, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24910, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24041, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24907, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24045, -16, -16, 32, 32, 0, height, -16, -16, height + 91);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24911, -16, -16, 32, 32, 0, height, -16, -16, height + 91);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24043, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24909, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24046, -16, -16, 32, 32, 0, height, -16, -16, height + 91);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24912, -16, -16, 32, 32, 0, height, -16, -16, height + 91);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24042, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24908, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    }
}

/** rct2: 0x008AC898 */
static void wooden_rc_track_diag_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24056, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24922, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24014, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24880, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24053, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24919, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24057, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24923, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24011, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24877, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24015, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24881, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24055, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24921, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24058, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24924, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24013, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24879, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24016, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24882, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24054, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24920, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24012, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24878, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AC8C8 */
static void wooden_rc_track_diag_25_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24032, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24898, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24029, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24895, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24033, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24899, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24031, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24897, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24034, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24900, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24030, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24896, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AC8D8 */
static void wooden_rc_track_diag_60_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24038, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24904, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24035, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24901, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24039, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24905, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24037, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24903, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24040, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24906, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24036, -16, -16, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24902, -16, -16, 16, 16, 2, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AC8A8 */
static void wooden_rc_track_diag_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24062, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24928, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24020, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24886, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24059, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24925, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24063, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24929, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24017, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24883, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24021, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24887, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24061, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24927, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24064, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24930, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24019, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24885, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24022, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24888, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24060, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24926, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24018, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24884, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008AC918 */
static void wooden_rc_track_diag_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24066, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24932, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24024, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24890, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24067, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24933, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24070, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24936, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24025, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24891, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24028, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24894, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24065, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24931, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24069, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24935, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24023, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24889, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24027, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24893, -16, -16, 32, 32, 0, height, -16, -16, height + 43);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24068, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24934, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24026, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24892, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008AC948 */
static void wooden_rc_track_diag_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24042, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24908, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24043, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24909, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24046, -16, -16, 32, 32, 0, height, -16, -16, height + 91);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24912, -16, -16, 32, 32, 0, height, -16, -16, height + 91);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24041, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24907, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24045, -16, -16, 32, 32, 0, height, -16, -16, height + 91);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24911, -16, -16, 32, 32, 0, height, -16, -16, height + 91);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24044, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24910, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 104, 0x20);
        break;
    }
}

/** rct2: 0x008AC8F8 */
static void wooden_rc_track_diag_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24060, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24926, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24018, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24884, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24061, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24927, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24064, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24930, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24019, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24885, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24022, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24888, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24059, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24925, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24063, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24929, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24017, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24883, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24021, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24887, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24062, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24928, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24020, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24886, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008AC928 */
static void wooden_rc_track_diag_25_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24036, -16, -16, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24902, -16, -16, 16, 16, 2, height, 0, 0, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24037, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24903, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24040, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24906, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24035, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24901, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24039, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24905, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24038, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24904, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AC938 */
static void wooden_rc_track_diag_60_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24030, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24896, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24031, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24897, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24034, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24900, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24029, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24895, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24033, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24899, -16, -16, 32, 32, 0, height, -16, -16, height + 59);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24032, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24898, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008AC908 */
static void wooden_rc_track_diag_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24054, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24920, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 3:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24012, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24878, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24055, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24921, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24058, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24924, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24013, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24879, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24016, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24882, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 0:
                wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24053, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24919, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24057, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24923, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        } else {
            switch (direction) {
            case 0:
                wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 1:
                wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 2:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24011, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24877, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24015, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24881, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
                wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            case 3:
                wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        if (track_element_is_lift_hill(mapElement)) {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24056, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24922, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        } else {
            switch (direction) {
            case 1:
                sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24014, -16, -16, 32, 32, 2, height, -16, -16, height);
                sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24880, -16, -16, 32, 32, 2, height, -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008ACA18 */
static void wooden_rc_track_diag_flat_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24080, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24946, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24077, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24943, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24081, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24947, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24079, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24945, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24082, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24948, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24078, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24944, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008AC9F8 */
static void wooden_rc_track_diag_flat_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24086, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24952, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24083, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24949, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24087, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24953, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24085, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24951, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24088, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24954, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24084, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24950, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008ACA08 */
static void wooden_rc_track_diag_left_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24084, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24950, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24085, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24951, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24088, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24954, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24083, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24949, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24087, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24953, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24086, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24952, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008ACA28 */
static void wooden_rc_track_diag_right_bank_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24078, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24944, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24079, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24945, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24082, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24948, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24077, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24943, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24081, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24947, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24080, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24946, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008ACA58 */
static void wooden_rc_track_diag_left_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24104, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24970, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24101, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24967, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24105, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24971, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24103, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24969, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24106, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24972, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24102, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24968, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008ACA68 */
static void wooden_rc_track_diag_right_bank_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24110, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24976, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24107, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24973, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24111, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24977, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24109, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24975, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24112, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24978, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24108, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24974, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008ACA38 */
static void wooden_rc_track_diag_25_deg_up_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24092, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24958, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24089, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24955, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24093, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24959, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24091, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24957, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24094, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24960, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24090, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24956, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008ACA48 */
static void wooden_rc_track_diag_25_deg_up_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24098, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24964, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24095, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24961, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24099, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24965, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24097, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24963, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24100, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24966, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24096, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24962, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008ACA78 */
static void wooden_rc_track_diag_left_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24096, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24962, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24097, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24963, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24100, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24966, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24095, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24961, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24099, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24965, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24098, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24964, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008ACA88 */
static void wooden_rc_track_diag_right_bank_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24090, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24956, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24091, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24957, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24094, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24960, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_b_supports_paint_setup(4, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_b_supports_paint_setup(5, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24089, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24955, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24093, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24959, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_b_supports_paint_setup(2, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_b_supports_paint_setup(3, 0, height + 16, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24092, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24958, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008ACA98 */
static void wooden_rc_track_diag_25_deg_down_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24108, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24974, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24109, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24975, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24112, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24978, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24107, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24973, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24111, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24977, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24110, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24976, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008ACAA8 */
static void wooden_rc_track_diag_25_deg_down_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24102, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24968, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24103, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24969, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24106, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24972, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24101, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24967, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24105, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24971, -16, -16, 32, 32, 0, height, -16, -16, height + 35);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24104, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24970, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008AC9D8 */
static void wooden_rc_track_diag_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24074, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24940, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24071, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24937, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24075, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24941, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24073, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24939, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24076, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24942, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24072, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24938, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008AC9E8 */
static void wooden_rc_track_diag_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24072, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24938, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24073, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24939, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24076, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24942, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24071, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24937, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24075, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24941, -16, -16, 32, 32, 0, height, -16, -16, height + 27);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24074, -16, -16, 32, 32, 2, height, -16, -16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24940, -16, -16, 32, 32, 2, height, -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008ACB38 */
static void wooden_rc_track_left_bank_to_left_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23958, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24824, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23960, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24826, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23971, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24837, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23962, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24828, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23956, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24822, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23969, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24835, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23957, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24823, 6, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23959, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24825, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23970, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24836, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23961, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24827, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23972, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24838, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23955, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24821, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23968, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24834, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_8);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008ACB48 */
static void wooden_rc_track_right_bank_to_right_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23947, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24813, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23963, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24829, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23949, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24815, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23951, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24817, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23966, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24832, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23953, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24819, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23948, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24814, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23964, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24830, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23950, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24816, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23965, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24831, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23952, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24818, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23967, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24833, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23954, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24820, 6, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height, TUNNEL_8);
            break;
        case 1:
            paint_util_push_tunnel_left(height, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008ACB58 */
static void wooden_rc_track_left_quarter_turn_3_25_deg_down_to_left_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23950, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24816, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23965, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24831, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23952, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24818, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23967, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24833, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23954, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24820, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23948, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24814, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23964, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24830, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23949, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24815, 6, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23951, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24817, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23966, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24832, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23953, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24819, 6, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23947, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24813, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23963, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24829, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008ACB68 */
static void wooden_rc_track_right_quarter_turn_3_25_deg_down_to_right_bank(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23955, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24821, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23968, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24834, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23957, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24823, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23959, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24825, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23970, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24836, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23961, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24827, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23972, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24838, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 2:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 48, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23956, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24822, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23969, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24835, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23958, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24824, 6, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23960, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24826, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23971, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24837, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23962, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24828, 6, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height, TUNNEL_6);
            break;
        case 1:
            paint_util_push_tunnel_left(height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    }
}

/** rct2: 0x008ACDF8 */
static void wooden_rc_track_block_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_paint(
        _wooden_rc_block_brakes_image_ids[direction][0],
        _wooden_rc_block_brakes_image_ids[direction][1], direction, 0, 2, 32, 25, 2, height, 0, 3, height);
    wooden_a_supports_paint_setup(direction & 1, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
    paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008ACCB8 */
static void wooden_rc_track_left_banked_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23932, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24798, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23934, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24800, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23945, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24811, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23936, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24802, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23930, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24796, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23943, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24809, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23931, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24797, 6, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23933, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24799, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23944, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24810, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23935, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24801, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23946, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24812, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23929, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24795, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23942, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24808, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_8);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008ACCC8 */
static void wooden_rc_track_right_banked_quarter_turn_3_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23921, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24787, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23937, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24803, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23923, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24789, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23925, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24791, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23940, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24806, 0, 6, 32, 20, 0, height, 0, 6, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23927, 0, 6, 32, 20, 2, height, 0, 6, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24793, 0, 6, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 2:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 56, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23922, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24788, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23938, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24804, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23924, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24790, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23939, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24805, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23926, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24792, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23941, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24807, 6, 0, 20, 32, 0, height, 6, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 23928, 6, 0, 20, 32, 2, height, 6, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 24794, 6, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_8);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008ACCD8 */
static void wooden_rc_track_left_banked_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    wooden_rc_track_right_banked_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008ACCE8 */
static void wooden_rc_track_right_banked_quarter_turn_3_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    wooden_rc_track_left_banked_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008ACC38 */
static void wooden_rc_track_left_banked_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24321, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25187, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24326, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25192, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24352, 0, 0, 32, 27, 0, height, 0, 2, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25218, 0, 0, 32, 27, 0, height, 0, 2, height + 67);
            wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24331, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25197, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24336, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25202, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24322, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25188, 0, 0, 32, 16, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24327, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25193, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24353, 0, 0, 32, 16, 0, height, 0, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25219, 0, 0, 32, 16, 0, height, 0, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24332, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25198, 0, 0, 32, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24337, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25203, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24359, 0, 0, 32, 16, 0, height, 0, 16, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25225, 0, 0, 32, 16, 0, height, 0, 16, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24323, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25189, 0, 0, 16, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24328, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25194, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24354, 0, 0, 16, 16, 0, height, 16, 16, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25220, 0, 0, 16, 16, 0, height, 16, 16, height + 59);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24333, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25199, 0, 0, 16, 16, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24338, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25204, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24360, 0, 0, 16, 16, 0, height, 0, 0, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25226, 0, 0, 16, 16, 0, height, 0, 0, height + 59);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24324, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25190, 0, 0, 16, 32, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24329, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25195, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24355, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25221, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24334, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25200, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24357, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25223, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24339, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25205, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24361, 0, 0, 16, 32, 0, height, 16, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25227, 0, 0, 16, 32, 0, height, 16, 0, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24325, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25191, 0, 0, 27, 32, 2, height, 2, 0, height);
            wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24330, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25196, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24356, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25222, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24335, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25201, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24358, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25224, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24340, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25206, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24362, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25228, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(height + 8, TUNNEL_8);
            break;
        case 3:
            paint_util_push_tunnel_left(height + 8, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008ACC48 */
static void wooden_rc_track_right_banked_quarter_turn_5_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24301, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25167, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24306, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25172, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24311, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25177, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24347, 0, 0, 32, 27, 0, height, 0, 2, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25213, 0, 0, 32, 27, 0, height, 0, 2, height + 67);
            wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24316, 0, 0, 32, 27, 2, height, 0, 2, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25182, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24302, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25168, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24341, 0, 0, 32, 16, 0, height, 0, 16, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25207, 0, 0, 32, 16, 0, height, 0, 16, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24307, 0, 0, 32, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25173, 0, 0, 32, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24312, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25178, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24348, 0, 0, 32, 16, 0, height, 0, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25214, 0, 0, 32, 16, 0, height, 0, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24317, 0, 0, 32, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25183, 0, 0, 32, 16, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24303, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25169, 0, 0, 16, 16, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24342, 0, 0, 16, 16, 0, height, 0, 0, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25208, 0, 0, 16, 16, 0, height, 0, 0, height + 59);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24308, 0, 0, 16, 16, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25174, 0, 0, 16, 16, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24313, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25179, 0, 0, 16, 16, 2, height, 16, 16, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24349, 0, 0, 16, 16, 0, height, 16, 16, height + 59);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25215, 0, 0, 16, 16, 0, height, 16, 16, height + 59);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24318, 0, 0, 16, 16, 2, height, 0, 16, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25184, 0, 0, 16, 16, 2, height, 0, 16, height);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 64, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24304, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25170, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24343, 0, 0, 16, 32, 0, height, 16, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25209, 0, 0, 16, 32, 0, height, 16, 0, height + 67);
            wooden_a_supports_paint_setup(4, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24309, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25175, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24345, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25211, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            wooden_a_supports_paint_setup(5, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24314, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25180, 0, 0, 16, 32, 2, height, 0, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24350, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25216, 0, 0, 16, 32, 0, height, 0, 0, height + 67);
            wooden_a_supports_paint_setup(2, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24319, 0, 0, 16, 32, 2, height, 16, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25185, 0, 0, 16, 32, 2, height, 16, 0, height);
            wooden_a_supports_paint_setup(3, 0, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24305, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25171, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24344, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25210, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24310, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25176, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24346, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25212, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24315, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25181, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24351, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25217, 0, 0, 27, 32, 0, height, 2, 0, height + 67);
            wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24320, 0, 0, 27, 32, 2, height, 2, 0, height);
            sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25186, 0, 0, 27, 32, 2, height, 2, 0, height);
            wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 0:
            paint_util_push_tunnel_right(height + 8, TUNNEL_8);
            break;
        case 1:
            paint_util_push_tunnel_left(height + 8, TUNNEL_8);
            break;
        }
        paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(height + 72, 0x20);
        break;
    }
}

/** rct2: 0x008ACC58 */
static void wooden_rc_track_left_banked_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    wooden_rc_track_right_banked_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008ACC68 */
static void wooden_rc_track_right_banked_quarter_turn_5_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    wooden_rc_track_left_banked_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008ACCF8 */
static void wooden_rc_track_25_deg_up_to_left_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24261, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25127, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24262, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25128, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24263, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25129, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24264, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25130, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008ACD08 */
static void wooden_rc_track_25_deg_up_to_right_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24265, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25131, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24266, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25132, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24267, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25133, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24268, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25134, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008ACD18 */
static void wooden_rc_track_left_banked_25_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24269, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25135, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24270, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25136, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24271, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25137, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24272, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25138, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008ACD28 */
static void wooden_rc_track_right_banked_25_deg_up_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24273, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25139, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 9, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24274, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25140, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 10, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24275, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25141, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 11, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24276, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25142, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 12, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 56, 0x20);
}

/** rct2: 0x008ACD38 */
static void wooden_rc_track_25_deg_down_to_left_banked_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_right_banked_25_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACD48 */
static void wooden_rc_track_25_deg_down_to_right_banked_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_left_banked_25_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACD58 */
static void wooden_rc_track_left_banked_25_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_25_deg_up_to_right_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACD68 */
static void wooden_rc_track_right_banked_25_deg_down_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_25_deg_up_to_left_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACD78 */
static void wooden_rc_track_left_banked_flat_to_left_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24277, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25143, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24278, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25144, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24293, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25159, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(1, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24279, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25145, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24294, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25160, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(0, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24280, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25146, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008ACD88 */
static void wooden_rc_track_right_banked_flat_to_right_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24281, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25147, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24282, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25148, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24295, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25161, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(1, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24283, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25149, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24296, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25162, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(0, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24284, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25150, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008ACD98 */
static void wooden_rc_track_left_banked_25_deg_up_to_left_banked_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24285, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25151, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24286, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25152, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24297, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25163, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(1, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24287, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25153, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24298, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25164, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(0, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24288, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25154, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008ACDA8 */
static void wooden_rc_track_right_banked_25_deg_up_to_right_banked_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24289, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25155, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24290, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25156, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24299, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25165, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(1, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24291, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25157, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24300, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25166, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(0, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24292, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25158, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008ACDB8 */
static void wooden_rc_track_left_banked_flat_to_left_banked_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_right_banked_25_deg_up_to_right_banked_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACDC8 */
static void wooden_rc_track_right_banked_flat_to_right_banked_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_left_banked_25_deg_up_to_left_banked_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACDD8 */
static void wooden_rc_track_left_banked_25_deg_down_to_left_banked_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_right_banked_flat_to_right_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACDE8 */
static void wooden_rc_track_right_banked_25_deg_down_to_right_banked_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_left_banked_flat_to_left_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACBB8 */
static void wooden_rc_track_flat_to_left_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24225, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25091, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24226, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25092, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24241, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25107, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(1, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24227, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25093, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24242, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25108, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(0, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24228, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25094, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008ACBC8 */
static void wooden_rc_track_flat_to_right_banked_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24229, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25095, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 1, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24230, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25096, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24243, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25109, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(1, 2, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24231, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25097, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24244, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25110, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(0, 3, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24232, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25098, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 4, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height, TUNNEL_8);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 48, 0x20);
}

/** rct2: 0x008ACBD8 */
static void wooden_rc_track_left_banked_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24233, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25099, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24234, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25100, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24245, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25111, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(1, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24235, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25101, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24246, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25112, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(0, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24236, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25102, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008ACBE8 */
static void wooden_rc_track_right_banked_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24237, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25103, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(0, 5, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24238, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25104, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24247, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25113, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(1, 6, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 2:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24239, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25105, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24248, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25114, 0, 0, 32, 1, 9, height, 0, 26, height + 5);
        wooden_a_supports_paint_setup(0, 7, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 3:
        sub_98197C_rotated(direction, wooden_rc_get_track_colour() | 24240, 0, 0, 32, 25, 2, height, 0, 3, height);
        sub_98199C_rotated(direction, wooden_rc_get_rails_colour() | 25106, 0, 0, 32, 25, 2, height, 0, 3, height);
        wooden_a_supports_paint_setup(1, 8, height, gTrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(direction, height - 8, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 40, 0x20);
}

/** rct2: 0x008ACBF8 */
static void wooden_rc_track_flat_to_left_banked_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_right_banked_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACC08 */
static void wooden_rc_track_flat_to_right_banked_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_left_banked_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACC18 */
static void wooden_rc_track_left_banked_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_flat_to_right_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008ACC28 */
static void wooden_rc_track_right_banked_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    wooden_rc_track_flat_to_left_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

TRACK_PAINT_FUNCTION get_track_paint_function_wooden_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return wooden_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return wooden_rc_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return wooden_rc_track_25_deg_up;
    case TRACK_ELEM_60_DEG_UP:
        return wooden_rc_track_60_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return wooden_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
        return wooden_rc_track_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
        return wooden_rc_track_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return wooden_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return wooden_rc_track_25_deg_down;
    case TRACK_ELEM_60_DEG_DOWN:
        return wooden_rc_track_60_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return wooden_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
        return wooden_rc_track_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
        return wooden_rc_track_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return wooden_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return wooden_rc_track_left_quarter_turn_5;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return wooden_rc_track_right_quarter_turn_5;
    case TRACK_ELEM_FLAT_TO_LEFT_BANK:
        return wooden_rc_track_flat_to_left_bank;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
        return wooden_rc_track_flat_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_FLAT:
        return wooden_rc_track_left_bank_to_flat;
    case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
        return wooden_rc_track_right_bank_to_flat;
    case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
        return wooden_rc_track_banked_left_quarter_turn_5;
    case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
        return wooden_rc_track_banked_right_quarter_turn_5;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
        return wooden_rc_track_left_bank_to_25_deg_up;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
        return wooden_rc_track_right_bank_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
        return wooden_rc_track_25_deg_up_to_left_bank;
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
        return wooden_rc_track_25_deg_up_to_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
        return wooden_rc_track_left_bank_to_25_deg_down;
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
        return wooden_rc_track_right_bank_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
        return wooden_rc_track_25_deg_down_to_left_bank;
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
        return wooden_rc_track_25_deg_down_to_right_bank;
    case TRACK_ELEM_LEFT_BANK:
        return wooden_rc_track_left_bank;
    case TRACK_ELEM_RIGHT_BANK:
        return wooden_rc_track_right_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return wooden_rc_track_left_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
        return wooden_rc_track_right_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return wooden_rc_track_left_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
        return wooden_rc_track_right_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_S_BEND_LEFT:
        return wooden_rc_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return wooden_rc_track_s_bend_right;
    case TRACK_ELEM_LEFT_VERTICAL_LOOP:
        return wooden_rc_track_left_vertical_loop;
    case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
        return wooden_rc_track_right_vertical_loop;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return wooden_rc_track_left_quarter_turn_3;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return wooden_rc_track_right_quarter_turn_3;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
        return wooden_rc_track_left_quarter_turn_3_bank;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
        return wooden_rc_track_right_quarter_turn_3_bank;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return wooden_rc_track_left_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return wooden_rc_track_right_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return wooden_rc_track_left_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
        return wooden_rc_track_right_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
        return wooden_rc_track_left_half_banked_helix_up_small;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
        return wooden_rc_track_right_half_banked_helix_up_small;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
        return wooden_rc_track_left_half_banked_helix_down_small;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
        return wooden_rc_track_right_half_banked_helix_down_small;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
        return wooden_rc_track_left_half_banked_helix_up_large;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
        return wooden_rc_track_right_half_banked_helix_up_large;
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
        return wooden_rc_track_left_half_banked_helix_down_large;
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
        return wooden_rc_track_right_half_banked_helix_down_large;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP:
        return wooden_rc_track_left_quarter_turn_1_60_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP:
        return wooden_rc_track_right_quarter_turn_1_60_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        return wooden_rc_track_left_quarter_turn_1_60_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        return wooden_rc_track_right_quarter_turn_1_60_deg_down;
    case TRACK_ELEM_BRAKES:
        return wooden_rc_track_brakes;
    case TRACK_ELEM_25_DEG_UP_LEFT_BANKED:
        return wooden_rc_track_25_deg_up_left_banked;
    case TRACK_ELEM_25_DEG_UP_RIGHT_BANKED:
        return wooden_rc_track_25_deg_up_right_banked;
    case TRACK_ELEM_ON_RIDE_PHOTO:
        return wooden_rc_track_on_ride_photo;
    case TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED:
        return wooden_rc_track_25_deg_down_left_banked;
    case TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED:
        return wooden_rc_track_25_deg_down_right_banked;
    case TRACK_ELEM_WATER_SPLASH:
        return wooden_rc_track_water_splash;
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
        return wooden_rc_track_left_eighth_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
        return wooden_rc_track_right_eighth_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
        return wooden_rc_track_left_eighth_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
        return wooden_rc_track_right_eighth_to_orthogonal;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
        return wooden_rc_track_left_eighth_bank_to_diag;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
        return wooden_rc_track_right_eighth_bank_to_diag;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
        return wooden_rc_track_left_eighth_bank_to_orthogonal;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
        return wooden_rc_track_right_eighth_bank_to_orthogonal;
    case TRACK_ELEM_DIAG_FLAT:
        return wooden_rc_track_diag_flat;
    case TRACK_ELEM_DIAG_25_DEG_UP:
        return wooden_rc_track_diag_25_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP:
        return wooden_rc_track_diag_60_deg_up;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
        return wooden_rc_track_diag_flat_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
        return wooden_rc_track_diag_25_deg_up_to_60_deg_up;
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
        return wooden_rc_track_diag_60_deg_up_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        return wooden_rc_track_diag_25_deg_up_to_flat;
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
        return wooden_rc_track_diag_25_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN:
        return wooden_rc_track_diag_60_deg_down;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
        return wooden_rc_track_diag_flat_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
        return wooden_rc_track_diag_25_deg_down_to_60_deg_down;
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
        return wooden_rc_track_diag_60_deg_down_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
        return wooden_rc_track_diag_25_deg_down_to_flat;
    case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
        return wooden_rc_track_diag_flat_to_left_bank;
    case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
        return wooden_rc_track_diag_flat_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
        return wooden_rc_track_diag_left_bank_to_flat;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
        return wooden_rc_track_diag_right_bank_to_flat;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
        return wooden_rc_track_diag_left_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
        return wooden_rc_track_diag_right_bank_to_25_deg_up;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
        return wooden_rc_track_diag_25_deg_up_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
        return wooden_rc_track_diag_25_deg_up_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
        return wooden_rc_track_diag_left_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
        return wooden_rc_track_diag_right_bank_to_25_deg_down;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
        return wooden_rc_track_diag_25_deg_down_to_left_bank;
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
        return wooden_rc_track_diag_25_deg_down_to_right_bank;
    case TRACK_ELEM_DIAG_LEFT_BANK:
        return wooden_rc_track_diag_left_bank;
    case TRACK_ELEM_DIAG_RIGHT_BANK:
        return wooden_rc_track_diag_right_bank;
    case TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return wooden_rc_track_left_bank_to_left_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        return wooden_rc_track_right_bank_to_right_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK:
        return wooden_rc_track_left_quarter_turn_3_25_deg_down_to_left_bank;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK:
        return wooden_rc_track_right_quarter_turn_3_25_deg_down_to_right_bank;
    case TRACK_ELEM_BLOCK_BRAKES:
        return wooden_rc_track_block_brakes;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
        return wooden_rc_track_left_banked_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
        return wooden_rc_track_right_banked_quarter_turn_3_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
        return wooden_rc_track_left_banked_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
        return wooden_rc_track_right_banked_quarter_turn_3_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
        return wooden_rc_track_left_banked_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
        return wooden_rc_track_right_banked_quarter_turn_5_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
        return wooden_rc_track_left_banked_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
        return wooden_rc_track_right_banked_quarter_turn_5_25_deg_down;
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP:
        return wooden_rc_track_25_deg_up_to_left_banked_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP:
        return wooden_rc_track_25_deg_up_to_right_banked_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP:
        return wooden_rc_track_left_banked_25_deg_up_to_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP:
        return wooden_rc_track_right_banked_25_deg_up_to_25_deg_up;
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN:
        return wooden_rc_track_25_deg_down_to_left_banked_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN:
        return wooden_rc_track_25_deg_down_to_right_banked_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
        return wooden_rc_track_left_banked_25_deg_down_to_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
        return wooden_rc_track_right_banked_25_deg_down_to_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP:
        return wooden_rc_track_left_banked_flat_to_left_banked_25_deg_up;
    case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
        return wooden_rc_track_right_banked_flat_to_right_banked_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT:
        return wooden_rc_track_left_banked_25_deg_up_to_left_banked_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT:
        return wooden_rc_track_right_banked_25_deg_up_to_right_banked_flat;
    case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
        return wooden_rc_track_left_banked_flat_to_left_banked_25_deg_down;
    case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
        return wooden_rc_track_right_banked_flat_to_right_banked_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT:
        return wooden_rc_track_left_banked_25_deg_down_to_left_banked_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT:
        return wooden_rc_track_right_banked_25_deg_down_to_right_banked_flat;
    case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP:
        return wooden_rc_track_flat_to_left_banked_25_deg_up;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
        return wooden_rc_track_flat_to_right_banked_25_deg_up;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT:
        return wooden_rc_track_left_banked_25_deg_up_to_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT:
        return wooden_rc_track_right_banked_25_deg_up_to_flat;
    case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
        return wooden_rc_track_flat_to_left_banked_25_deg_down;
    case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
        return wooden_rc_track_flat_to_right_banked_25_deg_down;
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT:
        return wooden_rc_track_left_banked_25_deg_down_to_flat;
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT:
        return wooden_rc_track_right_banked_25_deg_down_to_flat;
    }
    return NULL;
}
