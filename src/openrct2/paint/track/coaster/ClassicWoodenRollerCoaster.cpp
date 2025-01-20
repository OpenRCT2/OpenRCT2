/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../drawing/Drawing.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../sprites.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"
#include "WoodenRollerCoaster.hpp"

using namespace OpenRCT2;

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

    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_LEFT_BANK_0 = SPR_CSG_BEGIN + 65315,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_LEFT_BANK_1 = SPR_CSG_BEGIN + 65316,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_LEFT_BANK_2 = SPR_CSG_BEGIN + 65317,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_LEFT_BANK_3 = SPR_CSG_BEGIN + 65318,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_LEFT_BANK_FRONT_0 = SPR_CSG_BEGIN + 65319,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_LEFT_BANK_FRONT_2 = SPR_CSG_BEGIN + 65320,

    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_RIGHT_BANK_0 = SPR_CSG_BEGIN + 65321,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_RIGHT_BANK_1 = SPR_CSG_BEGIN + 65322,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_RIGHT_BANK_2 = SPR_CSG_BEGIN + 65323,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_RIGHT_BANK_3 = SPR_CSG_BEGIN + 65324,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_RIGHT_BANK_FRONT_0 = SPR_CSG_BEGIN + 65325,
    SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_RIGHT_BANK_FRONT_2 = SPR_CSG_BEGIN + 65326,

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

    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_0_0 = SPR_CSG_BEGIN + 65399,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_1_0 = SPR_CSG_BEGIN + 65400,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_2_0 = SPR_CSG_BEGIN + 65401,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_3_0 = SPR_CSG_BEGIN + 65402,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_0_1 = SPR_CSG_BEGIN + 65403,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_1_1 = SPR_CSG_BEGIN + 65404,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_2_1 = SPR_CSG_BEGIN + 65405,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_3_1 = SPR_CSG_BEGIN + 65406,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_0_2 = SPR_CSG_BEGIN + 65407,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_1_2 = SPR_CSG_BEGIN + 65408,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_2_2 = SPR_CSG_BEGIN + 65409,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_3_2 = SPR_CSG_BEGIN + 65410,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_0_3 = SPR_CSG_BEGIN + 65411,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_1_3 = SPR_CSG_BEGIN + 65412,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_2_3 = SPR_CSG_BEGIN + 65413,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_3_3 = SPR_CSG_BEGIN + 65414,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_0_0 = SPR_CSG_BEGIN + 65415,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_1_0 = SPR_CSG_BEGIN + 65416,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_2_0 = SPR_CSG_BEGIN + 65417,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_3_0 = SPR_CSG_BEGIN + 65418,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_0_2 = SPR_CSG_BEGIN + 65419,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_1_2 = SPR_CSG_BEGIN + 65420,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_2_2 = SPR_CSG_BEGIN + 65421,
    SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_3_2 = SPR_CSG_BEGIN + 65422,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_0_0 = SPR_CSG_BEGIN + 65423,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_1_0 = SPR_CSG_BEGIN + 65424,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_2_0 = SPR_CSG_BEGIN + 65425,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_3_0 = SPR_CSG_BEGIN + 65426,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_0_1 = SPR_CSG_BEGIN + 65427,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_1_1 = SPR_CSG_BEGIN + 65428,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_2_1 = SPR_CSG_BEGIN + 65429,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_3_1 = SPR_CSG_BEGIN + 65430,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_0_2 = SPR_CSG_BEGIN + 65431,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_1_2 = SPR_CSG_BEGIN + 65432,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_2_2 = SPR_CSG_BEGIN + 65433,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_3_2 = SPR_CSG_BEGIN + 65434,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_0_3 = SPR_CSG_BEGIN + 65435,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_1_3 = SPR_CSG_BEGIN + 65436,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_2_3 = SPR_CSG_BEGIN + 65437,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_3_3 = SPR_CSG_BEGIN + 65438,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_0_1 = SPR_CSG_BEGIN + 65439,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_1_1 = SPR_CSG_BEGIN + 65440,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_2_1 = SPR_CSG_BEGIN + 65441,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_3_1 = SPR_CSG_BEGIN + 65442,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_0_3 = SPR_CSG_BEGIN + 65443,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_1_3 = SPR_CSG_BEGIN + 65444,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_2_3 = SPR_CSG_BEGIN + 65445,
    SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_3_3 = SPR_CSG_BEGIN + 65446,
};

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kFlatToLeftBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_SW_NE,
    },
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_NW_SE,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE,
    },
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_NE_SW,
    },
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_SE_NW,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW,
    },
} };

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kFlatToRightBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_SW_NE,
    },
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_NW_SE,
    },
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_NE_SW,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_FRONT_NE_SW,
    },
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_SE_NW,
    },
} };

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kLeftBankImages = { {
    { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_SW_NE },
    { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_NW_SE },
    { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_NE_SW },
    { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_SE_NW },
} };

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kUp25ToLeftBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_SW_NE,
    },
    {
        SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_NW_SE,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_FRONT_NW_SE,
    },
    {
        SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_NE_SW,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_FRONT_NE_SW,
    },
    {
        SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_SE_NW,
    },
} };

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kUp25ToRightBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_SW_NE,
    },
    {
        SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_NW_SE,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_FRONT_NW_SE,
    },
    {
        SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_NE_SW,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_FRONT_NE_SW,
    },
    {
        SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_SE_NW,
    },
} };

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kLeftBankToUp25Images = { {
    {
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_SW_NE,
    },
    {
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_NW_SE,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_FRONT_NW_SE,
    },
    {
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_NE_SW,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_FRONT_NE_SW,
    },
    {
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_SE_NW,
    },
} };
static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kRightBankToUp25Images = { {
    {
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_SW_NE,
    },
    {
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_NW_SE,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_FRONT_NW_SE,
    },
    {
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_NE_SW,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_FRONT_NE_SW,
    },
    {
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_SE_NW,
    },
} };

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kDiagFlatToLeftBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_LEFT_BANK_0,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_LEFT_BANK_FRONT_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_LEFT_BANK_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_LEFT_BANK_2,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_LEFT_BANK_FRONT_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_LEFT_BANK_3,
    },
} };

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kDiagFlatToRightBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_RIGHT_BANK_0,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_RIGHT_BANK_FRONT_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_RIGHT_BANK_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_RIGHT_BANK_2,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_RIGHT_BANK_FRONT_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_RIGHT_BANK_3,
    },
} };

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kDiagLeftBankTo25DegUpImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_0,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_FRONT_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_2,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_FRONT_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_3,
    },
} };

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kDiagRightBankTo25DegUpImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_0,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_FRONT_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_2,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_FRONT_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_3,
    },
} };

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kDiagUp25ToLeftBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_0,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_FRONT_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_2,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_FRONT_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_3,
    },
} };

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kDiagLeftBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_0,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_FRONT_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_2,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_FRONT_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_3,
    },
} };

static constexpr std::array<WoodenTrackSection, kNumOrthogonalDirections> kDiagUp25ToRightBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_0,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_FRONT_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_2,
        kImageIndexUndefined,
        SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_FRONT_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_3,
    },
} };

static constexpr std::array<std::array<WoodenTrackSection, kNumOrthogonalDirections>, 4> kLeftEighthBankToDiagImages = { {
    { {
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_0_0,
        },
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_0_1,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_0_1,
        },
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_0_2,
        },
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_0_3,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_0_3,
        },
    } },
    { {
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_1_0,
        },
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_1_1,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_1_1,
        },
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_1_2,
        },
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_1_3,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_1_3,
        },
    } },
    { {
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_2_0,
        },
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_2_1,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_2_1,
        },
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_2_2,
        },
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_2_3,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_2_3,
        },
    } },
    { {
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_3_0,
        },
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_3_1,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_3_1,
        },
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_3_2,
        },
        {
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_3_3,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_LEFT_EIGHTH_BANK_TO_DIAG_FRONT_3_3,
        },
    } },
} };

static constexpr std::array<std::array<WoodenTrackSection, kNumOrthogonalDirections>, 4> kRightEighthBankToDiagImages = { {
    { {
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_0_0,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_0_0,
        },
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_0_1,
        },
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_0_2,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_0_2,
        },
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_0_3,
        },
    } },
    { {
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_1_0,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_1_0,
        },
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_1_1,
        },
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_1_2,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_1_2,
        },
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_1_3,
        },
    } },
    { {
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_2_0,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_2_0,
        },
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_2_1,
        },
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_2_2,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_2_2,
        },
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_2_3,
        },
    } },
    { {
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_3_0,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_3_0,
        },
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_3_1,
        },
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_3_2,
            kImageIndexUndefined,
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_FRONT_3_2,
        },
        {
            SPR_CLASSIC_WOODEN_RC_RIGHT_EIGHTH_BANK_TO_DIAG_3_3,
        },
    } },
} };

static void ClassicWoodenRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatToBank<true, kFlatToRightBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatToBank<true, kFlatToLeftBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatToBank<true, kLeftBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUpToBank<true, kUp25ToRightBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUpToBank<true, kUp25ToLeftBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackBankTo25DegUp<true, kRightBankToUp25Images>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackBankTo25DegUp<true, kLeftBankToUp25Images>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][7][2] = {
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_0,
                  0,
                  { 0, 2, 0 },
                  BoundBoxXYZ({ 0, 2, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_0,
                  0,
                  { 0, 2, 0 },
                  BoundBoxXYZ({ 0, 2, 27 }, { 32, 32, 0 }) },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_2,
                  0,
                  { 0, 16, 0 },
                  BoundBoxXYZ({ 0, 16, 0 }, { 32, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_2,
                  0,
                  { 0, 16, 0 },
                  BoundBoxXYZ({ 0, 16, 27 }, { 32, 16, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_3,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 16, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_3,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 27 }, { 16, 16, 0 }) },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_5,
                  0,
                  { 16, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 0 }, { 16, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_5,
                  0,
                  { 16, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 27 }, { 16, 32, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SW_SE_SEQ_6,
                  0,
                  { 2, 0, 0 },
                  BoundBoxXYZ({ 2, 0, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_SW_SE_SEQ_6,
                  0,
                  { 2, 0, 0 },
                  BoundBoxXYZ({ 2, 0, 27 }, { 32, 32, 0 }) },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_0,
                  0,
                  { 2, 0, 0 },
                  BoundBoxXYZ({ 2, 0, 0 }, { 32, 32, 2 }) },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_2,
                  0,
                  { 16, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 0 }, { 16, 32, 2 }) },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_3,
                  0,
                  { 0, 16, 0 },
                  BoundBoxXYZ({ 0, 16, 0 }, { 16, 16, 2 }) },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_5,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 32, 16, 2 }) },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NW_SW_SEQ_6,
                  0,
                  { 0, 2, 0 },
                  BoundBoxXYZ({ 0, 2, 0 }, { 32, 27, 2 }) },
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_0,
                  0,
                  { 0, 2, 0 },
                  BoundBoxXYZ({ 0, 2, 0 }, { 32, 27, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_0,
                  0,
                  { 0, 2, 0 },
                  BoundBoxXYZ({ 0, 2, 27 }, { 32, 27, 0 }) },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 32, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 27 }, { 32, 16, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_3,
                  0,
                  { 16, 16, 0 },
                  BoundBoxXYZ({ 16, 16, 0 }, { 16, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_3,
                  0,
                  { 16, 16, 0 },
                  BoundBoxXYZ({ 16, 16, 27 }, { 16, 16, 0 }) },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_5,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 16, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_5,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 27 }, { 16, 32, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_NE_NW_SEQ_6,
                  0,
                  { 2, 0, 0 },
                  BoundBoxXYZ({ 2, 0, 0 }, { 27, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_FRONT_NE_NW_SEQ_6,
                  0,
                  { 2, 0, 0 },
                  BoundBoxXYZ({ 2, 0, 27 }, { 27, 32, 0 }) },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_0,
                  0,
                  { 2, 0, 0 },
                  BoundBoxXYZ({ 2, 0, 0 }, { 27, 32, 2 }) },
                { 0, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 2, 27 }, { 32, 27, 0 }) },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 32, 16, 2 }) },
                { 0, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 0, 27 }, { 32, 16, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_3,
                  0,
                  { 16, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 0 }, { 16, 16, 2 }) },
                { 0, 0, { 0, 0, 0 }, BoundBoxXYZ({ 16, 16, 27 }, { 16, 16, 0 }) },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_5,
                  0,
                  { 0, 16, 0 },
                  BoundBoxXYZ({ 0, 16, 0 }, { 32, 16, 2 }) },
                { 0, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 0, 27 }, { 16, 32, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_5_SE_NE_SEQ_6,
                  0,
                  { 0, 2, 0 },
                  BoundBoxXYZ({ 0, 2, 0 }, { 32, 32, 2 }) },
                { 0, 0, { 0, 0, 0 }, BoundBoxXYZ({ 2, 0, 27 }, { 27, 32, 0 }) },
            },
        },
    };

    static constexpr int blockedSegments[7] = {
        kSegmentsAll,
        EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide),
        EnumsToFlags(
            PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
            PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
        EnumsToFlags(
            PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::centre,
            PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
        EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide),
        EnumsToFlags(
            PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
            PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
        kSegmentsAll,
    };

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    TrackPaintUtilRightQuarterTurn5TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    DrawSupportForSequenceA<TrackElemType::BankedRightQuarterTurn5Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void ClassicWoodenRCTrackBankedLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    ClassicWoodenRCTrackBankedRightQuarterTurn5(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][4][2] = {
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SW_SE_SEQ_0,
                  0,
                  { 0, 6, 0 },
                  BoundBoxXYZ({ 0, 6, 0 }, { 32, 20, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_SW_SE_SEQ_0,
                  0,
                  { 0, 6, 0 },
                  BoundBoxXYZ({ 0, 6, 27 }, { 32, 20, 0 }) },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SW_SE_SEQ_2,
                  0,
                  { 16, 16, 0 },
                  BoundBoxXYZ({ 16, 16, 0 }, { 16, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_SW_SE_SEQ_2,
                  0,
                  { 16, 16, 0 },
                  BoundBoxXYZ({ 16, 16, 27 }, { 16, 16, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SW_SE_SEQ_3,
                  0,
                  { 6, 0, 0 },
                  BoundBoxXYZ({ 6, 0, 0 }, { 20, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_SW_SE_SEQ_3,
                  0,
                  { 6, 0, 0 },
                  BoundBoxXYZ({ 6, 0, 27 }, { 20, 32, 0 }) },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NW_SW_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 6, 0, 0 }, { 20, 32, 2 }) },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NW_SW_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 0 }, { 16, 16, 2 }) },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NW_SW_SEQ_3,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 6, 0 }, { 32, 20, 2 }) },
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NE_NW_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 6, 0 }, { 32, 20, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_NE_NW_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 6, 27 }, { 32, 20, 0 }) },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NE_NW_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 16, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_NE_NW_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 27 }, { 16, 16, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_NE_NW_SEQ_3,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 6, 0, 0 }, { 20, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_FRONT_NE_NW_SEQ_3,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 6, 0, 27 }, { 20, 32, 0 }) },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SE_NE_SEQ_0,
                  0,
                  { 6, 0, 0 },
                  BoundBoxXYZ({ 6, 0, 0 }, { 20, 32, 2 }) },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SE_NE_SEQ_2,
                  0,
                  { 0, 16, 0 },
                  BoundBoxXYZ({ 0, 16, 0 }, { 16, 16, 2 }) },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_BANKED_QUARTER_TURN_3_SE_NE_SEQ_3,
                  0,
                  { 0, 6, 0 },
                  BoundBoxXYZ({ 0, 6, 0 }, { 32, 20, 2 }) },
                {},
            },
        },
    };

    static constexpr int blockedSegments[4] = {
        kSegmentsAll,
        0,
        EnumsToFlags(PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide),
        EnumsToFlags(
            PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::centre,
            PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
    };

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    DrawSupportForSequenceA<TrackElemType::RightBankedQuarterTurn3Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void ClassicWoodenRCTrackLeftQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    ClassicWoodenRCTrackRightQuarterTurn3Bank(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiagLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagFlatToBank<true, kDiagFlatToRightBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiagRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagFlatToBank<true, kDiagFlatToLeftBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiagDown25ToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagBankTo25DegUp<true, kDiagRightBankTo25DegUpImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiagDown25ToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagBankTo25DegUp<true, kDiagLeftBankTo25DegUpImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiagLeftBankToDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagUp25ToBank<true, kDiagUp25ToRightBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiagRightBankToDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagUp25ToBank<true, kDiagUp25ToLeftBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    WoodenRCTrackRightEighthBankToDiag<true, kRightEighthBankToDiagImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}
static void ClassicWoodenRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    WoodenRCTrackLeftEighthBankToDiag<true, kLeftEighthBankToDiagImages>(
        session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiagRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return WoodenRCTrackDiagLeftBank<true, kDiagLeftBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

// Stylistically, this coaster is _very_ similar to the regular Wooden Roller Coaster.
// The only difference is to which parts the colours are applied, and the degree of the banking.
// As such, all non-banked pieces are simply drawn as regular wooden roller coaster pieces with a different paint scheme.
TrackPaintFunction GetTrackPaintFunctionClassicWoodenRC(OpenRCT2::TrackElemType trackType)
{
    if (!IsCsgLoaded())
    {
        return GetTrackPaintFunctionClassicWoodenRCFallback(trackType);
    }

    switch (trackType)
    {
        case TrackElemType::FlatToLeftBank:
            return WoodenRCTrackFlatToBank<true, kFlatToLeftBankImages>;
        case TrackElemType::FlatToRightBank:
            return WoodenRCTrackFlatToBank<true, kFlatToRightBankImages>;
        case TrackElemType::LeftBankToFlat:
            return ClassicWoodenRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return ClassicWoodenRCTrackRightBankToFlat;
        case TrackElemType::LeftBank:
            return WoodenRCTrackFlatToBank<true, kLeftBankImages>;
        case TrackElemType::RightBank:
            return ClassicWoodenRCTrackRightBank;
        case TrackElemType::Up25ToLeftBank:
            return WoodenRCTrack25DegUpToBank<true, kUp25ToLeftBankImages>;
        case TrackElemType::Up25ToRightBank:
            return WoodenRCTrack25DegUpToBank<true, kUp25ToRightBankImages>;
        case TrackElemType::LeftBankToDown25:
            return ClassicWoodenRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return ClassicWoodenRCTrackRightBankTo25DegDown;
        case TrackElemType::LeftBankToUp25:
            return WoodenRCTrackBankTo25DegUp<true, kLeftBankToUp25Images>;
        case TrackElemType::RightBankToUp25:
            return WoodenRCTrackBankTo25DegUp<true, kRightBankToUp25Images>;
        case TrackElemType::Down25ToLeftBank:
            return ClassicWoodenRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return ClassicWoodenRCTrack25DegDownToRightBank;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return ClassicWoodenRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return ClassicWoodenRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return ClassicWoodenRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return ClassicWoodenRCTrackRightQuarterTurn3Bank;
        case TrackElemType::DiagFlatToLeftBank:
            return WoodenRCTrackDiagFlatToBank<true, kDiagFlatToLeftBankImages>;
        case TrackElemType::DiagFlatToRightBank:
            return WoodenRCTrackDiagFlatToBank<true, kDiagFlatToRightBankImages>;
        case TrackElemType::DiagLeftBankToFlat:
            return ClassicWoodenRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return ClassicWoodenRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return WoodenRCTrackDiagBankTo25DegUp<true, kDiagLeftBankTo25DegUpImages>;
        case TrackElemType::DiagRightBankToUp25:
            return WoodenRCTrackDiagBankTo25DegUp<true, kDiagRightBankTo25DegUpImages>;
        case TrackElemType::DiagDown25ToLeftBank:
            return ClassicWoodenRCTrackDiagDown25ToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return ClassicWoodenRCTrackDiagDown25ToRightBank;
        case TrackElemType::DiagUp25ToLeftBank:
            return WoodenRCTrackDiagUp25ToBank<true, kDiagUp25ToLeftBankImages>;
        case TrackElemType::DiagUp25ToRightBank:
            return WoodenRCTrackDiagUp25ToBank<true, kDiagUp25ToRightBankImages>;
        case TrackElemType::DiagLeftBankToDown25:
            return ClassicWoodenRCTrackDiagLeftBankToDown25;
        case TrackElemType::DiagRightBankToDown25:
            return ClassicWoodenRCTrackDiagRightBankToDown25;
        case TrackElemType::DiagLeftBank:
            return WoodenRCTrackDiagLeftBank<true, kDiagLeftBankImages>;
        case TrackElemType::DiagRightBank:
            return ClassicWoodenRCTrackDiagRightBank;
        case TrackElemType::LeftEighthBankToDiag:
            return WoodenRCTrackLeftEighthBankToDiag<true, kLeftEighthBankToDiagImages>;
        case TrackElemType::RightEighthBankToDiag:
            return WoodenRCTrackRightEighthBankToDiag<true, kRightEighthBankToDiagImages>;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return ClassicWoodenRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return ClassicWoodenRCTrackRightEighthBankToOrthogonal;
        default:
            return GetTrackPaintFunctionClassicWoodenRCFallback(trackType);
    }
}
