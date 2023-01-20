/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../paint/tile_element/Paint.TileElement.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"
#include "WoodenRollerCoaster.h"

enum
{
    SPR_CLASSIC_WOODEN_RC_BRAKE_0 = SPR_CSG_BEGIN + 64985,
    SPR_CLASSIC_WOODEN_RC_BRAKE_1 = SPR_CSG_BEGIN + 64986,

    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_SW_NE = SPR_CSG_BEGIN + 64788,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_NW_SE = SPR_CSG_BEGIN + 64789,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_NE_SW = SPR_CSG_BEGIN + 64790,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_SE_NW = SPR_CSG_BEGIN + 64791,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE = SPR_CSG_BEGIN + 64816,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW = SPR_CSG_BEGIN + 64817,

    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_SW_NE = SPR_CSG_BEGIN + 64792,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_NW_SE = SPR_CSG_BEGIN + 64793,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_NE_SW = SPR_CSG_BEGIN + 64794,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_SE_NW = SPR_CSG_BEGIN + 64795,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_FRONT_NE_SW = SPR_CSG_BEGIN + 64818,

    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_SW_NE = SPR_CSG_BEGIN + 64796,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_NW_SE = SPR_CSG_BEGIN + 64797,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_NE_SW = SPR_CSG_BEGIN + 64798,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_SE_NW = SPR_CSG_BEGIN + 64799,

    SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_SW_NE = SPR_CSG_BEGIN + 64800,
    SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_NW_SE = SPR_CSG_BEGIN + 64801,
    SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_NE_SW = SPR_CSG_BEGIN + 64802,
    SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_SE_NW = SPR_CSG_BEGIN + 64803,
    SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_FRONT_NW_SE = SPR_CSG_BEGIN + 64819,
    SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_FRONT_NE_SW = SPR_CSG_BEGIN + 64820,

    SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_SW_NE = SPR_CSG_BEGIN + 64804,
    SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_NW_SE = SPR_CSG_BEGIN + 64805,
    SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_NE_SW = SPR_CSG_BEGIN + 64806,
    SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_SE_NW = SPR_CSG_BEGIN + 64807,
    SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_FRONT_NW_SE = SPR_CSG_BEGIN + 64821,
    SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_FRONT_NE_SW = SPR_CSG_BEGIN + 64822,

    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_SW_NE = SPR_CSG_BEGIN + 64808,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_NW_SE = SPR_CSG_BEGIN + 64809,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_NE_SW = SPR_CSG_BEGIN + 64810,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_SE_NW = SPR_CSG_BEGIN + 64811,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_FRONT_NW_SE = SPR_CSG_BEGIN + 64823,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_FRONT_NE_SW = SPR_CSG_BEGIN + 64824,

    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_SW_NE = SPR_CSG_BEGIN + 64812,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_NW_SE = SPR_CSG_BEGIN + 64813,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_NE_SW = SPR_CSG_BEGIN + 64814,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_SE_NW = SPR_CSG_BEGIN + 64815,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_FRONT_NW_SE = SPR_CSG_BEGIN + 64825,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_FRONT_NE_SW = SPR_CSG_BEGIN + 64826,

    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_0 = 23635 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_2 = 23636 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_3 = 23637 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_5 = 23638 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_6 = 23639 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_0 = 23640 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_2 = 23641 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_3 = 23642 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_5 = 23643 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_6 = 23644 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_0 = 23645 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_2 = 23646 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_3 = 23647 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_5 = 23648 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_6 = 23649 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_0 = 23650 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_2 = 23651 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_3 = 23652 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_5 = 23653 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_6 = 23654 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_0 = 23655 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_2 = 23656 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_3 = 23657 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_5 = 23658 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_6 = 23659 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_0 = 23660 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_2 = 23661 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_3 = 23662 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_5 = 23663 + SPR_CSG_BEGIN + 41290,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_6 = 23664 + SPR_CSG_BEGIN + 41290,

    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SW_SE_SEQ_0 = SPR_CSG_BEGIN + 65129,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SW_SE_SEQ_2 = SPR_CSG_BEGIN + 65130,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SW_SE_SEQ_3 = SPR_CSG_BEGIN + 65131,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NW_SW_SEQ_0 = SPR_CSG_BEGIN + 65132,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NW_SW_SEQ_2 = SPR_CSG_BEGIN + 65133,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NW_SW_SEQ_3 = SPR_CSG_BEGIN + 65134,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NE_NW_SEQ_0 = SPR_CSG_BEGIN + 65135,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NE_NW_SEQ_2 = SPR_CSG_BEGIN + 65136,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NE_NW_SEQ_3 = SPR_CSG_BEGIN + 65137,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SE_NE_SEQ_0 = SPR_CSG_BEGIN + 65138,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SE_NE_SEQ_2 = SPR_CSG_BEGIN + 65139,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SE_NE_SEQ_3 = SPR_CSG_BEGIN + 65140,

    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_SW_SE_SEQ_0 = SPR_CSG_BEGIN + 65141,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_SW_SE_SEQ_2 = SPR_CSG_BEGIN + 65142,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_SW_SE_SEQ_3 = SPR_CSG_BEGIN + 65143,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_NE_NW_SEQ_0 = SPR_CSG_BEGIN + 65144,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_NE_NW_SEQ_2 = SPR_CSG_BEGIN + 65145,
    SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_NE_NW_SEQ_3 = SPR_CSG_BEGIN + 65146,

    SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_0 = SPR_CSG_BEGIN + 65309,
    SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_1 = SPR_CSG_BEGIN + 65310,
    SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_2 = SPR_CSG_BEGIN + 65311,
    SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_3 = SPR_CSG_BEGIN + 65312,
    SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_FRONT_0 = SPR_CSG_BEGIN + 65313,
    SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_FRONT_2 = SPR_CSG_BEGIN + 65314,

    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_0 = SPR_CSG_BEGIN + 65315,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_1 = SPR_CSG_BEGIN + 65316,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_2 = SPR_CSG_BEGIN + 65317,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_3 = SPR_CSG_BEGIN + 65318,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_FRONT_0 = SPR_CSG_BEGIN + 65319,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_FRONT_2 = SPR_CSG_BEGIN + 65320,

    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_0 = SPR_CSG_BEGIN + 65321,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_1 = SPR_CSG_BEGIN + 65322,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_2 = SPR_CSG_BEGIN + 65323,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_3 = SPR_CSG_BEGIN + 65324,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_FRONT_0 = SPR_CSG_BEGIN + 65325,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_FRONT_2 = SPR_CSG_BEGIN + 65326,

    SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_0 = SPR_CSG_BEGIN + 65327,
    SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_1 = SPR_CSG_BEGIN + 65328,
    SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_2 = SPR_CSG_BEGIN + 65329,
    SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_3 = SPR_CSG_BEGIN + 65330,
    SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_FRONT_0 = SPR_CSG_BEGIN + 65331,
    SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_FRONT_2 = SPR_CSG_BEGIN + 65332,

    SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_0 = SPR_CSG_BEGIN + 65333,
    SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_1 = SPR_CSG_BEGIN + 65334,
    SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_2 = SPR_CSG_BEGIN + 65335,
    SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_3 = SPR_CSG_BEGIN + 65336,
    SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_FRONT_0 = SPR_CSG_BEGIN + 65337,
    SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_FRONT_2 = SPR_CSG_BEGIN + 65338,

    SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_0 = SPR_CSG_BEGIN + 65339,
    SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_1 = SPR_CSG_BEGIN + 65340,
    SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_2 = SPR_CSG_BEGIN + 65341,
    SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_3 = SPR_CSG_BEGIN + 65342,
    SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_FRONT_0 = SPR_CSG_BEGIN + 65343,
    SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_FRONT_2 = SPR_CSG_BEGIN + 65344,

    SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_0 = SPR_CSG_BEGIN + 65345,
    SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_1 = SPR_CSG_BEGIN + 65346,
    SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_2 = SPR_CSG_BEGIN + 65347,
    SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_3 = SPR_CSG_BEGIN + 65348,
    SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_FRONT_0 = SPR_CSG_BEGIN + 65349,
    SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_FRONT_2 = SPR_CSG_BEGIN + 65350,

    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_0_SEQ_0 = SPR_CSG_BEGIN + 65399,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_0_SEQ_1 = SPR_CSG_BEGIN + 65400,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_0_SEQ_2 = SPR_CSG_BEGIN + 65401,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_0_SEQ_4 = SPR_CSG_BEGIN + 65402,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_0_SEQ_0 = SPR_CSG_BEGIN + 65415,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_0_SEQ_1 = SPR_CSG_BEGIN + 65416,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_0_SEQ_2 = SPR_CSG_BEGIN + 65417,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_0_SEQ_4 = SPR_CSG_BEGIN + 65418,

    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_1_SEQ_0 = SPR_CSG_BEGIN + 65403,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_1_SEQ_1 = SPR_CSG_BEGIN + 65404,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_1_SEQ_2 = SPR_CSG_BEGIN + 65405,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_1_SEQ_4 = SPR_CSG_BEGIN + 65406,

    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_2_SEQ_0 = SPR_CSG_BEGIN + 65407,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_2_SEQ_1 = SPR_CSG_BEGIN + 65408,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_2_SEQ_2 = SPR_CSG_BEGIN + 65409,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_2_SEQ_4 = SPR_CSG_BEGIN + 65410,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_2_SEQ_0 = SPR_CSG_BEGIN + 65419,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_2_SEQ_1 = SPR_CSG_BEGIN + 65420,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_2_SEQ_2 = SPR_CSG_BEGIN + 65421,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_2_SEQ_4 = SPR_CSG_BEGIN + 65422,

    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_3_SEQ_0 = SPR_CSG_BEGIN + 65411,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_3_SEQ_1 = SPR_CSG_BEGIN + 65412,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_3_SEQ_2 = SPR_CSG_BEGIN + 65413,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_3_SEQ_4 = SPR_CSG_BEGIN + 65414,

    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_0_SEQ_0 = SPR_CSG_BEGIN + 65399 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_0_SEQ_1 = SPR_CSG_BEGIN + 65400 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_0_SEQ_2 = SPR_CSG_BEGIN + 65401 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_0_SEQ_4 = SPR_CSG_BEGIN + 65402 + 24,

    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_1_SEQ_0 = SPR_CSG_BEGIN + 65403 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_1_SEQ_1 = SPR_CSG_BEGIN + 65404 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_1_SEQ_2 = SPR_CSG_BEGIN + 65405 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_1_SEQ_4 = SPR_CSG_BEGIN + 65406 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_1_SEQ_0 = SPR_CSG_BEGIN + 65439,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_1_SEQ_1 = SPR_CSG_BEGIN + 65440,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_1_SEQ_2 = SPR_CSG_BEGIN + 65441,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_1_SEQ_4 = SPR_CSG_BEGIN + 65442,

    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_2_SEQ_0 = SPR_CSG_BEGIN + 65407 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_2_SEQ_1 = SPR_CSG_BEGIN + 65408 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_2_SEQ_2 = SPR_CSG_BEGIN + 65409 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_2_SEQ_4 = SPR_CSG_BEGIN + 65410 + 24,

    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_3_SEQ_0 = SPR_CSG_BEGIN + 65411 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_3_SEQ_1 = SPR_CSG_BEGIN + 65412 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_3_SEQ_2 = SPR_CSG_BEGIN + 65413 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_3_SEQ_4 = SPR_CSG_BEGIN + 65414 + 24,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_3_SEQ_0 = SPR_CSG_BEGIN + 65443,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_3_SEQ_1 = SPR_CSG_BEGIN + 65444,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_3_SEQ_2 = SPR_CSG_BEGIN + 65445,
    SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_3_SEQ_4 = SPR_CSG_BEGIN + 65446,
};

static void ClassicWoodenRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)

{
    static constexpr const sprite_bb_2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_SW_NE, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_NW_SE, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE, 0, { 0, 0, 0 }, { 26, 0, 5 }, { 1, 32, 9 } },
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_NE_SW, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_SE_NW, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW, 0, { 0, 0, 0 }, { 26, 0, 5 }, { 1, 32, 9 } },
        } }
    };
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    WoodenASupportsPaintSetup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void ClassicWoodenRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_SW_NE, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_NW_SE, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
            {},
        } },

        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_NE_SW, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_FRONT_NE_SW, 0, { 0, 0, 0 }, { 0, 26, 5 }, { 32, 1, 9 } },
        } },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_SE_NW, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
                {},
            },
        }
    };
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    WoodenASupportsPaintSetup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void ClassicWoodenRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrackFlatToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrackFlatToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_SW_NE, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_NW_SE, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
            {},
        } },

        { {
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_NE_SW, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            {},
        } },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_SE_NW, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
                {},
            },
        }
    };
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenASupportsPaintSetup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void ClassicWoodenRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_SW_NE, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_NW_SE, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_FRONT_NW_SE, 0, { 0, 0, 0 }, { 26, 0, 5 }, { 1, 32, 9 } },
        } },

        { {
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_NE_SW, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_FRONT_NE_SW, 0, { 0, 0, 0 }, { 0, 26, 5 }, { 32, 1, 9 } },
        } },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_SE_NW, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
                {},
            },
        }
    };
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    WoodenASupportsPaintSetup(session, direction & 1, 1 + direction, height, session.TrackColours[SCHEME_SUPPORTS]);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

static void ClassicWoodenRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_SW_NE, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_NW_SE, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
            { SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_FRONT_NW_SE, 0, { 0, 0, 0 }, { 26, 0, 5 }, { 1, 32, 9 } },
        } },

        { {
            { SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_NE_SW, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            { SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_FRONT_NE_SW, 0, { 0, 0, 0 }, { 0, 26, 5 }, { 32, 1, 9 } },
        } },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_SE_NW, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
                {},
            },
        }
    };

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    WoodenASupportsPaintSetup(session, direction & 1, 1 + direction, height, session.TrackColours[SCHEME_SUPPORTS]);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

static void ClassicWoodenRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_SW_NE, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_NW_SE, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_FRONT_NW_SE, 0, { 0, 0, 0 }, { 26, 0, 5 }, { 1, 32, 9 } },
        } },

        { {
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_NE_SW, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_FRONT_NE_SW, 0, { 0, 0, 0 }, { 0, 26, 5 }, { 32, 1, 9 } },
        } },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_SE_NW, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
                {},
            },
        }
    };

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    WoodenASupportsPaintSetup(session, direction & 1, 5 + direction, height, session.TrackColours[SCHEME_SUPPORTS]);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

static void ClassicWoodenRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_SW_NE, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_NW_SE, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_FRONT_NW_SE, 0, { 0, 0, 0 }, { 26, 0, 5 }, { 1, 32, 9 } },
        } },

        { {
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_NE_SW, 0, { 0, 0, 0 }, { 0, 3, 0 }, { 32, 25, 2 } },
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_FRONT_NE_SW, 0, { 0, 0, 0 }, { 0, 26, 5 }, { 32, 1, 9 } },
        } },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_SE_NW, 0, { 0, 0, 0 }, { 3, 0, 0 }, { 25, 32, 2 } },
                {},
            },
        }
    };

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    WoodenASupportsPaintSetup(session, direction & 1, 5 + direction, height, session.TrackColours[SCHEME_SUPPORTS]);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

static void ClassicWoodenRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrack25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][7][2] = {
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_0, 0, { 0, 2, 0 }, { 0, 2, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_0, 0, { 0, 2, 0 }, { 0, 2, 27 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_2, 0, { 0, 16, 0 }, { 0, 16, 0 }, { 32, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_2,
                  0,
                  { 0, 16, 0 },
                  { 0, 16, 27 },
                  { 32, 16, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_3, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_3, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 16, 0 } },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_5, 0, { 16, 0, 0 }, { 16, 0, 0 }, { 16, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_5,
                  0,
                  { 16, 0, 0 },
                  { 16, 0, 27 },
                  { 16, 32, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_6, 0, { 2, 0, 0 }, { 2, 0, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_6, 0, { 2, 0, 0 }, { 2, 0, 27 }, { 32, 32, 0 } },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_0, 0, { 2, 0, 0 }, { 2, 0, 0 }, { 32, 32, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_2, 0, { 16, 0, 0 }, { 16, 0, 0 }, { 16, 32, 2 } },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_3, 0, { 0, 16, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_5, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_6, 0, { 0, 2, 0 }, { 0, 2, 0 }, { 32, 27, 2 } },
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_0, 0, { 0, 2, 0 }, { 0, 2, 0 }, { 32, 27, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_0, 0, { 0, 2, 0 }, { 0, 2, 27 }, { 32, 27, 0 } },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_2, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_2, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 16, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_3, 0, { 16, 16, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_3,
                  0,
                  { 16, 16, 0 },
                  { 16, 16, 27 },
                  { 16, 16, 0 } },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_5, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_5, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 32, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_6, 0, { 2, 0, 0 }, { 2, 0, 0 }, { 27, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_6, 0, { 2, 0, 0 }, { 2, 0, 27 }, { 27, 32, 0 } },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_0, 0, { 2, 0, 0 }, { 2, 0, 0 }, { 27, 32, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 2, 27 }, { 32, 27, 0 } },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_2, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 16, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_3, 0, { 16, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
                { 0, 0, { 0, 0, 0 }, { 16, 16, 27 }, { 16, 16, 0 } },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_5, 0, { 0, 16, 0 }, { 0, 16, 0 }, { 32, 16, 2 } },
                { 0, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 32, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_6, 0, { 0, 2, 0 }, { 0, 2, 0 }, { 32, 32, 2 } },
                { 0, 0, { 0, 0, 0 }, { 2, 0, 27 }, { 27, 32, 0 } },
            },
        },
    };
    static constexpr const int8_t supportType[4][7] = {
        { 0, -1, 4, 2, -1, 4, 1 },
        { 1, -1, 5, 3, -1, 5, 0 },
        { 0, -1, 2, 4, -1, 2, 1 },
        { 1, -1, 3, 5, -1, 3, 0 },
    };

    static constexpr const int blockedSegments[7] = {
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
        SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC,
        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
        SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC,
        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4,
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
    };

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    TrackPaintUtilRightQuarterTurn5TilesTunnel(session, height, direction, trackSequence, TUNNEL_SQUARE_FLAT);

    if (supportType[direction][trackSequence] != -1)
    {
        WoodenASupportsPaintSetup(
            session, supportType[direction][trackSequence], 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void ClassicWoodenRCTrackBankedLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    ClassicWoodenRCTrackBankedRightQuarterTurn5(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][4][2] = {
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SW_SE_SEQ_0, 0, { 0, 6, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_SW_SE_SEQ_0, 0, { 0, 6, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SW_SE_SEQ_2, 0, { 16, 16, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_SW_SE_SEQ_2,
                  0,
                  { 16, 16, 0 },
                  { 16, 16, 27 },
                  { 16, 16, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SW_SE_SEQ_3, 0, { 6, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_SW_SE_SEQ_3, 0, { 6, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NW_SW_SEQ_0, 0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NW_SW_SEQ_2, 0, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NW_SW_SEQ_3, 0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NE_NW_SEQ_0, 0, { 0, 0, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_NE_NW_SEQ_0, 0, { 0, 0, 0 }, { 0, 6, 27 }, { 32, 20, 0 } },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NE_NW_SEQ_2, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_NE_NW_SEQ_2, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 16, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NE_NW_SEQ_3, 0, { 0, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_NE_NW_SEQ_3, 0, { 0, 0, 0 }, { 6, 0, 27 }, { 20, 32, 0 } },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SE_NE_SEQ_0, 0, { 6, 0, 0 }, { 6, 0, 0 }, { 20, 32, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SE_NE_SEQ_2, 0, { 0, 16, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SE_NE_SEQ_3, 0, { 0, 6, 0 }, { 0, 6, 0 }, { 32, 20, 2 } },
                {},
            },
        },
    };

    static constexpr const int8_t supportType[4][4] = {
        { 4, -1, -1, 4 },
        { 5, -1, -1, 5 },
        { 2, -1, -1, 2 },
        { 3, -1, -1, 3 },
    };

    static constexpr const int blockedSegments[4] = {
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
        0,
        SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0,
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
    };

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, height, direction, trackSequence, TUNNEL_SQUARE_FLAT);

    if (supportType[direction][trackSequence] != -1)
    {
        WoodenASupportsPaintSetup(
            session, supportType[direction][trackSequence], 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void ClassicWoodenRCTrackLeftQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    ClassicWoodenRCTrackRightQuarterTurn3Bank(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackLeftEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][5][2] = {
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_0_SEQ_0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_0_SEQ_1, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 16, 2 } },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_0_SEQ_2, 0, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_0_SEQ_4, 0, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_1_SEQ_0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_1_SEQ_0, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 32, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_1_SEQ_1, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 34, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_1_SEQ_1, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 32, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_1_SEQ_2, 0, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_1_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  { 16, 16, 27 },
                  { 16, 16, 0 } },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_1_SEQ_4, 0, { 0, 0, 0 }, { 16, 0, 0 }, { 18, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_1_SEQ_4, 0, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 16, 0 } },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_2_SEQ_0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_2_SEQ_1, 0, { 0, 0, 0 }, { 0, 16, 0 }, { 32, 16, 2 } },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_2_SEQ_2, 0, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_2_SEQ_4, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_3_SEQ_0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_3_SEQ_0, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 32, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_3_SEQ_1, 0, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_3_SEQ_1, 0, { 0, 0, 0 }, { 16, 0, 27 }, { 16, 32, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_3_SEQ_2, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_3_SEQ_2, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 16, 0 } },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_3_SEQ_4, 0, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_3_SEQ_4, 0, { 0, 0, 0 }, { 0, 16, 27 }, { 16, 16, 0 } },
            },
        },
    };

    static constexpr const int8_t supportType[4][5] = {
        { 0, 0, 3, 5, -1 },
        { 1, 1, 4, 2, -1 },
        { 0, 0, 5, 3, -1 },
        { 1, 1, 2, 4, -1 },
    };

    static constexpr const int blockedSegments[5] = {
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
    };

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }

    if (supportType[direction][trackSequence] != -1)
    {
        WoodenASupportsPaintSetup(
            session, supportType[direction][trackSequence], 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void ClassicWoodenRCTrackRightEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][5][2] = {
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_0_SEQ_0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_0_SEQ_0, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 32, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_0_SEQ_1, 0, { 0, 0, 0 }, { 0, 16, 0 }, { 32, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_0_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  { 0, 16, 27 },
                  { 32, 16, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_0_SEQ_2, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_0_SEQ_2, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 16, 16, 0 } },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_0_SEQ_4, 0, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_0_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  { 16, 0, 27 },
                  { 16, 16, 0 } },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_1_SEQ_0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_1_SEQ_1, 0, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 32, 0 } },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_1_SEQ_2, 0, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 16, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_1_SEQ_4, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 16, 2 } },
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_2_SEQ_0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_2_SEQ_0, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 32, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_2_SEQ_1, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 34, 16, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_2_SEQ_1, 0, { 0, 0, 0 }, { 0, 0, 27 }, { 32, 16, 0 } },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_2_SEQ_2, 0, { 0, 0, 0 }, { 4, 4, 0 }, { 28, 28, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_2_SEQ_2, 0, { 0, 0, 0 }, { 4, 4, 27 }, { 28, 28, 0 } },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_2_SEQ_4, 0, { 0, 0, 0 }, { 0, 16, 0 }, { 16, 18, 2 } },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_2_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  { 0, 16, 27 },
                  { 16, 16, 0 } },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_3_SEQ_0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 32, 32, 2 } },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_3_SEQ_1, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 16, 32, 0 } },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_3_SEQ_2, 0, { 0, 0, 0 }, { 16, 0, 0 }, { 16, 16, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_3_SEQ_4, 0, { 0, 0, 0 }, { 16, 16, 0 }, { 16, 16, 2 } },
                {},
            },
        },
    };

    static constexpr const int8_t supportType[4][5] = {
        { 0, 0, 2, 4, -1 },
        { 1, 1, 3, 5, -1 },
        { 0, 0, 4, 2, -1 },
        { 1, 1, 5, 3, -1 },
    };

    static constexpr const int blockedSegments[5] = {
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
    };

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }

    if (supportType[direction][trackSequence] != -1)
    {
        WoodenASupportsPaintSetup(
            session, supportType[direction][trackSequence], 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void ClassicWoodenRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    ClassicWoodenRCTrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    ClassicWoodenRCTrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackDiagFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_0, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_FRONT_0, 0, { -16, -16, 0 }, { -16, -16, 27 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_1, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_2, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 23, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_FRONT_2, 0, { -16, -16, 0 }, { -16, -16, 27 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_3, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
    };

    static constexpr const int8_t supportType[4][4] = {
        { -1, 2, 4, -1 },
        { -1, 3, 5, -1 },
        { -1, 4, 2, -1 },
        { -1, 5, 3, -1 },
    };

    if (supportType[direction][trackSequence] != -1)
    {
        WoodenASupportsPaintSetup(
            session, supportType[direction][trackSequence], 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void ClassicWoodenRCTrackDiagFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_0, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_FRONT_0, 0, { -16, -16, 0 }, { -16, -16, 27 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_1, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_2, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 23, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_FRONT_2, 0, { -16, -16, 0 }, { -16, -16, 27 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_3, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
    };

    static constexpr const int8_t supportType[4][4] = {
        { -1, 2, 4, -1 },
        { -1, 3, 5, -1 },
        { -1, 4, 2, -1 },
        { -1, 5, 3, -1 },
    };

    if (supportType[direction][trackSequence] != -1)
    {
        WoodenASupportsPaintSetup(
            session, supportType[direction][trackSequence], 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void ClassicWoodenRCTrackDiagLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrackDiagFlatToRightBank(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackDiagRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrackDiagFlatToLeftBank(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackDiagLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_0, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_FRONT_0, 0, { -16, -16, 0 }, { -16, -16, 27 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_1, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_2, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 23, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_FRONT_2, 0, { -16, -16, 0 }, { -16, -16, 27 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_3, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
    };

    static constexpr const int8_t supportType[4][4] = {
        { -1, 2, 4, -1 },
        { -1, 3, 5, -1 },
        { -1, 4, 2, -1 },
        { -1, 5, 3, -1 },
    };

    if (supportType[direction][trackSequence] != -1)
    {
        WoodenASupportsPaintSetup(
            session, supportType[direction][trackSequence], 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void ClassicWoodenRCTrackDiagRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrackDiagLeftBank(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackDiagLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_0, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_FRONT_0, 0, { -16, -16, 0 }, { -16, -16, 35 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_1, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_2, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 23, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_FRONT_2, 0, { -16, -16, 0 }, { -16, -16, 35 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_3, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
    };

    static constexpr const int8_t supportType[4][4] = {
        { -1, 2, 4, -1 },
        { -1, 3, 5, -1 },
        { -1, 4, 2, -1 },
        { -1, 5, 3, -1 },
    };

    if (supportType[direction][trackSequence] != -1)
    {
        WoodenASupportsPaintSetup(
            session, supportType[direction][trackSequence], 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

static void ClassicWoodenRCTrackDiagRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_0, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_FRONT_0, 0, { -16, -16, 0 }, { -16, -16, 35 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_1, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_2, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 23, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_FRONT_2, 0, { -16, -16, 0 }, { -16, -16, 35 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_3, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
    };

    static constexpr const int8_t supportType[4][4] = {
        { -1, 2, 4, -1 },
        { -1, 3, 5, -1 },
        { -1, 4, 2, -1 },
        { -1, 5, 3, -1 },
    };

    if (supportType[direction][trackSequence] != -1)
    {
        WoodenASupportsPaintSetup(
            session, supportType[direction][trackSequence], 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

static void ClassicWoodenRCTrackDiag25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_0, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_FRONT_0, 0, { -16, -16, 0 }, { -16, -16, 35 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_1, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_2, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 23, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_FRONT_2, 0, { -16, -16, 0 }, { -16, -16, 35 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_3, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
    };

    static constexpr const int8_t supportType[4][4] = {
        { -1, 2, 4, -1 },
        { -1, 3, 5, -1 },
        { -1, 4, 2, -1 },
        { -1, 5, 3, -1 },
    };

    if (supportType[direction][trackSequence] != -1)
    {
        WoodenASupportsPaintSetup(
            session, supportType[direction][trackSequence], 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

static void ClassicWoodenRCTrackDiag25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const sprite_bb_2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_0, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_FRONT_0, 0, { -16, -16, 0 }, { -16, -16, 35 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_1, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
        },
        {
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_2, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 23, 2 } },
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_FRONT_2, 0, { -16, -16, 0 }, { -16, -16, 35 }, { 32, 32, 0 } },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_3, 0, { -16, -16, 0 }, { -16, -16, 0 }, { 32, 32, 2 } },
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
            {
                {},
                {},
            },
        },
    };

    static constexpr const int8_t supportType[4][4] = {
        { -1, 2, 4, -1 },
        { -1, 3, 5, -1 },
        { -1, 4, 2, -1 },
        { -1, 5, 3, -1 },
    };

    if (supportType[direction][trackSequence] != -1)
    {
        WoodenASupportsPaintSetup(
            session, supportType[direction][trackSequence], 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

static void ClassicWoodenRCTrackDiagLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrackDiag25DegUpToRightBank(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackDiagRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrackDiag25DegUpToLeftBank(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackDiag25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrackDiagRightBankTo25DegUp(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}

static void ClassicWoodenRCTrackDiag25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement)
{
    ClassicWoodenRCTrackDiagLeftBankTo25DegUp(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}

// Stylistically, this coaster is _very_ similar to the regular Wooden Roller Coaster.
// The only difference is to which parts the colours are applied, and the degree of the banking.
// As such, all non-banked pieces are simply drawn as regular wooden roller coaster pieces with a different paint scheme.
TRACK_PAINT_FUNCTION GetTrackPaintFunctionClassicWoodenRC(int32_t trackType)
{
    if (!IsCsgLoaded())
    {
        return GetTrackPaintFunctionClassicWoodenRCFallback(trackType);
    }

    switch (trackType)
    {
        case TrackElemType::FlatToLeftBank:
            return ClassicWoodenRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return ClassicWoodenRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return ClassicWoodenRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return ClassicWoodenRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return ClassicWoodenRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return ClassicWoodenRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return ClassicWoodenRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return ClassicWoodenRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return ClassicWoodenRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return ClassicWoodenRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return ClassicWoodenRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return ClassicWoodenRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return ClassicWoodenRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return ClassicWoodenRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return ClassicWoodenRCTrackLeftBank;
        case TrackElemType::RightBank:
            return ClassicWoodenRCTrackRightBank;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return ClassicWoodenRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return ClassicWoodenRCTrackRightQuarterTurn3Bank;
        case TrackElemType::LeftEighthBankToDiag:
            return ClassicWoodenRCTrackLeftEighthBankToDiag;
        case TrackElemType::RightEighthBankToDiag:
            return ClassicWoodenRCTrackRightEighthBankToDiag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return ClassicWoodenRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return ClassicWoodenRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::DiagFlatToLeftBank:
            return ClassicWoodenRCTrackDiagFlatToLeftBank;
        case TrackElemType::DiagFlatToRightBank:
            return ClassicWoodenRCTrackDiagFlatToRightBank;
        case TrackElemType::DiagLeftBankToFlat:
            return ClassicWoodenRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return ClassicWoodenRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return ClassicWoodenRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::DiagRightBankToUp25:
            return ClassicWoodenRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::DiagUp25ToLeftBank:
            return ClassicWoodenRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::DiagUp25ToRightBank:
            return ClassicWoodenRCTrackDiag25DegUpToRightBank;
        case TrackElemType::DiagLeftBankToDown25:
            return ClassicWoodenRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::DiagRightBankToDown25:
            return ClassicWoodenRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::DiagDown25ToLeftBank:
            return ClassicWoodenRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return ClassicWoodenRCTrackDiag25DegDownToRightBank;
        case TrackElemType::DiagLeftBank:
            return ClassicWoodenRCTrackDiagLeftBank;
        case TrackElemType::DiagRightBank:
            return ClassicWoodenRCTrackDiagRightBank;
    }

    return GetTrackPaintFunctionClassicWoodenRCFallback(trackType);
}
