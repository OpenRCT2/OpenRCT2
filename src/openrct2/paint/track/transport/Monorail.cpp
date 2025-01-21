/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../object/StationObject.h"
#include "../../../ride/RideData.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../Boundbox.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

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

static constexpr uint32_t kTrackPiecesFlat[kNumOrthogonalDirections] = {
    SPR_MONORAIL_FLAT_SW_NE,
    SPR_MONORAIL_FLAT_NW_SE,
    SPR_MONORAIL_FLAT_SW_NE,
    SPR_MONORAIL_FLAT_NW_SE,
};

static constexpr uint32_t monorail_track_pieces_flat_quarter_turn_5_tiles[kNumOrthogonalDirections][5] = {
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
    },
};

static constexpr uint32_t monorail_track_pieces_25_deg_up[kNumOrthogonalDirections] = {
    SPR_MONORAIL_25_DEG_UP_SW_NE,
    SPR_MONORAIL_25_DEG_UP_NW_SE,
    SPR_MONORAIL_25_DEG_UP_NE_SW,
    SPR_MONORAIL_25_DEG_UP_SE_NW,
};

static constexpr uint32_t monorail_track_pieces_flat_to_25_deg_up[kNumOrthogonalDirections] = {
    SPR_MONORAIL_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_MONORAIL_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_MONORAIL_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_MONORAIL_FLAT_TO_25_DEG_UP_SE_NW,
};

static constexpr uint32_t monorail_track_pieces_25_deg_up_to_flat[kNumOrthogonalDirections] = {
    SPR_MONORAIL_25_DEG_UP_TO_FLAT_SW_NE,
    SPR_MONORAIL_25_DEG_UP_TO_FLAT_NW_SE,
    SPR_MONORAIL_25_DEG_UP_TO_FLAT_NE_SW,
    SPR_MONORAIL_25_DEG_UP_TO_FLAT_SE_NW,
};

static constexpr uint32_t monorail_track_pieces_s_bend_left[2][kNumOrthogonalDirections] = {
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
    },
};

static constexpr uint32_t monorail_track_pieces_s_bend_right[2][kNumOrthogonalDirections] = {
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
    },
};

static constexpr uint32_t monorail_track_pieces_flat_quarter_turn_3_tiles[kNumOrthogonalDirections][3] = {
    {
        SPR_MONORAIL_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_SW_SE_PART_2,
    },
    {
        SPR_MONORAIL_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_NW_SW_PART_2,
    },
    {
        SPR_MONORAIL_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_NE_NW_PART_2,
    },
    {
        SPR_MONORAIL_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_MONORAIL_QUARTER_TURN_3_TILES_SE_NE_PART_2,
    },
};

static constexpr uint32_t ghost_train_track_pieces_right_eight_to_diag[4][4] = {
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

static constexpr BoundBoxXY GhostTrainTrackPiecesRightEightToDiagBoxes[4][4] = {
    {
        { { 0, 6 }, { 32, 20 } },
        { { 0, 16 }, { 32, 16 } },
        { { 0, 0 }, { 16, 16 } },
        { { 16, 0 }, { 16, 16 } },
    },
    {
        { { 6, 0 }, { 20, 32 } },
        { { 16, 0 }, { 16, 32 } },
        { { 0, 16 }, { 16, 16 } },
        { { 0, 0 }, { 16, 16 } },
    },
    {

        { { 0, 6 }, { 32, 20 } },
        { { 0, 0 }, { 34, 16 } },
        { { 4, 4 }, { 28, 28 } },
        { { 0, 16 }, { 16, 18 } },
    },
    {

        { { 6, 0 }, { 20, 32 } },
        { { 0, 0 }, { 16, 32 } },
        { { 16, 0 }, { 16, 16 } },
        { { 16, 16 }, { 16, 16 } },
    },
};

static constexpr uint32_t ghost_train_track_pieces_left_eight_to_diag[4][4] = {
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

static constexpr BoundBoxXY GhostTrainTrackPiecesLeftEightToDiagBoxes[4][4] = {
    {
        { { 0, 6 }, { 32, 20 } },
        { { 0, 0 }, { 32, 16 } },
        { { 0, 16 }, { 16, 16 } },
        { { 16, 16 }, { 16, 16 } },
    },
    {
        { { 6, 0 }, { 20, 32 } },
        { { 0, 0 }, { 16, 34 } },
        { { 16, 16 }, { 16, 16 } },
        { { 16, 0 }, { 18, 16 } },
    },
    {
        { { 0, 6 }, { 32, 20 } },
        { { 0, 16 }, { 32, 16 } },
        { { 16, 0 }, { 16, 16 } },
        { { 0, 0 }, { 16, 16 } },
    },
    {
        { { 6, 0 }, { 20, 32 } },
        { { 16, 0 }, { 16, 32 } },
        { { 0, 0 }, { 16, 16 } },
        { { 0, 16 }, { 16, 16 } },
    },
};

static constexpr uint32_t monorail_track_pieces_diag_flat[kNumOrthogonalDirections] = {
    SPR_MONORAIL_DIAG_FLAT_W_E,
    SPR_MONORAIL_DIAG_FLAT_N_S,
    SPR_MONORAIL_DIAG_FLAT_E_W,
    SPR_MONORAIL_DIAG_FLAT_S_N,
};

static constexpr uint32_t monorail_track_pieces_diag_flat_to_25_deg_up[kNumOrthogonalDirections] = {
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_W_E,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_N_S,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_E_W,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_S_N,
};

static constexpr uint32_t monorail_track_pieces_diag_25_deg_down_to_flat[kNumOrthogonalDirections] = {
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_E_W,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_S_N,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_W_E,
    SPR_MONORAIL_DIAG_FLAT_TO_25_DEG_UP_N_S,
};

static constexpr uint32_t monorail_track_pieces_diag_25_deg_up_to_flat[kNumOrthogonalDirections] = {
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_W_E,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_N_S,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_E_W,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_S_N,
};

static constexpr uint32_t monorail_track_pieces_diag_flat_to_25_deg_down[kNumOrthogonalDirections] = {
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_E_W,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_S_N,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_W_E,
    SPR_MONORAIL_DIAG_25_DEG_UP_TO_FLAT_N_S,
};

static constexpr uint32_t monorail_track_pieces_diag_25_deg_up[kNumOrthogonalDirections] = {
    SPR_MONORAIL_DIAG_25_DEG_UP_W_E,
    SPR_MONORAIL_DIAG_25_DEG_UP_N_S,
    SPR_MONORAIL_DIAG_25_DEG_UP_E_W,
    SPR_MONORAIL_DIAG_25_DEG_UP_S_N,
};

static constexpr uint32_t monorail_track_pieces_diag_25_deg_down[kNumOrthogonalDirections] = {
    SPR_MONORAIL_DIAG_25_DEG_UP_E_W,
    SPR_MONORAIL_DIAG_25_DEG_UP_S_N,
    SPR_MONORAIL_DIAG_25_DEG_UP_W_E,
    SPR_MONORAIL_DIAG_25_DEG_UP_N_S,
};

/** rct2: 0x008AE1AC */
static void PaintMonorailTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(kTrackPiecesFlat[direction]);

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 6, height }, { 32, 20, 3 });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 6, 0, height }, { 20, 32, 3 });
    }

    if (direction == 0 || direction == 2)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE25C, 0x008AE26C, 0x008AE27C */
static void PaintMonorailStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    const StationObject* stationObject = nullptr;

    stationObject = ride.GetStationObject();

    if (stationObject == nullptr || !(stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS))
    {
        if (direction == 0 || direction == 2)
        {
            imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_STATION_BASE_B_SW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 0, 2, height }, { 32, 28, 2 } });
        }
        else if (direction == 1 || direction == 3)
        {
            imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_STATION_BASE_B_NW_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 2, 0, height }, { 28, 32, 2 } });
        }
    }

    imageId = session.TrackColours.WithIndex(kTrackPiecesFlat[direction]);
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsChild(session, imageId, { 0, 6, height }, { { 0, 0, height }, { 32, 20, 2 } });
    }
    else
    {
        PaintAddImageAsChild(session, imageId, { 6, 0, height }, { { 0, 0, height }, { 20, 32, 2 } });
    }

    if (direction == 0 || direction == 2)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);

    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE1BC */
static void PaintMonorailTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(monorail_track_pieces_25_deg_up[direction]);

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 6, height }, { 32, 20, 3 });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 6, 0, height }, { 20, 32, 3 });
    }

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

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 8, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AE1CC */
static void PaintMonorailTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(monorail_track_pieces_flat_to_25_deg_up[direction]);

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 6, height }, { 32, 20, 3 });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 6, 0, height }, { 20, 32, 3 });
    }

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

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 3, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AE1DC */
static void PaintMonorailTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(monorail_track_pieces_25_deg_up_to_flat[direction]);

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 6, height }, { 32, 20, 3 });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 6, 0, height }, { 20, 32, 3 });
    }

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

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 6, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008AE1EC */
static void PaintMonorailTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMonorailTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AE1FC */
static void PaintMonorailTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMonorailTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AE20C */
static void PaintMonorailTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMonorailTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AE22C */
static void PaintMonorailTrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilRightQuarterTurn5TilesPaint(
        session, 3, height, direction, trackSequence, session.TrackColours, monorail_track_pieces_flat_quarter_turn_5_tiles,
        kDefaultRightQuarterTurn5TilesOffsets, kDefaultRightQuarterTurn5TilesBoundLengths, nullptr);

    switch (trackSequence)
    {
        case 0:
        case 6:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
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

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::rightCorner);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::bottomRightSide, PaintSegment::rightCorner,
                PaintSegment::bottomCorner, PaintSegment::topRightSide);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::centre);
            break;
        case 5:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRightSide, PaintSegment::centre, PaintSegment::bottomLeftSide, PaintSegment::leftCorner,
                PaintSegment::bottomCorner, PaintSegment::topLeftSide);
            break;
        case 6:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRightSide, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::leftCorner);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE21C */
static void PaintMonorailTrackLeftQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    PaintMonorailTrackRightQuarterTurn5Tiles(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x */
static void PaintMonorailTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t originalTrackSequence = trackSequence;
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    static constexpr BoundBoxXY boxList[] = {
        { { 0, 6 }, { 32, 20 } },
        { { 0, 0 }, { 32, 26 } },
        { { 0, 6 }, { 32, 26 } },
        { { 0, 6 }, { 32, 20 } },
    };

    auto imageId = session.TrackColours.WithIndex(monorail_track_pieces_s_bend_left[direction & 1][trackSequence]);
    const CoordsXY& offset = boxList[trackSequence].offset;
    const CoordsXY& bounds = boxList[trackSequence].length;
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { offset.x, offset.y, height }, { bounds.x, bounds.y, 3 });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { offset.y, offset.x, height }, { bounds.y, bounds.x, 3 });
    }

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
    DrawSBendLeftSupports(session, supportType.metal, originalTrackSequence, direction, height, 0, 0);

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::topCorner);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::leftCorner,
                PaintSegment::topLeftSide, PaintSegment::topCorner);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomCorner,
                PaintSegment::bottomRightSide, PaintSegment::rightCorner);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomCorner);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction & 1), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE24C */
static void PaintMonorailTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t originalTrackSequence = trackSequence;
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    static constexpr BoundBoxXY boxList[] = {
        { { 0, 6 }, { 32, 20 } }, { { 0, 6 }, { 32, 26 } }, { { 0, 0 }, { 32, 26 } }, { { 0, 6 }, { 32, 20 } }
    };

    auto imageId = session.TrackColours.WithIndex(monorail_track_pieces_s_bend_right[direction & 1][trackSequence]);
    const CoordsXY& offset = boxList[trackSequence].offset;
    const CoordsXY& bounds = boxList[trackSequence].length;
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { offset.x, offset.y, height }, { bounds.x, bounds.y, 3 });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { offset.y, offset.x, height }, { bounds.y, bounds.x, 3 });
    }

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
    DrawSBendRightSupports(session, supportType.metal, originalTrackSequence, direction, height, 0, 0);

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::rightCorner);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomCorner,
                PaintSegment::bottomRightSide, PaintSegment::rightCorner);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::leftCorner,
                PaintSegment::topLeftSide, PaintSegment::topCorner);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::leftCorner);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction & 1), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE29C */
static void PaintMonorailTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilRightQuarterTurn3TilesPaint(
        session, 3, height, direction, trackSequence, session.TrackColours, monorail_track_pieces_flat_quarter_turn_3_tiles,
        defaultRightQuarterTurn3TilesOffsets, defaultRightQuarterTurn3TilesBoundLengths, nullptr);
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
        case 3:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::rightCorner);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::bottomRightSide, PaintSegment::bottomCorner);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRightSide, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::leftCorner);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE28C */
static void PaintMonorailTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    PaintMonorailTrackRightQuarterTurn3Tiles(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

static constexpr int8_t paint_monorail_eighth_to_diag_index[] = {
    0, 1, 2, -1, 3,
};

/** rct2: 0x008AE31C */
static void PaintMonorailTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    int8_t index = paint_monorail_eighth_to_diag_index[trackSequence];
    if (index >= 0)
    {
        auto imageId = session.TrackColours.WithIndex(ghost_train_track_pieces_left_eight_to_diag[direction][index]);
        const CoordsXY& offset = GhostTrainTrackPiecesLeftEightToDiagBoxes[direction][index].offset;
        const CoordsXY& bounds = GhostTrainTrackPiecesLeftEightToDiagBoxes[direction][index].length;
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { { offset.x, offset.y, height }, { bounds.x, bounds.y, 2 } });
    }

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
        case 4:
            if (direction == 0)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomCorner, 0, height, session.SupportColours);
            if (direction == 1)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::LeftCorner, 0, height, session.SupportColours);
            if (direction == 2)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopCorner, 0, height, session.SupportColours);
            if (direction == 3)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::RightCorner, 0, height, session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::leftCorner,
                PaintSegment::topLeftSide, PaintSegment::topCorner);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomCorner, PaintSegment::bottomRightSide,
                PaintSegment::rightCorner);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::leftCorner, PaintSegment::topLeftSide);
            break;
        case 4:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::topLeftSide,
                PaintSegment::bottomCorner, PaintSegment::bottomRightSide);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE32C */
static void PaintMonorailTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    int8_t index = paint_monorail_eighth_to_diag_index[trackSequence];
    if (index >= 0)
    {
        auto imageId = session.TrackColours.WithIndex(ghost_train_track_pieces_right_eight_to_diag[direction][index]);
        const CoordsXY offset = GhostTrainTrackPiecesRightEightToDiagBoxes[direction][index].offset;
        const CoordsXY bounds = GhostTrainTrackPiecesRightEightToDiagBoxes[direction][index].length;
        PaintAddImageAsParent(
            session, imageId, { 0, 0, height }, { { offset.x, offset.y, height }, { bounds.x, bounds.y, 2 } });
    }

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
        case 4:
            if (direction == 0)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::LeftCorner, 0, height, session.SupportColours);
            if (direction == 1)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopCorner, 0, height, session.SupportColours);
            if (direction == 2)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::RightCorner, 0, height, session.SupportColours);
            if (direction == 3)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomCorner, 0, height, session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomCorner,
                PaintSegment::bottomRightSide, PaintSegment::rightCorner);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::leftCorner, PaintSegment::topLeftSide,
                PaintSegment::topCorner);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::bottomCorner, PaintSegment::bottomRightSide);
            break;
        case 4:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::leftCorner,
                PaintSegment::topLeftSide, PaintSegment::bottomRightSide);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE33C */
static void PaintMonorailTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    PaintMonorailTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AE34C */
static void PaintMonorailTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    PaintMonorailTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AE2AC */
static void PaintMonorailTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilDiagTilesPaintExtra(
        session, 2, height, direction, trackSequence, monorail_track_pieces_diag_flat, supportType.metal);
}

/** rct2: 0x008AE2DC */
static void PaintMonorailTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilDiagTilesPaint(
        session, 2, height, direction, trackSequence, monorail_track_pieces_diag_25_deg_up, defaultDiagTileOffsets,
        defaultDiagBoundLengths, nullptr);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetup(
            session, supportType.metal, kDiagSupportPlacement[direction], 8, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AE2BC */
static void PaintMonorailTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilDiagTilesPaint(
        session, 2, height, direction, trackSequence, monorail_track_pieces_diag_flat_to_25_deg_up, defaultDiagTileOffsets,
        defaultDiagBoundLengths, nullptr);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetup(
            session, supportType.metal, kDiagSupportPlacement[direction], 0, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AE2CC */
static void PaintMonorailTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilDiagTilesPaint(
        session, 2, height, direction, trackSequence, monorail_track_pieces_diag_25_deg_up_to_flat, defaultDiagTileOffsets,
        defaultDiagBoundLengths, nullptr);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetup(
            session, supportType.metal, kDiagSupportPlacement[direction], 4, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AE30C */
static void PaintMonorailTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilDiagTilesPaint(
        session, 2, height, direction, trackSequence, monorail_track_pieces_diag_25_deg_down, defaultDiagTileOffsets,
        defaultDiagBoundLengths, nullptr);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetup(
            session, supportType.metal, kDiagSupportPlacement[direction], 8, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AE2EC */
static void PaintMonorailTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilDiagTilesPaint(
        session, 2, height, direction, trackSequence, monorail_track_pieces_diag_flat_to_25_deg_down, defaultDiagTileOffsets,
        defaultDiagBoundLengths, nullptr);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetup(
            session, supportType.metal, kDiagSupportPlacement[direction], 4, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AE2FC */
static void PaintMonorailTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilDiagTilesPaint(
        session, 2, height, direction, trackSequence, monorail_track_pieces_diag_25_deg_down_to_flat, defaultDiagTileOffsets,
        defaultDiagBoundLengths, nullptr);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetup(
            session, supportType.metal, kDiagSupportPlacement[direction], 0, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/**
 * rct2: 0x008ADF34
 */
TrackPaintFunction GetTrackPaintFunctionMonorail(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintMonorailTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintMonorailStation;

        case TrackElemType::Up25:
            return PaintMonorailTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintMonorailTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintMonorailTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintMonorailTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintMonorailTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintMonorailTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn5Tiles:
            return PaintMonorailTrackLeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return PaintMonorailTrackRightQuarterTurn5Tiles;

        case TrackElemType::SBendLeft:
            return PaintMonorailTrackSBendLeft;
        case TrackElemType::SBendRight:
            return PaintMonorailTrackSBendRight;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintMonorailTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintMonorailTrackRightQuarterTurn3Tiles;

        case TrackElemType::LeftEighthToDiag:
            return PaintMonorailTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return PaintMonorailTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return PaintMonorailTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return PaintMonorailTrackRightEighthToOrthogonal;

        case TrackElemType::DiagFlat:
            return PaintMonorailTrackDiagFlat;

        case TrackElemType::DiagUp25:
            return PaintMonorailTrackDiag25DegUp;
        case TrackElemType::DiagFlatToUp25:
            return PaintMonorailTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return PaintMonorailTrackDiag25DegUpToFlat;

        case TrackElemType::DiagDown25:
            return PaintMonorailTrackDiag25DegDown;
        case TrackElemType::DiagFlatToDown25:
            return PaintMonorailTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return PaintMonorailTrackDiag25DegDownToFlat;
        default:
            return TrackPaintFunctionDummy;
    }
}
