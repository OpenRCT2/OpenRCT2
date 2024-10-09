/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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

// static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

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
    const TrackElement& trackElement, SupportType supportType)

{
    static constexpr SpriteBoundBox2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_SW_NE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_NW_SE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_NW_SE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 26, 0, 5 }, { 1, 32, 9 }) },
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_NE_SW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_SE_NW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_SE_NW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 26, 0, 5 }, { 1, 32, 9 }) },
        } }
    };
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    DrawSupportForSequenceA<TrackElemType::FlatToLeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void ClassicWoodenRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_SW_NE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_NW_SE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
            {},
        } },

        { {
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_NE_SW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            { SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_FRONT_NE_SW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 26, 5 }, { 32, 1, 9 }) },
        } },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_SE_NW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
                {},
            },
        }
    };
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    DrawSupportForSequenceA<TrackElemType::FlatToRightBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void ClassicWoodenRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrackFlatToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrackFlatToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_SW_NE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_NW_SE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
            {},
        } },

        { {
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_NE_SW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            {},
        } },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_SE_NW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
                {},
            },
        }
    };
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    DrawSupportForSequenceA<TrackElemType::LeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void ClassicWoodenRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_SW_NE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_NW_SE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_FRONT_NW_SE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 26, 0, 5 }, { 1, 32, 9 }) },
        } },

        { {
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_NE_SW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_FRONT_NE_SW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 26, 5 }, { 32, 1, 9 }) },
        } },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_25_UP_SE_NW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
                {},
            },
        }
    };
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    DrawSupportForSequenceA<TrackElemType::LeftBankToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void ClassicWoodenRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_SW_NE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_NW_SE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
            { SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_FRONT_NW_SE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 26, 0, 5 }, { 1, 32, 9 }) },
        } },

        { {
            { SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_NE_SW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            { SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_FRONT_NE_SW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 26, 5 }, { 32, 1, 9 }) },
        } },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_25_UP_SE_NW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
                {},
            },
        }
    };

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    DrawSupportForSequenceA<TrackElemType::RightBankToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void ClassicWoodenRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_SW_NE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_NW_SE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_FRONT_NW_SE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 26, 0, 5 }, { 1, 32, 9 }) },
        } },

        { {
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_NE_SW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_FRONT_NE_SW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 26, 5 }, { 32, 1, 9 }) },
        } },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_25_UP_TO_LEFT_BANK_SE_NW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
                {},
            },
        }
    };

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    DrawSupportForSequenceA<TrackElemType::Up25ToLeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

static void ClassicWoodenRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][1][2] = {
        { {
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_SW_NE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            {},
        } },
        { {
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_NW_SE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_FRONT_NW_SE, 0, { 0, 0, 0 }, BoundBoxXYZ({ 26, 0, 5 }, { 1, 32, 9 }) },
        } },

        { {
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_NE_SW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 3, 0 }, { 32, 25, 2 }) },
            { SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_FRONT_NE_SW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 0, 26, 5 }, { 32, 1, 9 }) },
        } },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_25_UP_TO_RIGHT_BANK_SE_NW, 0, { 0, 0, 0 }, BoundBoxXYZ({ 3, 0, 0 }, { 25, 32, 2 }) },
                {},
            },
        }
    };

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    DrawSupportForSequenceA<TrackElemType::Up25ToRightBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

static void ClassicWoodenRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrack25DegUpToRightBank(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrackRightBankTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
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

static void ClassicWoodenRCTrackLeftEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][5][2] = {
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_0_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 32, 32, 2 }) },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_0_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 32, 16, 2 }) },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_0_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 16, 0 }, { 16, 16, 2 }) },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_0_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 16, 0 }, { 16, 16, 2 }) },
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_1_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_1_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 27 }, { 32, 32, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_1_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 16, 34, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_1_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 27 }, { 16, 32, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_1_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 16, 0 }, { 16, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_1_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 16, 27 }, { 16, 16, 0 }) },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_1_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 0 }, { 18, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_1_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 27 }, { 16, 16, 0 }) },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_2_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 32, 32, 2 }) },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_2_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 16, 0 }, { 32, 16, 2 }) },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_2_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 0 }, { 16, 16, 2 }) },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_2_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 16, 16, 2 }) },
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_3_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_3_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 27 }, { 32, 32, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_3_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 0 }, { 16, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_3_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 27 }, { 16, 32, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_3_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 16, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_3_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 27 }, { 16, 16, 0 }) },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_3_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 16, 0 }, { 16, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_LEFT_BANKED_FRONT_3_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 16, 27 }, { 16, 16, 0 }) },
            },
        },
    };

    static constexpr int blockedSegments[5] = {
        kSegmentsAll, kSegmentsAll, kSegmentsAll, kSegmentsAll, kSegmentsAll,
    };

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportForSequenceA<TrackElemType::LeftEighthBankToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void ClassicWoodenRCTrackRightEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][5][2] = {
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_0_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_0_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 27 }, { 32, 32, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_0_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 16, 0 }, { 32, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_0_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 16, 27 }, { 32, 16, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_0_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 16, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_0_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 27 }, { 16, 16, 0 }) },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_0_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 0 }, { 16, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_0_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 27 }, { 16, 16, 0 }) },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_1_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 32, 32, 2 }) },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_1_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 0 }, { 16, 32, 0 }) },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_1_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 16, 0 }, { 16, 16, 2 }) },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_1_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 16, 16, 2 }) },
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_2_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_2_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 27 }, { 32, 32, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_2_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 34, 16, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_2_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 27 }, { 32, 16, 0 }) },
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_2_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 4, 4, 0 }, { 28, 28, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_2_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 4, 4, 27 }, { 28, 28, 0 }) },
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_2_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 16, 0 }, { 16, 18, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_FRONT_2_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 16, 27 }, { 16, 16, 0 }) },
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_3_SEQ_0,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 32, 32, 2 }) },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_3_SEQ_1,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 0, 0, 0 }, { 16, 32, 0 }) },
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_3_SEQ_2,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 0, 0 }, { 16, 16, 2 }) },
                {},
            },
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_ORTHO_TO_DIAG_RIGHT_BANKED_3_SEQ_4,
                  0,
                  { 0, 0, 0 },
                  BoundBoxXYZ({ 16, 16, 0 }, { 16, 16, 2 }) },
                {},
            },
        },
    };

    static constexpr int blockedSegments[5] = {
        kSegmentsAll, kSegmentsAll, kSegmentsAll, kSegmentsAll, kSegmentsAll,
    };

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportForSequenceA<TrackElemType::RightEighthBankToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void ClassicWoodenRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    ClassicWoodenRCTrackRightEighthBankToDiag(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    ClassicWoodenRCTrackLeftEighthBankToDiag(
        session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiagFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_FRONT_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 27 }, { 32, 32, 0 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_1,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 23, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_FRONT_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 27 }, { 32, 32, 0 }) },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_LEFT_3,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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

    DrawSupportForSequenceA<TrackElemType::DiagFlatToLeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void ClassicWoodenRCTrackDiagFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_FRONT_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 27 }, { 32, 32, 0 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_1,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 23, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_FRONT_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 27 }, { 32, 32, 0 }) },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_FLAT_TO_BANK_RIGHT_3,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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

    DrawSupportForSequenceA<TrackElemType::DiagFlatToRightBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void ClassicWoodenRCTrackDiagLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrackDiagFlatToRightBank(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiagRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrackDiagFlatToLeftBank(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiagLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_0, 0, { -16, -16, 0 }, BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_FRONT_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 27 }, { 32, 32, 0 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_1, 0, { -16, -16, 0 }, BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_2, 0, { -16, -16, 0 }, BoundBoxXYZ({ -16, -16, 0 }, { 32, 23, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_FRONT_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 27 }, { 32, 32, 0 }) },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_3, 0, { -16, -16, 0 }, BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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

    DrawSupportForSequenceA<TrackElemType::DiagLeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void ClassicWoodenRCTrackDiagRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrackDiagLeftBank(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiagLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_FRONT_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 35 }, { 32, 32, 0 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_1,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 23, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_FRONT_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 35 }, { 32, 32, 0 }) },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_LEFT_BANK_TO_25_UP_3,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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

    DrawSupportForSequenceA<TrackElemType::DiagLeftBankToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void ClassicWoodenRCTrackDiagRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_FRONT_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 35 }, { 32, 32, 0 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_1,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 23, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_FRONT_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 35 }, { 32, 32, 0 }) },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_RIGHT_BANK_TO_25_UP_3,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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

    DrawSupportForSequenceA<TrackElemType::DiagRightBankToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void ClassicWoodenRCTrackDiag25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_FRONT_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 35 }, { 32, 32, 0 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_1,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 23, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_FRONT_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 35 }, { 32, 32, 0 }) },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_LEFT_BANK_3,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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

    DrawSupportForSequenceA<TrackElemType::DiagUp25ToLeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void ClassicWoodenRCTrackDiag25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr SpriteBoundBox2 imageIds[4][4][2] = {
        {
            {
                {},
                {},
            },
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_FRONT_0,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 35 }, { 32, 32, 0 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_1,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 23, 2 }) },
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_FRONT_2,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 35 }, { 32, 32, 0 }) },
            },
            {
                {},
                {},
            },
        },
        {
            {
                { SPR_CLASSIC_WOODEN_RC_DIAG_25_UP_TO_RIGHT_BANK_3,
                  0,
                  { -16, -16, 0 },
                  BoundBoxXYZ({ -16, -16, 0 }, { 32, 32, 2 }) },
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

    DrawSupportForSequenceA<TrackElemType::DiagUp25ToRightBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][0], height);
    WoodenRCTrackPaintBb<true>(session, &imageIds[direction][trackSequence][1], height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void ClassicWoodenRCTrackDiagLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrackDiag25DegUpToRightBank(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiagRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrackDiag25DegUpToLeftBank(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiag25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrackDiagRightBankTo25DegUp(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void ClassicWoodenRCTrackDiag25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ClassicWoodenRCTrackDiagLeftBankTo25DegUp(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

// Stylistically, this coaster is _very_ similar to the regular Wooden Roller Coaster.
// The only difference is to which parts the colours are applied, and the degree of the banking.
// As such, all non-banked pieces are simply drawn as regular wooden roller coaster pieces with a different paint scheme.
TRACK_PAINT_FUNCTION GetTrackPaintFunctionClassicWoodenRC(OpenRCT2::TrackElemType trackType)
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
        default:
            return GetTrackPaintFunctionClassicWoodenRCFallback(trackType);
    }
}
