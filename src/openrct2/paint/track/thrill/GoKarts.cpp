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
#include "../../../ride/Ride.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

enum
{
    SPR_GO_KARTS_FLAT_SW_NE = 20752,
    SPR_GO_KARTS_FLAT_NW_SE = 20753,
    SPR_GO_KARTS_FLAT_FRONT_SW_NE = 20754,
    SPR_GO_KARTS_FLAT_FRONT_NW_SE = 20755,
    SPR_GO_KARTS_STARTING_GRID_END_SW_NE = 20756,
    SPR_GO_KARTS_STARTING_GRID_END_NW_SE = 20757,
    SPR_GO_KARTS_STARTING_GRID_END_NE_SW = 20758,
    SPR_GO_KARTS_STARTING_GRID_END_SE_NW = 20759,
    SPR_GO_KARTS_STARTING_GRID_END_FRONT_SW_NE = 20760,
    SPR_GO_KARTS_STARTING_GRID_END_FRONT_NW_SE = 20761,
    SPR_GO_KARTS_STARTING_GRID_END_FRONT_NE_SW = 20762,
    SPR_GO_KARTS_STARTING_GRID_END_FRONT_SE_NW = 20763,
    SPR_GO_KARTS_STARTING_GRID_SW_NE = 20764,
    SPR_GO_KARTS_STARTING_GRID_NW_SE = 20765,
    SPR_GO_KARTS_STARTING_GRID_NE_SW = 20766,
    SPR_GO_KARTS_STARTING_GRID_SE_NW = 20767,
    SPR_GO_KARTS_STARTING_GRID_FRONT_SW_NE = 20768,
    SPR_GO_KARTS_STARTING_GRID_FRONT_NW_SE = 20769,
    SPR_GO_KARTS_STARTING_GRID_FRONT_NE_SW = 20770,
    SPR_GO_KARTS_STARTING_GRID_FRONT_SE_NW = 20771,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SW_NE = 20772,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NW_SE = 20773,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NE_SW = 20774,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SE_NW = 20775,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SW_NE = 20776,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NW_SE = 20777,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NE_SW = 20778,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SE_NW = 20779,
    SPR_GO_KARTS_25_DEG_UP_SW_NE = 20780,
    SPR_GO_KARTS_25_DEG_UP_NW_SE = 20781,
    SPR_GO_KARTS_25_DEG_UP_NE_SW = 20782,
    SPR_GO_KARTS_25_DEG_UP_SE_NW = 20783,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 20784,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 20785,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 20786,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 20787,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SW_NE = 20788,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NW_SE = 20789,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NE_SW = 20790,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SE_NW = 20791,
    SPR_GO_KARTS_25_DEG_UP_FRONT_SW_NE = 20792,
    SPR_GO_KARTS_25_DEG_UP_FRONT_NW_SE = 20793,
    SPR_GO_KARTS_25_DEG_UP_FRONT_NE_SW = 20794,
    SPR_GO_KARTS_25_DEG_UP_FRONT_SE_NW = 20795,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_SW_NW = 20796,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_NW_NE = 20797,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_NE_SE = 20798,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_SE_SW = 20799,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_SW_NW = 20800,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_NW_NE = 20801,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_NE_SE = 20802,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_SE_SW = 20803,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_SW_NW = 20804, // Empty
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_NW_NE = 20805,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_NE_SE = 20806,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_SE_SW = 20807,
    SPR_GO_KARTS_START_POLE_RED_SW_NE = 20808,
    SPR_GO_KARTS_START_POLE_GREEN_SW_NE = 20809,
    SPR_GO_KARTS_START_POLE_NW_SE = 20810,
    SPR_GO_KARTS_START_POLE_NE_SW = 20811,
    SPR_GO_KARTS_START_POLE_RED_SE_NW = 20812,
    SPR_GO_KARTS_START_POLE_GREEN_SE_NW = 20813,
    SPR_GO_KARTS_START_LIGHTS_RED_SW_NE = 20814,
    SPR_GO_KARTS_START_LIGHTS_GREEN_SW_NE = 20815,
    SPR_GO_KARTS_START_LIGHTS_NW_SE = 20816,
    SPR_GO_KARTS_START_LIGHTS_NE_SW = 20817,
    SPR_GO_KARTS_START_LIGHTS_RED_SE_NW = 20818,
    SPR_GO_KARTS_START_LIGHTS_GREEN_SE_NW = 20819,
};

static constexpr uint32_t go_karts_track_pieces_starting_grid_end[4][2] = {
    { SPR_GO_KARTS_STARTING_GRID_END_SW_NE, SPR_GO_KARTS_STARTING_GRID_END_FRONT_SW_NE },
    { SPR_GO_KARTS_STARTING_GRID_END_NW_SE, SPR_GO_KARTS_STARTING_GRID_END_FRONT_NW_SE },
    { SPR_GO_KARTS_STARTING_GRID_END_NE_SW, SPR_GO_KARTS_STARTING_GRID_END_FRONT_NE_SW },
    { SPR_GO_KARTS_STARTING_GRID_END_SE_NW, SPR_GO_KARTS_STARTING_GRID_END_FRONT_SE_NW },
};

static constexpr uint32_t go_karts_track_pieces_starting_grid[4][2] = {
    { SPR_GO_KARTS_STARTING_GRID_SW_NE, SPR_GO_KARTS_STARTING_GRID_FRONT_SW_NE },
    { SPR_GO_KARTS_STARTING_GRID_NW_SE, SPR_GO_KARTS_STARTING_GRID_FRONT_NW_SE },
    { SPR_GO_KARTS_STARTING_GRID_NE_SW, SPR_GO_KARTS_STARTING_GRID_FRONT_NE_SW },
    { SPR_GO_KARTS_STARTING_GRID_SE_NW, SPR_GO_KARTS_STARTING_GRID_FRONT_SE_NW },
};

static constexpr uint32_t go_karts_track_pieces_25_deg_up[4][2] = {
    { SPR_GO_KARTS_25_DEG_UP_SW_NE, SPR_GO_KARTS_25_DEG_UP_FRONT_SW_NE },
    { SPR_GO_KARTS_25_DEG_UP_NW_SE, SPR_GO_KARTS_25_DEG_UP_FRONT_NW_SE },
    { SPR_GO_KARTS_25_DEG_UP_NE_SW, SPR_GO_KARTS_25_DEG_UP_FRONT_NE_SW },
    { SPR_GO_KARTS_25_DEG_UP_SE_NW, SPR_GO_KARTS_25_DEG_UP_FRONT_SE_NW },
};

static constexpr uint32_t go_karts_track_pieces_flat_to_25_deg_up[4][2] = {
    { SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SW_NE, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SW_NE },
    { SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NW_SE, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NW_SE },
    { SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NE_SW, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NE_SW },
    { SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SE_NW, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SE_NW },
};

static constexpr uint32_t go_karts_track_pieces_25_deg_up_to_flat[4][2] = {
    { SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SW_NE, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SW_NE },
    { SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NW_SE, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NW_SE },
    { SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NE_SW, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NE_SW },
    { SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SE_NW, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SE_NW },
};

static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsLeftQuarterTurn3TilesSprites = {
        SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 0,  SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 1,  SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 2,
        SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 3,  SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 4,  SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 5,
        SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 6,  SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 7,  SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 8,
        SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 9,  SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 10, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 11,
        SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 12, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 13, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 14,
        SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 15, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 16, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 17,
        SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 18, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 19, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 20,
        SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 21, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 22, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 23,
        SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 24, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 25, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 26,
        SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 27, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 28, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 29,
        SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 30, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 31,
    };

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsLeftQuarterTurn3TilesBoundBoxes = { {
        { {
            { { { { 0, 2, 1 }, { 32, 26, 1 } }, { { 0, 28, 2 }, { 32, 1, 16 } } } },
            { { { { 0, 16, 0 }, { 16, 16, 1 } }, { { 7, 3, 2 }, { 1, 1, 1 } } } },
            { { { { 16, 0, 0 }, { 16, 16, 1 } }, { { 16, 0, 22 }, { 16, 16, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 16, 16, 1 } }, { { 0, 0, 22 }, { 20, 32, 1 } } } },
            { { { { 16, 16, 0 }, { 16, 16, 1 } }, { { 16, 16, 22 }, { 16, 16, 1 } } } },
            { { { { 0, 0, 1 }, { 16, 16, 1 } }, { { 15, 15, 2 }, { 1, 1, 1 } } } },
            { { { { 0, 0, 0 }, { 16, 16, 1 } }, { { 0, 0, 22 }, { 32, 20, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } } },
            { { { { 16, 0, 0 }, { 16, 16, 1 } }, { { 3, 7, 1 }, { 1, 1, 1 } } } },
            { { { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 16, 22 }, { 16, 16, 1 } } } },
            { { { { 2, 0, 1 }, { 26, 32, 1 } }, { { 28, 0, 2 }, { 1, 32, 16 } } } },
        } },
        { {
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 8, 8, 1 } }, { { 7, 7, 1 }, { 1, 1, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 16, 16, 22 }, { 16, 16, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 22 }, { 32, 26, 1 } } } },
        } },
    } };

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 3>, 7>, kNumOrthogonalDirections>
kGoKartsLeftQuarterTurn5TilesSprites = {
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 0, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 1, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 2, kImageIndexUndefined, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 3, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 4, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 5, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 6, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 7, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 8, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 9, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 10, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 11, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 12, kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 13, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 14, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 15, kImageIndexUndefined, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 16, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 17, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 18, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 19, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 20,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 21, kImageIndexUndefined, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 22, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 23, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 24, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 25, kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 26, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 27, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 28, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 29, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 30, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 31, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 32, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 33, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 34, kImageIndexUndefined, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 35, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 36, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 37, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 38, kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 39, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 40, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 41, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 42, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 43, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 44, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 45, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 46, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 47, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 48, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 49, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 50, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 51, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 52, kImageIndexUndefined,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 3>, 7>, kNumOrthogonalDirections>
    kGoKartsLeftQuarterTurn5TilesBoundBoxes = { {
        { {
            { { { { 0, 2, 1 }, { 32, 26, 1 } }, { { 0, 28, 2 }, { 32, 1, 16 } } } },
            { { { { 0, 16, 0 }, { 16, 16, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 16, 1 } }, { { 0, 0, 22 }, { 32, 16, 1 } } } },
            { { { { 0, 0, 0 }, { 16, 32, 1 } }, { { 0, 0, 22 }, { 16, 32, 1 } } } },
            { { { { 0, 16, 1 }, { 16, 16, 1 } }, { { 7, 31, 2 }, { 1, 1, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 16, 1 } }, { { 0, 0, 22 }, { 32, 16, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 28, 0, 1 }, { 1, 32, 3 } } } },
            { { { { 16, 16, 0 }, { 16, 16, 1 } } } },
            { { { { 0, 0, 0 }, { 16, 16, 1 } }, { { 28, 0, 1 }, { 1, 16, 3 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 16, 16, 22 }, { 16, 16, 1 } }, { { 32, 32, 0 }, { 1, 1, 1 } } } },
            { { { { 16, 16, 0 }, { 16, 16, 1 } } } },
            { { { { 0, 0, 0 }, { 16, 16, 1 } }, { { 0, 28, 1 }, { 16, 1, 3 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 28, 1 }, { 32, 1, 3 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } } },
            { { { { 16, 0, 1 }, { 16, 16, 1 } }, { { 31, 7, 2 }, { 1, 1, 1 } } } },
            { { { { 0, 0, 0 }, { 16, 32, 1 } }, { { 0, 0, 22 }, { 16, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 16, 1 } }, { { 0, 0, 22 }, { 32, 16, 1 } } } },
            { { { { 16, 0, 0 }, { 16, 16, 1 } } } },
            { { { { 0, 0, 0 }, { 16, 32, 1 } }, { { 0, 0, 22 }, { 16, 32, 1 } } } },
            { { { { 2, 0, 1 }, { 26, 32, 1 } }, { { 28, 0, 2 }, { 1, 32, 16 } } } },
        } },
        { {
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } } } },
            { { { { 0, 0, 1 }, { 16, 16, 1 } }, { { 0, 16, 2 }, { 1, 1, 1 } } } },
            { { { { 16, 0, 2 }, { 16, 32, 1 } }, { { 16, 0, 22 }, { 16, 32, 1 } } } },
            { { { { 0, 0, 1 }, { 16, 16, 1 } }, { { 16, 16, 2 }, { 1, 1, 1 } } } },
            { { { { 0, 0, 1 }, { 16, 16, 1 } }, { { 16, 0, 2 }, { 1, 1, 1 } } } },
            { { { { 0, 16, 2 }, { 32, 16, 1 } }, { { 0, 16, 22 }, { 32, 16, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 22 }, { 32, 26, 1 } } } },
        } },
    } };

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 3>, 5>, kNumOrthogonalDirections>
kGoKartsLeftEighthToDiagSprites = {
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 0,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 1,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 2,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 3,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 4,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 5,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 6,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 7,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 8,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 9,  kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 10, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 11, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 12, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 13, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 14, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 15, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 16, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 17, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 18, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 19, kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 20,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 21,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 22,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 23,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 24,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 25,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 26,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 27,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 28,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 29,  kImageIndexUndefined,  kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 30,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 31,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 32,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 33,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 34,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 35,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 36,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 37,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 38,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 39,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 40,  kImageIndexUndefined,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 3>, 5>, kNumOrthogonalDirections>
    kGoKartsLeftEighthToDiagBoundBoxes = { {
        { {
            { { { { 0, 2, 1 }, { 32, 26, 1 } }, { { 0, 28, 2 }, { 32, 1, 16 } } } },
            { { { { 0, 0, 0 }, { 32, 28, 1 } }, { { 0, 0, 22 }, { 32, 28, 1 } } } },
            { { { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 16, 22 }, { 16, 16, 1 } } } },
            { { { { 16, 0, 0 }, { 16, 16, 1 } }, { { 16, 0, 22 }, { 16, 16, 1 } } } },
            { { { { 0, 0, 1 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 22 }, { 28, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 22 }, { 28, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 16, 16, 22 }, { 16, 16, 1 } } } },
            { { { { 0, 0, 0 }, { 16, 16, 1 } }, { { 8, 8, 1 }, { 1, 1, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } }, { { 1, 33, 1 }, { 1, 1, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 16, 1 } }, { { 0, 0, 22 }, { 32, 16, 1 } } } },
            { { { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 16, 22 }, { 16, 16, 1 } } } },
            { { { { 0, 0, 0 }, { 16, 16, 1 } } } },
        } },
        { {
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } } } },
            { { { { 32, 32, 1 }, { 1, 1, 1 } }, { { 8, 0, 0 }, { 24, 32, 1 } }, { { 8, 0, 22 }, { 24, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 16, 16, 1 } }, { { 15, 15, 0 }, { 1, 1, 1 } } } },
            { { { { 16, 16, 0 }, { 16, 16, 1 } }, { { 16, 16, 22 }, { 16, 16, 1 } } } },
            { { { { 0, 1, 0 }, { 1, 1, 1 } }, { { 30, 31, 0 }, { 1, 1, 1 } } } },
        } },
    } };

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 3>, 5>, kNumOrthogonalDirections>
kGoKartsRightEighthToDiagSprites = {
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 41,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 42,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 43,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 44,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 45,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 46,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 47,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 48,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 49,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 50,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 51,  kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 52, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 53, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 54,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 55, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 56, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 57, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 58, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 59, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 60, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 61, kImageIndexUndefined, kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 62,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 63,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 64,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 65,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 66,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 67,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 68,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 69,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 70,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 71,  kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 72,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 73,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 74,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 75,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 76,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 77,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 78,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 79,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 80,  SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 81,  kImageIndexUndefined,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 3>, 5>, kNumOrthogonalDirections>
    kGoKartsRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kGoKartsLeftEighthToDiagBoundBoxes);

static constexpr std::array<std::array<std::array<ImageIndex, 3>, 4>, kNumOrthogonalDirections> kGoKartsSBendLeftSprites = {
    SPR_G2_GO_KARTS_TRACK_S_BEND + 0,  SPR_G2_GO_KARTS_TRACK_S_BEND + 1,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 2,  SPR_G2_GO_KARTS_TRACK_S_BEND + 3,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 4,  SPR_G2_GO_KARTS_TRACK_S_BEND + 5,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 6,  SPR_G2_GO_KARTS_TRACK_S_BEND + 7,  kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_S_BEND + 8,  SPR_G2_GO_KARTS_TRACK_S_BEND + 9,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 10, SPR_G2_GO_KARTS_TRACK_S_BEND + 11, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 12, SPR_G2_GO_KARTS_TRACK_S_BEND + 13, SPR_G2_GO_KARTS_TRACK_S_BEND + 14,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 15, SPR_G2_GO_KARTS_TRACK_S_BEND + 16, kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_S_BEND + 6,  SPR_G2_GO_KARTS_TRACK_S_BEND + 7,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 4,  SPR_G2_GO_KARTS_TRACK_S_BEND + 5,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 2,  SPR_G2_GO_KARTS_TRACK_S_BEND + 3,  kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 0,  SPR_G2_GO_KARTS_TRACK_S_BEND + 1,  kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_S_BEND + 15, SPR_G2_GO_KARTS_TRACK_S_BEND + 16, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 12, SPR_G2_GO_KARTS_TRACK_S_BEND + 13, SPR_G2_GO_KARTS_TRACK_S_BEND + 14,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 10, SPR_G2_GO_KARTS_TRACK_S_BEND + 11, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 8,  SPR_G2_GO_KARTS_TRACK_S_BEND + 9,  kImageIndexUndefined,
};

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 3>, 4>, kNumOrthogonalDirections> kGoKartsSBendLeftBoundBoxes = {
    {
        { {
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 22 }, { 32, 26, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 26, 1 } }, { { 0, 0, 22 }, { 32, 26, 1 } } } },
            { { { { 0, 6, 0 }, { 32, 26, 1 } }, { { 0, 6, 22 }, { 32, 26, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 22 }, { 32, 26, 1 } } } },
        } },
        { {
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 26, 29, 1 } }, { { 0, 0, 22 }, { 26, 29, 1 } } } },
            { { { { 6, 0, 0 }, { 26, 32, 1 } }, { { 6, 0, 22 }, { 26, 32, 1 } }, { { 32, 32, 22 }, { 1, 1, 1 } } } },
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } } } },
        } },
        { {
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 22 }, { 32, 26, 1 } } } },
            { { { { 0, 6, 0 }, { 32, 26, 1 } }, { { 0, 6, 22 }, { 32, 26, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 26, 1 } }, { { 0, 0, 22 }, { 32, 26, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 22 }, { 32, 26, 1 } } } },
        } },
        { {
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } } } },
            { { { { 6, 0, 0 }, { 26, 32, 1 } }, { { 6, 0, 22 }, { 26, 32, 1 } }, { { 32, 32, 22 }, { 1, 1, 1 } } } },
            { { { { 0, 0, 0 }, { 26, 29, 1 } }, { { 0, 0, 22 }, { 26, 29, 1 } } } },
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } } } },
        } },
    }
};

static constexpr std::array<std::array<std::array<ImageIndex, 3>, 4>, kNumOrthogonalDirections> kGoKartsSBendRightSprites = {
    SPR_G2_GO_KARTS_TRACK_S_BEND + 17, SPR_G2_GO_KARTS_TRACK_S_BEND + 18, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 19, SPR_G2_GO_KARTS_TRACK_S_BEND + 20, SPR_G2_GO_KARTS_TRACK_S_BEND + 21,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 22, SPR_G2_GO_KARTS_TRACK_S_BEND + 23, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 24, SPR_G2_GO_KARTS_TRACK_S_BEND + 25, kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_S_BEND + 26, SPR_G2_GO_KARTS_TRACK_S_BEND + 27, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 28, SPR_G2_GO_KARTS_TRACK_S_BEND + 29, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 30, SPR_G2_GO_KARTS_TRACK_S_BEND + 31, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 32, SPR_G2_GO_KARTS_TRACK_S_BEND + 33, kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_S_BEND + 24, SPR_G2_GO_KARTS_TRACK_S_BEND + 25, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 22, SPR_G2_GO_KARTS_TRACK_S_BEND + 23, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 19, SPR_G2_GO_KARTS_TRACK_S_BEND + 20, SPR_G2_GO_KARTS_TRACK_S_BEND + 21,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 17, SPR_G2_GO_KARTS_TRACK_S_BEND + 18, kImageIndexUndefined,

    SPR_G2_GO_KARTS_TRACK_S_BEND + 32, SPR_G2_GO_KARTS_TRACK_S_BEND + 33, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 30, SPR_G2_GO_KARTS_TRACK_S_BEND + 31, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 28, SPR_G2_GO_KARTS_TRACK_S_BEND + 29, kImageIndexUndefined,
    SPR_G2_GO_KARTS_TRACK_S_BEND + 26, SPR_G2_GO_KARTS_TRACK_S_BEND + 27, kImageIndexUndefined,
};

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 3>, 4>, kNumOrthogonalDirections>
    kGoKartsSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kGoKartsSBendLeftBoundBoxes);

/** rct2: 0x0074A748 */
static void PaintGoKartsTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    if (direction == 0 || direction == 2)
    {
        imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 28, 1 } });

        imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_FRONT_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 2 }, { 32, 1, 3 } });

        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 28, 32, 1 } });

        imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_FRONT_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 0, height + 2 }, { 1, 32, 3 } });

        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0074A758 */
static void PaintGoKartsTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    PaintStruct* ps;

    imageId = session.TrackColours.WithIndex(go_karts_track_pieces_25_deg_up[direction][0]);
    if (direction == 0 || direction == 2)
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 28, 1 } });
    }
    else
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 28, 32, 1 } });
    }

    imageId = session.TrackColours.WithIndex(go_karts_track_pieces_25_deg_up[direction][1]);
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 2 }, { 32, 1, 11 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 0, height + 2 }, { 1, 32, 11 } });
    }

    session.WoodenSupportsPrependTo = ps;

    DrawSupportForSequenceA<TrackElemType::Up25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

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

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0074A768 */
static void PaintGoKartsTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    PaintStruct* ps;

    imageId = session.TrackColours.WithIndex(go_karts_track_pieces_flat_to_25_deg_up[direction][0]);
    if (direction == 0 || direction == 2)
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 28, 1 } });
    }
    else
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 28, 32, 1 } });
    }

    imageId = session.TrackColours.WithIndex(go_karts_track_pieces_flat_to_25_deg_up[direction][1]);
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 2 }, { 32, 1, 11 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 0, height + 2 }, { 1, 32, 11 } });
    }

    session.WoodenSupportsPrependTo = ps;

    DrawSupportForSequenceA<TrackElemType::FlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

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

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x */
static void PaintGoKartsTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    PaintStruct* ps;

    imageId = session.TrackColours.WithIndex(go_karts_track_pieces_25_deg_up_to_flat[direction][0]);
    if (direction == 0 || direction == 2)
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 28, 1 } });
    }
    else
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 28, 32, 1 } });
    }

    imageId = session.TrackColours.WithIndex(go_karts_track_pieces_25_deg_up_to_flat[direction][1]);
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 2 }, { 32, 1, 11 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 0, height + 2 }, { 1, 32, 11 } });
    }

    session.WoodenSupportsPrependTo = ps;

    DrawSupportForSequenceA<TrackElemType::Up25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

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

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x0074A788 */
static void PaintGoKartsTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGoKartsTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0074A798 */
static void PaintGoKartsTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGoKartsTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0074A7A8 */
static void PaintGoKartsTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGoKartsTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x */
static void PaintGoKartsStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const auto* stationObj = ride.getStationObject();

    bool hasFence;
    ImageId imageId;
    const uint32_t(*sprites)[2] = go_karts_track_pieces_starting_grid;

    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        sprites = go_karts_track_pieces_starting_grid_end;
    }

    imageId = session.TrackColours.WithIndex(sprites[direction][0]);
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 28, 1 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 28, 32, 1 } });
    }

    auto stationColour = GetStationColourScheme(session, trackElement);
    if (direction == 0 || direction == 2)
    {
        hasFence = TrackPaintUtilHasFence(EDGE_NW, session.MapPosition, trackElement, ride, session.CurrentRotation);
        TrackPaintUtilDrawStationCovers(session, EDGE_NW, hasFence, stationObj, height, stationColour);
    }
    else
    {
        hasFence = TrackPaintUtilHasFence(EDGE_NE, session.MapPosition, trackElement, ride, session.CurrentRotation);
        TrackPaintUtilDrawStationCovers(session, EDGE_NE, hasFence, stationObj, height, stationColour);
    }

    imageId = session.TrackColours.WithIndex(sprites[direction][1]);
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 2 }, { 32, 1, 3 } });

        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 0, height + 2 }, { 1, 32, 3 } });

        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 0 || direction == 2)
    {
        hasFence = TrackPaintUtilHasFence(EDGE_SE, session.MapPosition, trackElement, ride, session.CurrentRotation);
        TrackPaintUtilDrawStationCovers(session, EDGE_SE, hasFence, stationObj, height, stationColour);
    }
    else
    {
        hasFence = TrackPaintUtilHasFence(EDGE_SW, session.MapPosition, trackElement, ride, session.CurrentRotation);
        TrackPaintUtilDrawStationCovers(session, EDGE_SW, hasFence, stationObj, height, stationColour);
    }

    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        const bool hasGreenLight = trackElement.HasGreenLight();

        switch (direction)
        {
            case 0:
                imageId = session.TrackColours.WithIndex(
                    (hasGreenLight ? SPR_GO_KARTS_START_POLE_GREEN_SW_NE : SPR_GO_KARTS_START_POLE_RED_SW_NE));
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 1, height + 4 }, { 3, 3, 13 } });

                imageId = session.TrackColours.WithIndex(
                    (hasGreenLight ? SPR_GO_KARTS_START_LIGHTS_GREEN_SW_NE : SPR_GO_KARTS_START_LIGHTS_RED_SW_NE));
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 28, height + 4 }, { 3, 3, 13 } });
                break;
            case 1:
                imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_START_POLE_NW_SE);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 28, height + 4 }, { 3, 3, 13 } });

                imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_START_LIGHTS_NW_SE);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 28, height + 4 }, { 3, 3, 13 } });
                break;
            case 2:
                imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_START_POLE_NE_SW);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 1, height + 4 }, { 3, 3, 13 } });

                imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_START_LIGHTS_NE_SW);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 28, height + 4 }, { 3, 3, 13 } });
                break;
            case 3:
                imageId = session.TrackColours.WithIndex(
                    (hasGreenLight ? SPR_GO_KARTS_START_POLE_GREEN_SE_NW : SPR_GO_KARTS_START_POLE_RED_SE_NW));
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 1, height + 4 }, { 3, 3, 13 } });

                imageId = session.TrackColours.WithIndex(
                    (hasGreenLight ? SPR_GO_KARTS_START_LIGHTS_GREEN_SE_NW : SPR_GO_KARTS_START_LIGHTS_RED_SE_NW));
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 1, height + 4 }, { 3, 3, 13 } });
                break;
        }
    }

    DrawSupportForSequenceA<TrackElemType::EndStation>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0074A7E8 */
static void PaintGoKartsTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 28, 1 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 2, height + 2 }, { 1, 1, 3 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 29, height + 2 }, { 16, 1, 3 } });
            break;
        case 1:
            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_NE_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 30, 30, 1 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_NE_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 29, height + 2 }, { 16, 1, 3 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_NE_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 2, height + 2 }, { 1, 16, 3 } });
            break;
        case 2:
            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_SE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 28, 32, 1 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_SE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 2, height + 2 }, { 1, 1, 3 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_SE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 14, height + 2 }, { 1, 16, 3 } });
            break;
        case 3:
            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_SW_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_SW_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 29, height + 2 }, { 1, 1, 3 } });

            // The empty sprite isn't drawn
            break;
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 2:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 3:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0074A7F8 */
static void PaintGoKartsTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGoKartsTrackLeftQuarterTurn1Tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

static void TrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftQuarterTurn3TilesSprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsLeftQuarterTurn3TilesBoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftQuarterTurn3TilesSprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsLeftQuarterTurn3TilesBoundBoxes[direction][trackSequence][1]);

    static constexpr std::array<WoodenSupportSubType, 4> woodenSupportSubTypes = {
        WoodenSupportSubType::NeSw,
        WoodenSupportSubType::Null,
        WoodenSupportSubType::Corner3,
        WoodenSupportSubType::NwSe,
    };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, direction, height, session.SupportColours,
            WoodenSupportTransitionType::None);
    }
    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);
    static constexpr std::array<int32_t, 4> blockedSegments = {
        kSegmentsAll,
        kSegmentsAll,
        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackLeftQuarterTurn3Tiles(
        session, ride, kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence], DirectionPrev(direction), height,
        trackElement, supportType);
}

static void TrackLeftQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const uint8_t direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftQuarterTurn5TilesSprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsLeftQuarterTurn5TilesBoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftQuarterTurn5TilesSprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsLeftQuarterTurn5TilesBoundBoxes[direction][trackSequence][1]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftQuarterTurn5TilesSprites[direction][trackSequence][2]), height,
        { 0, 0, 0 }, kGoKartsLeftQuarterTurn5TilesBoundBoxes[direction][trackSequence][2]);

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftQuarterTurn5Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else if (trackSequence == 6 && (direction == 2 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, DirectionPrev(direction), height, kTunnelGroup, TunnelSubType::Flat);
    }
    static constexpr std::array<int32_t, 7> blockedSegments = {
        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
        kSegmentsAll,
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
            PaintSegment::bottomLeft),
        EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
        kSegmentsAll,
        EnumsToFlags(
            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackLeftQuarterTurn5Tiles(
        session, ride, kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence], DirectionPrev(direction), height,
        trackElement, supportType);
}

static void TrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftEighthToDiagSprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsLeftEighthToDiagBoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftEighthToDiagSprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsLeftEighthToDiagBoundBoxes[direction][trackSequence][1]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftEighthToDiagSprites[direction][trackSequence][2]), height,
        { 0, 0, 0 }, kGoKartsLeftEighthToDiagBoundBoxes[direction][trackSequence][2]);

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftEighthToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsRightEighthToDiagSprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsRightEighthToDiagBoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsRightEighthToDiagSprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsRightEighthToDiagBoundBoxes[direction][trackSequence][1]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsRightEighthToDiagSprites[direction][trackSequence][2]), height,
        { 0, 0, 0 }, kGoKartsRightEighthToDiagBoundBoxes[direction][trackSequence][2]);

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightEighthToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackRightEighthToDiag(
        session, ride, mapLeftEighthTurnToOrthogonal[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackLeftEighthToDiag(
        session, ride, mapLeftEighthTurnToOrthogonal[trackSequence], DirectionPrev(direction), height, trackElement,
        supportType);
}

static void TrackSBendLeft(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsSBendLeftSprites[direction][trackSequence][0]), height, { 0, 0, 0 },
        kGoKartsSBendLeftBoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsSBendLeftSprites[direction][trackSequence][1]), height, { 0, 0, 0 },
        kGoKartsSBendLeftBoundBoxes[direction][trackSequence][1]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsSBendLeftSprites[direction][trackSequence][2]), height, { 0, 0, 0 },
        kGoKartsSBendLeftBoundBoxes[direction][trackSequence][2]);

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::SBendLeft>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if ((trackSequence == 0 && (direction == 0 || direction == 3))
        || (trackSequence == 3 && (direction == 1 || direction == 2)))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    static constexpr std::array<int32_t, 4> blockedSegments = {
        kSegmentsAll,
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
            PaintSegment::bottomLeft),
        EnumsToFlags(
            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackSBendRight(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsSBendRightSprites[direction][trackSequence][0]), height, { 0, 0, 0 },
        kGoKartsSBendRightBoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsSBendRightSprites[direction][trackSequence][1]), height, { 0, 0, 0 },
        kGoKartsSBendRightBoundBoxes[direction][trackSequence][1]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsSBendRightSprites[direction][trackSequence][2]), height, { 0, 0, 0 },
        kGoKartsSBendRightBoundBoxes[direction][trackSequence][2]);

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::SBendRight>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if ((trackSequence == 0 && (direction == 0 || direction == 3))
        || (trackSequence == 3 && (direction == 1 || direction == 2)))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    static constexpr std::array<int32_t, 4> blockedSegments = {
        kSegmentsAll,
        EnumsToFlags(
            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
            PaintSegment::bottomLeft),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x0074A668
 */
TrackPaintFunction GetTrackPaintFunctionGoKarts(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintGoKartsTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintGoKartsStation;

        case TrackElemType::Up25:
            return PaintGoKartsTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintGoKartsTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintGoKartsTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintGoKartsTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintGoKartsTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintGoKartsTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintGoKartsTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintGoKartsTrackRightQuarterTurn1Tile;

        // Added by OpenRCT2

        // Small turns
        case TrackElemType::LeftQuarterTurn3Tiles:
            return TrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return TrackRightQuarterTurn3Tiles;

        // Medium turns
        case TrackElemType::LeftQuarterTurn5Tiles:
            return TrackLeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return TrackRightQuarterTurn5Tiles;

        // Large turns
        case TrackElemType::LeftEighthToDiag:
            return TrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return TrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return TrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return TrackRightEighthToOrthogonal;

        // S bends
        case TrackElemType::SBendLeft:
            return TrackSBendLeft;
        case TrackElemType::SBendRight:
            return TrackSBendRight;

        default:
            return TrackPaintFunctionDummy;
    }
}
