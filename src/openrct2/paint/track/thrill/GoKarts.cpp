/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections> kGoKartsUp60Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_STEEP + 16,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP + 17,
    kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP + 18,
    kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP + 19,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP + 20,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP + 21,
};

static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kGoKartsUp60BoundBoxes = { {
    { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 86 }, { 32, 28, 1 } } } },
    { { { { 0, 0, 0 }, { 0, 0, 0 } }, { { 4, 28, 0 }, { 24, 2, 77 } } } },
    { { { { 0, 0, 0 }, { 0, 0, 0 } }, { { 28, 4, 0 }, { 2, 24, 77 } } } },
    { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 86 }, { 28, 32, 1 } } } },
} };

static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections> kGoKartsUp25ToUp60Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_STEEP + 0, SPR_TRACKS_GO_KARTS_TRACK_STEEP + 1, SPR_TRACKS_GO_KARTS_TRACK_STEEP + 2,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP + 3, SPR_TRACKS_GO_KARTS_TRACK_STEEP + 4, SPR_TRACKS_GO_KARTS_TRACK_STEEP + 5,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP + 6, SPR_TRACKS_GO_KARTS_TRACK_STEEP + 7,
};

static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kGoKartsUp25ToUp60BoundBoxes = { {
    { { { { 0, 2, 0 }, { 32, 28, 0 } }, { { 0, 2, 54 }, { 32, 28, 1 } } } },
    { { { { 6, 0, 0 }, { 20, 32, 3 } }, { { 4, 28, 0 }, { 24, 2, 56 } } } },
    { { { { 0, 6, 0 }, { 32, 20, 3 } }, { { 28, 4, 0 }, { 2, 24, 56 } } } },
    { { { { 2, 0, 0 }, { 28, 32, 0 } }, { { 2, 0, 54 }, { 28, 32, 1 } } } },
} };

static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections> kGoKartsUp60ToUp25Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_STEEP + 8,  SPR_TRACKS_GO_KARTS_TRACK_STEEP + 9,  SPR_TRACKS_GO_KARTS_TRACK_STEEP + 10,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP + 11, SPR_TRACKS_GO_KARTS_TRACK_STEEP + 12, SPR_TRACKS_GO_KARTS_TRACK_STEEP + 13,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP + 14, SPR_TRACKS_GO_KARTS_TRACK_STEEP + 15,
};

static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsFlatToUp60LongBaseSprites = {
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 0,  SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 1,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 2,  SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 3,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 4,  SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 5,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 6,  SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 7,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 8,  SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 9,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 10, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 11,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 12, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 13,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 14, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 15,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 16, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 17,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 18, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 19,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 20, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 21,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 22, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 23,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 24, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 25,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 26, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 27,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 28, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 29,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 30, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 31,
    };

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsFlatToUp60LongBaseBoundBoxes = { {
        { {
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 30 }, { 32, 28, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 38 }, { 32, 28, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 46 }, { 32, 28, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 70 }, { 32, 28, 1 } } } },
        } },
        { {
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 30 }, { 28, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 38 }, { 28, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 46 }, { 28, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 0, 0, 0 } }, { { 4, 28, -16 }, { 24, 2, 56 } } } },
        } },
        { {
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 30 }, { 32, 28, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 38 }, { 32, 28, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 46 }, { 32, 28, 1 } } } },
            { { { { 0, 0, 0 }, { 0, 0, 0 } }, { { 28, 4, -16 }, { 2, 24, 56 } } } },
        } },
        { {
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 30 }, { 28, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 46 }, { 28, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 46 }, { 28, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 70 }, { 28, 32, 1 } } } },
        } },
    } };

static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsUp60ToFlatLongBaseSprites = {
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 32, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 33,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 34, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 35,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 36, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 37,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 38, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 39,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 40, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 41,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 42, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 43,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 44, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 45,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 46, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 47,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 48, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 49,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 50, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 51,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 52, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 53,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 54, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 55,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 56, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 57,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 58, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 59,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 60, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 61,
        SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 62, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_STEEP + 63,
    };

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsUp60ToFlatLongBaseBoundBoxes = { {
        { {
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 70 }, { 32, 28, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 70 }, { 32, 28, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 46 }, { 32, 28, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 30 }, { 32, 28, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 0, 0, 0 } }, { { 4, 28, -16 }, { 24, 2, 56 } } } },
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 70 }, { 28, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 46 }, { 28, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 30 }, { 28, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 0, 0, 0 } }, { { 28, 4, -16 }, { 2, 24, 56 } } } },
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 70 }, { 32, 28, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 46 }, { 32, 28, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 30 }, { 32, 28, 1 } } } },
        } },
        { {
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 70 }, { 28, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 70 }, { 28, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 46 }, { 28, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 30 }, { 28, 32, 1 } } } },
        } },
    } };

static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsLeftQuarterTurn3TilesSprites = {
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 0,  SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 1,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 2,  SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 3,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 4,  SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 5,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 6,  SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 7,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 8,  SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 9,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 10, SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 11,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 12, SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 13,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 14, SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 15,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 16, SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 17,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 18, SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 19,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 20, SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 21,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 22, SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 23,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 24, SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 25,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 26, SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 27,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 28, SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 29,
        SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 30, SPR_TRACKS_GO_KARTS_TRACK_SMALL_CURVE + 31,
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
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 0, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 1, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 2, kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 3, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 4, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 5, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 6, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 7, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 8, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 9, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 10, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 11, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 12, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 13, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 14, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 15, kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 16, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 17, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 18, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 19, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 20,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 21, kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 22, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 23, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 24, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 25, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 26, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 27, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 28, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 29, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 30, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 31, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 32, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 33, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 34, kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 35, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 36, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 37, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 38, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 39, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 40, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 41, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 42, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 43, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 44, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 45, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 46, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 47, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 48, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 49, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 50, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 51, SPR_TRACKS_GO_KARTS_TRACK_MEDIUM_CURVE + 52, kImageIndexUndefined,
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
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 0,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 1,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 2,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 3,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 4,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 5,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 6,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 7,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 8,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 9,  kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 10, SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 11, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 12, SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 13, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 14, SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 15, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 16, SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 17, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 18, SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 19, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 20,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 21,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 22,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 23,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 24,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 25,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 26,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 27,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 28,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 29,  kImageIndexUndefined,  kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 30,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 31,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 32,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 33,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 34,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 35,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 36,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 37,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 38,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 39,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 40,  kImageIndexUndefined,
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
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 41,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 42,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 43,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 44,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 45,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 46,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 47,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 48,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 49,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 50,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 51,  kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 52, SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 53, SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 54,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 55, SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 56, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 57, SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 58, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 59, SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 60, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 61, kImageIndexUndefined, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 62,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 63,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 64,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 65,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 66,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 67,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 68,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 69,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 70,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 71,  kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 72,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 73,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 74,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 75,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 76,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 77,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 78,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 79,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 80,  SPR_TRACKS_GO_KARTS_TRACK_LARGE_CURVE + 81,  kImageIndexUndefined,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 3>, 5>, kNumOrthogonalDirections>
    kGoKartsRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kGoKartsLeftEighthToDiagBoundBoxes);

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections> kGoKartsDiagFlatSprites = {
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 0, SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 1,
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 2, SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 3,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 4, SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 5,

    SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 7, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 6, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 4, SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 5,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 2, SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 3,
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 0, SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 1,

    SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 6, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_DIAGONAL + 7, kImageIndexUndefined,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections> kGoKartsDiagFlatBoundBoxes = {
    {
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } } },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 22 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 1 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } } },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 22 }, { 32, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { 0, 0, 1 }, { 32, 32, 1 } } } },
        } },
    }
};

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections> kGoKartsDiagFlatToUp25Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 0, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 1,
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 2, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 3,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 4, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 5,

    SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 6, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 7, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 8, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 9,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 10, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 11,
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 12, SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 13,

    SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 14, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 15, kImageIndexUndefined,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsDiagFlatToUp25BoundBoxes = { {
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } } },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 30 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 1 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } } },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 30 }, { 32, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { 16, 16, 0 }, { 16, 16, 1 } } } },
        } },
    } };

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections> kGoKartsDiagUp25ToFlatSprites = {
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 0, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 1,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 2, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 3,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 4, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 5,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 6, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 7, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 8, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 9,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 10, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 11,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 12, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 13,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 14, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 15, kImageIndexUndefined,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsDiagUp25ToFlatBoundBoxes = { {
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } } },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 30 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 1 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } } },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 30 }, { 32, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { 0, 0, 9 }, { 32, 32, 1 } } } },
        } },
    } };

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections> kGoKartsDiagUp25Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 0, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 1,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 2, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 3,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 4, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 5,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 6, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 7, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 8, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 9,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 10, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 11,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 12, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 13,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_DIAGONAL + 14, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections> kGoKartsDiagUp25BoundBoxes = {
    {
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 1 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { {} },
        } },
    }
};

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections> kGoKartsDiagUp25ToUp60Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 0, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 1,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 2, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 3,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 4, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 5,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 6, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 7, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 8, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 9,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 10, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 11,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 12, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 13,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 14, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 15, kImageIndexUndefined,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsDiagUp25ToUp60BoundBoxes = { {
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } } },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 54 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 1 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } } },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 54 }, { 32, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { 0, 0, 8 }, { 32, 32, 1 } } } },
        } },
    } };

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections> kGoKartsDiagUp60ToUp25Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 0, SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 1,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 2, SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 3,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 4, SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 5,

    SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 6, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 7, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 8, SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 9,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 10, SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 11,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 12, SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 13,

    SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 14, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 15, kImageIndexUndefined,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsDiagUp60ToUp25BoundBoxes = { {
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } } },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 54 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 1 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } } },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 54 }, { 32, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { 0, 0, 32 }, { 32, 32, 1 } } } },
        } },
    } };

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections> kGoKartsDiagUp60Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 0, SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 1,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 2, SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 3,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 4, SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 5,

    SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 6, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 7, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 8, SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 9,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 10, SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 11,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 12, SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 13,

    SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 14, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_STEEP_DIAGONAL + 15, kImageIndexUndefined,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections> kGoKartsDiagUp60BoundBoxes = {
    {
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 86 }, { 32, 32, 1 } } } },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 86 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 86 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 1 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 86 }, { 32, 32, 1 } } } },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 86 }, { 32, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 86 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { -16, -16, 0 }, { 32, 32, 1 } } } },
            { {} },
            { {} },
            { { { { 0, 0, 8 }, { 32, 32, 1 } } } },
        } },
    }
};

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
kGoKartsLeftQuarterTurn3TilesUp25Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 0, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 1,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 2, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 3,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 4, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 5,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 6, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 7,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 8, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 9,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 10, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 11,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 12, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 13,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 14, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 15,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 16, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 17,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 18, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 19,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 20, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 21,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 22, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 23,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 24, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 25,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsLeftQuarterTurn3TilesUp25BoundBoxes = { {
        { {
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } } },
            { { { { 0, 16, 0 }, { 16, 16, 0 } }, { { 0, 16, 22 }, { 16, 16, 1 } } } },
            { { { { 16, 0, 0 }, { 16, 16, 0 } }, { { 16, 0, 22 }, { 16, 16, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 16, 16, 0 } }, { { 0, 0, 22 }, { 16, 16, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 28, 1 } }, { { 0, 0, 38 }, { 32, 28, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 16, 0 }, { 16, 16, 0 } }, { { 0, 16, 22 }, { 16, 16, 1 } } } },
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } } },
        } },
        { {
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } } },
            { {} },
            { { { { 16, 16, 0 }, { 32, 32, 0 } }, { { 16, 16, 22 }, { 32, 32, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } } },
        } },
    } };

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
kGoKartsRightQuarterTurn3TilesUp25Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 26, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 27,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 28, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 29,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 30, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 31,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 32, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 33,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 34, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 35,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 36, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 37,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 38, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 39,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 40, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 41,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 42, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 43,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 44, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 45,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 46, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 47,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 48, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 49,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 50, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 51,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kGoKartsRightQuarterTurn3TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(
        kGoKartsLeftQuarterTurn3TilesUp25BoundBoxes);

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
kGoKartsLeftQuarterTurn5TilesUp25Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 0, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 1,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 2, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 3,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 4, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 5,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 6, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 7,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 8, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 9,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 10, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 11,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 12, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 13,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 14, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 15,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 16, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 17,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 18, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 19,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 20, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 21,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 22, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 23,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 24, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 25,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 26, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 27,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 28, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 29,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 30, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 31,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 32, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 33,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 34, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 35,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 36, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 37,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 38, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 39,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kGoKartsLeftQuarterTurn5TilesUp25BoundBoxes = { {
        { {
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 16, 1 } }, { { 0, 0, 38 }, { 32, 16, 1 } } } },
            { { { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 16, 38 }, { 16, 16, 1 } } } },
            { {} },
            { { { { 16, 0, 0 }, { 16, 32, 1 } }, { { 16, 0, 38 }, { 16, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } } },
            { {} },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } } },
            { { { { 0, 16, 0 }, { 32, 32, 1 } }, { { 0, 16, 38 }, { 32, 32, 1 } } } },
            { {} },
            { { { { -16, -16, 8 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 28, 1 } }, { { 0, 0, 38 }, { 32, 28, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 16, 0 }, { 32, 16, 1 } }, { { 0, 16, 38 }, { 32, 16, 1 } } } },
            { { { { 16, 0, 0 }, { 16, 16, 1 } }, { { 16, 0, 38 }, { 16, 16, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 16, 32, 1 } }, { { 0, 0, 38 }, { 16, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } } },
        } },
        { {
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } } },
            { {} },
            { { { { 16, 0, 0 }, { 16, 32, 1 } }, { { 16, 0, 38 }, { 16, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 16, 16, 1 } }, { { 0, 0, 38 }, { 16, 16, 1 } } } },
            { {} },
            { { { { 0, 16, 0 }, { 32, 16, 1 } }, { { 0, 16, 38 }, { 32, 16, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } } },
        } },
    } };

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
kGoKartsRightQuarterTurn5TilesUp25Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 40, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 41,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 42, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 43,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 44, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 45,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 46, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 47,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 48, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 49,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 50, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 51,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 52, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 53,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 54, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 55,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 56, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 57,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 58, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 59,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 60, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 61,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 62, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 63,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 64, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 65,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 66, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 67,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 68, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 69,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 70, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 71,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 72, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 73,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 74, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 75,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 76, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 77,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 78, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 79,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kGoKartsRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(
        kGoKartsLeftQuarterTurn5TilesUp25BoundBoxes);

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
kGoKartsLeftEighthToDiagUp25Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 0,  SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 1,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 2, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 3,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 4,  SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 5,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 6,  SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 7,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 8, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 9,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 10, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 11,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 12, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 13,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 14, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 15,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 16, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 17,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 18, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 19,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 20, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 21,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 22, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 23,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 24, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 25,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 26, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 27,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 28, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 29,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 30, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 31,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kGoKartsLeftEighthToDiagUp25BoundBoxes = { {
        { {
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 16, 1 } }, { { 0, 0, 38 }, { 32, 16, 1 } } } },
            { { { { 0, 16, 0 }, { 16, 16, 0 } }, { { 0, 16, 38 }, { 16, 16, 1 } } } },
            { {} },
            { { { { 16, 16, 0 }, { 16, 16, 1 } }, { { 16, 16, 38 }, { 16, 16, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 16, 34, 1 } }, { { 0, 0, 38 }, { 16, 34, 1 } } } },
            { { { { 16, 16, 0 }, { 32, 32, 1 } }, { { 16, 16, 38 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 64, 32, 1 } }, { { 0, 0, 38 }, { 64, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { { { { 16, 0, 0 }, { 16, 16, 1 } }, { { 16, 0, 38 }, { 16, 16, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 16, 16, 1 } }, { { 0, 0, 38 }, { 16, 16, 1 } } } },
        } },
        { {
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } } },
            { { { { 16, 0, 0 }, { 16, 32, 1 } }, { { 16, 0, 38 }, { 16, 32, 1 } } } },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
        } },
    } };

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
kGoKartsRightEighthToDiagUp25Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 32,  SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 33,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 34, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 35,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 36,  SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 37,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 38,  SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 39,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 40, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 41,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 42, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 43,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 44, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 45,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 46, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 47,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 48, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 49,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 50, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 51,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 52, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 53,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 54, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 55,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 56, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 57,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 58, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 59,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 60, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 61,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 62, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 63,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kGoKartsRightEighthToDiagUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kGoKartsLeftEighthToDiagUp25BoundBoxes);

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
kGoKartsLeftEighthToOrthogonalUp25Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 64,  SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 65,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 66, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 67,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 68,  SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 69,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 70,  SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 71,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 72, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 73,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 74, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 75,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 76, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 77,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 78, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 79,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 80, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 81,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 82, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 83,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 84, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 85,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 86, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 87,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 88, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 89,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 90, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 91,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 92, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 93,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 94, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 95,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kGoKartsLeftEighthToOrthogonalUp25BoundBoxes = { {
        { {
            { { { { 0, 0, 0 }, { 32, 32, 0 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { { { { 16, 16, 0 }, { 32, 32, 1 } }, { { 16, 16, 38 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 28, 1 } }, { { 0, 0, 38 }, { 32, 28, 1 } } } },
            { { { { 0, 0, 0 }, { 48, 28, 1 } }, { { 0, 0, 38 }, { 32, 28, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 0 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { { { { 16, 0, 0 }, { 16, 16, 1 } }, { { 16, 0, 38 }, { 16, 16, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 0 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 16, 16, 0 } }, { { 0, 0, 38 }, { 16, 16, 0 } } } },
            { { { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 16, 38 }, { 16, 16, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } } },
        } },
    } };

// clang-format off
static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
kGoKartsRightEighthToOrthogonalUp25Sprites = {
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 96,  SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 97,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 98, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 99,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 100,  SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 101,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 102,  SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 103,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 104, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 105,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 106, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 107,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 108, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 109,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 110, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 111,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 112, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 113,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 114, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 115,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 116, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 117,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 118, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 119,

    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 120, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 121,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 122, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 123,
    kImageIndexUndefined, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 124, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 125,
    SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 126, SPR_TRACKS_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 127,
};
// clang-format on

static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kGoKartsRightEighthToOrthogonalUp25BoundBoxes = { {
        { {
            { { { { 0, 0, 0 }, { 32, 32, 0 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { { { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 0 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { { { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 16, 38 }, { 16, 16, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 28, 1 } }, { { 0, 0, 38 }, { 32, 28, 1 } } } },
            { { { { 0, 0, 0 }, { 32, 28, 1 } }, { { 0, 0, 38 }, { 32, 28, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 32, 32, 0 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { { { { 16, 16, 0 }, { 32, 32, 1 } }, { { 16, 16, 38 }, { 32, 32, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } } },
            { { { { 0, 0, 0 }, { 28, 48, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } } },
        } },
        { {
            { { { { 0, 0, 0 }, { 16, 16, 0 } }, { { 0, 0, 38 }, { 16, 16, 1 } } } },
            { { { { 16, 0, 0 }, { 16, 16, 1 } }, { { 16, 0, 38 }, { 16, 16, 1 } } } },
            { {} },
            { { { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } } },
            { { { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } } },
        } },
    } };

static constexpr std::array<std::array<std::array<ImageIndex, 3>, 4>, kNumOrthogonalDirections> kGoKartsSBendLeftSprites = {
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 0,  SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 1,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 2,  SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 3,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 4,  SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 5,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 6,  SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 7,  kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 8,  SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 9,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 10, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 11, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 12, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 13, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 14,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 15, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 16, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 6,  SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 7,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 4,  SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 5,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 2,  SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 3,  kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 0,  SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 1,  kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 15, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 16, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 12, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 13, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 14,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 10, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 11, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 8,  SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 9,  kImageIndexUndefined,
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
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 17, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 18, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 19, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 20, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 21,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 22, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 23, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 24, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 25, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 26, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 27, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 28, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 29, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 30, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 31, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 32, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 33, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 24, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 25, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 22, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 23, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 19, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 20, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 21,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 17, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 18, kImageIndexUndefined,

    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 32, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 33, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 30, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 31, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 28, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 29, kImageIndexUndefined,
    SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 26, SPR_TRACKS_GO_KARTS_TRACK_S_BEND + 27, kImageIndexUndefined,
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

    DrawSupportForSequenceA<TrackElemType::flat>(
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

    DrawSupportForSequenceA<TrackElemType::up25>(
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

    DrawSupportForSequenceA<TrackElemType::flatToUp25>(
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

    DrawSupportForSequenceA<TrackElemType::up25ToFlat>(
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
    const uint32_t (*sprites)[2] = go_karts_track_pieces_starting_grid;

    if (trackElement.GetTrackType() == TrackElemType::endStation)
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

    if (trackElement.GetTrackType() == TrackElemType::endStation)
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

    DrawSupportForSequenceA<TrackElemType::endStation>(
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
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 24, 24, 1 } });

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
        session, supportType.wooden, WoodenSupportSubType::neSw, direction, height, session.SupportColours);

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

static void TrackUp60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsUp60Sprites[direction][0]), height, { 0, 0, 0 },
        kGoKartsUp60BoundBoxes[direction][0]);
    session.WoodenSupportsPrependTo = PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsUp60Sprites[direction][1]), height, { 0, 0, 0 },
        kGoKartsUp60BoundBoxes[direction][1]);

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::neSw, direction, height, session.SupportColours,
        WoodenSupportTransitionType::up60Deg);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

static void TrackUp25ToUp60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsUp25ToUp60Sprites[direction][0]), height, { 0, 0, 0 },
        kGoKartsUp25ToUp60BoundBoxes[direction][0]);
    session.WoodenSupportsPrependTo = PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsUp25ToUp60Sprites[direction][1]), height, { 0, 0, 0 },
        kGoKartsUp25ToUp60BoundBoxes[direction][1]);

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::neSw, direction, height, session.SupportColours,
        WoodenSupportTransitionType::up25DegToUp60Deg);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackUp60ToUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsUp60ToUp25Sprites[direction][0]), height, { 0, 0, 0 },
        kGoKartsUp25ToUp60BoundBoxes[direction][0]);
    session.WoodenSupportsPrependTo = PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsUp60ToUp25Sprites[direction][1]), height, { 0, 0, 0 },
        kGoKartsUp25ToUp60BoundBoxes[direction][1]);

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::neSw, direction, height, session.SupportColours,
        WoodenSupportTransitionType::up60DegToUp25Deg);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackDown60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackUp60(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDown25ToDown60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackUp60ToUp25(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDown60ToDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackUp25ToUp60(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackFlatToUp60LongBase(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const uint8_t direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsFlatToUp60LongBaseSprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsFlatToUp60LongBaseBoundBoxes[direction][trackSequence][0]);
    session.WoodenSupportsPrependTo = PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsFlatToUp60LongBaseSprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsFlatToUp60LongBaseBoundBoxes[direction][trackSequence][1]);

    DrawSupportForSequenceA<TrackElemType::flatToUp60LongBase>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else if (trackSequence == 3 && (direction == 1 || direction == 2))
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    static constexpr std::array generalSupportHeights = { 48, 48, 64, 80 };
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void TrackUp60ToFlatLongBase(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const uint8_t direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsUp60ToFlatLongBaseSprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsUp60ToFlatLongBaseBoundBoxes[direction][trackSequence][0]);
    session.WoodenSupportsPrependTo = PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsUp60ToFlatLongBaseSprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsUp60ToFlatLongBaseBoundBoxes[direction][trackSequence][1]);

    DrawSupportForSequenceA<TrackElemType::up60ToFlatLongBase>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else if (trackSequence == 3 && (direction == 1 || direction == 2))
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    static constexpr std::array generalSupportHeights = { 80, 80, 56, 40 };
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void TrackFlatToDown60LongBase(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackUp60ToFlatLongBase(session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDown60ToFlatLongBase(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackFlatToUp60LongBase(session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
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
        WoodenSupportSubType::neSw,
        WoodenSupportSubType::null,
        WoodenSupportSubType::corner3,
        WoodenSupportSubType::nwSe,
    };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, direction, height, session.SupportColours,
            WoodenSupportTransitionType::none);
    }
    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);
    static constexpr std::array<int32_t, 4> blockedSegments = {
        kSegmentsAll,
        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight),
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

    DrawSupportForSequenceA<TrackElemType::leftQuarterTurn5Tiles>(
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
        kSegmentsAll,
        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
            PaintSegment::bottomLeft),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
            PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        kSegmentsAll,
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

    DrawSupportForSequenceA<TrackElemType::leftEighthToDiag>(
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

    DrawSupportForSequenceA<TrackElemType::rightEighthToDiag>(
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

static void TrackDiagFlat(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagFlatSprites[direction][trackSequence][0]), height, { 0, 0, 0 },
        kGoKartsDiagFlatBoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagFlatSprites[direction][trackSequence][1]), height, { 0, 0, 0 },
        kGoKartsDiagFlatBoundBoxes[direction][trackSequence][1]);

    DrawSupportForSequenceA<TrackElemType::diagFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackDiagFlatToUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagFlatToUp25Sprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsDiagFlatToUp25BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagFlatToUp25Sprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsDiagFlatToUp25BoundBoxes[direction][trackSequence][1]);

    DrawSupportForSequenceA<TrackElemType::diagFlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void TrackDiagUp25ToFlat(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagUp25ToFlatSprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsDiagUp25ToFlatBoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagUp25ToFlatSprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsDiagUp25ToFlatBoundBoxes[direction][trackSequence][1]);

    DrawSupportForSequenceB<TrackElemType::diagUp25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void TrackDiagUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagUp25Sprites[direction][trackSequence][0]), height, { 0, 0, 0 },
        kGoKartsDiagUp25BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagUp25Sprites[direction][trackSequence][1]), height, { 0, 0, 0 },
        kGoKartsDiagUp25BoundBoxes[direction][trackSequence][1]);

    DrawSupportForSequenceB<TrackElemType::diagUp25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void TrackDiagFlatToDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    return TrackDiagUp25ToFlat(
        session, ride, kMapReversedDiagonalStraight[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackDiagDown25ToFlat(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    return TrackDiagFlatToUp25(
        session, ride, kMapReversedDiagonalStraight[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackDiagDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    return TrackDiagUp25(
        session, ride, kMapReversedDiagonalStraight[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackDiagUp25ToUp60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagUp25ToUp60Sprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsDiagUp25ToUp60BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagUp25ToUp60Sprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsDiagUp25ToUp60BoundBoxes[direction][trackSequence][1]);

    DrawSupportForSequenceB<TrackElemType::diagUp25ToUp60>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackDiagUp60ToUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagUp60ToUp25Sprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsDiagUp60ToUp25BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagUp60ToUp25Sprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsDiagUp60ToUp25BoundBoxes[direction][trackSequence][1]);

    DrawSupportForSequenceB<TrackElemType::diagUp60ToUp25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackDiagUp60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagUp60Sprites[direction][trackSequence][0]), height, { 0, 0, 0 },
        kGoKartsDiagUp60BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsDiagUp60Sprites[direction][trackSequence][1]), height, { 0, 0, 0 },
        kGoKartsDiagUp60BoundBoxes[direction][trackSequence][1]);

    DrawSupportForSequenceB<TrackElemType::diagUp60>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

static void TrackDiagDown25ToDown60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    return TrackDiagUp60ToUp25(
        session, ride, kMapReversedDiagonalStraight[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackDiagDown60ToDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    return TrackDiagUp25ToUp60(
        session, ride, kMapReversedDiagonalStraight[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackDiagDown60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    return TrackDiagUp60(
        session, ride, kMapReversedDiagonalStraight[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackLeftQuarterTurn3TilesUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftQuarterTurn3TilesUp25Sprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsLeftQuarterTurn3TilesUp25BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftQuarterTurn3TilesUp25Sprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsLeftQuarterTurn3TilesUp25BoundBoxes[direction][trackSequence][1]);

    static constexpr std::array<WoodenSupportSubType, 4> woodenSupportSubTypes = {
        WoodenSupportSubType::neSw,
        WoodenSupportSubType::null,
        WoodenSupportSubType::corner3,
        WoodenSupportSubType::neSw,
    };
    static constexpr std::array<WoodenSupportTransitionType, 4> woodenSupportTransitionTypes = {
        WoodenSupportTransitionType::up25Deg,
        WoodenSupportTransitionType::none,
        WoodenSupportTransitionType::none,
        WoodenSupportTransitionType::up25Deg,
    };
    static constexpr std::array woodenSupportExtraRotation = {
        0,
        0,
        0,
        -1,
    };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, (direction + woodenSupportExtraRotation[trackSequence]) & 3,
            height, session.SupportColours, woodenSupportTransitionTypes[trackSequence]);
    }

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else if (trackSequence == 3 && (direction == 2 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, DirectionPrev(direction), height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    static constexpr std::array<int32_t, 4> blockedSegments = {
        kSegmentsAll,
        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight),
        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);

    static constexpr std::array generalSupportHeights = { 72, 56, 56, 72 };
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void TrackRightQuarterTurn3TilesUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsRightQuarterTurn3TilesUp25Sprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsRightQuarterTurn3TilesUp25BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsRightQuarterTurn3TilesUp25Sprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsRightQuarterTurn3TilesUp25BoundBoxes[direction][trackSequence][1]);

    static constexpr std::array<WoodenSupportSubType, 4> woodenSupportSubTypes = {
        WoodenSupportSubType::neSw,
        WoodenSupportSubType::null,
        WoodenSupportSubType::corner2,
        WoodenSupportSubType::neSw,
    };
    static constexpr std::array<WoodenSupportTransitionType, 4> woodenSupportTransitionTypes = {
        WoodenSupportTransitionType::up25Deg,
        WoodenSupportTransitionType::none,
        WoodenSupportTransitionType::none,
        WoodenSupportTransitionType::up25Deg,
    };
    static constexpr std::array woodenSupportExtraRotation = {
        0,
        0,
        0,
        1,
    };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, (direction + woodenSupportExtraRotation[trackSequence]) & 3,
            height, session.SupportColours, woodenSupportTransitionTypes[trackSequence]);
    }

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else if (trackSequence == 3 && (direction == 0 || direction == 1))
    {
        PaintUtilPushTunnelRotated(session, DirectionNext(direction), height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    static constexpr std::array<int32_t, 4> blockedSegments = {
        kSegmentsAll,
        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::topRight),
        EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);

    static constexpr std::array generalSupportHeights = { 72, 56, 56, 72 };
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void TrackLeftQuarterTurn3TilesDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackRightQuarterTurn3TilesUp25(
        session, ride, kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence], DirectionNext(direction), height,
        trackElement, supportType);
}

static void TrackRightQuarterTurn3TilesDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackLeftQuarterTurn3TilesUp25(
        session, ride, kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence], DirectionPrev(direction), height,
        trackElement, supportType);
}

static void TrackLeftQuarterTurn5TilesUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftQuarterTurn5TilesUp25Sprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsLeftQuarterTurn5TilesUp25BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftQuarterTurn5TilesUp25Sprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsLeftQuarterTurn5TilesUp25BoundBoxes[direction][trackSequence][1]);

    static constexpr std::array<WoodenSupportSubType, 7> woodenSupportSubTypes = {
        WoodenSupportSubType::neSw, WoodenSupportSubType::null,    WoodenSupportSubType::corner3, WoodenSupportSubType::corner1,
        WoodenSupportSubType::null, WoodenSupportSubType::corner3, WoodenSupportSubType::neSw,
    };
    static constexpr std::array<WoodenSupportTransitionType, 7> woodenSupportTransitionTypes = {
        WoodenSupportTransitionType::up25Deg, WoodenSupportTransitionType::none, WoodenSupportTransitionType::none,
        WoodenSupportTransitionType::none,    WoodenSupportTransitionType::none, WoodenSupportTransitionType::none,
        WoodenSupportTransitionType::none,
    };
    static constexpr std::array woodenSupportHeight = { 0, 0, 0, -16, 0, 0, 0 };
    static constexpr std::array woodenSupportExtraRotation = { 0, 0, 0, 0, 0, 0, -1 };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, (direction + woodenSupportExtraRotation[trackSequence]) & 3,
            height + woodenSupportHeight[trackSequence], session.SupportColours, woodenSupportTransitionTypes[trackSequence]);
    }

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else if (trackSequence == 6 && (direction == 2 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, DirectionPrev(direction), height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    static constexpr std::array<int32_t, 7> blockedSegments = {
        kSegmentsAll,
        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
            PaintSegment::bottomLeft),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
            PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);

    static constexpr std::array generalSupportHeights = { 72, 72, 72, 64, 72, 72, 72 };
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void TrackRightQuarterTurn5TilesUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsRightQuarterTurn5TilesUp25Sprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsRightQuarterTurn5TilesUp25BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsRightQuarterTurn5TilesUp25Sprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsRightQuarterTurn5TilesUp25BoundBoxes[direction][trackSequence][1]);

    static constexpr std::array<WoodenSupportSubType, 7> woodenSupportSubTypes = {
        WoodenSupportSubType::neSw, WoodenSupportSubType::null,    WoodenSupportSubType::corner2, WoodenSupportSubType::corner0,
        WoodenSupportSubType::null, WoodenSupportSubType::corner2, WoodenSupportSubType::neSw,
    };
    static constexpr std::array<WoodenSupportTransitionType, 7> woodenSupportTransitionTypes = {
        WoodenSupportTransitionType::up25Deg, WoodenSupportTransitionType::none, WoodenSupportTransitionType::none,
        WoodenSupportTransitionType::none,    WoodenSupportTransitionType::none, WoodenSupportTransitionType::none,
        WoodenSupportTransitionType::none,
    };
    static constexpr std::array woodenSupportHeight = { 0, 0, 0, -16, 0, 0, 0 };
    static constexpr std::array woodenSupportExtraRotation = { 0, 0, 0, 0, 0, 0, 1 };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, (direction + woodenSupportExtraRotation[trackSequence]) & 3,
            height + woodenSupportHeight[trackSequence], session.SupportColours, woodenSupportTransitionTypes[trackSequence]);
    }

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else if (trackSequence == 6 && (direction == 0 || direction == 1))
    {
        PaintUtilPushTunnelRotated(session, DirectionNext(direction), height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    static constexpr std::array<int32_t, 7> blockedSegments = {
        kSegmentsAll,
        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight),
        EnumsToFlags(
            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft,
            PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight),
        EnumsToFlags(
            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);

    static constexpr std::array generalSupportHeights = { 72, 72, 72, 64, 72, 72, 72 };
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void TrackLeftQuarterTurn5TilesDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackRightQuarterTurn5TilesUp25(
        session, ride, kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence], DirectionNext(direction), height,
        trackElement, supportType);
}

static void TrackRightQuarterTurn5TilesDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackLeftQuarterTurn5TilesUp25(
        session, ride, kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence], DirectionPrev(direction), height,
        trackElement, supportType);
}

static void TrackLeftEighthToDiagUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftEighthToDiagUp25Sprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsLeftEighthToDiagUp25BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftEighthToDiagUp25Sprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsLeftEighthToDiagUp25BoundBoxes[direction][trackSequence][1]);

    static constexpr std::array<WoodenSupportSubType, 5> woodenSupportSubTypes = {
        WoodenSupportSubType::neSw,    WoodenSupportSubType::neSw,    WoodenSupportSubType::corner1,
        WoodenSupportSubType::corner3, WoodenSupportSubType::corner2,
    };
    static constexpr std::array<WoodenSupportTransitionType, 5> woodenSupportTransitionTypes = {
        WoodenSupportTransitionType::up25Deg, WoodenSupportTransitionType::none, WoodenSupportTransitionType::none,
        WoodenSupportTransitionType::none,    WoodenSupportTransitionType::none,
    };
    static constexpr std::array woodenSupportHeight = { 0, 0, -16, 0, 0 };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, direction, height + woodenSupportHeight[trackSequence],
            session.SupportColours, woodenSupportTransitionTypes[trackSequence]);
    }

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackRightEighthToDiagUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsRightEighthToDiagUp25Sprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsRightEighthToDiagUp25BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsRightEighthToDiagUp25Sprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsRightEighthToDiagUp25BoundBoxes[direction][trackSequence][1]);

    static constexpr std::array<WoodenSupportSubType, 5> woodenSupportSubTypes = {
        WoodenSupportSubType::neSw,    WoodenSupportSubType::neSw,    WoodenSupportSubType::corner0,
        WoodenSupportSubType::corner2, WoodenSupportSubType::corner3,
    };
    static constexpr std::array<WoodenSupportTransitionType, 5> woodenSupportTransitionTypes = {
        WoodenSupportTransitionType::up25Deg, WoodenSupportTransitionType::none, WoodenSupportTransitionType::none,
        WoodenSupportTransitionType::none,    WoodenSupportTransitionType::none,
    };
    static constexpr std::array woodenSupportHeight = { 0, 0, -16, 0, 0 };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, direction, height + woodenSupportHeight[trackSequence],
            session.SupportColours, woodenSupportTransitionTypes[trackSequence]);
    }

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackLeftEighthToOrthogonalUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftEighthToOrthogonalUp25Sprites[direction][trackSequence][0]), height,
        { 0, 0, 0 }, kGoKartsLeftEighthToOrthogonalUp25BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsLeftEighthToOrthogonalUp25Sprites[direction][trackSequence][1]), height,
        { 0, 0, 0 }, kGoKartsLeftEighthToOrthogonalUp25BoundBoxes[direction][trackSequence][1]);

    static constexpr std::array<WoodenSupportSubType, 5> woodenSupportSubTypes = {
        WoodenSupportSubType::null, WoodenSupportSubType::corner2, WoodenSupportSubType::corner0,
        WoodenSupportSubType::neSw, WoodenSupportSubType::neSw,
    };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, direction, height, session.SupportColours,
            WoodenSupportTransitionType::none);
    }

    if (trackSequence == 4 && (direction == 1 || direction == 2))
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackRightEighthToOrthogonalUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsRightEighthToOrthogonalUp25Sprites[direction][trackSequence][0]),
        height, { 0, 0, 0 }, kGoKartsRightEighthToOrthogonalUp25BoundBoxes[direction][trackSequence][0]);
    PaintAddImageAsParentHeight(
        session, session.TrackColours.WithIndex(kGoKartsRightEighthToOrthogonalUp25Sprites[direction][trackSequence][1]),
        height, { 0, 0, 0 }, kGoKartsRightEighthToOrthogonalUp25BoundBoxes[direction][trackSequence][1]);

    static constexpr std::array<WoodenSupportSubType, 5> woodenSupportSubTypes = {
        WoodenSupportSubType::null, WoodenSupportSubType::corner0, WoodenSupportSubType::corner2,
        WoodenSupportSubType::nwSe, WoodenSupportSubType::nwSe,
    };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, direction, height, session.SupportColours,
            WoodenSupportTransitionType::none);
    }

    if (trackSequence == 4 && (direction == 0 || direction == 1))
    {
        PaintUtilPushTunnelRotated(session, DirectionNext(direction), height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackLeftEighthToDiagDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    static constexpr std::array map = { 4, 3, 1, 2, 0 };
    TrackRightEighthToOrthogonalUp25(
        session, ride, map[trackSequence], DirectionNext(direction), height, trackElement, supportType);
}

static void TrackRightEighthToDiagDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    static constexpr std::array map = { 4, 3, 1, 2, 0 };
    TrackLeftEighthToOrthogonalUp25(
        session, ride, map[trackSequence], DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackLeftEighthToOrthogonalDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackRightEighthToDiagUp25(
        session, ride, mapLeftEighthTurnToOrthogonal[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackRightEighthToOrthogonalDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackLeftEighthToDiagUp25(
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

    DrawSupportForSequenceA<TrackElemType::sBendLeft>(
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

    DrawSupportForSequenceA<TrackElemType::sBendRight>(
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
TrackPaintFunction GetTrackPaintFunctionGoKarts(TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::flat:
            return PaintGoKartsTrackFlat;

        case TrackElemType::endStation:
        case TrackElemType::beginStation:
        case TrackElemType::middleStation:
            return PaintGoKartsStation;

        case TrackElemType::up25:
            return PaintGoKartsTrack25DegUp;
        case TrackElemType::flatToUp25:
            return PaintGoKartsTrackFlatTo25DegUp;
        case TrackElemType::up25ToFlat:
            return PaintGoKartsTrack25DegUpToFlat;

        case TrackElemType::down25:
            return PaintGoKartsTrack25DegDown;
        case TrackElemType::flatToDown25:
            return PaintGoKartsTrackFlatTo25DegDown;
        case TrackElemType::down25ToFlat:
            return PaintGoKartsTrack25DegDownToFlat;

        case TrackElemType::leftQuarterTurn1Tile:
            return PaintGoKartsTrackLeftQuarterTurn1Tile;
        case TrackElemType::rightQuarterTurn1Tile:
            return PaintGoKartsTrackRightQuarterTurn1Tile;

        // Added by OpenRCT2

        // Steep
        case TrackElemType::up60:
            return TrackUp60;
        case TrackElemType::up25ToUp60:
            return TrackUp25ToUp60;
        case TrackElemType::up60ToUp25:
            return TrackUp60ToUp25;
        case TrackElemType::down60:
            return TrackDown60;
        case TrackElemType::down25ToDown60:
            return TrackDown25ToDown60;
        case TrackElemType::down60ToDown25:
            return TrackDown60ToDown25;

        // Flat to steep
        case TrackElemType::flatToUp60LongBase:
            return TrackFlatToUp60LongBase;
        case TrackElemType::up60ToFlatLongBase:
            return TrackUp60ToFlatLongBase;
        case TrackElemType::flatToDown60LongBase:
            return TrackFlatToDown60LongBase;
        case TrackElemType::down60ToFlatLongBase:
            return TrackDown60ToFlatLongBase;

        // Small turns
        case TrackElemType::leftQuarterTurn3Tiles:
            return TrackLeftQuarterTurn3Tiles;
        case TrackElemType::rightQuarterTurn3Tiles:
            return TrackRightQuarterTurn3Tiles;

        // Medium turns
        case TrackElemType::leftQuarterTurn5Tiles:
            return TrackLeftQuarterTurn5Tiles;
        case TrackElemType::rightQuarterTurn5Tiles:
            return TrackRightQuarterTurn5Tiles;

        // Large turns
        case TrackElemType::leftEighthToDiag:
            return TrackLeftEighthToDiag;
        case TrackElemType::rightEighthToDiag:
            return TrackRightEighthToDiag;
        case TrackElemType::leftEighthToOrthogonal:
            return TrackLeftEighthToOrthogonal;
        case TrackElemType::rightEighthToOrthogonal:
            return TrackRightEighthToOrthogonal;

        // Diagonal
        case TrackElemType::diagFlat:
            return TrackDiagFlat;
        case TrackElemType::diagFlatToUp25:
            return TrackDiagFlatToUp25;
        case TrackElemType::diagUp25ToFlat:
            return TrackDiagUp25ToFlat;
        case TrackElemType::diagFlatToDown25:
            return TrackDiagFlatToDown25;
        case TrackElemType::diagDown25ToFlat:
            return TrackDiagDown25ToFlat;
        case TrackElemType::diagUp25:
            return TrackDiagUp25;
        case TrackElemType::diagDown25:
            return TrackDiagDown25;
        case TrackElemType::diagUp25ToUp60:
            return TrackDiagUp25ToUp60;
        case TrackElemType::diagUp60ToUp25:
            return TrackDiagUp60ToUp25;
        case TrackElemType::diagUp60:
            return TrackDiagUp60;
        case TrackElemType::diagDown25ToDown60:
            return TrackDiagDown25ToDown60;
        case TrackElemType::diagDown60ToDown25:
            return TrackDiagDown60ToDown25;
        case TrackElemType::diagDown60:
            return TrackDiagDown60;

        // Small gentle turns
        case TrackElemType::leftQuarterTurn3TilesUp25:
            return TrackLeftQuarterTurn3TilesUp25;
        case TrackElemType::rightQuarterTurn3TilesUp25:
            return TrackRightQuarterTurn3TilesUp25;
        case TrackElemType::leftQuarterTurn3TilesDown25:
            return TrackLeftQuarterTurn3TilesDown25;
        case TrackElemType::rightQuarterTurn3TilesDown25:
            return TrackRightQuarterTurn3TilesDown25;

        // Medium gentle turns
        case TrackElemType::leftQuarterTurn5TilesUp25:
            return TrackLeftQuarterTurn5TilesUp25;
        case TrackElemType::rightQuarterTurn5TilesUp25:
            return TrackRightQuarterTurn5TilesUp25;
        case TrackElemType::leftQuarterTurn5TilesDown25:
            return TrackLeftQuarterTurn5TilesDown25;
        case TrackElemType::rightQuarterTurn5TilesDown25:
            return TrackRightQuarterTurn5TilesDown25;

        // Large gentle turns
        case TrackElemType::leftEighthToDiagUp25:
            return TrackLeftEighthToDiagUp25;
        case TrackElemType::rightEighthToDiagUp25:
            return TrackRightEighthToDiagUp25;
        case TrackElemType::leftEighthToDiagDown25:
            return TrackLeftEighthToDiagDown25;
        case TrackElemType::rightEighthToDiagDown25:
            return TrackRightEighthToDiagDown25;
        case TrackElemType::leftEighthToOrthogonalUp25:
            return TrackLeftEighthToOrthogonalUp25;
        case TrackElemType::rightEighthToOrthogonalUp25:
            return TrackRightEighthToOrthogonalUp25;
        case TrackElemType::leftEighthToOrthogonalDown25:
            return TrackLeftEighthToOrthogonalDown25;
        case TrackElemType::rightEighthToOrthogonalDown25:
            return TrackRightEighthToOrthogonalDown25;

        // S bends
        case TrackElemType::sBendLeft:
            return TrackSBendLeft;
        case TrackElemType::sBendRight:
            return TrackSBendRight;

        default:
            return TrackPaintFunctionDummy;
    }
}
