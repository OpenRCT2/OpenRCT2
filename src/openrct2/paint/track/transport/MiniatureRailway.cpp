/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/PathElement.h"
#include "../../../world/tile_element/TileElement.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/TrackStyleSupports.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

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

static constexpr uint32_t miniature_railway_track_pieces_flat[4] = {
    SPR_MINIATURE_RAILWAY_FLAT_SW_NE,
    SPR_MINIATURE_RAILWAY_FLAT_NW_SE,
    SPR_MINIATURE_RAILWAY_FLAT_SW_NE,
    SPR_MINIATURE_RAILWAY_FLAT_NW_SE,
};

static constexpr uint32_t miniature_railway_track_pieces_flat_station[4] = {
    SPR_MINIATURE_RAILWAY_FLAT_NO_BASE_SW_NE,
    SPR_MINIATURE_RAILWAY_FLAT_NO_BASE_NW_SE,
    SPR_MINIATURE_RAILWAY_FLAT_NO_BASE_SW_NE,
    SPR_MINIATURE_RAILWAY_FLAT_NO_BASE_NW_SE,
};

static constexpr uint32_t miniature_railway_track_pieces_flat_quarter_turn_5_tiles[4][5] = {
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
    },
};

static constexpr uint32_t miniature_railway_track_pieces_25_deg_up[4] = {
    SPR_MINIATURE_RAILWAY_25_DEG_UP_SW_NE,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_NW_SE,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_NE_SW,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_SE_NW,
};

static constexpr uint32_t miniature_railway_track_pieces_flat_to_25_deg_up[4] = {
    SPR_MINIATURE_RAILWAY_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_MINIATURE_RAILWAY_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_MINIATURE_RAILWAY_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_MINIATURE_RAILWAY_FLAT_TO_25_DEG_UP_SE_NW,
};

static constexpr uint32_t miniature_railway_track_pieces_25_deg_up_to_flat[4] = {
    SPR_MINIATURE_RAILWAY_25_DEG_UP_TO_FLAT_SW_NE,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_TO_FLAT_NW_SE,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_TO_FLAT_NE_SW,
    SPR_MINIATURE_RAILWAY_25_DEG_UP_TO_FLAT_SE_NW,
};

static constexpr uint32_t miniature_railway_track_pieces_s_bend_left[2][4] = {
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
    },
};

static constexpr uint32_t miniature_railway_track_pieces_s_bend_right[2][4] = {
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
    },
};

static constexpr uint32_t miniature_railway_track_pieces_flat_quarter_turn_3_tiles[4][3] = {
    {
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_2,
    },
    {
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NW_SW_PART_2,
    },
    {
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_NE_NW_PART_2,
    },
    {
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SE_NE_PART_2,
    },
};

static constexpr uint32_t miniature_railway_track_pieces_right_eight_to_diag[4][4] = {
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

static constexpr CoordsXYZ miniature_railway_track_pieces_right_eight_to_diag_bounds[4][4] = {
    {
        { 32, 32, 2 },
        { 32, 16, 2 },
        { 16, 16, 2 },
        { 32, 32, 0 },
    },
    {
        { 32, 32, 2 },
        { 16, 32, 2 },
        { 16, 16, 2 },
        { 16, 16, 2 },
    },
    {
        { 32, 32, 2 },
        { 34, 16, 2 },
        { 16, 16, 2 },
        { 32, 34, 0 },
    },
    {
        { 32, 32, 2 },
        { 16, 32, 2 },
        { 16, 16, 2 },
        { 30, 30, 0 },
    },
};

static constexpr CoordsXY miniature_railway_track_pieces_right_eight_to_diag_offset[4][4] = {
    {
        { 0, 0 },
        { 0, 16 },
        { 0, 0 },
        { 0, 0 },
    },
    {
        { 0, 0 },
        { 16, 0 },
        { 0, 16 },
        { 0, 0 },
    },
    {
        { 0, 0 },
        { 0, 0 },
        { 16, 16 },
        { 0, 0 },
    },
    {
        { 0, 0 },
        { 0, 0 },
        { 16, 0 },
        { 0, 0 },
    },
};

static constexpr uint32_t miniature_railway_track_pieces_left_eight_to_diag[4][4] = {
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

static constexpr CoordsXYZ miniature_railway_track_pieces_left_eight_to_diag_bounds[4][4] = {
    {
        { 32, 32, 2 },
        { 32, 16, 2 },
        { 16, 16, 2 },
        { 30, 30, 0 },
    },
    {
        { 32, 32, 2 },
        { 16, 34, 2 },
        { 14, 14, 2 },
        { 34, 32, 0 },
    },
    {
        { 32, 32, 2 },
        { 32, 16, 2 },
        { 16, 16, 2 },
        { 16, 16, 2 },
    },
    {
        { 32, 32, 2 },
        { 16, 32, 2 },
        { 16, 16, 2 },
        { 32, 32, 0 },
    },
};

static constexpr CoordsXY miniature_railway_track_pieces_left_eight_to_diag_offset[4][4] = {
    {
        { 0, 0 },
        { 0, 0 },
        { 0, 16 },
        { 0, 0 },
    },
    {
        { 0, 0 },
        { 0, 0 },
        { 16, 16 },
        { 0, 0 },
    },
    {
        { 0, 0 },
        { 0, 16 },
        { 16, 0 },
        { 0, 0 },
    },
    {
        { 0, 0 },
        { 16, 0 },
        { 0, 0 },
        { 0, 0 },
    },
};

static constexpr CoordsXYZ miniature_railway_track_pieces_right_eight_to_orthog_bounds[4][4] = {
    {
        { 32, 32, 2 },
        { 32, 16, 2 },
        { 16, 16, 2 },
        { 14, 14, 2 },
    },
    {
        { 32, 32, 2 },
        { 16, 34, 2 },
        { 14, 14, 2 },
        { 18, 16, 2 },
    },
    {
        { 32, 32, 2 },
        { 32, 16, 2 },
        { 16, 16, 2 },
        { 16, 16, 2 },
    },
    {
        { 32, 32, 2 },
        { 16, 32, 2 },
        { 16, 16, 2 },
        { 16, 16, 2 },
    },
};

static constexpr CoordsXY miniature_railway_track_pieces_right_eight_to_orthog_offset[4][4] = {
    {
        { 0, 0 },
        { 0, 0 },
        { 0, 16 },
        { 16, 16 },
    },
    {
        { 0, 0 },
        { 0, 0 },
        { 16, 16 },
        { 16, 0 },
    },
    {
        { 0, 0 },
        { 0, 16 },
        { 16, 0 },
        { 0, 0 },
    },
    {
        { 0, 0 },
        { 16, 0 },
        { 0, 0 },
        { 0, 16 },
    },
};

static constexpr CoordsXYZ miniature_railway_track_pieces_left_eight_to_orthog_bounds[4][4] = {
    {
        { 32, 32, 2 },
        { 32, 16, 2 },
        { 16, 16, 2 },
        { 16, 16, 2 },
    },
    {
        { 32, 32, 2 },
        { 16, 32, 2 },
        { 16, 16, 2 },
        { 16, 16, 2 },
    },
    {
        { 32, 32, 2 },
        { 34, 16, 2 },
        { 16, 16, 2 },
        { 16, 18, 2 },
    },
    {
        { 32, 32, 2 },
        { 16, 32, 2 },
        { 16, 16, 2 },
        { 14, 14, 2 },
    },
};

static constexpr CoordsXY miniature_railway_track_pieces_left_eight_to_orthog_offset[4][4] = {
    {
        { 0, 0 },
        { 0, 16 },
        { 0, 0 },
        { 16, 0 },
    },
    {
        { 0, 0 },
        { 16, 0 },
        { 0, 16 },
        { 0, 0 },
    },
    {
        { 0, 0 },
        { 0, 0 },
        { 16, 16 },
        { 0, 16 },
    },
    {
        { 0, 0 },
        { 0, 0 },
        { 16, 0 },
        { 16, 16 },
    },
};

static constexpr uint32_t miniature_railway_track_pieces_diag_flat[4] = {
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_W_E,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_N_S,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_E_W,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_S_N,
};

static constexpr uint32_t miniature_railway_track_pieces_diag_flat_to_25_deg_up[4] = {
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_W_E,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_N_S,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_E_W,
    SPR_MINIATURE_RAILWAY_DIAG_FLAT_TO_25_DEG_UP_S_N,
};

static constexpr uint32_t miniature_railway_track_pieces_diag_25_deg_up_to_flat[4] = {
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_W_E,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_N_S,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_E_W,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_TO_FLAT_S_N,
};

static constexpr uint32_t miniature_railway_track_pieces_diag_25_deg_up[4] = {
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_W_E,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_N_S,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_E_W,
    SPR_MINIATURE_RAILWAY_DIAG_25_DEG_UP_S_N,
};

static ImageId MiniatureRailwayTrackToGravel(ImageId imageId)
{
    return ImageId(imageId.GetIndex() - SPR_MINIATURE_RAILWAY_FLAT_SW_NE + SPR_G2_MINIATURE_RAILWAY_GRAVEL_SW_NE);
}

static ImageId MiniatureRailwayTrackToGrooved(ImageId imageId)
{
    return ImageId(imageId.GetIndex() - SPR_MINIATURE_RAILWAY_FLAT_SW_NE + SPR_G2_MINIATURE_RAILWAY_GROOVED_SW_NE);
}

static ImageId MiniatureRailwayTrackToGroovedIndent(const TileElement* path, uint8_t direction, uint8_t rotation)
{
    if (path == nullptr)
    {
        return ImageId(0);
    }

    uint32_t imageIdAlt = SPR_G2_MINIATURE_RAILWAY_GROOVED_SW_NE;

    uint8_t correctedEdges = path->AsPath()->GetEdges();
    correctedEdges |= correctedEdges << 4;
    correctedEdges >>= 4 - rotation;
    correctedEdges &= 0x0F;

    if (direction & 0x1)
    {
        static constexpr uint32_t imageIds[2][2] = {
            {
                SPR_G2_MINIATURE_RAILWAY_INSET_NW_SE,
                SPR_G2_MINIATURE_RAILWAY_INSET_END_NW,
            },
            {
                SPR_G2_MINIATURE_RAILWAY_INSET_END_SE,
                SPR_G2_MINIATURE_RAILWAY_INSET_END_NW_SE,
            },
        };
        imageIdAlt = imageIds[(correctedEdges & 0x2) ? 0 : 1][(correctedEdges & 0x8) ? 0 : 1];
    }
    else
    {
        static constexpr uint32_t imageIds[2][2] = {
            {
                SPR_G2_MINIATURE_RAILWAY_INSET_SW_NE,
                SPR_G2_MINIATURE_RAILWAY_INSET_END_SW,
            },
            {
                SPR_G2_MINIATURE_RAILWAY_INSET_END_NE,
                SPR_G2_MINIATURE_RAILWAY_INSET_END_SW_NE,
            },
        };
        imageIdAlt = imageIds[(correctedEdges & 0x1) ? 0 : 1][(correctedEdges & 0x4) ? 0 : 1];
    }

    return ImageId(imageIdAlt);
}

/** rct2: 0x008AD0C0 */
static void PaintMiniatureRailwayTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool paintAsGravel = false;
    bool paintGrooved = false;

    if (session.PathElementOnSameHeight != nullptr)
    {
        paintAsGravel = true;
        paintGrooved = true;
    }

    ImageId imageId, imageIdAlt;

    // In the following 3 calls to PaintAddImageAsParentRotated/PaintAddImageAsChildRotated, we add 1 to the
    //  bound_box_offset_z argument to make straight tracks draw above footpaths
    imageId = session.TrackColours.WithIndex(miniature_railway_track_pieces_flat[direction]);
    if (!paintAsGravel)
    {
        PaintAddImageAsParentRotated(session, direction, imageId, { 0, 6, height }, { { 0, 6, height }, { 32, 20, 2 } });
    }
    else
    {
        imageIdAlt = MiniatureRailwayTrackToGravel(imageId);
        PaintAddImageAsParentRotated(session, direction, imageIdAlt, { 0, 6, height }, { { 0, 6, height }, { 32, 20, 2 } });
    }
    if (paintGrooved)
    {
        imageIdAlt = MiniatureRailwayTrackToGrooved(imageId);
        PaintAddImageAsParentRotated(session, direction, imageIdAlt, { 0, 6, height }, { { 0, 6, height + 2 }, { 32, 20, 2 } });
        imageIdAlt = MiniatureRailwayTrackToGroovedIndent(session.PathElementOnSameHeight, direction, session.CurrentRotation);
        PaintAddImageAsParentRotated(
            session, direction, imageIdAlt.WithTransparency(FilterPaletteID::PaletteDarken2), { 0, 6, height },
            { { 0, 6, height + 2 }, { 32, 20, 2 } });
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD170, 0x008AD180, 0x008AD190 */
static void PaintMiniatureRailwayStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const ImageId imageId = session.TrackColours.WithIndex(miniature_railway_track_pieces_flat_station[direction]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 6, height }, { { 0, 6, height + 1 }, { 32, 20, 1 } });

    TrackPaintUtilDrawStationTunnel(session, direction, height);

    TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, StationBaseType::a, -2, 7, 9);

    PaintUtilSetGeneralSupportHeight(session, height + 30);
}

/** rct2: 0x008AD0D0 */
static void PaintMiniatureRailwayTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(miniature_railway_track_pieces_25_deg_up[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 2, height }, { { 0, 3, height }, { 32, 25, 2 } });

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AD0E0 */
static void PaintMiniatureRailwayTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(miniature_railway_track_pieces_flat_to_25_deg_up[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 2, height }, { { 0, 3, height }, { 32, 25, 2 } });

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AD0F0 */
static void PaintMiniatureRailwayTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(miniature_railway_track_pieces_25_deg_up_to_flat[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 2, height }, { { 0, 3, height }, { 32, 25, 2 } });

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008AD100 */
static void PaintMiniatureRailwayTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniatureRailwayTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AD110 */
static void PaintMiniatureRailwayTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniatureRailwayTrack25DegUpToFlat(
        session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AD120 */
static void PaintMiniatureRailwayTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniatureRailwayTrackFlatTo25DegUp(
        session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

static constexpr CoordsXY miniature_railway_right_quarter_turn_5_tiles_offsets[4][5] = {
    {
        { 0, 2 },
        { 0, 16 },
        { 0, 0 },
        { 16, 0 },
        { 2, 0 },
    },
    {
        { 2, 0 },
        { 16, 0 },
        { 0, 16 },
        { 0, 0 },
        { 0, 2 },
    },
    {
        { 0, 2 },
        { 0, 0 },
        { 16, 16 },
        { 0, 0 },
        { 2, 0 },
    },
    {
        { 2, 0 },
        { 0, 0 },
        { 16, 0 },
        { 0, 16 },
        { 0, 2 },
    },
};

static constexpr CoordsXY miniature_railway_right_quarter_turn_5_tiles_bound_lengths[4][5] = {
    {
        { 32, 32 },
        { 32, 16 },
        { 16, 16 },
        { 16, 32 },
        { 32, 32 },
    },
    {
        { 32, 32 },
        { 16, 34 },
        { 16, 16 },
        { 32, 16 },
        { 32, 27 },
    },
    {
        { 32, 27 },
        { 32, 16 },
        { 16, 16 },
        { 16, 32 },
        { 27, 32 },
    },
    {
        { 27, 32 },
        { 16, 32 },
        { 16, 16 },
        { 32, 16 },
        { 32, 32 },
    },
};

static constexpr WoodenSupportSubType right_quarter_turn_5_supports_type[4][7] = {
    { WoodenSupportSubType::NeSw, WoodenSupportSubType::Null, WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner0,
      WoodenSupportSubType::Null, WoodenSupportSubType::Corner2, WoodenSupportSubType::NwSe },
    { WoodenSupportSubType::NwSe, WoodenSupportSubType::Null, WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner1,
      WoodenSupportSubType::Null, WoodenSupportSubType::Corner3, WoodenSupportSubType::NeSw },
    { WoodenSupportSubType::NeSw, WoodenSupportSubType::Null, WoodenSupportSubType::Corner0, WoodenSupportSubType::Corner2,
      WoodenSupportSubType::Null, WoodenSupportSubType::Corner0, WoodenSupportSubType::NwSe },
    { WoodenSupportSubType::NwSe, WoodenSupportSubType::Null, WoodenSupportSubType::Corner1, WoodenSupportSubType::Corner3,
      WoodenSupportSubType::Null, WoodenSupportSubType::Corner1, WoodenSupportSubType::NeSw },
};

/** rct2: 0x008AD140 */
static void PaintMiniatureRailwayTrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (right_quarter_turn_5_supports_type[direction][trackSequence] != WoodenSupportSubType::Null)
    {
        TrackPaintUtilRightQuarterTurn5TilesPaint(
            session, 2, height, direction, trackSequence, session.TrackColours,
            miniature_railway_track_pieces_flat_quarter_turn_5_tiles, miniature_railway_right_quarter_turn_5_tiles_offsets,
            miniature_railway_right_quarter_turn_5_tiles_bound_lengths, nullptr);
    }
    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 0 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 1 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD130 */
static void PaintMiniatureRailwayTrackLeftQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    PaintMiniatureRailwayTrackRightQuarterTurn5Tiles(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x8AD150 */
static void PaintMiniatureRailwayTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    static constexpr CoordsXY offsetList[] = {
        { 0, 2 },
        { 0, 0 },
        { 0, 6 },
        { 0, 2 },
    };

    static constexpr CoordsXY boundsList[] = {
        { 32, 27 },
        { 32, 26 },
        { 32, 26 },
        { 32, 27 },
    };

    auto imageId = session.TrackColours.WithIndex(miniature_railway_track_pieces_s_bend_left[direction & 1][trackSequence]);
    CoordsXY offset = offsetList[trackSequence];
    CoordsXY bounds = boundsList[trackSequence];

    PaintAddImageAsParentRotated(
        session, direction, imageId, { offset.x, offset.y, height },
        { { offset.x, offset.y, height }, { bounds.x, bounds.y, 2 } });

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else
    {
        if (trackSequence == 3)
        {
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD160 */
static void PaintMiniatureRailwayTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    static constexpr CoordsXY offsetList[] = {
        { 0, 2 },
        { 0, 6 },
        { 0, 0 },
        { 0, 2 },
    };

    static constexpr CoordsXY boundsList[] = {
        { 32, 27 },
        { 32, 26 },
        { 32, 26 },
        { 32, 27 },
    };

    auto imageId = session.TrackColours.WithIndex(miniature_railway_track_pieces_s_bend_right[direction & 1][trackSequence]);
    CoordsXY offset = offsetList[trackSequence];
    CoordsXY bounds = boundsList[trackSequence];
    PaintAddImageAsParentRotated(
        session, direction, imageId, { offset.x, offset.y, height },
        { { offset.x, offset.y, height }, { bounds.x, bounds.y, 2 } });

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else
    {
        if (trackSequence == 3)
        {
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD1B0 */
static void PaintMiniatureRailwayTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilRightQuarterTurn3TilesPaint(
        session, 3, height, direction, trackSequence, session.TrackColours,
        miniature_railway_track_pieces_flat_quarter_turn_3_tiles, defaultRightQuarterTurn3TilesOffsets,
        defaultRightQuarterTurn3TilesBoundLengths, nullptr);

    // The following piece was missing in vanilla RCT2
    if (trackSequence == 1 && direction == 0)
    {
        auto imageId = session.TrackColours.WithIndex(SPR_G2_MINIATURE_RAILWAY_QUARTER_TURN_3_TILES_SW_SE_PART_3);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 8, 8, 2 } });
    }
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD1A0 */
static void PaintMiniatureRailwayTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    PaintMiniatureRailwayTrackRightQuarterTurn3Tiles(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

static constexpr int8_t paint_miniature_railway_eighth_to_diag_index[] = {
    0, 1, 2, -1, 3,
};

/** rct2: 0x008AD1C0 */
static void PaintMiniatureRailwayTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool isRightEighthToOrthog = trackElement.GetTrackType() == TrackElemType::RightEighthToOrthogonal;

    ImageId imageId;
    int8_t index = paint_miniature_railway_eighth_to_diag_index[trackSequence];
    if (index >= 0)
    {
        imageId = session.TrackColours.WithIndex(miniature_railway_track_pieces_left_eight_to_diag[direction][index]);
        CoordsXY offset = miniature_railway_track_pieces_left_eight_to_diag_offset[direction][index];
        CoordsXYZ bounds = miniature_railway_track_pieces_left_eight_to_diag_bounds[direction][index];
        if (isRightEighthToOrthog)
        {
            bounds = miniature_railway_track_pieces_right_eight_to_orthog_bounds[direction][index];
            offset = miniature_railway_track_pieces_right_eight_to_orthog_offset[direction][index];
        }
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { offset, height }, bounds });
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

    if (trackSequence != 4 || !isRightEighthToOrthog)
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD1D0 */
static void PaintMiniatureRailwayTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool isLeftEighthToOrthog = trackElement.GetTrackType() == TrackElemType::LeftEighthToOrthogonal;

    ImageId imageId;
    int8_t index = paint_miniature_railway_eighth_to_diag_index[trackSequence];
    if (index >= 0)
    {
        imageId = session.TrackColours.WithIndex(miniature_railway_track_pieces_right_eight_to_diag[direction][index]);
        CoordsXY offset = miniature_railway_track_pieces_right_eight_to_diag_offset[direction][index];
        CoordsXYZ bounds = miniature_railway_track_pieces_right_eight_to_diag_bounds[direction][index];
        if (isLeftEighthToOrthog)
        {
            bounds = miniature_railway_track_pieces_left_eight_to_orthog_bounds[direction][index];
            offset = miniature_railway_track_pieces_left_eight_to_orthog_offset[direction][index];
        }
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { offset, height }, bounds });
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    // Fixes #5281.
    if (trackSequence != 4 || !isLeftEighthToOrthog)
        PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AD1E0 */
static void PaintMiniatureRailwayTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    PaintMiniatureRailwayTrackRightEighthToDiag(
        session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AD1F0 */
static void PaintMiniatureRailwayTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    PaintMiniatureRailwayTrackLeftEighthToDiag(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AD200 */
static void MiniatureRailwayTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint32_t imageId = miniature_railway_track_pieces_diag_flat[direction];
    bool drawRail = kDiagSpriteMap[direction][trackSequence];

    if (drawRail)
    {
        PaintAddImageAsParent(
            session, session.TrackColours.WithIndex(imageId), { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
    }
}

/** rct2: 0x008AD230 */
static void MiniatureRailwayTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint32_t imageId = miniature_railway_track_pieces_diag_25_deg_up[direction];
    bool drawRail = kDiagSpriteMap[direction][trackSequence];
    static constexpr int8_t offsetB[] = { +8, 0, +8, +8 };
    if (drawRail)
    {
        PaintAddImageAsParent(
            session, session.TrackColours.WithIndex(imageId), { -16, -16, height },
            { { -16, -16, height + offsetB[direction] }, { 32, 32, 2 } });
    }
}

/** rct2: 0x008AD210 */
static void MiniatureRailwayTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint32_t imageId = miniature_railway_track_pieces_diag_flat_to_25_deg_up[direction];
    bool drawRail = kDiagSpriteMap[direction][trackSequence];

    if (drawRail)
    {
        PaintAddImageAsParent(
            session, session.TrackColours.WithIndex(imageId), { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
    }
}

/** rct2: 0x008AD220 */
static void MiniatureRailwayTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint32_t imageId = miniature_railway_track_pieces_diag_25_deg_up_to_flat[direction];
    bool drawRail = kDiagSpriteMap[direction][trackSequence];

    static constexpr int8_t railOffsets[] = { +8, 0, +8, +8 };

    if (drawRail)
    {
        PaintAddImageAsParent(
            session, session.TrackColours.WithIndex(imageId), { -16, -16, height },
            { { -16, -16, height + railOffsets[direction] }, { 32, 32, 2 } });
    }
}

/** rct2: 0x008AD260 */
static void MiniatureRailwayTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint32_t imageId = miniature_railway_track_pieces_diag_25_deg_up[(direction + 2) % 4];
    bool drawRail = kDiagSpriteMap[direction][trackSequence];
    static constexpr int8_t railOffsets[] = { 0, +8, +8, +8 };

    if (drawRail)
    {
        PaintAddImageAsParent(
            session, session.TrackColours.WithIndex(imageId), { -16, -16, height },
            { { -16, -16, height + railOffsets[direction] }, { 32, 32, 2 } });
    }
}

/** rct2: 0x008AD240 */
static void MiniatureRailwayTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint32_t imageId = miniature_railway_track_pieces_diag_25_deg_up_to_flat[(direction + 2) % 4];
    bool drawRail = kDiagSpriteMap[direction][trackSequence];
    static constexpr int8_t railOffsets[] = { 0, +8, +8, +8 };

    if (drawRail)
    {
        PaintAddImageAsParent(
            session, session.TrackColours.WithIndex(imageId), { -16, -16, height },
            { { -16, -16, height + railOffsets[direction] }, { 32, 32, 2 } });
    }
}

static void MiniatureRailwayTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint32_t imageId = miniature_railway_track_pieces_diag_flat_to_25_deg_up[(direction + 2) % 4];
    bool drawRail = kDiagSpriteMap[direction][trackSequence];

    if (drawRail)
    {
        PaintAddImageAsParent(
            session, session.TrackColours.WithIndex(imageId), { -16, -16, height }, { { -16, -16, height }, { 32, 32, 2 } });
    }
}

/**
 * rct2: 0x008ACE48
 */
TrackPaintFunction GetTrackPaintFunctionMiniatureRailway(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintMiniatureRailwayTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintMiniatureRailwayStation;

        case TrackElemType::Up25:
            return PaintMiniatureRailwayTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintMiniatureRailwayTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintMiniatureRailwayTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintMiniatureRailwayTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintMiniatureRailwayTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintMiniatureRailwayTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn5Tiles:
            return PaintMiniatureRailwayTrackLeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return PaintMiniatureRailwayTrackRightQuarterTurn5Tiles;

        case TrackElemType::SBendLeft:
            return PaintMiniatureRailwayTrackSBendLeft;
        case TrackElemType::SBendRight:
            return PaintMiniatureRailwayTrackSBendRight;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintMiniatureRailwayTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintMiniatureRailwayTrackRightQuarterTurn3Tiles;

        case TrackElemType::LeftEighthToDiag:
            return PaintMiniatureRailwayTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return PaintMiniatureRailwayTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return PaintMiniatureRailwayTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return PaintMiniatureRailwayTrackRightEighthToOrthogonal;

        case TrackElemType::DiagFlat:
            return MiniatureRailwayTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return MiniatureRailwayTrackDiag25DegUp;
        case TrackElemType::DiagFlatToUp25:
            return MiniatureRailwayTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return MiniatureRailwayTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return MiniatureRailwayTrackDiag25DegDown;
        case TrackElemType::DiagFlatToDown25:
            return MiniatureRailwayTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return MiniatureRailwayTrackDiag25DegDownToFlat;
        default:
            return TrackPaintFunctionDummy;
    }
}
