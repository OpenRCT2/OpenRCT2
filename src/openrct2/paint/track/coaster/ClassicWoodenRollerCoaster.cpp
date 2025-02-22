/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../drawing/Drawing.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/TrackStyleSupports.h"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
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

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
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

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
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

// clang-format off
const TrackElementMetalSupports kClassicWoodenRollerCoasterMetalSupports[EnumValue(TrackElemType::Count)] = {
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Flat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // EndStation
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BeginStation
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MiddleStation
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60ToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60ToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn5Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn5Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BankedLeftQuarterTurn5Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BankedRightQuarterTurn5Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn5TilesUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn5TilesUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn5TilesDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn5TilesDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // SBendLeft
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // SBendRight
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 8, 8, 8, 8 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 20, 9, 16, 16 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 16, 16, 20, 9 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 8, 8, 8, 8 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftVerticalLoop
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 8, 8, 8, 8 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 16, 16, 9, 20 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 9, 20, 16, 16 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 8, 8, 8, 8 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightVerticalLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedQuarterTurn3Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedQuarterTurn3Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3TilesUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3TilesUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3TilesDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3TilesDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn1Tile
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn1Tile
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftTwistDownToUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightTwistDownToUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftTwistUpToDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightTwistUpToDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // HalfLoopUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // HalfLoopDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftCorkscrewUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightCorkscrewUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftCorkscrewDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightCorkscrewDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // TowerBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // TowerSection
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToUp25Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToUp60Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60ToUp25Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToFlatCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToDown25Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToDown60Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60ToDown25Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToFlatCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn5TilesCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn5TilesCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // SBendLeftCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // SBendRightCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3TilesCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3TilesCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftHalfBankedHelixUpSmall
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightHalfBankedHelixUpSmall
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftHalfBankedHelixDownSmall
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightHalfBankedHelixDownSmall
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftHalfBankedHelixUpLarge
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightHalfBankedHelixUpLarge
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftHalfBankedHelixDownLarge
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightHalfBankedHelixDownLarge
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn1TileUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn1TileUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn1TileDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn1TileDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Brakes
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Booster
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Maze
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterBankedHelixLargeUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterBankedHelixLargeUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterBankedHelixLargeDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterBankedHelixLargeDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterHelixLargeUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterHelixLargeUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterHelixLargeDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterHelixLargeDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25LeftBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25RightBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Waterfall
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Rapids
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // OnRidePhoto
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25LeftBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25RightBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Watersplash
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToUp60LongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60ToFlatLongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Whirlpool
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60ToFlatLongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToDown60LongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // CableLiftHill
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // ReverseFreefallSlope
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // ReverseFreefallVertical
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60ToUp90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down90ToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up90ToUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60ToDown90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BrakeForDrop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToDiag
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToDiag
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToOrthogonal
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToOrthogonal
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToDiag
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToDiag
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToOrthogonal
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToOrthogonal
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp60ToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown60ToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp60ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown60ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LogFlumeReverser
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // SpinningTunnel
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBarrelRollUpToDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBarrelRollUpToDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBarrelRollDownToUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBarrelRollDownToUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankToLeftQuarterTurn3TilesUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankToRightQuarterTurn3TilesUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3TilesDown25ToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3TilesDown25ToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // PoweredLift
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 5, 5, 5, 5 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 12, 6, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::TopLeftSide, { 0, 8, 21, 0 }, { 22, 14, 0, 22 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftLargeHalfLoopUp
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 5, 5, 5, 5 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 0, 0, 6, 12 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::BottomRightSide, { 0, 21, 8, 0 }, { 22, 0, 14, 22 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightLargeHalfLoopUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::BottomRightSide, { 0, 21, 8, 0 }, { 22, 0, 14, 22 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 0, 0, 6, 12 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 5, 5, 5, 5 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftLargeHalfLoopDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::TopLeftSide, { 0, 8, 21, 0 }, { 22, 14, 0, 22 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 12, 6, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 5, 5, 5, 5 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightLargeHalfLoopDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerTwistUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerTwistUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerTwistDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerTwistDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlyerHalfLoopUninvertedUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlyerHalfLoopInvertedDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerCorkscrewUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerCorkscrewUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerCorkscrewDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerCorkscrewDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // HeartLineTransferUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // HeartLineTransferDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftHeartLineRoll
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightHeartLineRoll
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MinigolfHoleA
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MinigolfHoleB
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MinigolfHoleC
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MinigolfHoleD
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MinigolfHoleE
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MultiDimInvertedFlatToDown90QuarterLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up90ToInvertedFlatQuarterLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // InvertedFlatToDown90QuarterLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftCurvedLiftHill
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightCurvedLiftHill
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftReverser
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightReverser
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // AirThrustTopCap
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // AirThrustVerticalDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // AirThrustVerticalDownToLevel
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BlockBrakes
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedQuarterTurn3TileUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedQuarterTurn3TileUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedQuarterTurn3TileDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedQuarterTurn3TileDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedQuarterTurn5TileUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedQuarterTurn5TileUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedQuarterTurn5TileDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedQuarterTurn5TileDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToLeftBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToRightBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedUp25ToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedUp25ToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToLeftBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToRightBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedDown25ToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedDown25ToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedFlatToLeftBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedFlatToRightBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedUp25ToLeftBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedUp25ToRightBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedFlatToLeftBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedFlatToRightBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedDown25ToLeftBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedDown25ToRightBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToLeftBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToRightBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedUp25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedUp25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToLeftBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToRightBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedDown25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedDown25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn1TileUp90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn1TileUp90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn1TileDown90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn1TileDown90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MultiDimUp90ToInvertedFlatQuarterLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MultiDimFlatToDown90QuarterLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MultiDimInvertedUp90ToFlatQuarterLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RotationControlToggle
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack1x4A
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack2x2
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack4x4
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack2x4
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack1x5
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack1x1A
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack1x4B
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack1x1B
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack1x4C
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack3x3
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftLargeCorkscrewUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightLargeCorkscrewUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftLargeCorkscrewDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightLargeCorkscrewDown
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 7, 7, 7, 7 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::typeB), MetalSupportPlace::TopLeftSide, { 0, 0, 0, 0 }, { 14, 16, 18, 14 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftMediumHalfLoopUp
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 7, 7, 7, 7 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::typeB), MetalSupportPlace::BottomRightSide, { 0, 0, 0, 0 }, { 14, 18, 16, 14 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightMediumHalfLoopUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::typeB), MetalSupportPlace::BottomRightSide, { 0, 0, 0, 0 }, { 14, 18, 16, 14 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 7, 7, 7, 7 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftMediumHalfLoopDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::typeB), MetalSupportPlace::TopLeftSide, { 0, 0, 0, 0 }, { 14, 16, 18, 14 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::drawCap), MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 7, 7, 7, 7 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightMediumHalfLoopDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftZeroGRollUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightZeroGRollUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftZeroGRollDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightZeroGRollDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftLargeZeroGRollUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightLargeZeroGRollUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftLargeZeroGRollDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightLargeZeroGRollDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerLargeHalfLoopUninvertedUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerLargeHalfLoopUninvertedUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerLargeHalfLoopInvertedDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerLargeHalfLoopInvertedDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerLargeHalfLoopInvertedUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerLargeHalfLoopInvertedUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerLargeHalfLoopUninvertedDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerLargeHalfLoopUninvertedDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlyerHalfLoopInvertedUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlyerHalfLoopUninvertedDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToDiagUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToDiagUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToDiagDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToDiagDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToOrthogonalUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToOrthogonalUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToOrthogonalDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToOrthogonalDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToLeftBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToRightBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedUp25ToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedUp25ToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToLeftBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToRightBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedDown25ToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedDown25ToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedFlatToLeftBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedFlatToRightBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedUp25ToLeftBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedUp25ToRightBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedFlatToLeftBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedFlatToRightBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedDown25ToLeftBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedDown25ToRightBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToLeftBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToRightBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedUp25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedUp25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToLeftBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToRightBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedDown25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedDown25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25LeftBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25RightBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25LeftBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25RightBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToDiagUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToDiagUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToDiagDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToDiagDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToOrthogonalUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToOrthogonalUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToOrthogonalDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToOrthogonalDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagBrakes
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagBlockBrakes
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25Brakes
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagBooster
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToUp60LongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp60ToFlatLongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToDown60LongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown60ToFlatLongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthDiveLoopUpToOrthogonal
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthDiveLoopUpToOrthogonal
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthDiveLoopDownToDiag
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthDiveLoopDownToDiag
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25Brakes
};
// clang-format on
